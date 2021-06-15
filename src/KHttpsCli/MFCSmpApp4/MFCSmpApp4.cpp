// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해
// 추가적으로 제공되는 내용입니다.
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MFCSmpApp4.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MFCSmpApp4.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "SmpDoc.h"
#include "SmpView.h"

#include "KwLib64/ThreadPool.h"
#include "KwLib64/KDebug.h"
#include "KwLib64/tchtool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSmpApp4

BEGIN_MESSAGE_MAP(CSmpApp4, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CSmpApp4::OnAppAbout)
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()
/*

class TStrSet
{
public:
	TStrSet(int sz)
		: _sz(sz) {}
	int _sz{ 1 };
	shared_ptr<char[]> sa;
	shared_ptr<wchar_t[]> sw;
	PAS Pas()
	{
		ASSERT(_sz == sizeof(char));
		return sa.get();
	}
	PWS Pws()
	{
		ASSERT(_sz == sizeof(wchar_t));
		return sw.get();
	}
};

template<typename TCH>
TStrSet tchprecedeTest(const TCH* wcs, int ch)
{
	ASSERT(ch);
	auto i = tchchrx(wcs, ch);
	TStrSet sst(sizeof(TCH));
	if(i >= 0)
	{
		if(sst._sz == 1) //sizeof(TCH) == 1)(char*)operator new( buffer_size_here )
		{
			sst.sa = shared_ptr<char[]>((char*)new char[i + 1]);
			char* p = sst.sa.get();
			p[i] = (TCH)0;
			tchncpy((char*)p, (const char*)wcs, i);
		}
		else
		{
			sst.sw = shared_ptr<wchar_t[]>((wchar_t*)new wchar_t[i + 1]);
			wchar_t* p = sst.sw.get();
			p[i] = (TCH)0;
			tchncpy((wchar_t*)p, (const wchar_t*)wcs, i);
		}
	}
	return sst;
}
template<typename TCH>
TStrSet tchbehindTest(const TCH* wcs, int ch)
{
	ASSERT(ch);
	auto i = tchchrx(wcs, ch);
	auto len = tchlen(wcs);
	TStrSet sst(sizeof(TCH));
	if(i >= 0)
	{
		auto blen = len - i - 1;// 12 - 7 - 1
		if(sst._sz == 1) //sizeof(TCH) == 1)(char*)operator new( buffer_size_here )
		{
			sst.sa = shared_ptr<char[]>((char*)new char[blen + 1]);
			char* p = sst.sa.get();
			p[0] = (TCH)0;
			p[blen] = (TCH)0;
			tchncpy((char*)p, (const char*)(wcs + i + 1), blen); // ch 다음 부터 복사 한다.
		}
		else
		{
			sst.sw = shared_ptr<wchar_t[]>((wchar_t*)new wchar_t[blen + 1]);
			wchar_t* p = sst.sw.get();
			p[0] = (TCH)0;
			p[blen] = (TCH)0;
			tchncpy((wchar_t*)p, (const wchar_t*)(wcs + i + 1), blen);
		}
	}
	return sst;
}*/

// CSmpApp4 생성

CSmpApp4::CSmpApp4() noexcept
{
#ifdef _DEBUG
	CString sw(L"request_post");
	bool bend = tchend((PWS)sw, L"_post");


	TStrSet sst = tchprecede((PWS)sw, '_');
	auto ssrt = &sst.sw;
	CString swBf = sst.Pws();
	auto ssrtr = &(sst.sw);
	CStringA sa("request_post");
	TStrSet ssta = tchprecede((PAS)sa, '_');
	CStringA swBfA = ssta.Pas();

	TStrSet sstaa = tchbehind((PAS)sa, '_');
	CStringA swBfAa = sstaa.Pas();


	TStrSet sstaw = tchbehind((PWS)sw, '_');
	CStringW swBfAw = sstaw.Pws();


	std::map<int, int> mp;
	mp[3] = 100;
	int x = mp[3];
	int x1 = mp[3];
#endif // _DEBUG


	m_bHiColorIcons = TRUE;

	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 애플리케이션을 공용 언어 런타임 지원을 사용하여 빌드한 경우(/clr):
	//     1) 이 추가 설정은 다시 시작 관리자 지원이 제대로 작동하는 데 필요합니다.
	//     2) 프로젝트에서 빌드하려면 System.Windows.Forms에 대한 참조를 추가해야 합니다.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 아래 애플리케이션 ID 문자열을 고유 ID 문자열로 바꾸십시오(권장).
	// 문자열에 대한 서식: CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("PetMe,SMP.SopManagementSystem.NativeApp"));///?kdw

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.

}

