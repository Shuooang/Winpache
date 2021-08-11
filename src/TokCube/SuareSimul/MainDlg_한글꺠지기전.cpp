
// MainDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "SuareSimul.h"
#include "MainDlg.h"
#include "afxdialogex.h"

#include <stdlib.h>
#include <cstdlib>
#include "KwLib64/Lock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// ��??�� ???? ????????��?��?.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ??����??��?��?.

// ����????��?��?.
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
	/// , _unit(0) ���⼭ �ʱ�ȭ class�� {2} �ʱ�ȭ�� ���� �ȴ�.
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

#ifdef _DEBUG
	JObj jo;
	jo("k1") = "�ѱ�";
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
		_unit = _sq._unit - 1;
	DDX_CBIndex(pDX, IDC_COMBO1, _unit);

	//
	if(pDX->m_bSaveAndValidate)// save, write, FALSE
	{
		_sq._unit = _unit + 1;
	}
// 	CString s = _sq._unit == 1 ? L"Space" : L"Spaces";
// 	SetDlgItemText(IDC_Spaces, s);
	DDX_CBIndex(pDX, IDC_Color, _sq._setCl);
}


void CMainDlg::OnCancel()
{
	if(AfxMessageBox(L"Are you sure?", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
		__super::OnCancel();
}


BEGIN_MESSAGE_MAP(CMainDlg, CDlgInvokable)
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
END_MESSAGE_MAP()

#include <gdiplus.h>				// Base include
using namespace Gdiplus;			// The "umbella"
#pragma comment(lib, "gdiplus.lib")	// The GDI+ binary

// CMainDlg message handlers

BOOL CMainDlg::OnInitDialog()
{
	CDlgInvokable::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon



	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, NULL);

	KwEnableWindow(this, IDC_Reset, TRUE);
	KwEnableWindow(this, IDC_Solve, FALSE);
	KwEnableWindow(this, IDC_Start, FALSE);

	CPoint lt(_sq._ptBoard);
	CSize sz(_sq._size);// , _sq._size);
	CRect rc(lt, sz);
	for(int r = 0; r < _sq._row; r++)
	{
		for(int c = 0; c < _sq._col; c++)
		{
			_sq._left[r][c] = _sq._leftInit[r][c] = rc.left;
			_sq._top[r][c] = _sq._topInit[r][c] = rc.top;
			rc.MoveToXY(rc.left + sz.cy, rc.top);
		}
		rc.MoveToXY(lt.x, rc.top + sz.cy);
	}

	srand(9470739);
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
CRect CMainDlg::BoadRect()
{
	int nrw = _sq.Narrow();
// 	CRect rcb(_sq._left[0][0] - nrw, _sq._top[0][0] - nrw,
// 		_sq._left[_sq._row - 1][_sq._col - 1] + _sq._size + nrw,
// 		_sq._top[_sq._row - 1][_sq._col - 1] + _sq._size + nrw);
	CRect rcb(CPoint(_ptDraw.x + _sq._ptBoard.x, _ptDraw.y + _sq._ptBoard.y),
		CSize(_sq._size.cx * _sq._col, _sq._size.cy * _sq._row));
// 		_sq._leftInit[_sq._row - 1][_sq._col - 1] + _sq._size + nrw,
// 		_sq._topInit[_sq._row - 1][_sq._col - 1] + _sq._size + nrw);
	return rcb;
}

#include "KwLib64/GdiTool.h"

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


		CRect rcc(
			CPoint(_sq._ptBoard.x + _ptDraw.x, _sq._ptBoard.y + _ptDraw.y),
			CSize(_sq._size.cx * _sq._col, _sq._size.cy * _sq._row));
		CAutoSmoothDraw _asd(rcc, &dc, dc.GetBkColor());//_tt._rcDraw
		CDC* pDC = _asd.GetDrawDC();
		_hdc = pDC->GetSafeHdc();
		//DeleteMeSafe(_gc); �������� �ѹ��� ���ֳ�? �̰� ������ �״µ�.
		_gc = ::new Graphics(_hdc);

		static Color set1[2][eCbR] = {
			{//excel ��
				Color(255, 255, 255),
				Color(255, 213,   0),
				Color(255,  88,   0),
				Color(183,  18,  52),
				Color(0,  70, 173),
				Color(0, 144,  72),
				Color(198, 224, 180),
			Color(117, 196, 255),
			Color(213, 150, 252),
		},
			{//ǥ�� ť���
				Color(255, 255, 204),
				Color(255, 217, 102),
				Color(244, 176, 132),
				Color(198, 224, 180),
				Color(117, 196, 255),
				Color(213, 150, 252),
						Color(183,  18,  52),
					Color(0,  70, 173),
					Color(0, 144,  72),
		}};

		if(_brCell[0][0] == nullptr)
		{
			_brBox = ::new SolidBrush(Color(114, 115, 107));
			_brLine = ::new SolidBrush(Color( 34, 25, 27)); //255, 0, 0));//����׶�
			for(int iset = 0; iset < eNumColorSet; iset++)
				for(int i = 0; i < eCbR; i++)
					_brCell[iset][i] = ::new SolidBrush(set1[iset][i]);
			Color clp(255, 0, 255);
			_penInv = ::new Gdiplus::Pen(clp, 3);
		}

//		TRACE("Paint \n");
		int du = _sq.Narrow();
		//CRect rcb = BoadRect(); �̰� �޷θ� DC�Ⱦ���.
		CRect rcb(CPoint(_sq._ptBoard.x, _sq._ptBoard.y), CSize(_sq._size.cx * _sq._col, _sq._size.cy * _sq._row));

		Gdiplus::Rect rct(rcb.left, rcb.top, rcb.Width(), rcb.Height());
		_gc->FillRectangle(_brBox, rct);

		//int wgrid = _sq._size.cx;// +du;//_sq.sq[0][0]

		//CRect rcw;	GetWindowRect(rcw);
		Brush* br = nullptr;
		for(int r = 0; r < _sq._row; r++)
		{
			for(int c = 0; c < _sq._col; c++)
			{
				int icl = _sq.sq[r][c];
				br = _brCell[_sq._setCl][icl];
				CPoint lt(	(int)((double)_sq._left[r][c] + (_sq._hor[r] * abs(_sq._hor[r]) * 0.1 * _sq._size.cx)),// -1 * 0.25 * 40
					(int)((double)_sq._top [r][c] + (_sq._ver[c] * abs(_sq._ver[c]) * 0.1 * _sq._size.cy)));
				CRect rccb(lt, _sq._size);
				//Gdiplus::Rect rct(rcb.left + (du / 2), rcb.top + (du / 2), rcb.Width() - du, rcb.Height() - du);
				Gdiplus::Rect rc1(rccb.left + (du / 2), rccb.top + (du / 2), rccb.Width()- du, rccb.Height() - du);
// 				if(c == 0 && r == 0)
// 					TRACE("Paint:Y(%d => %d) _ver[%d](%d)\n", _sq._top[r][c], y, c, _sq._ver[c]);
				if(_sq.IsStaticLine(r) && _sq.IsStaticLine(c))
					_gc->FillEllipse(br, rc1);
				else
					_gc->FillRectangle(br, rc1);

#ifdef _DEBUGxxx
				if(c == 0) // ǥ�� �� �� ����
				{
					Brush* brv = _brCell[_sq._setCl][r];
					Gdiplus::Rect rcb(_sq._left[r][c] - 30, _sq._top[r][c] + (du / 2), _sq._size/2, _sq._size - du);
					_gc->FillRectangle(brv, rcb);
				}
#endif // _DEBUGxxx

				// ĭ���� ���� ���� Į�� ���� ���� ������
				if((c % _sq._unit) == (_sq._unit - 1) && c < (_sq._col -1))
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
#ifdef _DEBUGxx
		Gdiplus::Rect rcInv(_rcInvalid.left, _rcInvalid.top, _rcInvalid.Width(), _rcInvalid.Height());
		_gc->DrawRectangle(_penInv, rcInv);
#endif // _DEBUG
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
	int c = col;
	int r = 0;
//	int du = _sq.Narrow();

	CPoint lt(_ptDraw.x + _sq._ptBoard.x, _ptDraw.y + _sq._ptBoard.y);
// 	int x = _sq._leftInit[r][c];// +(_sq._hor[r] * 0.1 * wgrid);// -1 * 0.25 * 40
// 	int y = _sq._topInit[r][c];// +(_sq._ver[c] * 0.1 * wgrid);
	CRect rc1(CPoint(_sq._leftInit[r][c] + lt.x, _sq._topInit[r][c] + lt.y),
		CSize(_sq._size.cx, _sq._size.cy * _sq._row));// _sq._size, _sq._size);
	CRect rcB = BoadRect();
	//CRect rc2;	BOOL b = rc2.SubtractRect(rc1, rcB);
	InplaceRect(rc1, rcB);
	TRACE("PopulateCol:%2d (%d) (%d, %d, %d, %d)\n", iAny, col, rc1.left, rc1.top, rc1.Width(), rc1.Height());
	InvalidBoard(rc1);
}
void CMainDlg::PopulateRow(int row)
{
	int c = 0;
	int r = row;
//	int du = _sq.Narrow();

	CPoint lt(_ptDraw.x + _sq._ptBoard.x, _ptDraw.y + _sq._ptBoard.y);
	CRect rc1(CPoint(_sq._leftInit[r][c] + lt.x, _sq._topInit[r][c] + lt.y),
		CSize(_sq._size.cx * _sq._col, _sq._size.cy));// _sq._size, _sq._size);

	CRect rcB = BoadRect();
	InplaceRect(rc1, rcB);
	TRACE("PopulateRow(%d) (%d, %d, %d, %d)\n", row, rc1.left, rc1.top, rc1.Width(), rc1.Height());
	InvalidBoard(rc1);
}
void CMainDlg::Populate()
{
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
	KillTimer(eTimerID);
	//               54     2�� �׳� ����
	int cmul = 10; // 1�̸� �ʹ� ����
	int seed = (_sq._row * _sq._col) * cmul * _sq._unit;
	int r1 = std::rand();
	int n = (seed * 3) + (r1 % seed);
	TRACE("Mix: %d = (%d * 3) + (%d %% %d)\n", n, seed, r1, seed);
#ifdef _DEBUG
	n = 2;
#endif // _DEBUG
	_sq.Mix(n);
	Populate();
	KwEnableWindow(this, IDC_Start, TRUE);
	_countDown = 10;
	SetTimer(eTimerCountdown, 1000, NULL);
}


void CMainDlg::OnBnClickedStart()
{
	KillTimer(eTimerCountdown);
	_bStarted = true;
	auto app = (CSuareSimulApp*)GetMainApp();
	auto& appd = (app)->_docApp;
	AUTOLOCK(appd._csAppDoc);

	_sq._tikStart = KwGetTickCount100Nano();
	_timer = 1000;
	_sjGame = appd.AddGame();

	SetTimer(eTimerID, _timer, NULL);

	KwEnableWindow(this, IDC_COMBO1, FALSE);
	KwEnableWindow(this, IDC_Mix, FALSE);
	KwEnableWindow(this, IDC_Solve, TRUE);
	KwEnableWindow(this, IDC_Reset, TRUE);
	KwEnableWindow(this, IDC_Start, FALSE);
}


UINT MyThreadProc(LPVOID pParam)
{
	CMainDlg* th = (CMainDlg*)pParam;

	th->_sq.Reverse([&](int direction, int ln, int tilt) -> int
		{
			if(th->_bReverseStop)
				return 0;
			KwBeginInvoke(th, ([&, th]()-> void
				{
					switch(direction)
					{
					case 0: th->PopulateRow(ln); break;
					case 1: th->PopulateRow(ln); break;
					case 2: th->PopulateCol(ln); break;
					case 3: th->PopulateCol(ln); break;
					}
				}));//?beginInvoke 4
			if(th->_bReverseStop)
				return 0;
			Sleep(5);
			if(th->_bReverseStop)
				return 0;
			return 1;
		});
	KwEnableWindow(th, IDC_Mix, TRUE);

	return 0;   // thread completed successfully
}
void CMainDlg::OnBnClickedSolve()
{
	KillTimer(eTimerID);
//	InterlockedDecrement(&_bReverseStop);
	//InterlockedExchange(&_bReverseStop, 0);
//	_bReverseStop = 0;
	_bReverseStop.Reset();

	KwEnableWindow(this, IDC_Solve, FALSE);

	AfxBeginThread(MyThreadProc, this);
	SetDlgItemText(IDC_Time, L"");
	SetDlgItemText(IDC_Count, L"");
	//	Populate();
}

UINT MyThreadProcMoveAny(LPVOID pParam)
{
	CMainDlg* th = (CMainDlg*)pParam;
	//KMove mx = th->_sq._moveQ;
	th->_sq.MoveAny(th->_sq._moveQ, [&, th](int direction, int ln, int tilt) -> void
		{
			th->InvalidatePart(direction, ln, tilt);
		});
	return 0;   // thread completed successfully
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
	if(nthFramme == (_sq._nFramme - 1))//���� ������
	{
		OnAfterMoved();
	}

}
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
	AfxBeginThread(MyThreadProcMoveAny, this);
	///Moved();/// ��濡�� ���� �ִϷ� �����̴� ���̶� ���� ���迭 �̹Ƿ� �ִ� ������ ������������ �ű�
}
BOOL CMainDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	UINT nID = LOWORD(wParam);
 	HWND hWndCtrl = (HWND)(lParam);    //Control handle
 	int nCode = HIWORD(wParam);              //Notification code
	if(nCode == BN_CLICKED)
	{
		bool bInGame = IsInGame();
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
	_bReverseStop.Set("true: ���� ����, false: ���� ��");
	KillTimer(eTimerCountdown);
	KillTimer(eTimerID);
	_sq.Reset();
	CString s; s.Format(L"Count: %d", _sq._count);
	SetDlgItemText(IDC_Count, s);
	SetDlgItemText(IDC_Time, L"Ready...");

	Populate();
	KwEnableWindow(this, IDC_COMBO1, TRUE);
	KwEnableWindow(this, IDC_Mix, TRUE);

	//InterlockedIncrement(&_bReverseStop);
}


void CMainDlg::OnCbnSelchangeCombo1()
{
	UpdateData();
	OnBnClickedReset();
	/// <summary>
	/// _sq._unit�� �ٲ�ſ� ���� ��ġ �̵��� �Ǿ���.
	/// </summary>
// 	_sq.MoveUnit();

 	InvalidBoard();
}


void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	CString s;
	if(nIDEvent == eTimerID)
	{
		auto tik = KwGetTickCount100Nano();
		auto elp = tik - _sq._tikStart;

		if(_rtMache > 0.6)// 60% �Ѱ� ���߾������� �и�sec���� ���δ�.
		{
			if(_timer == 1000)
			{
				KillTimer(eTimerID);
				_timer = 100;
				SetTimer(eTimerID, _timer, NULL);
			}
			s = MakeTimeStr(elp, false);
		}
		else
		{
			if(_timer == 100)
			{
				KillTimer(eTimerID);
				_timer = 1000;
				SetTimer(eTimerID, _timer, NULL);
			}
			s = MakeTimeStr(elp, true);
		}
		SetDlgItemText(IDC_Time, s);
	}
	else if(nIDEvent == eTimerCountdown)
	{
		_countDown--;
		if(_countDown > 0)
		{
			s.Format(L"-%d    ", _countDown);//����
		}
		else // start
		{
			KillTimer(eTimerCountdown);
			s = L"Start";
			KwBeginInvoke(this, ([&]()-> void {
				OnBnClickedStart();
				}));
		}
		SetDlgItemText(IDC_Time, s);
	}
	CDlgInvokable::OnTimer(nIDEvent);
}

CString CMainDlg::MakeTimeStr(LONGLONG elp, bool bShort)
{
	CString s;
	double dt = (double)elp / 10000000.;/// �ʴ����� �����.	s.Format(L"%9.3f msec", );
	if(bShort)
	{
		if(dt < 60)
			s.Format(L"%9.0f sec", dt);
		else
		{
			int m = (int)(dt / 60);
			double dt1 = dt - (m * 60);
			if(dt < 3600)
				s.Format(L"%02d:%02.0f sec", m, dt1);
			else
			{
				int h = m / 60;
				int m1 = m - (h * 60);
				s.Format(L"%d:%02d:%02.0f sec", h, m1, dt1);
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
				s.Format(L"%02d:%06.3f sec", m, dt1);
			else
			{
				int h = m / 60;
				int m1 = m - (h * 60);
				s.Format(L"%d:%02d:%06.3f sec", h, m1, dt1);
			}
		}
	}
	return s;
}
bool CMainDlg::IsInGame()
{
	bool bInGame = _sq._mix.size() > 0;// !GetDlgItem(IDC_Mix)->IsWindowEnabled();
#ifdef _DEBUGx
	bInGame = true;
#endif // _DEBUG
	return bInGame;
}
void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	bool bInGame = IsInGame() && IsStarted();
	if(bInGame)
	{
		_bLbDown = TRUE;
		_arPt.push_back(point);
		TRACE("Down: %d, %d\n", point.x, point.y);
	}
	CDlgInvokable::OnLButtonDown(nFlags, point);
}

void CMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	bool bInGame = IsInGame();
	if(bInGame)
	{
		if(_bLbDown)
		{
			CPoint pt = _arPt.back();
			//NTRACE("move: %d, %d\n", point.x, point.y, point.x - pt.x, point.y - pt.y);
			_arPt.push_back(point);///������ �����͸� ��� ����
		}
	}
	CDlgInvokable::OnMouseMove(nFlags, point);
}

void CMainDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	bool bInGame = IsInGame();
	if(bInGame)
	{
		if(_bLbDown)
		{
			CSize tt;
			auto n = _arPt.size();
			for(int i = 1; i < n; i++)
			{
				CSize ds(_arPt[i].x - _arPt[i - 1].x, _arPt[i].y - _arPt[i - 1].y);
				tt.cx += ds.cx;///������ ���� ��� ���ļ� (��ձ��Ϸ���)
				tt.cy += ds.cy;
// 				NTRACE("ds : %d = (%d - %d), %d = (%d - %d) : T(%d, %d)\n",
// 					_arPt[i].x - _arPt[i - 1].x, _arPt[i].x, _arPt[i - 1].x,
// 					_arPt[i].y - _arPt[i - 1].y, _arPt[i].y, _arPt[i - 1].y,
// 					tt.cx, tt.cy);
			}

			TRACE("Up  : (%d, %d) npt(%d) total:(%d, %d)\n", point.x, point.y, n, tt.cx, tt.cy);
		
			//CPoint lt(_sq._left[0][0], _sq._top[0][0]);/// �ϴ� Left Top ���� ��ǥ�� �˾Ƴ���
			/// �ϴ� Left Top ���� ��ǥ�� �˾Ƴ���
			CPoint lt(_ptDraw.x + _sq._ptBoard.x, _ptDraw.y + _sq._ptBoard.y);
			//int du = _sq.Narrow();
			//int wgrid = _sq._size;// +du;//_sq.sq[0][0]


			if(abs(tt.cx) < 1 && abs(tt.cy) < 1) /// ��Ż �����ΰ� 2ĭ���ϸ� �׳� ��ġ��.
			{
				///�׷� ���� point�� ��� ���ȳ� ���� ����������.
				CPoint cel((point.x - lt.x) / _sq._size.cx, (point.y - lt.y) / _sq._size.cy);/// Left Top ��(���)�� �˾Ƴ���

				///~~ ��... ���� ������ �������� �������� �𸣰ڱ���.
			}
			else if(IsStarted())
			{
				double dx = (double)tt.cx / n;///�������� ����� ���Ѵ�.
				double dy = (double)tt.cy / n;

				/// ��ġ ������ ��ġ ���
				CPoint cel((_arPt[0].x - lt.x) / _sq._size.cx, (_arPt[0].y - lt.y) / _sq._size.cy);/// Left Top ��(���)�� �˾Ƴ���
				///if(abs(dx) > 1 || abs(dy) > 1) �߰� ��� �������� false
				if(abs(tt.cx) > 5 || abs(tt.cy) > 5)
				{
					double rtX = dy == 0. ? 100. : abs(dx) / abs(dy);
					double rtY = dx == 0. ? 100. : abs(dy) / abs(dx);
					auto tik = KwGetTickCount100Nano();
					auto elp = tik - _sq._tikStart;
					if(rtY >= 2.)/// ���η� ���� ����������. ���� ���� �� 3�� �̻�
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


void CMainDlg::TransferScore()
{
	auto app = (CSuareSimulApp*)GetMainApp();
	auto& appd = (app)->_docApp;
	AUTOLOCK(appd._csAppDoc);
	(*_sjGame)("startIndex") = (int)_sq._mixOnly.size();/// mixed ���� �׼����� �Ѿ �κ�
	(*_sjGame)("time") = _sq._tElapsed;// ������ �׼� �ð� �׼��� "elapsed"�� ������ ���� �־� �ʿ� �ֳ�
	
	ShJArr arAct = appd.GetAction(_sjGame);

	for(auto& it : _sq._mix)
		appd.AddAction(arAct, it.dr, it.l, it.n, it.tik);

	appd.CubeRegisterUser(_sjGame);
	// ������, GUID, �ð� 100 nano second, count, ip?
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
/// ����Ʈ�� ���� ���ϰ�, �ٸ� ��°� ��Ȳ�� üũ
/// </summary>
void CMainDlg::OnAfterMoved()
{
	if(!IsMixed() || !IsStarted())
		return;/// ���� ���� �����ݾ�

	_sq._count++;
	CString s; s.Format(L"Count: %d", _sq._count);
	SetDlgItemText(IDC_Count, s);

	_rtMache = _sq.CheckEx();// �� ���߾���
	if(_rtMache != 1.)
		return;/// ���� �� �ȸ��߾��ݾ�.
	_sq._tikStart = 0;///?�̰� �߿�. IsStarted()�� ��Ұ� �ȴ�.
	_bStarted = false;//?����
	KillTimer(eTimerID);//�ϴ� Ÿ�̸� ����
	/// ���� �ɸ� �ð�: ������ ��ġ �׼� �ð����� ��� �Ǿ�����. 100nano. /10000000.= msec
	CString stm = MakeTimeStr(_sq._tElapsed, false);
	///?err: _sq._tElapsed = 9224212489147 => "256:13:41:249"
	//       _sq._tElapsed = 9226719378768
	SetDlgItemText(IDC_Time, stm);
	//CString stm1 = MakeTimeStr(9224212489147L, false);
	TransferScore();

	//OnBnClickedReset();/// ���⼭ mix.clear, KillTimer ������ �ʹ� �ʾ�
	/// ���� �ϸ� ���̴� ������ ���� ���� �׷���.
	Sleep(1000);
	AfxMessageBox(L"Complete!");

}

void CMainDlg::OnDestroy()
{
	CDlgInvokable::OnDestroy();
	if(_brBox)
	{
		::delete _brBox;
		::delete _brLine;
		for(int i = 0; i < eNumColorSet; i++)
			for(int j = 0; j < eCbR; j++)
				::delete _brCell[i][j];
	}
	if(_gc)
		::delete _gc;
	//DeleteMeSafe(_gc);
	GdiplusShutdown(_gdiplusToken);
}


void CMainDlg::OnCbnSelchangeColor()
{
	UpdateData();
	InvalidateRect(NULL);
}

