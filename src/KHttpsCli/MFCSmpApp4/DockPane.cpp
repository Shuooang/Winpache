#include "pch.h"

#include "KwLib64/inlinefnc.h"

#include "DockPane.h"

#include "resource.h"
#include "MainFrm.h"
#include "AppBase.h"

#include "DockPetDetail.h"
#include "DockEx2.h"
#include "DockCustomDetail.h" //?dock everytime:
#include "DockCustomHistory.h"
#include "DockMyHistory.h"
#include "DockMyPetList.h"
#include "DockPetDetail.h"
#include "DockPetChecks.h"
#include "DockScheduleDetail.h"
#include "DockCustomList.h"
#include "DockOrder.h"
#include "DockProducts.h"
#include "DockOptions.h"
#include "DockWorkLogList.h"





IMPLEMENT_DYNCREATE(DockClientBase, CDockFormChild)


DockClientBase::DockClientBase(UINT nID, CPaneForm* ppn)
	: CDockFormChild(nID, ppn) //DockEx2::IDD IDD_DockEx2
{
}

DockClientBase::~DockClientBase()
{
	/// �̰� ���� ���� OnDestroy ���� _prData->RemovaAll() �ؾ� ��Ʈ�� ���� �ؼ� ���� �ȳ���.
	DeleteMeSafe(_prData);
}
//#pragma comment(lib, "msimg32.lib")
//[��ó] LoadBitmap �Լ� | �ۼ��� ������


void DockClientBase::StartBlink(int cnt /*= 10*/)
{
	SetLambdaTimer("active_blink", 50, [&, cnt](int ntm, PAS sid)
		{
			//KTrace(L"%d. %s (%s)\n", ntm, __FUNCTIONW__, L"Lambda timer test");
			KTimerObj* po = GetTmObj(sid);
			if(ntm == 0)
			{
				// first
				HBITMAP hb = LoadBitmap(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP5));
				po->_param = (LPARAM)hb;
			}
			auto cBlink = (CStatic*)GetDlgItem(IDC_Blink);
			if(cBlink)
			{
				if((ntm % 2) == 1)
				{//LoadBitmap
					HBITMAP hb = (HBITMAP)po->_param;// LoadImageW(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP5));
					cBlink->SetBitmap(hb);
				}
				else
					cBlink->SetBitmap(NULL);
			}
			if(ntm == cnt -1)
			{
				// last
				HBITMAP hb = (HBITMAP)po->_param;// LoadImageW(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDB_BITMAP5));
				//DeleteObject(hb); �� SelectObject �ϴ� WM_PAINT���� ���� �׸���
			}
		}, cnt);
}

void DockClientBase::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(DockClientBase, CDockFormChild)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

void DockClientBase::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus(pOldWnd);
	//_cProperty.SetFocus();
	//auto pfn = (CMainFrame*)AfxGetMainWnd();
	//pfn->ContextCategory(ID_CONTEXT_Order);
	OnDockFocus();
}

BOOL DockClientBase::Filter(ShJObj sjoRow)
{

	return TRUE;
}
void DockClientBase::MakeSort(ShJArr stable)
{
	_sort.clear();
	for(auto& sjv : *stable)
	{
		auto& sjo = sjv->AsObject();
		if(Filter(sjo))
			_sort.Insert(sjo->S(_sortKey.c_str()), sjo);
	}
}

//?���
void DockClientBase::SetListItem(CListCtrl& cList, int itm, int subitm, _STitleWidthField* sar, ShJObj sjo)
{
	CString str = sjo->Str(sar[subitm - 1].field);
	cList.SetItem(itm, subitm, LVIF_TEXT, str, 0, 0, 0, NULL, 0);
}

void DockClientBase::SetListRow(CListCtrl& cList, int i, int nCols, ShJObj sjo, PWS hd)
{
	cList.InsertItem(i, hd);
	for(int j = 1; j < nCols; j++)
		(*_ldInsertItem)(i, j, sjo);
};

void DockClientBase::InsertAllItem(CListCtrl& cList, int nCols, PWS hd)
{
	if(_sortDirection == 1)
		_sort.for_loop([&](auto k, auto sjo, int i) { SetListRow(cList, i, nCols, sjo, hd); });
	else
		_sort.for_RLoop([&](auto k, auto sjo, int i) { SetListRow(cList, i, nCols, sjo, hd); });
}


