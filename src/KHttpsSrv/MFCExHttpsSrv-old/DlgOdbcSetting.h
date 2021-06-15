#pragma once


// DlgOdbcSetting dialog

class DlgOdbcSetting : public CDialogEx
{
	DECLARE_DYNAMIC(DlgOdbcSetting)

public:
	DlgOdbcSetting(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DlgOdbcSetting();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OdbcSetting };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedCreatesimple();
	afx_msg void OnBnClickedCreatesimple2();
	CString _UID;
	CString _PWD;
};
