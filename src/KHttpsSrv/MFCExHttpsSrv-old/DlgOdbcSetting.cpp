// DlgOdbcSetting.cpp : implementation file
//

#include "pch.h"
#include "MFCExHttpsSrv.h"
#include "DlgOdbcSetting.h"
#include "afxdialogex.h"


// DlgOdbcSetting dialog

IMPLEMENT_DYNAMIC(DlgOdbcSetting, CDialogEx)

DlgOdbcSetting::DlgOdbcSetting(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, _UID(_T("root"))
	, _PWD(_T(""))
{

}

DlgOdbcSetting::~DlgOdbcSetting()
{
}

void DlgOdbcSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_UUD, _UID);
	DDX_Text(pDX, IDC_PWD, _PWD);
}


BEGIN_MESSAGE_MAP(DlgOdbcSetting, CDialogEx)
	ON_BN_CLICKED(IDC_Close, &DlgOdbcSetting::OnBnClickedClose)
	ON_BN_CLICKED(IDC_CreateSimple, &DlgOdbcSetting::OnBnClickedCreatesimple)
	ON_BN_CLICKED(IDC_CreateSimple2, &DlgOdbcSetting::OnBnClickedCreatesimple2)
END_MESSAGE_MAP()


// DlgOdbcSetting message handlers


void DlgOdbcSetting::OnBnClickedClose()
{
	OnOK();
}


void DlgOdbcSetting::OnBnClickedCreatesimple()
{
	KStrMap kmap;
	kmap["SERVER"] =
		int rv = KDatabase::RegODBCMySQL(L"Winpache", KStrMap & kmap)// LPCTSTR sServer, LPCTSTR sDriver, LPCTSTR sDatabase, PS sPort)
	// TODO: Add your control notification handler code here
}


void DlgOdbcSetting::OnBnClickedCreatesimple2()
{
	auto frm = (CMainFrame*)AfxGetMainWnd();
	frm->OnOdbcSetting();
}
