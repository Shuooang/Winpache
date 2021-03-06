
// MainDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include <afxdialogex.h>
#include <stdlib.h>
#include <cstdlib>

#include "KwLib64/GdiTool.h"
#include "KwLib64/Lock.h"
#include "KwLib64/KDebug.h"

#include "SuareSimul.h"
#include "MainDlg.h"
#include "DlgUser.h"
#include "DlgRanking.h"
#include <gdiplus.h>				// Base include
using namespace Gdiplus;			// The "umbella"
#pragma comment(lib, "gdiplus.lib")	// The GDI+ binary

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#ifdef _code_piece
KwBeginInvoke(this, ([&, initGGUID]()-> void {
	}));

CMainPool::QueueFunc([&, sjGame, lstRk]()
	{
	});
#endif // _code_piece


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CMainDlg dialog


CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/)
	: CDlgInvokable(IDD_SUARESIMUL_DIALOG, pParent)
	/// , _unit(0) 여기서 초기화 class내 {2} 초기화는 무시 된다.
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

#ifdef _DEBUG
	JObj jo;
	jo("k1") = "한글";
	jo("e1") = L"abc";
	jo("n1") = 123;
	CStringA jreq = jo.ToJsonStringUtf8();

#endif // _DEBUG

}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDlgInvokable::DoDataExchange(pDX);
	if(!pDX->m_bSaveAndValidate)// save, write, FALSE
	{
		_unit = _sq._unit - 1;
		_multiSpeed = _sq._multiSpeed - 1;
	}
	DDX_CBIndex(pDX, IDC_COMBO1, _unit);
	DDX_CBIndex(pDX, IDC_ReplaySpeed, _multiSpeed);

	//
	if(pDX->m_bSaveAndValidate)// save, write, FALSE
	{
		_sq._unit = _unit + 1;
		_sq._multiSpeed = _multiSpeed + 1;
	}
// 	CString s = _sq._unit == 1 ? L"Space" : L"Spaces";
// 	SetDlgItemText(IDC_Spaces, s);
	DDX_CBIndex(pDX, IDC_Color, _sq._setCl);
}


void CMainDlg::OnCancel()
{
	if(!IsStarted())
		SetEvent(_eventClose);
#ifndef _DEBUG
	if(MessageBox(KwRsc(IDS_WillYouClose), L"Exit!", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
#endif // _DEBUG
		__super::OnCancel();
}


BEGIN_MESSAGE_MAP(CMainDlg, CDlgInvokable)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Mix, &CMainDlg::OnBnClickedMix)
	ON_BN_CLICKED(IDC_Close, &CMainDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_Solve, &CMainDlg::OnBnClickedSolve)
	ON_BN_CLICKED(IDC_Reset, &CMainDlg::OnBnClickedReset)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CMainDlg::OnCbnSelchangeCombo1)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_Color, &CMainDlg::OnCbnSelchangeColor)
	ON_BN_CLICKED(IDC_Start, &CMainDlg::OnBnClickedStart)
	ON_CBN_SELCHANGE(IDC_ReplaySpeed, &CMainDlg::OnCbnSelchangeReplayspeed)
END_MESSAGE_MAP()


