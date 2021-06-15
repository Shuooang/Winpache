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

// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once
#include "FileView.h"
#include "ClassView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"

#include "AppBase.h"
#include "DockPane.h"///?kdw 

class CMainFrame : public CMDIFrameWndExInvokable
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame() noexcept;

// 특성입니다.
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

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CMFCRibbonBar     m_wndRibbonBar;

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	//CFileView         m_wndFileView;
	CClassView        m_wndClassView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;

public:
	void ContextCategory(int idd, bool bShow = true);

// 생성된 메시지 맵 함수
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
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	afx_msg void OnViewDockCustomDetail(); //?dock everytime:
	afx_msg void OnUpdateViewDockCustomDetail(CCmdUI* pCmdUI); //?dock everytime:
	afx_msg void OnViewDockCustomHistory();
	afx_msg void OnUpdateViewDockCustomHistory(CCmdUI* pCmdUI);
	afx_msg void OnViewDockMyHistory();
	afx_msg void OnUpdateViewDockMyHistory(CCmdUI* pCmdUI);
	afx_msg void OnViewDockMyPetList();
	afx_msg void OnUpdateViewDockMyPetList(CCmdUI* pCmdUI);
	afx_msg void OnViewDockPetDetail();
	afx_msg void OnUpdateViewDockPetDetail(CCmdUI* pCmdUI);
	afx_msg void OnViewDockMyPetChecks();
	afx_msg void OnUpdateViewDockMyPetChecks(CCmdUI* pCmdUI);


	afx_msg void OnViewDockScheduleDetail();
	afx_msg void OnUpdateViewDockScheduleDetail(CCmdUI* pCmdUI);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnViewDockOrder();
	afx_msg void OnViewDockProducts();
	afx_msg void OnViewDockOptions();
	afx_msg void OnUpdateViewDockOrder(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewDockProducts(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewDockOptions(CCmdUI* pCmdUI);
};


