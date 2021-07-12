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
#include "mysql.h"
#include "KwLib64/Lock.h"

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

	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	auto& jobj = *appd._json;
	BOOL bFirst = jobj.I("LoadCount") == 0;





	int idOw = COutputWnd::s_me->_id;
	AddCallbackExtraTrace([&, this, idOw](string txt) -> void //?ExTrace 1 ���� ��ƾ�� ���� �Ѵ�.����
		{
			KwBeginInvoke(this, ([&, txt]()-> void {
				//wstring wstr = ToWStr(txt); �̰� �ѱ��� ����� ��ȯ�� �ȵȴ�.
				ExtraTrace(txt, idOw);
				}));
		});


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
	m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, !bFirst, &pTabbedBar);
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



	if(bFirst)
	{
		DockClientBase::ShowHide(this, m_wndProperties, FALSE);
		DockClientBase::ShowHide(this, m_wndClassView, FALSE);
		DockClientBase::ShowHide(this, m_wndFileView, FALSE);
	}
	ConnectMainDB();

	///?warning ���⼭ KwBeginInvoke�ϸ� View::OnInitialUpdate ���� �ʰ� ���� �ȴ�.
	KwBeginInvoke(this, ([&]()-> void {
			CheckRecoverServers();//?server recover 3 �������üũ
		}));
	
	/// ���� ���ִ� ������ ã�Ƽ�, ������ üũ �ð��� update �Ѵ�.
	SetLambdaTimer("Pending Server Check", 5000, [&](int ntm, PAS tmk)//?server recover time 
		{
			TimerCheckServers();//CMainFrame::OnCreate::<lambda_4b040ea4cfd81847d7d539b3ca5fbc01>::operator ()
			//KTrace(L"%d. %s (%s)\n", ntm, __FUNCTIONW__, L"Lambda timer test");
		});

	return 0;
}
void CMainFrame::ExtraTrace(string txt, int idOw)
{
	auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());//BG���� �ҷ� ���� ���� ����.
//bool bVu = !ivc ? false : ivc->ViewFind(idVu);
	bool bOutput = !ivc ? false : ivc->ViewFind(idOw);
	if(bOutput)// && bVu)//?destroy 7 : ���⼭ NULL�� ���;� �ϴµ�, 0xddddddddddd
	{
		COutputWnd::s_me->TraceQueue(txt);//?ExTrace 7
		COutputWnd::s_me->TraceFlush();//?ExTrace 7 //?destroy 8
	}
}


