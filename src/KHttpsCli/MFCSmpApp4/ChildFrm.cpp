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

// ChildFrm.cpp: CChildFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "MFCSmpApp4.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	ON_COMMAND(ID_FILE_PRINT, &CChildFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CChildFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CChildFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CChildFrame::OnUpdateFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CChildFrame 생성/소멸

CChildFrame::CChildFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서 Window 클래스 또는 스타일을 수정합니다.
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame 진단

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 메시지 처리기

void CChildFrame::OnFilePrint()
{
	if (m_dockManager.IsPrintPreviewValid())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CChildFrame::OnFilePrintPreview()
{
	if (m_dockManager.IsPrintPreviewValid())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // 인쇄 미리 보기 모드를 닫습니다.
	}
}

void CChildFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_dockManager.IsPrintPreviewValid());
}



int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	//SetWindowText(L"타임테이블1");

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if(pSysMenu)
	{
		ASSERT(pSysMenu != NULL);
		VERIFY(pSysMenu->DeleteMenu(SC_CLOSE, MF_BYCOMMAND));///?kdw prevent child frame
		VERIFY(pSysMenu->RemoveMenu(5, MF_BYPOSITION));
	}
	return 0;
}
void CChildFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(nID == SC_CLOSE)///?kdw prevent child frame
		return;
	CMDIChildWnd::OnSysCommand(nID, lParam);
}

void CChildFrame::OnClose()
{
	///?kdw prevent closing child frame //CMDIChildWndEx::OnClose(); 
	// tab button 누를때 안닫힌다.
}

CString CChildFrame::GetFrameText() const///?kdw 
{
	CString str = CMDIChildWndEx::GetFrameText();
	bool IsTabbedMDIChild = m_pMDIFrame != NULL && m_pMDIFrame->AreMDITabs();
	if(IsTabbedMDIChild)// && str == _T("TestFile2.ini"))
		str = _T("타임 테이블");//?kdw child frame title 바꾸기

	return str;
}

