#include "pch.h"
#include "Kw_tool.h"
#include "DlgTool.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CMDIFrameWndInvokable, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMDIFrameWndInvokable, CMDIFrameWnd)
	ON_MESSAGE(WM_USER_INVOKE, &CMDIFrameWndInvokable::OnBeginInvoke)//?beginInvoke 2
	ON_WM_TIMER() //?LbTimer 4
END_MESSAGE_MAP()

#ifdef _DEBUG
LRESULT CMDIFrameWndInvokable::OnBeginInvoke(WPARAM wParam, LPARAM lParam)
{
	CSyncAutoLock __lock(KBeginInvoke::GetCS(), TRUE, __FUNCTION__, __LINE__, "CMDIFrameWndInvokable");
	KBeginInvoke* pbi = (KBeginInvoke*)lParam;
	pbi->_bCalled = true; (*pbi->m_pLambda)();
	KBeginInvoke::freeInvokeFree();
	return 0;
}
#else
OnBeginInvoke_Define(CMDIFrameWndInvokable)//?beginInvoke 3
#endif // _DEBUG
CMDIFrameWndInvokable::CMDIFrameWndInvokable() noexcept
	: KLambdaTimer(this) //?LbTimer 2
{
	//auto lt = dynamic_cast<KLambdaTimer*>(this);
	//lt->_wnd = this;
}

void CMDIFrameWndInvokable::OnTimer(UINT_PTR nIDEvent)//?LbTimer 5
{
	DoTimerTask(nIDEvent);

	CMDIFrameWnd::OnTimer(nIDEvent);
}





IMPLEMENT_DYNAMIC(CMDIFrameWndExInvokable, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMDIFrameWndExInvokable, CMDIFrameWndEx)
	ON_MESSAGE(WM_USER_INVOKE, &CMDIFrameWndExInvokable::OnBeginInvoke)//?beginInvoke 2
	ON_WM_TIMER() //?LbTimer 4
END_MESSAGE_MAP()

#ifdef _DEBUG
LRESULT CMDIFrameWndExInvokable::OnBeginInvoke(WPARAM wParam, LPARAM lParam)
{	CSyncAutoLock __lock(KBeginInvoke::GetCS(), TRUE, __FUNCTION__, __LINE__, "CMDIFrameWndExInvokable"); 
	KBeginInvoke* pbi = (KBeginInvoke*)lParam; 
	pbi->_bCalled = true; (*pbi->m_pLambda)(); 
	KBeginInvoke::freeInvokeFree(); 
	return 0;
}
#else
OnBeginInvoke_Define(CMDIFrameWndExInvokable)//?beginInvoke 3
#endif // _DEBUG
CMDIFrameWndExInvokable::CMDIFrameWndExInvokable() noexcept
	: KLambdaTimer(this) //?LbTimer 2
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}
void CMDIFrameWndExInvokable::OnTimer(UINT_PTR nIDEvent)//?LbTimer 5
{
	DoTimerTask(nIDEvent);

	CMDIFrameWndEx::OnTimer(nIDEvent);
}









IMPLEMENT_DYNAMIC(CDockablePaneExInvokable, CDockablePane)
BEGIN_MESSAGE_MAP(CDockablePaneExInvokable, CDockablePane)
	ON_MESSAGE(WM_USER_INVOKE, &CDockablePaneExInvokable::OnBeginInvoke)//?beginInvoke 2
	ON_WM_TIMER() //?LbTimer 4
END_MESSAGE_MAP()
#ifdef _DEBUG
LRESULT CDockablePaneExInvokable::OnBeginInvoke(WPARAM wParam, LPARAM lParam)
{
	CSyncAutoLock __lock(KBeginInvoke::GetCS(), TRUE, __FUNCTION__, __LINE__, "CDockablePaneExInvokable");
	KBeginInvoke* pbi = (KBeginInvoke*)lParam;
	pbi->_bCalled = true; (*pbi->m_pLambda)();
	KBeginInvoke::freeInvokeFree();
	return 0;
}
#else
OnBeginInvoke_Define(CDockablePaneExInvokable)//?beginInvoke 3
#endif // _DEBUG
CDockablePaneExInvokable::CDockablePaneExInvokable() noexcept
	: KLambdaTimer(this) //?LbTimer 2
{
}
void CDockablePaneExInvokable::OnTimer(UINT_PTR nIDEvent)//?LbTimer 5
{
	DoTimerTask(nIDEvent);
	CDockablePane::OnTimer(nIDEvent);
}






















