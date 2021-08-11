
// MainDlg.h : header file
//

#pragma once

#include <gdiplus.h>				// Base include
#include <vector>
#include "KwLib64/DlgTool.h"
#include "CSquare.h"


// CMainDlg dialog
class CMainDlg : public CDlgInvokable //CDialogEx
{
// Construction
public:
	CMainDlg(CWnd* pParent = nullptr);	// standard constructor
	~CMainDlg();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SUARESIMUL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();

public:
	CSquare _sq;
	void PopulateCol(int col, int iAny = -1);
	void PopulateRow(int row);
	void Populate();
	void PopulateDirection(int direction, int ln);

	void MoveAny(int edr, int rc, int n);

// Implementation
public:
	HICON m_hIcon;

	HDC _hdc{nullptr};
	
	Gdiplus::Graphics* _gc{nullptr};
	
	Gdiplus::Brush* _brBG{nullptr,};
	Gdiplus::Brush* _brBox{nullptr,};
	Gdiplus::Brush* _brLine{nullptr,};
	Gdiplus::Pen* _penInv{nullptr,};
	
	enum { eNumColorSet = 2};
	Gdiplus::Brush* _brCell[eNumColorSet][eCbR]{nullptr,};


	///[image cell
	SHP<Gdiplus::Image> _imgCell[eCbR];
	int _idPng[eCbR]{IDB_PNG_white, IDB_PNG_yellow,IDB_PNG_orange,IDB_PNG_red,IDB_PNG_blue,IDB_PNG_green,};

	SHP<Gdiplus::Image> _imgMargin[4];
	int _idMgn[4]{IDB_PNG_left, IDB_PNG_top,IDB_PNG_right,IDB_PNG_bottom,};
	SHP<Gdiplus::Image> _imgRevet;

	///]image cell




	CPoint _ptDraw{50,50};
	CRect _rcInvalid{0,0,0,0};
	void InvalidBoard(CRect rc = CRect(0,0,0,0));
	CRect BoadRect();
	/// how many space to move
	/// == _sq._unit -1 과 1차이남.
	int _unit{2}; //constructor에 초기화 하면 여기 초기값은 무시 된다.
	//LONGLONG _tikStart{0};
	int _multiSpeed{0};
	
	/// reset할때 0과 InterlockedExchange한다.
	//ULONG 
	KSyncBool _bReverseStop;//TRUE로 만들려면 InterlockedIncrement 한다.
	KSyncBool _bReplayStop;//TRUE로 만들려면 InterlockedIncrement 한다.
	KSyncBool _bClosedDlg;//TRUE로 만들려면 InterlockedIncrement 한다.
	HANDLE _eventClose{CreateEventW(0,1,0, L"Close")};
	//    2 bManualReset, 3 bInitialState,
	BOOL _bInReverse{0};//KSyncRunning
	BOOL _bInMove{0};

	BOOL _bLbDown{FALSE};
	CPoint _pDown{-1, -1};
	std::vector<CPoint> _arPt;

	/// 앱 수준에서 게임을 참조 하여 가지고 사용
	ShJObj _sjGame;

	double _rtMache{0.};
	enum { eTimerElapsed = 100, eTimerCountdown = 101,};
	int _msecInterval{1000};
#define MAX_Countdown 10
	int _countDown{MAX_Countdown};

	//?보류: _tikStart에 숫작 들어 가면 시작 이고, 멈추면 체크해서 100%이면 0이된다.
	bool _bStarted{false};

	//CString _initGGUID;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	afx_msg	void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnBnClickedMix();
	afx_msg void OnBnClickedClose();
	afx_msg void OnBnClickedSolve();
	afx_msg void OnBnClickedReset();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg	void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeColor();
	afx_msg void OnBnClickedStart();


public:
	void TransferScore(SHP<std::list<KMove>> slstMv);
	void TransferScore(std::list<KMove>& lstMv);

	UINT ReverseGame();

	void SetStaticBlink(int idc);
	bool IsMixed();
	bool IsStarted();
	void OnAfterMoved();
	bool IsMixedByHere();
	void InplaceRect(CRect& rc, CRect rcB);
	void InvalidatePart(int direction, int ln, int tilt = -1);
	CString MakeTimeStr(LONGLONG elp, bool bShort);
	
	void DisplayElapsed();

	void TrySameScramble(ShJObj sjGame);
	void StandbyToPlay(CString initGGUID = L"");

	void ReplaySample(ShJObj sjGame, int iOp = 0);
	UINT StartReplay(ShJObj sjGame, SHP<std::list<KMove>> lstRk);

	void StartGameCountdown(CString initGGUID);
	void StartGame(PWS initGGUID = NULL);

	afx_msg void OnCbnSelchangeReplayspeed();
};
