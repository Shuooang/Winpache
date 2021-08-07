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
#include "MainFrm.h"

#include "CmnDoc.h"
#include "SrvDoc.h"
#include "SrvView.h"
#include "DlgSizeAdjust.h"
#include "DlgSslSetting.h"
#include "DlgParallel.h"
#include "OutputWnd.h"

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
	ON_BN_CLICKED(IDC_Parallel, &CSrvView::OnBnClickedParallel)
END_MESSAGE_MAP()

// CSrvView construction/destruction

CSrvView::CSrvView() noexcept
	: CFormInvokable(IDD_MFCEXHTTPSSRV_FORM)
	, CmnView(this)
{
	auto ivc = dynamic_cast<KCheckWnd*>(GetMainApp());
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
	JObj& jbj = doc->_jdata;
	AUTOLOCK(doc->_csJdata);

	KDDXJ_Check(_bSSL);
	KDDXJ_Check(_bStaticCache);
	
	KDDXJ_Int(_CacheLife);
	KDDXJ_Int(_port);

	KDDXJ_Text(_rootLocal);
	KDDXJ_Text(_defFile);
	KDDXJ_Text(_uploadLocal);
	KDDXJ_Text(_ApiURL);
	KDDXJ_Text(_ODBCDSN);
	KDDXJ_Text(_note);

	DDX_Control(pDX, IDC__CacheLife, c_CacheLife);
	DDX_Control(pDX, IDC__rootLocal, c_rootLocal);
	DDX_Control(pDX, IDC__defFile, c_defFile);
	DDX_Control(pDX, IDC__ApiURL, c_ApiURL);
	DDX_Control(pDX, IDC__port, c_port);
	DDX_Control(pDX, IDC__ODBCDSN, c_ODBCDSN);
	DDX_Control(pDX, IDC__note, c_note);

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
			auto ivc = dynamic_cast<KCheckWnd*>(GetMainApp());
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
	AUTOLOCK(doc->_csJdata);
	doc->_svr->_api->AddCallbackOutput([&](string msg, int err) -> void
		{
			OTrace(msg.c_str());
		});

	int nCol = 0;
	auto arLC = GetArListConf(&nCol);
	KwSetListReportStyle(&_cMonitor);
	KwSetListColumn(&_cMonitor, arLC, nCol); // _countof(GetArListConf()));

	if(doc->_jdata.I("_port") == 0)
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
	auto& appd = ((CMFCExHttpsSrvApp*)GetMainApp())->_docApp;
	AUTOLOCK(appd._csAppDoc);
	auto& jobj = *appd._json;
	//auto ivc = dynamic_cast<KCheckWnd*>(GetMainApp());
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
	auto& appd = ((CMFCExHttpsSrvApp*)GetMainApp())->_docApp;
	auto doc = GetDocument();
	AUTOLOCK(doc->_csJdata);

	CStringA guid = doc->_jdata.SA("_GUID");//Ű�� GUID
	appd.UnregisterServerStart(guid);

	return __super::CallbackOnStopped(hev, vuid);
}

int CSrvView::CallbackOnReceived(const void* buffer, size_t size)
{
	return 0;
}
int CSrvView::CallbackOnReceivedRequest(KSessionInfo& inf, int vuid, SHP<KBinData> shbin, HTTPResponse& res)
{
	auto& appd = ((CMFCExHttpsSrvApp*)GetMainApp())->_docApp;
	AUTOLOCK(appd._csAppDoc);
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
	auto& appd = ((CMFCExHttpsSrvApp*)GetMainApp())->_docApp;
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
		EnableCommand(IDC__bSSL, FALSE);
		EnableCommand(IDC__bStaticCache, FALSE);
		EnableCommand(IDC__CacheLife, FALSE);
		EnableCommand(IDC__port, FALSE);

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
		EnableCommand(IDC__bSSL, FALSE);
		EnableCommand(IDC__bStaticCache, FALSE);
		EnableCommand(IDC__CacheLife, FALSE);
		EnableCommand(IDC__port, FALSE);
		EnableCommand(IDC__defFile, FALSE);
		EnableCommand(IDC__ApiURL, FALSE);
		EnableCommand(IDC__ODBCDSN, FALSE);
		EnableCommand(IDC__rootLocal, FALSE);
		EnableCommand(IDC__uploadLocal, FALSE);
		EnableCommand(IDC_BtnRootLocal, FALSE);
		EnableCommand(IDC_BtnUploadLocal, FALSE);
		EnableCommand(IDC_BtnSslSetting, FALSE);
		EnableCommand(IDC_Parallel, FALSE);

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
		EnableCommand(IDC__bSSL, 1, iOp);
		EnableCommand(IDC__bStaticCache, 1, iOp);//?destroy 4.5
		EnableCommand(IDC__CacheLife, 1, iOp);//?destroy 4.5
		EnableCommand(IDC__port, 1);
		EnableCommand(IDC__defFile, 1);
		EnableCommand(IDC__ApiURL, 1);
		EnableCommand(IDC__ODBCDSN, 1);
		EnableCommand(IDC__rootLocal, 1);
		EnableCommand(IDC__uploadLocal, 1);
		EnableCommand(IDC_BtnRootLocal, 1);
		EnableCommand(IDC_BtnUploadLocal, 1);
		EnableCommand(IDC_BtnSslSetting, 1);
		EnableCommand(IDC_Parallel, 1);

		EnableCommand(ID_Start, 1, iOp);
		EnableCommand(ID_Stop, FALSE, iOp);
		EnableCommand(ID_Restart, FALSE, iOp);
	}

	auto frm = (CMainFrame*)AfxGetMainWnd();
	frm->UpdateControl(stat, iOp);
}



