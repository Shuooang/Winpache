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

// SrvView.cpp : implementation of the CSrvView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.

#include <direct.h> //_getcwd
#include "KwLib64/DlgTool.h"
#include "KwLib64/tchtool.h"
#include "KwLib64/TimeTool.h"
#include "KwLib64/ThreadPool.h"
#include "ApiBase.h"
#include "ApiSite1.h"

#include "KwLib64/Recordset.h"
#include "KwLib64/KDebug.h"

#ifndef SHARED_HANDLERS
#include "MFCExHttpsSrv.h"
#endif
#include "CmnDoc.h"

#include "SrvDoc.h"
#include "SrvView.h"
#include "DlgSizeAdjust.h"
#include "DlgSslSetting.h"
#include "OutputWnd.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSrvView

IMPLEMENT_DYNCREATE(CSrvView, CFormInvokable)

BEGIN_MESSAGE_MAP(CSrvView, CFormInvokable)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()

	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CLOSE()



/// ��ư�� ��������, �����޴��� ����. �˾��޴��� ����
// 	ON_BN_CLICKED(IDC_Start, &CSrvView::OnBnClickedStart)
// 	ON_BN_CLICKED(IDC_Stop, &CSrvView::OnBnClickedStop)
// 	ON_BN_CLICKED(IDC_Restart, &CSrvView::OnBnClickedRestart)
// 	ON_BN_CLICKED(IDC_BtnPath, &CSrvView::OnBnClickedBtnpath)


//ON_BN_CLICKED(IDC_bSSL, &CSrvView::OnBnClickedbssl)
	ON_BN_CLICKED(IDC_BtnRootLocal, &CSrvView::OnBnClickedBtnimalocal)
	//ON_BN_CLICKED(ID_StartDB, &CSrvView::OnBnClickedStartDB)
	//ON_BN_CLICKED(IDC_TestAPI, &CSrvView::OnBnClickedTestapi)
	//	ON_BN_CLICKED(IDC_StartUDP, &CSrvView::OnBnClickedStartUDP)
	ON_BN_CLICKED(IDC_BtnUploadLocal, &CSrvView::OnBnClickedBtnUploadLocal)
	ON_BN_CLICKED(IDC_BtnSslSetting, &CSrvView::OnBnClickedBtnSslSetting)

	ON_COMMAND(ID_MIGRATION_IMAGESIZEADJUST, &CSrvView::OnMigrationImageSizeAdjust)
//	ON_COMMAND(ID_FreeLibrary, &CSrvView::OnFreeLibrary)
	ON_COMMAND(ID_SiteStart, &CSrvView::OnSiteStart)
	ON_UPDATE_COMMAND_UI(ID_SiteStart, &CSrvView::OnUpdateSiteStart)
	ON_COMMAND(ID_SiteStop, &CSrvView::OnSiteStop)
	ON_UPDATE_COMMAND_UI(ID_SiteStop, &CSrvView::OnUpdateSiteStop)
	ON_COMMAND(ID_SiteRestart, &CSrvView::OnSiteRestart)
	ON_UPDATE_COMMAND_UI(ID_SiteRestart, &CSrvView::OnUpdateSiteRestart)
	
	ON_COMMAND(ID_ConnectSiteDB, &CSrvView::OnConnectSiteDB)
	ON_UPDATE_COMMAND_UI(ID_ConnectSiteDB, &CSrvView::OnUpdateConnectSiteDB)
END_MESSAGE_MAP()

// CSrvView construction/destruction

CSrvView::CSrvView() noexcept
	: CFormInvokable(IDD_MFCEXHTTPSSRV_FORM)
	, CmnView(this)
{
	auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
	_id = ivc->ViewRegister(this);
	// �� �䰡 �������� ���⼭ app�� ��� �Ѵ�.
		
}

CSrvView::~CSrvView()
{
}

