// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해
// 추가적으로 제공되는 내용입니다.
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// SmpView.cpp: CSmpView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MFCSmpApp4.h"
#endif

#include "SmpDoc.h"
#include "SmpView.h"

#include "KwLib64/ThreadPool.h" // QueueFUNCN
#include "KwLib64/DlgTool.h" // KwBeginInvoke
#include "KwLib64/tchtool.h" // tchlen
#include "KwLib64/KDebug.h"
#include "KwLib64/KTemple.h"
#include "KwLib64/MfcEx.h"

#include "Requ.h"
#include "resource.h"
#include "Mainfrm.h"
#include "DockScheduleDetail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSmpView

IMPLEMENT_DYNCREATE(CSmpView, CSafeViewBase)

BEGIN_MESSAGE_MAP(CSmpView, CSafeViewBase)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CFormInvokable::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormInvokable::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSmpView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_BN_CLICKED(IDC_ReadTimeTable, &CSmpView::OnBnClickedReadTimeTable)
	ON_BN_CLICKED(IDC_Refresh, &CSmpView::OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_Clear, &CSmpView::OnBnClickedClear)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_DelSchedule, &CSmpView::OnDelSchedule)
	ON_COMMAND(ID_AddOrder, &CSmpView::OnAddOrder)
	ON_COMMAND(ID_AddSchedule, &CSmpView::OnAddSchedule)
	ON_UPDATE_COMMAND_UI(ID_AddOrder, &CSmpView::OnUpdateAddOrder)
	ON_UPDATE_COMMAND_UI(ID_AddSchedule, &CSmpView::OnUpdateAddSchedule)
	ON_UPDATE_COMMAND_UI(ID_DelSchedule, &CSmpView::OnUpdateDelSchedule)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_SLIDER_Zoom, &CSmpView::OnSliderZoom)
	ON_COMMAND(ID_AlwaysSyncSD, &CSmpView::OnAlwaysSyncSD)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CSmpView 생성/소멸

CSmpView::CSmpView() noexcept
	: CSafeViewBase(IDD_MFCSMPAPP4_FORM)
	, _tt(this)
{
	// TODO: 여기에 생성 코드를 추가합니다.

	//_tt._pjtbl = &_jtbl;
	//_tt._pcs_jtbl = &_cs_jtbl;
}

CSmpView::~CSmpView()
{
}

void CSmpView::DoDataExchange(CDataExchange* pDX)
{
	CSafeViewBase::DoDataExchange(pDX);
}

BOOL CSmpView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CSafeViewBase::PreCreateWindow(cs);
}

void CSmpView::OnInitialUpdate()
{
	CSafeViewBase::OnInitialUpdate();
	ResizeParentToFit();
	
#ifdef DEBUG
	_tt._day = CTime(2021, 2, 9, 0,0,0);//L"2021-02-09";
#else
	_tt._day = KwGetTodayTime();
#endif

	RM_GetTodaySchdule(0); // sjo 는 더미
}

//?request standard
void CSmpView::RM_GetTodaySchdule(ShJObj sjo, int iOp)
{
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud 작업 큐
			{
				RM_GetTodaySchdule(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);

	JObj jin, jout;

	jin("fTest") = L"1234abcd가나다라";//샵
	jin("fBizID") = L"biz-0002";//샵
	jin("fUsrID") = L"staff-a021";
	jin("fAllStaff") = L"every";//or "total"
	jin("fDay") = _tt._day;//L"2021-02-09";

	KRequ req;//요청한 사람
	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if(!tchsame(jout.S("Return"), L"OK"))
		return;

	_KwBeginInvoke(this, [&, jout]() /// foreground 작업 큐
		{
			FOREGROUND();
			AUTOLOCK(_tt._cs_jtbl); // _jtbl 이 멀티로 접근 하므로 락해야.
	
			_tt._bRedraw = true;
			_tt.Reset();
			_tt._jtbl = ShJObj(new JObj(jout)); // jout는 백그라운드 스택에서 사라지기 전에 복사
			_tt.ArrangeColumn();// fOrderSched에 따라 컬럼 배열을 만들어 준다.
			GetDlgItem(IDC_STATICBG)->ShowWindow(SW_HIDE);
			Invalidate(1);
		});
}

// CSmpView 인쇄


void CSmpView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CSmpView::OnPreparePrinting(CPrintInfo* pInfo)
{	return DoPreparePrinting(pInfo);}
void CSmpView::OnBeginPrinting(CDC*, CPrintInfo* ){}
void CSmpView::OnEndPrinting(CDC*, CPrintInfo*) {}
void CSmpView::OnPrint(CDC*, CPrintInfo* ){}

// CSmpView 진단
#ifdef _DEBUG
void CSmpView::AssertValid() const
{	CFormInvokable::AssertValid();
}
void CSmpView::Dump(CDumpContext& dc) const
{	CFormInvokable::Dump(dc);
}
CSmpDoc* CSmpView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSmpDoc)));
	return (CSmpDoc*)m_pDocument;
}
#endif //_DEBUG

