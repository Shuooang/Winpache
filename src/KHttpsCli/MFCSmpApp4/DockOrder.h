#pragma once
#include "DockPane.h"



// DockOrder form view

class DockOrder;
class KPropertyCtrl : public CMFCPropertyGridCtrl
{
public:
	DockOrder* _dock{ nullptr };
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};

class DockOrder : public DockClientBase
{
	DECLARE_DYNCREATE(DockOrder)

public:
	DockOrder(UINT nID = 0, CPaneForm* ppn = NULL);           // protected constructor used by dynamic creation
	virtual ~DockOrder();
	static DockOrder* s_me;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DockOrder};
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	CListCtrl _cList1;
	CListCtrl _cList2;
	CListCtrl _cList3;

	KPropertyCtrl _cProperty;
	//KProperty* _prData{ nullptr };

	//ShJArr _sjaTable;
	//ShJArr _sjaEnum;
	/// 편집중이면 이걸 _prData 에 넣는게 좋을거 같은데
	CStringA _eStatEdit; // "new", "edit", ""
protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	void OnDockFocus() override;


public:
	void AdjustLayout();
	void InitPropList(ShJObj sjo = nullptr) override;
	void InitPropertyPet(KProperty& dt, int iPet);
	void InitPropertyProduct(KProperty& dt, int iPet, int iPrd);
	void InitPropertyOption(KProperty& dt, int iPet, int iPrd, int iOpt);
	void SetVSDotNetLook(BOOL bSet)
	{
		_cProperty.SetVSDotNetLook(bSet);
		_cProperty.SetGroupNameFullWidth(bSet);
	}
	void Refresh(string eStep, ShJObj sjo = ShJObj(), int iOp = 0) override;
	_STitleWidthField* getListFieldInfo(int* pnCols = nullptr) override;

	void RM_Function(CStringA sFunc, ShJObj sjo = nullptr, int iOp = 0);
	//void RM_SelectFullOrder(ShJObj sjo, int iOp = 0);
	//void RM_AddOrder1stNew(ShJObj sjo, int iOp = 0);
	//void RM_AddOrder2ndPet(ShJObj sjo, int iOp = 0);
	//void RM_AddOrder3rdProduct(ShJObj sjo, int iOp = 0);
	//void RM_DeleteOrder2ndPet(ShJObj sjo, int iOp = 0);
	//void RM_DeleteOrder3rdProduct(ShJObj sjo, int iOp = 0);

	//void StartOrder(CStringW fUsrID, int iOp = 0);
	//void DoOrderAddPet(PAS eStep, ShJObj sjo = ShJObj(), int iOp = 0);
	void NewOrder(PAS eStep, ShJObj sjoPet, int iOp);
	BOOL AddPet(PAS eStep, ShJObj sjoPet = nullptr, int iOp = 0);
	BOOL AddProd(PAS eStep, ShJObj sjoin = nullptr, int iOp = 0);
	BOOL AddOption(PAS eStep, ShJObj sjoin = nullptr, int iOp = 0);

	BOOL DelItem(CStringA item, int iOp = 0);

	BOOL DelPet(int iOp = 0);
	BOOL DelProd(int iOp = 0);
	BOOL DelOption(int iOp = 0);
	void WorkLogOrder(ShJObj sjo, PAS fMode, PAS fMemo = nullptr);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg void OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnBnClickedRead();
	afx_msg void OnBnClickedAddpet();
	afx_msg void OnBnClickedStartorder();

	afx_msg void OnPopOrderAddPet();
	afx_msg void OnPopOrderAddProd();
	afx_msg void OnPopOrderAddOpt();
	
	afx_msg void OnPopOrderDelPet();
	afx_msg void OnPopOrderDelProd();
	afx_msg void OnPopOrderDelOpt();

	afx_msg void OnAddPet();
	afx_msg void OnAddProd();
	afx_msg void OnAddOption();
	afx_msg void OnAddSchedule();
	afx_msg void OnPayment();

	afx_msg void OnAddPetRb();
	afx_msg void OnAddProdRb();
	afx_msg void OnAddOptionRb();
	afx_msg void OnAddScheduleRb();
	afx_msg void OnPaymentRb();
	afx_msg void OnBnClickedAddprod();
	afx_msg void OnDelPet();
	afx_msg void OnDelProd();
	afx_msg void OnDelOption();
	afx_msg void OnUpdatePopOrderDelPet(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopOrderAddProd(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopOrderDelProd(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopOrderAddOpt(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopOrderDelOpt(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddPet(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePopOrderAddPet(CCmdUI* pCmdUI);
};


class DockOrderPane : public CPaneForm
{
	DECLARE_DYNCREATE(DockOrderPane)
public:
	//CreateDockingWin 에서 채워진다. 이 pane이 아니고 child formview의 ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockOrder(_nID, this); }
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	//afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	BOOL OnShowControlBarMenu(CPoint point) override
	{
		__super::OnShowControlBarMenu(point);
		return FALSE;//기본으로 뜨는 팝업메뉴를 막기 위해
	}
	void OnBeforeChangeParent(CWnd* pWndNewParent, BOOL bDelay = FALSE) override
	{
		__super::OnBeforeChangeParent(pWndNewParent, bDelay);
		TRACE("%s\n", __FUNCTION__);
	}
	void OnAfterChangeParent(CWnd* pWndOldParent) override
	{
		__super::OnAfterChangeParent(pWndOldParent);
		TRACE("%s\n", __FUNCTION__);
	}
	void OnAfterDockFromMiniFrame()  override
	{
		__super::OnAfterDockFromMiniFrame();
		TRACE("%s\n", __FUNCTION__);
	}
	BOOL OnBeforeFloat(CRect& rectFloat, AFX_DOCK_METHOD dockMethod) override
	{
		return __super::OnBeforeFloat(rectFloat, dockMethod);
		TRACE("%s\n", __FUNCTION__);
	}
	void OnContinueMoving()  override
	{
		__super::OnContinueMoving();
		TRACE("%s\n", __FUNCTION__);
	}//	able control bar doesn't move after docking
	void OnAfterDock(CBasePane* pBar, LPCRECT lpRect, AFX_DOCK_METHOD dockMethod) override
	{
		__super::OnAfterDock(pBar, lpRect, dockMethod);
		TRACE("%s\n", __FUNCTION__);
	}
	void OnPressCloseButton() override
	{
		__super::OnPressCloseButton();
		TRACE("%s\n", __FUNCTION__);
	}
	void OnTrackCaptionButtons(CPoint point) override
	{
		__super::OnTrackCaptionButtons(point);
		TRACE("%s\n", __FUNCTION__);
	}

};
// error C2664 : 'DockEx1Pane::DockEx1Pane(const DockEx1Pane &)' : 인수 1을(를) 'DockEx1Pane *'에서 'const DockEx1Pane &'(으)로 변환할 수 없습니다.