void CSrvView::DoDataExchange(CDataExchange* pDX)
{
	CFormInvokable::DoDataExchange(pDX);
// 	SrvDoc* doc = GetDocument();
// 	if(doc == NULL)
// 		return;

	//CmnView::DoDataExchange(pDX);
	CmnDoc* doc = GetDocument();
	if(doc == NULL)
		return;
	KDDX_Check(_bSSL);
	KDDX_Check(_bStaticCache);
	KDDX_Text(_CacheLife);
	KDDX_TextA(_rootLocal);
	KDDX_TextA(_defFile);
	KDDX_TextA(_uploadLocal);
	KDDX_TextA(_ApiURL);
	KDDX_Text(_port);
	KDDX_TextA(_ODBCDSN);
	KDDX_Text(_note);

	DDX_Control(pDX, IDC_CacheLife, c_CacheLife);
	DDX_Control(pDX, IDC_rootLocal, c_rootLocal);
	DDX_Control(pDX, IDC_defFile, c_defFile);
	DDX_Control(pDX, IDC_ApiURL, c_ApiURL);
	DDX_Control(pDX, IDC_port, c_port);
	DDX_Control(pDX, IDC_ODBCDSN, c_ODBCDSN);
	DDX_Control(pDX, IDC_note, c_note);

	if(!pDX->m_bSaveAndValidate) //�о� ���̸� UpdateData(); 
		doc->InitApi();


	DDX_Control(pDX, IDC_MonitorList, _cMonitor);
}


BOOL CSrvView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormInvokable::PreCreateWindow(cs);
}



void CSrvView::OnInitialUpdate()
{
	CFormInvokable::OnInitialUpdate();
	ResizeParentToFit();

	_output = COutputWnd::s_me;
	int idOw = COutputWnd::s_me->_id;
	int idVu = _id;
	
	//��ư�� �����. �����޴��� �����Ƿ�
// 	GetDlgItem(IDC_Start)->ShowWindow(SW_HIDE);
// 	GetDlgItem(IDC_Stop)->ShowWindow(SW_HIDE);
// 	GetDlgItem(IDC_Restart)->ShowWindow(SW_HIDE);
// 	GetDlgItem(IDC_TestAPI)->ShowWindow(SW_HIDE);
// 	GetDlgItem(IDC_StartDB)->ShowWindow(SW_HIDE);

	UpdateControl("init");// �̰� �����. Enable  ������ ��� �־ �޴��� UpdateEnable ����

	/// CmnView::_fncExtraTrace �� OutputWnd�� �������� ���ٸ� ����Ѵ�.
	// �� _fncExtraTrace�� ��ŸƮ�����Ҷ� �������Ե� ���� �ȴ�.
#ifdef __MovedToMainFrame
	AddCallbackExtraTrace([&, this, idVu, idOw](string txt) -> void //?ExTrace 1 ���� ��ƾ�� ���� �Ѵ�.����
		{
			// ���� �Ҹ������� ���� vuid�� ���� ��� �ִ��� Ȯ�� �Ѵ�.
			auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
			bool bVu = !ivc ? false : ivc->ViewFind(idVu);
			bool bOutput = !ivc ? false : ivc->ViewFind(idOw);
			if(bOutput && bVu)//?destroy 7 : ���⼭ NULL�� ���;� �ϴµ�, 0xddddddddddd
			{
				COutputWnd::s_me->TraceQueue(txt.c_str());//?ExTrace 7
				COutputWnd::s_me->TraceFlush();//?ExTrace 7 //?destroy 8
				/*if(CMainPool::s_UiThreadId == ::GetCurrentThreadId())//FOREGROUND();
					COutputWnd::s_me->TraceFlush();//?ExTrace 7 //?destroy 8
				else
				{
					//if(bVu)// _KwBeginInvoke�Ϸ��� this View�� ��� �־����.
					{
						_KwBeginInvoke(this, ([&, txt]()-> void { //?beginInvoke 4
							COutputWnd::s_me->TraceFlush();//?ExTrace 7 //?destroy 8
							}));
					}
				}*/
			}
		});
#endif // __MovedToMainFrame

	SrvDoc* doc = GetDocument();
	doc->_svr->_api->AddCallbackOutput([&](string msg, int err) -> void
		{
			OTrace(msg.c_str());
		});

	int nCol = 0;
	auto arLC = GetArListConf(&nCol);
	KwSetListReportStyle(&_cMonitor);
	KwSetListColumn(&_cMonitor, arLC, nCol); // _countof(GetArListConf()));

	if(doc->_port == 0)
	{
		SampleServer();
		UpdateData(0);
	}
	KwEnableWindow(this, ID_Stop, FALSE);
	KwEnableWindow(this, ID_Restart, FALSE);

	RecoverServer();//?server recover 5 contunue running server
}

