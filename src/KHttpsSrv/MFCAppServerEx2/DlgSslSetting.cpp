// DlgSslSetting.cpp : implementation file
//

#include "pch.h"
#include "resource.h"
#include "DlgSslSetting.h"
#include "afxdialogex.h"
#include "MFCAppServerEx2Doc.h"
#include "KwLib64/DlgTool.h"
#include "KwLib64/Kw_tool.h"

// DlgSslSetting dialog

IMPLEMENT_DYNAMIC(DlgSslSetting, CDialogEx)

DlgSslSetting::DlgSslSetting(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SslSetting, pParent)
{

}

DlgSslSetting::~DlgSslSetting()
{
}

void DlgSslSetting::DoDataExchange(CDataExchange* pDX)
{
	auto doc = (CMFCAppServerEx2Doc*)_doc;
	ASSERT(doc);
	CDialogEx::DoDataExchange(pDX);
	KDDX_TextA(_certificate);
	KDDX_TextA(_privatekey);
	KDDX_TextA(_dhparam);
	KDDX_TextA(_prvpwd);
	DDX_Control(pDX, IDC_certificate, c_certificate);
	DDX_Control(pDX, IDC_privatekey, c_privatekey);
	DDX_Control(pDX, IDC_dhparam, c_dhparam);
	DDX_Control(pDX, IDC_prvpwd, c_prvpwd);
}


BEGIN_MESSAGE_MAP(DlgSslSetting, CDialogEx)
	ON_BN_CLICKED(IDC_BtnCertificate, &DlgSslSetting::OnBnClickedBtncertificate)
	ON_BN_CLICKED(IDC_BtnPrivate, &DlgSslSetting::OnBnClickedBtnprivate)
	ON_BN_CLICKED(IDC_BtnDhparam, &DlgSslSetting::OnBnClickedBtndhparam)
	ON_BN_CLICKED(IDOK, &DlgSslSetting::OnBnClickedOk)
END_MESSAGE_MAP()


BOOL DlgSslSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	UpdateData(0); // _doc 의 데이터를 화면에 출력.

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

// DlgSslSetting message handlers




void DlgSslSetting::OnBnClickedBtncertificate()
{
	auto doc = (CMFCAppServerEx2Doc*)_doc;
	CString fname;
	if(KwOpenFileOpenDlg(GetSafeHwnd(), fname, L"*.pem"))
	{
		doc->_certificate = fname;
		UpdateData(0);
	}
}


void DlgSslSetting::OnBnClickedBtnprivate()
{
	auto doc = (CMFCAppServerEx2Doc*)_doc;
	CString fname;
	if(KwOpenFileOpenDlg(GetSafeHwnd(), fname, L"*.pem"))
	{
		doc->_privatekey = fname;
		UpdateData(0);
	}
}


void DlgSslSetting::OnBnClickedBtndhparam()
{
	auto doc = (CMFCAppServerEx2Doc*)_doc;
	CString fname;
	if(KwOpenFileOpenDlg(GetSafeHwnd(), fname, L"*.pem"))
	{
		doc->_dhparam = fname;
		UpdateData(0);
	}
}


void DlgSslSetting::OnBnClickedOk()
{
// 	KDDX_TextA(_certificate);
// 	KDDX_TextA(_privatekey);
// 	KDDX_TextA(_dhparam);
// 	KDDX_TextA(_prvpwd);
#define GETDLGTEXT1(key)	{CString s##key;	GetDlgItemText(IDC##key, s##key);\
if(!KwIfFileExist(s##key))\
	{KwMessageBox(L"File \"%s\" is not found!", s##key);\
	return;}\
}
	GETDLGTEXT1(_certificate);
	GETDLGTEXT1(_privatekey);
	GETDLGTEXT1(_dhparam);
	
	UpdateData(); // _doc 의 데이터로 들어 간다.
	CDialogEx::OnOK();
}