void DockClientBase::OnInitialUpdate()
{
	__super::OnInitialUpdate();
}


void DockClientBase::InsertDataToListImpl(CStringA dataKey, CStringA enumTable)
{
	CListCtrl* pList = getListCtrl();
	int nCols = 0;
	auto setList = getListFieldInfo(&nCols);
	ASSERT(setList);
	auto LdSetItem = [&, pList, setList, dataKey, enumTable](int i, int subi, ShJObj sjo)
	{
		ASSERT(pList);
		if(subi == 1)
		{
			_data.Add(sjo->S(dataKey));//"fPetID"
			pList->SetItemData(i, (DWORD_PTR)(PWS)_data.GetAt(i));// sjo->S("fPetID"));//��â�� �����Ƿ� ������ ����
			CStringA dtA((PWS)_data.GetAt(i));
			TRACE("i(%d) %s\n", i, dtA);
		}
		PAS k = setList[subi].field;
		CString str = sjo->Str(k);
		CStringW enumTableW(enumTable);
		auto tenumpr = _mapEnum[(PWS)enumTableW];//L"tmypets"
		wstring fKR;
		CStringW stru;
		if(tenumpr && tenumpr->Lookup((PWS)str, fKR))
			stru = fKR.c_str();
		else
			stru = str;
		pList->SetItem(i, subi, LVIF_TEXT, stru, 0, 0, 0, NULL, 0);
	};

	InsertAllItemT(*pList, nCols, LdSetItem);//_countof(s_arlstMyPet)
}

void DockClientBase::SetListSort(CListCtrl& cList, LPNMLISTVIEW pNMLV, _STitleWidthField* arlist, PAS table)
{
	ASSERT(_sjo);
	ShJArr stable = _sjo->Array(table);/// _sjo �� ����.
	SetListSort(cList, pNMLV, arlist, stable);
}
void DockClientBase::SetListSort(CListCtrl& cList, LPNMLISTVIEW pNMLV, _STitleWidthField* arlist, ShJArr stable)
{
	if(pNMLV->iItem < 0 && pNMLV->iSubItem > 0)
	{
		string sortKey = arlist[pNMLV->iSubItem].field;
		if(sortKey != _sortKey)
			_sortKey = arlist[pNMLV->iSubItem].field;
		else
			_sortDirection *= -1;

		auto hdr = cList.GetHeaderCtrl();
		HDITEM headerInfo = { 0 };
		headerInfo.mask = HDI_FORMAT;
		if(TRUE == hdr->GetItem(pNMLV->iSubItem, &headerInfo))
		{
			if(_sortDirection == 1)
			{
				headerInfo.fmt |= HDF_SORTUP;
				headerInfo.fmt &= ~HDF_SORTDOWN;
			}
			else
			{
				headerInfo.fmt |= HDF_SORTDOWN;
				headerInfo.fmt &= ~HDF_SORTUP;
			}
			hdr->SetItem(pNMLV->iSubItem, &headerInfo);
		}
		//ShJArr stable = _sjo->Array(table);/// _sjo �� ����.
		_sort.clear();
		MakeSort(stable);
		InsertDataToList();
	}
}

















KDockPane::KDockPane()
{
}