void CSrvView::OTrace(PAS txt, int iOp)
{
	if(iOp == 0)
	{
		string str = txt;
		_KwBeginInvoke(this, ([&, str]()-> void
			{ //?beginInvoke 4
				OTrace(str.c_str(), 1);
			}));
	}
	else if(iOp == 1)
	{
		auto fm = (CMainFrame*)AfxGetMainWnd();
		if(fm && fm->_fncExtraTrace)
			(*fm->_fncExtraTrace)(txt);
		TRACE("%s\n", txt);
	}

}

void CSrvView::OTrace(PWS txt, int iOp)
{
	CStringA str(txt);
	OTrace(str, iOp);
// 	auto fm = (CMainFrame*)AfxGetMainWnd();
// 	CStringA stra(txt);
// 	if(fm->_fncExtraTrace)
// 		(*fm->_fncExtraTrace)((PAS)stra);
// 	TRACE("%s\n", txt);
}

void CSrvView::CallbackOnStarted(int vuid)//?server recover 1 ��� ���� ���� OnStarted����
{
	BACKGROUND(1);
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	auto& jobj = *appd._json;
	//auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
	//bool bVu = !ivc ? false : ivc->ViewFind(vuid);
	//if(bVu)//_vu && ::IsWindow(_vu->GetSafeHwnd()))
	auto doc = GetDocument();
	/// Start�� ������ ���� �صд�.
	ShJObj sjsvr = std::make_shared<JObj>();
	doc->JsonToData(sjsvr, true);///���������� JObj�� ��� �´�.
	appd.RegisterServerStart(sjsvr);//?server recover 1.1
	//doc->GetPathName();
	__super::CallbackOnStarted(vuid);//�ַ� EnableControl�Ѵ�
	
}
int CSrvView::CallbackOnStopped(HANDLE hev, int vuid)//?server recover 2 ��� ���� ����
{
	BACKGROUND(1);
	/// Stop�� ������ ���� �Ѵ�.
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	auto doc = GetDocument();

	auto guid = doc->_GUID;//Ű�� GUID
	appd.UnregisterServerStart(guid);

	return __super::CallbackOnStopped(hev, vuid);
}

int CSrvView::CallbackOnReceived(const void* buffer, size_t size)
{
	return 0;
}
int CSrvView::CallbackOnReceivedRequest(KSessionInfo& inf, int vuid, SHP<KBinData> shbin, HTTPResponse& res)
{
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	auto& jobj = *appd._json;

	appd.ReqOccured(inf._ssid);
	//TRACE("CallbackOnReceived %I64u\n", size);
	_KwBeginInvoke(this, ([&]()-> void
		{
			double npm = appd.GetSpeedPerSec();
			CString s;// 100msec: 0.1��: 1000ms/100ms
			if(npm > 0)//��� ������ �ʴ� Ƚ���� �ٲ�
			{
				s.Format(L"Speed: %4.0f/sec", 10000000. / npm);
				SetDlgItemText(IDC_NPM, s);//�������Ƿ�. �������� ����
			}
		}));
	return 0;
}

int CSrvView::CallbackOnSent(KSessionInfo& inf, int vuid, size_t sent, size_t pending)
{
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	if(pending == 0)
	{
		double npm = appd.OnResponse(inf._ssid);
		if(npm > 0)
		{
			_KwBeginInvoke(this, ([&, npm]()-> void
				{
					CString s;
					s.Format(L"Elapsed: %7.3f msec", npm / 10000.);
					SetDlgItemText(IDC_Elapsed, s);//�������Ƿ�. �������� ����
				}));
		}
	}
	return __super::CallbackOnSent(inf, vuid, sent, pending);
}

void CSrvView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CSrvView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CSrvView diagnostics

#ifdef _DEBUG
void CSrvView::AssertValid() const
{
	CFormInvokable::AssertValid();
}

void CSrvView::Dump(CDumpContext& dc) const
{
	CFormInvokable::Dump(dc);
}
SrvDoc* CSrvView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(SrvDoc)));
	return (SrvDoc*)m_pDocument;
}
#endif //_DEBUG


void CSrvView::UpdateControl(CStringA stat, int iOp)
{
	_KwBeginInvoke(this, ([&, stat, iOp]()-> void
		{ //?beginInvoke 4
			UpdateControlFore(stat, iOp);
		}));
}