// 유일한 CSmpApp4 개체입니다.

CSmpApp4 theApp;


// CSmpApp4 초기화
int CSmpApp4::Run()
{
	int rv = 0;
	try /// 이거 실패 더 미리 MFC안쪽에서 가로 채어 버린다. 아마 앱 전체에서 Dialog, dock mainframe, view 에 모두 WindowProc catch가 안되어 있나 보다.
	{
		try
		{
			rv = CWinAppEx::Run();
		} CATCH_APP;
	}
	catch(KException* e)/// 그리고 여기가 2번째로 오니 여기서 모든 오류 처리 해야 한다.
	{
		CRuntimeClass* rc = this->GetRuntimeClass();
		e->m_strStateNativeOrigin = rc->m_lpszClassName;
		TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTION__, __LINE__);
		AppBase::CallClientException(e);//DockClientBase
		e->Delete();
	}

	return rv;
}

BOOL CSmpApp4::InitInstance()
{
	// 애플리케이션 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다. 
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	/// 소캣 안쓰는 옵션
	//if(!AfxSocketInit())
	//{
	//	AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
	//	return FALSE;
	//}
	/// memory loak check
	///_CrtSetBreakAlloc(25817);// 10797);// 10753);
	///_CrtMemDumpAllObjectsSince(0);
	
	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// RichEdit 컨트롤을 사용하려면 AfxInitRichEdit2()가 있어야 합니다.
	// AfxInitRichEdit2();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("PetMe2"));///?kdw
	LoadStdProfileSettings(0);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.



	/// ]///////////////////////////////////////////////////////


	GdiplusStartupInput gdiplusStartupInput;
	//ULONG_PTR _gdiplusToken;
	GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, NULL);


	auto mp = CMainPool::Pool(4);//FOREGROUND() 할때도 필요 하다.
	mp->AddExceptionTaskCB([&](KException* e)
		{
			AppBase::CallClientException(e);//CMainPool
		});

	g_opTrace = 1;//KTrace level 출력여부 레벨정한다. 초기값으로 1


	



	_login = make_shared<JObj>();
#ifdef DEBUG
	JObj& lg = *_login;
	lg("fUsrID") = L"admin-0001";
	lg("fBizID") = L"biz-0002";
#endif

	/// [///////////////////////////////////////////////////////

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);


	// 애플리케이션의 문서 템플릿을 등록합니다.  문서 템플릿은
	//  문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_SmpApp4TYPE,
		RUNTIME_CLASS(CSmpDoc),
		RUNTIME_CLASS(CChildFrame), // 사용자 지정 MDI 자식 프레임입니다.
		RUNTIME_CLASS(CSmpView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 주 MDI 프레임 창을 만듭니다.
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;


	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 명령줄에 지정된 명령을 디스패치합니다.
	// 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 주 창이 초기화되었으므로 이를 표시하고 업데이트합니다.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

//CString getBizID()
//{
//	return _login->S("fBizID");
//};//로그인 한 사람. HTTP request 마다 사용

CString CSmpApp4::getLoginData(PAS field)
{
	static int s_loginMsg = 0;
	if(!_login->Has(field))//"fUsrID"))
	{
		if(!s_loginMsg)
		{
			s_loginMsg = 1;
			KwMessageBoxError(L"로그인을 먼저 하십시요");
		}
		throw_str("로그인 안함");
	}
	else
		return _login->S(field);
};//로그인 한 사람. HTTP request 마다 사용

int CSmpApp4::ExitInstance()
{
	//TODO: 추가한 추가 리소스를 처리합니다.
	AfxOleTerm(FALSE);
	GdiplusShutdown(_gdiplusToken);
	return CWinAppEx::ExitInstance();
}

// CSmpApp4 메시지 처리기


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CSmpApp4::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CSmpApp4 사용자 지정 로드/저장 방법

void CSmpApp4::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
	
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_Order);
}

void CSmpApp4::LoadCustomState()
{
}

void CSmpApp4::SaveCustomState()
{
}

// CSmpApp4 메시지 처리기



