#pragma once

#include "DockPane.h"


// DockOdbc form view

class DockOdbc : public DockClientBase
{
	DECLARE_DYNCREATE(DockOdbc)

public:
	DockOdbc(UINT nID = 0, CPaneForm* ppn = NULL);           // protected constructor used by dynamic creation
	virtual ~DockOdbc();
	static DockOdbc* s_me;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DockOdbc };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	CString _DSN{ L"Winpache" };
	CString _UID{ L"root" };
	CString _PWD;

protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void AdjustLayout();
public:

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	afx_msg void OnBnClickedCreatesimple();
	afx_msg void OnBnClickedCreatesimple2();





	afx_msg void OnBnClickedConnectDB();
};




class DockOdbcPane : public CPaneForm
{
	DECLARE_DYNCREATE(DockOdbcPane)
public:
	//CreateDockingWin 에서 채워진다. 이 pane이 아니고 child formview의 ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockOdbc(_nID, this); }
};