// CSrvView message handlers
/// StartServer�� ��� �ϴ� ��ĥ�� �� �ϸ鼭 ���ľ�
void CSrvView::RecoverServer()//?server recover 5.1 contunue running server
{
	//("_bRecover") = TRUE; �̹� OnStart���� ��� �ϸ鼭 �Ѵ�.
	auto app = (CMFCExHttpsSrvApp*)GetMainApp();
	auto& appd = app->_docApp;
	auto doc = GetDocument();
	{
		//AUTOLOCK(appd._csRecover);
		AUTOLOCK(appd._csAppDoc);
		auto& jobj = *appd._json;
		auto dsnMain = jobj.S("DSN");
		auto srsv = jobj.O("RunningServers");
		if(!srsv || srsv->size() == 0)//������� �ְ�
			return;

		CStringA guid = doc->GetStrA("_GUID");
		auto sjo = srsv->O(guid);//�� �Ƶ� ������ ���� ��������� ����
		if(!sjo)
			return;

		if(sjo->I("_bRecover") == FALSE)
			return;
		//���� ���ϵ����� ���� RunningServers�� new�ϼ� �ִ�. ���� ��������. �׷��� ���� ����.
		doc->JsonToData(sjo, false); //���� ������� sjo���� ���� ����. ���� ������
	}

	UpdateData(0);

	AUTOLOCK(doc->_csJdata);
	/// �̰� ���ǹ� �ϰ� �Ǿ���.
	doc->_svr->_api->_ODBCDSN = doc->_jdata.S("_ODBCDSN");
	doc->_svr->_api->_ODBCDSNlog = appd.MakeDsnString();

	auto fm = (CMainFrame*)AfxGetMainWnd();
	if(fm->_fncExtraTrace)
		doc->AddCallbackOnTrace(fm->_fncExtraTrace);//?ExTrace 3 CmnView -> MyHttps + std_cout(KTrace)

	//SSL���� ����� RunningServers���� ���� �ǹǷ� 
	///ASSERT(!doc->_jdata.I("_bSSL") || doc->_jdata.Len("_prvpwd"));
	/// ��� ���̵� ���� ���� �ȴ�.
	StartServer();
}

void CSrvView::OnBnClickedStart()
{
	FOREGROUND();
	UpdateData();

	auto& appd = ((CMFCExHttpsSrvApp*)GetMainApp())->_docApp;
	AUTOLOCK(appd._csAppDoc);
	auto& jobj = *appd._json;

	CmnDoc* doc = GetDocument();
	doc->_svr->_api->_ODBCDSNlog = appd.MakeDsnString();

	auto fm = (CMainFrame*)AfxGetMainWnd();
	if(fm->_fncExtraTrace)
		doc->AddCallbackOnTrace(fm->_fncExtraTrace);//?ExTrace 3 CmnView -> MyHttps + std_cout(KTrace)

	{//?synchronized
		AUTOLOCK(doc->_csJdata);
		if(doc->_jdata.I("_bSSL") && !doc->_jdata.Len("_prvpwd"))
		{
			OnBnClickedBtnSslSetting();//����� �޴´�.
			if(!doc->_jdata.Len("_prvpwd"))//doc->_prvpwd.GetLength() == 0)
			{
				TRACE("No password key.");
				//return;
			}
		}
	}

	///moveto CallbackOnStarted
	//[ server start���� ���� ���ο� ����ϰ�, ���� �ϸ� ���� �Ѵ�. �߰��� ������� ���� �ο����� �ٷ� ���� �Ѵ�.
	//auto& appd = ((CMFCExHttpsSrvApp*)GetMainApp())->_docApp;
	//ShJObj sjobjSvr;// = std::make_shared<JObj>();
	//doc->JsonToData(sjobjSvr, TRUE);
	//appd.RegisterServerStart(sjobjSvr);
	//]
	StartServer();
}


