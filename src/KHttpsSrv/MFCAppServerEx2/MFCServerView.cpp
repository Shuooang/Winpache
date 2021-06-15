
// MFCServerView.cpp: CMFCServerView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
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
#include "MFCAppServerEx2.h"
#endif
#include "CmnDoc.h"

#include "MFCAppServerEx2Doc.h"
#include "MFCServerView.h"
#include "DlgSizeAdjust.h"
#include "DlgSslSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCServerView


IMPLEMENT_DYNCREATE(CMFCServerView, CFormInvokable)

BEGIN_MESSAGE_MAP(CMFCServerView, CFormInvokable)
	// 표준 인쇄 명령입니다.
	ON_WM_DESTROY()
	ON_WM_SIZE()

	ON_MESSAGE(WM_USER_INVOKE, &CMFCServerView::OnBeginInvoke)//?beginInvoke 2

	ON_COMMAND(ID_FILE_PRINT, &CFormInvokable::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormInvokable::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormInvokable::OnFilePrintPreview)

	//ON_COMMAND(ID_ImageMigration, &CMFCServerView::OnImageMigration)

	ON_BN_CLICKED(IDC_Start, &CMFCServerView::OnBnClickedStart)
	ON_BN_CLICKED(IDC_Stop, &CMFCServerView::OnBnClickedStop)
	ON_BN_CLICKED(IDC_Restart, &CMFCServerView::OnBnClickedRestart)
	ON_BN_CLICKED(IDC_BtnPath, &CMFCServerView::OnBnClickedBtnpath)
	//ON_BN_CLICKED(IDC_bSSL, &CMFCServerView::OnBnClickedbssl)
	ON_BN_CLICKED(IDC_BtnRootLocal, &CMFCServerView::OnBnClickedBtnimalocal)
	ON_BN_CLICKED(IDC_StartDB, &CMFCServerView::OnBnClickedStartDB)
	ON_BN_CLICKED(IDC_TestAPI, &CMFCServerView::OnBnClickedTestapi)
//	ON_BN_CLICKED(IDC_StartUDP, &CMFCServerView::OnBnClickedStartUDP)
	ON_BN_CLICKED(IDC_BtnUploadLocal, &CMFCServerView::OnBnClickedBtnUploadLocal)
	ON_BN_CLICKED(IDC_BtnSslSetting, &CMFCServerView::OnBnClickedBtnSslSetting)
	
	ON_COMMAND(ID_MIGRATION_IMAGESIZEADJUST, &CMFCServerView::OnMigrationImageSizeAdjust)
	ON_COMMAND(ID_FreeLibrary, &CMFCServerView::OnFreeLibrary)
END_MESSAGE_MAP()

// CMFCServerView 생성/소멸

CMFCServerView::CMFCServerView() noexcept
	: CFormInvokable(IDD_MFCAPPSERVEREX2_FORM)
	, CmnView(this)
{
	auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
	_id = ivc->ViewRegister(this);
}

CMFCServerView::~CMFCServerView()
{
}

void CMFCServerView::DoDataExchange(CDataExchange* pDX)
{
	CFormInvokable::DoDataExchange(pDX);

// 	CMFCAppServerEx2Doc* doc = GetDocument();
// 	if(doc == NULL)
// 		return;

	CmnView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_MonitorList, _cMonitor);

}

BOOL CMFCServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CFormInvokable::PreCreateWindow(cs);
}

void CMFCServerView::OnInitialUpdate()
{
	CFormInvokable::OnInitialUpdate();
	ResizeParentToFit();

	CMFCAppServerEx2Doc* doc = GetDocument();

#ifdef _DEBUGx
	CString sDay = L"2020-12-27";//CAST('2020-12-27' AS CHAR CHARACTER SET UTF16) COLLATE 'utf16_unicode_ci';
	CString sNextDay = L"2020-12-28";//CAST('2020-12-27' AS CHAR CHARACTER SET UTF16) COLLATE 'utf16_unicode_ci';

	Quat qs;//TEST
	PWS wsql = L"SELECT * FROM (\
			SELECT nf.fOrder, nf.fMode, nf.fBizID, nf.fUsrIdStf, nf.fOffBegin, nf.fOffEnd, nf.fOff, nf.fNote, nf.fLat, nf.fLon, b2.fDist from\
			(\
				SELECT 2 fOrder, 'open' fMode, b.fBizID, NULL fUsrIdStf,\
				TIMESTAMP(DATE(@sDay), b.fBegin)";	
	qs_Field(qs, sDay);
	qs_Field(qs, sNextDay);
	qs.SetSQL(wsql);
	//qs.Apply();

	Quat qs1;
	qs1.CopyField(qs);
	qs1.SetSQL(wsql);
	qs1.Apply();
#endif
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
	//OnBnClickedbssl();
}


