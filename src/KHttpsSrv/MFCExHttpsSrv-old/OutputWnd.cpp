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
	_id = ivc->ViewRegister(this);//BG���� �� â�� ��� �ִ��� Ȯ���� �ʿ䰡 ������ ���
	// OnDestroy���� ViewRemove(_id)�� ����� �Ѵ�.

}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
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
	_lstTrace.push_back(txt);
}

void COutputWnd::TraceFlush()
{
	if(!::IsWindow(m_hWnd))
		return;
	AUTOLOCK(_cs_lstTrace);//?destroy 8.5

// 	auto n = _lstTrace.size();
// 	for(size_t i = 0;i<n;i++)
	for(auto& v : _lstTrace)
	{
		//string str = _lstTrace.pop_back();
		CStringW sw(v.c_str());
		m_wndOutputBuild.InsertString(0, sw);
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
	if(n > 100)// �ʹ� ������ �Ѳ����� ������ �� ����
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
	m_wndOutputBuild.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputDebug.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputFind.SetFont(&afxGlobalData.fontRegular);
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
	// ���� �� �ݾ �ȿ´�. WM_CLOSE�� ���ʿ� [x]�� ������ �ΰŰ���.
	auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
	ivc->ViewRemove(_id);

	CDockablePane::OnClose();
}


void COutputWnd::OnDestroy()
{
	auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
	ivc->ViewRemove(_id);//BG���� �� â�� ��� �ִ��� Ȯ���� �ʿ䰡 ������ ���
	CDockablePane::OnDestroy();

	// TODO: Add your message handler code here
}
