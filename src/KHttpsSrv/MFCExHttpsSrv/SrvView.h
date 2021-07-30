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
#include "KwLib64/stdecl.h"
#include "KwLib64/HTTPS/HttpClient.h"

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

	SHP<CHttpClient> _httpCl;
	// Attributes
public:
	SrvDoc* GetDocument() const;
	virtual CDocument* GetDoc() override;
	_STitleWidthField* GetArListConf(int* nCols) override;

	void MonitorRequest(SHP<KArray<string>> shar) override;

	void CopyRequest();
	void CopyOutput();
	void OnUpdateCmn(CCmdUI* pCmdUI, int idc);
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
	void UpdateControl(CStringA stat, int iOp = 0) override;
	void UpdateControlFore(CStringA stat, int iOp);

	void EnableCtrl(int idc, int bEnable) override
	{
		// 버튼이 있을때만.
		KwEnableWindow(_vu, idc, bEnable);
	}
	void OTrace(PAS txt, int iOp = 0);
	void OTrace(PWS txt, int iOp = 0);

	void CallbackOnStarted(int vuid) override;
	int CallbackOnStopped(HANDLE hev, int vuid) override;
	int CallbackOnReceived(const void* buffer, size_t size) override;
	int CallbackOnReceivedRequest(KSessionInfo& inf, int vuid, SHP<KBinData> shbin, HTTPResponse& res) override;
	int CallbackOnSent(KSessionInfo& inf, int vuid, size_t sent, size_t pending) override;
	int CallbackCluster(KSessionInfo& inf, shared_ptr<KBinData> shbin) override;

	
	void RecoverServer();

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
	afx_msg void OnSiteStart();
	afx_msg void OnUpdateSiteStart(CCmdUI* pCmdUI);

	afx_msg void OnSiteStop();
	afx_msg void OnUpdateSiteStop(CCmdUI* pCmdUI);
	afx_msg void OnSiteRestart();
	afx_msg void OnUpdateSiteRestart(CCmdUI* pCmdUI);
	afx_msg void OnConnectSiteDB();
	afx_msg void OnUpdateConnectSiteDB(CCmdUI* pCmdUI);
	afx_msg void OnBnClickedParallel();
};

#ifndef _DEBUG  // debug version in SrvView.cpp
inline SrvDoc* CSrvView::GetDocument() const
   { return reinterpret_cast<SrvDoc*>(m_pDocument); }
#endif

