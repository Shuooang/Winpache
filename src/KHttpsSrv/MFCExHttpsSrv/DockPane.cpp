#include "pch.h"

#include "KwLib64/inlinefnc.h"

#include "DockPane.h"

#include "resource.h"
#include "MainFrm.h"

#include "DockTestApi.h"
#include "DockOdbc.h"





IMPLEMENT_DYNCREATE(DockClientBase, CDockFormChild)


DockClientBase::DockClientBase(UINT nID, CPaneForm* ppn)
	: CDockFormChild(nID, ppn) //DockEx2::IDD IDD_DockEx2
{
}

DockClientBase::~DockClientBase()
{
	/// 이거 보다 먼저 OnDestroy 에서 _prData->RemovaAll() 해야 컨트롤 관련 해서 오류 안난다.
}
//#pragma comment(lib, "msimg32.lib")
//[출처] LoadBitmap 함수 | 작성자 히니즈



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


void DockClientBase::OnInitialUpdate()
{
	__super::OnInitialUpdate();
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
//_##DockEx1, FormView 안쓰고 해보려고
#define SettingNoIdDOCK(type, title, style) \
	SettingDock(type##Pane::GetThisClass(), title, 0, style)

#define SettingDOCK(type, title, style) \
	SettingDock(type##Pane::GetThisClass(), title, IDD_##type, CBRS_##style, IDI_##type##_HC)
///  SettingDock(DockEx1Pane::GetThisClass(), L"샘플독1", IDD_DockEx1, CBRS_LEFT),

void KDockPane::InitDocks(CFrameWnd* mframe)
{
	_mframe = (CMDIFrameWndEx*)mframe;
	//여기서 도킹 셋팅이 다 된다.
	SettingDock sk[] =
	{
		// 프로퍼티를 직접 붙이고 따라 해보려 했는데 안됨. 그냥 폼뷰를 붙이기로 함.
		//SettingDock(DockPetDetail::GetThisClass(), L"펫 상세", IDD_DockPetDetail, CBRS_LEFT),

		/// 샘플 독
		//SettingDOCK(DockEx2, L"DockEx2", LEFT),

		/// 앞으로 만들 독
		SettingDOCK(DockTestApi, L"Test API", RIGHT),
		SettingDOCK(DockOdbc, L"Connect to DB", RIGHT),

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
		paneForm->_nID = sk._nID;// 이 pane이 아니고 child formview의 ID
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
	pane->_nID = sk._nID;// 이 pane이 아니고 child formview의 ID
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
	}
	else
	{
		CStringA s; s.Format("%s 도킹 창을 만들지 못했습니다.\n", CStringA(sk._title));
		TRACE(s);
		return FALSE; // 만들지 못했습니다.
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

if(!CreateDockingWin(m_place, L"명칭검색", this, CRect(0, 0, 200, 200), TRUE, ID_PANE_m_place, wsl)) // 4444를 위의 ID_VIEW_PROPERTIESWND 같으면서 각종 오류 발생
return FALSE; // 만들지 못했습니다.
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
		//try
		{
			lr = CDockFormChild::WindowProc(message, wParam, lParam);
		} //CATCH_APP;
	}
	catch(KException* e)/// 그리고 여기가 2번째로 오니 여기서 모든 오류 처리 해야 한다.
	{
		CRuntimeClass* rc = this->GetRuntimeClass();
		// __FUNCTION__ 하면 이 함수가 넘어 가서
		e->m_strStateNativeOrigin = rc->m_lpszClassName;
		KwMessageBoxError(L"KException:%s - %s %d\n", e->m_strError, __FUNCTIONW__, __LINE__);
		//auto pfn = (CMainFrame*)AfxGetMainWnd();
		//AppBase::CallClientException(e);//DockClientBase
		//ASSERT(e->IsAutoDelete());///아닐수도 있으니 반드시 아래 ->Delete()를 써야 한다.
		e->Delete();
	}
	return lr;
}


void DockClientBase::ShowHide(CMDIFrameWndEx* frm, CBasePane& win, BOOL bShow)
{
	win.ShowPane(bShow, TRUE, TRUE);
	frm->RecalcLayout(FALSE);
}

/// 이건 BOOL 값이 따로 없고, IsVisible 로 하고 저장은 윈도셋팅에 저장 되므로 BOOL 값으로 Setting 과는 상관 없다. 
void DockClientBase::ShowHide(CMDIFrameWndEx* frm, CBasePane& win, CCmdUI* pCmdUI)//?petme dock showhide
{
	if(win.GetSafeHwnd() == NULL)
		return;

	if(pCmdUI)
	{
		pCmdUI->SetCheck(win.IsVisible());
	}
	else
	{
		win.ShowPane(!win.IsVisible(), TRUE, TRUE);
		frm->RecalcLayout(FALSE);
	}
}