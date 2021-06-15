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

// MFCExHttpsSrv.h : main header file for the MFCExHttpsSrv application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "KwLib64/DlgTool.h"//KCheckWnd
// CMFCExHttpsSrvApp:
// See MFCExHttpsSrv.cpp for the implementation of this class
//
class KAppDoc : public CDocument
{
public:
	DECLARE_DYNCREATE(KAppDoc)
	KAppDoc() noexcept;
	virtual ~KAppDoc();
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	void LoadData();
	CString GetFilePath();
	void SaveData();
	CString _GUID;
	PWS _fname{ L"Winpache.json" };
	CString _DSN{ L"Winpache" };
	CString _UID{ L"root" };
	CString _PWD;
	CString _database{ L"winpache" };
	CStringA _statDB{ "none" };// "MariaDB", "ODBC", "conected"
	shared_ptr<KDatabase> _dbMain{ std::make_shared<KDatabase>() };//이래야 _dbLog 와 share할수 있음

};
class CMFCExHttpsSrvApp 
	: public CWinAppEx
	, public KCheckWnd
//	, public KAppDoc ON_COMMAND(...)에서 error C2594: 'static_cast': ambiguous conversions from 'void (__cdecl CMFCExHttpsSrvApp::* )(void)' to 'AFX_PMSG'
// 나므로  CDocument를 App과 합치면 안되겠다.
{
public:
	CMFCExHttpsSrvApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();


// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	KAppDoc _doc;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCExHttpsSrvApp theApp;
