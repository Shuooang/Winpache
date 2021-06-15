
// StartWinpacheDlg.h : header file
//

#pragma once


// CStartWinpacheDlg dialog
class CStartWinpacheDlg : public CDialogEx
{
// Construction
public:
	CStartWinpacheDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STARTWINPACHE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedWinpachepro();
	afx_msg void OnBnClickedWinpache();
	afx_msg void OnBnClickedWincatcher();
	afx_msg void OnBnClickedGotobinary();
	afx_msg void OnBnClickedGotoproject();
	afx_msg void OnBnClickedGotoprogramfiles();
	CStatic c_trace;
	void OpenFile(LPCWSTR tdir, LPCWSTR fsln, LPCWSTR desc);
	void GotoDirectory(int csidl, LPCWSTR adddir, LPCWSTR desc);
	afx_msg void OnBnClickedOpentheproject();
};