void CSrvView::UpdateControlFore(CStringA stat, int iOp)
{
	FOREGROUND();
	//iOp not used yet
	if(stat == "init")
	{
		EnableCommand(ID_StartDB, 1);
		stat = "stopped";
	}
	else if(stat == "error")
	{
		stat = "stopped";
	}

	if(stat == "starting")
	{
		EnableCommand(IDC_bSSL, FALSE);
		EnableCommand(IDC_bStaticCache, FALSE);
		EnableCommand(IDC_CacheLife, FALSE);
		EnableCommand(IDC_port, FALSE);

		EnableCommand(ID_Start, FALSE);
		EnableCommand(ID_Stop, FALSE);
		EnableCommand(ID_Restart, FALSE);
	}
	else if(stat == "restarting")
	{
		EnableCommand(ID_Start, FALSE);
		EnableCommand(ID_Stop, FALSE);
		EnableCommand(ID_Restart, FALSE);
	}
	else if(stat == "started")
	{
		EnableCommand(IDC_bSSL, FALSE);
		EnableCommand(IDC_bStaticCache, FALSE);
		EnableCommand(IDC_CacheLife, FALSE);
		EnableCommand(IDC_port, FALSE);
		EnableCommand(IDC_defFile, FALSE);
		EnableCommand(IDC_ApiURL, FALSE);
		EnableCommand(IDC_ODBCDSN, FALSE);
		EnableCommand(IDC_rootLocal, FALSE);
		EnableCommand(IDC_uploadLocal, FALSE);

		EnableCommand(ID_Start, FALSE);
		EnableCommand(ID_Stop, 1);
		EnableCommand(ID_Restart, 1);
	}
	else if(stat == "stopping")
	{
		EnableCommand(ID_Start, FALSE);
		EnableCommand(ID_Stop, FALSE);
		EnableCommand(ID_Restart, FALSE);
	}
	else if(stat == "stopped")
	{//iOp not used
		EnableCommand(IDC_bSSL, 1, iOp);
		EnableCommand(IDC_bStaticCache, 1, iOp);//?destroy 4.5
		EnableCommand(IDC_CacheLife, 1, iOp);//?destroy 4.5
		EnableCommand(IDC_port, 1);
		EnableCommand(IDC_defFile, 1);
		EnableCommand(IDC_ApiURL, 1);
		EnableCommand(IDC_ODBCDSN, 1);
		EnableCommand(IDC_rootLocal, 1);
		EnableCommand(IDC_uploadLocal, 1);

		EnableCommand(ID_Start, 1, iOp);
		EnableCommand(ID_Stop, FALSE, iOp);
		EnableCommand(ID_Restart, FALSE, iOp);
	}

	auto frm = (CMainFrame*)AfxGetMainWnd();
	frm->UpdateControl(stat, iOp);
}



// CSrvView message handlers

void CSrvView::RecoverServer()//?server recover 5.1 contunue running server
{
	//("_bRecover") = TRUE; �̹� OnStart���� ��� �ϸ鼭 �Ѵ�.
	auto app = (CMFCExHttpsSrvApp*)AfxGetApp();
	auto& appd = app->_docApp;
	auto doc = GetDocument();
	{
		AUTOLOCK(appd._csRecover);
		auto& jobj = *appd._json;
		auto dsnMain = jobj.S("DSN");
		auto srsv = jobj.O("RunningServers");
		if(!srsv || srsv->size() == 0)//������� �ְ�
			return;

		auto sjo = srsv->O(doc->_GUID);//�� �Ƶ� ������ ���� ��������� ����
		if(!sjo)
			return;

		if(sjo->I("_bRecover") == FALSE)
			return;
		//���� ���ϵ����� ���� RunningServers�� new�ϼ� �ִ�. ���� ��������. �׷��� ���� ����.
		doc->JsonToData(sjo, false); //���� ������� sjo���� ���� ����. ���� ������
	}
	UpdateData(0);

	/// �̰� ���ǹ� �ϰ� �Ǿ���.
	//if(doc->_bDbConnected)//��� �����߿� DB�� ����� ���� ������ �ٽ� ���� �ؾ���.
	{
//		ASSERT(appd._dbMain->IsOpen());
		CString ODBCDSN(doc->_ODBCDSN);
		//auto rs = doc->_svr->_api->CheckDB(ODBCDSN, appd._dbMain);
		doc->_svr->_api->_ODBCDSN= doc->_ODBCDSN;
		doc->_svr->_api->_ODBCDSNlog = appd.MakeDsnString();
	}

	auto fm = (CMainFrame*)AfxGetMainWnd();
	if(fm->_fncExtraTrace)
		doc->AddCallbackOnTrace(fm->_fncExtraTrace);//?ExTrace 3 CmnView -> MyHttps + std_cout(KTrace)

	//SSL���� ����� RunningServers���� ���� �ǹǷ� 
	ASSERT(!doc->_bSSL || doc->_prvpwd.GetLength() > 0);
	StartServer();
}