///?변경
/// Invoke때 할당한 게 Post 받아서 읿을  수도 있으니, gabage를 프리 하는 방법을 쓴다.
/// static auto free array를 쓴다.
WORD KBeginInvoke::s_srl = 0;

KBeginInvoke::KBeginInvoke(std::function<void()>* pLambda, PAS fnc, int line)
	: m_pLambda(pLambda)
	, m_fnc(fnc)
	, m_line(line)
{
	s_srl++;
	_srl = s_srl;
	_tik = GetTickCount64();
	auto pbi = this;
	//TRACE("!!! Created, %ld, %s (%d)\n", pbi->_srl, pbi->m_fnc, pbi->m_line);
}

void KBeginInvoke::setInvokeFree(KBeginInvoke* pbi)
{
	CSyncAutoLock __lock(KBeginInvoke::GetCS(), TRUE, __FUNCTION__, __LINE__, "KBeginInvoke");
	KPtrList<KBeginInvoke>* pl = getGabage();
	pl->push_back(pbi);
}

void KBeginInvoke::freeInvokeFree()
{
	CSyncAutoLock __lock(KBeginInvoke::GetCS(), TRUE, __FUNCTION__, __LINE__, "KBeginInvoke");
	KPtrList<KBeginInvoke>* pl = getGabage();
	auto sz = pl->size();
	int nDeleted = 0;
	int nNotYet = 0;
	BOOL bRapped = FALSE;
	for(ULONGLONG i = 0; i < sz; i++)
	{
		KBeginInvoke* pbi = (KBeginInvoke*)pl->front();
		if(pbi)
		{
			if(pbi->_bCalled)// 수행하기 전에 free된 경우도 있드라.
			{
				if(i < (sz - 2000))// 2000개는 항상 남겨 두자.
				{
					//TRACE("@@@ CALLED delete, %ld, %s (%d)\n", pbi->_srl, pbi->m_fnc, pbi->m_line);
					delete pbi;
					pl->pop_front();
					nDeleted++;
				}
			}
			else
			{
				//TRACE("### NOT CALLED YET, %ld, %s (%d)\n", pbi->_srl, pbi->m_fnc, pbi->m_line);
				bRapped = TRUE;
				nNotYet++;
				break;
			};
		}
	}
}



IMPLEMENT_DYNAMIC(CFormInvokable, CFormView)

BEGIN_MESSAGE_MAP(CFormInvokable, CFormView)
	ON_MESSAGE(WM_USER_INVOKE, &CFormInvokable::OnBeginInvoke)//?beginInvoke 2
	ON_WM_TIMER() //?LbTimer 4
END_MESSAGE_MAP()

CKTrace std_trace;

#ifdef _DEBUG
LRESULT CFormInvokable::OnBeginInvoke(WPARAM wParam, LPARAM lParam)
{	
	CSyncAutoLock __lock(KBeginInvoke::GetCS(), TRUE, __FUNCTION__, __LINE__, "CFormInvokable");
	KBeginInvoke* pbi = (KBeginInvoke*)lParam;
	//std_cout << "HTTPSCacheServer::onStarted " << std_endl;
	//std_trace << "OnBeginInvoke " << (pbi->_bCalled ? "T" : "F") << " " << wParam << " = " << pbi->_srl << std_endl;
	//std_trace << "OnBeginInvoke " << (pbi->_bCalled ? "T" : "F") << wParam << " = " << pbi->_srl << ", " << pbi->m_fnc << " (" << pbi->m_line << ")" << std_endl;
	if(!pbi->_bCalled)
		_break;
	pbi->_bCalled = true;
	(*pbi->m_pLambda)();
	KBeginInvoke::freeInvokeFree();

	//delete pbi;

	return 0;
}
#else
OnBeginInvoke_Define(CFormInvokable)//?beginInvoke 3
#endif

CFormInvokable::CFormInvokable(UINT nIDTemplate)
	: CFormView(nIDTemplate)//아들이 할아버지 객체 것을 바로 부를수 없어서 아버지가 전달 해 줌.
	, KLambdaTimer(this) //?LbTimer 2
	, m_sz(0, 0)
{
}
CFormInvokable::~CFormInvokable()
{
}
void CFormInvokable::OnTimer(UINT_PTR nIDEvent)//?LbTimer 5
{
	DoTimerTask(nIDEvent);

	CFormView::OnTimer(nIDEvent);
}

