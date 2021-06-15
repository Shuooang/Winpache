
#include "pch.h"
#include "Mainfrm.h"
#include "DockWorkLogList.h"
#include "resource.h"
#include "Requ.h"



IMPLEMENT_DYNCREATE(DockWorkLogListPane, CPaneForm)

// DockWorkLogList

IMPLEMENT_DYNCREATE(DockWorkLogList, DockClientBase)
DockWorkLogList* DockWorkLogList::s_me = NULL;

DockWorkLogList::DockWorkLogList(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn) //DockWorkLogList::IDD IDD_DockWorkLogList
{
	s_me = this;
}

DockWorkLogList::~DockWorkLogList()
{
}

void DockWorkLogList::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, _cList);
	DDX_Control(pDX, IDC_Filter, _cFilter);
}

// DDX/DDV �����Դϴ�.


BEGIN_MESSAGE_MAP(DockWorkLogList, DockClientBase)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_NOTIFY(HDN_ITEMCLICK, 0, &DockWorkLogList::OnHdnItemclickList1)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &DockWorkLogList::OnLvnColumnclickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &DockWorkLogList::OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &DockWorkLogList::OnNMDblclkList1)
	ON_EN_CHANGE(IDC_Filter, &DockWorkLogList::OnEnChangeFilter)
	ON_BN_CLICKED(IDC_Read, &DockWorkLogList::OnBnClickedRead)
END_MESSAGE_MAP()


// DockWorkLogList �����Դϴ�.

#ifdef _DEBUG
void DockWorkLogList::AssertValid() const
{
	__super::AssertValid();
}

#ifndef _WIN32_WCE
void DockWorkLogList::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}
#endif
#endif //_DEBUG


// DockWorkLogList �޽��� ó�����Դϴ�.
// CBranchVu1 �޽��� ó�����Դϴ�.
void DockWorkLogList::OnInitialUpdate()
{
	if(m_bInited)
		return;
	__super::OnInitialUpdate();
	m_bInited = TRUE;


	InitPropList();

	AdjustLayout();
	KwSetListReportStyle(&_cList);
	SetInsertRoutine();// Lambda �ʱ�ȭ �Ϸ���
}

void DockWorkLogList::AdjustLayout()
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

void DockWorkLogList::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	AdjustLayout();
}

void DockWorkLogList::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);
	//_cProperty.SetFocus();
}


BOOL DockWorkLogList::Filter(ShJObj sjoRow)
{
	if(
		tchstr(sjoRow->Str("fObjName"), (PWS)_filter) ||
		tchstr(sjoRow->Str("fMemo"), (PWS)_filter) ||
		tchstr(sjoRow->Str("fMode"), (PWS)_filter)
		)	return TRUE;
	return FALSE;
}


_STitleWidthField DockWorkLogList::s_arlst[] =
{
	{  20, nullptr     , L"#"    ,},
	{ 60, "fTitle"     , L"���",},
	{ 80, "fObjName"   , L"�̸�",},
	{ 70, "fMode"     , L"�۾�",},
	{ 70, "fTmWork"   , L"�ð�",},
	{ 100, "fMemo"     , L"�޸�",},
};
_STitleWidthField* DockWorkLogList::getListFieldInfo(int* pnCols)
{
	if(pnCols) *pnCols = _countof(s_arlst);
	return s_arlst;
}

void DockWorkLogList::InitPropList(ShJObj sjo)
{
	/// �� ��ŷâ�� ������ �տ� ���Ƿ� �ʿ� ����.
	KwSetListColumn(&_cList, s_arlst, _countof(s_arlst));
}

