// DlgOdbcSetting.cpp : implementation file
//

#include "pch.h"
#include "afxdialogex.h"

#include "MFCExHttpsSrv.h"
#include "DlgOdbcSetting.h"
#include "MainFrm.h"


// DlgOdbcSetting dialog 

IMPLEMENT_DYNAMIC(DlgOdbcSetting, CDialogEx)

DlgOdbcSetting::DlgOdbcSetting(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OdbcSetting, pParent)
	, _UID(_T("root"))
	, _PWD(_T(""))
	, _DSN(L"Winpache")
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
	DDX_Text(pDX, IDC_DSN, _DSN);
}


BEGIN_MESSAGE_MAP(DlgOdbcSetting, CDialogEx)
	ON_BN_CLICKED(IDC_Close, &DlgOdbcSetting::OnBnClickedClose)
// 	ON_BN_CLICKED(IDC_CreateSimple, &DlgOdbcSetting::OnBnClickedCreatesimple)
ON_BN_CLICKED(IDC_OdbcSetting, &DlgOdbcSetting::OnBnClickedOdbcsetting)
END_MESSAGE_MAP()


// DlgOdbcSetting message handlers


void DlgOdbcSetting::OnBnClickedClose()
{
	OnOK();
}


void DlgOdbcSetting::OnBnClickedCreatesimple()
{
	/*UpdateData();
	KWStrMap kmap;
	try
	{
		kmap[L"UID"] = (PWS)_UID;
		if(_PWD.GetLength() > 0)
			kmap["PWD"] = ToAStr((PWS)_PWD);
		int rv = KDatabase::RegODBCMySQL((_DSN), kmap);
		if (rv == 0)
		{
			KwMessageBox(L"ODBC DSN [Winpache] is set.");
			//여기서 연결 해야 한다. 임시로 라도.
			// 그래야 계속 create database, tables를 하지
		}
	}
	catch (CDBException* e)
	{
		KwMessageBox(L"DB Fail to create ODBC DSN [Winpache].");
		e->Delete();
	}
	catch (CException* e)
	{
		KwMessageBox(L"Fail to create ODBC DSN [Winpache] by unknown error.");
		e->Delete();
	}*/
}



void DlgOdbcSetting::OnBnClickedOdbcsetting()
{
// 	auto frm = (CMainFrame*)AfxGetMainWnd();
// 	frm->OnOdbcSetting();
	WCHAR my_documents[MAX_PATH]{0,};
	::GetSystemDirectory(my_documents, MAX_PATH);//위랑 결과가 같다.
	CStringW flPrj = my_documents;
	flPrj += L"\\odbcad32.exe";
	::ShellExecute(0, 0, flPrj, 0, 0, SW_SHOW);
}
