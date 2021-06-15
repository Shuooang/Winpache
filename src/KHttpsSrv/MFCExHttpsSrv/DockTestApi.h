#pragma once

#include "DockPane.h"


// DockTestApi form view

class DockTestApi : public DockClientBase
{
	DECLARE_DYNCREATE(DockTestApi)

public:
	DockTestApi(UINT nID = 0, CPaneForm* ppn = NULL);           // protected constructor used by dynamic creation
	virtual ~DockTestApi();
	static DockTestApi* s_me;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DockTestApi };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	void AdjustLayout();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);


public:
	CString _Function;
	CEditCtrlCV c_Input;
	CEditCtrlCV c_Output;
	CString _Input;
	CString _Output;
	afx_msg void OnBnClickedRunapi();
	afx_msg	void OnBnClickedSelect();
	CComboBox c_ExList;
public:
	void CreateDatabase();
	void SetApiFunc(PWS sfnc);

};



class DockTestApiPane : public CPaneForm
{
	DECLARE_DYNCREATE(DockTestApiPane)
public:
	//CreateDockingWin 에서 채워진다. 이 pane이 아니고 child formview의 ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockTestApi(_nID, this); }
};

