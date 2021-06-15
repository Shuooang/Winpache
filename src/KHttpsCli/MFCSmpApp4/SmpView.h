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

// SmpView.h: CSmpView 클래스의 인터페이스
//

#pragma once

#include "TimeTable.h"///?kdw
#include "KwLib64/DlgTool.h"

class CSmpView : public CSafeViewBase
{
protected: // serialization에서만 만들어집니다.
	CSmpView() noexcept;
	DECLARE_DYNCREATE(CSmpView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_MFCSMPAPP4_FORM };
#endif

// 특성입니다.
public:
	CSmpDoc* GetDocument() const;

// 작업입니다.
public:
	
	TimeTable _tt;///?kdw
	void RM_GetTodaySchdule(ShJObj sjo = nullptr, int iOp = 0);
	void RM_UpdateTodaySchedule(ShJObj sjo = nullptr, int iOp = 0);
	void RM_DeleteTodaySchedule(ShJObj sjo = nullptr, int iOp = 0);
	void RM_AddTodaySchedule(ShJObj sjo = nullptr, int iOp = 0);
	void RM_AddTodayOrder(ShJObj sjo = nullptr, int iOp = 0);
	void RM_SelectOrderSchedule(ShJObj sjo = nullptr, int iOp = 0);
	void RM_SelectBizSchedule(ShJObj sjo = nullptr, int iOp = 0);


	void RefreshScheduleDetail();
	void DeleteSchedule();

	CWnd* _wCapture{ nullptr };
// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC); ///?kdw // 이 TimeTable 를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CSmpView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:
	//Right Button Cliced saved
	UINT _nFlagsR{ 0 };
	CPoint _scposRBtn{ -1,-1 };

	void OnBoxSelected(bool bDblClk = false);
	/// scroll 상황을 고려한 View 한 현위치
	void GetScrolledPos(LPPOINT pt);
	bool IsRBtnOnBoxClicked();


// 생성된 메시지 맵 함수
protected:
	afx_msg	void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedReadTimeTable();
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedClear();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);



	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnDelSchedule();
	afx_msg void OnAddOrder();
	afx_msg void OnAddSchedule();
	afx_msg void OnUpdateAddOrder(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddSchedule(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelSchedule(CCmdUI* pCmdUI);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSliderZoom();
	afx_msg void OnAlwaysSyncSD();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // SmpView.cpp의 디버그 버전
inline CSmpDoc* CSmpView::GetDocument() const
   { return reinterpret_cast<CSmpDoc*>(m_pDocument); }
#endif