void CSrvView::OnBnClickedStart()
{
	FOREGROUND();
	UpdateData();

	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	auto& jobj = *appd._json;

	CmnDoc* doc = GetDocument();
	doc->_svr->_api->_ODBCDSNlog = appd.MakeDsnString();

	auto fm = (CMainFrame*)AfxGetMainWnd();
	if(fm->_fncExtraTrace)
		doc->AddCallbackOnTrace(fm->_fncExtraTrace);//?ExTrace 3 CmnView -> MyHttps + std_cout(KTrace)

	if(doc->_bSSL && doc->_prvpwd.GetLength() == 0)
	{
		OnBnClickedBtnSslSetting();//����� �޴´�.
		if(doc->_prvpwd.GetLength() == 0)
			return;
	}

	///moveto CallbackOnStarted
	//[ server start���� ���� ���ο� ����ϰ�, ���� �ϸ� ���� �Ѵ�. �߰��� ������� ���� �ο����� �ٷ� ���� �Ѵ�.
	//auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	//ShJObj sjobjSvr;// = std::make_shared<JObj>();
	//doc->JsonToData(sjobjSvr, TRUE);
	//appd.RegisterServerStart(sjobjSvr);
	//]
	StartServer();
}


void CSrvView::OnBnClickedStop()
{
	CmnDoc* doc = GetDocument();
// 	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
// 	appd.UnregisterServerStart(doc->_GUID);

	StopServer();
}

void CSrvView::OnBnClickedRestart()
{
	RestartServer();
}


void CSrvView::OnBnClickedBtnpath()
{
	SrvDoc* doc = GetDocument();
	SelectFolder(doc->_cachedPath);
}



void CSrvView::OnBnClickedBtnimalocal()
{
	SrvDoc* doc = GetDocument();
	//CmnDoc* doc = dynamic_cast<CmnDoc*>(GetDocument());
	SelectFolder(doc->_rootLocal);
}
void CSrvView::OnBnClickedBtnUploadLocal()
{
	SrvDoc* doc = GetDocument();
	SelectFolder(doc->_uploadLocal);
}



void CSrvView::OnDestroy()
{
	FOREGROUND();


	CFormInvokable::OnDestroy();
}


// int CSrvView::ViewCount()
// {
// 	auto app = (CMFCExHttpsSrvApp*)AfxGetApp();
// 	return app->_mapWnd.size();
// }
void CSrvView::OnClose()
{
	///?�߿�: ChildFrame::OnClose ���� Shutdown �θ���.
	//������ ���⳪ OnDestroy���� KCheckWnd::ViewRemove�� �ҷ��� �ϴµ�,
	//	���⼭�� ChldFrame::OnClose ���� CmnView::Shutdown�� �θ��� �ŷ� ��ü�ߴ�.
	__super::OnClose();//����� �Ⱥҷ� ����
}

void CSrvView::OnSize(UINT nType, int cx, int cy)
{
	CFormInvokable::OnSize(nType, cx, cy);

	int idc[] = {
		//IDC_cachedPath,
		//IDC_ODBCDSN,
		IDC_rootLocal,
		IDC_uploadLocal,
		IDC_note,
		IDC_MonitorList, 
	};//IDC_vpath,   

	BOOL b = OnSizeDefault(nType, cx, cy, _countof(idc), idc);
}





void CSrvView::OnBnClickedBtnSslSetting()
{
	DlgSslSetting dlg;
	SrvDoc* doc = GetDocument();
	dlg._doc = doc;
	if(dlg.DoModal() == IDOK) {}
}

