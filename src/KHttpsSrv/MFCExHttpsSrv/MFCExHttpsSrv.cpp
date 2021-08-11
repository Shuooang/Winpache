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

// MFCExHttpsSrv.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MFCExHttpsSrv.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "SrvDoc.h"
#include "SrvView.h"
#include "OpenSsl.h"
#include "KwLib64/KDebug.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCExHttpsSrvApp

BEGIN_MESSAGE_MAP(CMFCExHttpsSrvApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CMFCExHttpsSrvApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
END_MESSAGE_MAP()



#include <tuple>

/// error C2065: 'value': undeclared identifier
template <typename TP> size_t TupleSizeT(TP& tp)
{
	return std::tuple_size<decltype(tp)>::value;
}

#define TplCount(tp) std::tuple_size<decltype(tp)>::value
#define TplItem(i, tp) std::get<i>(tp)
using std::pair;
using std::tuple;
using std::make_tuple;



void Test1()
{
	std::map<double, double> map;
	map.insert(pair<double, double>(0.123, 0.1));
	map.insert(pair<double, double>(2.5, 0.4));
	map.insert(pair<double, double>(5.6, 0.8));
	map.insert(pair<double, double>(6.6, 0.8));
	map.insert(pair<double, double>(7.6, 0.8));
	map.insert(pair<double, double>(8.6, 0.8));
	map.insert(pair<double, double>(9.6, 0.8));
	map.insert(pair<double, double>(18.6, 0.8));
	map.insert(pair<double, double>(28.6, 0.8));

	std::map<double, double>::iterator low, high;
	double pos = 10.0;
	low = map.lower_bound(pos);
	high = map.upper_bound(pos);
}

int Test2()
{
	std::map<int, int> mymap;

	mymap[1] = 10;
	mymap[2] = 40;
	mymap[3] = 100;
	mymap[4] = 200;
	mymap[5] = 500;

	int wantedvalue = 50;

	///키가 아니고 값을 찾는다.
	auto it = std::min_element(mymap.begin(), mymap.end(),
		[&](const auto& p1, const auto& p2)
		{// p1이 더 가까워
			return std::abs((long)p1.second - wantedvalue) <
				std::abs((long)p2.second - wantedvalue);
		});

	std_coutD << "The closest value of " << wantedvalue << " in the map is located";
	std_coutD << " on " << it->first << " and is " << it->second << std_endl;

	return 0;
}


template <typename T1, typename T2, typename T3>
class Tpl3
	: public std::tuple<T1, T2, T3>
{
public:
	Tpl3(){}
	Tpl3(T1 t1, T2 t2, T3 t3)
	{
		Set(t1, t2, t3);
	}
	void Set(T1 t1, T2 t2, T3 t3)
	{
		//*this = std::make_tuple((T1)t1, (T2)t2, (T3)t3);//error C2679: binary '=': no operator found which takes a right-hand operand of type 'std::tuple<int,std::string,bool>' (or there is no acceptable conversion)
		std::get<0>(*this) = t1;
		std::get<1>(*this) = t2;
		std::get<2>(*this) = t3;
	}
// 	template <typename T>
// 	T Get(size_t i, T defval)
// 	{
// 		return std::get<i>(*this);// TplItem(i, *this);
// 	}
	//size_t Size(){	return TplCount(*this);	} always 3
	T1 a() { return TplItem(0, *this); }
	T2 b() { return TplItem(1, *this); }
	T3 c() { return TplItem(2, *this); }
};
int Test3()
{
	// make tuple variable.
	typedef std::tuple<int, std::string, bool> OddOrEven;
	OddOrEven myNumber = std::make_tuple(10, std::string("Even"), true);
	Tpl3<int, string, bool> t3;
	t3.Set(11, "Odd", true);
	Tpl3<int, string, bool> t31(11, "Odd", true);
	int v1 = t31.a();

	// get tuple size
	auto tsv = std::tuple_size<decltype(myNumber)>::value;
	//tsv	3	unsigned __int64

	std_coutD << "size 1: " << std::tuple_size<decltype(myNumber)>::value << std_endl;
	//std_coutD << "size 2: " << TupleSizeT(myNumber) << std_endl;
	std_coutD << "size 3: " << TplCount(myNumber) << std_endl;

	// get each value and get type using std::tuple_element, auto keyword.
	std::tuple_element<0, decltype(myNumber)>::type nNum = std::get<0>(myNumber);
	auto szVal = std::get<1>(myNumber);
	bool bEven = std::get<2>(myNumber);

	auto nNum1 = TplItem(0, myNumber);
	auto szVal1 = TplItem(1, myNumber);
	bool bEven1 = TplItem(2, myNumber);

	Tas ss;
	ss << nNum << ", " << szVal << ", " << std::boolalpha << bEven << std_endl;

	return 0;
}
// CMFCExHttpsSrvApp construction

CMFCExHttpsSrvApp::CMFCExHttpsSrvApp() noexcept
{
	Test3();

	m_bHiColorIcons = TRUE;


	m_nAppLook = 0;
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	//SetAppID(_T("WinpachePro.AppID.NoVersion"));
	SetAppID(_T("Keepspeed.Winpache.Winpache.Ver1.0.1"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CMFCExHttpsSrvApp object

CMFCExHttpsSrvApp theApp;

// AfxGetApp()는 BACKGROUND에서 에러 난다.
CWinAppEx* GetMainApp()
{
	return &theApp;
}

// CMFCExHttpsSrvApp initialization

BOOL CMFCExHttpsSrvApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Keepspeed"));/// 이건 SOFTWARE 바로 밑에 생기는 것이다. WinpachePro_k"));//?zzz// Local AppWizard - Generated Applications"));
	/// 이렇게 주면 안된다. free 하고 _tcsdup해서 할당 해야 내부적으로 free할때 안죽는다.
	///m_pszProfileName = L"WinpachePro";
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup(L"Winpache");// m_pszAppName);는 어디서 초기화 되는지 몰라서 긱접.

	g_opTrace |= 1 << 0;

	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

	/// NUMBER_OF_PROCESSORS 시스템 변수가 CPU갯수
	// WinAPI function; it returns a   dwNumberOfProcessors
	SYSTEM_INFO si{0,};
	GetSystemInfo(&si);
// 	dwOemId	9	unsigned long
// 	wProcessorArchitecture	9	unsigned short
// 	wReserved	0	unsigned short
// 	dwPageSize	4096	unsigned long
// 	lpMinimumApplicationAddress	0x0000000000010000	void*
// 	lpMaximumApplicationAddress	0x00007ffffffeffff	void*
// 	dwActiveProcessorMask	15	unsigned __int64
// 	dwNumberOfProcessors	4	unsigned long
// 	dwProcessorType	8664	unsigned long
// 	dwAllocationGranularity	65536	unsigned long
// 	wProcessorLevel	6	unsigned short
// 	wProcessorRevision	15363	unsigned short
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION slpi{0,};
	DWORD len  = sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
	BOOL b1 = GetLogicalProcessorInformation(&slpi, &len);
	// b1 == FALSE : 실패
	CMainPool::Pool(si.dwNumberOfProcessors * 2);//?zzz
	///  4 * 2 인데, 14개 사용 되는거 보면, 내부적으로 5개, UI 1개 쓰인가 보다.

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);



	_docApp.LoadData();
	/// 여기서 창이 뜨기 전에 LoadCount 가 1이면 초기화 창중에 숨길건 숨긴다.

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_WinpacheTYPE,
		RUNTIME_CLASS(SrvDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSrvView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);


// 	enum {
// 		FileNew, FileOpen, FilePrint, FilePrintTo, FileDDE, FileDDENoShow, AppRegister,
// 		AppUnregister, RestartByRestartManager, FileNothing = -1
// 	} m_nShellCommand;
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;//?PreventViewFirst 
	//이렇게 해야 Mainframe 창에 뜬 후에  view 뜨기 전에 BeginInvoke한 DB접속 문제를 해결 한다.
	//그리고 나서 접속 성공 하면, 그때 app->OnFileNew() 를 불려

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CMFCExHttpsSrvApp::ExitInstance()
{
	_docApp.SaveData();
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CMFCExHttpsSrvApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CMFCExHttpsSrvApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CMFCExHttpsSrvApp customization load/save methods


void CMFCExHttpsSrvApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CMFCExHttpsSrvApp::LoadCustomState()
{
}

void CMFCExHttpsSrvApp::SaveCustomState()
{
}

// CMFCExHttpsSrvApp message handlers


/// //////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(KAppDoc, CDocument)

// BEGIN_MESSAGE_MAP(KAppDoc, CDocument)
// END_MESSAGE_MAP()


// KAppDoc construction/destruction

KAppDoc::KAppDoc() noexcept
{
}

KAppDoc::~KAppDoc()
{
}

BOOL KAppDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}



/// <summary>
/// 이건 처음 셋팅할때와 계속 쓸때가 다르다.
/// </summary>
/// <param name="bFirst">처음 실행때만 true이고 이 후 사용떄는 항상 
///		false가 default 값이다.
/// </param>
/// <returns>ODBC OpenEx에 연결에 쓰일 DSN문자열</returns>
CString KAppDoc::MakeDsnString(bool bFirst)//false
{
	AUTOLOCK(_csAppDoc);
	auto& jOdbc = *_json->O("ODBC", true);
	CString dsnLog;
	dsnLog.Format(L"DSN=%s;UID=%s;PWD=%s"
		, jOdbc.S("DSN")
		, jOdbc.S("UID")
		, jOdbc.S("PWD")
	);//PWD가 있는거 ODBC에 확인 했으니, DSN만으로 로그인 시도

	if(bFirst)
	{
		CString ex;
		ex.Format(L";database=%s;SERVER=%s;Driver=%s;PORT=%s;TCPIP=%s"
			, jOdbc.S("DATABASE")
			, jOdbc.S("SERVER")
			, jOdbc.S("Driver")
			, jOdbc.S("PORT")
			, jOdbc.S("TCPIP"));
		dsnLog += ex;
	}
	return dsnLog;
}
void KAppDoc::InitDoc()
{
	AUTOLOCK(_csAppDoc);
	auto& jobj = *_json;
	if(!jobj.Len("GUID"))
		jobj("GUID") = KwGetFormattedGuid(FALSE);//main cfg GUID

	//jobj("_GUID") = L"";
	jobj("FName") = L"WinpacheMain.cfg";
	jobj("LoadCount") = 0;// 이거로 bFirst ㅈ 판별
	jobj("LastSpeed") = 0.;//분당 처리 횟수: 최근 5초 안에 응답한 요청만 계산
	jobj("Elapsed") = 0.;//평균 처리 속도: 요청 들어와서 응답한 시간까지 계속 (합산/카운터)
	jobj("StatDB") = L"none";

	jobj("RunningServers") = JObj(); // RunningServers : { GUID1 : { }, GUID2:{} }
	jobj("ODBC") = JObj(); // RunningServers : { GUID1 : { }, GUID2:{} }

	//auto sjo = jobj.O("ODBC");
	auto& jOdbc = *jobj.O("ODBC");

	jOdbc("DSN") = L"Winpache";
	jOdbc("UID") = L"root";
	jOdbc("PWD") = L"";//있는경우 메인 DB 비번
	jOdbc("DATABASE") = L"winpache";
	jOdbc("SERVER") = L"localhost";
	jOdbc("Driver") = L"MariaDB ODBC 3.1 Driver";
	jOdbc("PORT") = L"3306";
	jOdbc("TCPIP") = L"1";

	/// KDatabase::RegGetODBCMySQL 로 읽어 오면 bnmnnm,
#ifdef _DEBUGx
	{
		"Elapsed":0,
			"FName" : "WinpacheMain.cfg",
			"LastSpeed" : 0,
			"LoadCount" : 1,
			"ODBC" :
		{
			"DATABASE":"winpache",
				"DSN" : "Winpache",
				"Driver" : "MariaDB ODBC 3.1 Driver",
				"PORT" : "3306",
				"PWD" : "bnmnnm,",
				"SERVER" : "localhost",
				"TCPIP" : "1",
				"UID" : "root"
		},
			"RunningServers":{
				// 여기 Start 중인 서버 정보
			},
				"StatDB" : "login",
				"GUID" : "D4D41002DBB647C79D68EFAA68178C3C"
	}
#endif // _DEBUGx
}

void KAppDoc::LoadData()
{
	AUTOLOCK(_csAppDoc);
	if(_json->size() == 0)
		InitDoc();

	CString full = GetFilePath();
	if (KwIfFileExist(full))
	{
		if (!OnOpenDocument(full))
		{
			TRACE("Error! File Winpache.json loading.");
			return;
		}
		SetPathName(full, FALSE);
		//OnDocumentEvent(CDocument::onAfterOpenDocument);
#ifdef _DEBUG
		/// clustering test
		auto& jobj = *_json;
		if(!jobj.Has("Clustering"))
		{//이전 데이터 읽을때 없으면
			auto& jClst = *jobj.O("Clustering", true);
			auto jobcl = JObj();
			jobcl("option") = 1;
			jobcl("count") = 0;
			//jobcl("url") = L"http://parent.co.kr:80";
			jClst("http://parent.co.kr:80") = jobcl;
		}
#endif // _DEBUG

	}
}

CString KAppDoc::GetFilePath()
{
	AUTOLOCK(_csAppDoc);
	TCHAR curDir[1002];
	GetCurrentDirectory(1000, curDir);//C:\Users\dwkang\AppData\Roaming\Outbin\KHttpsSrv\x64\Debug
	//L"Winpache.json";
	WCHAR appdata[MAX_PATH];//CSIDL_PERSONAL
	HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, appdata);
	auto& jobj = *_json;
	if(jobj.size() == 0)
		InitDoc();

	CString fl; fl.Format(L"%s\\Winpache\\", appdata);
	ASSERT(jobj.Len("FName"));
	CString fname = jobj.S("FName"); //	fname = L"WinpacheMain.cfg";
	CString full = fl + fname;
	return full;
}
//C:\Program Files(x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30037
// \atlmfc\src\mfc\afximpl.h
//#include <afximpl.h>
void KAppDoc::SaveData()
{
#ifdef _DEBUGx //KwGetFullPathName test : 폴더 존재 여부에 상관 없이 경로 현실화 함
	PWS fname = L"..\\..\\..\\log\\t_reqlog.log";
	PWS fname1 = L"..\\..\\..\\log\\t_reqlogx.log";//wrong file
	PWS fname2 = L"..\\..\\..\\logx\\t_reqlog.log";//wrong dir
	PWS fname3 = L"..\\..\\..\\logx\\t_reqlogx.log";//both wrong
// 	TCHAR lpszPathOut[512];
// 	BOOL b1 = AfxFullPath(lpszPathOut, fname);
	CString path;
	PWS pFile{ 0 };//result is always OK
	DWORD dw0 = KwGetFullPathName(fname, path, &pFile);//57
	DWORD dw1 = KwGetFullPathName(fname1, path, &pFile);//58
	DWORD dw2 = KwGetFullPathName(fname2, path, &pFile);//58
	DWORD dw3 = KwGetFullPathName(fname3, path, &pFile);//59
#endif // _DEBUG
	try
	{
		/// rule
		/// memory에 pwd를 가지고 있다가. 
		/// SaveData할때 running중인 서버가 있으면 pwd도 저장 하고, 없으면 제외 한다.
		///		따라서 모든 서버가 정상적으로 Stop한 경우, Winpache가 launch될때 마다 비번을 물든다.
		///		비번에 ODBC registry에 포함되어 있다만 그걸 읽어서 비번 안물을수도 있다.
		/// SaveData는 종료할때, 값이 변경되었을때, 서버 Start했을때, 매 5분마다 호출 된다.
		/// 따라서 비정상 종료한 Start 서버 목록이 5분이 안되어서 다시 로운치 할때는, 
		///	자동 문서 오픈, Site DB 연결, Start가 되어야 한다.
		const CString& full0 = GetPathName();//load한 파일 이면
		if(full0.GetLength() > 0)
			VERIFY(DoFileSave()); // 그대로 덮어쓰기.
		else // 새파일 처음
		{
			CString full = GetFilePath();	//full += L"WinpacheMain.cfg";
			VERIFY(DoSave(full, 0));
		}
	}
	catch (CFileException* e)
	{
		CString sErrorW;
		e->GetErrorMessage(sErrorW.GetBuffer(1024), 1024); sErrorW.ReleaseBuffer();
		CStringA err(sErrorW);
		TRACE("CFileException(%d, %ld, %s)\n", e->m_cause, e->m_lOsError, err);
	}
	catch (CException* e)
	{
		CString sErrorW;
		e->GetErrorMessage(sErrorW.GetBuffer(1024), 1024); sErrorW.ReleaseBuffer();
		CStringA err(sErrorW);
		auto rc = e->GetRuntimeClass();
		
		TRACE("CException<%s>(%s)\n", rc->m_lpszClassName, err);
	}
}


