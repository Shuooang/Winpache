#pragma once

#include "ktypedef.h"
#include "stdecl.h"
#include "Lock.h"
#include "Kw_tool.h"
#include <functional>
#include "afxdialogex.h"

#include "afxmdiframewndex.h"

extern CKTrace std_trace;

#ifdef _Use_Sample
void CMFCAppServerEx1Doc::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		CJsonPbj js;
		//js["xxx"] = xxx;
		JSPUT(_port);
		JSPUT(_path);
		JSPUT(_vpath);
		JSPUT(_certificate);
		JSPUT(_privatekey);
		JSPUT(_prvpwd);
		JSPUT(_dhparam);
		CFile* fr = ar.GetFile();

		CStringA sUtf8 = js.ToJsonStringUtf8();
		fr->Write((PAS)sUtf8, sUtf8.GetLength());
	}
	else
	{
		CFile* fr = ar.GetFile();
		ULONGLONG len = fr->GetLength();
		CStringA sa;
		char* buf = sa.GetBufferSetLength(len);
		fr->Read(buf, len);
		CString sWstr;
		KwUTF8ToWchar(buf, sWstr);

		JSONValue* jdoc = JSON::Parse((PWS)sWstr);
		KAtEnd d_jdoc([&]() {
			delete jdoc; });
		CJsonPbj js(jdoc->AsObject());

		JSGETN(_port);// ������ ���
		JSGETS(_path);
		JSGETS(_vpath);
		JSGETS(_certificate);
		JSGETS(_privatekey);
		JSGETS(_dhparam);
		JSGETS(_prvpwd);
	}
}
#endif


/// //////////////////////////////////////////////////////////////////////////
// PostMessage�� �̿��� �񵿱� ȣ�� ���
// .h ��
///	afx_msg LRESULT OnBeginInvoke(WPARAM wParam, LPARAM lParam);//?beginInvoke 1
//	.cpp ��
// BEGIN_MESSAGE_MAP(CMFCKwAppDlg1Dlg, CDialogEx)
/// 	ON_MESSAGE(WM_USER_INVOKE, &CMFCKwAppDlg1Dlg::OnBeginInvoke)//?beginInvoke 2
// END_MESSAGE_MAP()
// 
///	OnBeginInvoke_Define(CMFCKwAppDlg1Dlg)//?beginInvoke 3
//
// void CMFCKwAppDlg1Dlg::OnBnClickedButton1()
// {
// 	int iv = m_iv;//����� �Ѱ� �ٷ��� ��������� �޾ƾ� �Ѵ�. ��������� [m_iv]���޵��� �ʴ´�.
/// 	KwBeginInvoke(this, ([&, iv]()-> void //?beginInvoke 4
// 	{	this->SomeFunc(iv); // iv < m_iv
// 	}));
// } // lambda�κ��� �� ()�� �δ� ������ ��ũ�� �̹Ƿ� �׷���. _KwBeginInvoke �� ���� ����� ()�� �Ƚε� �ȴ�.
#ifdef _Use_Sample_
afx_msg LRESULT OnBeginInvoke(WPARAM wParam, LPARAM lParam);//?beginInvoke 1
ON_MESSAGE(WM_USER_INVOKE, &CMFCKwAppDlg1Dlg::OnBeginInvoke)//?beginInvoke 2
OnBeginInvoke_Define(CMFCKwAppDlg1Dlg)//?beginInvoke 3
KwBeginInvoke(this, [&, iv]()-> void {	this->SomeFunc(iv); });//?beginInvoke 4.1
//UpdateData()�� ��׶��忡�� �θ��� �׳� �״´�. ���ν����忡���� �θ��� �ִ� UI���� �Լ���.
// ���������� 
KwBeginInvoke(this, [&]()-> void {	this->UpdateData(0);	});//?beginInvoke 4.2

#endif // _Use_Sample_

/// 2020-10-12
#define KDDX_Text(field)  DDX_Text( pDX, IDC##field, doc->field)

#define KDDX_CBBox(field) DDX_CBString(pDX, IDC##field, doc->field)
#define KDDX_Radio(field) DDX_Radio(pDX, IDC##field, doc->field)
#define KDDX_Check(field) DDX_Check(pDX, IDC##field, doc->field)
//DDX_Check(CDataExchange* pDX, int nIDC, int& value);

