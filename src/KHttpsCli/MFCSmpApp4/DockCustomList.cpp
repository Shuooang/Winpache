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

// DDX/DDV 지원입니다.


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


// DockCustomList 진단입니다.

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


// DockCustomList 메시지 처리기입니다.
// CBranchVu1 메시지 처리기입니다.
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
	{ 80, "fName"     , L"펫이름",},
	{ 100, "fTel"      , L"휴대폰",},
	{  80, "fNickName" , L"닉네임",},
	{ 80, "fNameBreed", L"품종", },
};
_STitleWidthField* DockCustomList::getListFieldInfo(int* pnCols)
{
	if(pnCols) *pnCols = _countof(s_arlstCustom);
	return s_arlstCustom;
}

void DockCustomList::InitPropList(ShJObj sjo)
{
	/// 이 도킹창은 나열이 앞에 서므로 필요 없다.
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
			(*sjo)("fBizID") = app->getLoginData("fBizID"); //L"biz-0002"; /// test용#
		}
		ShJObj sjin = sjo;
		RM_SelectCustomList(sjin);
		return;
	}
	else if(eStep == "output")
	{
		FOREGROUND();
		ShJObj sjout = sjo;/// _sjo 와 같다.

		if(sjout->Has("tenum"))
		{
			auto tenum = sjo->Array("tenum");//필드별 코드와 출력용문자열 데이터 챙긴다.

		}
		if(sjout->Has("table"))
		{
			if(_sortKey.empty())
				_sortKey = "fNickName";

			ShJArr stable = sjout->Array("table");/// _sjo 와 같다.
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
	for(auto& [k, sjo] : _sort)//| std::views::reverse 는 c++20
	{
		auto item = _cList.InsertItem(i, L"");
		i++;
	}
	i = _sortDirection == 1 ? 0 : _cList.GetItemCount() -1;
	for(auto& [k, sjo] : _sort )//| std::views::reverse 는 c++20
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
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud 작업 큐
			{
				RM_SelectCustomList(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);
	JObj jin, jout;
	KRequ req;
	jin.Copy(*sjo, "fBizID");// 이거와 fMode가 동시에 있어야 키가 된다. 변경시에는 이거만 있어도.
	jin("fUsrIdUpt") = req._UsrID;//변경한 사람은 로그인한 사람.
	req.RequestPost(__FUNCTION__, jin, jout);
	if(tchsame(jout.S("Return"), L"OK"))
	{
		_sjo = make_shared<JObj>(jout);
		Refresh("output", _sjo, iOp | ePostAsync8); /// _selBox 가 지워 졌으니 상세 창도 clear되야지
		return;
	}
}



void DockCustomList::OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	//TRACE("%s \n", __FUNCTION__);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}


void DockCustomList::OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	TRACE("%s (%d, %d)\n", __FUNCTION__, pNMLV->iItem, pNMLV->iSubItem);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.s_arlst1 _sortKey
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
	//	ShJArr stable = _sjo->Array("table");/// _sjo 와 같다.
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
	//                                          8                                          2    |    1 = 3 // 상태변화가 있고 선택되었거나, 포커스가 가있을 경우
	if(pNMLV->iItem >= 0 && pNMLV->uChanged == LVIF_STATE && pNMLV->uNewState == (LVIS_SELECTED | LVIS_FOCUSED))
	{
		SyncCustomDetail(pNMLV->iItem);//, eShowDock1 | eSyncByAction64를 안준다.
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
					ShJObj sjin = make_shared<JObj>();///?ex: 고객상세창 Refresh
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
					//auto sjoU = _sjo->GetArrayItem("table", isel);// array를 구한 후 0번쨰 항

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
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// DockClientBase::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	_cFilter.GetWindowTextW(_filter);
	auto This = this;
	SetLambdaTimer("filter", 500, [This](int, PAS)
		{
			//_KwBeginInvoke(This, []() /// foreground 작업 큐
			//	{
			ShJArr stable = This->_sjo->Array("table");/// _sjo 와 같다.
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
		Refresh("request", NULL, ePostAsync8);//처음엔 반짝이게. 로그인 해야 하므로 하나 마나
}
