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
#include "DlgOdbcSetting.h"
#include "DockOdbc.h"

#pragma comment(lib, "shell32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndExInvokable)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndExInvokable)
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
// 	ON_COMMAND(ID_VIEW_PROPERTIESWND, &CMainFrame::OnViewPropertiesWindow)
// 	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIESWND, &CMainFrame::OnUpdateViewPropertiesWindow)
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
	ON_COMMAND(ID_CreateDatabase, &CMainFrame::OnCreateDatabase)
	ON_COMMAND(ID_InitODBC, &CMainFrame::OnInitodbc)
	ON_COMMAND(ID_TableBasic, &CMainFrame::OnCreateTableBasic)
	ON_COMMAND(ID_DownloadOpenSSL, &CMainFrame::OnDownloadOpenSSL)
	ON_COMMAND(ID_DockOdbcSetting, &CMainFrame::OnDockOdbcSetting)
	ON_COMMAND(ID_VIEW_TestAPI, &CMainFrame::OnViewTestAPI)
// 	ON_COMMAND(ID_VIEW_OdbcConnect, &CMainFrame::OnViewOdbCconnect)
// 	ON_UPDATE_COMMAND_UI(ID_VIEW_OdbcConnect, &CMainFrame::OnUpdateViewOdbcConnect)
	//ON_COMMAND(ID_TableSample, &CMainFrame::OnTableSample)
//	ON_COMMAND(ID_BuildProject, &CMainFrame::OnBuildProject)
ON_UPDATE_COMMAND_UI(ID_VIEW_TestAPI, &CMainFrame::OnUpdateViewTestAPI)
	ON_COMMAND(ID_GoToProject, &CMainFrame::OnGotoProject)
	ON_COMMAND(ID_GotoOutput, &CMainFrame::OnGotoOutput)
	ON_COMMAND(ID_RunHeidiSQL, &CMainFrame::OnRunHeidiSQL)
	ON_COMMAND(ID_CopyRequest, &CMainFrame::OnCopyRequest)
	ON_COMMAND(ID_CopyOutput, &CMainFrame::OnCopyOutput)
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
	if (CMDIFrameWndExInvokable::OnCreate(lpCreateStruct) == -1)
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

	KwBeginInvoke(this, ([&]()-> void { //?beginInvoke 4
		ConnectMainDB();
		}));

	return 0;
}

