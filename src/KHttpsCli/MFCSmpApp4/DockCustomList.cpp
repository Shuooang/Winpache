#include "pch.h"
#include "MFCSmpApp4.h"
#include "Mainfrm.h"
#include "DockCustomList.h"
#include "DockCustomDetail.h"
#include "DockMyPetList.h"
#include "DockOrder.h"
#include "resource.h"
#include "Requ.h"



IMPLEMENT_DYNCREATE(DockCustomListPane, CPaneForm)

// DockCustomList

IMPLEMENT_DYNCREATE(DockCustomList, DockClientBase)
DockCustomList* DockCustomList::s_me = NULL;

DockCustomList::DockCustomList(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn) //DockCustomList::IDD IDD_DockCustomList
{
	s_me = this;
}

DockCustomList::~DockCustomList()
{
}

void DockCustomList::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, _cList);
	DDX_Control(pDX, IDC_Filter, _cFilter);
}

// DDX/DDV �����Դϴ�.


BEGIN_MESSAGE_MAP(DockCustomList, DockClientBase)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &DockCustomList::OnBnClickedButton1)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_BN_CLICKED(IDC_Read, &DockCustomList::OnBnClickedRead)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &DockCustomList::OnHdnItemclickList1)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &DockCustomList::OnLvnColumnclickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &DockCustomList::OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &DockCustomList::OnNMDblclkList1)
	ON_EN_CHANGE(IDC_Filter, &DockCustomList::OnEnChangeFilter)
	ON_BN_CLICKED(IDC_OrderStart, &DockCustomList::OnBnClickedOrderStart)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// DockCustomList �����Դϴ�.

#ifdef _DEBUG
void DockCustomList::AssertValid() const
{
	__super::AssertValid();
}

#ifndef _WIN32_WCE
void DockCustomList::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}
#endif
#endif //_DEBUG


// DockCustomList �޽��� ó�����Դϴ�.
// CBranchVu1 �޽��� ó�����Դϴ�.
void DockCustomList::OnInitialUpdate()
{
	if(m_bInited)
		return;
	__super::OnInitialUpdate();
	m_bInited = TRUE;


	InitPropList();

	AdjustLayout();
	KwSetListReportStyle(&_cList);

}