// CMainDlg message handlers
void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	//if((nID & 0xFFF0) == IDM_ABOUTBOX) 0xFFF0 으로 mask 하면 다 16 된다.
	auto app = (CSuareSimulApp*)GetMainApp();
	auto& appd = (app)->_docApp;
	if((nID & 0xFFFF) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if((nID & 0xFFFF) == IDM_User)
	{
		DlgUser dlg(this);
		{
			AUTOLOCK(appd._csAppDoc);
			dlg._email = appd._json->S("email");
			dlg._SNS = appd._json->S("SNS");
			dlg._Nickname = appd._json->S("Nickname");
		}
		if(dlg.DoModal() == IDOK)
		{
// 			AUTOLOCK(appd._csAppDoc);
// 			auto sjUsr = make_shared<JObj>(appd._json);
// 			sjUsr->DeleteKey("GamesRun");
// 			int rv = appd.CubeRegisterUser(sjUsr, false);
// 			if(rv == 0)
// 			{
// 				(*appd._json)("email") = dlg._email;
// 				(*appd._json)("SNS") = dlg._SNS;
// 				(*appd._json)("Nickname") = dlg._Nickname;
// 			}
// 			else if(rv == -10)//Nickname duplicated
// 			{
// 				KwMessageBoxError(KwRsc(IDC_NicknameDuplicated));
// 			}
		}
	}
	else if((nID & 0xFFFF) == IDM_Ranking)
	{
		DlgRanking dlg(this);
		auto id = dlg.DoModal();
		if(id == IDCANCEL || dlg._iSel < 0)
			return;

		auto sjv = dlg._sjTbl->GetAt(dlg._iSel);	if(!sjv->IsObject()) return;
		auto sjGame = sjv->AsObject();				if(!sjGame) return;
		if(id == IDC_ReplaySample)
		{
			//appd.CubeGetGameActions(..)
			ReplaySample(sjGame);
		}
		else if(id == IDC_TrySame)
		{
			//_initGGUID == fgguid;
			TrySameScramble(sjGame);
		}
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
BOOL CMainDlg::OnInitDialog()
{
	CDlgInvokable::OnInitDialog();
	_bClosedDlg.Reset();/// 이거는 여기서 단한번 실행 되므로 안해도 된다.
	TRACE("ResetEvent(_eventClose);\n");
	ResetEvent(_eventClose);

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if(pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if(!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
			pSysMenu->AppendMenu(MF_STRING, IDM_User, KwRsc(IDS_UserInformation));
			pSysMenu->AppendMenu(MF_STRING, IDM_Ranking, KwRsc(IDS_Ranking));
		}
	}


	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon




	KwEnableWindow(this, IDC_Reset, TRUE);
	KwEnableWindow(this, IDC_Solve, FALSE);
	KwEnableWindow(this, IDC_Start, FALSE);
	GetDlgItem(IDC_ReplaySpeed)->ShowWindow(SW_HIDE);

	/// 각셀의 Rect를 초기화 한다.
	_sq.InitCellRects();

	//srand(9470739); Initinstance로
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.


void CMainDlg::OnOK()
{
	__super::OnOK();
}

void CMainDlg::InvalidBoard(CRect rc)
{
	CRect rcb = rc.Width() > 0 ? rc : BoadRect();
	_rcInvalid = rcb;
	InvalidateRect(rcb, 0);
}

/// draw area
/// left top은 margin 만큼 후퇴 한다. 그래서 윈도우 전체 위치가 셀이 딱 떨어지게 한다.
CRect CMainDlg::BoadRect()
{
	//int nrw = _sq.Narrow();
	CRect rcb(CPoint(_ptDraw.x - _sq._ltMargin.cx, _ptDraw.y - _sq._ltMargin.cy),
		CSize((_sq._size.cx * _sq._col) + (_sq._ltMargin.cx + _sq._rbMargin.cx), 
			(_sq._size.cy * _sq._row) + (_sq._ltMargin.cy + _sq._rbMargin.cy)));
	return rcb;
}


void CMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // device context for painting


		CRect rcc = BoadRect();
		CAutoSmoothDraw _asd(rcc, &dc);//, RGB(127, 127, 127));// dc.GetBkColor(검은색));
		CDC* pDC = _asd.GetDrawDC();
		_hdc = pDC->GetSafeHdc();
		//DeleteMeSafe(_gc); 마지막에 한번만 해주나? 이거 넣으니 죽는데.
		_gc = ::new Graphics(_hdc);
		KAtEnd d_gc([&]() {::delete _gc; });

		static Color set1[2][eCbR] = {
			{//excel 색
				Color(255, 255, 255),
				Color(255, 213,   0),
				Color(255,  88,   0),
				Color(183,  18,  52),
				Color(0,  70, 173),
				Color(0, 144,  72),
// 				Color(198, 224, 180),
// 			Color(117, 196, 255),
// 			Color(213, 150, 252),
		},
			{//표준 큐브색
				Color(255, 255, 204),
				Color(255, 217, 102),
				Color(244, 176, 132),
				Color(198, 224, 180),
				Color(117, 196, 255),
				Color(213, 150, 252),
// 						Color(183,  18,  52),
// 					Color(0,  70, 173),
// 					Color(0, 144,  72),
		}};

		if(_brCell[0][0] == nullptr)
		{
			_brBG = ::new SolidBrush(Color(220, 220, 220));
			_brBox = ::new SolidBrush(Color(114, 115, 107));
			_brLine = ::new SolidBrush(Color( 34, 25, 27)); //255, 0, 0));//디버그때
			for(int iset = 0; iset < eNumColorSet; iset++)
				for(int i = 0; i < eCbR; i++)
					_brCell[iset][i] = ::new SolidBrush(set1[iset][i]);
			Color clp(255, 0, 255);
			_penInv = ::new Gdiplus::Pen(clp, 3);
		}



		/// 바탕색
		Gdiplus::Rect rcbg(0,0, rcc.Width(), rcc.Height());
		_gc->FillRectangle(_brBG, rcbg);



//		TRACE("Paint \n");
		int du = _sq.Narrow();
		//CRect rcb = BoadRect(); 이건 메로리 DC안쓸때.
		CRect rcb(CPoint(_sq._ltMargin.cx, _sq._ltMargin.cy), CSize(_sq._size.cx * _sq._col, _sq._size.cy * _sq._row));

		Gdiplus::Rect rct(rcb.left, rcb.top, rcb.Width(), rcb.Height());
		_gc->FillRectangle(_brBox, rct);

		//int wgrid = _sq._size.cx;// +du;//_sq.sq[0][0]
		/// brush 방식
		if(_sq._setCl > 0)
		{
			Brush* br = nullptr;
			for(int r = 0; r < _sq._row; r++)
			{
				for(int c = 0; c < _sq._col; c++)
				{
					int icl = _sq.sq[r][c];
					br = _brCell[_sq._setCl - 1][icl];
					///             셀의 상대위치(0,0에서 시작)   그릴때마진적용       움직임       정도에 10% 에넓이를 곱한거
					CPoint lt((int)((double)_sq._left[r][c] + _sq._ltMargin.cx + (_sq._hor[r] * abs(_sq._hor[r]) * 0.1 * _sq._size.cx)),// -1 * 0.25 * 40
						(int)((double)_sq._top[r][c] + _sq._ltMargin.cy + (_sq._ver[c] * abs(_sq._ver[c]) * 0.1 * _sq._size.cy)));
					CRect rccb(lt, _sq._size);
					//Gdiplus::Rect rct(rcb.left + (du / 2), rcb.top + (du / 2), rcb.Width() - du, rcb.Height() - du);
					Gdiplus::Rect rc1(rccb.left + (du / 2), rccb.top + (du / 2), rccb.Width() - du, rccb.Height() - du);
					// 				if(c == 0 && r == 0)
					// 					TRACE("Paint:Y(%d => %d) _ver[%d](%d)\n", _sq._top[r][c], y, c, _sq._ver[c]);
					if(_sq.IsStaticLine(r) && _sq.IsStaticLine(c))
						_gc->FillEllipse(br, rc1);
					else
						_gc->FillRectangle(br, rc1);

#ifdef _DEBUGxxx
					if(c == 0) // 표준 색 바 왼쪽
					{
						Brush* brv = _brCell[_sq._setCl][r];
						Gdiplus::Rect rcb(_sq._left[r][c] - 30, _sq._top[r][c] + (du / 2), _sq._size / 2, _sq._size - du);
						_gc->FillRectangle(brv, rcb);
					}
#endif // _DEBUGxxx

					// 칸수에 따른 진한 칼러 구분 라인 검은색
					if((c % _sq._unit) == (_sq._unit - 1) && c < (_sq._col - 1))
					{
						Gdiplus::Rect rcln(rccb.right - (du / 2), rccb.top, du, rccb.Height());
						//TRACE("(%d,%d): %d = c:%d %% ut:%d == %d (%d,%d,%d,%d)\n",c,r, (c% _sq._unit), c, _sq._unit, _sq._unit - 1, rcln.X, rcln.Y, rcln.Width, rcln.Height);
						_gc->FillRectangle(_brLine, rcln);
					}
					if((r % _sq._unit) == (_sq._unit - 1) && r < (_sq._row - 1))
					{
						if(c == 2 && r == 2)
							_break;
						Gdiplus::Rect rcln(rccb.left, rccb.bottom - (du / 2), rccb.Width(), du);
						//TRACE("(%d,%d): %d = r:%d %% ut:%d == %d (%d,%d,%d,%d)\n", c, r, (r % _sq._unit), r, _sq._unit, _sq._unit - 1, rcln.X, rcln.Y, rcln.Width, rcln.Height);
						_gc->FillRectangle(_brLine, rcln);
					}
				}
			}
		}
		else
		{
			for(int i = 0; i < 4 && !_imgMargin[i]; i++)
				_imgMargin[i] = make_shared<Gdiplus::Image>(KwCreateStreamOnResource(_idMgn[i]));
			for(int i = 0; i < eCbR && !_imgCell[i]; i++)
				_imgCell[i] = make_shared<Gdiplus::Image>(KwCreateStreamOnResource(_idPng[i]));
			_imgRevet = make_shared<Gdiplus::Image>(KwCreateStreamOnResource(IDB_PNG_rivet));

			for(int r = 0; r < _sq._row; r++)
			{
				for(int c = 0; c < _sq._col; c++)
				{
					int icl = _sq.sq[r][c];
					///             셀의 상대위치(0,0에서 시작)   그릴때마진적용       움직임       정도에 10% 에넓이를 곱한거
					CPoint lt((int)((double)_sq._left[r][c] + _sq._ltMargin.cx + (_sq._hor[r] * abs(_sq._hor[r]) * 0.1 * _sq._size.cx)),// -1 * 0.25 * 40
						(int)((double)_sq._top[r][c] + _sq._ltMargin.cy + (_sq._ver[c] * abs(_sq._ver[c]) * 0.1 * _sq._size.cy)));
					CRect rccb(lt, _sq._size);
					//Gdiplus::Rect rc1(rccb.left + (du / 2), rccb.top + (du / 2), rccb.Width() - du, rccb.Height() - du);
					Gdiplus::Rect rc1(rccb.left, rccb.top, rccb.Width(), rccb.Height());

					Image* gimg = _imgCell[icl].get();
					_gc->DrawImage(gimg, rc1);

					if(_sq.IsStaticLine(r) && _sq.IsStaticLine(c))
					{
						int w = rc1.Width;	int h = rc1.Height;
						int szr = 7;	int mg = 7;
						Gdiplus::Rect rcrv1(rc1.X + mg, rc1.Y + mg, szr, szr);
						_gc->DrawImage(_imgRevet.get(), rcrv1);
						Gdiplus::Rect rcrv2(rc1.X + w - mg - szr, rc1.Y + mg, szr, szr);
						_gc->DrawImage(_imgRevet.get(), rcrv2);
						Gdiplus::Rect rcrv3(rc1.X + mg, rc1.Y + h - mg - szr, szr, szr);
						_gc->DrawImage(_imgRevet.get(), rcrv3);
						Gdiplus::Rect rcrv4(rc1.X + w - mg - szr, rc1.Y + h - mg - szr, szr, szr);
						_gc->DrawImage(_imgRevet.get(), rcrv4);
					}

					const int cmg = 3;
					if(r == 0)
					{
						gimg = _imgMargin[eTop].get();
						Gdiplus::Rect rcmg(rc1.X, rc1.Y - cmg, rc1.Width, cmg);
						_gc->DrawImage(gimg, rcmg);// , 50, 50);
					}
					else if(r == _sq._row - 1)
					{
						gimg = _imgMargin[eBottom].get();
						Gdiplus::Rect rcmg(rc1.X, rc1.GetBottom(), rc1.Width, cmg);
						_gc->DrawImage(gimg, rcmg);// , 50, 50);
					}

					if(c == 0)
					{
						gimg = _imgMargin[eLeft].get();
						Gdiplus::Rect rcmg(rc1.X - cmg, rc1.Y, cmg, rc1.Height);
						_gc->DrawImage(gimg, rcmg);// , 50, 50);
					}
					else if(c == _sq._col - 1)
					{
						gimg = _imgMargin[eRight].get();
						Gdiplus::Rect rcmg(rc1.GetRight(), rc1.Y, cmg, rc1.Height);
						_gc->DrawImage(gimg, rcmg);// , 50, 50);
					}
				}
			}
		}//if(_sq._setCl > 0)

#ifdef _try_image_test
		Image gimg(KwCreateStreamOnResource(IDB_PNG_blue));

		/// IDB_PNG_red는 실패 BMP로 바꿔서 하니 됨.
		//img.LoadFromResource(AfxGetApp()->m_hInstance, IDB_BITMAP_orange); // OK
#ifdef _try_CImage
		CImage img;
		img.Load(gimg);
		img.Draw((HDC)_hdc, 40, 40, 50, 50);
		HBITMAP hb = (HBITMAP)img;
#endif // _try_CImage

		_gc->DrawImage(&gimg, 40, 40);// , 50, 50);
#endif // _try_image_test
		//LoadPNGFromStaticRes(GetModuleHandle(0), IDB_PNG_red, Image** ppImg)

		_gc->Flush(FlushIntention::FlushIntentionSync);
		CDlgInvokable::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMainDlg::OnBnClickedClose()
{

	OnCancel();
}






// #define PutCell(r, c) \
// 	SetDlgItemInt(IDC_Cell_##r##c, (UINT)_sq.sq[r][c])

void CMainDlg::InplaceRect(CRect& rc1, CRect rcB)
{
	if(rc1.left < rcB.left)
		rc1.left = rcB.left;
	if(rc1.top < rcB.top)
		rc1.top = rcB.top;
	if(rc1.right > rcB.right)
		rc1.right = rcB.right;
	if(rc1.bottom > rcB.bottom)
		rc1.bottom = rcB.bottom;
}
void CMainDlg::PopulateCol(int col, int iAny)
{
	if(_bClosedDlg)
		return;
	int c = col;
	int r = 0;
	CRect rcB = BoadRect();

//	CPoint lt(_ptDraw.x + _sq._ltMargin.cx, _ptDraw.y + _sq._ltMargin.cy);
	CRect rc1(CPoint(_sq._leftInit[r][c] + _sq._ltMargin.cx + rcB.left, _sq._topInit[r][c] + _sq._ltMargin.cy + rcB.top),
		CSize(_sq._size.cx, _sq._size.cy * _sq._row));// _sq._size, _sq._size);
// 	CRect rc1(CPoint(_sq._leftInit[r][c] - _sq._ltMargin.cx + lt.x, _sq._topInit[r][c] - _sq._ltMargin.cy + lt.y),
// 		CSize(_sq._size.cx, _sq._size.cy * _sq._row));// _sq._size, _sq._size);
	InplaceRect(rc1, rcB);
	//TRACE("PopulateCol:%2d (%d) (%d, %d, %d, %d)\n", iAny, col, rc1.left, rc1.top, rc1.Width(), rc1.Height());
	InvalidBoard(rc1);
}
void CMainDlg::PopulateRow(int row)
{
	if(_bClosedDlg)
		return;
	int c = 0;
	int r = row;
//	int du = _sq.Narrow();
	CRect rcB = BoadRect();

	//CPoint lt(_ptDraw.x + _sq._ltMargin.cx, _ptDraw.y + _sq._ltMargin.cy);
	CRect rc1(CPoint(_sq._leftInit[r][c] + _sq._ltMargin.cx + rcB.left, _sq._topInit[r][c] + _sq._ltMargin.cy + rcB.top),
		CSize(_sq._size.cx * _sq._col, _sq._size.cy));// _sq._size, _sq._size);
// 	CRect rc1(CPoint(_sq._leftInit[r][c] - _sq._ltMargin.cx + lt.x, _sq._topInit[r][c] - _sq._ltMargin.cy + lt.y),
// 		CSize(_sq._size.cx * _sq._col, _sq._size.cy));// _sq._size, _sq._size);
	InplaceRect(rc1, rcB);
	//TRACE("PopulateRow(%d) (%d, %d, %d, %d)\n", row, rc1.left, rc1.top, rc1.Width(), rc1.Height());
	InvalidBoard(rc1);
}
void CMainDlg::Populate()
{
	if(_bClosedDlg)
		return;
	ASSERT(IDC_Cell_04 == 1004);
	ASSERT(IDC_Cell_32 == 1032);
	ASSERT(IDC_Cell_58 == 1058);
	for(int r=0;r<_sq._row;r++)
	{
		for(int c = 0; c < _sq._col; c++)
		{
			UINT idc = 1000 + (r * 10) + c;
			//SetDlgItemInt(idc, (UINT)_sq.sq[r][c]);
		}
	}
	InvalidBoard();
}


void CMainDlg::OnBnClickedMix()
{
	_bReverseStop.Set("true: 역재생 중지, false: 역재생 중");// button이 diable 되어 있으니 여기 안오지만 혹시
	_bReplayStop.Set("true: 재현 중지, false: 재현 중");// 재현 중에 Mix를 누르면

	//_initGGUID.Empty();
	//KillTimer(eTimerElapsed);
	PauseTimer("eTimerCountdown");
	PauseTimer("eTimerElapsed");
	//               54     2는 그냥 변수
	const int cmul = 3; // 1이면 너무 적다
	int seed = (_sq._row * _sq._col) * cmul;// *_sq._unit;
	int r1 = std::rand();
	//TRACE("Mix: %d = (%d * 3) + (%d %% %d)\n", n, seed, r1, seed);
#ifdef _DEBUG
	seed = 10;
	//n = 20 ;
#endif // _DEBUG
	int n = seed + (r1 % seed);
	_sq.Mix(n);
	StandbyToPlay();
}

void CMainDlg::TrySameScramble(ShJObj sjGame)
{
	auto fgguid = sjGame->SN("fgguid");		if(!fgguid) return;
	auto fcube = sjGame->SN("fcube");		if(!fgguid) return;
	//KillTimer(eTimerElapsed);
	PauseTimer("eTimerElapsed");//혹시
	_sq.Mix(fcube);
	StandbyToPlay(fgguid);
}

void CMainDlg::ReplaySample(ShJObj sjGame, int iOp)
{
	if(iOp == 0)
	{
		UpdateData();
		SetDlgItemText(IDC_Time, L"");
		GetDlgItem(IDC_ReplaySpeed)->ShowWindow(SW_SHOW);
		PauseTimer("eTimerElapsed");//혹시
		PauseTimer("eTimerCountdown");

		CMainPool::QueueFunc([&, sjGame]()
			{	ReplaySample(sjGame, 1);	});
		return;
	}
	// iOp == 1
	auto app = (CSuareSimulApp*)GetMainApp();
	auto& appd = (app)->_docApp;
	//AUTOLOCK(appd._csAppDoc);

	ShJArr sjTbl;
	if(!sjGame->Len("fgguid"))
	{
		KwMessageBoxError(L"Game data is missing!");
		return;
	}
	int rv = appd.CubeGetGameActions(sjGame->S("fgguid"), sjTbl);
	if(rv != 0 ||!sjTbl) 
		return;
	auto lstRk = make_shared<std::list<KMove>>();	
	for(auto& sjv : *sjTbl)
	{//a.findex, a.fdirect, a.fline, a.fspace, a.felapsed 
		auto sjo = sjv->AsObject();
		KMove mv;
		mv.dr = sjo->I("fdirect");
		mv.l = sjo->I("fline");
		mv.n = sjo->I("fspace");
		mv.tik = sjo->I("felapsed");
		lstRk->push_back(mv);
	}
	StartReplay(sjGame, lstRk);
}

/// <summary>
/// 게임 시작 10초전으로 카운드 다운 시작 된다.
/// </summary>
void CMainDlg::StandbyToPlay(CString initGGUID)
{
	Populate();
	KwEnableWindow(this, IDC_Start, TRUE);
	_countDown = 10;
	//SetTimer(eTimerCountdown, 1000, NULL);
	SetTimerLambda("eTimerCountdown", 1000, [&, initGGUID](int ntm, PAS tmk)
		{
			KTrace(L"%d. %s (%s)\n", ntm, Pws(tmk), __FUNCTIONW__);
			StartGameCountdown(initGGUID);
		});
}
void CMainDlg::StartGameCountdown(CString initGGUID)
{
	CString s;
	_countDown--;
	if(_countDown > 0)
	{
		s.Format(L"%s %d    ", KwRsc(IDS_Standby), _countDown);//초전
	}
	else // start
	{
		//KillTimer(eTimerCountdown);
		PauseTimer("eTimerCountdown");
		s = KwRsc(IDS_Start);// L"Start";
		KwBeginInvoke(this, ([&, initGGUID]()-> void {
			StartGame(initGGUID);
			}));
	}
	SetDlgItemText(IDC_Time, s);
	if(_countDown == 0 || _countDown == (MAX_Countdown-1))
		SetStaticBlink(IDC_Time);
}
void CMainDlg::OnBnClickedStart()
{
	StartGame();
}
void CMainDlg::StartGame(PWS initGGUID)
{
	KwEnableWindow(this, IDC_COMBO1, FALSE);
	KwEnableWindow(this, IDC_Mix, FALSE);
	KwEnableWindow(this, IDC_Solve, TRUE && _sq._mix.size() > 0);///TrySame 때는 disabled
	KwEnableWindow(this, IDC_Reset, TRUE);
	KwEnableWindow(this, IDC_Start, FALSE);

	//KillTimer(eTimerCountdown);
	PauseTimer("eTimerCountdown");
	_bStarted = true;
	auto app = (CSuareSimulApp*)GetMainApp();
	auto& appd = (app)->_docApp;
	AUTOLOCK(appd._csAppDoc);

	_rtMache = 0;
	_sq._tikStart = KwGetTickCount100Nano();
	_msecInterval = 1000;
	CStringA cb, sbuf;
	char* buf = sbuf.GetBuffer(8);

	for(int r = 0;r<_sq._row;r++)
	{
		for(int c = 0; c < _sq._col; c++)
		{
			cb += KwItoaA(_sq.sq[r][c], buf, 8);
			cb += (c < _sq._col - 1) ? "," : "\n";
		}
	}//	s.ReleaseBuffer();
	_sjGame = appd.AddGame(cb, _sq._unit, _sq._mode, initGGUID);

	//SetTimer(eTimerElapsed, _msecInterval, NULL);
	SetTimerLambda("eTimerElapsed", 1000, [&](int ntm, PAS tmk)
		{
			KTrace(L"%d. %s (%s)\n", ntm, Pws(tmk), __FUNCTIONW__);
			DisplayElapsed();
		});

}


#ifdef _use_background_thread_1
UINT MyThreadProc(LPVOID pParam)
{
	CMainDlg* th = (CMainDlg*)pParam;
	th->ReverseGame();
}
#endif // _use_background_thread_1

void CMainDlg::PopulateDirection(int direction, int ln)
{
	switch(direction)
	{
	case 0: PopulateRow(ln); break;
	case 1: PopulateRow(ln); break;
	case 2: PopulateCol(ln); break;
	case 3: PopulateCol(ln); break;
	}
}
UINT CMainDlg::StartReplay(ShJObj sjGame, SHP<std::list<KMove>> lstRk)
{
	KSyncRunning sr_(_bInReverse);
	BACKGROUND(1);
	_bReplayStop.Reset();

	CString scmb = sjGame->S("fcube");
	TRACE(L"%s\n", scmb);
	_sq.Mix(scmb);
	KwBeginInvoke(this, ([&]()-> void
		{
			SetDlgItemText(IDC_Time, KwRsc(IDS_Standby));
			SetDlgItemText(IDC_Count, L"");
			InvalidBoard();	
			SetStaticBlink(IDC_Time);
		}));
	Sleep(3000);
	auto nmix = lstRk->size();
	_sq.Replay(*lstRk, false, true, [&, nmix](KMove& mv, int iFram, int idx) -> int
		{
			if(_bReplayStop || _bClosedDlg) return 0; //_bReverseStop || 
			KwBeginInvoke(this, ([&, mv]()-> void
				{
					if(_bReplayStop || _bClosedDlg) return;//_bReverseStop || 
					PopulateDirection(mv.dr, mv.l);
					if(iFram == 0)//첫프레임일때만
					{
						CString s; KwItoaAW(nmix - idx - 1, s.GetBuffer(20), 20);
						SetDlgItemText(IDC_Time, s);
					}
				}));
			int rms = (int)(nmix - idx);
			int slp = rms < 10 ? 200 : rms < 20 ? 100 : rms < 30 ? 50 : 10;
			Sleep(slp);
			return 1;
		});

	if(_bReplayStop || _bClosedDlg) 
	{	
		if(_bClosedDlg)
		{
			SetEvent(_eventClose);
			Sleep(5);		//return 0;
		}
	}
	else
	{
		KwBeginInvoke(this, ([&]()-> void
			{
				if(_bClosedDlg) return;
				KwEnableWindow(this, IDC_Mix, TRUE);
				//KwMessageBox(this, L"Done!");
				GetDlgItem(IDC_ReplaySpeed)->ShowWindow(SW_HIDE);
				SetDlgItemText(IDC_Time, KwRsc(IDS_ReplayDone));// L"Replay done!");
				SetDlgItemText(IDC_Count, L"");
				InvalidBoard();
				SetEvent(_eventClose); Sleep(5);
				SetStaticBlink(IDC_Time);
			}));//?beginInvoke 4
	}
	return 0;   // thread completed successfully
}

#ifdef _DEBUG
#define REVERSE_AS_GAME
#endif // _DEBUG


UINT CMainDlg::ReverseGame()
{
	BACKGROUND(1);
	KSyncRunning sr_(_bInReverse);
	//_sq.Reverse([&](int direction, int ln, int tilt) -> int
	auto slstMv = make_shared<std::list<KMove>>();
	LONGLONG tik = 0;
	auto nmix = _sq._mix.size();
	_sq.Replay(_sq._mix, true, false, [this, slstMv, &tik, nmix](KMove& mv, int iFram, int idx) -> int
		{
			/// 매 프레임 마다 불린다.
			if(_bReverseStop || _bClosedDlg)
				return 0;
			KwBeginInvoke(this, ([&, mv, nmix, iFram, idx]()-> void
				{
					if(_bReverseStop || _bClosedDlg) return;
					PopulateDirection(mv.dr, mv.l);
					if(iFram == 0)//첫프레임일때만
					{
						CString s; KwItoaAW(nmix - idx - 1, s.GetBuffer(20), 20);
						SetDlgItemText(IDC_Time, s);
					}
				}));
	#ifdef REVERSE_AS_GAME
			if(idx == 0)
			{
				tik += 10000000;
				KMove mv1 = mv;// = it;
				mv1.tik = tik;
				slstMv->push_back(mv1);
				_sq._tElapsed = tik;
			}
	#endif // _DEBUG
			int rms = (int)(nmix - idx);
			int slp = rms < 10 ? 200 : rms < 20 ? 100 : rms < 30 ? 50 : 10;
			Sleep(slp);// (int)dsl);//100 nano -> 1 mili
			return 1;
		});

#ifdef REVERSE_AS_GAME /// 개발용 어려운 스크램블을 역순으로 푸는거 샘플 데이터로 생성 하기위해
		_rtMache = _sq.CheckEx();// 다 마추었나
		if(_bReverseStop || _bClosedDlg)
			return 0;
		if(_rtMache == 1.)
		{
			_sq._tikStart = 0;///?이게 중요. IsStarted()의 요소가 된다.
			_bStarted = false;//?보류
			CMainPool::QueueFunc([&, slstMv]()
				{ TransferScore(slstMv); });
		}
#endif // _DEBUG // 개발용 어려운 스크램블을 역순으로 푸는거 샘플 데이터로 생성 하기위해
	//TRACE("ReverseGame _sq.Reverse before end _bClosedDlg(%d)\n", _bClosedDlg._val);
	if(!_bClosedDlg)
	{
		TRACE("ReverseGame if(!_bClosedDlg) _bClosedDlg(%d)\n", _bClosedDlg._val);
		_KwBeginInvoke(this, [&]()-> void
			{
				TRACE("ReverseGame KwBeginInvoke 2 _bReverseStop(%d),_bClosedDlg(%d)\n", _bReverseStop._val, _bClosedDlg._val);
				if(_bClosedDlg)
				{
					TRACE("ReverseGame 2 if(_bClosedDlg(%d)) return\n", _bClosedDlg._val);
					return;
				}
				SetDlgItemText(IDC_Time, KwRsc(IDS_Finished));
				InvalidBoard();
				TRACE("ReverseGame KwBeginInvoke SetEvent(_eventClose);\n");
				SetEvent(_eventClose);
				Sleep(5);

				SetStaticBlink(IDC_Time);

			});//?beginInvoke 4
		//auto p = u8"asdfkasdfk / zlxdskfj";
		KwEnableWindow(this, IDC_Mix, TRUE);
	}
	else
	{
		//TRACE("ReverseGame at end _bClosedDlg(%d): SetEvent(_eventClose);\n", _bClosedDlg._val);
		SetEvent(_eventClose);
		Sleep(5);
	}
	return 0;   // thread completed successfully
}
void CMainDlg::SetStaticBlink(int idc)
{
	const int nCount = 8;
	SetTimerLambda("eTimerFinished", 75, [&, idc](int ntm, PAS tmk) -> void
		{
			auto ctm = (CStatic*)GetDlgItem(idc);
			CString s, s1;
			GetDlgItemText(idc, s);
			s.Trim();
			if(ntm == (nCount -1))
				s1 = s;
			else if(ntm % 2 == 0)
				s1.Format(L"%s ", s);
				//ctm->ModifyStyle(0, SS_BLACKRECT, 0);
			else
				s1.Format(L" %s", s);
				//ctm->ModifyStyle(SS_BLACKRECT, 0);
			SetDlgItemText(idc, s1);
		}, nCount);

}
void CMainDlg::OnBnClickedSolve()
{
	PauseTimer("eTimerElapsed");
	PauseTimer("eTimerCountdown");//혹시

	_bReverseStop.Reset();

	KwEnableWindow(this, IDC_Solve, FALSE);

#ifdef _use_background_thread_1
	AfxBeginThread(MyThreadProc, this);
#else
	CMainPool::QueueFunc([&]()
		{
			ReverseGame();
		});

#endif // _use_background_thread_1
	SetDlgItemText(IDC_Time, L"");
	SetDlgItemText(IDC_Count, L"");
}

void CMainDlg::InvalidatePart(int direction, int ln, int nthFramme)
{
	KwBeginInvoke(this, ([&, direction, ln, nthFramme]()-> void {
		switch(direction)
		{
		case eLF: PopulateRow(ln); break;
		case eRT: PopulateRow(ln); break;
		case eUP: PopulateCol(ln); break;
		case eDN: PopulateCol(ln, nthFramme); break;
		}
		}));//?beginInvoke 4
	if(nthFramme == (_sq._nFramme - 1))//최종 프레임
	{
		OnAfterMoved();
	}

}

/// background task 1 : begin thread procedure
#ifdef _use_background_thread_1
UINT MyThreadProcMoveAny(LPVOID pParam)
{
	CMainDlg* th = (CMainDlg*)pParam;
	th->_sq.MoveAny(th->_sq._moveQ, [&, th](int direction, int ln, int tilt) -> void
		{
			th->InvalidatePart(direction, ln, tilt);//UI thread:특정 부분 다시 그리기
		});
	return 0;// 0:thread completed successfully
}
#endif // _use_background_thread_1
void CMainDlg::MoveAny(int edr, int rc, int n)
{
	KMove mv;
	mv.dr = edr; mv.l = rc; mv.n = n;
	mv.tik = KwGetTickCount100Nano() - _sq._tikStart;
	TRACE("MoveAny: %s(%d) %d\n", _sq._arDirection[edr], mv.l, mv.n);
	{
		AUTOLOCK(_sq._csMove);
		_sq._moveQ.push_back(mv);
	}

	/// background task 1 : begin thread
#ifdef _use_background_thread_1
	AfxBeginThread(MyThreadProcMoveAny, this);
#else
	/// background task 2 : thread pool
	CMainPool::QueueFunc([&]()
		{
			_sq.MoveAny(this->_sq._moveQ, [&](int direction, int ln, int tilt) -> void
				{
					if(_bClosedDlg) return;
					InvalidatePart(direction, ln, tilt);//UI thread:특정 부분 다시 그리기
				});//비동기 람다
			TRACE("MoveAny SetEvent(_eventClose);\n");
			SetEvent(_eventClose);
		});
#endif // _use_background_thread_1
	///Moved();/// 배경에서 아직 애니로 움직이는 중이라 이전 셀배열 이므로 애니 프레임 마지막꺼에로 옮김
}

BOOL CMainDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	UINT nID = LOWORD(wParam);
 	HWND hWndCtrl = (HWND)(lParam);    //Control handle
 	int nCode = HIWORD(wParam);              //Notification code
	if(nCode == BN_CLICKED)
	{
		bool bInGame = IsStarted();
		if(bInGame)
		{
			int ut = _sq._unit;
#ifdef _DEBUGxxOld
			if(IDC_Lf0 <= nID && nID <= IDC_Lf5)
			{
				_sq.lf(nID - IDC_Lf0, ut);
			}
			else if(IDC_Rt0 <= nID && nID <= IDC_Rt5)
			{
				_sq.rt(nID - IDC_Rt0, ut);
			}
			else if(IDC_Up0 <= nID && nID <= IDC_Up8)
			{
				int id0 = IDC_Up0;
				int col = nID - id0;
				_sq.up(col, ut);
			}
			else if(IDC_Dn0 <= nID && nID <= IDC_Dn8)
			{
				_sq.dn(nID - IDC_Dn0, ut);
			}

#endif // _DEBUGxxOld
			if(IDC_Lf0 <= nID && nID <= IDC_Dn8)
				OnAfterMoved();
		}
	}

	return CDlgInvokable::OnCommand(wParam, lParam);
}


void CMainDlg::OnBnClickedReset()
{
	_bReverseStop.Set("true: 역재생 중지, false: 역재생 중");
	_bReplayStop.Set("true: 재현 중지, false: 재현 중");
	PauseTimer("eTimerCountdown");
	PauseTimer("eTimerElapsed");
	_sq.Reset();

	CString s; s.Format(L"%s: %d", KwRsc(IDS_Count), _sq._count);
	SetDlgItemText(IDC_Count, s);
	SetDlgItemText(IDC_Time, KwRsc(IDS_Ready));// L"Ready...");

	Populate();
	KwEnableWindow(this, IDC_COMBO1, TRUE);
	KwEnableWindow(this, IDC_Mix, TRUE);
	SetStaticBlink(IDC_Time);
}


void CMainDlg::OnCbnSelchangeCombo1()
{
	UpdateData();
	OnBnClickedReset();
	/// <summary>
	/// _sq._unit이 바뀐거에 따라 위치 이동이 되었다.
	/// </summary>
// 	_sq.MoveUnit();

 	InvalidBoard();
}

void CMainDlg::DisplayElapsed()
{
	CString s;
	auto tik = KwGetTickCount100Nano();
	auto elp = tik - _sq._tikStart;

	if(_rtMache > 0.6)// 60% 넘게 맞추었을때는 밀리sec까지 보인다.
	{
		if(_msecInterval == 1000)
		{

			//KillTimer(eTimerElapsed);
			_msecInterval = 100;
			//SetTimer(eTimerElapsed, _msecInterval, NULL);
			ChangeInterval("eTimerElapsed", _msecInterval);

		}
		s = MakeTimeStr(elp, false);
	}
	else
	{
		if(_msecInterval == 100)
		{
			//KillTimer(eTimerElapsed);
			_msecInterval = 1000;
			//SetTimer(eTimerElapsed, _msecInterval, NULL);
			ChangeInterval("eTimerElapsed", _msecInterval);
		}
		s = MakeTimeStr(elp, true);
	}
	SetDlgItemText(IDC_Time, s);
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	//CString s;
	if(nIDEvent == eTimerElapsed)
	{
		//DisplayElapsed();
	}
 	else if(nIDEvent == eTimerCountdown) //lambda형식으로 바꿈 StandbyToPlay
 	{	//StartGameCountdown();
 	}
	CDlgInvokable::OnTimer(nIDEvent);
}

CString CMainDlg::MakeTimeStr(LONGLONG elp, bool bShort)
{
	CString s;
	double dt = (double)elp / 10000000.;/// 초단위로 만든다.	s.Format(L"%9.3f msec", );
	auto sec = KwRsc(IDS_Second);
	if(bShort)
	{
		if(dt < 60)
			s.Format(L"%9.0f %s", dt, sec);
		else
		{
			int m = (int)(dt / 60);
			double dt1 = dt - (m * 60);
			if(dt < 3600)
				s.Format(L"%02d:%02.0f %s", m, dt1, sec);
			else
			{
				int h = m / 60;
				int m1 = m - (h * 60);
				s.Format(L"%d:%02d:%02.0f %s", h, m1, dt1, sec);
			}
		}
	}
	else
	{
		if(dt < 60)
			s.Format(L"%9.3f sec", dt);
		else
		{
			int m = (int)(dt / 60);
			double dt1 = dt - (m * 60);
			if(dt < 3600)
				s.Format(L"%02d:%06.3f %s", m, dt1, sec);
			else
			{
				int h = m / 60;
				int m1 = m - (h * 60);
				s.Format(L"%d:%02d:%06.3f %s", h, m1, dt1, sec);
			}
		}
	}
	return s;
}

//?deprecated
bool CMainDlg::IsMixedByHere()
{
	return _sq._mix.size() > 0;// !GetDlgItem(IDC_Mix)->IsWindowEnabled();
}
void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	bool bInGame = IsStarted(); //IsMixedByHere() && 
	if(bInGame)
	{
		_bLbDown = TRUE;
		_arPt.push_back(point);
		//TRACE("Down: %d, %d\n", point.x, point.y);
	}
	CDlgInvokable::OnLButtonDown(nFlags, point);
}

void CMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	bool bInGame = IsStarted();
	if(bInGame)
	{
		if(_bLbDown)
		{
			CPoint pt = _arPt.back();
			//NTRACE("move: %d, %d\n", point.x, point.y, point.x - pt.x, point.y - pt.y);
			_arPt.push_back(point);///움직인 포인터를 모두 저장
		}
	}
	CDlgInvokable::OnMouseMove(nFlags, point);
}

void CMainDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	bool bInGame = IsStarted();
	CRect rcB = BoadRect();
	if(bInGame)
	{
		if(_bLbDown)
		{
			CSize tt;
			auto n = _arPt.size();
			for(int i = 1; i < n; i++)
			{
				CSize ds(_arPt[i].x - _arPt[i - 1].x, _arPt[i].y - _arPt[i - 1].y);
				tt.cx += ds.cx;///움직인 양을 모두 합쳐서 (평균구하려고)
				tt.cy += ds.cy;
// 				NTRACE("ds : %d = (%d - %d), %d = (%d - %d) : T(%d, %d)\n",
// 					_arPt[i].x - _arPt[i - 1].x, _arPt[i].x, _arPt[i - 1].x,
// 					_arPt[i].y - _arPt[i - 1].y, _arPt[i].y, _arPt[i - 1].y,
// 					tt.cx, tt.cy);
			}

			//TRACE("Up  : (%d, %d) npt(%d) total:(%d, %d)\n", point.x, point.y, n, tt.cx, tt.cy);
		
			//CPoint lt(_sq._left[0][0], _sq._top[0][0]);/// 일단 Left Top 구석 좌표를 알아내고
			/// 일단 Left Top 구석 좌표를 알아내고
			CPoint lt(rcB.left + _sq._ltMargin.cx, rcB.top + _sq._ltMargin.cy);
			//int du = _sq.Narrow();
			//int wgrid = _sq._size;// +du;//_sq.sq[0][0]


			if(abs(tt.cx) < 1 && abs(tt.cy) < 1) /// 토탈 임직인게 2칸이하면 그냥 터치네.
			{
				///그럼 현재 point가 어디 눌렸나 봐서 움직여야지.
				CPoint cel((point.x - lt.x) / _sq._size.cx, (point.y - lt.y) / _sq._size.cy);/// Left Top 셀(행렬)을 알아내고

				///~~ 아... 구석 누르면 가로인제 세로인지 모르겠구나.
			}
			else if(IsStarted())
			{
				double dx = (double)tt.cx / n;///움직임의 평균을 구한다.
				double dy = (double)tt.cy / n;

				/// 터치 시작한 위치 행렬
				CPoint cel((_arPt[0].x - lt.x) / _sq._size.cx, (_arPt[0].y - lt.y) / _sq._size.cy);/// Left Top 셀(행렬)을 알아내고
				///if(abs(dx) > 1 || abs(dy) > 1) 잘게 길게 움직여도 false
				if(abs(tt.cx) > 5 || abs(tt.cy) > 5)
				{
					double rtX = dy == 0. ? 100. : abs(dx) / abs(dy);
					double rtY = dx == 0. ? 100. : abs(dy) / abs(dx);
					auto tik = KwGetTickCount100Nano();
					auto elp = tik - _sq._tikStart;
					if(rtY >= 2.)/// 세로로 많이 움직였으면. 가로 세로 비가 3배 이상
					{
						if(!_sq.IsStaticLine(cel.x))
						{
							_sq._tElapsed = elp;
							if(dy > 0)
								MoveAny(eDN, cel.x, _sq._unit);//_sq.dn(cel.x, _sq._unit, 1);
							else
								MoveAny(eUP, cel.x, _sq._unit);//_sq.up(cel.x, _sq._unit, 1);
						}
					}
					else if(rtX >= 2.)
					{
						if(!_sq.IsStaticLine(cel.y))
						{
							_sq._tElapsed = elp;
							if(dx > 0)
								MoveAny(eRT, cel.y, _sq._unit);//_sq.rt(cel.y, _sq._unit, 1);
							else
								MoveAny(eLF, cel.y, _sq._unit);//_sq.lf(cel.y, _sq._unit, 1);
						}
					}
				}
			}
			_bLbDown = FALSE;
			_arPt.clear();
		}
	}

	CDlgInvokable::OnLButtonUp(nFlags, point);
}