void CMainFrame::ConnectMainDB()
{
	try
	{
		auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_doc;
		
		KWStrMap kmap1;
		int rv = KDatabase::RegGetODBCMySQL(appd._DSN, kmap1);
		auto Driver0 = kmap1.Get(L"ODBC Data Sources");
		auto Driver1 = kmap1.Get(L"Driver");
	 	auto UID1 = kmap1.Get(L"UID");
	 	auto PWD1 = kmap1.Get(L"PWD");
		auto& t = appd._statDB;
		if(t == "none" || t == "installDB")
		{
			TRACE(L"ODBC is not configured yet.\n");
		}
		else if (kmap1.size() > 0 && Driver0.length() > 0 && Driver0 == Driver1 
			&& UID1.length() > 0 && PWD1.length() > 0)// && uid1 == uid0)
		{
			CString dsn;
			dsn.Format(L"DSN=%s", appd._DSN);//PWD가 있는거 ODBC에 확인 했으니, DSN만으로 로그인 시도
			BOOL bOK = appd._dbMain->OpenEx(dsn, CDatabase::noOdbcDialog);
			if(bOK)
			{
				try
				{
					appd._dbMain->ExecuteSQL(L"use `winpache`");
					appd._statDB = L"login";
					appd.SaveData();
				}
				catch (CDBException* e)
				{
					CString smsg; smsg.Format(L"Error! Database `winpache` is not created yet.\n%s, %s.", e->m_strError, e->m_strStateNativeOrigin);
					TRACE(L"%s\n", smsg);
				}
			}
		}else
		{
			// 만약 statDB가 ODBC 인데, 값이 비어 있으면 도킹창 ODBC로 간다.
			DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc), TRUE);
		}
	}
	catch (CDBException* e)
	{
		//Access denied for user 'root'@'localhost' (using password: YES)
		//State:28000, Native : 1045, Origin : [ma - 3.1.12]
		PWS Access_denied = L"State:28000";
		if (tchstr((PWS)e->m_strStateNativeOrigin, Access_denied))
		{
			KwMessageBoxError(L"While connecting to main DB.\n%s.\nRetry on 'Connect to DB' docking window'.", e->m_strError);
			DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc), TRUE);
		}
		CString smsg; smsg.Format(L"Error! %s, %s.", e->m_strError, e->m_strStateNativeOrigin);
		CaptionMessage(smsg);
	}
	catch (CException* e)
	{
		CString serr;
		auto ek = dynamic_cast<KException*>(e);
		if (ek)
			serr = ek->m_strError;
		else
			e->GetErrorMessage(serr.GetBuffer(1024), 1024); serr.ReleaseBuffer();
		CString smsg; smsg.Format(L"Error: %s.", serr);
		CaptionMessage(smsg);
	}

}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndExInvokable::PreCreateWindow(cs) )
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
	CMDIFrameWndExInvokable::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndExInvokable::Dump(dc);
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
	COutputWnd::s_me->Trace(msg);
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
	m_wndProperties.ShowPane(!m_wndProperties.IsVisible(), FALSE, TRUE);
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
	m_wndFileView.ShowPane(!m_wndFileView.IsVisible(), FALSE, TRUE);
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
	m_wndClassView.ShowPane(!m_wndClassView.IsVisible(), FALSE, TRUE);
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
// 	m_wndProperties.ShowPane(!m_wndProperties.IsVisible(), FALSE, TRUE);
// 	m_wndProperties.SetFocus();
}