/// site server start 된것을 메인 설정에 저장 한다. 
/// 예상하지 않은 정치때 다음 런에 자동 시작 하도록

void KAppDoc::RegisterServerStart(ShJObj sjsvr)//?server recover 1.2
{
	BACKGROUND(1);
	AUTOLOCK(_csAppDoc);
	//AUTOLOCK(_csRecover);
// 	auto lc = sjsvr->I("LoadCount");
// 	(*sjsvr)("LoadCount") = lc + 1;//다시 시도 되면 2
	(*sjsvr)("_bRecover") = TRUE;//일단 RunningServers에만 TRUE로 저장
	(*sjsvr)("_tLastRunning") = KwGetCurrentTimeFullString();

	auto& jobjM = *_json;
	auto oRS = jobjM.O("RunningServers");
	CStringW guid = sjsvr->S("_GUID");
	oRS->DeleteKey(guid);//없어야 하지만 만약에 있다면 없앤다.
	(*oRS)(guid) = sjsvr;// 사이트문서 전체를 넣어 둔다.
						 //jobjM("RunningServers") = oRS; clone이 아니니까 다시 넣을 필요 없다.
	SaveData();
}

void KAppDoc::UnregisterServerStart(CStringA GUID)
{
	BACKGROUND(1);
	AUTOLOCK(_csAppDoc);
	//AUTOLOCK(_csRecover);

	auto& jobjM = *_json;
	auto oRS = jobjM.O("RunningServers");
	if(oRS->DeleteKey(GUID))
		SaveData();
}