#define _Use_Smooth
#include "KwLib64/GdiTool.h"
// CSmpView 메시지 처리기
void CSmpView::OnDraw(CDC* pDC)
{
	CRect rcc;
	GetClientRect(rcc);
	SCROLLBARINFO sbi{ 0 };
	BOOL b = ::GetScrollBarInfo(GetSafeHwnd(), OBJID_CLIENT, &sbi);
	int cy = 0;
	if(b)
		cy = GetScrollLimit(SB_VERT);
	cy = GetScrollLimit(SB_VERT);
	if(cy > 0)
		_break;
	//BOOL b = GetScrollBarInfo(OBJID_VSCROLL, &sbi);
	int mn{ 0 }, mx{ 0 };
	GetScrollRange(SB_VERT,&mn, &mx);
	
	rcc.bottom = mx;
	_tt._rcc = rcc;

#ifdef _Use_Smooth
	CAutoSmoothDraw _asd(rcc, pDC, pDC->GetBkColor());//_tt._rcDraw
	pDC = _asd.GetDrawDC();
#endif // _Use_Smooth

	_tt._hDC = pDC->m_hDC;///?kdw
	_tt.DrawTimeTable();
}


void CSmpView::OnBnClickedReadTimeTable()
{
	ShJObj sjo;// = ShJObj(new JObj);
	RM_GetTodaySchdule(sjo);
}


void CSmpView::OnBnClickedRefresh()
{
//	Invalidate(1);
	_tt._bRedraw = true;
	CRect rcc;
	GetClientRect(_tt._rcc);

	InvalidateRect(NULL, 0);
}


void CSmpView::OnBnClickedClear()
{
	_tt._bRedraw = false;
	GetClientRect(_tt._rcc);
	InvalidateRect(NULL, 1);
}

/*
CTRL 키가 눌러져 있으면 MK_CONTROL 설정 됩니다.
마우스 왼쪽 단추를 누른 경우에 설정 MK_LBUTTON 합니다.
마우스 가운데 단추가 눌러져 있으면 MK_MBUTTON 설정 됩니다.
마우스 오른쪽 단추를 누르고 있으면 MK_RBUTTON 설정 됩니다.
SHIFT 키가 눌러져 있으면 MK_SHIFT 설정 됩니다.
#define MK_LBUTTON          0x0001
#define MK_RBUTTON          0x0002
#define MK_SHIFT            0x0004
#define MK_CONTROL          0x0008
#define MK_MBUTTON          0x0010
//(_WIN32_WINNT >= 0x0500)
#define MK_XBUTTON1         0x0020
#define MK_XBUTTON2         0x0040

*/
void CSmpView::OnLButtonDown(UINT nFlags, CPoint point)
{
	//TRACE("%s(%d) (%u)\n", __FUNCTION__, 0, nFlags);
	// TODO: Add your message handler code here and/or call default
	//int scy = GetScrollPos(SB_VERT);
	//int scx = GetScrollPos(SB_HORZ);
	CPoint scpos(point);
	KwGetViewScrolledPos(this, scpos);

	if(_tt.ButtonDown(scpos, nFlags))/// _curBox 가 지정 된다.
	{
		OnBoxSelected();
	}
	auto pfn = (CMainFrame*)AfxGetMainWnd();
	pfn->ContextCategory(ID_CONTEXT_TimeTable);

	//if(nFlags & MK_LBUTTON)// left button 일떄만 작동
	//	_wCapture = SetCapture();//fail: 이전 캡쳐윈도우 리턴 하는 구나

	CFormInvokable::OnLButtonDown(nFlags, point);
}