void CMainFrame::OnUpdateViewPropertiesWindow(CCmdUI* pCmdUI)
{
	//pCmdUI->Enable(TRUE);
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndExInvokable::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::OnClose()
{
	// template는 하나.
	for (POSITION posTemplate = AfxGetApp()->GetFirstDocTemplatePosition(); posTemplate; )
	{
		/// doc은 열고 있는 문서 갯수
		auto pDocTemplate = AfxGetApp()->GetNextDocTemplate(posTemplate);
		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		while (posDoc)
		{
			CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
			/// view는 각 문서에 View가 여러개 인경우 doc 단위 이고 view에 함수가 있으므로 여러개 view라 하더 라도 한번만 실행 한다.
			POSITION posView = pDoc->GetFirstViewPosition();
			while (posView)
			{
				CView* pView = pDoc->GetNextView(posView);
				auto cvu = dynamic_cast<CmnView*>(pView);
				if (cvu)
					cvu->Shutdown("CMainFrame::OnClose()");//CChildFrame::OnClose() 에서도 불러야 한다.
				//pView->GetParentFrame()->DestroyWindow();
				break;//현재 doc하나 당 single view
			}
		}
	}


/*
	auto vu = GetActiveCmnView();
// 	auto fr = this->GetActiveFrame();
// 	if(fr)
// 	{
// 		auto vu = fr->GetActiveView();
		if(vu)
		{
			auto cvu = dynamic_cast<CmnView*>(vu);
			if(cvu)
				cvu->Shutdown("CMainFrame::OnClose()");//?Shutdown CChildFrame::OnClose() 에서도 불러야 한다.
		}
//	}
*/
	CMDIFrameWndExInvokable::OnClose();
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
	CaptionMessage(L"In order to overwrite the DLL file, you must first temporarily stop the server.");
	auto cvu = dynamic_cast<CmnView*>(GetActiveCmnView());
	if(cvu)
	{
		auto doc = cvu->GetDocument();
		auto res = doc->_svr;
		auto api = res->_api;
		auto dll = api->getExLibName();
		if(doc->FreeDllLibrary() == 0)
			KwMessageBox(L"Now, the API library [%s] can be changed.", api->getExLibName());
		else
			KwMessageBox(L"Error, while freeing the library [%s].", api->getExLibName());
	}
}




#define OnVIEWMAN(XXXX) \
void CMainFrame::OnView##XXXX() \
{	DockTool::ShowHide(this, *Pane(IDD_##XXXX)); \
} \
void CMainFrame::OnUpdateView##XXXX(CCmdUI* pCmdUI) \
{	DockTool::ShowHide(this, *Pane(IDD_##XXXX), pCmdUI); \
}
void CMainFrame::OnViewOdbCconnect()
{
	DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc));
}
void CMainFrame::OnUpdateViewOdbcConnect(CCmdUI* pCmdUI)
{
	DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc), pCmdUI);
}
void CMainFrame::OnViewTestAPI()
{
	DockClientBase::ShowHide(this, *Pane(IDD_DockTestApi));
}
void CMainFrame::OnUpdateViewTestAPI(CCmdUI* pCmdUI)
{
	DockClientBase::ShowHide(this, *Pane(IDD_DockTestApi), pCmdUI);
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

/// Open the Site Project
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
		CString s; s.Format(L"Step 2. Open the Project [%s] for a Site DLL.", flPrj);
		CaptionMessage(s);
	}
	else
	{
		KwMessageBoxError(L"Step 2. Error occurred!\n\
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
void CMainFrame::OnInitodbc()
{
	InitOdbc(3);
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_doc;
	appd._statDB = "ODBC";
	appd.SaveData();

	CaptionMessage(L"Step 3. ODBC has been set up on the newly installed MariaDB..");
}
void CMainFrame::OnCreateDatabase()
{
	InitOdbc(4);
// 	DockClientBase::ShowHide(this, *Pane(IDD_DockTestApi), TRUE);
// 	DockTestApi::s_me->CreateDatabase();
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_doc;
	appd._statDB = "database";
	appd.SaveData();
	CaptionMessage(L"Step 4. A database 'winpache' for logs and samples has been created.");
}



void CMainFrame::OnCreateTableBasic()
{
	InitOdbc(5);
	CaptionMessage(L"Step 5. Main tables have been created.");
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_doc;
	appd._statDB = "table";
	appd.SaveData();
}

//?deprecated
void CMainFrame::OnTableSample()
{
	InitOdbc(6);
	CaptionMessage(L"Step 6. A few tables have been created for the sample API...");
}

void CMainFrame::InitOdbc(int step)
{
// 	DlgOdbcSetting dlg;
// 	dlg.DoModal();
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_doc;
	try
	{
		PWS dsn0 = L"Winpache";
		PWS uid0 = L"root";

		if(step == 4 || step == 5 || step == 6)
		{
			if (appd._dbMain->IsOpen())
				appd._dbMain->Reopen();
			else
			{
				KwMessageBoxError(L"Database is not connected. Select Step 3 to connect DB.");
				return;
			}
		}

		/// 1. no pwd로 로그인 시도 해보고, 되면 
		if(step == 3)// Step 3. init ODBC
		{
			KWStrMap kmap1;
			int rv = KDatabase::RegGetODBCMySQL(appd._DSN, kmap1);
// +		[L"DSN"]	L"MariaDB ODBC 3.1 Driver"
// +		[L"Driver"]	L"MariaDB ODBC 3.1 Driver"
// +		[L"PORT"]	L"3306"
// +		[L"PWD"]	L"hjkjjklxxx"
// +		[L"SERVER"]	L"localhost"
// +		[L"TCPIP"]	L"1"
// +		[L"UID"]	L"root"
			auto Driver0 = kmap1.Get(L"ODBC Data Sources");
			auto Driver1 = kmap1.Get(L"Driver");
			auto UID1 = kmap1.Get(L"UID");
			auto PWD1 = kmap1.Get(L"PWD");
			if(kmap1.size() > 0 && Driver0.length() > 0 && Driver0 == Driver1)// && uid1 == uid0)
			{//이미 있다.
				/// 1. PWD가 없는 경우 ODBC Setting창이 뜨고 비번을 입력 하게 한 후 접속 된다. 하지만 여기서 입력한 경우 비번이 저장 되지는 않는다.
				/// 2. PWD가 틀린 경우 CDBException이나고 비번이 틀린 오류 박스 아래에서 뜬다. ODBC Docking 창이 떠서 비번을 입력 받고 다시 접속 하도록 한다. 역시 입력한 PWD가 저장되지 않는다.
				appd._DSN = dsn0;// DSN1.c_str();// L"Winpache";
				appd._UID = UID1.c_str();
				if (PWD1.length() > 0)
					appd._PWD = L"**********";
				//appd._PWD = PWD1.c_str();
				DockOdbc::s_me->_DSN = appd._DSN;
				DockOdbc::s_me->_UID = appd._UID;
#ifdef _DEBUGx
				if (_PWD.GetLength())
					DockOdbc::s_me->_PWD = _PWD;
#endif // _DEBUGx

				DockOdbc::s_me->UpdateData(0); /// ODBC Setting창에 뿌려 준다.

				CString dsn; 
				if (!appd._UID.IsEmpty())
				{
					if (appd._PWD.IsEmpty())
						dsn.Format(L"DSN=%s;UID=%s", appd._DSN, appd._UID);//이거는 모든 필수 항목이 다 들어가 있고.
					else
						dsn.Format(L"DSN=%s;UID=%s;PWD=%s", appd._DSN, appd._UID, appd._PWD);//이거는 모든 필수 항목이 다 들어가 있고.
				}
				else
					dsn.Format(L"DSN=%s", appd._DSN);//이거는 모든 필수 항목이 다 들어가 있고.
				if (appd._dbMain->IsOpen())
					appd._dbMain->Close();
				appd._dbMain->OpenEx(dsn);//_T("DSN=UserInfo")); 여기서 비번 오류 난다.
				/// 아직 database 안만들었을수 있으니. appd._dbMain->ExecuteSQL(L"use `winpache`");

				auto smsg = L"Server default (Winpache) ODBC is already initialized. Now connected.";
				CaptionMessage(smsg);
				KwMessageBox(smsg);
				DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc), TRUE);
			}
			else
			{
				appd._DSN = dsn0;// L"Winpache";
				appd._UID = uid0;
				KWStrMap kmap;
				kmap[L"UID"] = (PWS)appd._UID;
				/// 1.1 ODBC no pwd로 처음 만들고
				int rv = KDatabase::RegODBCMySQL(appd._DSN, kmap);
				
				DockOdbc::s_me->_DSN = appd._DSN;
				DockOdbc::s_me->_UID = appd._UID;
				DockOdbc::s_me->UpdateData(0); /// ODBC Setting창에 뿌려 준다.
				DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc), TRUE);
				
				/// 1.2 접속 한다.
				CString dsn; dsn.Format(L"DSN=%s;UID=%s", appd._DSN, appd._UID);//이거는 모든 필수 항목이 다 들어가 있고.
				appd._dbMain->OpenEx(dsn);//_T("DSN=UserInfo"));
				//아직 database도 안만들었다. appd._dbMain->ExecuteSQL(L"use `winpache`");

				CaptionMessage(L"ODBC connected without password as root user to the installed DB server.");
				KwMessageBox(L"You can save the password in ODBC Setting.");
			}
		}
		else if (step == 4)
		{
			/// 3. 첫번째 database를 winpache를 만든다.
			appd._dbMain->ExecuteSQL(L"CREATE DATABASE `winpache` COLLATE 'utf16_unicode_ci'");
			appd._dbMain->ExecuteSQL(L"use `winpache`");
			KwMessageBox(L"Database `winpache` has been created.");
		}
		else if (step == 5)
		{
			appd._dbMain->ExecuteSQL(L"use `winpache`");
			WCHAR my_documents[MAX_PATH];//CSIDL_PERSONAL
			HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, my_documents);
			CStringW flPrj = my_documents;
			PWS tdir = L"\\Winpache\\DB\\";
			PWS arfsln[] = { 
				L"CREATE_TABLE_t_reqlog.sql",
				L"CREATE_TABLE_t_ldblog.sql",
				L"CREATE_TABLE_t_excepsvr.sql",
				L"CREATE_TABLE_tbiz.sql",
				L"CREATE_TABLE_tbizclass.sql", };
			flPrj += tdir;
			CString sDir = flPrj;
			for (auto fsql : arfsln)
			{
				CString fullw = flPrj + fsql;
				CStringA full(fullw);
				KDatabase::CreateTable(*appd._dbMain, full);
			}
			/// 4. 첫번째 log table을 만든다.
// 			KDatabase::CreateTable(*appd._dbMain, "..\\..\\DB\\CREATE_TABLE_t_reqlog.sql");
// 			KDatabase::CreateTable(*appd._dbMain, "..\\..\\DB\\CREATE_TABLE_t_excepsvr.sql");
// 			KDatabase::CreateTable(*appd._dbMain, "..\\..\\DB\\CREATE_TABLE_tbiz.sql");
// 			KDatabase::CreateTable(*appd._dbMain, "..\\..\\DB\\CREATE_TABLE_tbizclass.sql");
			KwMessageBox(L"Basic tables have been created.");
		}
		else if (step == 6)
		{
// 			appd._dbMain->ExecuteSQL(L"use `winpache`");
// 			/// 4. 첫번째 log table을 만든다.
// 			KDatabase::CreateTable(*appd._dbMain, "..\\..\\CREATE_TABLE_tbiz.sql");
// 			KDatabase::CreateTable(*appd._dbMain, "..\\..\\CREATE_TABLE_tbizclass.sql");
// 			KwMessageBox(L"Tables for the sample have been created..");
		}
	}
	catch (CDBException* e)
	{
		CString smsg; smsg.Format(L"Error! %s, (%d) %s.", e->m_strError, e->m_nRetCode, e->m_strStateNativeOrigin);
		CaptionMessage(smsg);
		KwMessageBoxError(smsg);
		DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc), TRUE);
	}
	catch (CException* e)
	{
		CString serr;
		auto ek = dynamic_cast<KException*>(e);
		if (ek)
			serr = ek->m_strError;
		else
			e->GetErrorMessage(serr.GetBuffer(1024), 1024); serr.ReleaseBuffer();
		CString smsg; smsg.Format(L"Error: %s.", serr);
		CaptionMessage(smsg);
		KwMessageBoxError(smsg);
	}

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
			CString s; s.Format(L"Step 1. Error (%d) while installing DB [%s]. Dir:%s, CurDir:%s", (int)(__int64)hi, fname, fl, curDir);
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
			CString s; s.Format(L"Step 2. Error (%d) while installing ODBC [%s].  Dir:%s", (int)(__int64)hi, fname, fl);
			CaptionMessage(s);
		}
	}
	else
		KwMessageBoxError(L"Step 2. File (%s) is not found.", full);
}

