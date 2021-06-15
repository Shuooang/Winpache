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

// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "MFCSmpApp4.h"

#include "MainFrm.h"


#include "DockTool.h"///?kdw

#include "Requ.h"
#include "KwLib64/MfcEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndExInvokable)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndExInvokable)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	//ON_COMMAND(ID_VIEW_FILEVIEW, &CMainFrame::OnViewFileView)
	//ON_UPDATE_COMMAND_UI(ID_VIEW_FILEVIEW, &CMainFrame::OnUpdateViewFileView)
	//ON_COMMAND(ID_VIEW_CLASSVIEW, &CMainFrame::OnViewClassView)
	//ON_UPDATE_COMMAND_UI(ID_VIEW_CLASSVIEW, &CMainFrame::OnUpdateViewClassView)
	ON_COMMAND(ID_VIEW_OUTPUTWND, &CMainFrame::OnViewOutputWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUTWND, &CMainFrame::OnUpdateViewOutputWindow)
	ON_COMMAND(ID_VIEW_PROPERTIESWND, &CMainFrame::OnViewPropertiesWindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIESWND, &CMainFrame::OnUpdateViewPropertiesWindow)
	ON_WM_SETTINGCHANGE()

	ON_COMMAND(ID_ViewDockCustomDetail, &CMainFrame::OnViewDockCustomDetail)//?dock everytime: 리본 메뉴에서 보이기 숨기기
	ON_UPDATE_COMMAND_UI(ID_ViewDockCustomDetail, &CMainFrame::OnUpdateViewDockCustomDetail)//?dock everytime:
	ON_COMMAND(ID_ViewDockCustomHistory, &CMainFrame::OnViewDockCustomHistory)
	ON_UPDATE_COMMAND_UI(ID_ViewDockCustomHistory, &CMainFrame::OnUpdateViewDockCustomHistory)
	ON_COMMAND(ID_ViewDockMyHistory, &CMainFrame::OnViewDockMyHistory)
	ON_UPDATE_COMMAND_UI(ID_ViewDockMyHistory, &CMainFrame::OnUpdateViewDockMyHistory)
	ON_COMMAND(ID_ViewDockMyPetList, &CMainFrame::OnViewDockMyPetList)
	ON_UPDATE_COMMAND_UI(ID_ViewDockMyPetList, &CMainFrame::OnUpdateViewDockMyPetList)
	ON_COMMAND(ID_ViewDockPetDetail, &CMainFrame::OnViewDockPetDetail)
	ON_UPDATE_COMMAND_UI(ID_ViewDockPetDetail, &CMainFrame::OnUpdateViewDockPetDetail)
	ON_COMMAND(ID_ViewDockMyPetChecks, &CMainFrame::OnViewDockMyPetChecks)
	ON_UPDATE_COMMAND_UI(ID_ViewDockMyPetChecks, &CMainFrame::OnUpdateViewDockMyPetChecks)
	ON_COMMAND(ID_ViewDockScheduleDetail, &CMainFrame::OnViewDockScheduleDetail)
	ON_UPDATE_COMMAND_UI(ID_ViewDockScheduleDetail, &CMainFrame::OnUpdateViewDockScheduleDetail)
	ON_COMMAND(ID_ViewDockOrder, &CMainFrame::OnViewDockOrder)
	ON_COMMAND(ID_ViewDockProducts, &CMainFrame::OnViewDockProducts)
	ON_COMMAND(ID_ViewDockOptions, &CMainFrame::OnViewDockOptions)
	ON_UPDATE_COMMAND_UI(ID_ViewDockOrder, &CMainFrame::OnUpdateViewDockOrder)
	ON_UPDATE_COMMAND_UI(ID_ViewDockProducts, &CMainFrame::OnUpdateViewDockProducts)
	ON_UPDATE_COMMAND_UI(ID_ViewDockOptions, &CMainFrame::OnUpdateViewDockOptions)
END_MESSAGE_MAP()











// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndExInvokable::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 사용할 수 있는 다른 스타일...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // FALSE로 설정하여 탭 영역 오른쪽에 닫기 단추를 배치합니다.
	mdiTabParams.m_bTabIcons = FALSE;    // TRUE로 설정하여 MDI 탭의 문서 아이콘을 활성화합니다.
	mdiTabParams.m_bAutoColor = TRUE;    // FALSE로 설정하여 MDI 탭의 자동 색 지정을 비활성화합니다.
	mdiTabParams.m_bDocumentMenu = TRUE; // 탭 영역의 오른쪽 가장자리에 문서 메뉴를 활성화합니다.
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 메뉴 항목 이미지를 로드합니다(표준 도구 모음에 없음).
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 도킹 창을 만듭니다.
	if (!CreateDockingWindows())
	{
		TRACE0("도킹 창을 만들지 못했습니다.\n");
		return -1;
	}

	//m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	//DockPane(&m_wndFileView);
	//CDockablePane* pTabbedBar = nullptr;
	//m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);


	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);

	// 보관된 값에 따라 비주얼 관리자 및 스타일을 설정합니다.
	OnApplicationLook(theApp.m_nAppLook);

	// 향상된 창 관리 대화 상자를 활성화합니다.
	EnableWindowsDialog(ID_WINDOW_MANAGER, ID_WINDOW_MANAGER, TRUE);

	// 창 제목 표시줄에서 문서 이름 및 애플리케이션 이름의 순서를 전환합니다.
	// 문서 이름이 축소판 그림과 함께 표시되므로 작업 표시줄의 기능성이 개선됩니다.
	ModifyStyle(0, FWS_PREFIXTITLE);

	///?kdw 전쳬 창 캡션
	SetTitle(L"PetMe 샾 관리 프로그램");//?kdw 메인프레임 타이틀 변경 1단계
	auto lt = dynamic_cast<KLambdaTimer*>(this);
	lt->SetLambdaTimer("menu_blink", 500, [&](int ntm, PAS tmk)
		{
			KTrace(L"%d. %s (%s)\n", ntm, __FUNCTIONW__, L"Lambda timer menu_blink");
		}, 10);
	return 0;
}
void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)//?kdw 메인프레임 타이틀 변경 2단계
{
	if((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave it alone!
	///AfxSetWindowText(m_hWnd, L"샵 관리 프로그램");이거 안먹는다.
	/// 이 아래 다 코멘트
	//// allow hook to set the title (used for OLE support)
	//if(m_pNotifyHook != NULL && m_pNotifyHook->OnUpdateFrameTitle())
	//	return;

	//CDocument* pDocument = GetActiveDocument();
	//if(bAddToTitle && pDocument != NULL)
	//	UpdateFrameTitleForDocument(pDocument->GetTitle());
	//else
	//	UpdateFrameTitleForDocument(NULL);
}
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndExInvokable::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 클래스 뷰를 만듭니다.
	//CString strClassView;
	//bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
	//ASSERT(bNameValid);
	//if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("클래스 뷰 창을 만들지 못했습니다.\n");
	//	return FALSE; // 만들지 못했습니다.
	//}

	// 파일 뷰를 만듭니다.
	//CString strFileView;
	//bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	//ASSERT(bNameValid);
	//if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	//{
	//	TRACE0("파일 뷰 창을 만들지 못했습니다.\n");
	//	return FALSE; // 만들지 못했습니다.
	//}

	// 출력 창을 만듭니다.
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("출력 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	// 속성 창을 만듭니다.
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("속성 창을 만들지 못했습니다.\n");
		return FALSE; // 만들지 못했습니다.
	}

	/// /////////////////////////////////////////////////////////////
	_dock.InitDocks(this);///?kdw 
	/// /////////////////////////////////////////////////////////////



	/// Ribbon Menu 초기화 //////////////////////////////////////////
	auto sld = (CMFCRibbonSlider*)KwGetRibbonMenu(&m_wndRibbonBar, ID_SLIDER_Zoom);
	sld->SetRange(100, 200);
	sld->SetPos(130);
	/// Ribbon Menu 초기화 //////////////////////////////////////////

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	//HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	//HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	//m_wndClassView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndExInvokable::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndExInvokable::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	m_wndOutput.UpdateFonts();
	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewFileView()
{
	// 현재 상태에 따라 창을 표시하거나 활성화합니다.
	// 창을 닫으려면 창 프레임의 [x] 단추를 사용해야 합니다.
	//m_wndFileView.ShowPane(TRUE, FALSE, TRUE);
	//m_wndFileView.SetFocus();
}

void CMainFrame::OnUpdateViewFileView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewClassView()
{
	// 현재 상태에 따라 창을 표시하거나 활성화합니다.
	// 창을 닫으려면 창 프레임의 [x] 단추를 사용해야 합니다.
	//m_wndClassView.ShowPane(TRUE, FALSE, TRUE);
	//m_wndClassView.SetFocus();
}

void CMainFrame::OnUpdateViewClassView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewOutputWindow()
{
	// 현재 상태에 따라 창을 표시하거나 활성화합니다.
	// 창을 닫으려면 창 프레임의 [x] 단추를 사용해야 합니다.
	m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
	m_wndOutput.SetFocus();
}

void CMainFrame::OnUpdateViewOutputWindow(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnViewPropertiesWindow()
{
	// 현재 상태에 따라 창을 표시하거나 활성화합니다.
	// 창을 닫으려면 창 프레임의 [x] 단추를 사용해야 합니다.
	m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
	m_wndProperties.SetFocus();
}

void CMainFrame::OnUpdateViewPropertiesWindow(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}


void CMainFrame::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CMDIFrameWndExInvokable::OnSettingChange(uFlags, lpszSection);
	m_wndOutput.UpdateFonts();
};



//?dock 보기이 숨기기 함수 매크로 (고정)
#define OnVIEWMAN(XXXX) \
void CMainFrame::OnView##XXXX() \
{	DockTool::ShowHide(this, *Pane(IDD_##XXXX)); \
} \
void CMainFrame::OnUpdateView##XXXX(CCmdUI* pCmdUI) \
{	DockTool::ShowHide(this, *Pane(IDD_##XXXX), pCmdUI); \
}
/*void CMainFrame::OnViewCustomDetail()
{	DockTool::ShowHide(this, *Pane(IDD_DockCustomDetail));}
void CMainFrame::OnUpdateViewCustomDetail(CCmdUI* pCmdUI)
{	DockTool::ShowHide(this, *Pane(IDD_DockCustomDetail), pCmdUI);}*/




OnVIEWMAN(DockCustomDetail); //?dock everytime: 보이기 숨기기 구현
OnVIEWMAN(DockCustomHistory);
OnVIEWMAN(DockMyHistory);
OnVIEWMAN(DockMyPetList);
OnVIEWMAN(DockPetDetail);
OnVIEWMAN(DockMyPetChecks);
OnVIEWMAN(DockScheduleDetail);
OnVIEWMAN(DockOrder);
OnVIEWMAN(DockProducts);
OnVIEWMAN(DockOptions);


void CMainFrame::ContextCategory(int idd, bool bShow)
{
	KRibbon::ContextCategory(this, &m_wndRibbonBar, bShow, idd);
	/*
	auto pfm = this;// (CMainFrame*)AfxGetMainWnd();
	if(bShow)
	{
		// 순서 지켜야 함
		pfm->m_wndRibbonBar.ShowContextCategories(idd, TRUE);
		int ret = pfm->m_wndRibbonBar.ActivateContextCategory(idd);
	}
	else
	{
		//	비활성화		// 단일 Context
		pfm->m_wndRibbonBar.ShowContextCategories(idd, FALSE);
		// 모든 Context
		pfm->m_wndRibbonBar.HideAllContextCategories();
	}
	// 이후 반드시 호출
	pfm->m_wndRibbonBar.RecalcLayout();
	pfm->m_wndRibbonBar.RedrawWindow();

	SendMessage(WM_NCPAINT, 0, 0);
	*/
}



///?kdw TODO: 이거 DialogEx 인가 에도 Invokable 해줘야 하고, 이것도 해줘야 한다.
LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = 0;
	try
	{
		try
		{
			lr = CMDIFrameWndExInvokable::WindowProc(message, wParam, lParam);
		} CATCH_APP;
	}
	catch(KException* e)/// 그리고 여기가 2번째로 오니 여기서 모든 오류 처리 해야 한다.
	{
		TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTION__, __LINE__);
		//auto pfn = (CMainFrame*)AfxGetMainWnd();
		AppBase::CallClientException(e);//CMainFrame
		//ASSERT(e->IsAutoDelete());
		e->Delete();
	}
	return lr;
}

//void CMainFrame::OnViewDockOrder()
//{
//}
//void CMainFrame::OnViewDockProducts()
//{
//}
//void CMainFrame::OnViewDockOptions()
//{
//}
//void CMainFrame::OnUpdateViewDockOrder(CCmdUI* pCmdUI)
//{
//}
//void CMainFrame::OnUpdateViewDockProducts(CCmdUI* pCmdUI)
//{
//}
//void CMainFrame::OnUpdateViewDockOptions(CCmdUI* pCmdUI)
//{
//}


