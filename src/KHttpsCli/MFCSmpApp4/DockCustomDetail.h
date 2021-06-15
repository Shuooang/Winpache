#pragma once
#include "KwLib64\MfcEx.h"
#include "TimeTable.h"

class DockCustomDetail :
    public DockClientBase
{
	DECLARE_DYNCREATE(DockCustomDetail)

public:
	DockCustomDetail(UINT nID = 0, CPaneForm* ppn = NULL); // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~DockCustomDetail();
	static DockCustomDetail* s_me;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DockCustomDetail };//?dock declare
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	void OnDockFocus() override;

public:
	void Refresh(string eStep, ShJObj sjo = ShJObj(), int iOp = 0) override;
	CMFCPropertyGridCtrl _cProperty;// DockClientBase::_prData

	void SetVSDotNetLook(BOOL bSet)
	{
		_cProperty.SetVSDotNetLook(bSet);
		_cProperty.SetGroupNameFullWidth(bSet);
	}

	void AdjustLayout();
	void InitPropList(ShJObj sjo = nullptr) override;
	void RM_SelectCustomDetail(ShJObj sjo, int iOp = 0);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnNMThemeChangedMfcpropertygrid1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStnClickedMfcpropertygrid1();
	afx_msg void OnStnDblclickMfcpropertygrid1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedStartOrder();
	afx_msg void OnBnClickedMyPetList();
};


class DockCustomDetailPane : public CPaneForm
{
	DECLARE_DYNCREATE(DockCustomDetailPane)
public:
	//CreateDockingWin 에서 채워진다. 이 pane이 아니고 child formview의 ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockCustomDetail(_nID, this); }
};
// error C2664 : 'DockEx1Pane::DockEx1Pane(const DockEx1Pane &)' : 인수 1을(를) 'DockEx1Pane *'에서 'const DockEx1Pane &'(으)로 변환할 수 없습니다.