//?deprecated
void CMainFrame::OnBuildProject()
{
	auto cvu = dynamic_cast<CmnView*>(GetActiveCmnView());
	if(cvu)
	{
		auto doc = cvu->GetDocument();
		auto res = doc->_svr;
		auto api = res->_api;
		auto dll = api->getExLibName();
		CString s;s.Format(L"Step 3. You can now build the modified code to overwrite the DLL file [%s].", dll);
		CaptionMessage(s);
		KwMessageBox(s);
	}
}













void CMainFrame::OnDownloadMariaODBC()
{
	//CString fl = L".\\MariaDB Connector-ODBC - MariaDB.url";
	PWS url = L"https://downloads.mariadb.org/connector-odbc/";
	if (1)//KwIfFileExist(fl))
	{
		::ShellExecute(0, 0, url, 0, 0, SW_SHOW);
		CaptionMessage(L"Navigating to download site for MariaDB ODBC driver!");
	}
	//else	KwMessageBox(L"File \"%s\" is not found.", fl);

	// TODO: Add your command handler code here
}

void CMainFrame::OnDownloadMariaDB()
{
	//CString fl = L".\\Downloads - MariaDB.url";
	// 	ShellExecuteW(_In_opt_ HWND hwnd, _In_opt_ LPCWSTR lpOperation, _In_ LPCWSTR lpFile, _In_opt_ LPCWSTR lpParameters,
	// 		_In_opt_ LPCWSTR lpDirectory, _In_ INT nShowCmd);
	PWS url = L"https://downloads.mariadb.org/";
	if (1)//KwIfFileExist(fl))
	{
		::ShellExecute(0, 0, url, 0, 0, SW_SHOW);
		CaptionMessage(L"Navigating to download site for MariaDB!");
		auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_doc;
		appd._statDB = "installDB";
		appd.SaveData();
	}
	//else		KwMessageBox(L"File \"%s\" is not found.", fl);
}



