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

// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "CmnView.h"
#include "SrvView.h"
#include "DockPane.h"///?kdw 

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame() noexcept;

// Attributes
public:
	KDockPane _dock;///?kdw 
	CDockFormChild* Dock(UINT nID)
	{
		auto fp = _dock.GetDockView(nID);
		return fp;
	}
	CDockablePane* Pane(UINT nID)
	{
		auto fp = _dock.GetDockPane(nID);
		return fp;
	}


// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CFileView         m_wndFileView;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	CMFCCaptionBar    m_wndCaptionBar;


public:
	void ContextCategory(int idd, bool bShow = true);


// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewFileView();
	afx_msg void OnUpdateViewFileView(CCmdUI* pCmdUI);
	afx_msg void OnViewClassView();
	afx_msg void OnUpdateViewClassView(CCmdUI* pCmdUI);
	afx_msg void OnViewOutputWindow();
	afx_msg void OnUpdateViewOutputWindow(CCmdUI* pCmdUI);
	afx_msg void OnViewPropertiesWindow();
	afx_msg void OnUpdateViewPropertiesWindow(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	BOOL CreateCaptionBar();
public:
	CSrvView* GetActiveCmnView();
	void OnUpdateCmn(CCmdUI* pCmdUI, int idc);
	void CaptionMessage(PWS msg);

public:
	afx_msg void OnClose();
	
	afx_msg void OnRibbonStart();
	afx_msg void OnUpdateStart(CCmdUI* pCmdUI);
	afx_msg void OnRibbonStop();
	afx_msg void OnUpdateStop(CCmdUI* pCmdUI);
	afx_msg void OnRibbonRestart();
	afx_msg void OnUpdateRestart(CCmdUI* pCmdUI);
	afx_msg void OnRibbonStartDB();
	afx_msg void OnUpdateStartDB(CCmdUI* pCmdUI);
	afx_msg void OnFreeLibrary();
	afx_msg void OnDownloadMariaDB();
	afx_msg void OnDownloadMariaODBC();
	afx_msg void OnTestAPI();
	afx_msg void OnRunClient();
	afx_msg void OnViewProperties();
	afx_msg void OnUpdateViewProperties(CCmdUI* pCmdUI);
	afx_msg void OnSiteProject();
	afx_msg void OnOdbcSetting();
	afx_msg void OnVisualStudioDownload();
	afx_msg void OnInstallMariaDB();
	afx_msg void OnInstallMariaODBC();
	afx_msg void OnBuildProject();
	afx_msg void OnCreateDatabase();
};