KDockPane::~KDockPane()
{
	for(auto [k, dck] : m_docks)
	{
		DeleteMeSafe(dck);
	}
}
//_##DockEx1, FormView �Ⱦ��� �غ�����
#define SettingNoIdDOCK(type, title, style) \
	SettingDock(type##Pane::GetThisClass(), title, 0, style)

#define SettingDOCK(type, title, style) \
	SettingDock(type##Pane::GetThisClass(), title, IDD_##type, CBRS_##style, IDI_##type##_HC)
///  SettingDock(DockEx1Pane::GetThisClass(), L"���õ�1", IDD_DockEx1, CBRS_LEFT),

void KDockPane::InitDocks(CFrameWnd* mframe)
{
	_mframe = (CMDIFrameWndEx*)mframe;
	//���⼭ ��ŷ ������ �� �ȴ�.
	SettingDock sk[] =
	{
		// ������Ƽ�� ���� ���̰� ���� �غ��� �ߴµ� �ȵ�. �׳� ���並 ���̱�� ��.
		//SettingDock(DockPetDetail::GetThisClass(), L"�� ��", IDD_DockPetDetail, CBRS_LEFT),

		/// ���� ��
		//SettingDOCK(DockEx2, L"DockEx2", LEFT),
		
		/// ������ ���� ��
		SettingDOCK(DockCustomList, L"�� ���", RIGHT),
		SettingDOCK(DockCustomDetail, L"�� ��", LEFT),//?dock everytime:
		SettingDOCK(DockCustomHistory, L"�� �̷�", LEFT),

		//SettingDOCK(DockMyHistory, L"�۾� �̷�", RIGHT),
		SettingDOCK(DockMyPetList, L"�� ���", RIGHT),
		//SettingDOCK(DockPetDetail, L"�� ��", LEFT),
		SettingDOCK(DockPetChecks, L"üũ����Ʈ", LEFT), // ����Ʈ�� �ƴϰ� �׸��� üũ������� Ī�Ѵ�.
		SettingDOCK(DockScheduleDetail, L"���� ��", LEFT),
		SettingDOCK(DockOrder, L"����", BOTTOM),
		SettingDOCK(DockProducts, L"��ǰ", RIGHT),
		SettingDOCK(DockOptions, L"��ǰ�ɼ�", RIGHT),
		SettingDOCK(DockWorkLogList, L"�۾����", LEFT),
		
	};
	//auto rc = DockEx1Pane::GetThisClass();

	for(auto sk1 : sk)
		CreateDockingWin(sk1);
	_mframe->UpdateMDITabbedBarsIcons();

}

BOOL KDockPane::CreateDockingWin(SettingDock& sk)
{
	sk._dock = sk._pRC->CreateObject();
	auto pane = (CDockablePane*)sk._dock;
	if(pane->IsKindOf(RUNTIME_CLASS(CPaneForm)))
	{
		CPaneForm* paneForm = (CPaneForm*)pane;
		paneForm->_nID = sk._nID;// �� pane�� �ƴϰ� child formview�� ID
	}

	BOOL b = pane->Create(sk._title, _mframe, sk._rc, sk._bHasGripper, sk._nID, sk._dwStyle,
		sk._dwTabbedStyle, sk._dwControlBarStyle);
	if(b)
	{
		HICON hIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(sk._idiHicon), 
			IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
		pane->SetIcon(hIcon, FALSE);
		
		pane->EnableDocking(CBRS_ALIGN_ANY);
		_mframe->DockPane(pane);
		m_docks[sk._nID] = pane;
	}
	return b;
}

BOOL KDockPane::CreateDockingForm(SettingDock& sk)
{
	sk._dock = sk._pRC->CreateObject();
	auto pane = (CPaneForm*)sk._dock;
	pane->_nID = sk._nID;// �� pane�� �ƴϰ� child formview�� ID
	//Create(LPCTSTR lpszCaption, CWnd * pParentWnd, const RECT & rect, BOOL bHasGripper, UINT nID
	//	, DWORD dwStyle, DWORD dwTabbedStyle = AFX_CBRS_REGULAR_TABS, DWORD dwControlBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE, CCreateContext * pContext = NULL);
	BOOL b = pane->Create(sk._title, _mframe, sk._rc, sk._bHasGripper, sk._nID, 
		sk._dwStyle | sk._dwTabbedStyle | sk._dwControlBarStyle
		, sk._dwTabbedStyle, sk._dwControlBarStyle);
	if(b)
	{
		pane->EnableDocking(CBRS_ALIGN_ANY);
		_mframe->DockPane(pane);
		m_docks[sk._nID] = pane;
	} else
	{
		CStringA s; s.Format("%s ��ŷ â�� ������ ���߽��ϴ�.\n", CStringA(sk._title));
		TRACE(s);
		return FALSE; // ������ ���߽��ϴ�.
	}
	return b;
}

CDockablePane* KDockPane::GetDockPane(UINT nID)
{
	if(m_docks.find(nID) != m_docks.end())
		return (CPaneForm*)m_docks[nID];
	return nullptr;
}

CDockFormChild* KDockPane::GetDockView(UINT nID)
{
	auto pf = (CPaneForm*)m_docks[nID];
	if(pf)
		return pf->m_pForm;
	return nullptr;
}


/*
, CDockablePane& dock, PS title, CWnd* pParent, CRect& rc, BOOL bHasGripper,
	UINT nID, DWORD dwStyle, DWORD dwTabbedStyle, DWORD dwControlBarStyle)
{
	//virtual BOOL Create(LPCTSTR lpszCaption, CWnd* pParentWnd, const RECT& rect, BOOL bHasGripper, UINT nID, DWORD dwStyle, DWORD dwTabbedStyle = AFX_CBRS_REGULAR_TABS, DWORD dwControlBarStyle = AFX_DEFAULT_DOCKING_PANE_STYLE, CCreateContext* pContext = NULL);
	BOOL b = dock.Create(title, pParent, rc, bHasGripper, nID, dwStyle, dwTabbedStyle, dwControlBarStyle);
	if(b)
	{
		m_docks.SetAt(nID, &dock);
	}
	return b;
}

int wsl = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI;

if(!CreateDockingWindows())

BOOL CMainFrame::CreateDockingWindows()

if(!CreateDockingWin(m_place, L"��Ī�˻�", this, CRect(0, 0, 200, 200), TRUE, ID_PANE_m_place, wsl)) // 4444�� ���� ID_VIEW_PROPERTIESWND �����鼭 ���� ���� �߻�
return FALSE; // ������ ���߽��ϴ�.
*/



BOOL DockClientBase::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	BOOL b = FALSE;
	//try
	//{
		//try
		//{
			b = CDockFormChild::PreTranslateMessage(pMsg);
	//	}//CATCH_GE;
	//	catch(KException* e)
	//	{
	//		TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTION__, __LINE__);
	//		throw e;
	//	}
	//	catch(CException* e)
	//	{
	//		auto buf = new TCHAR(1024);
	//		KAtEnd d_buf([&]() { delete buf; });
	//		e->GetErrorMessage(buf, 1000);
	//		TRACE(L"CException:%s - %s %d\n", buf, __FUNCTION__, __LINE__);
	//		throw new KException("CException", GetLastError(), 0, buf, NULL, __FUNCTION__, __LINE__);
	//	}
	//	catch(std::exception& e)
	//	{
	//		TRACE("catch std::exception %s  - %s %d\n", e.what(), __FUNCTION__, __LINE__);
	//		throw new KException("std::exception", -1, 0, CStringW(e.what()), NULL, __FUNCTION__, __LINE__);
	//	}
	//	catch(PWS e)
	//	{
	//		TRACE("catch LPCWSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__);
	//		throw new KException("LPCWSTR", GetLastError(), 0, e, NULL, __FUNCTION__, __LINE__);
	//	}
	//	catch(PAS e)
	//	{
	//		TRACE("catch LPCSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__);
	//		throw new KException("LPCSTR", GetLastError(), 0, CStringW(e), NULL, __FUNCTION__, __LINE__);
	//	}
	//	catch(...)
	//	{
	//		TRACE("catch ...  - %s %d\n", __FUNCTION__, __LINE__);
	//		throw new KException("Unknown", GetLastError(), 0, L"Unknown catch(...) Error.", NULL, __FUNCTION__, __LINE__);
	//	}
	//}
	//catch(KException* e)
	//{
	//	TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTION__, __LINE__);
	//}
	//catch(std::exception& e)
	//{
	//	TRACE("catch std::exception %s  - %s %d\n", e.what(), __FUNCTION__, __LINE__);
	//}
	//catch(PWS e)
	//{
	//	TRACE("catch LPCWSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__);
	//}
	//catch(PAS e)
	//{
	//	TRACE("catch LPCSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__);
	//}
	//catch(...)
	//{
	//	TRACE("catch ...  - %s %d\n", __FUNCTION__, __LINE__);
	//}

	return b;
}


LRESULT DockClientBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	LRESULT lr = 0;
	try
	{
		try
		{
			lr = CDockFormChild::WindowProc(message, wParam, lParam);
		} CATCH_APP;
	}
	catch(KException* e)/// �׸��� ���Ⱑ 2��°�� ���� ���⼭ ��� ���� ó�� �ؾ� �Ѵ�.
	{
		CRuntimeClass* rc = this->GetRuntimeClass();
		// __FUNCTION__ �ϸ� �� �Լ��� �Ѿ� ����
		e->m_strStateNativeOrigin = rc->m_lpszClassName;
		TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTION__, __LINE__);
		//auto pfn = (CMainFrame*)AfxGetMainWnd();
		AppBase::CallClientException(e);//DockClientBase
		//ASSERT(e->IsAutoDelete());///�ƴҼ��� ������ �ݵ�� �Ʒ� ->Delete()�� ��� �Ѵ�.
		e->Delete();
	}
	return lr;
}

