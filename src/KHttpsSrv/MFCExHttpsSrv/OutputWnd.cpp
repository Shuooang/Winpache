// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface
// (the "Fluent UI") and is provided only as referential material to supplement the
// Microsoft Foundation Classes Reference and related electronic documentation
// included with the MFC C++ library software.
// License terms to copy, use or distribute the Fluent UI are available separately.
// To learn more about our Fluent UI licensing program, please visit
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#include "pch.h"
#include "framework.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar
COutputWnd* COutputWnd::s_me = NULL;

COutputWnd::COutputWnd() noexcept
{
	s_me = this;
	auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
	_id = ivc->ViewRegister(this);//BG에서 이 창이 살아 있는지 확인할 필요가 있을때 사용
	// OnDestroy에서 ViewRemove(_id)도 해줘야 한다.

}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePaneExInvokable)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create tabs window:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}

	// Create output panes:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndOutputBuild.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndOutputDebug.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
		!m_wndOutputFind.Create(dwStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("Failed to create output windows\n");
		return -1;      // fail to create
	}

	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// Attach list windows to tab:
	bNameValid = strTabName.LoadString(IDS_BUILD_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputBuild, strTabName, (UINT)0);
	bNameValid = strTabName.LoadString(IDS_DEBUG_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputDebug, strTabName, (UINT)1);
	bNameValid = strTabName.LoadString(IDS_FIND_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputFind, strTabName, (UINT)2);



	// Fill output tabs with some dummy text (nothing magic here)
	FillBuildWindow();
	FillDebugWindow();
	FillFindWindow();

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// Tab control should cover the whole client area:
	m_wndTabs.SetWindowPos (nullptr, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);
		auto v1 = (int)dc.GetTextExtent(strItem).cx;
		cxExtentMax = cxExtentMax < v1 ? v1 : cxExtentMax;
		//cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillBuildWindow()
{
// 	m_wndOutputBuild.AddString(_T("------------------------------------------------------"));
// 	m_wndOutputBuild.AddString(_T("Trace output is being displayed here."));
// 	m_wndOutputBuild.AddString(_T("The output is being displayed in rows of a list view"));
// 	m_wndOutputBuild.AddString(_T("but you can change the way it is displayed as you wish..."));
}
void COutputWnd::TraceQueue(string txt)
{
	if(!::IsWindow(m_hWnd))
		return;
	AUTOLOCK(_cs_lstTrace);//?destroy 7.5
	vector<string> txts;
	KwCutByToken(txt.c_str(), "\r\n", txts);
	for(auto t : txts)
		_lstTrace.push_back(t);
}

void COutputWnd::TraceFlush()
{
	KwBeginInvoke(this, ([&]()-> void { //?beginInvoke 4
		COutputWnd::s_me->TraceFlushFore();//?ExTrace 7 //?destroy 8
		}));
}
void COutputWnd::TraceFlushFore()
{
	FOREGROUND();
	if(!::IsWindow(m_hWnd))
		return;
	AUTOLOCK(_cs_lstTrace);//?destroy 8.5

// 	auto n = _lstTrace.size();
// 	for(size_t i = 0;i<n;i++)
	CString stm = KwGetCurrentTimeFullString();
	for(auto& v : _lstTrace)
	{
		//string str = _lstTrace.pop_back();
		CStringW msg(v.c_str());
		int i0 = -1;
		int icnt = 0;
		for(int i=0;i<5;i++)
		{
			if(m_wndOutputBuild.GetCount() <= i)
				break;
			CString txt;
			m_wndOutputBuild.GetText(i, txt);
			std::vector<std::wstring> ars;
			KwCutByToken(txt, L"\t", ars, true);
			if(ars.size() > 1 && ars[1] == (PWS)msg)/// 이미 동일한게 바로 밑에 있어.
			{
				if(ars.size() > 2)
				{
					CString cnt = ars[2].c_str();
					cnt.Trim(L"()");
					icnt = KwAtoi((PWS)cnt);
					ASSERT(icnt >= 2);
				}
				else
					icnt = 1;
				i0 = i;
				break;
			}
		}
		CStringW sw;
		if(i0 < 0)
		{
			sw.Format(L"%s \t%s", stm, msg);
			m_wndOutputBuild.InsertString(0, sw);
		}
		else
		{
			icnt++;
			CString cnt1; cnt1.Format(L"(%d)", icnt);
			sw.Format(L"%s \t%s \t%s", stm, msg, cnt1);
			m_wndOutputBuild.InsertString(0, sw);
			m_wndOutputBuild.DeleteString(i0 +1);
		}
	}
	_lstTrace.clear();
#ifdef _DEBUG
	const int nmax = 100;
	const int ncut = 50;
#else
	const int nmax = 5000;
	const int ncut = 500;
#endif // _DEBUG
	int n = m_wndOutputBuild.GetCount();
	if(n > 100)// 너무 많으면 한꺼번에 오래된 것 없애
	{
		for(int i = n - 1; i > n - 50; i--)
			m_wndOutputBuild.DeleteString(i);
	}
}

