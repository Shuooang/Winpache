#pragma once

#include "KwLib64/MfcEx.h"
/// 다른 창에 함수 호출할 때
/// iOp 에 들어 가는 내용
enum ERefresh
{
	eShowDock1 = 1 << 0,
	eNoBlinkBmp2 = 1 << 1, // 아이콘이 반짝 거려서 반응 하는걸 보인다.
	eReqNoErrBox4 = 1 << 2, // 에러 박스가 안뜬다.
	ePostAsync8 = 1 << 3, //PostMessage -> _KwBeginInvoke 로 메시지 큐에 넣어서 비동기로 처리. 또는 백그라운드에서 포그라운드로 작업 큐
	eNoCheckMsgBox16 = 1 << 4, // 16 삭제 확인 하는 메시지박스 안뜬다.
	eNoWorkLog32 = 1 << 5, // 작업 로그를 무시 한다.
	eSyncByAction64 = 1 << 6, // 사용자 액션으로 1차 동기화 요청 임을 표시
	eRunInBG128 = 1 << 7,
	eNotInBG256 = 1 << 8, // 백그라운드 작업 작업 금지
	eUpdateUI512 = 1 << 9,
};



class DockClientBase :
	public CDockFormChild
{
	DECLARE_DYNCREATE(DockClientBase)

public:
	DockClientBase(UINT nID = 0, CPaneForm* ppn = NULL);           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~DockClientBase();

public:
	static void ShowHide(CMDIFrameWndEx* frm, CBasePane& win, CCmdUI* pCmdUI = nullptr);
	static void ShowHide(CMDIFrameWndEx* frm, CBasePane& win, BOOL bShow);


protected:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
		_dwStyle |= dwStyle; //주의: or로 병합 한다. 다를경우 따로 준다.
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
	KStdMap<UINT, CObject*> m_docks;// KStdMapPtr 은 auto free때만 쓴다.

	//template<typename TDockPane>
	//void InitDockPane(TDockPane*& pane, UINT ID)
	//{
	//	pane = new TDockPane(ID);
	//}

	CDockablePane* GetDockPane(UINT nID);
	CDockFormChild* GetDockView(UINT nID);
};