void CMainFrame::OnVisualStudioDownload()
{
	TCHAR curDir[1002];
	GetCurrentDirectory(1000, curDir);//C:\Users\dwkang\AppData\Roaming\Outbin\KHttpsSrv\x64\Debug

	WCHAR appdata[MAX_PATH];//CSIDL_PERSONAL CSIDL_PROGRAM_FILES
	HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appdata);

	PWS fname = L"vs_community__1141927769.1601659846.exe";// L"Visual Studio Download.url";
	CString fl; fl.Format(L"%s\\Keepspeed\\Winpache\\", appdata);
	CString full = fl + fname;
	CString url = L"https://visualstudio.microsoft.com/ko/thank-you-downloading-visual-studio/?sku=Community&rel=16";
	if (KwIfFileExist(full))
	{
		::ShellExecute(0, 0, full, 0, 0, SW_SHOW);
		CaptionMessage(L"Step 1. Install the Visual Studio Community!");
	}
	else
	{
		CString smsg; smsg.Format(L"Step 1. Error occurred.\nYou can download and install Visual Studio from the site.\n % s", full);
		CaptionMessage(smsg);
		KwMessageBoxError(smsg);
		::ShellExecute(0, 0, url, 0, 0, SW_SHOW);
	}
}



void CMainFrame::OnDownloadOpenSSL()
{
	CString fl = L"https://sourceforge.net/projects/openssl/";
	// 	ShellExecuteW(_In_opt_ HWND hwnd, _In_opt_ LPCWSTR lpOperation, _In_ LPCWSTR lpFile, _In_opt_ LPCWSTR lpParameters,
	// 		_In_opt_ LPCWSTR lpDirectory, _In_ INT nShowCmd);
	if (1)//KwIfFileExist(fl))
	{
		::ShellExecute(0, 0, fl, 0, 0, SW_SHOW);
		CaptionMessage(L"Navigating to download site for MariaDB!");
	}
	else
		KwMessageBox(L"URL \"%s\" is not found.", fl);
}