void CSmpView::OnMouseMove(UINT nFlags, CPoint point)
{
	//KTrace(L"%d\n", nFlags);
	/// TODO: Add your message handler code here and/or call default
	//if(nFlags & MK_LBUTTON)// left button 일떄만 작동
	_tt.MouseMove(nFlags, point);/// 안에서 _curBox 체크하고 아니면 커서모양만 바꾼다.

	CFormInvokable::OnMouseMove(nFlags, point);
}

void CSmpView::OnLButtonUp(UINT nFlags, CPoint point)
{
	//auto wc = GetCapture();
	//TRACE("%s(%d) (%u) _cur(%X) _sel(%X)\n", __FUNCTION__, 0, nFlags, _tt._curBox, _tt._selBox);
	// TODO: Add your message handler code here and/or call default
	//if(nFlags & MK_LBUTTON)// left button 일떄만 작동
	if(_tt._curBox)
	{
		_tt.LButtonUp(nFlags, point,
			[&]()
			{//onSelected
				_KwBeginInvoke(this, [&]() {
					RefreshScheduleDetail(); /// 선택한 박스가 바뀐 경우 일정 상세창 주체가 바뀐다.
				});
			},
			[&]()
			{//onUpdated
				_KwBeginInvoke(this, [&]() {
					RM_UpdateTodaySchedule(0);/// 박스 위치나 크기 변경 된 경우, 서버에 변경 요청을 한다.
				});
			});

		ReleaseCapture();
	}
	CFormInvokable::OnLButtonUp(nFlags, point);
}

void CSmpView::OnRButtonDown(UINT nFlags, CPoint point)
{
	//TRACE("%s(%d) (%u)\n", __FUNCTION__, 0, nFlags);
	// TODO: Add your message handler code here and/or call default
	//box선택하면서 컨텍스트 메뉴에서 삭제 가능 하다.
	CRect rccp;
	GetParent()->GetClientRect(rccp);
	CRect rcc;
	GetWindowRect(rcc);
	GetParent()->ScreenToClient(rcc);
	
	auto pfn = (CMainFrame*)AfxGetMainWnd();
	pfn->ContextCategory(ID_CONTEXT_TimeTable);

	//int scy = GetScrollPos(SB_VERT);
	//int scx = GetScrollPos(SB_HORZ);
	//CPoint spt(point.x + scx, point.y + scy);
	CPoint scpos(point);
	KwGetViewScrolledPos(this, scpos);
	_nFlagsR = nFlags;
	_scposRBtn = scpos;

	if(_tt.ButtonDown(scpos, nFlags))
	{
		_KwBeginInvoke(this, [&]()-> void
			{ //?beginInvoke 4
				OnBoxSelected();
			});
		InvalidateRect(NULL, 0);
	}

	/// 여기서는 SetCapture를 안한다.

	CFormInvokable::OnRButtonDown(nFlags, point);
}

void CSmpView::OnRButtonUp(UINT nFlags, CPoint point)
{
	this->ClientToScreen(&point);//popup menu는 child가 아니므로 독립적으로 뜨므로 Screen좌표가 필요 하다.
	_KwBeginInvoke(this, [&, nFlags, point]() /// foreground 작업 큐
		{
			OnContextMenu(this, point);
		});
}
void CSmpView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}



void CSmpView::DeleteSchedule()
{
	//TRACE("%s(%d) sel(%X)\n", __FUNCTION__, 0, _tt._selBox);
	_break;
	///?todo

	if(_tt._selBox)
	{
		RM_DeleteTodaySchedule();
	}
}

void CSmpView::RefreshScheduleDetail()
{
	//TRACE("%s(%d)\n", __FUNCTION__, 0);
	_break;
	/// 일정 상세 보기 뿌려 진다.
// 일반일정이냐, 오더일정이냐에 따라 뿌리는게 다르다.
// 뿌린 후 프로퍼티 창에서 편집이 가능 해야 한다.

	if(_tt._selBox)
	{
		if(!_tt._selBox->_jocc)
			return;

		JObj& jocc = *(_tt._selBox->_jocc);//shared의 shared
		//CString keyShed = _tt.MakeBoxKey(jocc);//
		
		/// 이런식으로 데이터를 빼다가 뿌린다.
		CString fVChrgID = jocc.S("fVChrgID");
		bool bCmnSched = fVChrgID.IsEmpty();//공통 일정이면 fVChrgID == null 넓게 펼쳐져 있다.

		///?todo
	}
	else
	{
		///빈칸으로 보여야
	} 
}