BOOL CFormInvokable::OnSizeDefault(UINT nType, int cx, int cy, int nCtrl, int arIdc[])
{

	if(cx == 0 || cy == 0)
		return FALSE;

	//CRect rc[6];
	//int idc[6] ={IDC_path, IDC_certificate, IDC_privatekey, IDC_dhparam, IDC_vpath, IDC_MonitorList,};
	//int n = 6;

	CRect rcc;
	GetClientRect(rcc);
	int w = rcc.Width();
	int h = rcc.Height();
	// width: m_cItems, m_splTBR, m_ie
	// height: m_cChannel m_ie
	if(SIZE_MINIMIZED != nType && m_sz.cx > 0 && m_sz.cy > 0)
	{
		int cw = cx - m_sz.cx;
		int ch = cy - m_sz.cy;

		for(int i=0;i<nCtrl;i++)
		{
			CRect rc;
			KwGetCtrlRect(this, arIdc[i], rc);
			rc.right = w - 8;//우측으로 늘어난다.
			if(i == nCtrl - 1)//맨 아래꺼는 밑으로도 늘어 난다.
			{
				rc.bottom = rcc.bottom - 5;

// 				rc.bottom += ch;
// 				if(h < rc.bottom)
// 					rc.bottom = h;
			}
			GetDlgItem(arIdc[i])->MoveWindow(rc);
		}
	}

	if(SIZE_MINIMIZED != nType)
	{
		m_sz.cx = cx;
		m_sz.cy = cy;
	}
	return TRUE;
}




/// ////////////////////////////////////////////////////////////////////////////////////////
/// ////////////////////////////////////////////////////////////////////////////////////////
/// ////////////////////////////////////////////////////////////////////////////////////////
/// ////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CDlgInvokable, CDialogEx)

BEGIN_MESSAGE_MAP(CDlgInvokable, CDialogEx)
	ON_MESSAGE(WM_USER_INVOKE, &CDlgInvokable::OnBeginInvoke)//?beginInvoke 2
END_MESSAGE_MAP()

#ifdef _DEBUG
LRESULT CDlgInvokable::OnBeginInvoke(WPARAM wParam, LPARAM lParam)
{
	CSyncAutoLock __lock(KBeginInvoke::GetCS(), TRUE, __FUNCTION__, __LINE__, "CDlgInvokable");
	KBeginInvoke* pbi = (KBeginInvoke*)lParam;
	pbi->_bCalled = true; (*pbi->m_pLambda)();
	TRACE("### NOT CALLED YET, %ld, %s (%d)\n", pbi->_srl, pbi->m_fnc, pbi->m_line);
	KBeginInvoke::freeInvokeFree();
	return 0;
}
#else
OnBeginInvoke_Define(CDlgInvokable)//?beginInvoke 3
#endif // _DEBUG


/// ////////////////////////////////////////////////////////////////////////////////////////
/// ////////////////////////////////////////////////////////////////////////////////////////
/// ////////////////////////////////////////////////////////////////////////////////////////
/// ////////////////////////////////////////////////////////////////////////////////////////


PWS KwOpenFileOpenDlg(HWND hwnd, CStringW& fname, PWS filter)
{
	OPENFILENAME ofn;       // common dialog box structure
	TCHAR szFile[260];       // buffer for file name
//	HANDLE hf;              // file handle

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"All\0*.*\0Excel(*.xlsx)\0*.xlsx\0Excel2003(*.xls)\0*.xls\0";
	if(filter)
		ofn.lpstrFilter = filter;
	//	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if(GetOpenFileName(&ofn))
	{
		// 		hf = CreateFile(ofn.lpstrFile, 
		// 			GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING,
		// 			FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
		fname = ofn.lpstrFile;
		return fname;
	}
	return NULL;
}