void CSrvView::OnBnClickedStartDB()
{
	CWaitCursor sandClock;
	UpdateData(); //InitApi(); ����.

	SrvDoc* doc = GetDocument();
	auto frm = (CMainFrame*)AfxGetMainWnd();
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
	auto& jobj = *appd._json;

	if(doc->_ODBCDSN.IsEmpty())
	{
		PAS pas = "Data Source name is empty.\nEx:DSN=MyDSN;UID=myid;PWD=pypwd;database=sitedb";
		KwMessageBoxA(pas);
		OTrace(pas);
		return;
	}
	OTrace("ODBC Data Source conneting...");
	try
	{
		doc->_svr->_api->_ODBCDSNlog = appd.MakeDsnString();
		doc->_svr->_api->_ODBCDSN = doc->_ODBCDSN;

		SHP<KDatabase> sdb = KDatabase::getDbConnected((PWS)doc->_svr->_api->_ODBCDSN);
		if(sdb)
		{
			doc->_svr->_api->InitDbLogProc(sdb.get());

			SetDlgItemText(IDC_STATICDB, L" DB Connected.");
			OTrace("Site Database is Tested.");
		}
	}
	catch(CDBException* e)
	{
		CString smsg; //smsg.Format(L"%s\n%s.", e->m_strError, e->m_strStateNativeOrigin);
		smsg.Format(L"%s\nDo you want to set up ODBC?", e->m_strError, e->m_strStateNativeOrigin);
//		KwMessageBoxError(smsg);
		int rv = ::MessageBox(GetSafeHwnd(), smsg, L"Error", MB_OKCANCEL | MB_ICONERROR);
		if(rv == IDOK)
		{
			KDatabase::OpenOdbcSetting();
		}
	}
	catch (KException* e)
	{
		CString smsg; smsg.Format(L"%s\n%s.", e->m_strError, e->m_strStateNativeOrigin);
		KwMessageBoxError(smsg);
	}
	catch(CException* e)
	{
		CString serr;
		e->GetErrorMessage(serr.GetBuffer(1024), 1024); serr.ReleaseBuffer();
		KwMessageBoxError(serr);
	}
}
void CSrvView::OnMigrationImageSizeAdjust()
{
	DlgSizeAdjust dlg;
	if(dlg.DoModal() == IDOK)
	{
	}
}


void CSrvView::OnBnClickedTestapi()
{
	SrvDoc* doc = GetDocument();
	CString smsg;
	//auto app = (CMFCExHttpsSrvApp*)AfxGetApp();
	auto api = (ApiSite1*)&doc->_svr->_api;
	try
	{
		UpdateData(); //InitApi(); ����.

		CStringW ODBCDSN(doc->_ODBCDSN);
	}
	catch(CDBException* e)
	{
		//CString s;s.Format (L"CDBException:%s - %s\n", e->m_strError, e->m_strStateNativeOrigin);
		KwMessageBox(L"CDBException:%s - %s\n", e->m_strError, e->m_strStateNativeOrigin);
	}
	catch(KException* e)
	{
		//CString s; s.Format(L"KException:%s - %s\n", e->m_strError, e->m_strStateNativeOrigin);
		KwMessageBox(L"KException:%s - %s\n", e->m_strError, e->m_strStateNativeOrigin);
	}
	catch(_com_error* e)
	{
		//CString s; s.Format(L"KException:%s - %s\n", e->m_strError, e->m_strStateNativeOrigin);
		KwMessageBox(L"_com_error:%s\n", e->ErrorMessage());
	}
	catch(CException* e)
	{
		auto buf = new TCHAR(1024);
		KAtEnd d_buf([&]() { delete buf; });
		e->GetErrorMessage(buf, 1000);
		TRACE(L"CException:%s - %s %d\n", buf, __FUNCTION__, __LINE__);
		KwMessageBox(buf);
	}
	catch(TException ex)
	{
		TErrCode ec = ex.code();
		smsg.Format(L"catch boost::systemerror %s  - %s %d\n", ec.message().c_str(), __FUNCTIONW__, __LINE__); \
	}
	catch(std::exception& e)\
	{	smsg.Format(L"catch std::exception %s  - %s %d\n", e.what(), __FUNCTIONW__, __LINE__); \
	}
	catch(PWS e)\
	{	smsg.Format(L"catch LPCWSTR %s  - %s %d\n", e, __FUNCTIONW__, __LINE__); \
	}
	catch(PAS e)\
	{	CString sw(e);
	smsg.Format(L"catch LPCSTR %s  - %s %d\n", sw, __FUNCTIONW__, __LINE__); \
	}
	catch(...)\
	{	smsg.Format(L"catch ...  - %s %d\n", __FUNCTIONW__, __LINE__); \
	}
	if(smsg.GetLength())
		AfxMessageBox(smsg);


}