void DockClientBase::SetEnum(JArr& jar)
{
	_mapEnum.clear();
	for(auto& sjv : jar)
	{
		auto sjo = sjv->AsObject();
		wstring fTable = sjo->S("fTable");
		KStdMap<wstring, wstring>* pt = nullptr;
		if(!_mapEnum.Lookup(fTable, pt))
		{
			pt = new KStdMap<wstring, wstring>();
			_mapEnum[fTable] = pt;
		}
		pt->SetAt(sjo->S("fCode"), sjo->S("fKR"));

	}
}

void DockClientBase::DoList(ShJObj sjout)
{
	if(!tchsame(sjout->S("Return"), L"OK"))
	{
		InsertDataToList(false);//û��
		InitPropList();//û��
		return;
	}

	auto sjaTable = sjout->Array("table");
	auto sjaEnum = sjout->Array("tenum");

	if(sjaTable && sjaEnum)//sjout->Has("table"))
	{
		SetEnum(*sjaEnum);

		if(_sortKey.empty())
		{
			int nCols = 0;
			auto parinfo = getListFieldInfo(&nCols);
			_sortKey = (PAS)CStringA(parinfo[1].field);// "fName";
		}

		MakeSort(sjaTable);
		InsertDataToList();
		InitPropList();//û��
	}

}

