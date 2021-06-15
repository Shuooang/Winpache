#pragma once
#include "KwLib64/DlgTool.h"


// DlgSizeAdjust 대화 상자

class DlgSizeAdjust : public CDlgInvokable
{
	DECLARE_DYNAMIC(DlgSizeAdjust)

public:
	DlgSizeAdjust(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~DlgSizeAdjust();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ImageSizeAdjust };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	void FileAdjust(LPCTSTR pDir, CSize szf);
	CString _folder;
	int _width{ 1024 };
	int _height{ 1024 };
	int _nImgFile{ 0 };
	int _nImgFileChanged{ 0 };
	int _nExceptDir{ 0 };

	afx_msg void OnBnClickedCancel();
	BOOL _Simulation;
	// 이 디렉토리 명을 포함한 풀 패스 파일은 제외 한다.
	CString _ExceptDir;
};