void CMainFrame::OnDockOdbcSetting()
{
	DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc), TRUE);
}




void CMainFrame::OnGotoProject()
{
	PWS tdir = L"\\Winpache\\src\\KHttpsSrv\\";
// 	GotoTheDirectory(tdir);
// 	WCHAR my_documents[MAX_PATH];//CSIDL_PERSONAL
// 	HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, my_documents);
// 	CStringW flPrj = my_documents;
// 	PWS tdir = L"\\Winpache\\src\\KHttpsSrv\\";// KHttpsSrvSite.sln";
// 	//PWS fsln = L"KHttpsSrvSite.sln";
// 	flPrj += tdir;
// 	CString sDir = flPrj;
// 	//flPrj += fsln;
// 	HINSTANCE hi = ::ShellExecute(0, 0, flPrj, 0, sDir, SW_SHOW);
// 	//ERROR_FILE_NOT_FOUND
// 	if ((LONGLONG)hi > 32L)
// 	{
// 		CString s; s.Format(L"Open the Explorer navigating [%s] for a Site Project.", flPrj);
// 		CaptionMessage(s);
// 	}
// 	else
// 	{
// 		KwMessageBoxError(L"Error occurred!\n\While opening the directory [%s].", sDir);
// 	}
}


void CMainFrame::OnGotoOutput()
{
	PWS tdir = L"\\Winpache\\bin\\";
	GotoTheDirectory(tdir);
// 	WCHAR my_documents[MAX_PATH];//CSIDL_PERSONAL
// 	HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, my_documents);
// 	CStringW flPrj = my_documents;
// 	PWS tdir = L"\\Winpache\\bin\\";
// 	flPrj += tdir;
// 	CString sDir = flPrj;
// 	HINSTANCE hi = ::ShellExecute(0, 0, flPrj, 0, sDir, SW_SHOW);
// 	if ((LONGLONG)hi > 32L)
// 	{
// 		CString s; s.Format(L"Open the Explorer navigating [%s] for a Site Project.", flPrj);
// 		CaptionMessage(s);
// 	}
// 	else
// 	{
// 		KwMessageBoxError(L"Error occurred!\n\While opening the directory [%s].", sDir);
// 	}
}

