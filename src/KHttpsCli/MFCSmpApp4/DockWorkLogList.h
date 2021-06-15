#pragma once
#include "DockPane.h"


class DockWorkLogList :
	public DockClientBase
{
	DECLARE_DYNCREATE(DockWorkLogList)

public:
	DockWorkLogList(UINT nID = 0, CPaneForm* ppn = NULL);           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~DockWorkLogList();
	static DockWorkLogList* s_me;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DockWorkLogList };
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
	BOOL Filter(ShJObj sjoRow) override;
	CEdit _cFilter;
	static _STitleWidthField s_arlst[6];

protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
public:
	void Refresh(string eStep, ShJObj sjo = ShJObj(), int iOp = 0) override;
	void WorkLog(ShJObj sjo);
	// 	void WorkLog(PAS fObject, PAS fObjID, PAS fMode, PAS fObjName)
// 	{
// 		WorkLog(fObject, CStringW(fObjID), fMode, fObjName);
// 	}
//	void WorkLog(PAS fObject, PWS fObjID, PAS fMode, PAS fObjName);
	void SetInsertRoutine();
	void InsertDataToList(bool bHasData = true) override;
	_STitleWidthField* getListFieldInfo(int* pnCols = nullptr) override;
	void SyncWorkLog(int isel, int iOp = 0);

	void AdjustLayout();
	void InitPropList(ShJObj sjo = nullptr) override;
	void RM_SelectWorkLogList(ShJObj sjo = nullptr, int iOp = 0);

	void RM_AddWorkLog(ShJObj sjo, int iOp = 0);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeFilter();
	afx_msg void OnBnClickedRead();
};


class DockWorkLogListPane : public CPaneForm
{
	DECLARE_DYNCREATE(DockWorkLogListPane)
public:
	//CreateDockingWin 에서 채워진다. 이 pane이 아니고 child formview의 ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockWorkLogList(_nID, this); }
};
// error C2664 : 'DockEx1Pane::DockEx1Pane(const DockEx1Pane &)' : 인수 1을(를) 'DockEx1Pane *'에서 'const DockEx1Pane &'(으)로 변환할 수 없습니다.

