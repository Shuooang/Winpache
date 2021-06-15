#pragma once
#include "KwLib64/MfcEx.h"






class DockScheduleDetail :
	public DockClientBase
{
	DECLARE_DYNCREATE(DockScheduleDetail)

public:
	DockScheduleDetail(UINT nID = 0, CPaneForm* ppn = NULL);           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~DockScheduleDetail();
	static DockScheduleDetail* s_me;

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DockScheduleDetail };
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

	void OnDockFocus() override;

public:
	void Refresh(string eStep, ShJObj sjo = ShJObj(), int iOp = 0) override;

	CMFCPropertyGridCtrl _cProperty;
	CStringA _table;
	void SetVSDotNetLook(BOOL bSet)
	{
		_cProperty.SetVSDotNetLook(bSet);
		_cProperty.SetGroupNameFullWidth(bSet);
	}

	void AdjustLayout();
	void InitPropList(CStringA fModeA)
	{
		auto sjo = make_shared<JObj>();
		(*sjo)("table") = (PWS)CStringW(fModeA);
		InitPropList(sjo);
	}
	void InitPropList(ShJObj sjo = nullptr) override;

	//void Populate(ShJObj sjo);

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnNMThemeChangedMfcpropertygrid1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnStnClickedMfcpropertygrid1();
	afx_msg void OnStnDblclickMfcpropertygrid1();
};


class DockScheduleDetailPane : public CPaneForm
{
	DECLARE_DYNCREATE(DockScheduleDetailPane)
public:
	//CreateDockingWin ���� ä������. �� pane�� �ƴϰ� child formview�� ID
	virtual DockClientBase* _CreateForm() { ASSERT(_nID != 0);  return new DockScheduleDetail(_nID, this); }
};
// error C2664 : 'DockEx1Pane::DockEx1Pane(const DockEx1Pane &)' : �μ� 1��(��) 'DockEx1Pane *'���� 'const DockEx1Pane &'(��)�� ��ȯ�� �� �����ϴ�.