void DockWorkLogList::Refresh(string eStep, ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, eStep);
	if(KwAttrDel(iOp, ePostAsync8))
	{
		_KwBeginInvoke(this, [&, eStep, sjo, iOp]()
			{
				Refresh(eStep, sjo, iOp);
			});
		return;
	}
	else if(!DefaultSync(iOp))
		return;
	FOREGROUND();

	if(eStep == "request")
	{
		ShJObj sjin = sjo;
		RM_SelectWorkLogList(sjin);
		return;
	}
	else if(eStep == "list")
	{
		FOREGROUND();
		ShJObj sjout = sjo;/// _sjo �� ����.

		if(sjout->Has("tenum"))
		{
			auto tenum = sjo->Array("tenum");//�ʵ庰 �ڵ�� ��¿빮�ڿ� ������ ì���.
			SetEnum(*tenum);
		}
		if(sjout->Has("table"))
		{
			if(_sortKey.empty())
				_sortKey = "fTmWork";
			_sortDirection = -1;

			ShJArr stable = sjout->Array("table");/// _sjo �� ����.
			MakeSort(stable);
			InsertDataToList();
		}
	}
}

/// ���� ��Ͽ� �ְ� ������ �״´�.
/// jin.Copy(*sjo, "fObject");
/// jin.Copy(*sjo, "fObjID");
/// jin.Copy(*sjo, "fObjName");
/// jin.Copy(*sjo, "fMode");
// void DockWorkLogList::WorkLog(PAS fObject, PWS fObjID, PAS fMode, PAS fObjName)
// {
// 	ShJObj sjo = make_shared<JObj>();
// 	(*sjo)("fObject") = fObject;
// 	(*sjo)("fObjID") = fObjID;
// 	(*sjo)("fObjName") = fObjName;
// 	(*sjo)("fMode") = fMode;
// 	WorkLog(sjo);
// }
void DockWorkLogList::WorkLog(ShJObj sjo)
{
	_KwBeginInvoke(this, [&, sjo]()
		{
			SetInsertRoutine();
			SetListRow(_cList, 0, _countof(s_arlst), sjo, L"");
		});
}

//?deprecated
void DockWorkLogList::SetInsertRoutine()
{
	if(!_ldInsertItem)
	{
		auto LdSetItem = [&](int i, int subi, ShJObj sjo)
		{
			if(subi == 1)
			{
				_data.Add(sjo->S("fWorkID"));//zzz
				_cList.SetItemData(i, (DWORD_PTR)(PWS)_data.GetAt(i));// sjo->S("fPetID"));//��â�� �����Ƿ� ������ ����
			}
			PAS k = s_arlst[subi].field;//zzz
			CString str = sjo->Str(k);
			CStringW stru;
			if(str.GetLength() > 0)
			{
				if(str == L"NULL")
					stru = L"";
				else
				{
					if(tchsame(k, "fTmWork"))
						stru = str.Mid(11);
					else
					{
						auto tenumpr = _mapEnum[L"tworklog"];//zzz
						wstring fKR;
						if(tenumpr && tenumpr->Lookup((PWS)str, fKR))
							stru = fKR.c_str();
						else
							stru = str;
					}
				}
			}
			_cList.SetItem(i, subi, LVIF_TEXT, stru, 0, 0, 0, NULL, 0);
		};
		_ldInsertItem = make_shared<function<void(int, int, ShJObj)>>(LdSetItem);
	}
}

void DockWorkLogList::InsertDataToList(bool bHasData)
{
	SetInsertRoutine();
	ClearList(0);
	if(!bHasData)
		return;
	//InsertAllItemT(_cList, _countof(s_arlst), *_ldInsertItem);
	InsertDataToListImpl("fWorkID", "tworklog");

// 	int i = 0;
// 	for(auto& [k, sjo] : _sort)//| std::views::reverse �� c++20
// 	{
// 		auto item = _cList.InsertItem(i, L"");
// 		i++;
// 	}
// 	i = _sortDirection == 1 ? 0 : _cList.GetItemCount() - 1;
// 	for(auto& [k, sjo] : _sort)//| std::views::reverse �� c++20
// 	{
// 		//auto item = _cList.InsertItem(i, L"");fCreated
// 		_cList.SetItem(i, 1, LVIF_TEXT, sjo->S("fObjName"), 0, 0, 0, NULL, 0);
// 		_cList.SetItem(i, 2, LVIF_TEXT, sjo->S("fMode"), 0, 0, 0, NULL, 0);
// 		_cList.SetItem(i, 3, LVIF_TEXT, sjo->S("fMemo"), 0, 0, 0, NULL, 0);
// 		_cList.SetItem(i, 3, LVIF_TEXT, sjo->S("fCreated"), 0, 0, 0, NULL, 0);
// 		_cList.SetItemData(i, (DWORD_PTR)sjo->S("fWorkID"));
// 		if(_sortDirection == 1)
// 			i++;
// 		else
// 			i--;
// 	}
}


