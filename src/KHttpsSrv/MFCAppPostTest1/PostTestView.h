
// PostTestView.h: CPostTestView 클래스의 인터페이스
//

#pragma once
#include "KwLib64/DlgTool.h"


class CPostTestView : public CFormInvokable
{
protected: // serialization에서만 만들어집니다.
	CPostTestView() noexcept;
	DECLARE_DYNCREATE(CPostTestView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_MFCAPPPOSTTEST1_FORM };
#endif

// 특성입니다.
public:
	CPostTestDoc* GetDocument() const;

	ULONGLONG _start = GetTickCount64();

	CCriticalSection _csi;
	ULONG _iSend;

	LONG _nResponse;
	int _nSent;
	bool _bStop{ false };
	CEditCtrlCV c_URL;
	CEditCtrlCV c_Request;
	CEditCtrlCV c_Response;
	CEditCtrlCV c_Count;
	CEditCtrlCV c_Delay;
	CEditCtrlCV c_Thread;
	CEditCtrlCV c_Parallel;

// 작업입니다.
public:
	void SendSync();
	void SendAsync();
	void AddUrlParams(CString& surl);
	void SendOneAsync(int i);
	void UiOutput(int i, PAS sAstrR);
	void SetCount(int i);
	void SendOneSyncImage(int i, PWS fname, PWS cntType);
	void SetElapsed();
	void MonitorRequest(CStringA url);
	void UiForAsync(int i, const CStringA& resp);

// 재정의입니다.
public:
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
	virtual ~CPostTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	// GET;POST
	CComboBox _cMethod;
	CString _method;

	afx_msg void OnBnClickedSend();
	//CEdit _cURL;
	CString _URL;
	CString _request;
	CString _response;

	CListBox _cListResponse;
	afx_msg void OnLbnSelchangeListresponse();
	int _MaxItems;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedUploadimages();
	afx_msg void OnBnClickedStop();
	//int _NumTest;
	UINT _test;
};

#ifndef _DEBUG  // PostTestView.cpp의 디버그 버전
inline CPostTestDoc* CPostTestView::GetDocument() const
   { return reinterpret_cast<CPostTestDoc*>(m_pDocument); }
#endif

