#include "pch.h"
#include "Mainfrm.h"
#include "DockCustomHistory.h"

#include "resource.h"
//?kdw 

IMPLEMENT_DYNCREATE(DockCustomHistoryPane, CPaneForm)

// DockCustomHistory

IMPLEMENT_DYNCREATE(DockCustomHistory, DockClientBase)
DockCustomHistory* DockCustomHistory::s_me = NULL;

DockCustomHistory::DockCustomHistory(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn) //DockCustomHistory::IDD IDD_DockCustomHistory
{
	s_me = this;
}

DockCustomHistory::~DockCustomHistory()
{
}

void DockCustomHistory::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, _cList);
}

BEGIN_MESSAGE_MAP(DockCustomHistory, DockClientBase)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &DockCustomHistory::OnBnClickedButton1)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()


// DockCustomHistory 진단입니다.

#ifdef _DEBUG
void DockCustomHistory::AssertValid() const
{
	__super::AssertValid();
}

#ifndef _WIN32_WCE
void DockCustomHistory::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}
#endif
#endif //_DEBUG


// DockCustomHistory 메시지 처리기입니다.
// CBranchVu1 메시지 처리기입니다.
void DockCustomHistory::OnInitialUpdate()
{
	if(m_bInited)
		return;
	__super::OnInitialUpdate();
	m_bInited = TRUE;


	InitPropList();

	AdjustLayout();

	KwSetListReportStyle(&_cList);
}

void DockCustomHistory::AdjustLayout()
{
	if(GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rc;
	GetClientRect(rc);

	int cyTlb = 30;// m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	//m_wndObjectCombo.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	//m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	if(_cList.GetSafeHwnd() && !_cList.IsIconic())
		_cList.SetWindowPos(nullptr, rc.left, rc.top + cyTlb, rc.Width(), rc.Height() - (cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

void DockCustomHistory::OnSize(UINT nType, int cx, int cy)
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

void DockCustomHistory::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);
	//_cProperty.SetFocus();
}

void DockCustomHistory::OnBnClickedButton1()
{
	//auto app = (CMfcSmpApp1App*)AfxGetApp();
	auto mfm = (CMainFrame*)AfxGetMainWnd();
	auto d1 = mfm->Dock(IDD_DockCustomHistory);
	if(d1)
		AfxMessageBox(L"DockCustomHistory ok");
}



void DockCustomHistory::InitPropList(ShJObj sjo)
{
	
}

