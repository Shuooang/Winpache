#pragma once

#include "KwLib64/DlgTool.h"

// DlgRanking dialog

class DlgRanking : public CDlgInvokable
{
	DECLARE_DYNAMIC(DlgRanking)

public:
	DlgRanking(CWnd* pParent = nullptr);   // standard constructor
	virtual ~DlgRanking();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DlgRanking };
#endif
	ShJArr _sjTbl;
	int _iSel{-1};
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void Refresh();
	CListCtrl _cList;
	afx_msg void OnBnClickedReplaysample();
	afx_msg void OnBnClickedTrysame();
	afx_msg void OnBnClickedCancel();
};
