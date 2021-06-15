#include "pch.h"
#include "Mainfrm.h"
#include "DockMyHistory.h"

#include "resource.h"
//?kdw 
#include "AppBase.h"

IMPLEMENT_DYNCREATE(DockMyHistoryPane, CPaneForm)

// DockMyHistory

IMPLEMENT_DYNCREATE(DockMyHistory, DockClientBase)
DockMyHistory* DockMyHistory::s_me = NULL;

DockMyHistory::DockMyHistory(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn) //DockMyHistory::IDD IDD_DockMyHistory
{
	s_me = this;
}

DockMyHistory::~DockMyHistory()
{
}

void DockMyHistory::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, _cList);
}

BEGIN_MESSAGE_MAP(DockMyHistory, DockClientBase)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &DockMyHistory::OnBnClickedButton1)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()


// DockMyHistory 진단입니다.

#ifdef _DEBUG
void DockMyHistory::AssertValid() const
{
	__super::AssertValid();
}

#ifndef _WIN32_WCE
void DockMyHistory::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}
#endif
#endif //_DEBUG


// DockMyHistory 메시지 처리기입니다.
// CBranchVu1 메시지 처리기입니다.
void DockMyHistory::OnInitialUpdate()
{
	if(m_bInited)
		return;
	__super::OnInitialUpdate();
	m_bInited = TRUE;


	InitPropList();

	AdjustLayout();
	KwSetListReportStyle(&_cList);

}

void DockMyHistory::AdjustLayout()
{
	if(GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rc;
	GetClientRect(rc);

	int cyTlb = 30;
	if(_cList.GetSafeHwnd() && !_cList.IsIconic())
		_cList.SetWindowPos(nullptr, rc.left, rc.top + cyTlb, rc.Width(), rc.Height() - (cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

void DockMyHistory::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	//	if(m_tree.GetSafeHwnd())
	{
		CRect rc;
		GetClientRect(rc);
		//rc.top += H_TBAR;
		//m_tree.MoveWindow(rc);
	}
	AdjustLayout();

}

void DockMyHistory::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);
	_cList.SetFocus();
}

void DockMyHistory::OnBnClickedButton1()
{
	//auto app = (CMfcSmpApp1App*)AfxGetApp();
	auto mfm = (CMainFrame*)AfxGetMainWnd();
	CMainPool::QueueFunc([&]
		{
			JObj jo;
			jo.ErrTest();// O("xxx");
		});
	auto d1 = mfm->Dock(IDD_DockMyHistory);
	if(d1)
		AfxMessageBox(L"DockMyHistory ok");
}


//>	MFCSmpApp4.exe!DockMyHistory::OnBnClickedButton1() Line 114	C++
//	mfc140ud.dll!_AfxDispatchCmdMsg(CCmdTarget * pTarget=0x00000205736a56f0, unsigned int nID=1002, int nCode=0, void(CCmdTarget::*)() pfn=0x00007ff7149000bd, void * pExtra=0x0000000000000000, unsigned __int64 nSig=58, AFX_CMDHANDLERINFO * pHandlerInfo=0x0000000000000000) Line 78	C++
//	mfc140ud.dll!CCmdTarget::OnCmdMsg(unsigned int nID=1002, int nCode=0, void * pExtra=0x0000000000000000, AFX_CMDHANDLERINFO * pHandlerInfo=0x0000000000000000) Line 372	C++
//	mfc140ud.dll!CView::OnCmdMsg(unsigned int nID=1002, int nCode=0, void * pExtra=0x0000000000000000, AFX_CMDHANDLERINFO * pHandlerInfo=0x0000000000000000) Line 164	C++
//	mfc140ud.dll!CWnd::OnCommand(unsigned __int64 wParam=1002, __int64 lParam=726108) Line 2801	C++
//	mfc140ud.dll!CWnd::OnWndMsg(unsigned int message=273, unsigned __int64 wParam=1002, __int64 lParam=726108, __int64 * pResult=0x0000009df614d698) Line 2113	C++
//	mfc140ud.dll!CWnd::WindowProc(unsigned int message=273, unsigned __int64 wParam=1002, __int64 lParam=726108) Line 2099	C++
///	MFCSmpApp4.exe!DockClientBase::WindowProc(unsigned int message=273, unsigned __int64 wParam=1002, __int64 lParam=726108) Line 281	C++
//	mfc140ud.dll!AfxCallWndProc(CWnd * pWnd=0x00000205736a56f0, HWND__ * hWnd=0x00000000000f1240, unsigned int nMsg=273, unsigned __int64 wParam=1002, __int64 lParam=726108) Line 265	C++
//	mfc140ud.dll!AfxWndProc(HWND__ * hWnd=0x00000000000f1240, unsigned int nMsg=273, unsigned __int64 wParam=1002, __int64 lParam=726108) Line 418	C++
//	mfc140ud.dll!AfxWndProcBase(HWND__ * hWnd=0x00000000000f1240, unsigned int nMsg=273, unsigned __int64 wParam=1002, __int64 lParam=726108) Line 299	C++
//	user32.dll!00007ffaa4e6e858()	Unknown
//;;;
//	user32.dll!00007ffaa4e6c050()	Unknown
//	mfc140ud.dll!CWnd::IsDialogMessageW(tagMSG * lpMsg=0x0000020573605ea8) Line 194	C++
//	mfc140ud.dll!CWnd::PreTranslateInput(tagMSG * lpMsg=0x0000020573605ea8) Line 4607	C++
//	mfc140ud.dll!CFormView::PreTranslateMessage(tagMSG * pMsg=0x0000020573605ea8) Line 234	C++
///	MFCSmpApp4.exe!DockClientBase::PreTranslateMessage(tagMSG * pMsg=0x0000020573605ea8) Line 211	C++
//	mfc140ud.dll!CWnd::WalkPreTranslateTree(HWND__ * hWndStop=0x0000000000310388, tagMSG * pMsg=0x0000020573605ea8) Line 3379	C++
//	mfc140ud.dll!AfxInternalPreTranslateMessage(tagMSG * pMsg=0x0000020573605ea8) Line 233	C++
//	mfc140ud.dll!CWinThread::PreTranslateMessage(tagMSG * pMsg=0x0000020573605ea8) Line 778	C++
//	mfc140ud.dll!AfxPreTranslateMessage(tagMSG * pMsg=0x0000020573605ea8) Line 252	C++
//	mfc140ud.dll!AfxInternalPumpMessage() Line 178	C++
//	mfc140ud.dll!CWinThread::PumpMessage() Line 900	C++
//	mfc140ud.dll!CWinThread::Run() Line 629	C++
//	mfc140ud.dll!CWinApp::Run() Line 787	C++
//	mfc140ud.dll!AfxWinMain(HINSTANCE__ * hInstance=0x00007ff714860000, HINSTANCE__ * hPrevInstance=0x0000000000000000, wchar_t * lpCmdLine=0x00000205735e298c, int nCmdShow=10) Line 47	C++
//	MFCSmpApp4.exe!wWinMain(HINSTANCE__ * hInstance=0x00007ff714860000, HINSTANCE__ * hPrevInstance=0x0000000000000000, wchar_t * lpCmdLine=0x00000205735e298c, int nCmdShow=10) Line 26	C++
//	MFCSmpApp4.exe!invoke_main() Line 123	C++


void DockMyHistory::InitPropList(ShJObj sjo)
{
	

}