//?deprecated
/*void CSrvView::OnFreeLibrary()
{
	SrvDoc* doc = GetDocument();

	doc->FreeDllLibrary();
}
*/
CDocument* CSrvView::GetDoc()
{
	return GetDocument();
}



_STitleWidthField s_arlstCtrl[] =
{
	{ 20, nullptr     , L"#"    ,},
	{ 130, "fTime"     , L"Time",},
	{ 60, "fMethod", L"Method", },
	{ 140, "fURL"   , L"URL",},
	{ 60, "fCache"  , L"Cache",},
	{ 80, "fType"  , L"Type",},
	{ 60, "fLength"  , L"Length",},
	//	{ 0, "fOrder"  , NULL,},
};
/// <summary>
/// �̰� ���߿� ���ٰž�. ��ŷâ���� �ű�ž�.
/// </summary>
/// <param name="nCols"></param>
/// <returns></returns>
_STitleWidthField* CSrvView::GetArListConf(int* nCols)
{
	if(nCols)
		*nCols = _countof(s_arlstCtrl);
	return s_arlstCtrl;
}
// void CSrvView::MonitorRequest(SHP<KArray<string>> shar)
// {
// 	//��ü ����Ʈ�� �ƴϰ� �ٸ� ��ŷ â���� �ؾ� �Ѵ�.
// 	/// DockReceiveList::s_me->MonitorRequest(shar);
// }
void CSrvView::MonitorRequest(SHP<KArray<string>> shar)
{
	int i = 0;
	auto& ar = *shar;
	_cMonitor.InsertItem(i, CStringW(ar[0].c_str()));
	auto nCols = 0;
	_STitleWidthField* arList = GetArListConf(&nCols);// _countof(s_arlstCtrl);
	if(arList == NULL)
		return;

	for(int j = 1; j < nCols && j < ar.size(); j++)
	{
		CStringW txt(ar[j].c_str());
		if(txt.GetLength() == 0)
			_break;
		_cMonitor.SetItem(i, j, LVIF_TEXT, txt, 0, 0, 0, NULL, 0);
	}

	// if list too many, cut tail
	int lst = _cMonitor.GetItemCount();
	if(lst > 10000)
	{
		for(int j = 0; j < 1000; j++, lst--)
		{
			_cMonitor.DeleteItem(lst - 1);
		}
	}
}


void CSrvView::CopyRequest()
{
	int n = _cMonitor.GetItemCount();
	auto hdctrl = _cMonitor.GetHeaderCtrl();
	int nc = hdctrl->GetItemCount();

	Tss ss;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < nc; j++)
		{
			CString cel = _cMonitor.GetItemText(i, j);
			ss << (PWS)cel;
			if (j == (nc - 1))
				ss << L"\r\n";
			else
				ss << L"\t";
		}
	}
	KwCopyTextClipboad(this, ss.str().c_str());
}
void CSrvView::CopyOutput()
{
	auto& out = COutputWnd::s_me->m_wndOutputBuild;
	int n = out.GetCount();
	Tss ss;
	for (int i = 0; i < n; i++)
	{
		CString cel;
		out.GetText(i, cel);
		ss << (PWS)cel << L"\r\n";
	}
	KwCopyTextClipboad(this, ss.str().c_str());
}


void CSrvView::OnUpdateCmn(CCmdUI* pCmdUI, int idc)
{
	BOOL b = this->GetUpdate(idc);//CChildFrame::OnClose() ������ �ҷ��� �Ѵ�.
	pCmdUI->Enable(b);
}

void CSrvView::OnSiteStart()
{
	OnBnClickedStart();
}
void CSrvView::OnUpdateSiteStart(CCmdUI* pCmdUI)
{
	OnUpdateCmn(pCmdUI, ID_Start);
}
void CSrvView::OnSiteStop()
{
	OnBnClickedStop();
}
void CSrvView::OnUpdateSiteStop(CCmdUI* pCmdUI)
{
	OnUpdateCmn(pCmdUI, ID_Stop);
}
void CSrvView::OnSiteRestart()
{
	OnBnClickedRestart();
}
void CSrvView::OnUpdateSiteRestart(CCmdUI* pCmdUI)
{
	OnUpdateCmn(pCmdUI, ID_Restart);
}
void CSrvView::OnConnectSiteDB()
{
	OnBnClickedStartDB();
}
void CSrvView::OnUpdateConnectSiteDB(CCmdUI* pCmdUI)
{
	OnUpdateCmn(pCmdUI, ID_StartDB);
}