/// ó���� �㶧 �ѹ� �о� �´�. OnShowWindow �ΰ�?
void DockWorkLogList::RM_SelectWorkLogList(ShJObj sjo, int iOp)
{
	TRACE("%s(%d)\n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud �۾� ť
			{
				RM_SelectWorkLogList(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);
	JObj jin, jout;
	KRequ req;
	jin("fUsrID") = req._UsrID;//������ ����� �α����� ���.
	req.RequestPost(__FUNCTION__, jin, jout);
	if(tchsame(jout.S("Return"), L"OK"))
	{
		_sjo = make_shared<JObj>(jout);
		Refresh("list", _sjo, iOp | ePostAsync8); /// _selBox �� ���� ������ �� â�� clear�Ǿ���
	}
}

void DockWorkLogList::RM_AddWorkLog(ShJObj sjo, int iOp)
{
	AppBase::RM_AddWorkLog(sjo, iOp);
}


void DockWorkLogList::OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	//TRACE("%s \n", __FUNCTION__);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;
}


void DockWorkLogList::OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	TRACE("%s (%d, %d)\n", __FUNCTION__, pNMLV->iItem, pNMLV->iSubItem);
	//pNMLV->iItem, pNMLV->iSubItem
	SetListSort(_cList, pNMLV, s_arlst, "table");
	* pResult = 0;
}


void DockWorkLogList::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	//TRACE("%s (%d, %d, %d)\n", __FUNCTION__, pNMLV->iItem, pNMLV->uChanged, pNMLV->uNewState);
	if(pNMLV->iItem >= 0 && pNMLV->uChanged == LVIF_STATE && pNMLV->uNewState == (LVIS_SELECTED | LVIS_FOCUSED))
	{
		/// �׳� Ŭ���Ҷ��� �ƹ��͵� ���ϱ�� ����.
		///SyncWorkLog(pNMLV->iItem);
	}
	*pResult = 0;
}

/// �ϳ��� ���� �ϸ�, ����ȭ �� �ʿ䰡 �ֳ�? ���� Ŭ�� �Ҷ� �ؾ���.
void DockWorkLogList::SyncWorkLog(int isel, int iOp)
{
	if(isel >= 0)
	{
		CString fUsrID = (PWS)_cList.GetItemData(isel);// , (DWORD_PTR)sjo->S("fUsrID"));
		if(fUsrID.GetLength() > 0)
		{
			ShJObj sjin = make_shared<JObj>();///?ex: ����â Refresh
			(*sjin)("fUsrID") = fUsrID;
			_KwBeginInvoke(this, [&, sjin, iOp]() // foreground �۾� ť
				{
					FOREGROUND();
					/// ���� �۾����� ���� ����.
				});
		}
	}
}
void DockWorkLogList::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	SyncWorkLog(pNMItemActivate->iItem, eShowDock1);
	*pResult = 0;
}


void DockWorkLogList::OnEnChangeFilter()
{
	_cFilter.GetWindowTextW(_filter);
	auto This = this;
	SetLambdaTimer("filter", 500, [This](int, PAS)
		{
			ShJArr stable = This->_sjo->Array("table");/// _sjo �� ����.
			This->MakeSort(stable);
			This->InsertDataToList();
		}, 1);
}


void DockWorkLogList::OnBnClickedRead()
{
	RM_SelectWorkLogList(0);
}