int KwMessageBox(LPCWSTR lpFormat, ...)
{
	CStringW buf;
	CSTRING_SPRINTF(lpFormat, buf);
	auto wnd = AfxGetApp()->GetMainWnd();// ::GetDesktopWindow();
	int rv = ::MessageBox(wnd->GetSafeHwnd(), buf, L"Information", MB_OK | MB_ICONINFORMATION);
	//int rv = AfxMessageBox(buf, MB_OK|MB_ICONEXCLAMATION);
	return  rv;
}
int KwMessageBoxA(LPCSTR lpFormat, ...)
{
	CStringA buf;
	CSTRINGA_SPRINTF(lpFormat, buf);
	CStringW bufw(buf);
	auto wnd = AfxGetApp()->GetMainWnd();// ::GetDesktopWindow(); //MB_ICONEXCLAMATION
	int rv = ::MessageBox(wnd->GetSafeHwnd(), (PWS)bufw, L"Information", MB_OK | MB_ICONINFORMATION);
	return  rv;
}
int KwMessageBoxError(LPCTSTR lpFormat, ...)
{
	CString buf;
	CSTRING_SPRINTF(lpFormat, buf);
	auto wnd = AfxGetApp()->GetMainWnd();// ::GetDesktopWindow();
	int rv = ::MessageBox(wnd->GetSafeHwnd(), buf, L"Error", MB_OK | MB_ICONERROR);
	//int rv = AfxMessageBox(buf, MB_OK|MB_ICONERROR);
	return  rv;
}


long KwGetCtrlRect(CWnd* pParent, int idc, LPRECT lpRect)
{
	return KwGetCtrlRect(pParent, pParent->GetDlgItem(idc), lpRect);
}
long KwGetCtrlRect(CWnd* pParent, CWnd* pCtrl, LPRECT lpRect)
{
	if(pParent == NULL)
		return -1;
	if(pCtrl == NULL) // 이게 release 에서 디버그없이 실행 
		return -2;
	pCtrl->GetWindowRect(lpRect);

	ASSERT(pCtrl->GetParent() == pParent);

	pParent->ScreenToClient(lpRect);
	long lStyleOld = GetWindowLong(pParent->GetSafeHwnd(), GWL_STYLE);
	return lStyleOld;
}

BOOL KwIsEnableWindow(CWnd* pw, int idc)
{
	if(pw->GetSafeHwnd() != NULL)
	{
		if(::IsWindow(pw->GetSafeHwnd()))
		{
			CWnd* pctr = pw->GetDlgItem(idc);
			if(::IsWindow(pw->GetSafeHwnd()))
				return ::IsWindowEnabled(pctr->GetSafeHwnd());
		}
	}
	return FALSE;
}

void KwEnableWindow(CWnd* pw, int idc, BOOL bEnable)
{
	if(pw->GetSafeHwnd() != NULL)
	{
		if(::IsWindow(pw->GetSafeHwnd()))
		{
			CWnd* ctrl = pw->GetDlgItem(idc);
			if(ctrl && ::IsWindow(ctrl->GetSafeHwnd()))
				ctrl->EnableWindow(bEnable);
		}
	}
}

void KwEnableWindow(CWnd* pw, int *idc, BOOL* bEnable, int cnt)
{
	if(pw->GetSafeHwnd() != NULL)
	{
		if(::IsWindow(pw->GetSafeHwnd()))
		{
			for(int i=0;i<cnt;i++) {
				CWnd* ctrl = pw->GetDlgItem(idc[i]);
				if(ctrl && ::IsWindow(ctrl->GetSafeHwnd()))
					ctrl->EnableWindow(bEnable[i]);
			}
		}
	}
}

