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
	bool _bFirst{false};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedCreatesimple();
	CString _UID;
	CString _PWD;
	CString _DSN;
	afx_msg void OnBnClickedOdbcsetting();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