/// �ð��� ���� �����, �ֱٱ��� ���ϰ� �־��ٴ°� �ȴ�.
/// ���߿� �����Ҷ� �ֱٱ��� ���� �־��� ������ �����Ѵ�.
/// ���� ���ִ� ������ ã�Ƽ�, ������ üũ �ð��� updaate �Ѵ�.
void CMainFrame::TimerCheckServers()
{
	// TODO: 1. ���� ä���
	// 	  2. CheckRecoverServers ���� �ð� ������ 10�� �̳� ���� ����� ���� ������.
	auto app = (CMFCExHttpsSrvApp*)AfxGetApp();
	auto& appd = (app)->_docApp;
	AUTOLOCK(appd._csRecover);
	auto& jobj = *appd._json;

	auto srsv = jobj.O("RunningServers");
	if(!srsv || srsv->size() == 0)
		return;

	for(auto& [kguid, sjval] : *srsv)
	{
		auto sjsvr = sjval->AsObject();
		(*sjsvr)("_tLastRunning") = KwGetCurrentTimeFullString();//?server recover time 
	}
	appd.SaveData();
}
void CMainFrame::CheckRecoverServers()
{

	auto app = (CMFCExHttpsSrvApp*)AfxGetApp();
	auto& appd = (app)->_docApp;
	AUTOLOCK(appd._csRecover);

	auto& jobj = *appd._json;
	auto srsv = jobj.O("RunningServers");
	if(!srsv)
		return;
	CTime now = KwGetCurrentTime();
	CTimeSpan sp10(0, 0, 10, 0);
	CTime now10 = now - sp10;

	KList<wstring> svrToStop;
	for(auto& [kguid, sjval] : *srsv)
	{
		auto sjsvr = sjval->AsObject();
		auto full = sjsvr->S("_fullPath");
		auto guid = sjsvr->S("_GUID");
		auto stLast = sjsvr->S("_tLastRunning");
		if(tchlen(stLast) > 10)//�ð����ڿ� �ּұ���
		{
			CTime tLast = KwCStringToCTime(stLast);
			if(now10 > tLast)//?server recover time  10������ ���� �͸� recover�Ѵ�.
			{
				svrToStop.push_back(guid);//����������� ���� ����. �� �Ʒ����� ����.
				continue;
			}
		}
		//������ ���� ���´�.
		//  	CWinApp::OpenDocumentFile(const wchar_t * lpszFileName) Line 86	C++
		if(tchlen(full) > 0)
			app->OpenDocumentFile(full);//?server recover 3.1 ��������� ������ ����: ������ ������ ����
		else
		{
			appd.PushRecoveringServer(guid);//?server recover 3.2
			app->NewFile();///_GUID�� Ǫ���� �ξ��ٰ�, SrvDoc::OnNewDocument() ���� pop�Ͽ� GUID�� �����Ѵ�.
		}
// 			app->Create CreateNewDocument();
		// open server file
		// conect to site DB
		// start server
		//_tLastRunning; //������ ���� ���� üũ Ÿ���̴�, 5�о����� ������ ��쿡�� ���� �Ѵ�.
		// Ÿ�̸ӵ� ������, ���� ���� ���� _tLastRunning �� ���� �Ѵ�.
	}
	if(svrToStop.size() > 0)
	{
		for(auto svr : svrToStop)//?server recover time // ������ ����Ÿ�� üũ�� 10���� ���� �Ŵ� �ٽ� ���� ���ϰ�, ����
			srsv->DeleteKey(svr.c_str());
		appd.SaveData();
	}
}
void CMainFrame::ConnectMainDB()
{
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	auto& jobj = *appd._json;

	int cntLoad = jobj.I("LoadCount") + 1;
	jobj("LoadCount") = cntLoad;
	auto pappd = &appd;//app��ü���� ���� �ϴ�, ������ �׳� ���� �ص� �ȴ�.
	KAtEnd _sv([&, pappd]() {
		pappd->SaveData();
		});
	KWStrMap kmap1;
	CString DSN2, UID2, PWD2, database2;
	bool bRetry = false;
	bool bOkODBC = false;
	while(1)
	{
		try
		{
			int rv = 0;
			if(kmap1.size() == 0) // loop �̹Ƿ� ó���� ������ ��� ����.
			{
				/// first loop while
				rv = KDatabase::RegGetODBCMySQL(jobj.S("_DSN"), kmap1);

				auto Driver0 = kmap1.Get(L"ODBC Data Sources");
				auto Driver1 = kmap1.Get(L"Driver");
				if(Driver0.length() == 0 || Driver1.length() == 0 || Driver0 != Driver1)
				{
					CString smsg(L"Main DB is not initialized yet.\r\nYou must proceed to step 5 in the Database menu section.");
					CaptionMessage(smsg);
					CMFCRibbonCategory* pmrc = m_wndRibbonBar.GetCategory(2);
					if(pmrc)
					{
						m_wndRibbonBar.ShowCategory(2, TRUE);
						m_wndRibbonBar.SetActiveCategory(pmrc, TRUE);
					}
					break;
				}
				else
					bOkODBC = Driver0.length() > 0 && Driver0 == Driver1;
		
				/// cfg���� �������� �ϵ�, ������ registry ODBC���� �о� �´�.
				if(DSN2.GetLength() == 0)
				{
					DSN2 = jobj.S("_DSN");
					if(!jobj.LenS("_UID", UID2))
						UID2 = kmap1.Get(L"UID").c_str();
					if(!jobj.LenS("_PWD", PWD2))
						PWD2 = kmap1.Get(L"PWD").c_str();
					if(!jobj.LenS("_database", database2))
						database2 = kmap1.Get(L"database").c_str();
				}
			}
			
			if(bOkODBC)				//&& UID1.length() > 0)// && PWD1.length() > 0)// && uid1 == uid0)
			{
				/// UID2, PWD2�� �Ʒ� DlgOdbcSetting ���� �ٽ� ������ �ִ�. while loop �ι���
				CString dsn;
				dsn.Format(L"DSN=%s;UID=%s;PWD=%s;database=%s", DSN2, UID2, PWD2, database2);//PWD�� �ִ°� ODBC�� Ȯ�� ������, DSN������ �α��� �õ�
				
				/// ////////////////////////////////////////////////////////////////////////////////
				BOOL bOK = appd._dbMain->OpenEx(dsn, CDatabase::noOdbcDialog);
				/// ////////////////////////////////////////////////////////////////////////////////
				// ������ ���� �� �� 0�� �ƴ� ������ ���� �˴ϴ�. �׷��� ������ ����ڰ� �߰� ���� ������ �䱸 �ϴ� 
				// ��ȭ ���ڰ� ǥ�� �� �� ��Ҹ� ���� �ϴ� ��� 0�Դϴ�. 
				// �ٸ� ��� ��쿡�� ������ ��ũ���� ���ܸ� throw �մϴ�.
				if(bOK)
				{
// 					try
// 					{
						///appd._dbMain->ExecuteSQL(L"use `winpache`"); OpenEx() ���� database����
						jobj("StatDB") = L"login";
						CString smsg; smsg.Format(L"Main Dababase is connected! (DSN: %s).", DSN2);
						CaptionMessage(smsg);
// 					}
// 					catch(CDBException* e)
// 					{
// 						CString smsg; smsg.Format(L"Error! Database `winpache` is not created yet.\n%s, %s.", e->m_strError, e->m_strStateNativeOrigin);
// 						TRACE(L"%s\n", smsg);
// 					}
					
					jobj("_DSN") = DSN2;
					jobj("_UID") = UID2;
					jobj("_PWD") = PWD2;
					DockOdbc::s_me->_DSN = DSN2;
					DockOdbc::s_me->_UID = UID2;
					KwBeginInvokeNt(this, ([&]()-> void
						{
							DockOdbc::s_me->UpdateUI();
						}), "to DockOdbc");
				///	jobj("_database") = database2; �ʱⰪ�� ���� �ϴµ��� ����.
					// ���� statDB�� ODBC �ε�, ���� ��� ������ ��ŷâ ODBC�� ����.
					//DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc), TRUE);
				}
				else
				{
					CString smsg; smsg.Format(L"Error! Database `winpache` is not created yet.");
					KwMessageBoxError(smsg);
					break;
				}
			}
			bRetry = false;
			KwBeginInvoke(this, ([&]()-> void
				{
					auto app = (CMFCExHttpsSrvApp*)AfxGetApp();
					app->NewFile();//?PreventViewFirst 
				}));
			break;
		}
		catch(CDBException* e)
		{
			//Access denied for user 'root'@'localhost' (using password: YES)
			//State:28000, Native : 1045, Origin : [ma - 3.1.12]
			PWS Access_denied = L"State:28000";
			if(tchstr((PWS)e->m_strStateNativeOrigin, Access_denied))
			{
				//KwMessageBoxError(L"While connecting to main DB.\n%s.\nRetry on 'Connect to DB' docking window'.", e->m_strError);
				//DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc), TRUE);
			}
			CString smsg; smsg.Format(L"Error! %s, %s.", e->m_strError, e->m_strStateNativeOrigin);
			CaptionMessage(smsg);
			//m_strError	L"Access denied for user 'winpache'@'localhost' (using password: YES)\n"
			if(e->m_strError.Find(L"Access denied for user") >= 0)
				KwMessageBoxError(smsg);
			bRetry = true;
		}
		catch(CException* e)
		{
			CString serr;
			auto ek = dynamic_cast<KException*>(e);
			if(ek)
				serr = ek->m_strError;
			else
				e->GetErrorMessage(serr.GetBuffer(1024), 1024); serr.ReleaseBuffer();
			CString smsg; smsg.Format(L"Error: %s.", serr);
			CaptionMessage(smsg);
			bRetry = false;
			break;
		}

		/// ��� �������� �ް� �ٽ� �õ�
		if(bRetry)
		{
			DlgOdbcSetting dlg;
			dlg._DSN = DSN2;
			dlg._UID = UID2;
			dlg._PWD = PWD2;
			if(dlg.DoModal() != IDOK)
			{
				CString s(L"Server errors and logs are recorded only when connecting to the main database.");
				CaptionMessage(s);
				KwMessageBoxError(s);
				break;// while break
			}
			else
			{
				DSN2 = dlg._DSN;					// ^
				UID2 = dlg._UID;					// |
				PWD2 = dlg._PWD;					// |
				continue;// while loop				// .
			}
		}
	}//while(1)
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
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	auto& jobj = *appd._json;
	bool bFirst = jobj.I("LoadCount") == 0;

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
	//DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc), !bFirst);

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

void CMainFrame::UpdateControl(CStringA stat, int iOp)
{
	KwBeginInvoke(this, ([&, stat, iOp]()-> void
		{
			m_wndRibbonBar.ForceRecalcLayout();
		}));
}


void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}
void CMainFrame::CaptionMessage(PWS msg)
{
	FOREGROUND();
	if(!::IsWindow(GetSafeHwnd()))
		return;//�ʹ� ���� �Ҹ��� ���
	wstring smsg = msg;
	KwBeginInvoke(this, ([&, smsg]()-> void
		{
			if(!::IsWindow(m_wndCaptionBar.GetSafeHwnd()))
				return;//�ʹ� ���� �Ҹ��� ���
			CString sw(smsg.c_str());
			m_wndCaptionBar.SetText(sw, CMFCCaptionBar::ALIGN_LEFT);
			m_wndCaptionBar.ShowWindow(SW_SHOW);
			RecalcLayout(FALSE);
			CStringA sa(sw);
			COutputWnd::s_me->Trace((PAS)sa);
		}));

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


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndExInvokable::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
}


