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

// SrvView.h : interface of the CSrvView class
//

#pragma once

#include "KwLib64/DlgTool.h"
#include "KwLib64/HTTPS/HttpSvr.h"
#include "KwLib64/UdpSvr.h"

#include "HttpsSvr.h"
#include "HttpSvr.h"
#include "CmnView.h"

class SrvDoc;

class CSrvView 
	: public CFormInvokable
	, public CmnView
{
protected: // create from serialization only
	CSrvView() noexcept;
	DECLARE_DYNCREATE(CSrvView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_MFCEXHTTPSSRV_FORM };
#endif
	CDockablePane* _output{ nullptr };

	// Attributes
public:
	SrvDoc* GetDocument() const;
	virtual CDocument* GetDoc() override;
	_STitleWidthField* GetArListConf(int* nCols) override;

	void MonitorRequest(shared_ptr<KArray<string>> shar) override;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CSrvView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	void EnableCtrl(int idc, int bEnable) override
	{
		// 버튼이 있을때만.
		KwEnableWindow(_vu, idc, bEnable);
	}
protected:
	CListCtrl _cMonitor;

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnBnClickedBtnpath();
	afx_msg void OnDestroy();

	//afx_msg void OnBnClickedbssl();
	afx_msg void OnBnClickedBtnimalocal();
	afx_msg void OnImageMigration();
	afx_msg void OnMigrationImageSizeAdjust();
	afx_msg void OnBnClickedTestapi();
	afx_msg void OnBnClickedStartUDP();

	afx_msg void OnBnClickedBtnUploadLocal();
	afx_msg void OnBnClickedBtnSslSetting();
	afx_msg	void OnClose();

public://리본메뉴에서 부른다.
	afx_msg	void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedRestart();
	afx_msg void OnBnClickedStartDB();
	//afx_msg void OnFreeLibrary();

};

#ifndef _DEBUG  // debug version in SrvView.cpp
inline SrvDoc* CSrvView::GetDocument() const
   { return reinterpret_cast<SrvDoc*>(m_pDocument); }
#endif

