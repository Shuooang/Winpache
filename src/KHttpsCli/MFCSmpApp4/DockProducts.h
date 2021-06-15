#pragma once
#include "DockPane.h"



// DockProducts form view

class DockProducts : public DockClientBase
{
	DECLARE_DYNCREATE(DockProducts)

public:
	DockProducts(UINT nID = 0, CPaneForm* ppn = NULL);           // protected constructor used by dynamic creation
	virtual ~DockProducts();
	static DockProducts* s_me;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DockProducts };
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.


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
	//void ClearList(int iOp = 0) override;
	_STitleWidthField* getListFieldInfo(int* pnCols = nullptr) override;

	void RM_SelectProductList(ShJObj sjo, int iOp = 0);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedRead();
	afx_msg void OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedAddtoorder();
};


class DockProductsPane : public CPaneForm
{
	DECLARE_DYNCREATE(DockProductsPane)
public:
	//CreateDockingWin 에서 채워진다. 이 pane이 아니고 child formview의 ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockProducts(_nID, this); }
};
// error C2664 : 'DockEx1Pane::DockEx1Pane(const DockEx1Pane &)' : 인수 1을(를) 'DockEx1Pane *'에서 'const DockEx1Pane &'(으)로 변환할 수 없습니다.