// CMFCServerView 인쇄

BOOL CMFCServerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMFCServerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMFCServerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CMFCServerView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: 여기에 사용자 지정 인쇄 코드를 추가합니다.
}


// CMFCServerView 진단

#ifdef _DEBUG
void CMFCServerView::AssertValid() const
{
	CFormInvokable::AssertValid();
}

void CMFCServerView::Dump(CDumpContext& dc) const
{
	CFormInvokable::Dump(dc);
}

CMFCAppServerEx2Doc* CMFCServerView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCAppServerEx2Doc)));
	return (CMFCAppServerEx2Doc*)m_pDocument;
}
#endif //_DEBUG


/*
//서버 열기 전에 API함수 쓸기전에. image migration...
// CMainFrame.OnCreate 보다 여기 OnInitalUpdate가 먼저 불려 진다.
void CMFCServerView::InitApi()
{
	CMFCAppServerEx2Doc* doc = GetDocument();
	auto app = (CMFCAppServerEx2*)AfxGetApp();
	doc->_svr->_api->_ODBCDSN = doc->_ODBCDSN;
	doc->_svr->_api->_rootLocal = doc->_rootLocal;
	doc->_svr->_api->_uploadLocal = doc->_uploadLocal;
	doc->_svr->_api->_rootURL = doc->_rootURL;
	doc->_svr->_api->_defFile = doc->_defFile;
	
	doc->_svr->_api->_ApiURL = doc->_ApiURL;
}*/

void CMFCServerView::OnBnClickedStart()
{
	FOREGROUND();
	UpdateData(0);
	CmnDoc* doc = GetDocument();
	if(doc->_bSSL && doc->_prvpwd.GetLength() == 0)
	{
		OnBnClickedBtnSslSetting();
		if(doc->_prvpwd.GetLength() == 0)
			return;
	}
	StartServer();
}


void CMFCServerView::OnBnClickedStop()
{
	StopServer();
}

void CMFCServerView::OnBnClickedRestart()
{
	RestartServer();
}


void CMFCServerView::OnBnClickedBtnpath()
{
	CMFCAppServerEx2Doc* doc = GetDocument();
	SelectFolder(doc->_cachedPath);
}



void CMFCServerView::OnBnClickedBtnimalocal()
{
	CMFCAppServerEx2Doc* doc = GetDocument();
	SelectFolder(doc->_rootLocal);
}
void CMFCServerView::OnBnClickedBtnUploadLocal()
{
	CMFCAppServerEx2Doc* doc = GetDocument();
	SelectFolder(doc->_uploadLocal);
}



void CMFCServerView::OnDestroy()
{
//	_vu = NULL;
// 	CMFCAppServerEx2Doc* doc = GetDocument();
// 	doc->ShutdownServer();///Winpache Pro에서는 ChildFrame OnClose 에서 끝낸다.
//	Shutdown();
	CFormInvokable::OnDestroy();
}


void CMFCServerView::OnSize(UINT nType, int cx, int cy)
{
	CFormInvokable::OnSize(nType, cx, cy);

	int idc[] ={ 
		//IDC_cachedPath, 
		//IDC_ODBCDSN, 
		IDC_rootLocal, 
		IDC_uploadLocal, 
		IDC_note, 
		IDC_MonitorList,
	};//IDC_vpath,   

	BOOL b = OnSizeDefault(nType, cx, cy, _countof(idc), idc);
}





void CMFCServerView::OnBnClickedBtnSslSetting()
{
	DlgSslSetting dlg;
	CMFCAppServerEx2Doc* doc = GetDocument();
	dlg._doc = doc;
	if(dlg.DoModal() == IDOK) {}
}