void DockCustomList::AdjustLayout()
{
	if(GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rc;
	GetClientRect(rc);

	int cyTlb = 30;// m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	//m_wndObjectCombo.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	//m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	if(_cList.GetSafeHwnd() && !_cList.IsIconic())
		_cList.SetWindowPos(nullptr, rc.left, rc.top + cyTlb, rc.Width(), rc.Height() - (cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

void DockCustomList::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	AdjustLayout();
}

void DockCustomList::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);
	//_cProperty.SetFocus();
}

void DockCustomList::OnBnClickedButton1()
{
	//auto app = (CMfcSmpApp1App*)AfxGetApp();
	auto mfm = (CMainFrame*)AfxGetMainWnd();
	auto d1 = mfm->Dock(IDD_DockCustomList);
	if(d1)
		AfxMessageBox(L"DockCustomList ok");
}

BOOL DockCustomList::Filter(ShJObj sjoRow)
{
	if(
		tchstr(sjoRow->Str("fNickName"), (PWS)_filter) ||
		tchstr(sjoRow->Str("fTel"), (PWS)_filter) ||
		tchstr(sjoRow->Str("fName"), (PWS)_filter) ||
		tchstr(sjoRow->Str("fNameBreed"), (PWS)_filter)
		)	return TRUE;
	return FALSE;
}


_STitleWidthField s_arlstCustom[] =
{
	{  20, nullptr     , L"#"    ,},
	{ 80, "fName"     , L"���̸�",},
	{ 100, "fTel"      , L"�޴���",},
	{  80, "fNickName" , L"�г���",},
	{ 80, "fNameBreed", L"ǰ��", },
};
_STitleWidthField* DockCustomList::getListFieldInfo(int* pnCols)
{
	if(pnCols) *pnCols = _countof(s_arlstCustom);
	return s_arlstCustom;
}

void DockCustomList::InitPropList(ShJObj sjo)
{
	/// �� ��ŷâ�� ������ �տ� ���Ƿ� �ʿ� ����.
	KwSetListColumn(&_cList, s_arlstCustom, _countof(s_arlstCustom));
}


void DockCustomList::Refresh(string eStep, ShJObj sjo, int iOp)
{
	if(KwAttrDel(iOp, ePostAsync8))
	{
		_KwBeginInvoke(this, [&, eStep, sjo, iOp]()
			{	Refresh(eStep, sjo, iOp);	});
		return;
	}
	else if(!DefaultSync(iOp))
		return;

	if(eStep == "request")
	{
		if(!sjo)
			sjo = make_shared<JObj>();
		if(!sjo->Has("fBizID"))
		{
			auto app = (CSmpApp4*)AfxGetApp();
			(*sjo)("fBizID") = app->getLoginData("fBizID"); //L"biz-0002"; /// test��#
		}
		ShJObj sjin = sjo;
		RM_SelectCustomList(sjin);
		return;
	}
	else if(eStep == "output")
	{
		FOREGROUND();
		ShJObj sjout = sjo;/// _sjo �� ����.

		if(sjout->Has("tenum"))
		{
			auto tenum = sjo->Array("tenum");//�ʵ庰 �ڵ�� ��¿빮�ڿ� ������ ì���.

		}
		if(sjout->Has("table"))
		{
			if(_sortKey.empty())
				_sortKey = "fNickName";

			ShJArr stable = sjout->Array("table");/// _sjo �� ����.
			MakeSort(stable);
			InsertDataToList();
		}
	}
}
/*
void DockCustomList::ClearList(int iOp)
{
	if(KwAttrDel(iOp, ePostAsync8))
	{
		_KwBeginInvoke(this, [&, iOp]()
			{
				ClearList(iOp);
			});
		return;
	}
	_cList.DeleteAllItems();
	_data.RemoveAll();
	InitPropList();
}*/

void DockCustomList::InsertDataToList(bool bHasData)
{
	ClearList();
	if(!bHasData)
		return;
	InsertDataToListImpl("fUsrID", "tuser");
/*	_cList.DeleteAllItems();
	_data.RemoveAll();
	if(!bHasData)
		return;
	int i = 0;
	for(auto& [k, sjo] : _sort)//| std::views::reverse �� c++20
	{
		auto item = _cList.InsertItem(i, L"");
		i++;
	}
	i = _sortDirection == 1 ? 0 : _cList.GetItemCount() -1;
	for(auto& [k, sjo] : _sort )//| std::views::reverse �� c++20
	{
		//auto item = _cList.InsertItem(i, L"");
		_cList.SetItem(i, 1, LVIF_TEXT, sjo->S("fNickName"), 0, 0, 0, NULL, 0);
		_cList.SetItem(i, 2, LVIF_TEXT, sjo->S("fTel"), 0, 0, 0, NULL, 0);
		_cList.SetItem(i, 3, LVIF_TEXT, sjo->S("fName"), 0, 0, 0, NULL, 0);
		_cList.SetItem(i, 4, LVIF_TEXT, sjo->S("fNameBreed"), 0, 0, 0, NULL, 0);
		_cList.SetItemData(i, (DWORD_PTR)sjo->S("fUsrID"));
		if(_sortDirection == 1)
			i++;
		else
			i--;
	}
	*/
}

void DockCustomList::RM_SelectCustomList(ShJObj sjo, int iOp)
{
	TRACE("%s(%d)\n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		//FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud �۾� ť
			{
				RM_SelectCustomList(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);
	JObj jin, jout;
	KRequ req;
	jin.Copy(*sjo, "fBizID");// �̰ſ� fMode�� ���ÿ� �־�� Ű�� �ȴ�. ����ÿ��� �̰Ÿ� �־.
	jin("fUsrIdUpt") = req._UsrID;//������ ����� �α����� ���.
	req.RequestPost(__FUNCTION__, jin, jout);
	if(tchsame(jout.S("Return"), L"OK"))
	{
		_sjo = make_shared<JObj>(jout);
		Refresh("output", _sjo, iOp | ePostAsync8); /// _selBox �� ���� ������ �� â�� clear�Ǿ���
		return;
	}
}



void DockCustomList::OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	//TRACE("%s \n", __FUNCTION__);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void DockCustomList::OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	TRACE("%s (%d, %d)\n", __FUNCTION__, pNMLV->iItem, pNMLV->iSubItem);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.s_arlst1 _sortKey
	//pNMLV->iItem, pNMLV->iSubItem
	SetListSort(_cList, pNMLV, s_arlstCustom, "table");
	//if(pNMLV->iItem < 0 && pNMLV->iSubItem > 0)
	//{
	//	string sortKey = s_arlstCustom[pNMLV->iSubItem].field;
	//	if(sortKey != _sortKey)
	//		_sortKey = s_arlstCustom[pNMLV->iSubItem].field;
	//	else
	//		_sortDirection *= -1;

	//	auto hdr = _cList.GetHeaderCtrl();
	//	HDITEM headerInfo = { 0 };
	//	headerInfo.mask = HDI_FORMAT;
	//	if(TRUE == hdr->GetItem(pNMLV->iSubItem, &headerInfo))
	//	{
	//		if(_sortDirection == 1)
	//		{
	//			headerInfo.fmt |= HDF_SORTUP;
	//			headerInfo.fmt &= ~HDF_SORTDOWN;
	//		}
	//		else
	//		{
	//			headerInfo.fmt |= HDF_SORTDOWN;
	//			headerInfo.fmt &= ~HDF_SORTUP;
	//		}
	//		hdr->SetItem(pNMLV->iSubItem, &headerInfo);
	//	}
	//	ShJArr stable = _sjo->Array("table");/// _sjo �� ����.
	//	_sort.clear();
	//	MakeSort(stable);
	//	InsertDataToList();
	//}
	*pResult = 0;
}


void DockCustomList::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	//TRACE("%s (%d, %d, %d)\n", __FUNCTION__, pNMLV->iItem, pNMLV->uChanged, pNMLV->uNewState);
	//                                          8                                          2    |    1 = 3 // ���º�ȭ�� �ְ� ���õǾ��ų�, ��Ŀ���� ������ ���
	if(pNMLV->iItem >= 0 && pNMLV->uChanged == LVIF_STATE && pNMLV->uNewState == (LVIS_SELECTED | LVIS_FOCUSED))
	{
		SyncCustomDetail(pNMLV->iItem);//, eShowDock1 | eSyncByAction64�� ���ش�.
	}
	*pResult = 0;
}

void DockCustomList::SyncCustomDetail(int isel, int iOp)
{
	SetLambdaTimer("sync_cutomDetail", 700, [&, isel, iOp](int ntm, PAS tmk)
		{
			if(isel >= 0)
			{
				CString fUsrID = (PWS)_cList.GetItemData(isel);// , (DWORD_PTR)sjo->S("fUsrID"));
				if(fUsrID.GetLength() > 0)
				{
					ShJObj sjin = make_shared<JObj>();///?ex: ����â Refresh
					(*sjin)("fUsrID") = fUsrID;
					DockCustomDetail::s_me->Refresh("sync", sjin, iOp);

					ShJObj sjoU;
					for(auto& [k, v] : _sort)
					{
						if(v->SameS("fUsrID", fUsrID))
						{
							sjoU = v;
							break;
						}
					}

					if(sjoU)
					{
						sjin->Copy(*sjoU, "fUsrID");
						sjin->Copy(*sjoU, "fNickName");
						sjin->Copy(*sjoU, "fTel");
						DockMyPetList::s_me->Refresh("sync", sjin, ePostAsync8);//iOp2 | eSyncByAction64) | eReqNoErrBox4
					}
					//auto sjoU = _sjo->GetArrayItem("table", isel);// array�� ���� �� 0���� ��

					//int iOp2 = iOp;
					//iOp2 = KwAttrRemove(iOp2, eSyncByAction64 | eReqNoErrBox4) ;
				}
			}
		}, 1);
}

void DockCustomList::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	SyncCustomDetail(pNMItemActivate->iItem, eShowDock1 | eSyncByAction64);
	*pResult = 0;
}


