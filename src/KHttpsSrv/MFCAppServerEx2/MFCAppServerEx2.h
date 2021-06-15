
// MFCAppServerEx2.h: MFCAppServerEx2 애플리케이션의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.

#include "Response1.h"

// CMFCAppServerEx2:
// 이 클래스의 구현에 대해서는 MFCAppServerEx2.cpp을(를) 참조하세요.
//

class CMFCAppServerEx2 
	: public CWinApp
	, public KCheckWnd
{
public:
	CMFCAppServerEx2() noexcept;

	/// doc으로 이동 해야, 셋팅이 doc별로 다를 수 있지.
// 	CResponse1* _svr;//CApiBase 
// 	KAtEnd _d_svr;
// 재정의입니다.
public:
	virtual BOOL InitInstance();
	void InitApp();
	virtual int ExitInstance();

// 구현입니다.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCAppServerEx2 theApp;