/*
void CMFCServerView::OnBnClickedbssl()
{
	UpdateData();
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMFCAppServerEx2Doc* doc = GetDocument();
	BOOL b = doc->_bSSL;
//	BOOL bStarted = KwIsEnableWindow(this, IDC_certificate);

	//KwEnableWindow(this, IDC_Start, b);
// 	KwEnableWindow(this, IDC_certificate, b);
// 	KwEnableWindow(this, IDC_privatekey, b);
// 	KwEnableWindow(this, IDC_dhparam, b);
// 	KwEnableWindow(this, IDC_prvpwd, b);
// 	KwEnableWindow(this, IDC_BtnCertificate, b);
// 	KwEnableWindow(this, IDC_BtnPrivate, b);
// 	KwEnableWindow(this, IDC_BtnDhparam, b);

	if(b)
	{
	}
}
*/

//#include "DlgImageMigration.h"
//void CMFCServerView::OnImageMigration()
// {
// 	OnBnClickedStartDB();
// 
// 	DlgImageMigration dlg;
// 	CMFCAppServerEx2Doc* doc = GetDocument();
// 	dlg._doc = doc;
// 	if(dlg.DoModal() == IDOK) {}
// }


void CMFCServerView::OnBnClickedStartDB()
{
	UpdateData(); //InitApi(); 포함.
	auto app = (CMFCAppServerEx2*)AfxGetApp();
	CMFCAppServerEx2Doc* doc = GetDocument();

	CStringW ODBCDSN(doc->_ODBCDSN);
	if(tchlen((PWS)ODBCDSN) == 0)
	{
		KwMessageBox(L"Data Source name is empty.");
		return;
	}
	SetDlgItemText(IDC_STATICDB, L"..connecting...");

	KwBeginInvoke(this, ([&, doc, ODBCDSN]()-> void
		{ //?beginInvoke 4
			auto rs = doc->_svr->_api->CheckDB(ODBCDSN);
			SetDlgItemText(IDC_STATICDB, (rs == nullptr) ? L"Connected." : L"Error while ODBC connecting.");
		}));

}


void CMFCServerView::OnMigrationImageSizeAdjust()
{
	DlgSizeAdjust dlg;
	if (dlg.DoModal() == IDOK)
	{
	}
}


