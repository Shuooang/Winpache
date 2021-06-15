
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "MFCAppServerEx2.h"

#include "MainFrm.h"
#include "CmnView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndInvokable)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndInvokable)
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndInvokable::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	//KwBeginInvoke(this, ([&]()-> void
	//	{ //?beginInvoke 4
	//		auto app = (CMFCAppServerEx2*)AfxGetApp();
	//		app->InitApp();
	//	}));

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndInvokable::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndInvokable::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndInvokable::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기



void CMainFrame::OnClose()
{
	for (POSITION posTemplate = AfxGetApp()->GetFirstDocTemplatePosition(); posTemplate; )
	{
		auto pDocTemplate = AfxGetApp()->GetNextDocTemplate(posTemplate);
		POSITION posDoc = pDocTemplate->GetFirstDocPosition();
		while (posDoc)
		{
			CDocument* pDoc = pDocTemplate->GetNextDoc(posDoc);
			POSITION posView = pDoc->GetFirstViewPosition();
			while (posView)
			{
				CView* pView = pDoc->GetNextView(posView);
				auto cvu = dynamic_cast<CmnView*>(pView);
				if (cvu)
					cvu->Shutdown("CMainFrame::OnClose()");//CChildFrame::OnClose() 에서도 불러야 한다.
				//pView->GetParentFrame()->DestroyWindow();
				break;//현재 doc하나 당 single view
			}
		}
	}





/*
	auto fr = this->GetActiveFrame();
	if(fr)
	{
		auto vu = fr->GetActiveView();
		if(vu)
		{
			auto cvu = dynamic_cast<CmnView*>(vu);
			if(cvu)
				cvu->Shutdown("CMainFrame::OnClose()");//CChildFrame::OnClose() 에서도 불러야 한다.
		}
	}
*/
	CMDIFrameWndInvokable::OnClose();
}
