#pragma once
#include "DockPane.h"


// DockOptions dialog

class DockOptions : public DockClientBase
{
	DECLARE_DYNCREATE(DockOptions)

public:
	DockOptions(UINT nID = 0, CPaneForm* ppn = NULL);   // standard constructor
	virtual ~DockOptions();
	static DockOptions* s_me;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DockOptions };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	CListCtrl _cList;
	CListCtrl* getListCtrl() override
	{
		return &_cList;
	}
	CMFCPropertyGridCtrl _cProperty;// DockClientBase::_prData
	ShJArr _sjaTable;
	ShJArr _sjaEnum;
protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.


public:
	void AdjustLayout();
	void InitPropList(ShJObj sjo = nullptr) override;
	void SetVSDotNetLook(BOOL bSet)
	{
		_cProperty.SetVSDotNetLook(bSet);
		_cProperty.SetGroupNameFullWidth(bSet);
	}
	void Refresh(string eStep, ShJObj sjo = ShJObj(), int iOp = 0) override;
	void InsertDataToList(bool bHasData = true) override;
	_STitleWidthField* getListFieldInfo(int* pnCols = nullptr) override;

	void RM_SelectOptions(ShJObj sjo, int iOp = 0);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedRead();
	afx_msg void OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
};


class DockOptionsPane : public CPaneForm
{
	DECLARE_DYNCREATE(DockOptionsPane)
public:
	//CreateDockingWin ���� ä������. �� pane�� �ƴϰ� child formview�� ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockOptions(_nID, this); }
};
// error C2664 : 'DockEx1Pane::DockEx1Pane(const DockEx1Pane &)' : �μ� 1��(��) 'DockEx1Pane *'���� 'const DockEx1Pane &'(��)�� ��ȯ�� �� �����ϴ�.

