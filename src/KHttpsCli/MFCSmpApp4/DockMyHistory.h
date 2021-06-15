#pragma once
#include "KwLib64/MfcEx.h"

class DockMyHistory :
	public DockClientBase
{
	DECLARE_DYNCREATE(DockMyHistory)

public:
	DockMyHistory(UINT nID = 0, CPaneForm* ppn = NULL);           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
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
	//CreateDockingWin ���� ä������. �� pane�� �ƴϰ� child formview�� ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockMyHistory(_nID, this); }
};
// error C2664 : 'DockEx1Pane::DockEx1Pane(const DockEx1Pane &)' : �μ� 1��(��) 'DockEx1Pane *'���� 'const DockEx1Pane &'(��)�� ��ȯ�� �� �����ϴ�.