void CMainFrame::OnClose()
{
	// template�� �ϳ�.
	for (POSITION posTemplate = AfxGetApp()->GetFirstDocTemplatePosition(); posTemplate; )
	{
		/// doc�� ���� �ִ� ���� ����
		auto pDocTemplate = AfxGetApp()->GetNextDocTemplate(posTemplate);
		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		while (posDoc)
		{
			CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);

			CmnDoc* doc = dynamic_cast<CmnDoc*>(pDoc);
// 			auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
// 			appd.UnregisterServerStart(doc->_GUID);

			if(doc->IsStarted())
				doc->ShutdownServer();

			/*/// view�� �� ������ View�� ������ �ΰ�� doc ���� �̰� view�� �Լ��� �����Ƿ� ������ view�� �ϴ� �� �ѹ��� ���� �Ѵ�.
			POSITION posView = pDoc->GetFirstViewPosition();
			while (posView)
			{
				CView* pView = pDoc->GetNextView(posView);
				auto cvu = dynamic_cast<CmnView*>(pView);
				if (cvu) // ���⼭ 
					cvu->Shutdown("CMainFrame::OnClose()");//CChildFrame::OnClose() ������ �ҷ��� �Ѵ�.
				//pView->GetParentFrame()->DestroyWindow();
				break;//���� doc�ϳ� �� single view
			}*/
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
				cvu->Shutdown("CMainFrame::OnClose()");//?Shutdown CChildFrame::OnClose() ������ �ҷ��� �Ѵ�.
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
		BOOL b = cvu->GetUpdate(idc);//CChildFrame::OnClose() ������ �ҷ��� �Ѵ�.
		pCmdUI->Enable(b);
	}
}


