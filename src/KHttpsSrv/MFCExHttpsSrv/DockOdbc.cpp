// DockOdbc.cpp : implementation file
//

#include "pch.h"
#include "MFCExHttpsSrv.h"
#include "DockOdbc.h"
#include "Mainfrm.h"
#include "resource.h"
#include "Response1.h"
#include "ApiBase.h"

IMPLEMENT_DYNCREATE(DockOdbcPane, CPaneForm)

// DockOdbc

IMPLEMENT_DYNCREATE(DockOdbc, DockClientBase)
DockOdbc* DockOdbc::s_me = NULL;

DockOdbc::DockOdbc(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn)
{
	s_me = this;

}

DockOdbc::~DockOdbc()
{
}

void DockOdbc::DoDataExchange(CDataExchange* pDX)
{
	DockClientBase::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_UUD, _UID);
	DDX_Text(pDX, IDC_PWD, _PWD);
	DDX_Text(pDX, IDC_DSN, _DSN);
}

BEGIN_MESSAGE_MAP(DockOdbc, DockClientBase)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_BN_CLICKED(IDC_CreateSimple2, &DockOdbc::OnBnClickedCreatesimple2)
	ON_BN_CLICKED(IDC_ConnectDB, &DockOdbc::OnBnClickedConnectDB)
END_MESSAGE_MAP()


// DockOdbc diagnostics

#ifdef _DEBUG
void DockOdbc::AssertValid() const
{
	DockClientBase::AssertValid();
}

#ifndef _WIN32_WCE
void DockOdbc::Dump(CDumpContext& dc) const
{
	DockClientBase::Dump(dc);
}
#endif
#endif //_DEBUG


// DockOdbc message handlers
void DockOdbc::OnInitialUpdate()
{
	if (m_bInited)
		return;
	__super::OnInitialUpdate();
	m_bInited = TRUE;



	AdjustLayout();
	UpdateData(0);
}

void DockOdbc::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rc, rcL, rcP;
	GetClientRect(rc);
}


void DockOdbc::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	AdjustLayout();
}

void DockOdbc::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);
	//_cProperty.SetFocus();
}


//?deprecated : see CMainFrame::InitOdbc(3)
void DockOdbc::OnBnClickedCreatesimple()
{
	/*UpdateData();
	KStrMap kmap;
	try
	{
		kmap["UID"] = ToAStr((PWS)_UID);
		if (_PWD.GetLength() > 0)
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

void DockOdbc::OnBnClickedCreatesimple2()
{
	auto frm = (CMainFrame*)AfxGetMainWnd();
	frm->OnOdbcSetting();
}


void DockOdbc::OnBnClickedConnectDB()
{
	try
	{
		UpdateData();
		if(_DSN.IsEmpty() || _UID.IsEmpty() || _PWD.IsEmpty())
		{
			CString smsg; smsg.Format(L"Error: Each value cannot be empty.");
			KwMessageBoxError(smsg);
			return;
		}
		//CString dsn; dsn.Format(L"DSN=%s;UID=%s;PWD=%s", _DSN, _UID, _PWD);//이거는 모든 필수 항목이 다 들어가 있고.
		auto frm = (CMainFrame*)AfxGetMainWnd();
		auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;
		//auto& jobj = *appd._json;
		AUTOLOCK(appd._csAppDoc);
		auto& jOdbc = *appd._json->OMake("ODBC");



// 		if(appd._dbMain->IsOpen())
// 			appd._dbMain->Close();
// 		appd._dbMain->OpenEx(dsn, CDatabase::noOdbcDialog);

		jOdbc("DSN") = _DSN;
		jOdbc("UID") = _UID;
		if(_PWD.GetLength())
			jOdbc("PWD") = L"**********";

		CString dsn = appd.MakeDsnString();
		auto sdb = KDatabase::getDbConnected((PWS)dsn);

		//appd._dbMain->ExecuteSQL(L"use `winpache`");
		appd.SaveData();
		PWS s = L"Connected to ODBC as the main DB to log transactions..";
		frm->CaptionMessage(s);
		KwMessageBox(s);
	}
	catch (CDBException* e)
	{
		CString smsg; smsg.Format(L"Error! %s, %s.", e->m_strError, e->m_strStateNativeOrigin);
		KwMessageBoxError(smsg);
	}
	catch (CException* e)
	{
		CString serr;
		e->GetErrorMessage(serr.GetBuffer(1024), 1024); serr.ReleaseBuffer();
		CString smsg; smsg.Format(L"Error: %s.", serr);
		KwMessageBoxError(smsg);
	}

}
