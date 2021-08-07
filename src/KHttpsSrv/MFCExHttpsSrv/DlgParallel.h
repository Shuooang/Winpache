#pragma once


// DlgParallel dialog

class DlgParallel : public CDialogEx
{
	DECLARE_DYNAMIC(DlgParallel)

public:
	DlgParallel(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DlgParallel();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_Clustering };
#endif
	CDocument* _doc{0};
	CString _url;
	CString _mode;
	CString _action;
	CListCtrl _cList;

	ShJArr _arSvr{make_shared<JArr>()};
	void Refresh();
	bool CheckDuplicate(PWS newUrl, int iExcept = -1);
	ShJObj GetCurJObj();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg	void OnSelchangeMode();
	afx_msg void OnSelchangeAction();
	afx_msg void OnChangeUrl();
	afx_msg void OnBnClickedOk();
	afx_msg void OnItemchangedClustering(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDel();
	afx_msg void OnBnClickedApply();
};