/// <summary>
/// ScrollView의 스크롤을 감안한 뷰안에서의 위치 좌표를 구한다.
/// </summary>
/// <param name="view"></param>
/// <param name="pt">client pos, -1 이어야 직접 감지 한다.</param>
void KwGetViewScrolledPos(CScrollView* view, CPoint& pt)
{
	CPoint point(0, 0);
	if(pt.x == -1)
	{
		GetCursorPos(&point);// -1 이 왔을때는 현재 스크린 위치를
		view->ScreenToClient(&point);// 클라이언트 위치로 바꿔서
		pt.x = point.x;
		pt.y = point.y;
	}
	SIZE sc{ view->GetScrollPos(SB_HORZ), view->GetScrollPos(SB_VERT) };// 
	pt += sc; // 스크롤 된만큼 더한다.
	//pt.x += sc.cx; // 스크롤 된만큼 더한다.
	//pt.y += sc.cy;
}
//void KwSetListReportStyle(CListCtrl* pList, DWORD dwStyle = LVS_SHOWSELALWAYS | LVS_SINGLESEL, DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
void KwSetListReportStyle(CListCtrl* pList, DWORD dwStyle, DWORD dwExStyle)
{
	long lStyleOld = GetWindowLong(pList->GetSafeHwnd(), GWL_STYLE);//, bEx ? GWL_EXSTYLE : 
	lStyleOld |= dwStyle;
	SetWindowLong(pList->GetSafeHwnd(), GWL_STYLE, lStyleOld);

	DWORD dw1 = pList->GetExtendedStyle();// | LVS_EX_FULLROWSELECT;
	pList->SetExtendedStyle(dw1 | dwExStyle);
	// 	KwSetWindowStyle(pList->GetSafeHwnd(), LVS_SHOWSELALWAYS);
	// 	KwSetWindowStyle(m_cList1.GetSafeHwnd(), LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES, TRUE, true);
	// 	DWORD dw1 = m_cList1.GetExtendedStyle() | LVS_EX_FULLROWSELECT;
	// 	m_cList1.SetExtendedStyle(dw1);
}
// _STitleWidth2 s_arlst1[] =
// {
// 	{  20, eInvalid, L"#",         },
// 	{  80, eInvalid, L"이름",      },
// 	{ 100, eInvalid, L"휴대폰",    },
// 	{ 100, eInvalid, L"회사전화1", },
// 	{ 100, eInvalid, L"전자우편1", },
// 	{ 100, eInvalid, L"집 전화1",  },
// };
void KwSetListColumn(CListCtrl* pList, _STitleWidthField* parLst, int count)
{
	int nColumnCount = pList->GetHeaderCtrl()->GetItemCount();
	for(int i = 0; i < nColumnCount; i++)
		pList->DeleteColumn(0);

	CHeaderCtrl* phd1 = pList->GetHeaderCtrl();
	for_each0(count)
	{
		pList->InsertColumn(_i, parLst[_i].title, LVCFMT_LEFT);
		pList->SetColumnWidth(_i, parLst[_i].width);

		HDITEM oHeaderItem;
		KwZeroMemory(oHeaderItem);
		oHeaderItem.mask = HDI_LPARAM;
		//yphd1->GetItem(_i, &oHeaderItem);
		oHeaderItem.lParam = (LPARAM)parLst[_i].field;
		VERIFY(phd1->SetItem(_i, &oHeaderItem));
	}
}



int KwSelectListItemEx(CListCtrl* pl, int curSel, bool bShow)
{
	if(curSel < 0)
		curSel = 0;

	int last = pl->GetItemCount() - 1;
	if(last < curSel)
		curSel = last;

	if(curSel >= 0)
		KwSelectListItem(pl, curSel, bShow);

	return curSel;
}

void KwSelectListItem(CListCtrl* pl, int iItem, bool bShow, int iOp)
{
	if(pl->GetItemCount() > iItem)
	{
		if(KwAttr(iOp, eLc_Clear))
			KwClearSelectedListItem(pl);
		if(KwAttr(iOp, eLc_Unselect))
		{
			if(bShow)
				pl->SetItemState(iItem, 0, LVIS_SELECTED | LVIS_FOCUSED);//LVIF_STATE);
			else
				pl->SetItemState(iItem, 0, LVIS_SELECTED);//LVIF_STATE);
		}
		else
		{
			if(bShow)
				pl->SetItemState(iItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);//LVIF_STATE);
			else
				pl->SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);//LVIF_STATE);
		}
		pl->EnsureVisible(iItem, FALSE);
	}
}
void KwFocusListItem(CListCtrl* pl, int iItem)
{
	if(pl->GetItemCount() > iItem)
	{
		pl->SetItemState(iItem, LVIS_FOCUSED, LVIS_FOCUSED);//LVIF_STATE);
		pl->EnsureVisible(iItem, FALSE);
	}
}

int KwClearSelectedListItem(CListCtrl* pl)
{
	UINT i, uSelectedCount = pl->GetSelectedCount();
	int  nItem = -1;

	if(uSelectedCount > 0)
	{
		for(i = 0; i < uSelectedCount; i++)
		{
			pl->SetItemState(nItem, 0, LVIS_SELECTED | LVIS_FOCUSED);//LVIF_STATE);
			nItem = pl->GetNextItem(nItem, LVNI_SELECTED | LVNI_FOCUSED);

			if(nItem >= 0)
				return nItem;
		}
	}
	return -1;
}