void CMainFrame::GotoTheDirectory(PWS tdir)
{
	WCHAR my_documents[MAX_PATH];//CSIDL_PERSONAL
	HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, my_documents);
	CStringW flPrj = my_documents;
	//PWS tdir = L"\\Winpache\\bin\\";
	flPrj += tdir;
	CString sDir = flPrj;
	HINSTANCE hi = ::ShellExecute(0, 0, flPrj, 0, sDir, SW_SHOW);
	if ((LONGLONG)hi > 32L)
	{
		CString s; s.Format(L"Open the Explorer navigating [%s] for a Site Project.", flPrj);
		CaptionMessage(s);
	}
	else
	{
		KwMessageBoxError(L"Error occurred!\nWhile opening the directory [%s].", sDir);
	}
}


void CMainFrame::OnRunHeidiSQL()
{
	WCHAR progrm[MAX_PATH];//CSIDL_PERSONAL CSIDL_PROGRAM_FILES
	HRESULT result = SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, progrm);
	PWS fname = L"heidisql.exe";
	CString fl; fl.Format(L"%s\\HeidiSQL\\%s", progrm, fname);
	if (KwIfFileExist(fl))
	{
		HINSTANCE hi = ::ShellExecute(0, 0, fl, 0, 0, SW_SHOW);
		if ((LONGLONG)hi > 32L)
		{
			CString s; s.Format(L"DB tool [%s] is run.", fname);
			CaptionMessage(s);
		}
		else
		{
			CString s; s.Format(L"Error while launcching the DB tool [%s].", fname);
			CaptionMessage(s);
		}
	}
	else
		KwMessageBoxError(L"File (%s) is not found.", fl);

}


void CMainFrame::OnCopyRequest()
{
	auto cvu = GetActiveCmnView();
	if (cvu)
		cvu->CopyRequest();
}


void CMainFrame::OnCopyOutput()
{
	auto cvu = GetActiveCmnView();
	if (cvu)
		cvu->CopyOutput();
}
