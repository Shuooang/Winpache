#pragma once
#include "DockPane.h"


class DockCustomList :
	public DockClientBase
{
	DECLARE_DYNCREATE(DockCustomList)

public:
	DockCustomList(UINT nID = 0, CPaneForm* ppn = NULL);           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~DockCustomList();
	static DockCustomList* s_me;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DockCustomList };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	CListCtrl _cList;
	BOOL Filter(ShJObj sjoRow) override;
	CListCtrl* getListCtrl() override
	{
		return &_cList;
	}

protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	void Refresh(string eStep, ShJObj sjo = ShJObj(), int iOp = 0) override;
	void InsertDataToList(bool bHasData = true) override;
	//void ClearList(int iOp = 0) override;
	_STitleWidthField* getListFieldInfo(int* pnCols = nullptr) override;
	void SyncCustomDetail(int isel, int iOp = 0);

public:
	void AdjustLayout();
	void InitPropList(ShJObj sjo = nullptr) override;
	void RM_SelectCustomList(ShJObj sjo, int iOp = 0);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedRead();
	afx_msg void OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	CEdit _cFilter;
	afx_msg void OnEnChangeFilter();
	afx_msg void OnBnClickedOrderStart();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


class DockCustomListPane : public CPaneForm
{
	DECLARE_DYNCREATE(DockCustomListPane)
public:
	//CreateDockingWin ���� ä������. �� pane�� �ƴϰ� child formview�� ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockCustomList(_nID, this); }
};
// error C2664 : 'DockEx1Pane::DockEx1Pane(const DockEx1Pane &)' : �μ� 1��(��) 'DockEx1Pane *'���� 'const DockEx1Pane &'(��)�� ��ȯ�� �� �����ϴ�.