void KAppDoc::PushRecoveringServer(wstring guid)
{
	FOREGROUND();
	AUTOLOCK(_csRecover);
	_pendingNewServer.push_back(guid);
}

bool KAppDoc::PopRecoveringServer(CString& guid)
{
	AUTOLOCK(_csRecover);
	FOREGROUND();

	if(_pendingNewServer.size() > 0)
	{
		wstring wguid = _pendingNewServer.front();
		guid = wguid.c_str();
		_pendingNewServer.pop_front();
		return true;
	}
	return false;
}
void KAppDoc::ReqOccured(string ssid)
{
	AUTOLOCK(_csReqs);
	auto tik = KwGetTickCount100Nano();
	SHP<KReqRes> sre = make_shared<KReqRes>(tik, ssid);
	_lstReqs2.push_back(sre);
	/// 나중에 elapsed time 계산 하기 위해
	_mapReq[ssid] = sre;
	//TRACE("ReqOccured %I64u\n", tik);
}

/// <summary>
/// OnSent 때 응답시각 기록
/// </summary>
/// <param name="ssid"></param>
/// <returns>최근 10개 평균을 내서 return</returns>
double KAppDoc::OnResponse(string ssid)
{
	AUTOLOCK(_csReqs);
	//auto tik = GetTickCount64();
	const int gap = 5000000; // usec 측정 간격 = 0.5sec
	const int nsum = 10;//최근거 10개의 평균 간격을 편균 낸다.
	auto tik = KwGetTickCount100Nano();
	SHP<KReqRes> sre;
	if(!_mapReq.Lookup(ssid, sre))//시작 들어있고, 끝
		return 0;
	sre->_tRes = tik;

	if(_lastCheckElapsed > (ULONGLONG)(tik - gap))//간격
		return -1;//최근것이 1초도 안 지났으면 너무 자주 계산 하니 리턴

	//LONGLONG dfElapesed = sre->_tRes - sre->_tReq;
	//TRACE("OnResponse %I64d = %I64u - %I64u \n", dfElapesed, sre->_tReq, sre->_tRes);

	size_t sz = _lstReqs2.size();
	size_t szm = sz > nsum ? nsum : sz;
	ULONGLONG tsumElp = 0;
#ifdef _DEBUGx
	CStringW sa1;
#endif // _DEBUG
	int szma = 0;
	for(size_t i = 0; i < sz; i++)///뒤에 10를 평균 된다.
	{
		auto sre1 = _lstReqs2.back();
		if(sre1->_tRes > 0)
		{
			auto elp = sre1->_tRes - sre1->_tReq;
			tsumElp += elp;
			szma++;
			//_lstReqs2.pop_back();//제거는 GetSpeedPerSec 에서 한다.
#ifdef _DEBUGx
			CStringW sa; sa.Format(L"%I64d+", elp);
			sa1 += sa;
#endif // _DEBUG
			if(szma >= nsum)
				break;
		}
	}
	_lastCheckElapsed = tik;
	double dElapesed = szma == 0 ? 0. : (double)tsumElp / szma;// _mapReq[ssid] - tik;
#ifdef _DEBUGx
	KTrace(L"OnResponse %I64u/%d = %f, %d : %s\n", tsumElp, szma, dElapesed, sz, sa1);
#endif // _DEBUG
	return dElapesed;
}