bool CMainDlg::IsMixed()
{
	return _sq._mixOnly.size() > 0;
}
bool CMainDlg::IsStarted()
{
	return _sq._tikStart > 0;//&& _bStarted;
}

/// <summary>
/// 페인트에 관여 안하고, 다른 출력과 상황만 체크
/// </summary>
void CMainDlg::OnAfterMoved()
{
	if(!IsStarted())//!IsMixed() || 
		return;/// 아직 시작 안햇잖아

	_sq._count++;
	CString s; s.Format(L"%s: %d", KwRsc(IDS_Count), _sq._count);
	SetDlgItemText(IDC_Count, s);

	_rtMache = _sq.CheckEx();// 다 마추었나
	if(_rtMache != 1.)
		return;/// 아직 다 안맞추었잖아.


	/// <summary>
	/// 다 맞춘 경우
	/// </summary>
	_sq._tikStart = 0;///?이게 중요. IsStarted()의 요소가 된다.
	_bStarted = false;//?보류
	//KillTimer(eTimerElapsed);//일단 타이머 스톱
	PauseTimer("eTimerElapsed");//일단 타이머 스톱

	/// 최종 걸린 시간: 마지막 터치 액션 시각으로 계산 되었던거. 100nano. /10000000.= msec
	CString stm = MakeTimeStr(_sq._tElapsed, false);
	///?err: _sq._tElapsed = 9224212489147 => "256:13:41:249"
	//       _sq._tElapsed = 9226719378768
	SetDlgItemText(IDC_Time, stm);
	//CString stm1 = MakeTimeStr(9224212489147L, false);
	CMainPool::QueueFunc([&]()
		{
			TransferScore(this->_sq._mix);
		});
	//AfxMessageBox(IDS_Complete);
	MessageBox(KwRsc(IDS_Complete), L"Infomation", MB_OK|MB_ICONEXCLAMATION);


	//OnBnClickedReset();/// 여기서 mix.clear, KillTimer 하지만 너무 늦어
	/// 레셋 하면 보이는 정보가 없어 져서 그래도.
	//Sleep(1000);

}
void CMainDlg::TransferScore(SHP<std::list<KMove>> slstMv)
{
	TransferScore(*slstMv);
}
void CMainDlg::TransferScore(std::list<KMove>& lstMv)
{
	BACKGROUND(1);
	auto app = (CSuareSimulApp*)GetMainApp();
	auto& appd = (app)->_docApp;
	AUTOLOCK(appd._csAppDoc);
	//(*_sjGame)("startIndex") = (int)_sq._mixOnly.size();/// mixed 에서 액션으로 넘어간 부분
	//auto startIndex = _sq._mixOnly.size();/// mixed 
	/// _sjGame은 StartGame에서 appd.AddGame 으로 appdoc에 등록 까지 하고 
	(*_sjGame)("time") = _sq._tElapsed;// 마지막 액션 시각 액션의 "elapsed"와 같은데 굳이 넣얼 필요 있나

	ShJArr arAct = appd.GetAction(_sjGame);
	int i = 0;
	for(auto& it : lstMv)
	{
		if(it.tik > 0)
		{
			appd.AddAction(arAct, i, it.dr, it.l, it.n, it.tik);
			i++;
		}//else mix part
	}
	appd.CubeSaveCubeScore(_sjGame);// 서버에, GUID, 시간 100 nano second, count, ip?
}

