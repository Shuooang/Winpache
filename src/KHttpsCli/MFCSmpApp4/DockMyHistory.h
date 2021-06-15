#pragma once
#include "KwLib64/MfcEx.h"

class DockMyHistory :
	public DockClientBase
{
	DECLARE_DYNCREATE(DockMyHistory)

public:
	DockMyHistory(UINT nID = 0, CPaneForm* ppn = NULL);           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~DockMyHistory();
	static DockMyHistory* s_me;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DockMyHistory };
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
	CListCtrl _cList;
	CListCtrl* getListCtrl() override
	{
		return &_cList;
	}

	void AdjustLayout();
	void InitPropList(ShJObj sjo = nullptr) override;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBnClickedButton1();
};


class DockMyHistoryPane : public CPaneForm
{
	DECLARE_DYNCREATE(DockMyHistoryPane)
public:
	//CreateDockingWin 에서 채워진다. 이 pane이 아니고 child formview의 ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockMyHistory(_nID, this); }
};
// error C2664 : 'DockEx1Pane::DockEx1Pane(const DockEx1Pane &)' : 인수 1을(를) 'DockEx1Pane *'에서 'const DockEx1Pane &'(으)로 변환할 수 없습니다.