void CMainFrame::OnRibbonStart()
{
	auto cvu = GetActiveCmnView();
	if(cvu)
		cvu->OnBnClickedStart();//CChildFrame::OnClose() ������ �ҷ��� �Ѵ�.
}
void CMainFrame::OnUpdateStart(CCmdUI* pCmdUI)
{
	OnUpdateCmn(pCmdUI, ID_Start);
}
void CMainFrame::OnRibbonStop()
{
	auto cvu = GetActiveCmnView();
	if(cvu)
		cvu->OnBnClickedStop();
}
void CMainFrame::OnUpdateStop(CCmdUI* pCmdUI)
{
	OnUpdateCmn(pCmdUI, ID_Stop);
}
void CMainFrame::OnRibbonRestart()
{
	auto cvu = GetActiveCmnView();
	if(cvu)
		cvu->OnBnClickedRestart();
}
void CMainFrame::OnUpdateRestart(CCmdUI* pCmdUI)
{
	OnUpdateCmn(pCmdUI, ID_Restart);
}
void CMainFrame::OnRibbonStartDB()
{
	auto cvu = GetActiveCmnView();
	if(cvu)
		cvu->OnBnClickedStartDB();
}
void CMainFrame::OnUpdateStartDB(CCmdUI* pCmdUI)
{
	OnUpdateCmn(pCmdUI, ID_StartDB);
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
	::GetSystemDirectory(my_documents, MAX_PATH);//���� ����� ����.
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
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	auto& jobj = *appd._json;

	jobj("StatDB") = "ODBC";
	appd.SaveData();

	CaptionMessage(L"Step 3. ODBC has been set up on the newly installed MariaDB..");
}
void CMainFrame::OnCreateDatabase()
{
	InitOdbc(4);
// 	DockClientBase::ShowHide(this, *Pane(IDD_DockTestApi), TRUE);
// 	DockTestApi::s_me->CreateDatabase();
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	auto& jobj = *appd._json;

	jobj("StatDB") = "database";
	appd.SaveData();
	CaptionMessage(L"Step 4. A database 'winpache' for logs and samples has been created.");
}