void CSmpView::OnBoxSelected(bool bDblClk)
{
	//TRACE("%s(%d)\n", __FUNCTION__, 0);
	if(!_tt._selBox)
		return;
	auto& box = *_tt._selBox;
	
	if(box._side == "top")
	{
		/// 예약, 개인일정 시작 시간 조작 시작
	}
	else if(box._side == "bottom")
	{
		/// 예약, 개인일정 끝 시간 조작 시작
	}
	else if(box._side == "center")
	{
		/// 예약, 개인일정 상세 일정 읽어와서 보여주기. 
	}
	auto app = (CSmpApp4*)AfxGetApp();
	if(!app->_sjoSet->SameS("fSyncSchDetail", L"Always") && !bDblClk)
	{
		if(!DockScheduleDetail::s_me->IsShown())
			return;//지금 창도 안보이고 옵션도 항상이 아니니 안한다.
	}
	int iOp = eSyncByAction64;
	if(bDblClk)
		iOp |= eShowDock1;
	SetLambdaTimer("sync_cutomDetail", 700, [&, iOp](int ntm, PAS tmk)
		{
			RM_SelectOrderSchedule(nullptr, iOp);
		}, 1);

}

void CSmpView::RM_SelectOrderSchedule(ShJObj sjo, int iOp)
{
	//TRACE("%s(%d)\n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]()	
			{
				RM_SelectOrderSchedule(sjo, iOp);	
			});
		return;
	}

	BACKGROUND(1);
	if(!_tt._selBox || !(_tt._selBox->_jocc))
		return;

	auto& jocc = *(_tt._selBox->_jocc);

	JObj jin, jout;
	KRequ req;
	CString fMode = jocc.S("fMode");
	if(fMode == L"order")
	{
		jin.Copy(jocc, "fOrdShedID", "fSchedOrgID");// 이거와 fMode가 동시에 있어야 키가 된다. 변경시에는 이거만 있어도.
		//jin.Copy(jocc, "fMode");//order or schedule
		req.RequestPost(__FUNCTION__, jin, jout);
	} 
	else if(fMode == L"schedule")
	{
		jin.Copy(jocc, "fSchdID", "fSchedOrgID");// 이거와 fMode가 동시에 있어야 키가 된다. 변경시에는 이거만 있어도.
		req.RequestPost("SelectBizSchedule", jin, jout);
	}

	if(tchsame(jout.S("Return"), L"OK"))
	{
		ShJObj sjo = make_shared<JObj>(jout);
		_KwBeginInvoke(this, [&, sjo, iOp]() /// foreground 작업 큐
			{
				FOREGROUND();
				DockScheduleDetail::s_me->Refresh("sync", sjo, iOp);
			});
		return;
	}
}