void CMFCServerView::OnBnClickedTestapi()
{
	CMFCAppServerEx2Doc* doc = GetDocument();
	CString smsg;
	auto app = (CMFCAppServerEx2*)AfxGetApp();
	auto api = (ApiSite1*)&doc->_svr->_api;
	try
	{
		//api->CheckDB();
		UpdateData(); //InitApi(); 포함.

		CStringW ODBCDSN(doc->_ODBCDSN);
		if(tchlen((PWS)ODBCDSN) == 0)
		{
			KwMessageBox(L"Data Source name is empty.");
			return;
		}

		auto rs = doc->_svr->_api->CheckDB(ODBCDSN);
		if (rs)
			KTrace(L"Error while ODBC Connting!");



#define DllImport   __declspec( dllimport )

#ifdef _DEBUG
		// 		char curDir1[1000] = {'\0', };
		// 		_getcwd(curDir1, 1000);
		// 		TRACE("Cur dir is %s\n", curDir1);

		typedef BOOL(_stdcall* API_TYPE)(HANDLE, HANDLE, HANDLE);
		API_TYPE ApiDll;//  {MFCLibrary1.dll!ExportedFunction2}	int(*)(void *, void *, void *)

		HMODULE hDll = ::LoadLibraryW(L"MFCLibrary1.dll");
		if(hDll == NULL)
			return;
		// 			HMODULE hDll1 = ::GetModuleHandleW(L"C:\\Outbin\\KHttpsSrv\\x64\\Debug\\MFCLibrary1.dll");
		// 			HMODULE hDll2 = ::LoadLibraryW(L"C:\\Outbin\\KHttpsSrv\\x64\\Debug\\MFCLibrary1.dll");

		ApiDll = (API_TYPE)::GetProcAddress(hDll, "ExportedFunction2");
		if(ApiDll)
		{
			BOOL b = ApiDll((HANDLE)1, (HANDLE)2, (HANDLE)3);
			_break;
		}

		typedef HRESULT(_stdcall* API_TYPE3)(KDatabase&, JObj&, JObj&, int);
		/*API_TYPE3 ApiDll3 = (API_TYPE3)::GetProcAddress(hDll, "ExportedFunction3");
		// HRESULT(*)(KDatabase&, Kw::JObj&, Kw::JObj&, int)
		if(ApiDll3)
		{
			JObj jpa, jrs;
			jpa("fBegin") = L"2021-02-09 14:00:00";
			jpa("fEnd") = L"2021-02-09 14:30:00";
			jpa("fUsrID") = L"user-0001";
			jpa("fLat") = 37.;
			jpa("fLon") = 127.;
			HRESULT hr = ApiDll3(doc->_svr->_api->_db, jpa, jrs, 0);
		}*/


		typedef HRESULT(_stdcall* API_SITE)(KDatabase&, JObj&, JObj&, int);

		API_SITE SampleExSelectUUID = (API_SITE)::GetProcAddress(hDll, "SampleExSelectUUID");
		// HRESULT(*)(KDatabase&, Kw::JObj&, Kw::JObj&, int)
		if(SampleExSelectUUID)
		{
			JObj jpa, jrs;
			jpa("limit") = 5;
			HRESULT hr = SampleExSelectUUID(doc->_svr->_api->_db, jpa, jrs, 0);
		}

#endif // _DEBUG
















#ifdef _APIEX
#define _RequestCertCode

		
		JObj jpa, jrs;
#if _SearchScheduleByLocation
		jpa("fBegin") = L"2021-02-09 14:00:00";
		jpa("fEnd") = L"2021-02-09 14:30:00";

		jpa("fUsrID") = L"user-0001";
		//jpa("fDay") = L"2021-02-09 00:00:00";
		jpa("fLat") = 37.;
		jpa("fLon") = 127.;
		api->SearchScheduleByLocation(jpa, jrs, 0);
#elif defined(_RequestCertCode)
		jpa("fUsrID") = L"user-0001";
		api->RequestCertCode(jpa, jrs, 0);
#elif defined(_RequestLogin)
		//jpa("fUsrID") = L"user-0001";
		jpa("fLoginID") = L"id0001";
		jpa("fPcPwd") = L"pwd0001";
		api->RequestLogin(jpa, jrs, 0);
#elif defined(_GetTodaySchdule)
		jpa("fAllStaff") = L"every";// 1;//항상 1 이어야 한다. 0은 deprecated
		jpa("fUsrID") = L"user-0001";
		jpa("fBizID") = L"biz-0002";
		jpa("fDay") = L"2021-02-09 14:30:00";
		api->GetTodaySchdule(jpa, jrs, 0);
#elif defined(_UserManage)
		jpa("fUsrID") = L"user-0001";
		jpa("fProj") = L"petme2";

		JObj jpo;
		jpo("action") = L"select";
		jpa("Option") = jpo;
		api->UserManage(jpa, jrs, 0);
#elif defined(_SelectFullOrder)
		//SET @fOrderID = w_UCast('order1-0001');
		jpa("fOrderID") = L"order1-0001";
		api->SelectFullOrder(jpa, jrs, 0);
#endif		
			CStringW sin = jpa.ToJsonStringW();
		CStringW sw = jrs.ToJsonStringW();

		SetDlgItemText(IDC_note, sw);
#endif // _APIEX

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





void CMFCServerView::OnFreeLibrary()
{
	CMFCAppServerEx2Doc* doc = GetDocument();
	CString smsg;
	auto app = (CMFCAppServerEx2*)AfxGetApp();
	auto api = (ApiSite1*)&doc->_svr->_api;
	if(CApiBase::s_hDllExtra)
	{
		BOOL b = ::FreeLibrary(CApiBase::s_hDllExtra);
		if(b)
		{
			CApiBase::s_hDllExtra = NULL;
			KwMessageBox(L"Now, the API library \"%s\" can be changed.", doc->_svr->_api->getExLibName());
		}
		else
			KwMessageBox(L"Error, while free library \"%s\".", doc->_svr->_api->getExLibName());
	}
// 	Sleep(1000);
// 	CApiBase::s_hDllExtra = ::LoadLibraryW(L"MFCLibrary1.dll");
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

_STitleWidthField* CMFCServerView::GetArListConf(int* nCols)
{
	if(nCols)
		*nCols = _countof(s_arlstCtrl);
	return s_arlstCtrl;
}

void CMFCServerView::MonitorRequest(shared_ptr<KArray<string>> shar)
{
	int i = 0;
	auto& ar = *shar;
	_cMonitor.InsertItem(i, CStringW(ar[0].c_str()));
	auto nCols = 0;
	_STitleWidthField* arList = GetArListConf(&nCols);// _countof(s_arlstCtrl);
	if(arList == NULL)
		return;

	for(int j = 1; j < nCols && j < (int)ar.size(); j++)
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