void CSrvView::OnBnClickedStop()
{
	CmnDoc* doc = GetDocument();
// 	auto& appd = ((CMFCExHttpsSrvApp*)GetMainApp())->_docApp;
// 	appd.UnregisterServerStart(doc->_GUID);

	StopServer();
}

void CSrvView::OnBnClickedRestart()
{
	RestartServer();
}

//?deprecated
void CSrvView::OnBnClickedBtnpath()
{
	SrvDoc* doc = GetDocument();
	CString spth = doc->GetStr("_cachedPath");
	CString cpth = SelectFolder(spth);
	doc->SetStr("_cachedPath", cpth);

	//SetDlgItemText(IDC_cachedPath, cpth);
}

void CSrvView::OnBnClickedBtnimalocal()
{
	SrvDoc* doc = GetDocument();
	CString spth = doc->GetStr("_rootLocal");
	CString cpth = SelectFolder(spth);
	doc->SetStr("_rootLocal", cpth);
	
	SetDlgItemText(IDC__rootLocal, cpth);
}
void CSrvView::OnBnClickedBtnUploadLocal()
{
	SrvDoc* doc = GetDocument();
	CString spth = doc->GetStr("_uploadLocal");
	CString cpth = SelectFolder(spth);
	doc->SetStr("_uploadLocal", cpth);

	SetDlgItemText(IDC__uploadLocal, cpth);
}



void CSrvView::OnDestroy()
{
	FOREGROUND();


	CFormInvokable::OnDestroy();
}


// int CSrvView::ViewCount()
// {
// 	auto app = (CMFCExHttpsSrvApp*)GetMainApp();
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
		IDC__rootLocal,
		IDC__uploadLocal,
		IDC__note,
		IDC_MonitorList, 
	};//IDC_vpath,   

	BOOL b = OnSizeDefault(nType, cx, cy, _countof(idc), idc);
}





void CSrvView::OnBnClickedBtnSslSetting()
{
	DlgSslSetting dlg(this);
	SrvDoc* doc = GetDocument();
	dlg._jobj = doc->GetJData();
	dlg._doc = doc;
	if(dlg.DoModal() == IDOK) {
		doc->_jdata.Clone(dlg._jobj, true);
	}
}