//?deprecated
void CSmpView::RM_SelectBizSchedule(ShJObj sjo, int iOp)
{
	//TRACE("%s(%d)\n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud 작업 큐
			{
				RM_SelectBizSchedule(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);
	if(!_tt._selBox)
		return;
	if(!(_tt._selBox->_jocc))//이런 경우도 있나?
		return;

	auto& jocc = *(_tt._selBox->_jocc);

	JObj jin, jout;
	KRequ req;
	jin.Copy(jocc, "fSchedOrgID");// 이거와 fMode가 동시에 있어야 키가 된다. 변경시에는 이거만 있어도.
	jin.Copy(jocc, "fMode");//order or schedule

	jin("fUsrIdUpt") = req._UsrID;//변경한 사람은 로그인한 사람.

	req.RequestPost(__FUNCTION__, jin, jout);
	if(tchsame(jout.S("Return"), L"OK"))
	{
		_KwBeginInvoke(this, [&, jout]() /// foreground 작업 큐
			{
				FOREGROUND();
				_tt.DeleteSelBox();// _selBox
				Invalidate(1);
				RefreshScheduleDetail(); /// _selBox 가 지워 졌으니 상세 창도 clear되야지
			});
		return;
	}
}


//?request standard
void CSmpView::RM_UpdateTodaySchedule(ShJObj sjo, int iOp)
{
	//TRACE("%s(%d)\n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud 작업 큐
			{
				RM_UpdateTodaySchedule(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);
	if(!_tt._selBox)
		return;
	if(!(_tt._selBox->_jocc))
		return;

	auto& jocc = *(_tt._selBox->_jocc);
	CString fVChrgID = jocc.S("fVChrgID");

	JObj jin, jout;
	KRequ req;
	jin.Copy(jocc, "fSchedOrgID");// 이거와 fMode가 동시에 있어야 키가 된다. 변경시에는 이거만 있어도.
	jin.Copy(jocc, "fMode");//order or schedule
	jin.Copy(jocc, "fVChrgID");//박스 이동시 컬럼이 바뀌면 담당자가 바뀌므로 null이라도 보내야
	jin.Copy(jocc, "fUsrID");//박스 이동시 컬럼이 바뀌면 담당자가 바뀌므로 null이라도 보내야
	jin.Copy(jocc, "fOffBegin");// 로우 드래그때 이것이 바뀐다.
	jin.Copy(jocc, "fOffEnd");

	jin("fUsrIdUpt") = req._UsrID;//변경한 사람은 로그인한 사람.

	req.RequestPost(__FUNCTION__, jin, jout);
	if(!tchsame(jout.S("Return"), L"OK"))
	{
		_KwBeginInvoke(this, [&, jout]() /// foreground 작업 큐
			{
				AUTOLOCK(_tt._cs_jtbl); // _jtbl 이 멀티로 접근 하므로 락해야.
				FOREGROUND();
				_tt.RecoverUpdate();
				//_tt.ArrangeColumn();// fOrderSched에 따라 컬럼 배열을 만들어 준다.
				Invalidate(1);
			});
		return;
	}
}

void CSmpView::RM_DeleteTodaySchedule(ShJObj sjo, int iOp)
{
	//TRACE("%s(%d)\n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud 작업 큐
			{
				RM_DeleteTodaySchedule(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);
	if(!_tt._selBox)
		return;
	if(!(_tt._selBox->_jocc))//이런 경우도 있나?
		return;

	auto& jocc = *(_tt._selBox->_jocc);

	JObj jin, jout;
	KRequ req;
	jin.Copy(jocc, "fSchedOrgID");// 이거와 fMode가 동시에 있어야 키가 된다. 변경시에는 이거만 있어도.
	jin.Copy(jocc, "fMode");//order or schedule

	jin("fUsrIdUpt") = req._UsrID;//변경한 사람은 로그인한 사람.

	req.RequestPost(__FUNCTION__, jin, jout);
	if(tchsame(jout.S("Return"), L"OK"))
	{
		_KwBeginInvoke(this, [&, jout]() /// foreground 작업 큐
			{
				FOREGROUND();
				_tt.DeleteSelBox();// _selBox
				Invalidate(1);
				RefreshScheduleDetail(); /// _selBox 가 지워 졌으니 상세 창도 clear되야지
			});
		return;
	}
}

void CSmpView::OnSetFocus(CWnd* pOldWnd)
{
	CFormInvokable::OnSetFocus(pOldWnd);
	auto pfn = (CMainFrame*)AfxGetMainWnd();
	///pfn->ContextCategory(ID_CONTEXT_TimeTable); 처음에 뜨자마자 바뀌는거 방지

	// TODO: Add your message handler code here
}


void CSmpView::OnDelSchedule()
{
	//TRACE("%s(%d) sel(%X)\n", __FUNCTION__, 0, _tt._selBox);
	DeleteSchedule();
}

#include "DockOrder.h"

void CSmpView::OnAddOrder()
{
	if(_tt._curColRow.y < _tt._nRows)
	{
		auto gOrder = DockOrder::s_me->_sjo->OO("torder", "gOrder");
		auto sjo = make_shared<JObj>(gOrder);
		_tt.AddOrder(_tt._curColRow.x, _tt._curColRow.y, sjo);
		Invalidate();

		//sjo->Copy(*gOrder, "fSchedOrgID", "fOrderID");//선택한 사람
		RM_AddTodayOrder(sjo);
	}
}
void CSmpView::RM_AddTodayOrder(ShJObj sjo, int iOp)
{
	//TRACE("%s(%d)\n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud 작업 큐
			{
				RM_AddTodayOrder(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);
	/// ------ 클라이언트에서 임시 생성 ------ ///
	/// fIDX : 최고점 + 1
	/// fOrder : 1140
	/// fSrcDesc : '예약'
	/// fBizID : 다른거랑 동일
	/// fOff : 'off'
	/// fMode : 'order'
	/// fRepeat : 'once'

	/// fSchedOrgID : GUID 서버에서 생성 -> fOrdShedID
	/// fVChrgIdWrk : _tt._curColRow.x 로 지정 하고 -> fVChrgID
	/// fOffBegin, fOffEnd : y로 시간 지정 하여 30분 짜리 -> fBeginTo, fEndTo
	/// fOrderID : 오더 선택은 바로 상세창에서 하고 서버에서 가상담당 = 진짜사용자 연결 한다.

	/// 서버에 추가 후 실패 하면 newIDX 로 _mapBoxes 에서 모두 제거 해야 화면에서 없어진다. 애쓴게 없으니 지워도 된다.
	/// 성공 하면 받은 fSchedOrgID 로 _requestedIDX 에서만 제거 한다.

	if(!_tt._selBox)
		return;
	if(!(_tt._selBox->_jocc))//이런 경우도 있나?
		return;

	auto& jocc = *(_tt._selBox->_jocc);
	JObj jin, jout;
	KRequ req;
	jin.Copy(jocc, "fSchedOrgID");// uuid
	jin.Copy(jocc, "fMode");// order
	jin.Copy(jocc, "fOffBegin");
	jin.Copy(jocc, "fOffEnd");
	jin.Copy(jocc, "fOrderID"); // ONULL()
	jin.Copy(jocc, "fVChrgID"); // ONULL()
	jin("fUsrIdUpt") = req._UsrID;//변경한 사람은 로그인한 사람.

	req.RequestPost(__FUNCTION__, jin, jout);
	bool bOK = tchsame(jout.S("Return"), L"OK");

	CString fSchedOrgID = jin.S("fSchedOrgID");
	_KwBeginInvoke(this, [&, bOK, fSchedOrgID]() /// foreground 작업 큐
		{
			FOREGROUND();
			_tt.AfterAddOrder(bOK, fSchedOrgID);// _selBox
			Invalidate(1);
			RefreshScheduleDetail(); /// _selBox 가 지워 졌으니 상세 창도 clear되야지
		});
}

void CSmpView::OnAddSchedule()
{
	if(_tt._curColRow.y < _tt._nRows)
	{
		_tt.AddSchedule(_tt._curColRow.x, _tt._curColRow.y);
		Invalidate();
		RM_AddTodaySchedule(0);
	}
}

void CSmpView::RM_AddTodaySchedule(ShJObj sjo, int iOp)
{
	//TRACE("%s(%d)\n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud 작업 큐
			{
				RM_AddTodaySchedule(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);
	if(!_tt._selBox)
		return;
	if(!(_tt._selBox->_jocc))//이런 경우도 있나?
		return;

	auto& jocc = *(_tt._selBox->_jocc);

	JObj jin, jout;
	KRequ req;
	jin.Copy(jocc, "fSchedOrgID");// uuid
	jin.Copy(jocc, "fMode");// order
	jin.Copy(jocc, "fOffBegin");
	jin.Copy(jocc, "fOffEnd");
	//jin.Copy(jocc, "fOrderID"); // ONULL()
	//jin.Copy(jocc, "fVChrgID"); // ONULL()
	jin.Copy(jocc, "fUsrID"); // ONULL()

	jin.Copy(jocc, "fOrder");
	jin.Copy(jocc, "fSrcDesc");
	jin.Copy(jocc, "fOff");
	jin.Copy(jocc, "fMode");
	jin.Copy(jocc, "fAttr");
	jin.Copy(jocc, "fRepeat");

	auto app = (CSmpApp4*)AfxGetApp();
	
	jin("fUsrIdUpt") = req._UsrID;//변경한 사람은 로그인한 사람.
	//jin("fUsrIdUpt") = app->getUsrID();//변경한 사람은 로그인한 사람.
	jin("fBizID") = req._BizID;// app->getBizID();//변경한 사람은 로그인한 사람.

	req.RequestPost(__FUNCTION__, jin, jout);
	bool bOK = tchsame(jout.S("Return"), L"OK");

	CString fSchedOrgID = jin.S("fSchedOrgID");
	_KwBeginInvoke(this, [&, bOK, fSchedOrgID]() /// foreground 작업 큐
		{
			FOREGROUND();
			_tt.AfterAddOrder(bOK, fSchedOrgID);// _selBox
			Invalidate(1);
			RefreshScheduleDetail(); /// _selBox 가 지워 졌으니 상세 창도 clear되야지
		});
}


bool CSmpView::IsRBtnOnBoxClicked()
{
	CPoint scpos(-1, -1);// -1이어야 직접 감지 한다.
	KwGetViewScrolledPos(this, scpos);
	//_nFlagsR = nFlags;
	//_scposR = scpos;

	CStringA side;
	TBox* box = _tt.IsCursorInBox(_scposRBtn, side);
	return box != nullptr;
}
void CSmpView::OnUpdateAddOrder(CCmdUI* pCmdUI)
{
	//CPoint pt;
	//GetCursorPos(&pt);
	//ScreenToClient(&pt);
	//CRect rcMain = _tt.getMainArea();
	//bool bIn = rcMain.PtInRect(pt);
	bool bInColRow = KwIsInside(_tt._curColRow.x, 0L, _tt._nCols - 1L) && KwIsInside(_tt._curColRow.y, 0L, _tt._nRows - 1L);
	pCmdUI->Enable(!IsRBtnOnBoxClicked() && bInColRow); // false
}
void CSmpView::OnUpdateAddSchedule(CCmdUI* pCmdUI)
{
	//CPoint pt;
	//GetCursorPos(&pt);
	//ScreenToClient(&pt);
	//CRect rcMain = _tt.getMainArea();
	//bool bIn = rcMain.PtInRect(pt);
	bool bUsr = false;
	bool bInColRow = KwIsInside(_tt._curColRow.x, 0L, _tt._nCols - 1L) && KwIsInside(_tt._curColRow.y, 0L, _tt._nRows - 1L);
	if(bInColRow)
	{
		auto& jvc = *_tt._col[_tt._curColRow.x];
		bUsr = jvc.Has("fUsrID");/// 사용자가 연결된 가상담당만 일정 입력이 가능 하다.
	}
	pCmdUI->Enable(!IsRBtnOnBoxClicked() && bInColRow && bUsr); // false
}
void CSmpView::OnUpdateDelSchedule(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsRBtnOnBoxClicked());
}


void CSmpView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
#ifdef DEBUG
	//Invalidate(1);
#endif
	CSafeViewBase::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CSmpView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
#ifdef DEBUG
	//Invalidate(1);
#endif
	CSafeViewBase::OnHScroll(nSBCode, nPos, pScrollBar);
}


BOOL CSmpView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
#ifdef DEBUG
	//Invalidate(0);
#endif
	return CSafeViewBase::OnMouseWheel(nFlags, zDelta, pt);
}


void CSmpView::OnSliderZoom()
{
	auto frm = (CMainFrame*)AfxGetMainWnd();
	auto sld = (CMFCRibbonSlider*)KwGetRibbonMenu(&frm->m_wndRibbonBar, ID_SLIDER_Zoom);
	int iz = sld->GetPos();
	//TRACE("%s(%d)\n", __FUNCTION__, iz);
	auto This = this;
	SetLambdaTimer("zoom_msg", 1000, [This](int, PAS)
		{
			_KwBeginInvoke(This, []() /// foreground 작업 큐
				{
					AfxMessageBox(L"개발 예정 입니다.");
				});
		}, 1);
}


void CSmpView::OnAlwaysSyncSD()
{
	auto frm = (CMainFrame*)AfxGetMainWnd();
	auto cCheck = (CMFCRibbonCheckBox*)KwGetRibbonMenu(&frm->m_wndRibbonBar, ID_AlwaysSyncSD);
	auto bChecked = cCheck->IsChecked();
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	auto app = (CSmpApp4*)AfxGetApp();
	(*app->_sjoSet)("fSyncSchDetail") = L"Always";
}


void CSmpView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	OnBoxSelected(true);

	CSafeViewBase::OnLButtonDblClk(nFlags, point);
}
