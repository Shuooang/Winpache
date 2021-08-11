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

	///Ű�� �ƴϰ� ���� ã�´�.
	auto it = std::min_element(mymap.begin(), mymap.end(),
		[&](const auto& p1, const auto& p2)
		{// p1�� �� �����
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

// AfxGetApp()�� BACKGROUND���� ���� ����.
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
	SetRegistryKey(_T("Keepspeed"));/// �̰� SOFTWARE �ٷ� �ؿ� ����� ���̴�. WinpachePro_k"));//?zzz// Local AppWizard - Generated Applications"));
	/// �̷��� �ָ� �ȵȴ�. free �ϰ� _tcsdup�ؼ� �Ҵ� �ؾ� ���������� free�Ҷ� ���״´�.
	///m_pszProfileName = L"WinpachePro";
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup(L"Winpache");// m_pszAppName);�� ��� �ʱ�ȭ �Ǵ��� ���� ����.

	g_opTrace |= 1 << 0;

	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

	/// NUMBER_OF_PROCESSORS �ý��� ������ CPU����
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
	// b1 == FALSE : ����
	CMainPool::Pool(si.dwNumberOfProcessors * 2);//?zzz
	///  4 * 2 �ε�, 14�� ��� �Ǵ°� ����, ���������� 5��, UI 1�� ���ΰ� ����.

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);



	_docApp.LoadData();
	/// ���⼭ â�� �߱� ���� LoadCount �� 1�̸� �ʱ�ȭ â�߿� ����� �����.

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
	//�̷��� �ؾ� Mainframe â�� �� �Ŀ�  view �߱� ���� BeginInvoke�� DB���� ������ �ذ� �Ѵ�.
	//�׸��� ���� ���� ���� �ϸ�, �׶� app->OnFileNew() �� �ҷ�

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
/// �̰� ó�� �����Ҷ��� ��� ������ �ٸ���.
/// </summary>
/// <param name="bFirst">ó�� ���ට�� true�̰� �� �� ��닚�� �׻� 
///		false�� default ���̴�.
/// </param>
/// <returns>ODBC OpenEx�� ���ῡ ���� DSN���ڿ�</returns>
CString KAppDoc::MakeDsnString(bool bFirst)//false
{
	AUTOLOCK(_csAppDoc);
	auto& jOdbc = *_json->O("ODBC", true);
	CString dsnLog;
	dsnLog.Format(L"DSN=%s;UID=%s;PWD=%s"
		, jOdbc.S("DSN")
		, jOdbc.S("UID")
		, jOdbc.S("PWD")
	);//PWD�� �ִ°� ODBC�� Ȯ�� ������, DSN������ �α��� �õ�

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
	jobj("LoadCount") = 0;// �̰ŷ� bFirst �� �Ǻ�
	jobj("LastSpeed") = 0.;//�д� ó�� Ƚ��: �ֱ� 5�� �ȿ� ������ ��û�� ���
	jobj("Elapsed") = 0.;//��� ó�� �ӵ�: ��û ���ͼ� ������ �ð����� ��� (�ջ�/ī����)
	jobj("StatDB") = L"none";

	jobj("RunningServers") = JObj(); // RunningServers : { GUID1 : { }, GUID2:{} }
	jobj("ODBC") = JObj(); // RunningServers : { GUID1 : { }, GUID2:{} }

	//auto sjo = jobj.O("ODBC");
	auto& jOdbc = *jobj.O("ODBC");

	jOdbc("DSN") = L"Winpache";
	jOdbc("UID") = L"root";
	jOdbc("PWD") = L"";//�ִ°�� ���� DB ���
	jOdbc("DATABASE") = L"winpache";
	jOdbc("SERVER") = L"localhost";
	jOdbc("Driver") = L"MariaDB ODBC 3.1 Driver";
	jOdbc("PORT") = L"3306";
	jOdbc("TCPIP") = L"1";

	/// KDatabase::RegGetODBCMySQL �� �о� ���� bnmnnm,
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
				// ���� Start ���� ���� ����
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
		{//���� ������ ������ ������
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
#ifdef _DEBUGx //KwGetFullPathName test : ���� ���� ���ο� ��� ���� ��� ����ȭ ��
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
		/// memory�� pwd�� ������ �ִٰ�. 
		/// SaveData�Ҷ� running���� ������ ������ pwd�� ���� �ϰ�, ������ ���� �Ѵ�.
		///		���� ��� ������ ���������� Stop�� ���, Winpache�� launch�ɶ� ���� ����� �����.
		///		����� ODBC registry�� ���ԵǾ� �ִٸ� �װ� �о ��� �ȹ������� �ִ�.
		/// SaveData�� �����Ҷ�, ���� ����Ǿ�����, ���� Start������, �� 5�и��� ȣ�� �ȴ�.
		/// ���� ������ ������ Start ���� ����� 5���� �ȵǾ �ٽ� �ο�ġ �Ҷ���, 
		///	�ڵ� ���� ����, Site DB ����, Start�� �Ǿ�� �Ѵ�.
		const CString& full0 = GetPathName();//load�� ���� �̸�
		if(full0.GetLength() > 0)
			VERIFY(DoFileSave()); // �״�� �����.
		else // ������ ó��
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


/// site server start �Ȱ��� ���� ������ ���� �Ѵ�. 
/// �������� ���� ��ġ�� ���� ���� �ڵ� ���� �ϵ���

void KAppDoc::RegisterServerStart(ShJObj sjsvr)//?server recover 1.2
{
	BACKGROUND(1);
	AUTOLOCK(_csAppDoc);
	//AUTOLOCK(_csRecover);
// 	auto lc = sjsvr->I("LoadCount");
// 	(*sjsvr)("LoadCount") = lc + 1;//�ٽ� �õ� �Ǹ� 2
	(*sjsvr)("_bRecover") = TRUE;//�ϴ� RunningServers���� TRUE�� ����
	(*sjsvr)("_tLastRunning") = KwGetCurrentTimeFullString();

	auto& jobjM = *_json;
	auto oRS = jobjM.O("RunningServers");
	CStringW guid = sjsvr->S("_GUID");
	oRS->DeleteKey(guid);//����� ������ ���࿡ �ִٸ� ���ش�.
	(*oRS)(guid) = sjsvr;// ����Ʈ���� ��ü�� �־� �д�.
						 //jobjM("RunningServers") = oRS; clone�� �ƴϴϱ� �ٽ� ���� �ʿ� ����.
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
	/// ���߿� elapsed time ��� �ϱ� ����
	_mapReq[ssid] = sre;
	//TRACE("ReqOccured %I64u\n", tik);
}

/// <summary>
/// OnSent �� ����ð� ���
/// </summary>
/// <param name="ssid"></param>
/// <returns>�ֱ� 10�� ����� ���� return</returns>
double KAppDoc::OnResponse(string ssid)
{
	AUTOLOCK(_csReqs);
	//auto tik = GetTickCount64();
	const int gap = 5000000; // usec ���� ���� = 0.5sec
	const int nsum = 10;//�ֱٰ� 10���� ��� ������ ��� ����.
	auto tik = KwGetTickCount100Nano();
	SHP<KReqRes> sre;
	if(!_mapReq.Lookup(ssid, sre))//���� ����ְ�, ��
		return 0;
	sre->_tRes = tik;

	if(_lastCheckElapsed > (ULONGLONG)(tik - gap))//����
		return -1;//�ֱٰ��� 1�ʵ� �� �������� �ʹ� ���� ��� �ϴ� ����

	//LONGLONG dfElapesed = sre->_tRes - sre->_tReq;
	//TRACE("OnResponse %I64d = %I64u - %I64u \n", dfElapesed, sre->_tReq, sre->_tRes);

	size_t sz = _lstReqs2.size();
	size_t szm = sz > nsum ? nsum : sz;
	ULONGLONG tsumElp = 0;
#ifdef _DEBUGx
	CStringW sa1;
#endif // _DEBUG
	int szma = 0;
	for(size_t i = 0; i < sz; i++)///�ڿ� 10�� ��� �ȴ�.
	{
		auto sre1 = _lstReqs2.back();
		if(sre1->_tRes > 0)
		{
			auto elp = sre1->_tRes - sre1->_tReq;
			tsumElp += elp;
			szma++;
			//_lstReqs2.pop_back();//���Ŵ� GetSpeedPerSec ���� �Ѵ�.
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
/// ��û�� �ֱ� 10�� ��û������ ����� ���� �ʴ� ó�� �ӵ��� ��� �Ѵ�.
/// </summary>
/// <returns>�ֱ� 10�� ��û������ ���</returns>
double KAppDoc::GetSpeedPerSec()
{
	AUTOLOCK(_csReqs);
	const int nsum = 10;//�ֱٰ� 10���� ��� ������ ��� ����.
	const int gap = 5000000; // usec ���� ���� = 0.5sec

	auto now = KwGetTickCount100Nano();// 0.1usec = 100 nano sec
	if(_lastCheckSpeed > (ULONGLONG)(now - gap))//����
		return -1;//�ֱٰ��� 1�ʵ� �� �������� �ʹ� ���� ��� �ϴ� ����
	_lastCheckSpeed = now;
	int n = 0;
	int x = 0;
	size_t sz = _lstReqs2.size();
/*
	for(int i = 0; i < (int)sz - nsum; i++)// ULONGLONG ������ �ϸ� - ���� ū ����� �ٲ��.
	{
		auto& sre = _lstReqs2.front();
		_mapReq.RemoveKey(sre->_ssid);
		_lstReqs2.pop_front();//5�����ݸ� ���
	}
*/
	if(sz > 10)
		_break;
	size_t cnt = sz >= nsum ? nsum : sz;//���� ���� 5�� ����
	ULONGLONG tsum = 0;
	ULONGLONG tbfr = 0;
#ifdef _DEBUGx
	CStringW sa1;
#endif // _DEBUG
	int cntSum = 0;
	//for(auto sre : _lstReqs2)
	//for(auto i : boost::adaptors::reverse(_lstReqs2))	{	}
	//for(auto&& el : std::reverse(_lstReqs2)) {	}  c++20
// 	for(int i = 0; i < (int)sz; i++)// ULONGLONG ������ �ϸ� - ���� ū ����� �ٲ��.
	for_reverse(sre, _lstReqs2)
	{
// 		auto& sre = _lstReqs2.back();
		if(tbfr != 0)
		{
			LONGLONG df = tbfr - sre->_tReq;//durtns dlamfh
			tsum += df;//�ð� ������ �ջ�
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
	for(int i = 0; i < (int)sz - 5000; i++)// 5000�� ������ �����.
	{
		auto& sre = _lstReqs2.front();
		_mapReq.RemoveKey(sre->_ssid);
		_lstReqs2.pop_front();//5�����ݸ� ���
	}

	return rn;
}
void KAppDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU)
{
	__super::SetPathName(lpszPathName, FALSE);//������ FALSE
}


void KAppDoc::Serialize(CArchive& ar)
{
	AUTOLOCK(_csAppDoc);
	const CString& full0 = GetPathName();//load�� ���� �̸�
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
		CString PWD = jobj.S("PWD");// ���� ���� ������ ���� ��� ���� �ϱ� ���� �ӽ÷� �޾� �д�.
		auto rs = jobj.O("RunningServers");
		bool bPendingServer = rs && rs->size() > 0;// && jobj.Len("PWD");
		if(!bPendingServer)
			jobj.DeleteKey("PWD");
		/// else 
		///	pending server�� �ִ� ���, launch�� �ڵ����� �ý����� ���ִµ�, �׶� �Ʒ� 3������ �Ͽ�, �ٽ� ���� �ǰ� �ؾ� �Ѵ�.
		///		���� ����
		///		DB����
		///		SSL�ΰ�� ����� ���� �Ǿ� �־�� �Ѵ�.
		///		Start server

		CFile* fr = ar.GetFile();
		CStringA sUtf8 = jobj.ToJsonStringUtf8();
		if(!bPendingServer && PWD.GetLength() > 0)// ������ ����� ������ ������ ����� ���� �ʴ´�.
			jobj("PWD") = PWD;

		int descLen = sUtf8.GetLength() * 2 ;

// 		PUCHAR pu8 = new UCHAR[descLen];
// 		KAtEnd d_pu8([&]() { delete pu8; });
		KBinary bin8(descLen);
		PUCHAR pu8 = (PUCHAR)bin8.GetPA();

		BOOL bec = ose.Encrypt((PUCHAR)(PAS)sUtf8, sUtf8.GetLength(), pu8, &descLen);
		pu8[descLen] = '\0';//binary�ε� �ڿ� '\0' �ִ°Ŵ� �ͼ���

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
		int descLen = lenData * 2;//�׳� �˳���
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
			KwMessageBoxError(L"WinpacheMain.cfg ���� ���� ����.");

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
				//KJSGETI(_UID);// ������ ���
		*/
	}
}
