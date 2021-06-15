#pragma once

#include "KwLib64/MfcEx.h"
/// �ٸ� â�� �Լ� ȣ���� ��
/// iOp �� ��� ���� ����
enum ERefresh
{
	eShowDock1 = 1 << 0,
	eNoBlinkBmp2 = 1 << 1, // �������� ��¦ �ŷ��� ���� �ϴ°� ���δ�.
	eReqNoErrBox4 = 1 << 2, // ���� �ڽ��� �ȶ��.
	ePostAsync8 = 1 << 3, //PostMessage -> _KwBeginInvoke �� �޽��� ť�� �־ �񵿱�� ó��. �Ǵ� ��׶��忡�� ���׶���� �۾� ť
	eNoCheckMsgBox16 = 1 << 4, // 16 ���� Ȯ�� �ϴ� �޽����ڽ� �ȶ��.
	eNoWorkLog32 = 1 << 5, // �۾� �α׸� ���� �Ѵ�.
	eSyncByAction64 = 1 << 6, // ����� �׼����� 1�� ����ȭ ��û ���� ǥ��
	eRunInBG128 = 1 << 7,
	eNotInBG256 = 1 << 8, // ��׶��� �۾� �۾� ����
	eUpdateUI512 = 1 << 9,
};



class DockClientBase :
	public CDockFormChild
{
	DECLARE_DYNCREATE(DockClientBase)

public:
	DockClientBase(UINT nID = 0, CPaneForm* ppn = NULL);           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~DockClientBase();

public:
	static void ShowHide(CMDIFrameWndEx* frm, CBasePane& win, CCmdUI* pCmdUI = nullptr);
	static void ShowHide(CMDIFrameWndEx* frm, CBasePane& win, BOOL bShow);


protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


class SettingDock
{
public:
	SettingDock(CRuntimeClass* prc, LPCWSTR title, UINT nID, DWORD dwStyle, int idiHicon)
	{
		_pRC = prc;
		//_dock = dock;//CDockablePane* dock, 
		_title = title;
		_rc = CRect(CPoint(0, 0), CSize(200, 200));
		_nID = nID;
		_dwStyle |= dwStyle; //����: or�� ���� �Ѵ�. �ٸ���� ���� �ش�.
		_idiHicon = idiHicon;
	}
	CRuntimeClass* _pRC{ nullptr };
	CObject* _dock{ nullptr };//CDockablePane
	LPCWSTR _title{ nullptr };
	//CWnd* pParent, 
	CRect _rc{ 0,0,0,0 };
	BOOL _bHasGripper{ TRUE };
	UINT _nID{ 0 };
	DWORD _dwStyle{ WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI };
	DWORD _dwTabbedStyle{ AFX_CBRS_REGULAR_TABS };
	DWORD _dwControlBarStyle{ AFX_DEFAULT_DOCKING_PANE_STYLE };
	int _idiHicon{ -1 };
	//CBRS_LEFT | 
};

class DockEx1Pane;
class KDockPane
{
public:
	KDockPane();
	~KDockPane();
	void InitDocks(CFrameWnd* mframe);

	CMDIFrameWndEx* _mframe{ nullptr };
	//DockEx1Pane* _DockEx1{ nullptr };
	BOOL CreateDockingWin(SettingDock& sdck);
	BOOL CreateDockingForm(SettingDock& sdck);
	KStdMap<UINT, CObject*> m_docks;// KStdMapPtr �� auto free���� ����.

	//template<typename TDockPane>
	//void InitDockPane(TDockPane*& pane, UINT ID)
	//{
	//	pane = new TDockPane(ID);
	//}

	CDockablePane* GetDockPane(UINT nID);
	CDockFormChild* GetDockView(UINT nID);
};
