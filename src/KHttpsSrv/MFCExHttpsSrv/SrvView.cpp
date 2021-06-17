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




	ON_BN_CLICKED(IDC_Start, &CSrvView::OnBnClickedStart)
	ON_BN_CLICKED(IDC_Stop, &CSrvView::OnBnClickedStop)
	ON_BN_CLICKED(IDC_Restart, &CSrvView::OnBnClickedRestart)
	ON_BN_CLICKED(IDC_BtnPath, &CSrvView::OnBnClickedBtnpath)
	//ON_BN_CLICKED(IDC_bSSL, &CSrvView::OnBnClickedbssl)
	ON_BN_CLICKED(IDC_BtnRootLocal, &CSrvView::OnBnClickedBtnimalocal)
	ON_BN_CLICKED(IDC_StartDB, &CSrvView::OnBnClickedStartDB)
	ON_BN_CLICKED(IDC_TestAPI, &CSrvView::OnBnClickedTestapi)
	//	ON_BN_CLICKED(IDC_StartUDP, &CSrvView::OnBnClickedStartUDP)
	ON_BN_CLICKED(IDC_BtnUploadLocal, &CSrvView::OnBnClickedBtnUploadLocal)
	ON_BN_CLICKED(IDC_BtnSslSetting, &CSrvView::OnBnClickedBtnSslSetting)

	ON_COMMAND(ID_MIGRATION_IMAGESIZEADJUST, &CSrvView::OnMigrationImageSizeAdjust)
//	ON_COMMAND(ID_FreeLibrary, &CSrvView::OnFreeLibrary)
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CSrvView construction/destruction

CSrvView::CSrvView() noexcept
	: CFormInvokable(IDD_MFCEXHTTPSSRV_FORM)
	, CmnView(this)
{
	auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
	_id = ivc->ViewRegister(this);
	// 이 뷰가 생성됨을 여기서 app에 등록 한다.
		
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

	CmnView::DoDataExchange(pDX);

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
	
	//버튼을 숨긴다. 리본메뉴에 있으므로
	GetDlgItem(IDC_Start)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_Stop)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_Restart)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TestAPI)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_StartDB)->ShowWindow(SW_HIDE);

	UpdateControl("init");// 이걸 해줘야. Enable  정보가 들어 있어서 메뉴가 UpdateEnable 정보

	/// CmnView::_fncExtraTrace 에 OutputWnd에 내보내는 람다를 등록한다.
	// 이 _fncExtraTrace는 스타트서버할때 서버에게도 복사 된다.
	AddCallbackExtraTrace([&, this, idVu, idOw](string txt) -> void //?ExTrace 1 실제 루틴을 정의 한다.람다
		{
			// 람다 불리기전에 받은 vuid가 아직 살아 있는지 확인 한다.
			auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
			bool bVu = !ivc ? false : ivc->ViewFind(idVu);
			bool bOutput = !ivc ? false : ivc->ViewFind(idOw);
			if(bOutput)//?destroy 7 : 여기서 NULL이 나와야 하는데, 0xddddddddddd
			{
				COutputWnd::s_me->TraceQueue(txt.c_str());//?ExTrace 7
				if(CMainPool::s_UiThreadId == ::GetCurrentThreadId())//FOREGROUND();
					COutputWnd::s_me->TraceFlush();//?ExTrace 7 //?destroy 8
				else
				{
					if(bVu)// KwBeginInvoke하려면 this View가 살아 있어야지.
					{
						KwBeginInvoke(this, ([&, txt]()-> void { //?beginInvoke 4
								COutputWnd::s_me->TraceFlush();//?ExTrace 7 //?destroy 8
							}));
					}
				}
			}
		});

	SrvDoc* doc = GetDocument();
	doc->_svr->_api->AddCabackOutput([&](PAS msg, int err) -> void
		{
			Trace(msg);
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
	KwEnableWindow(this, IDC_Stop, FALSE);
	KwEnableWindow(this, IDC_Restart, FALSE);
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


// CSrvView message handlers

void CSrvView::OnBnClickedStart()
{
	FOREGROUND();
	UpdateData();
	CmnDoc* doc = GetDocument();
	if(doc->_bSSL && doc->_prvpwd.GetLength() == 0)
	{
		OnBnClickedBtnSslSetting();//비번을 받는다.
		if(doc->_prvpwd.GetLength() == 0)
			return;
	}

// 	KwEnableWindow(this, IDC_Start, FALSE);
// 	KwEnableWindow(this, IDC_Stop, FALSE);
// 	KwEnableWindow(this, IDC_Restart, FALSE);
// 	KwEnableWindow(this, IDC_bSSL, FALSE);
// 	KwEnableWindow(this, IDC_bStaticCache, FALSE);
	StartServer();
}


void CSrvView::OnBnClickedStop()
{
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
	///?중요: ChildFrame::OnClose 에서 Shutdown 부른다.
	//원래는 여기나 OnDestroy에서 KCheckWnd::ViewRemove를 불러야 하는데,
	//	여기서는 ChldFrame::OnClose 에서 CmnView::Shutdown을 부르는 거로 대체했다.
	__super::OnClose();//여기는 안불려 진다
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
	CWaitCursor _sandClock;
	UpdateData(); //InitApi(); 포함.
	SrvDoc* doc = GetDocument();

	CStringW ODBCDSN(doc->_ODBCDSN);
	if(tchlen((PWS)ODBCDSN) == 0)
	{
		PAS pas = "Data Source name is empty.";
		KwMessageBoxA(pas);
		Trace(pas);
		return;
	}
	Trace("ODBC Data Source conneting...");
	auto frm = (CMainFrame*)AfxGetMainWnd();
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_doc;
	auto rs = doc->_svr->_api->CheckDB(ODBCDSN, appd._dbMain);
	if(rs)
		Trace(rs);
	SetDlgItemText(IDC_STATICDB, L" DB Connected.");
	Trace("Site Database Connected.");
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
		//api->CheckDB();
		UpdateData(); //InitApi(); 포함.

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
/// 이거 나중에 없앨거야. 도킹창으로 옮길거야.
/// </summary>
/// <param name="nCols"></param>
/// <returns></returns>
_STitleWidthField* CSrvView::GetArListConf(int* nCols)
{
	if(nCols)
		*nCols = _countof(s_arlstCtrl);
	return s_arlstCtrl;
}
// void CSrvView::MonitorRequest(shared_ptr<KArray<string>> shar)
// {
// 	//자체 리스트가 아니고 다른 도킹 창에서 해야 한다.
// 	/// DockReceiveList::s_me->MonitorRequest(shar);
// }
void CSrvView::MonitorRequest(shared_ptr<KArray<string>> shar)
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
