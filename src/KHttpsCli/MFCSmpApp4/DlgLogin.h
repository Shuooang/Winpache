#pragma once


// DlgLogin 대화 상자

class DlgLogin : public CDialogEx
{
	DECLARE_DYNAMIC(DlgLogin)

public:
	DlgLogin(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~DlgLogin();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LogIn };
#endif
	void RM_RequestLogin(ShJObj sjo, int iOp);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString _sLoginID;
	CString _sPcPwd;
	CString _CertCode;
};