void CMainDlg::OnDestroy()
{
	_bClosedDlg.Set("true: Exid, false: App Running");
	TRACE("OnDestroy ::WaitForSingleObject(_eventClose, 60000); _bClosedDlg(%d)\n", _bClosedDlg._val);
	::WaitForSingleObject(_eventClose, 
#ifdef _DEBUG
		60000
#else
		5000
#endif // _DEBUG
	);
	TRACE("OnDestroy ::WaitForSingleObject Event received _bClosedDlg(%d)\n", _bClosedDlg._val);

	CDlgInvokable::OnDestroy();
	if(_brBox)
	{
		if(_brBG)
			::delete _brBG;
		if(_brBox)
			::delete _brBox;
		if(_brLine)
			::delete _brLine;
		for(int i = 0; i < eNumColorSet; i++)
			for(int j = 0; j < eCbR; j++)
				if(_brCell[i][j])
					::delete _brCell[i][j];
		if(_penInv)
			::delete _penInv;

		/// 묘하게 Gdiplus객체는 delete를 쓰면 자신의 DLL에 있는 
		/// 자체 delete를 쓰게 되는데 죽는다.
		//_imgRevet->~Image();
		//_imgRevet->operator delete;
		if(_imgRevet)
			_imgRevet.reset();

		for(int j = 0; j < _countof(_imgCell); j++)//_sq._row
		{
			//_imgCell[j]->~Image();
 			//_imgCell[j]->operator delete;
			if(_imgCell[j])
				_imgCell[j].reset();
		}
		try
		{
			for(int j = 0; j < _countof(_imgMargin); j++)
			{
				//_imgMargin[j]->~Image();
				//_imgMargin[j]->operator delete;
				if(_imgMargin[j])
					_imgMargin[j].reset();
			}
		}
		catch (CMemoryException* )
		{
			TRACE("OnDestroy CMemoryException;\n");
		}
		catch (CFileException* )
		{
			TRACE("OnDestroy CFileException;\n");
		}
		catch(CException* )
		{
			TRACE("OnDestroy CException;\n");
		}
		catch(...)
		{
			TRACE("OnDestroy catch(...);\n");
		}
	}
// 	if(_gc) OnPaint에서 한다.
// 		::delete _gc;
// 	//DeleteMeSafe(_gc);
	TRACE("OnDestroy ResetEvent(_eventClose);\n");
	ResetEvent(_eventClose);

}


void CMainDlg::OnCbnSelchangeColor()
{
	UpdateData();
	InvalidateRect(NULL);
}



void CMainDlg::OnCbnSelchangeReplayspeed()
{
	UpdateData();
}