void DockCustomList::OnEnChangeFilter()
{
	// TODO:  RICHEDIT ��Ʈ���� ���, �� ��Ʈ����
	// DockClientBase::OnInitDialog() �Լ��� ������ 
	//�ϰ� ����ũ�� OR �����Ͽ� ������ ENM_CHANGE �÷��׸� �����Ͽ� CRichEditCtrl().SetEventMask()�� ȣ������ ������
	// �� �˸� �޽����� ������ �ʽ��ϴ�.

	_cFilter.GetWindowTextW(_filter);
	auto This = this;
	SetLambdaTimer("filter", 500, [This](int, PAS)
		{
			//_KwBeginInvoke(This, []() /// foreground �۾� ť
			//	{
			ShJArr stable = This->_sjo->Array("table");/// _sjo �� ����.
			This->MakeSort(stable);
			This->InsertDataToList();
//				});
		}, 1);

}


void DockCustomList::OnBnClickedOrderStart()
{
	int isel = KwGetSelectedListItem(&_cList);
	if(isel >= 0)
	{
		CString fUsrID = (PWS)_cList.GetItemData(isel);// , (DWORD_PTR)sjo->S("fUsrID"));
		if(fUsrID.GetLength() > 0)
		{
			auto sjoin = make_shared<JObj>();
			(*sjoin)("fUsrID") = fUsrID;// L"user-0002";
			DockOrder::s_me->NewOrder("request", sjoin, eShowDock1 | ePostAsync8);
		}
	}
}

void DockCustomList::OnBnClickedRead()
{
	Refresh("request", NULL, eNoBlinkBmp2);
}

void DockCustomList::OnShowWindow(BOOL bShow, UINT nStatus)
{
	DockClientBase::OnShowWindow(bShow, nStatus);
	if(bShow)
		Refresh("request", NULL, ePostAsync8);//ó���� ��¦�̰�. �α��� �ؾ� �ϹǷ� �ϳ� ����
}