void CSrvView::OnBnClickedStartDB()
{
	CWaitCursor sandClock;
	UpdateData(); //InitApi(); ����.

	SrvDoc* doc = GetDocument();
	auto frm = (CMainFrame*)AfxGetMainWnd();
	auto& appd = ((CMFCExHttpsSrvApp*)GetMainApp())->_docApp;
	AUTOLOCK(appd._csAppDoc);
	auto& jobj = *appd._json;

	if(doc->_jdata.IsEmpty("_ODBCDSN"))
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
		doc->_svr->_api->_ODBCDSN = doc->_jdata.S("_ODBCDSN");

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
	//auto app = (CMFCExHttpsSrvApp*)GetMainApp();
	auto api = (ApiSite1*)&doc->_svr->_api;
	try
	{
		UpdateData(); //InitApi(); ����.

		//CStringW ODBCDSN(doc->_jdata.S("_ODBCDSN"));
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
	catch(std::exception& e)
	{	smsg.Format(L"catch std::exception %s  - %s %d\n", e.what(), __FUNCTIONW__, __LINE__); 
	}
	catch(PWS e)
	{	smsg.Format(L"catch LPCWSTR %s  - %s %d\n", e, __FUNCTIONW__, __LINE__); 
	}
	catch(PAS e)
	{	CString sw(e);
	smsg.Format(L"catch LPCSTR %s  - %s %d\n", sw, __FUNCTIONW__, __LINE__); 
	}
	catch(...)
	{	smsg.Format(L"catch ...  - %s %d\n", __FUNCTIONW__, __LINE__); 
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

#include "KwLib64/HTTPS/HttpClient.h"

/// <summary>
/// ���� ó���� ��ü �Ǹ� ��ϵ� �ּҷ� ����ó�� 
/// �ٸ� ������ �ٽ� ������ ���� �ͼ� ���� �Ѵ�.
/// </summary>
/// <param name="inf">���� ����: url, dir, parameter</param>
/// <param name="shbin">INOUT binary data</param>
/// <returns>0�̸� ���� ���ϰ� ����ó�� �Ѵ�.</returns>
int CSrvView::CallbackCluster(KSessionInfo& inf, shared_ptr<KBinData> shbin)
{
	///TODO: "https://192.168.35.177:1234" 
	//  ó�� http(s), �ּ�, port, bUse 4���� ���� ����
	//  2 �׸� URL �� iOp �� �ȴ�. iOp: 0:�̻��, 1:���, 2:�ٻܶ��� ���, -1:error

	/// appdoc���� "Clustering" array�� ���� �ͼ�.
	/// for�� �����鼭, option�� 1 �Ǵ� 2�� ������ �������� ��û/���� �޾� ���� �Ѵ�.
	auto& appd = ((CMFCExHttpsSrvApp*)GetMainApp())->_docApp;
// 	{
// 		AUTOLOCK(appd._csAppDoc);//���⼭ �ð��� ���� �ɸ��Ƿ� ������ �ͼ� ��� �Ѵ�.
// 		auto sclt0 = appd._json->O("Clustering");
// 		arSvr->Clone(sclt0, true);
// 	}
	CmnDoc* doc = GetDocument();
	if(doc == NULL)
		return 0;
//	JObj& jbj = doc->_jdata;

// 	ShJArr arSvr = make_shared<JArr>();
// 	{
// 		AUTOLOCK(doc->_csJdata);
// 		auto sclt0 = jbj.Array("Clustering");
// 		arSvr->Clone(sclt0, true);/// �����۾��̶� �����ؼ� ����.
// 	}
	auto sjdata = doc->GetJData();/// �����۾��̶� �����ؼ� ����.
	ShJArr arSvr = sjdata->Array("Clustering");

	if(!arSvr || arSvr->size() == 0)//������� �ְ�
		return 0;
#ifdef _DEBUG12
	KStdMap<__int64, ShJVal> msclt;
	for(auto& [kurl, shval] : *arSvr)
	{
		auto shsvr = shval->AsObject();
		auto cnt = shsvr->I("count");// shval->AsInt64();
		msclt[cnt] = shval;// count�� ����. ���� ������ �� ����
	}
#endif // _DEBUG12

	/// <summary>
	InterlockedIncrement(&doc->_cntReq);
	/// </summary>

	KArray<ShJObj> arSvrAct;
	for(int i=0;i<arSvr->size();i++)
	{
		auto sitm = arSvr->GetAt(i); if(!sitm->IsObject()) 	continue;
		auto svr = sitm->AsObject(); if(!svr) continue;
		if(svr->SameS("action", L"open")) // ������ΰŸ� �迭�� �ٽ� ��´�.
		{
			string surlA = svr->SA("url");
			int cntErr = 0;
			doc->_mpCntError.Lookup(surlA, cntErr);
			if(cntErr < 3) /// 3�� ���� �л�ó�� ���� �ĺ����� Ż��
				arSvrAct.push_back(svr);
			else
			{
				TRACE("error > 3 : %s\n", surlA.c_str());
				/// ���� ������ "action":"error" �� �ٲ��
				doc->SetClusteringServer(surlA.c_str(), "action", L"closed");

// 				AUTOLOCK(doc->_csJdata);
// 				ShJArr arSvr1 = doc->_jdata.Array("Clustering");
// 				for(int c = 0; c < arSvr1->size(); c++)
// 				{
// 					auto sitm1 = arSvr1->GetAt(i); if(!sitm1->IsObject()) continue;
// 					auto svr1 = sitm1->AsObject(); if(!svr1) continue;
// 					if(svr1->SameSA("url", surlA.c_str()))
// 					{
// 						(*svr1)("action") = L"closed";
// 						break;
// 					}
// 				}
			}
		}
	}

	auto nSvrs = arSvrAct.size();//�ּ��� ������ ��� �ִ�.
	if(nSvrs == 0)/// �� ������ ���� ���� �Ǹ� ���� ������ closed ���� ó�� �ϵ��� 2�� ����.
		return 2;///���⼭ ���� ���� �θ���, svr->_fncCluster)...) HttpCmn::onReceivedRequest
	auto chns = doc->_cntReq % nSvrs; // ������ þ��
	int nTry = 3;
	while(nTry--)
	{
		ShJObj svrChns = arSvrAct.GetAt((int)chns);
		if(svrChns->SameS("mode", L"main")) // main �̸� ���� �������� ó�� �ϹǷ� ���� 2
			return 2;// 2:�� �ڽ��� ó�� �ϴ� �Ŵ�. 1: �̸� ���⼭ �ٽ� ȣ�� �Ѵ�.

		hres hr = S_OK;
// 		if(!_httpCl)
// 		{	_httpCl = make_shared<CHttpClient>();
// 			_httpCl->m_nMilSecTimeoutResolve = 500; // DOMAIN => IP
// 			_httpCl->m_nMilSecTimeoutConnect = 1000;// �̰� ���� �ٿ� �ε� �������� ������. �Ⱦ���.
// 		}
//		CHttpClient& cl = *_httpCl;
		CHttpClient cl;
		cl.m_nMilSecTimeoutResolve = 500; // DOMAIN => IP
		cl.m_nMilSecTimeoutConnect = 1000;// �̰� ���� �ٿ� �ε� �������� ������. �Ⱦ���.
		KBinary binr, bin;
		string surlA = svrChns->SA("url");// http://192.168.33.155:4343

		CString surl(surlA.c_str()); // http://192.168.33.155:4343
		surl += CString(inf._url.c_str()); // �� _url �� �ּ�:��Ʈ �ڿ� �κ��̴�. ���̸� �ϼ�

		/// POST ������ ���� �ϰ�
		bin.SetPtr((PAS)shbin->m_p, shbin->m_len);

		/// ����� ���� �ϰ�
		KStdMap<string, string> hdrs;
		for(auto& [k, v] : inf._headers)
			hdrs[k] = v;

		/// <summary>
		hr = cl.RequestPostSSL(surl, &binr, &bin, &hdrs);
		/// </summary>
		/// <param name="surl">���� ���ļ��� �ּ��� URL(ex: http://192.168.1.30:8080/api?func=ExFunc1</param>
		/// <param name="binr">out JSON data</param>
		/// <param name="bin">in JSON data</param>
		/// <param name="hdrs">���� ���� �������</param>
		/// <returns>0�̸� ����</returns>
		if(hr == 0)
		{
			shbin->Attach(binr.m_p, binr.m_len);/// responded data
			return 1;
		}
		else //error ���� error Ƚ�� ���� ���Ѿ�
		{
			// 		AUTOLOCK(doc->_csJdata);
			// 		auto sclt0 = jbj.Array("Clustering");
			int cntErr = 0;
			doc->_mpCntError.Lookup(surlA, cntErr);
			cntErr++;
			doc->_mpCntError[surlA] = cntErr;
			if(nSvrs > 2)
			{
				chns++;
				if(chns == nSvrs)//���� ���� ���� �ϸ� 0������
					chns = 0;
				continue; // chns�� ���� ������ �Ѿ� ������ ���� ���ο��� �ٽ� �õ�
			}
			return -1;
		}
	}


//	ShJObj minsvr = doc->_mpClst.begin()->second;
// 	for(auto& [cnt, shval] : doc->_mpClst)
// 	{
// 		minsvr = shval;
// 		break;
// 	}
	// url vs jobj{option:, ..}
#ifdef _old_gabage
	for(auto& [kurl, shval] : *arSvr)
	{
		if(!shval->IsObject())
			continue;
		auto& sjbj = shval->AsObject();
		int iOp = sjbj->I("option");
		/// 1��Ʈ: ��������, 2��Ʈ:�ʴ�ó�� �ӵ� 40�Ѿ� ����. CPU�� ������, DB�� �۾��� �и���
		if(iOp & (1|2|4))//KwAttr(iOp, 1) || KwAttr(iOp, 2))
		{
			if(iOp & 4)//�ڱ� �ڽ�
				return 4;

			hres hr = S_OK;
			CHttpClient cl;
			CString surl(kurl.c_str());// sjbj->S("url"); // http://192.168.33.155:4343
			KBinary binr, bin;
			surl += CString(inf._url.c_str());
			bin.SetPtr((PAS)shbin->m_p, shbin->m_len);
			KStdMap<string, string> hdrs;
			for(auto& [k,v] : inf._headers)
				hdrs[k] = v;
			hr = cl.RequestPostSSL(surl, &binr, &bin, &hdrs);
			if(hr == 0)
			{
				shbin->Attach(binr.m_p, binr.m_len);// returned data
				return 1;
			}
			else
				return -1;
		}
	}
#endif // _old_gabage
	return 0;///svr->_fncCluster)...) HttpCmn::onReceivedRequest
}

void CSrvView::OnBnClickedParallel()
{
	DlgParallel dlg(this);
	SrvDoc* doc = GetDocument();
	if(doc == NULL)
		return;
	dlg._doc = dynamic_cast<CDocument*>(doc);
	dlg.DoModal();
}
