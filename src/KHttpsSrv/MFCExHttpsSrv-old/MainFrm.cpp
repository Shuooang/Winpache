// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface
// (the "Fluent UI") and is provided only as referential material to supplement the
// Microsoft Foundation Classes Reference and related electronic documentation
// included with the MFC C++ library software.
// License terms to copy, use or distribute the Fluent UI are available separately.
// To learn more about our Fluent UI licensing program, please visit
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "MFCExHttpsSrv.h"

#include "MainFrm.h"
#include "SrvView.h"
#include "CmnDoc.h"
#include "ApiSite1.h"
#include "KwLib64/MfcEx.h"
#include "DockTestApi.h"

#pragma comment(lib, "shell32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETTINGCHANGE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_VIEW_FILEVIEW, &CMainFrame::OnViewFileView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILEVIEW, &CMainFrame::OnUpdateViewFileView)
	ON_COMMAND(ID_VIEW_CLASSVIEW, &CMainFrame::OnViewClassView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CLASSVIEW, &CMainFrame::OnUpdateViewClassView)
	ON_COMMAND(ID_VIEW_OUTPUTWND, &CMainFrame::OnViewOutputWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUTWND, &CMainFrame::OnUpdateViewOutputWindow)
	ON_COMMAND(ID_VIEW_PROPERTIESWND, &CMainFrame::OnViewPropertiesWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIESWND, &CMainFrame::OnUpdateViewPropertiesWindow)
	ON_COMMAND(ID_Start, &CMainFrame::OnRibbonStart)
	ON_UPDATE_COMMAND_UI(ID_Start, &CMainFrame::OnUpdateStart)
	ON_COMMAND(ID_Stop, &CMainFrame::OnRibbonStop)
	ON_UPDATE_COMMAND_UI(ID_Stop, &CMainFrame::OnUpdateStop)
	ON_COMMAND(ID_Restart, &CMainFrame::OnRibbonRestart)
	ON_UPDATE_COMMAND_UI(ID_Restart, &CMainFrame::OnUpdateRestart)
	ON_COMMAND(ID_StartDB, &CMainFrame::OnRibbonStartDB)
	ON_UPDATE_COMMAND_UI(ID_StartDB, &CMainFrame::OnUpdateStartDB)
	ON_COMMAND(ID_VIEW_PROPERTIES, &CMainFrame::OnViewProperties)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, &CMainFrame::OnUpdateViewProperties)
	ON_COMMAND(ID_FreeLibrary, &CMainFrame::OnFreeLibrary)
	ON_COMMAND(ID_DownloadMariaDB, &CMainFrame::OnDownloadMariaDB)
	ON_COMMAND(ID_DownloadMariaODBC, &CMainFrame::OnDownloadMariaODBC)
	ON_COMMAND(ID_TestAPI, &CMainFrame::OnTestAPI)
	ON_COMMAND(ID_RunClient, &CMainFrame::OnRunClient)
	ON_COMMAND(ID_SiteProject, &CMainFrame::OnSiteProject)
	ON_COMMAND(ID_InstallMariaDB, &CMainFrame::OnInstallMariaDB)
	ON_COMMAND(ID_InstallMariaODBC, &CMainFrame::OnInstallMariaODBC)
	ON_COMMAND(ID_OdbcSetting, &CMainFrame::OnOdbcSetting)
	ON_COMMAND(ID_VisualStudioDownload, &CMainFrame::OnVisualStudioDownload)
	ON_COMMAND(ID_BuildProject, &CMainFrame::OnBuildProject)
	ON_COMMAND(ID_CreateDatabase, &CMainFrame::OnCreateDatabase)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
	mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
	mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
	mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Create a caption bar:
	if (!CreateCaptionBar())
	{
		TRACE0("Failed to create caption bar\n");
		return -1;      // fail to create
	}

	// Load menu item image (not placed on any standard toolbars):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = nullptr;
	m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	// Enable enhanced windows management dialog
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// Switch the order of document name and application name on the window title bar. This
	// improves the usability of the taskbar because the document name is visible with the thumbnail.
	ModifyStyle(0, FWS_PREFIXTITLE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// Create class view
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	ASSERT(bNameValid);
	if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Class View window\n");
		return FALSE; // failed to create
	}

	// Create file view
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create File View window\n");
		return FALSE; // failed to create
	}

	// Create output window
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Output window\n");
		return FALSE; // failed to create
	}

	// Create properties window
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties window\n");
		return FALSE; // failed to create
	}

	/// /////////////////////////////////////////////////////////////
	_dock.InitDocks(this);///?kdw 
	/// /////////////////////////////////////////////////////////////


	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("Failed to create caption bar\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::ContextCategory(int idd, bool bShow)
{
	KRibbon::ContextCategory(this, &m_wndRibbonBar, bShow, idd);
}


void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}
void CMainFrame::CaptionMessage(PWS msg)
{
	CString smsg(msg);
	m_wndCaptionBar.SetText(smsg, CMFCCaptionBar::ALIGN_LEFT);
	m_wndCaptionBar.ShowWindow(SW_SHOW);
	RecalcLayout(FALSE);
}
void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}
void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}
void CMainFrame::OnViewProperties()
{
// 	m_wndProperties.ShowWindow(m_wndProperties.IsVisible() ? SW_HIDE : SW_SHOW);
// 	RecalcLayout(FALSE);
	m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
	m_wndProperties.SetFocus();
}
void CMainFrame::OnUpdateViewProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndProperties.IsVisible());
}

