#pragma once
#include "DockPane.h"

class DockMyPetList :
	public DockClientBase
{
	DECLARE_DYNCREATE(DockMyPetList)

public:
	DockMyPetList(UINT nID = 0, CPaneForm* ppn = NULL);           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~DockMyPetList();
	static DockMyPetList* s_me;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DockMyPetList };
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

	ShJArr _sjoUsr;
	CString _fUsrID;
	//ShJArr _sjaTable;
	//ShJArr _sjaEnum;
	bool _bNoLogForItemChange = { false };
protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	void OnDockFocus() override;


public:
	void AdjustLayout();
	void InitPropList(ShJObj sjo = nullptr) override;
	void ClearList(int iOp = 0) override;
	void SetVSDotNetLook(BOOL bSet)
	{
		_cProperty.SetVSDotNetLook(bSet);
		_cProperty.SetGroupNameFullWidth(bSet);
	}
	//void Refresh(string eStep, CString fPetID);
	void Refresh(string eStep, ShJObj sjo = ShJObj(), int iOp = 0) override;
	void InsertDataToList(bool bHasData = true) override;
	_STitleWidthField* getListFieldInfo(int* pnCols = nullptr) override;
	
	//ShJObj FindByID(PWS fPetID);
	//void StartPetManage(CStringW fUsrID, int iOp = 0);

	void RM_SelectPetList(ShJObj sjo, int iOp = 0);
	void RM_AddMyPet(ShJObj sjo, int iOp = 0);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedRead();
	afx_msg void OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedAddToOrder();
	afx_msg void OnBnClickedAddMyPet();
	afx_msg void OnBnClickedNeworder();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};


class DockMyPetListPane : public CPaneForm
{
	DECLARE_DYNCREATE(DockMyPetListPane)
public:
	//CreateDockingWin 에서 채워진다. 이 pane이 아니고 child formview의 ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockMyPetList(_nID, this); }
};
// error C2664 : 'DockEx1Pane::DockEx1Pane(const DockEx1Pane &)' : 인수 1을(를) 'DockEx1Pane *'에서 'const DockEx1Pane &'(으)로 변환할 수 없습니다.