/// ��Ģ: eShowDock1 �ϋ��� ������ ���δ�.
/// ��ũ�� �⺻ �̰� ������ ������ eNoBlinkBmp2 �� �ش�.
BOOL DockClientBase::DefaultSync(int iOp)
{

	if(!IsShown())
	{
		if(KwAttr(iOp, eShowDock1))
		{
			Pane()->ShowDock();
			//_KwBeginInvoke(this, [&]()	{	});
		}
		else
			return FALSE;
	}
	if(!KwAttr(iOp, eNoBlinkBmp2))
	{
		StartBlink();
		//_KwBeginInvoke(this, [&]()	{	});
	}
	return TRUE;
}

CStringA DockClientBase::GetPrGridFullKey(CMFCPropertyGridCtrl* cProperty)
{
	CMFCPropertyGridProperty* sel = cProperty->GetCurSel();// const { return m_pSel; }
	if(sel)
	{
		int k = (int)sel->GetData();
		string fk = _prData->FindFullkey(k);
		return fk.c_str();
	}
	return nullptr;
}

ShJObj DockClientBase::FindByID(PWS fObjID, PAS keyObj, PAS table)
{
	auto sjaTable = _sjo->Array(table);
	if(sjaTable)
	{
		for(auto& sjv : *sjaTable)// ���Ͼ� ��ġ
		{
			auto sjoRow = sjv->AsObject();
			if(sjoRow->size() == 0)
				_break;
			if(sjoRow->SameS(keyObj, fObjID))//�߰� "fPetID"
				return sjoRow;
		}
	}
	return nullptr;
}

void DockClientBase::ClearList(int iOp)
{
	if(KwAttrDel(iOp, ePostAsync8))
	{
		_KwBeginInvoke(this, [&, iOp]()
			{
				ClearList(iOp);
			});
		return;
	}
	auto cList = getListCtrl();
	if(cList)
	{
		cList->DeleteAllItems();
		_data.RemoveAll();
		InitPropList();
	}
}