void COutputWnd::FillDebugWindow()
{
	m_wndOutputDebug.AddString(_T("Reserved."));
	m_wndOutputDebug.AddString(_T("Debug output is being displayed here."));
	m_wndOutputDebug.AddString(_T("The output is being displayed in rows of a list view"));
	m_wndOutputDebug.AddString(_T("but you can change the way it is displayed as you wish..."));
}

void COutputWnd::FillFindWindow()
{
	m_wndOutputFind.AddString(_T("Reserved."));
	m_wndOutputFind.AddString(_T("Find output is being displayed here."));
	m_wndOutputFind.AddString(_T("The output is being displayed in rows of a list view"));
	m_wndOutputFind.AddString(_T("but you can change the way it is displayed as you wish..."));
}

void COutputWnd::UpdateFonts()
{
	// 갑자기 시스템 폰트로 바뀜
// 	m_wndOutputBuild.SetFont(&afxGlobalData.fontRegular);
// 	m_wndOutputDebug.SetFont(&afxGlobalData.fontRegular);
// 	m_wndOutputFind.SetFont(&afxGlobalData.fontRegular);
	/* 이거 갑자기 안되.
	CFont fntItem;
	BOOL bf = fntItem.CreateFont(
		15,                  // 문자 폭
		0,                   // 문자 높이
		0,                   // 문자 기울기
		0,                   // 문자 방향
		FW_NORMAL,           // 문자 굵기
		FALSE,               // 기울기
		FALSE,               // 밑줄
		0,                   // 취소선
		DEFAULT_CHARSET,     // 문자셋
		OUT_DEFAULT_PRECIS,  // 출력 정확도
		CLIP_DEFAULT_PRECIS, // 클리핑 정확도
		DEFAULT_QUALITY,     // 출력의 질
		DEFAULT_PITCH | FF_SWISS, L"Arial ");*/
	
	CFont font;
	LOGFONT lf;
	::ZeroMemory(&lf, sizeof(lf));
	lf.lfHeight = 16;
	lf.lfWeight = FW_NORMAL;// FW_BOLD;
	tchcpy(lf.lfFaceName, (LPCWSTR)"Consolas");
	BOOL bf1 = font.CreateFontIndirect(&lf);
//	GetDlgItem(IDC_STATIC)->SetFont(&font);

	m_wndOutputBuild.SetFont(&font);
	m_wndOutputDebug.SetFont(&font);
	m_wndOutputFind.SetFont(&font);
	font.Detach();

}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList() noexcept
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList message handlers

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	//MessageBox(_T("Copy output"));
	Tss ss;
	int n = this->GetCount();
	for(int i=0;i<n;i++)
	{
		CString s;
		GetText(i, s);
		ss << (LPCWSTR)s << L"\r\n";
	}
	BOOL b = KwCopyTextClipboad(this, ss.str().c_str());
}

void COutputList::OnEditClear()
{
	//MessageBox(_T("Clear output"));
	this->ResetContent();
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != nullptr && pParentBar != nullptr)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();

	}
}


void COutputWnd::OnClose()
{
	// 여긴 앱 닫어도 안온다. WM_CLOSE는 애초에 [x]를 누를때 인거같다.
	auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
	ivc->ViewRemove(_id);

	__super::OnClose();
}


void COutputWnd::OnDestroy()
{
	auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
	ivc->ViewRemove(_id);//BG에서 이 창이 살아 있는지 확인할 필요가 있을때 사용
	__super::OnDestroy();

	// TODO: Add your message handler code here
}