void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != nullptr);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}

void CMainFrame::OnViewFileView()
{
	// Show or activate the pane, depending on current state.  The
	// pane can only be closed via the [x] button on the pane frame.
	m_wndFileView.ShowPane(TRUE, FALSE, TRUE);
	m_wndFileView.SetFocus();
}

void CMainFrame::OnUpdateViewFileView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewClassView()
{
	// Show or activate the pane, depending on current state.  The
	// pane can only be closed via the [x] button on the pane frame.
	m_wndClassView.ShowPane(TRUE, FALSE, TRUE);
	m_wndClassView.SetFocus();
}

void CMainFrame::OnUpdateViewClassView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewOutputWindow()
{
	// Show or activate the pane, depending on current state.  The
	// pane can only be closed via the [x] button on the pane frame.
	m_wndOutput.ShowPane(!m_wndOutput.IsVisible(), FALSE, TRUE);
	m_wndOutput.SetFocus();
}

void CMainFrame::OnUpdateViewOutputWindow(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndOutput.IsVisible());
}

void CMainFrame::OnViewPropertiesWindow()
{
	// Show or activate the pane, depending on current state.  The
	// pane can only be closed via the [x] button on the pane frame.
	m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
	m_wndProperties.SetFocus();
}

void CMainFrame::OnUpdateViewPropertiesWindow(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndEx::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::OnClose()
{
	auto fr = this->GetActiveFrame();
	if(fr)
	{
		auto vu = fr->GetActiveView();
		if(vu)
		{
			auto cvu = dynamic_cast<CmnView*>(vu);
			if(cvu)
				cvu->Shutdown();//?Shutdown CChildFrame::OnClose() 에서도 불러야 한다.
		}
	}
	CMDIFrameWndEx::OnClose();
}


CSrvView* CMainFrame::GetActiveCmnView()
{
	auto fr = this->GetActiveFrame();
	if(fr)
	{
		auto vu = (CSrvView*)fr->GetActiveView();
		return vu;
// 		if(vu)
// 		{
// 			auto cvu = dynamic_cast<CSrvView*>(vu);
// 			return cvu;
// 		}
	}
	return nullptr;
}
void CMainFrame::OnUpdateCmn(CCmdUI* pCmdUI, int idc)
{
	auto cvu = GetActiveCmnView();
	if(cvu)
	{
		BOOL b = cvu->GetUpdate(idc);//CChildFrame::OnClose() 에서도 불러야 한다.
		pCmdUI->Enable(b);
	}
}


void CMainFrame::OnRibbonStart()
{
	auto cvu = GetActiveCmnView();
	if(cvu)
		cvu->OnBnClickedStart();//CChildFrame::OnClose() 에서도 불러야 한다.
}
void CMainFrame::OnUpdateStart(CCmdUI* pCmdUI)
{
	OnUpdateCmn(pCmdUI, IDC_Start);
}
void CMainFrame::OnRibbonStop()
{
	auto cvu = GetActiveCmnView();
	if(cvu)
		cvu->OnBnClickedStop();
}
void CMainFrame::OnUpdateStop(CCmdUI* pCmdUI)
{
	OnUpdateCmn(pCmdUI, IDC_Stop);
}
void CMainFrame::OnRibbonRestart()
{
	auto cvu = GetActiveCmnView();
	if(cvu)
		cvu->OnBnClickedRestart();
}
void CMainFrame::OnUpdateRestart(CCmdUI* pCmdUI)
{
	OnUpdateCmn(pCmdUI, IDC_Restart);
}
void CMainFrame::OnRibbonStartDB()
{
	auto cvu = GetActiveCmnView();
	if(cvu)
		cvu->OnBnClickedStartDB();
}
void CMainFrame::OnUpdateStartDB(CCmdUI* pCmdUI)
{
	OnUpdateCmn(pCmdUI, IDC_StartDB);
}


void CMainFrame::OnFreeLibrary()
{
	CaptionMessage(L"Step 6. In order to overwrite the DLL file, you must first temporarily stop all servers.");
	auto cvu = dynamic_cast<CmnView*>(GetActiveCmnView());
	if(cvu)
	{
		auto doc = cvu->GetDocument();
		auto res = doc->_svr;
		auto api = res->_api;
		auto dll = api->getExLibName();
		if(doc->FreeDllLibrary() == 0)
			KwMessageBox(L"Step 6. Now, the API library [%s] can be changed.", api->getExLibName());
		else
			KwMessageBox(L"Step 6. Error, while free library [%s].", api->getExLibName());
	}
}


void CMainFrame::OnDownloadMariaDB()
{
	CString fl = L".\\Downloads - MariaDB.url";
// 	ShellExecuteW(_In_opt_ HWND hwnd, _In_opt_ LPCWSTR lpOperation, _In_ LPCWSTR lpFile, _In_opt_ LPCWSTR lpParameters,
// 		_In_opt_ LPCWSTR lpDirectory, _In_ INT nShowCmd);
	if(KwIfFileExist(fl))
	{
		::ShellExecute(0, 0, fl, 0, 0, SW_SHOW);
		CaptionMessage(L"Navigating to download site for MariaDB!");
	}
	else
		KwMessageBox(L"File \"%s\" is not found.", fl);
}


void CMainFrame::OnDownloadMariaODBC()
{
	CString fl = L".\\MariaDB Connector-ODBC - MariaDB.url";
	if(KwIfFileExist(fl))
	{
		::ShellExecute(0, 0, fl, 0, 0, SW_SHOW);
		CaptionMessage(L"Navigating to download site for MariaDB ODBC driver!");
	}
	else
		KwMessageBox(L"File \"%s\" is not found.", fl);

	// TODO: Add your command handler code here
}

#define OnVIEWMAN(XXXX) \
void CMainFrame::OnView##XXXX() \
{	DockTool::ShowHide(this, *Pane(IDD_##XXXX)); \
} \
void CMainFrame::OnUpdateView##XXXX(CCmdUI* pCmdUI) \
{	DockTool::ShowHide(this, *Pane(IDD_##XXXX), pCmdUI); \
}

void CMainFrame::OnTestAPI()
{
//	KwMessageBox(L"Reserved!");
	//m_wndFileView.ShowPane(TRUE, FALSE, TRUE);
	//m_wndFileView.SetFocus();
	DockClientBase::ShowHide(this, *Pane(IDD_DockTestApi));
}


void CMainFrame::OnRunClient()
{
	PWS fname = L"Wincatcher.exe";
	CString fl; fl.Format(L".\\%s", fname);
	if(KwIfFileExist(fname))
	{

		HINSTANCE hi = ::ShellExecute(0, 0, fl, 0, 0, SW_SHOW);
		if((LONGLONG)hi > 32L)
		{
			CString s; s.Format(L"HTTP client test application [%s] is run.", fname);
			CaptionMessage(s);
		}
		else
		{
			CString s; s.Format(L"Error while launcching the HTTP client test application [%s].", fname);
			CaptionMessage(s);
		}
	}
	else
		KwMessageBoxError(L"File (%s) is not found.", fl);


}


void CMainFrame::OnSiteProject()
{
	WCHAR my_documents[MAX_PATH];//CSIDL_PERSONAL
	HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, my_documents);
	CStringW flPrj = my_documents;
	PWS tdir = L"\\Winpache\\src\\KHttpsSrv\\";// KHttpsSrvSite.sln";
	PWS fsln = L"KHttpsSrvSite.sln";
	flPrj += tdir;
	CString sDir = flPrj;
	flPrj += fsln;
	HINSTANCE hi = ::ShellExecute(0, 0, flPrj, 0, sDir, SW_SHOW);
	//ERROR_FILE_NOT_FOUND
	if((LONGLONG)hi > 32L)
	{
		CString s; s.Format(L"Step 5. Open the Project [%s] for a Site DLL.", flPrj);
		CaptionMessage(s);
	}
	else
	{
		KwMessageBoxError(L"Step 5. Error occurred!\n\
You can open the '*.sln' file directly from the directory [%s%s]. CurDir:%s", my_documents, tdir, sDir);
	}
}


void CMainFrame::OnOdbcSetting()
{
//#define CSIDL_SYSTEM                    0x0025        // GetSystemDirectory()

	WCHAR my_documents[MAX_PATH]{ 0, };
	//WCHAR my_documents2[MAX_PATH]{ 0, };
	//HRESULT result = SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, my_documents);
	::GetSystemDirectory(my_documents, MAX_PATH);//위랑 결과가 같다.
	CStringW flPrj = my_documents;
	flPrj += L"\\odbcad32.exe";
	::ShellExecute(0, 0, flPrj, 0, 0, SW_SHOW);
	{
		CString s; s.Format(L"Step 3. Running [%s] to set up ODBC DSN.", flPrj);
		CaptionMessage(s);
	}
}
// https://visualstudio.microsoft.com/{ko/}thank-you-downloading-visual-studio/?sku=Community&rel=16

void CMainFrame::OnOdbcSetting()
{
	DlgOdbcSetting dlg;
	dlg.DoModal();
}
void CMainFrame::OnVisualStudioDownload()
{
	TCHAR curDir[1002];
	GetCurrentDirectory(1000, curDir);//C:\Users\dwkang\AppData\Roaming\Outbin\KHttpsSrv\x64\Debug

	WCHAR appdata[MAX_PATH];//CSIDL_PERSONAL
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, appdata);

	PWS fname = L"Visual Studio Download.url";
	CString fl; fl.Format(L"%s\\Keepspeed\\Winpache\\", appdata);
	CString full = fl + fname;
	if(KwIfFileExist(full))
	{
		::ShellExecute(0, 0, full, 0, fl, SW_SHOW);
		CaptionMessage(L"Step 4. Download and install the Visual Studio Community!");
	}
	else
		KwMessageBox(L"Step 4. Error occurred.\nYou can download and install Visual Studio from the site.\n%s", full);
}


void CMainFrame::OnInstallMariaDB()
{
	TCHAR curDir[1002];
	GetCurrentDirectory(1000, curDir);//C:\Users\dwkang\AppData\Roaming\Outbin\KHttpsSrv\x64\Debug

	WCHAR appdata[MAX_PATH];//CSIDL_PERSONAL
	HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appdata);

	PWS fname = L"mariadb-10.5.10-winx64.msi";
	CString fl; fl.Format(L"%s\\Winpache\\bin\\", appdata);
	CString full = fl + fname;
	//CString fl; fl.Format(L"..\\..\\%s", fname);
	if(KwIfFileExist(full))
	{

		HINSTANCE hi = ::ShellExecute(0, 0, full, 0, fl, SW_SHOW);
		if((LONGLONG)hi > 32L)
		{
			CString s; s.Format(L"Step 1. This starts installing the MaraiDB [%s].", fname);
			CaptionMessage(s);
		}
		else
		{
			CString s; s.Format(L"Step 1. Error (%d) while installing DB [%s]. Dir:%s, CurDir:%s", (int)hi, fname, fl, curDir);
			CaptionMessage(s);
		}
	}
	else
		KwMessageBoxError(L"Step 1. File (%s) is not found.", full);
}


void CMainFrame::OnInstallMariaODBC()
{
	WCHAR appdata[MAX_PATH];//CSIDL_PERSONAL
	HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appdata);
	CString fl; fl.Format(L"%s\\Winpache\\bin\\", appdata);
	PWS fname = L"mariadb-connector-odbc-3.1.12-win64.msi";
	CString full = fl + fname;
	//CString fl; fl.Format(L"..\\..\\%s", fname);// bin\x64\Release
	if(KwIfFileExist(full))
	{
		HINSTANCE hi = ::ShellExecute(0, 0, full, 0, fl, SW_SHOW);
		if((LONGLONG)hi > 32L)
		{
			CString s; s.Format(L"Step 2. This starts installing the MaraiDB ODBC Dirver [%s].", fname);
			CaptionMessage(s);
		}
		else
		{
			CString s; s.Format(L"Step 2. Error (%d) while installing ODBC [%s].  Dir:%s", (int)hi, fname, fl);
			CaptionMessage(s);
		}
	}
	else
		KwMessageBoxError(L"Step 2. File (%s) is not found.", full);
}


void CMainFrame::OnBuildProject()
{
	auto cvu = dynamic_cast<CmnView*>(GetActiveCmnView());
	if(cvu)
	{
		auto doc = cvu->GetDocument();
		auto res = doc->_svr;
		auto api = res->_api;
		auto dll = api->getExLibName();
		CString s;s.Format(L"Step 7. You can now build the modified code to overwrite the DLL file [%s].", dll);
		CaptionMessage(s);
		KwMessageBox(s);
	}
}


void CMainFrame::OnCreateDatabase()
{
	DockClientBase::ShowHide(this, *Pane(IDD_DockTestApi), TRUE);
	DockTestApi::s_me->CreateDatabase();

}