int KwGetSelectedListItem(CListCtrl* pl)
{
	UINT i, uSelectedCount = pl->GetSelectedCount();
	int  nItem = -1;

	if(uSelectedCount > 0)
	{
		for(i = 0; i < uSelectedCount; i++)
		{
			nItem = pl->GetNextItem(nItem, LVNI_SELECTED | LVNI_FOCUSED);

			if(nItem >= 0)
				return nItem;
		}
	}
	return -1;
}
int KwGetSelectedListItem(CListCtrl* pl, CDWordArray& ar)
{
	UINT i;//, uSelectedCount = pl->GetSelectedCount();
	int  nItem = -1;

	ar.RemoveAll();
	//if(uSelectedCount > 0)
	{
		for(i = 0;; i++)//i < uSelectedCount
		{
			nItem = pl->GetNextItem(nItem, LVNI_SELECTED);
			if(nItem >= 0)
				ar.Add(nItem);
			else
				break;
		}
	}
	return (int)ar.GetCount();
}

int KwGetSelectedCount(CListCtrl* pl)
{
	int  nItem = 0;
	int  i = -1;
	for(;;)
	{
		i = pl->GetNextItem(i, LVNI_SELECTED);
		if(i >= 0)
			nItem++;
		else
			break;
	}
	return nItem;
}

void KLambdaTimer::SetLambdaTimerImple(PAS sid, UINT elapsed, shared_ptr<function<void(int, PAS)>> lmda, int maxCount /*= 0*/)
{
	if(_mapTmObj.Has(sid))
	{
		auto t2 = GetTickCount();
		auto tobj = _mapTmObj[sid];
		_wnd->KillTimer(tobj->_idTimer);
		tobj->_tickStart = GetTickCount();
		_wnd->SetTimer(tobj->_idTimer, tobj->_elapsed, NULL);
		//TRACE("Lambda Timer: %s %d msec restarted~\n", sid, (int)(t2 - tobj->_tickStart));
		tobj->_tickStart = t2;
		return;
	}
	_idTm++;

	_mapTmID[sid] = _idTm;
	_mapRTmID[_idTm] = sid;
	auto tobj = new KTimerObj;
	_mapTmObj[sid] = tobj;
	tobj->_maxCount = maxCount;
	tobj->_idTimer = _idTm;
	tobj->_elapsed = elapsed;
	_mapTimer[_idTm] = lmda; // make_shared < function<void(int, PAS)>>(lmda);

	tobj->_tickStart = GetTickCount();
	_wnd->SetTimer(_idTm, elapsed, NULL);
	//TRACE("Lambda Timer: %s SetTimer <<\n", sid);
}

void KLambdaTimer::KillLambdaTimer(PAS sid, bool bKill /*= true*/)
{
	UINT_PTR idTm = _mapTmID[sid];
	if(bKill)
		_wnd->KillTimer(idTm);
	//TRACE("Lambda Timer: %s KillTime >>\n", sid);

	_mapTmObj.DeleteKey(sid);
	_mapTmID.erase(sid);
	_mapTimer.erase(idTm);
	_mapRTmID.erase(idTm);
}

void KLambdaTimer::DoTimerTask(UINT_PTR nIDEvent)
{
	if(!_mapTimer.Has(nIDEvent))
		return;

	auto sid = _mapRTmID[nIDEvent];
	auto tobj = _mapTmObj[sid];
	ASSERT(tobj);
	if(tobj)
	{
		BOOL bLast = tobj->_maxCount > 0 && (tobj->_i + 1) >= tobj->_maxCount;
		if(bLast)
			_wnd->KillTimer(nIDEvent);

		auto t2 = GetTickCount();
		//TRACE("Lambda Timer: %s(%d) %d msec Task Done!\n", sid.c_str(), tobj->_i, (int)(t2 - tobj->_tickStart));
		tobj->_tickStart = t2;
		auto shFnc = _mapTimer[nIDEvent];
		int i = tobj->_i;
		(*shFnc)(i, sid.c_str());
		if(bLast)
			KillLambdaTimer(sid.c_str(), false);/// GetTmObj 로 람다 안에서 부를 수 있는데, 여기서 해제 해버리면 얻으게
		else
			tobj->_i++;
		/// 만약 타이머 안에서 팝업이 뜨면 여기서 계속 기다린다. Kill 하고 맨 나중에 한다.
	}
}
