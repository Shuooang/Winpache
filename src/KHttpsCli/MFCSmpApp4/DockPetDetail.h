#pragma once
#include "KwLib64/MfcEx.h"

#pragma once
#include "KwLib64/MfcEx.h"

class DockPetDetail :
	public DockClientBase
{
	DECLARE_DYNCREATE(DockPetDetail)

public:
	DockPetDetail(UINT nID = 0, CPaneForm* ppn = NULL);           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~DockPetDetail();
	static DockPetDetail* s_me;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DockPetDetail };
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
	void Refresh(string eStep, ShJObj sjo = ShJObj(), int iOp = 0) override
	{
	}


public:
	CMFCPropertyGridCtrl _cProperty;
	void SetVSDotNetLook(BOOL bSet)
	{
		_cProperty.SetVSDotNetLook(bSet);
		_cProperty.SetGroupNameFullWidth(bSet);
	}

	void AdjustLayout();
	void InitPropList(ShJObj sjo = nullptr) override;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnNMThemeChangedMfcpropertygrid1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStnClickedMfcpropertygrid1();
	afx_msg void OnStnDblclickMfcpropertygrid1();
	afx_msg void OnBnClickedStartOrder();
};


class DockPetDetailPane : public CPaneForm
{
	DECLARE_DYNCREATE(DockPetDetailPane)
public:
	//CreateDockingWin 에서 채워진다. 이 pane이 아니고 child formview의 ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockPetDetail(_nID, this); }
};
// error C2664 : 'DockEx1Pane::DockEx1Pane(const DockEx1Pane &)' : 인수 1을(를) 'DockEx1Pane *'에서 'const DockEx1Pane &'(으)로 변환할 수 없습니다.