void CMainFrame::OnCreateTableBasic()
{
	InitOdbc(5);
	CaptionMessage(L"Step 5. Main tables have been created.");
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	auto& jobj = *appd._json;
	
	jobj("StatDB") = "table";
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
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	auto& jobj = *appd._json;

	try
	{
		PWS dsn0 = L"Winpache";
		PWS uid0 = L"root";//��ó������ �ʿ�

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

		/// 1. no pwd�� �α��� �õ� �غ���, �Ǹ� 
		if(step == 3)// Step 3. init ODBC
		{
			/// 1�ܰ迡�� MariaDB�� ��ġ�Ҷ�, root ����� ������ ����� ��� �Ѵ�.
			/// 3�ܰ迡�� ODBC�� ����� ���� ���, OpenEx�ϸ� ODBC����â�� �߸鼭 ����� ������ ������ �ȵȴ�.
			///		�׷��� �� ���� Dlg�� ��� �޾Ƽ� ;PWD=%s �� �־� OpenEx�ϸ� ������ ODBC����â�� �ȶ��.
			KWStrMap kmap1;
			ASSERT(tchsame(dsn0, jobj.S("_DSN")));
			int rv = KDatabase::RegGetODBCMySQL(jobj.S("_DSN"), kmap1);
// +		[L"DSN"]	L"MariaDB ODBC 3.1 Driver"
// +		[L"Driver"]	L"MariaDB ODBC 3.1 Driver"
// +		[L"PORT"]	L"3306"
// +		[L"PWD"]	L"bnmnnm,"
// +		[L"SERVER"]	L"localhost"
// +		[L"TCPIP"]	L"1"
// +		[L"UID"]	L"root"
			auto Driver0 = kmap1.Get(L"ODBC Data Sources");
			auto Driver1 = kmap1.Get(L"Driver");
			auto UID1 = kmap1.Get(L"UID");
			auto PWD1 = kmap1.Get(L"PWD");
			if(kmap1.size() > 0 && Driver0.length() > 0 && Driver0 == Driver1)// && uid1 == uid0)
			{//�̹� �ִ�.
				/// 1. PWD�� ���� ��� ODBC Settingâ�� �߰� ����� �Է� �ϰ� �� �� ���� �ȴ�. ������ ���⼭ �Է��� ��� ����� ���� ������ �ʴ´�.
				/// 2. PWD�� Ʋ�� ��� CDBException�̳��� ����� Ʋ�� ���� �ڽ� �Ʒ����� ���. ODBC Docking â�� ���� ����� �Է� �ް� �ٽ� ���� �ϵ��� �Ѵ�. ���� �Է��� PWD�� ������� �ʴ´�.
				jobj("_DSN") = dsn0;// DSN1.c_str();// L"Winpache";
				jobj("_UID") = UID1.c_str();
				//if (PWD1.length() > 0) jobj("_PWD") = L"**********";
				jobj("_PWD") = PWD1.c_str();
				jobj("_SERVER") = kmap1.Get(L"SERVER").c_str();

#ifdef _DEBUGx
				if (_PWD.GetLength())
					DockOdbc::s_me->_PWD = _PWD;
#endif // _DEBUGx
				CString UID2 = jobj.S("_UID");
				CString PWD2 = jobj.S("_PWD");
				if(PWD2.GetLength() > 0)//jobj.Len("_UID"))
				{
					DlgOdbcSetting dlg;
					dlg._DSN = jobj.S("_DSN");//readable
					dlg._UID = UID2;  // editable
					dlg._PWD = L"";   // editable
					if(dlg.DoModal() == IDOK)
					{
						UID2 = dlg._UID;
						PWD2 = dlg._PWD;
					}
				}

				DockOdbc::s_me->UpdateData(0); /// ODBC Settingâ�� �ѷ� �ش�.

				CString dsn; 
				if (UID2.GetLength() > 0)//jobj.Len("_UID"))
				{
					if (PWD2.GetLength() == 0)
						dsn.Format(L"DSN=%s;UID=%s", jobj.S("_DSN"), UID2);//�̰Ŵ� ��� �ʼ� �׸��� �� �� �ְ�.
					else
						dsn.Format(L"DSN=%s;UID=%s;PWD=%s", jobj.S("_DSN"), UID2, PWD2);// jobj.S("_PWD"));//�̰Ŵ� ��� �ʼ� �׸��� �� �� �ְ�.
				}
				else
					dsn.Format(L"DSN=%s", jobj.S("_DSN"));//�̰Ŵ� ��� �ʼ� �׸��� �� �� �ְ�.
				if (appd._dbMain->IsOpen())
					appd._dbMain->Close();
				appd._dbMain->OpenEx(dsn);//_T("DSN=UserInfo")); ���⼭ ��� ���� ����.
				/// ���� database �ȸ�������� ������. appd._dbMain->ExecuteSQL(L"use `winpache`");
				
				// ���� ����. ����.
				jobj("_UID") = UID2;
				jobj("_PWD") = PWD2;
				DockOdbc::s_me->_DSN = jobj.S("_DSN");//ODBC ��ŷâ����
				DockOdbc::s_me->_UID = UID2;

				auto smsg = L"Server default (Winpache) ODBC is already initialized. Now connected.";
				CaptionMessage(smsg);
				KwMessageBox(smsg);
				///DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc), TRUE);
			}
			else
			{
				jobj("_DSN") = dsn0;// L"Winpache";
				jobj("_UID") = uid0;
				KWStrMap kmap;
				kmap[L"UID"] = (PWS)jobj.S("_UID");
				/// 1.1 ODBC no pwd�� ó�� �����
				int rv = KDatabase::RegODBCMySQL(jobj.S("_DSN"), kmap);
				
				DockOdbc::s_me->_DSN = jobj.S("_DSN");
				DockOdbc::s_me->_UID = jobj.S("_UID");
				DockOdbc::s_me->UpdateData(0); /// ODBC Settingâ�� �ѷ� �ش�.
				///DockClientBase::ShowHide(this, *Pane(IDD_DockOdbc), TRUE);
				
				/// 1.2 ���� �Ѵ�.
				CString dsn; dsn.Format(L"DSN=%s;UID=%s", jobj.S("_DSN"), jobj.S("_UID"));//�̰Ŵ� ��� �ʼ� �׸��� �� �� �ְ�.
				appd._dbMain->OpenEx(dsn);//_T("DSN=UserInfo"));
				//���� database�� �ȸ������. appd._dbMain->ExecuteSQL(L"use `winpache`");

				CaptionMessage(L"ODBC connected without password as root user to the installed DB server.");
				KwMessageBox(L"Connected.\nYou can save the password in ODBC Setting.");
			}
			appd.SaveData();//running���� ������ ������ PWD�� ���� ���Ѵ�.
		}
		else if (step == 4)
		{
			/// 3. ù��° database�� winpache�� �����.
			appd._dbMain->ExecuteSQL(L"CREATE DATABASE `winpache` COLLATE 'utf16_unicode_ci'");
			appd._dbMain->ExecuteSQL(L"use `winpache`");
			KwMessageBox(L"Database `winpache` has been created.");
		}
		else if (step == 5 || step == 6)
		{
			WCHAR my_documents[MAX_PATH];//CSIDL_PERSONAL
			HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, my_documents);
			CStringW flPrj = my_documents;
			PWS tdir = L"\\Winpache\\DB\\";


			appd._dbMain->ExecuteSQL(L"use `winpache`");
			flPrj += tdir;
			CString sDir = flPrj;


			try
			{
				appd._dbMain->CreateTablesInFolderLD(flPrj, [&](CString fn, CString sql, CDBException* e) -> void
					{
						//tbiz_af_ins.sql: 
						///This command is not supported in the prepared statement protocol yet
						///State:S1000,Native:1295,Origin:[ma-3.1.12][10.5.10-MariaDB]
						if(e->m_strError.Find(L"command is not supported") >= 0 &&
							e->m_strStateNativeOrigin.Find(L"Native:1295") >= 0)
						{
							/// CREATE TRIGGER�ϸ� ODBC���� ���� ���� �ʴ´�. MariaDB lib�� ���� �̿� �Ѵ�.
							try
							{
								MYSQL* con = mysql_init(NULL);
// 								auto server = jobj.SA("_SERVER");
// 								auto server = jobj.SA("_DSN");
// 								auto server = jobj.SA("_SERVER");
// 								auto server = jobj.SA("_SERVER");
								if(mysql_real_connect(con, jobj.SA("_SERVER"), jobj.SA("_UID"), jobj.SA("_PWD"),
														jobj.SA("_database"), 0, NULL, 0) == NULL)
									throw_str(mysql_error(con));
								//if(mysql_query(con, "DROP TABLE IF EXISTS writers"))
								//if(mysql_query(con, "CREATE TABLE writers(id INT PRIMARY KEY AUTO_INCREMENT, name VARCHAR(255))"))
								CStringA sqlA(sql);
								if(mysql_query(con, (PAS)sqlA))
									throw_str(mysql_error(con));
								//"CREATE DEFINER=`root`@`localhost` TRIGGER `test_trigger` BEFORE INSERT ON `writers` 
								//FOR EACH ROW BEGIN insert into writerslog(fname) values(NEW.`name`); END"))
								//if(mysql_query(con, "INSERT INTO writers(name) VALUES('Leo Tolstoy')"))
								//__int64 id = mysql_insert_id(con);
								mysql_close(con);
							}
							catch(KException* e)
							{
								if(e->m_strStateNativeOrigin.Find(L"already exists") >= 0)
									throw e;
								CString s; s.Format(L"KException:%s - %s\n", e->m_strError, e->m_strStateNativeOrigin);
								TRACE(L"%s\n", s);
							}
							catch(CException* )
							{
								TRACE("CException\n");
							}
						}
						else
						{
							CString smsg; smsg.Format(L"Error! %s: %s, %s.", fn, e->m_strError, e->m_strStateNativeOrigin);
							CaptionMessage(smsg);
						}
// 						if(e->m_strStateNativeOrigin.Find(L"S0001") < 0)//�̹� ���� �ϴ� ���� �ڽ� �ȶ���.
// 							KwMessageBoxError(smsg);
					});
			}
			catch(KException* e)
			{
				CString s; s.Format(L"KException:%s - %s\n", e->m_strError, e->m_strStateNativeOrigin);
				TRACE(L"%s\n", s);
			}
// 			catch(CDBException* e)
// 			{
// 				CString smsg; smsg.Format(L"Error! %s, %s.", e->m_strError, e->m_strStateNativeOrigin);
// 				CaptionMessage(smsg);
// 				if(e->m_strStateNativeOrigin.Find(L"S0001") < 0)//�̹� ���� �ϴ� ���� �ڽ� �ȶ���.
// 					KwMessageBoxError(smsg);
// 			}
			catch(CException* )
			{
				TRACE("CException \n");
			}
			/*
			PWS arfsln[] = { 
				L"CREATE_TABLE_t_reqlog.sql",
				L"CREATE_TABLE_t_ldblog.sql",
				L"CREATE_TABLE_t_excepsvr.sql",
				/// �Ʒ� �� ���̺��� sample ���̺� �̴�.
 				L"CREATE_TABLE_tbiz.sql",
 				L"CREATE_TABLE_tbizclass.sql", 
			};
			for (auto fsql : arfsln)
			{
				CString fullw = flPrj + fsql;
				CStringA full(fullw);
				try
				{
					KDatabase::CreateTable(*appd._dbMain, full);
				}
				catch(CDBException* e)
				{
					CString smsg; smsg.Format(L"Error! %s, %s.", e->m_strError, e->m_strStateNativeOrigin);
					CaptionMessage(smsg);
					if(e->m_strStateNativeOrigin.Find(L"S0001") < 0)//�̹� ���� �ϴ� ���� �ڽ� �ȶ���.
						KwMessageBoxError(smsg);
				}
			}*/
			/// 4. ù��° log table�� �����.
// 			KDatabase::CreateTable(*appd._dbMain, "..\\..\\DB\\CREATE_TABLE_t_reqlog.sql");
// 			KDatabase::CreateTable(*appd._dbMain, "..\\..\\DB\\CREATE_TABLE_t_excepsvr.sql");
// 			KDatabase::CreateTable(*appd._dbMain, "..\\..\\DB\\CREATE_TABLE_tbiz.sql");
// 			KDatabase::CreateTable(*appd._dbMain, "..\\..\\DB\\CREATE_TABLE_tbizclass.sql");
			KwMessageBox(L"Basic tables have been created.");
		}
		else if (step == 6)
		{
// 			appd._dbMain->ExecuteSQL(L"use `winpache`");
// 			/// 4. ù��° log table�� �����.
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
		
		auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
		auto& jobj = *appd._json;
		
		jobj("StatDB") = "installDB";
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
	PWS fname = L"heidisql.exe";// C:\Program Files (x86)\Common Files\MariaDBShared\HeidiSQL
	CString fl; 

	// env ProgramFiles(x86) = C:\Program Files(x86)
	WCHAR pf86[MAX_PATH];
	//ExpandEnvironmentStrings(L"%ProgramW6432%", pf86, ARRAYSIZE(pf86));//Program Files
	ExpandEnvironmentStrings(L"%ProgramFiles(x86)%", pf86, ARRAYSIZE(pf86));//"%Program Files (x86)%"
// 	CString s; GetEnvironmentVariableW(L"ProgramFiles(x86)", (LPWSTR)s.GetBuffer(1000), 1000);	s.ReleaseBuffer();
	fl.Format(L"%s\\Common Files\\MariaDBShared\\HeidiSQL\\%s", pf86, fname);///�̰� MariaDB�� ����� �򸮴°�

	if (!KwIfFileExist(fl))
		fl.Format(L"%s\\HeidiSQL\\%s", progrm, fname);///�̰� HeidiSQL64 ���� ���� ���
	
	if (!KwIfFileExist(fl))
		fl.Format(L"%s\\HeidiSQL\\%s", pf86, fname);///�̰� HeidiSQL32 ���� ���� ���

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


