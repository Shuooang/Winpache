#pragma once
#include "KwLib64/DlgTool.h"

// DlgUser dialog

class DlgUser : public CDlgInvokable
{
	DECLARE_DYNAMIC(DlgUser)

public:
	DlgUser(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DlgUser();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DlgUser };
#endif
	void OnAfterSave(int rv, ShJObj sjUsr);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString _email;
	CString _SNS;
	afx_msg	void OnBnClickedOk();
	CString _Nickname;
};
