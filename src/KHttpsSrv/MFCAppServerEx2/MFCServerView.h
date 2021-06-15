
// MFCServerView.h: CMFCServerView 클래스의 인터페이스
//

#pragma once

#include "KwLib64/DlgTool.h"
#include "KwLib64/HTTPS/HttpSvr.h"
#include "KwLib64/UdpSvr.h"

#include "HttpsSvr.h"
#include "HttpSvr.h"
#include "CmnView.h"


class CMFCServerView 
	: public CFormInvokable
	, public CmnView
{
protected: // serialization에서만 만들어집니다.
	CMFCServerView() noexcept;
	DECLARE_DYNCREATE(CMFCServerView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_MFCAPPSERVEREX2_FORM };
#endif

	//Ctrl-C/ Ctrl-V 키가 먹히게 하려면

// 특성입니다.
public:
	CMFCAppServerEx2Doc* GetDocument() const;
	virtual CDocument* GetDoc() override
	{
		return GetDocument();
	}
	_STitleWidthField* GetArListConf(int* nCols) override;
	void MonitorRequest(shared_ptr<KArray<string>> shar) override;

// 작업입니다.
public:

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
	virtual ~CMFCServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


protected:
	//CMyHttps* _https;
	CListCtrl _cMonitor;

	//void MakeJsonResponse(HTTPResponse& res, string_view js);
	
	
	//void InitApi();

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg	void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedRestart();
	afx_msg void OnBnClickedBtnpath();
	afx_msg void OnDestroy();

	//afx_msg void OnBnClickedbssl();
	afx_msg void OnBnClickedBtnimalocal();
	afx_msg void OnImageMigration();
	afx_msg void OnBnClickedStartDB();
	afx_msg void OnMigrationImageSizeAdjust();
	afx_msg void OnBnClickedTestapi();
	afx_msg void OnBnClickedStartUDP();

	afx_msg void OnBnClickedBtnUploadLocal();
	afx_msg void OnBnClickedBtnSslSetting();
	afx_msg void OnFreeLibrary();
};

#ifndef _DEBUG  // MFCServerView.cpp의 디버그 버전
inline CMFCAppServerEx2Doc* CMFCServerView::GetDocument() const
   { return reinterpret_cast<CMFCAppServerEx2Doc*>(m_pDocument); }
#endif

