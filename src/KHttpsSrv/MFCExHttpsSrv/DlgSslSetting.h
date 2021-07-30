#pragma once


// DlgSslSetting dialog

class DlgSslSetting : public CDialogEx
{
	DECLARE_DYNAMIC(DlgSslSetting)

public:
	DlgSslSetting(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DlgSslSetting();

	CEditCtrlCV c_certificate;
	CEditCtrlCV c_privatekey;
	CEditCtrlCV c_dhparam;
	CEditCtrlCV c_prvpwd;

	CDocument* _doc{ nullptr };
	ShJObj _jobj;
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SslSetting };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnBnClickedBtncertificate();
	afx_msg void OnBnClickedBtnprivate();
	afx_msg void OnBnClickedBtndhparam();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