#define KDDX_TextA(field)  { if(pDX->m_bSaveAndValidate) { CString v; \
    DDX_Text(pDX, IDC##field, v); doc->field = CStringA(v);} else { \
    CStringW v(doc->field);\
    DDX_Text(pDX, IDC##field, v); }}

#define KDDX_CBBoxA(field) { if(pDX->m_bSaveAndValidate) {\
    CString v; DDX_CBString(pDX, IDC##field, v); doc->field = CStringA(v);} else { \
    CStringW v(doc->field);\
    DDX_CBString(pDX, IDC##field, v); }}



#define WM_USER_INVOKE (WM_USER+0x0100)

class KBeginInvoke
{
public:
	//HWND m_hwnd; , m_hwnd(hwnd)
	std::function<void()>* m_pLambda;
	bool _bCalled{ false };
	WPARAM _srl{ 0 };
	CStringA _note;
	CStringA m_fnc;
	int m_line;

	KBeginInvoke(std::function<void()>* pLambda, PAS fnc, int line)
		: m_pLambda(pLambda)
		, m_fnc(fnc)
		, m_line(line)
	{
	}
	~KBeginInvoke()
	{
		delete m_pLambda;
	}

	/// BeginInvoke�� ���ÿ� ���� �����忡�� �Ǵ°� gabage free�� ���� �ϴ�.
	static CKCriticalSection* GetCS()
	{
		static CKCriticalSection _csInvoke;//Ʈ����� �ִ� �Լ� ����Ǫ �ҷ� ���� Ʈ������� ��ø �ȴ�.
		return &_csInvoke;
	}
	///?����
	/// Invoke�� �Ҵ��� �� Post �޾Ƽ� ����  ���� ������, gabage�� ���� �ϴ� ����� ����.
	/// static auto free array�� ����.
	static void setInvokeFree(KBeginInvoke* pbi);
	static void freeInvokeFree();
	static KPtrList<KBeginInvoke>* getGabage()
	{
		static KPtrList<KBeginInvoke> s_list;
		return &s_list;
	}

	// 	template<typename Func>
	// 	static void Begin(CWnd* pWnd, Func lmda)
	// 	{
	// 		// lmda�� ���� ���� ���� �̹Ƿ� ���� ���� �ؼ� ���� �ؾ� �Ѵ�. �񵿱� �̹Ƿ� ���ÿ��� ��� ����.
	// 		KBeginInvoke* pbi = new KBeginInvoke()
	// 			std::function<void()> *pLambda = new std::function<void()>(lmda);
	// 		::PostMessage(pWnd->GetSafeHwnd(), WM_USER_INVOKE, 0, (LPARAM)pLambda);
	// 	}
};


#define OnBeginInvoke_Define(clss) \
LRESULT clss::OnBeginInvoke(WPARAM wParam, LPARAM lParam)\
{	CSyncAutoLock __lock(KBeginInvoke::GetCS(), TRUE, __FUNCTION__, __LINE__, #clss);\
	KBeginInvoke* pbi = (KBeginInvoke*)lParam;\
	(*pbi->m_pLambda)(); pbi->_bCalled = true; KBeginInvoke::freeInvokeFree(); \
	return 0;\
}
//auto pLambda = (std::function<void()>*)lParam; delete pbi;

template<typename Func>
void _KwBeginInvoke(HWND hw, Func lmda, PAS fnc = NULL, int line = -1, LPCSTR note = NULL)
{
	// lmda�� ���� ���� ���� �̹Ƿ� ���� ���� �ؼ� ���� �ؾ� �Ѵ�. �񵿱� �̹Ƿ� ���ÿ��� ��� ����.
	std::function<void()>* pLambda = 
#ifdef _DEBUG
		DEBUG_NEW
#else
		new
#endif // _DEBUG
		std::function<void()>(lmda);

	KBeginInvoke* pbi = 
#ifdef _DEBUG
		DEBUG_NEW
#else
		new
#endif // _DEBUG
		KBeginInvoke(pLambda, fnc, line);
	static WPARAM s_w = 0;
	s_w++;
	pbi->_srl = s_w;
	pbi->_note = note;
	KBeginInvoke::setInvokeFree(pbi);
	::PostMessage(hw, WM_USER_INVOKE, s_w, (LPARAM)pbi);
}
//_KwBeginInvoke(_wnd, [&, param]()-> void
//	{
//		OnBoxSelected(param);
//	}, __FUNCTION__, __LINE__);

template<typename Func>
void _KwSendInvoke(HWND hw, Func lmda, PAS fnc = NULL, int line = -1, LPCSTR note = NULL)
{
	std::function<void()>* pLambda = 
#ifdef _DEBUG
		DEBUG_NEW
#else
		new
#endif // _DEBUG
		std::function<void()>(lmda);
	KBeginInvoke* pbi = 
#ifdef _DEBUG
		DEBUG_NEW
#else
		new
#endif // _DEBUG
		KBeginInvoke(pLambda, fnc, line);
	pbi->_note = note;
	KBeginInvoke::setInvokeFree(pbi);
	::SendMessage(hw, WM_USER_INVOKE, 0, (LPARAM)pbi);
}

template<typename Func>
void _KwBeginInvoke(CWnd* pWnd, Func lmda, PAS fnc = NULL, int line = -1, LPCSTR note = NULL)
{
	_KwBeginInvoke(pWnd->GetSafeHwnd(), lmda, fnc, line, note);
}
template<typename Func>
void _KwSendInvoke(CWnd* pWnd, Func lmda, PAS fnc = NULL, int line = -1, LPCSTR note = NULL)
{
	_KwSendInvoke(pWnd->GetSafeHwnd(), lmda, fnc, line, note);
}


//?����: ���ٽĿ��� [&] �� ��������, [&,val] ó�� �� ��� ���� ��� ���ٽ� ��ü�� ���η� �� ���� ��� �Ѵ�.
#define KwBeginInvoke(wnd, lmda) _KwBeginInvoke((wnd), (lmda), __FUNCTION__, __LINE__)
#define KwSendInvoke(wnd, lmda) _KwSendInvoke((wnd), (lmda), __FUNCTION__, __LINE__)
#define KwBeginInvokeNt(wnd, lmda, note) _KwBeginInvoke((wnd), (lmda), __FUNCTION__, __LINE__, note)
#define KwSendInvokeNt(wnd, lmda, note) _KwSendInvoke((wnd), (lmda), __FUNCTION__, __LINE__, note)
/*
* this: message���� ������ CWnd*
* i,sAstrR : �񵿱�� ���ÿ��� ����� ������ ������ ���� �Ѵ�.
	KwBeginInvoke(this, ([&, i, sAstrR]()-> void
		{
			UiForAsync(i, sAstrR);
		}));//?beginInvoke 4

*/


PWS KwOpenFileOpenDlg(HWND hwnd, CStringW& fname, PWS filter = NULL);


/////////////////////////////////////////////////////////////////////
//  AfxMessageBox("abcdefgh"); ������ �޽��� �ڽ����� ���� ���� �Ҷ�..
int KwMessageBox(LPCWSTR lpFormat, ...);
int KwMessageBoxA(LPCSTR lpFormat, ...);
int KwMessageBoxError(LPCTSTR lpFormat, ...);






/**************** Lambda sample *******
* 	auto lt = dynamic_cast<KLambdaTimer*>(this);
	lt->_wnd = this;

void DockOrder::SetMyTimer() //?LbTimer ex
{
	SetLambdaTimer("test", 1000, [&](int ntm, PAS tmk)
		{
			KTrace(L"%d. %s (%s)\n", ntm, __FUNCTIONW__, L"Lambda timer test");
		});
}
*/

class KTimerObj
{
public:
	UINT_PTR _idTimer{ 0 };
	UINT _elapsed{ 1000 };
	int _i{ 0 };// �ݺ��� ���� 1�� ����. _maxCount -1 �ϰ� �����.
	int _maxCount{ 0 };//0�̸� ������
	LPARAM _param{ NULL };
	DWORD _tickStart{ 0 };
};
class KLambdaTimer
{
public:
	KLambdaTimer(CWnd* wnd = nullptr)
		: _wnd(wnd)
	{
	}
	~KLambdaTimer()
	{
		_mapTmObj.DeleteAll();
	}
	CWnd* _wnd{ nullptr };
	KStdMap < UINT_PTR, shared_ptr<function<void(int, PAS)>>> _mapTimer;
	KStdMap <string, UINT_PTR> _mapTmID;
	//KStdMap <string, int> _mapTmConter;
	//KStdMap <string, int> _mapTmMaxCount;
	KStdMap <UINT_PTR, string> _mapRTmID;
	KStdMapPtr<string, KTimerObj> _mapTmObj;
	int _idTm{ 9470 };


	template<typename TFNC>
	void SetLambdaTimer(PAS sid, UINT elapsed, TFNC lmda, int maxCount = 0)
	{
		auto fnc = make_shared < function<void(int, PAS)>>(lmda);
		SetLambdaTimerImple(sid, elapsed, fnc, maxCount);
	}
	
	void SetLambdaTimerImple(PAS sid, UINT elapsed, shared_ptr<function<void(int, PAS)>> lmda, int maxCount = 0);

	void KillLambdaTimer(PAS sid, bool bKill = true);

	void DoTimerTask(UINT_PTR nIDEvent);

	KTimerObj* GetTmObj(PAS sid)
	{
		auto tobj = _mapTmObj[sid];
		return tobj;
	}

};


/// ��׶��� �ݹ��� �񵿱�� �Ҹ����� 
///		â�� ������ �𸣰�, â ����� �����ϴ� �״°� ���� �ϱ� ����,
///		â�� ID�� ���(ViewRegister) �ߴٰ�, ��� �ִ��� Ȯ��(ViewFind)�� �� �ִ�.
/// ����, OnClose������ �ݵ�� ����(ViewRemove)�ؾ� �Ѵ�.
class KCheckWnd
{
public:
	/// ���� �����尡 �̿� �ϹǷ�
	CKCriticalSection _cs_mapVu;
	KStdMap<int, CWnd*> _mapWnd;
	/// ex: ��뿹
#ifdef _Sample
	auto* ivc = dynamic_cast<IVuCount*>(AfxGetApp());
	_id = ivc->ViewRegister(this);
#endif // _Sample

	int ViewRegister(CWnd* wnd)
	{
		AUTOLOCK(_cs_mapVu);
		static int s_ID = 0;
		++s_ID;
		_mapWnd[s_ID] = wnd;
		return s_ID;
	}
	bool ViewFind(int id)
	{
		AUTOLOCK(_cs_mapVu);
		return _mapWnd.find(id) != _mapWnd.end();
	}
	void ViewRemove(int id)
	{
		if(!ViewFind(id))
			return;
		AUTOLOCK(_cs_mapVu);
		_mapWnd.erase(id);
	}
	size_t ViewCount()
	{
		AUTOLOCK(_cs_mapVu);
		return (size_t)_mapWnd.size();
	}
};


class CDlgInvokable : public CDialogEx
{
public: // serialization������ ��������ϴ�.
	//CFormInvokable() noexcept;
	CDlgInvokable()
		: CDialogEx()
	{}
		CDlgInvokable(UINT nIDTemplate, CWnd* pParent = NULL)
		: CDialogEx(nIDTemplate, pParent)
	{}
	CDlgInvokable(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL)
		: CDialogEx(lpszTemplateName, pParentWnd)
	{}

	DECLARE_DYNAMIC(CDlgInvokable)

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnBeginInvoke(WPARAM wParam, LPARAM lParam);//?beginInvoke 1

};


class CMDIFrameWndInvokable : public CMDIFrameWnd
	, public KLambdaTimer//?LbTimer 1
{
	DECLARE_DYNAMIC(CMDIFrameWndInvokable)
public:
	CMDIFrameWndInvokable() noexcept;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnBeginInvoke(WPARAM wParam, LPARAM lParam);//?beginInvoke 1
	afx_msg void OnTimer(UINT_PTR nIDEvent);//?LbTimer 3
};

class CMDIFrameWndExInvokable : public CMDIFrameWndEx
	, public KLambdaTimer//?LbTimer 1
{
	DECLARE_DYNAMIC(CMDIFrameWndExInvokable)
public:
	CMDIFrameWndExInvokable() noexcept;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnBeginInvoke(WPARAM wParam, LPARAM lParam);//?beginInvoke 1
	afx_msg void OnTimer(UINT_PTR nIDEvent);//?LbTimer 3
};



class CFormInvokable 
	: public CFormView
	, public KLambdaTimer//?LbTimer 1
{
protected: // serialization������ ��������ϴ�.
	//CFormInvokable() noexcept;
	CFormInvokable(UINT nIDTemplate);
	DECLARE_DYNAMIC(CFormInvokable)

public:
	/// <summary>
	/// OnSize���� �ø��� ���� �þ� ���°� �ҋ� ���� ũ�� ���� �صδ� ��
	/// </summary>
	CSize m_sz;

	// Ư���Դϴ�.
public:
	// �۾��Դϴ�.
public:
	virtual CDocument* GetDoc()
	{
		return NULL;
	}
	// �������Դϴ�.
public:
protected:
	//�ʿ� �ϸ� �ְ�, �ڽ��� �Ϳ��� �ƹ����� ����� ��Ī �ؾ�
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	//virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

	// �����Դϴ�.
public:
	virtual ~CFormInvokable();
#ifdef _DEBUG
	//virtual void AssertValid() const;
	//virtual void Dump(CDumpContext& dc) const;
#endif

	BOOL OnSizeDefault(UINT nType, int cx, int cy, int nCtrl, int arIdc[]);

protected:

	// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnBeginInvoke(WPARAM wParam, LPARAM lParam);//?beginInvoke 1
	afx_msg void OnTimer(UINT_PTR nIDEvent);//?LbTimer 3

};

/* CFormInvokable ��� �ϴ� �� 4����
* CFormView �� CFormInvokable �θ� �ٲٸ� ��.
* ������� 1
class CPostTestView : public CFormInvokable
* cpp���� 2
IMPLEMENT_DYNCREATE(CPostTestView, CFormInvokable)
* cpp���� 3
BEGIN_MESSAGE_MAP(CPostTestView, CFormInvokable)
* cpp���� 4
CPostTestView::CPostTestView() noexcept
	: CFormInvokable(IDD_MFCAPPPOSTTEST1_FORM) //�Ҿƹ��� ��ü ���� �ٷ� �θ��� ��� �ƹ����� �θ�.

*/

long KwGetCtrlRect(CWnd* pParent, CWnd* pCtrl, LPRECT lpRect);
long KwGetCtrlRect(CWnd* pParent, int idc, LPRECT lpRect);

BOOL KwIsEnableWindow(CWnd* pw, int idc);

void KwEnableWindow(CWnd* pw, int idc, BOOL bEnable = TRUE);

void KwEnableWindow(CWnd* pw, int* idc, BOOL* bEnable, int cnt);



class CEditCtrlCV : public CEdit
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg)
		//BOOL CEditCtrlCV::PreTranslateMessage(MSG* pMsg)
	{
		switch (pMsg->message)
		{
		case WM_KEYDOWN:
		{
			short ctrl = ::GetKeyState(VK_CONTROL);
			if (ctrl < 0)
			{
				switch (pMsg->wParam)
				{
				case 'Z':this->Undo();
					return TRUE;
				case 'X':this->Cut();
					return TRUE;
				case 'C':
				{
					int sc = -1, ec = -1;
					this->GetSel(sc, ec);
					if (sc == -1 && ec == -1)
						this->SetSel(0, -1);
					this->Copy();
					return TRUE;
				}
				case 'V':this->Paste();
					return TRUE;
				case 'A':this->SetSel(0, -1);
					return TRUE;
				}
			}
		}
		}
		return __super::PreTranslateMessage(pMsg);
	}
};

void KwGetViewScrolledPos(CScrollView* view, CPoint& pt);
void KwSetListReportStyle(CListCtrl* pList, DWORD dwStyle = LVS_SHOWSELALWAYS | LVS_SINGLESEL, DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

struct _STitleWidthField
{
	int width;
	PAS field;
	PWS title; // �ڿ� comment ���� ���°� ���� ���� �ڷ� ����
	LPARAM lp;
};
void KwSetListColumn(CListCtrl* pList, _STitleWidthField* parLst, int count);


enum ELC_
{
	eLc_None,
	eLc_Clear,
	eLc_Unselect,
};
int KwSelectListItemEx(CListCtrl* pl, int curSel, bool bShow = true);
void KwSelectListItem(CListCtrl& pl, int iItem, bool bShow = true);
void KwSelectListItem(CListCtrl* pl, int iItem, bool bShow = true, int iOp = eLc_None);
void KwFocusListItem(CListCtrl* pl, int iItem);
int KwGetSelectedListItem(CListCtrl* pl);
int KwGetSelectedListItem(CListCtrl* pl, CDWordArray& ar);
int KwGetSelectedCount(CListCtrl* pl);
int KwClearSelectedListItem(CListCtrl* pl);