//#include <boost/range/adaptor/reversed.hpp>
/// <summary>
/// 요청시 최근 10의 요청간격의 평균을 내서 초당 처리 속도를 계산 한다.
/// </summary>
/// <returns>최근 10의 요청간격의 평균</returns>
double KAppDoc::GetSpeedPerSec()
{
	AUTOLOCK(_csReqs);
	const int nsum = 10;//최근거 10개의 평균 간격을 편균 낸다.
	const int gap = 5000000; // usec 측정 간격 = 0.5sec

	auto now = KwGetTickCount100Nano();// 0.1usec = 100 nano sec
	if(_lastCheckSpeed > (ULONGLONG)(now - gap))//간격
		return -1;//최근것이 1초도 안 지났으면 너무 자주 계산 하니 리턴
	_lastCheckSpeed = now;
	int n = 0;
	int x = 0;
	size_t sz = _lstReqs2.size();
/*
	for(int i = 0; i < (int)sz - nsum; i++)// ULONGLONG 형으로 하면 - 값이 큰 양수로 바뀐다.
	{
		auto& sre = _lstReqs2.front();
		_mapReq.RemoveKey(sre->_ssid);
		_lstReqs2.pop_front();//5개간격만 평균
	}
*/
	if(sz > 10)
		_break;
	size_t cnt = sz >= nsum ? nsum : sz;//실제 갯수 5개 이하
	ULONGLONG tsum = 0;
	ULONGLONG tbfr = 0;
#ifdef _DEBUGx
	CStringW sa1;
#endif // _DEBUG
	int cntSum = 0;
	//for(auto sre : _lstReqs2)
	//for(auto i : boost::adaptors::reverse(_lstReqs2))	{	}
	//for(auto&& el : std::reverse(_lstReqs2)) {	}  c++20
// 	for(int i = 0; i < (int)sz; i++)// ULONGLONG 형으로 하면 - 값이 큰 양수로 바뀐다.
	for_reverse(sre, _lstReqs2)
	{
// 		auto& sre = _lstReqs2.back();
		if(tbfr != 0)
		{
			LONGLONG df = tbfr - sre->_tReq;//durtns dlamfh
			tsum += df;//시간 간격을 합산
#ifdef _DEBUGx
			CStringW sa; sa.Format(L"(%I64d:%I64d)+", sre->_tReq, df);
			sa1 += sa;
#endif // _DEBUG
			cntSum++;
			if(cntSum >= 10)
				break;
		}
		tbfr = sre->_tReq;
	}
	double rn = cntSum == 0 ? 0 : (double)tsum / cntSum;// (cnt - 1.);

#ifdef _DEBUGx
	size_t cnt2 = _lstReqs2.size();
	KTrace(L"GetSpeedPerSec %I64u/%d = %f, %d : %s\n", tsum, cnt, rn, sz, sa1);
#endif // _DEBUG
	for(int i = 0; i < (int)sz - 5000; i++)// 5000개 넘으면 지운다.
	{
		auto& sre = _lstReqs2.front();
		_mapReq.RemoveKey(sre->_ssid);
		_lstReqs2.pop_front();//5개간격만 평균
	}

	return rn;
}
void KAppDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	__super::SetPathName(lpszPathName, FALSE);//무조건 FALSE
}


