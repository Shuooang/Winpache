
// StartWinpacheDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "StartWinpache.h"
#include "StartWinpacheDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStartWinpacheDlg dialog



CStartWinpacheDlg::CStartWinpacheDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STARTWINPACHE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStartWinpacheDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Trace, c_trace);
}

BEGIN_MESSAGE_MAP(CStartWinpacheDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_WinpachePro, &CStartWinpacheDlg::OnBnClickedWinpachepro)
	ON_BN_CLICKED(IDC_Winpache, &CStartWinpacheDlg::OnBnClickedWinpache)
	ON_BN_CLICKED(IDC_Wincatcher, &CStartWinpacheDlg::OnBnClickedWincatcher)
	ON_BN_CLICKED(IDC_GotoBinary, &CStartWinpacheDlg::OnBnClickedGotobinary)
	ON_BN_CLICKED(IDC_GotoProject, &CStartWinpacheDlg::OnBnClickedGotoproject)
	ON_BN_CLICKED(IDC_GotoProgramFiles, &CStartWinpacheDlg::OnBnClickedGotoprogramfiles)
	ON_BN_CLICKED(IDC_OpenTheProject, &CStartWinpacheDlg::OnBnClickedOpentheproject)
END_MESSAGE_MAP()


// CStartWinpacheDlg message handlers

BOOL CStartWinpacheDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStartWinpacheDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CStartWinpacheDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CStartWinpacheDlg::OpenFile(LPCWSTR tdir, LPCWSTR fsln, LPCWSTR desc)
{
	WCHAR my_documents[MAX_PATH];//CSIDL_PERSONAL
	HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, my_documents);
	CStringW flPrj = my_documents;
	//LPCWSTR tdir = L"\\Winpache\\bin\\x64\\Release\\";
	//LPCWSTR fsln = L"WinpachePro.exe";
	flPrj += tdir;
	CString sDir = flPrj;
	flPrj += fsln;
	HINSTANCE hi = ::ShellExecute(0, 0, flPrj, 0, sDir, SW_SHOW);
	if((LONGLONG)hi > 32L)
	{
		//CString s; s.Format(L"Open the Winpache Pro as shareware.", title);
		c_trace.SetWindowText(desc);
	}
	else
	{
		WCHAR curDir[1024];
		GetCurrentDirectory(1022, curDir);
		CString s; s.Format(L"Error occurred!\n\
You can open the '%s' from the [%s%s]. Cur Dir: %s", fsln, my_documents, tdir, curDir);
		c_trace.SetWindowText(s);
	}
}

void CStartWinpacheDlg::OnBnClickedWinpachepro()
{
	LPCWSTR tdir = L"\\Winpache\\bin\\x64\\Release\\";
	LPCWSTR fsln = L"WinpachePro.exe";
	LPCWSTR desc = L"Open the Winpache Pro Server as shareware.";
	OpenFile(tdir, fsln, desc);
}

void CStartWinpacheDlg::OnBnClickedWinpache()
{
	LPCWSTR tdir = L"\\Winpache\\bin\\x64\\Release\\";
	LPCWSTR fsln = L"Winpache.exe";
	LPCWSTR desc = L"Open the Winpache Server as freeware.";
	OpenFile(tdir, fsln, desc);
}


void CStartWinpacheDlg::OnBnClickedWincatcher()
{
	LPCWSTR tdir = L"\\Winpache\\bin\\x64\\Release\\";
	LPCWSTR fsln = L"Wincatcher.exe";
	LPCWSTR desc = L"Open the Wincatcher Clent Tester as freeware.";
	OpenFile(tdir, fsln, desc);
}


void CStartWinpacheDlg::OnBnClickedGotobinary()
{
	LPCWSTR adddir = L"\\Winpache\\bin\\x64\\Release\\";
	LPCWSTR desc = L"Go to the binary directory including the excutable files and DLL files.";
	GotoDirectory(CSIDL_APPDATA, adddir, desc);
}
void CStartWinpacheDlg::GotoDirectory(int csidl, LPCWSTR adddir, LPCWSTR desc)
{
	WCHAR my_documents[MAX_PATH];//CSIDL_PERSONAL
	HRESULT result = SHGetFolderPath(NULL, csidl, NULL, SHGFP_TYPE_CURRENT, my_documents);
	CStringW flPrj = my_documents;
	LPCWSTR tdir = adddir;// L"\\Winpache\\bin\\x64\\Release\\";
	flPrj += tdir;
	HINSTANCE hi = ::ShellExecute(0, 0, flPrj, 0, 0, SW_SHOW);
	if((LONGLONG)hi > 32L)
	{
		c_trace.SetWindowText(desc);
	}
	else
	{
		CString s; s.Format(L"Error occurred!\n\
You can open the directory '%s' on the Explorer.", flPrj);
		c_trace.SetWindowText(s);
	}
}


void CStartWinpacheDlg::OnBnClickedGotoproject()
{
	LPCWSTR adddir = L"\\Winpache\\src\\KHttpsSrv\\";
	LPCWSTR desc = L"Go to the binary directory including the excutable files and DLL files.";
	GotoDirectory(CSIDL_APPDATA, adddir, desc);
}


void CStartWinpacheDlg::OnBnClickedGotoprogramfiles()
{
	LPCWSTR adddir = L"\\Keepspeed\\Winpache\\";
	LPCWSTR desc = L"Go to the Program Files for Winpache.";
	GotoDirectory(CSIDL_PROGRAM_FILES, adddir, desc);
}


void CStartWinpacheDlg::OnBnClickedOpentheproject()
{
	LPCWSTR tdir = L"\\Winpache\\src\\KHttpsSrv\\";
	LPCWSTR fsln = L"KHttpsSrvSite.sln";
	LPCWSTR desc = L"Open the Solution file 'KHttpsSrvSite.sln' for Site API DLL.";
	OpenFile(tdir, fsln, desc);
}