void KAppDoc::Serialize(CArchive& ar)
{
	AUTOLOCK(_csAppDoc);
	const CString& full0 = GetPathName();//load한 파일 이면
	CString full1 = GetFilePath();	//full += L"WinpacheMain.cfg";

	char* ecdkey = "947083839";
	if(ar.IsStoring())
	{
		//JObj js;
		auto& jobj = *_json;// ->AsObject();// .get();

		ASSERT(jobj.Len("GUID"));//		jobj("GUID") = KwGetFormattedGuid(FALSE);
		CStringA guidA(jobj.S("GUID"));
		OSslEncrypt ose((PAS)guidA.Left(24), (PAS)guidA.Right(8));//key, salt

		//jobj("RunningServers") = JObj(); // RunningServers : { GUID1 : { }, GUID2:{} }
		CString PWD = jobj.S("PWD");// 러닝 중인 서버가 없는 경우 제외 하기 위해 임시로 받아 둔다.
		auto rs = jobj.O("RunningServers");
		bool bPendingServer = rs && rs->size() > 0;// && jobj.Len("PWD");
		if(!bPendingServer)
			jobj.DeleteKey("PWD");
		/// else 
		///	pending server가 있는 경우, launch가 자동으로 시스템이 해주는데, 그때 아래 3가지를 하여, 다시 가동 되게 해야 한다.
		///		문서 오픈
		///		DB연결
		///		SSL인경우 비번도 저장 되어 있어야 한다.
		///		Start server

		CFile* fr = ar.GetFile();
		CStringA sUtf8 = jobj.ToJsonStringUtf8();
		if(!bPendingServer && PWD.GetLength() > 0)// 물안전 종료된 서버가 없으면 비번은 저장 않는다.
			jobj("PWD") = PWD;

		int descLen = sUtf8.GetLength() * 2 ;

// 		PUCHAR pu8 = new UCHAR[descLen];
// 		KAtEnd d_pu8([&]() { delete pu8; });
		KBinary bin8(descLen);
		PUCHAR pu8 = (PUCHAR)bin8.GetPA();

		BOOL bec = ose.Encrypt((PUCHAR)(PAS)sUtf8, sUtf8.GetLength(), pu8, &descLen);
		pu8[descLen] = '\0';//binary인데 뒤에 '\0' 넣는거는 넌센스

		if(bec)
		{
			KBinary guidB(guidA);
			KBinary guidBE(guidA.GetLength());
			EncodeBinary(guidB, ecdkey, true, &guidBE);

			fr->Write(guidBE.m_p, guidA.GetLength());
			fr->Write(pu8, descLen);
#ifdef _DEBUG
			CString full2 = full1 + L".json";
			KwFileSafeRemove(full2);
			CStringA json; KwUTF8ToChar(sUtf8, json);
			CFile fj(full2, CFile::modeCreate | CFile::modeWrite);
			fj.Write((PAS)json, json.GetLength());
			fj.Close();
#endif // _DEBUG
		}
		else
			TRACE("Encoding error.");
	}
	else
	{
		int glen = 32;
		CFile* fr = ar.GetFile();
		int lenData = (int)fr->GetLength() - glen;

		KBinary guidBE(glen);
		fr->Read(guidBE.m_p, glen);// guidA.GetBufferSetLength(32), 32);
		KBinary guidB(glen);
		EncodeBinary(guidBE, ecdkey, false, &guidB);

		CStringA guidA(guidB.m_p);//header 32byte
// 		guidA.GetBufferSetLength(32)
// 		guidA.ReleaseBuffer();

		KBinary bUtf8(lenData);
		fr->Read(bUtf8.m_p, lenData);

		OSslEncrypt ose((PAS)guidA.Left(24), (PAS)guidA.Right(8));
		int descLen = lenData * 2;//그냥 넉넉히
		KBinary bin8(descLen);
		PUCHAR pu8 = (PUCHAR)bin8.GetPA();

		bool bError = false;
		BOOL bdc = ose.Decrypt((PUCHAR)bUtf8.m_p, lenData, pu8, &descLen);
		if(!bdc)
			bError = true;
		CString sWstr;
		KwUTF8ToWchar((PAS)pu8, sWstr);

		auto jdoc = ShJVal(Json::Parse((PWS)sWstr));
		if(!jdoc)//.get() == nullptr)
			bError = true;
		_json = jdoc->AsObject();// .get();
		
		if(bError)
			KwMessageBoxError(L"WinpacheMain.cfg 파일 포맷 오류.");

		/*
				auto& js = *jdoc->AsObject();// .get();
				KJSGETS(_GUID);
				if (_GUID.IsEmpty())
					_jdata("_GUID") = KwGetFormattedGuid();
				KJSGETS(_fname);
				KJSGETS(_DSN);
				KJSGETS(_UID);
				//KJSGETS(_PWD);
				KJSGETS(_database);
				KJSGETS(_statDB);
				//KJSGETI(_UID);// 숫자인 경우
		*/
	}
}
