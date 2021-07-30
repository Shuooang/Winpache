#pragma once

#include "afxdialogex.h"

#include "afxmdiframewndex.h"
#include "afxdockablepane.h"

#include <functional>
#include "ktypedef.h"
#include "stdecl.h"
#include "Lock.h"
#include "Kw_tool.h"

// C:\Program Files(x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30037\
// atlmfc\include\afxdockablepane.h
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

		JSGETN(_port);// 숫자인 경우
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
// PostMessage를 이용한 비동기 호출 방법
// .h 에
///	afx_msg LRESULT OnBeginInvoke(WPARAM wParam, LPARAM lParam);//?beginInvoke 1
//	.cpp 에
// BEGIN_MESSAGE_MAP(CMFCKwAppDlg1Dlg, CDialogEx)
/// 	ON_MESSAGE(WM_USER_INVOKE, &CMFCKwAppDlg1Dlg::OnBeginInvoke)//?beginInvoke 2
// END_MESSAGE_MAP()
// 
///	OnBeginInvoke_Define(CMFCKwAppDlg1Dlg)//?beginInvoke 3
//
// void CMFCKwAppDlg1Dlg::OnBnClickedButton1()
// {
// 	int iv = m_iv;//복사로 넘겨 줄려면 멤버변수를 받아야 한다. 멤버변수는 [m_iv]전달되지 않는다.
/// 	KwBeginInvoke(this, ([&, iv]()-> void //?beginInvoke 4
// 	{	this->SomeFunc(iv); // iv < m_iv
// 	}));
// } // lambda부분을 또 ()로 싸는 이유는 매크로 이므로 그렇다. _KwBeginInvoke 를 직접 쓸경우 ()로 안싸도 된다.
#ifdef _Use_Sample_
afx_msg LRESULT OnBeginInvoke(WPARAM wParam, LPARAM lParam);//?beginInvoke 1
ON_MESSAGE(WM_USER_INVOKE, &CMFCKwAppDlg1Dlg::OnBeginInvoke)//?beginInvoke 2
OnBeginInvoke_Define(CMFCKwAppDlg1Dlg)//?beginInvoke 3
KwBeginInvoke(this, [&, iv]()-> void {	this->SomeFunc(iv); });//?beginInvoke 4.1
//UpdateData()는 백그라운드에서 부르면 그냥 죽는다. 메인스레드에서만 부를수 있는 UI전용 함수들.
// 내부적으로 
KwBeginInvoke(this, [&]()-> void {	this->UpdateData(0);	});//?beginInvoke 4.2

#endif // _Use_Sample_

/// 2020-10-12
/// auto* doc = 뭔가 있어야 한다.
// #define KDDX_Text(field)  DDX_Text( pDX, IDC##field, doc->field)
// #define KDDX_CBBox(field) DDX_CBString(pDX, IDC##field, doc->field)
// #define KDDX_Radio(field) DDX_Radio(pDX, IDC##field, doc->field)
// #define KDDX_Check(field) DDX_Check(pDX, IDC##field, doc->field)

///DDX_Check(CDataExchange* pDX, int nIDC, int& value);
/// 변수명이 반드시 _value처럼 앞에 '_'가 있어야 한다.
#define KDDX_Text(field)  DDX_Text( pDX, IDC##field, doc->field)
#define KDDX_CBBox(field) DDX_CBString(pDX, IDC##field, doc->field)
#define KDDX_Radio(field) DDX_Radio(pDX, IDC##field, doc->field)
#define KDDX_Check(field) DDX_Check(pDX, IDC##field, doc->field)

#define KDDX_TextA(field)  { if(pDX->m_bSaveAndValidate) { CString v; \
    DDX_Text(pDX, IDC##field, v); doc->field = CStringA(v);} else { \
    CStringW v(doc->field);\
    DDX_Text(pDX, IDC##field, v); }}

#define KDDX_CBBoxA(field) { if(pDX->m_bSaveAndValidate) { CString v;\
	DDX_CBString(pDX, IDC##field, v); doc->field = CStringA(v);} else { \
    CStringW v(doc->field);\
    DDX_CBString(pDX, IDC##field, v); }}


/// 변수명이 JSON 키 이므로 _value처럼 앞에 '_'가 안 붙을 수 있으므로 IDC_## 이다. IDC## 가 아님.
/// JObj& jbj = 뭔가 있어야 한다.

#define KDDXJ_IntTok(field, tok) {int v;\
	if(!pDX->m_bSaveAndValidate) v = jbj.I(#field); \
	DDX_Text(pDX, IDC##tok##field, v);\
	if(pDX->m_bSaveAndValidate) jbj(#field) = v;}

#define KDDXJ_TextTok(field, tok) {CString v;\
	if(!pDX->m_bSaveAndValidate) v = jbj.S(#field); \
	DDX_Text(pDX, IDC##tok##field, v);\
	if(pDX->m_bSaveAndValidate) jbj(#field) = v;}
//#define KDDXJ_TextATok(field, tok) KDDXJ_TextTok(field, tok)

#define KDDXJ_CBBoxTok(field, tok) {CString v;\
	if(!pDX->m_bSaveAndValidate) v = jbj.S(#field); \
	DDX_CBString(pDX, IDC##tok##field, v);\
	if(pDX->m_bSaveAndValidate) jbj(#field) = v;}

#define KDDXJ_RadioTok(field, tok) {int v; \
	if(!pDX->m_bSaveAndValidate) v = jbj.I(#field); \
	DDX_Radio(pDX, IDC##tok##field, v); jbj(#field) = v;\
	if(pDX->m_bSaveAndValidate) jbj(#field) = v;}
#define KDDXJ_CheckTok(field, tok) {int v; \
	if(!pDX->m_bSaveAndValidate) v = jbj.I(#field); \
	DDX_Check(pDX, IDC##tok##field, v); jbj(#field) = v;\
	if(pDX->m_bSaveAndValidate) jbj(#field) = v;}


#define KDDXJ_Int(field) KDDXJ_IntTok(field, _)
#define KDDXJ_Text(field) KDDXJ_TextTok(field, _)
//#define KDDXJ_TextA(field) KDDXJ_Text(field) // 같다. 아니 JObj내부에 sa로 저장 하지 않는다.
#define KDDXJ_Radio(field) KDDXJ_RadioTok(field, _)
#define KDDXJ_Check(field) KDDXJ_CheckTok(field, _)
#define KDDXJ_CBBox(field) KDDXJ_CBBoxTok(field, _)

#ifdef _DEBUGxxx
#define KDDXJ_Check(field) {int v = -1;\
	if(!pDX->m_bSaveAndValidate) v = jbj.I(#field); \
	DDX_Check(pDX, IDC_##field, v); \
	if(pDX->m_bSaveAndValidate) jbj(#field) = v;}

#define KDDXJ_Int(field) {int v;\
	if(!pDX->m_bSaveAndValidate) v = jbj.I(#field); \
	DDX_Text(pDX, IDC_##field, v);\
	if(pDX->m_bSaveAndValidate) jbj(#field) = v;}

#define KDDXJ_Text(field) {CString v;\
	if(!pDX->m_bSaveAndValidate) v = jbj.S(#field); \
	DDX_Text(pDX, IDC_##field, v);\
	if(pDX->m_bSaveAndValidate) jbj(#field) = v;}
#define KDDXJ_TextA(field) KDDXJ_Text1(field)

#define KDDXJ_Radio(field) {int v; \
	if(!pDX->m_bSaveAndValidate) v = jbj.I(#field); \
	DDX_Radio(pDX, IDC_##field, v); jbj(#field) = v;\
	if(pDX->m_bSaveAndValidate) jbj(#field) = v;}
#define KDDXJ_Check(field) {int v; \
	if(!pDX->m_bSaveAndValidate) v = jbj.I(#field); \
	DDX_Check(pDX, IDC_##field, v); jbj(#field) = v;\
	if(pDX->m_bSaveAndValidate) jbj(#field) = v;}
#endif // _DEBUGxxx


#define WM_USER_INVOKE (WM_USER+0x0100)

/// PostMessage뿐만 아니라 SendMessage에도 쓰인다.
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
	ULONGLONG _tik{0};
	HWND _hwnd{NULL};
	bool _bSend{false};

	KBeginInvoke(std::function<void()>* pLambda, PAS fnc, int line);
	~KBeginInvoke()
	{
		delete m_pLambda;
	}
	static WORD s_srl;
	/// BeginInvoke가 동시에 여러 스레드에서 되는건 gabage free에 위험 하다.
	static CKCriticalSection* GetCS()
	{
		static CKCriticalSection _csInvoke;//트랜잭션 있는 함수 연거푸 불려 질때 트랜잭션이 중첩 된다.
		return &_csInvoke;
	}
	///?변경
	/// Invoke때 할당한 게 Post 받아서 읿을  수도 있으니, gabage를 프리 하는 방법을 쓴다.
	/// static auto free array를 쓴다.
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
	// 		// lmda가 로컬 람다 변수 이므로 힙에 복제 해서 전달 해야 한다. 비동기 이므로 스택에서 사라 진다.
	// 		KBeginInvoke* pbi = new KBeginInvoke()
	// 			std::function<void()> *pLambda = new std::function<void()>(lmda);
	// 		::PostMessage(pWnd->GetSafeHwnd(), WM_USER_INVOKE, 0, (LPARAM)pLambda);
	// 	}
};


#define OnBeginInvoke_Define(clss) \
LRESULT clss::OnBeginInvoke(WPARAM wParam, LPARAM lParam)\
{	CSyncAutoLock __lock(KBeginInvoke::GetCS(), TRUE, __FUNCTION__, __LINE__, #clss);\
	KBeginInvoke* pbi = (KBeginInvoke*)lParam;\
	pbi->_bCalled = true; (*pbi->m_pLambda)(); KBeginInvoke::freeInvokeFree(); \
	return 0;\
}
//auto pLambda = (std::function<void()>*)lParam; delete pbi;

template<typename Func>
void _KwBeginInvoke(HWND hw, Func lmda, PAS fnc = NULL, int line = -1, LPCSTR note = NULL)
{
	// lmda가 로컬 람다 변수 이므로 힙에 복제 해서 전달 해야 한다. 비동기 이므로 스택에서 사라 진다.
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
// 	static WPARAM s_w = 0;
// 	s_w++;
// 	pbi->_srl = s_w; class constructor 에서 한다.
	pbi->_note = note;
	pbi->_hwnd = hw;
	KBeginInvoke::setInvokeFree(pbi);
	::PostMessage(hw, WM_USER_INVOKE, pbi->_srl, (LPARAM)pbi);
}
///?example : 매크로 KwBeginInvoke를 쓸때는 람다 부분을 가로로 한번더 싸 줘야 한다.
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
	pbi->_hwnd = hw;
	pbi->_bSend = true;
	KBeginInvoke::setInvokeFree(pbi);
	::SendMessage(hw, WM_USER_INVOKE, pbi->_srl, (LPARAM)pbi);
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


//?주의: 람다식에서 [&] 는 괜찮은데, [&,val] 처럼 더 들어 가는 경우 람다식 전체를 가로로 더 묶어 줘야 한다.
#define KwBeginInvoke(wnd, lmda) _KwBeginInvoke((wnd), (lmda), __FUNCTION__, __LINE__)
#define KwSendInvoke(wnd, lmda) _KwSendInvoke((wnd), (lmda), __FUNCTION__, __LINE__)
#define KwBeginInvokeNt(wnd, lmda, note) _KwBeginInvoke((wnd), (lmda), __FUNCTION__, __LINE__, note)
#define KwSendInvokeNt(wnd, lmda, note) _KwSendInvoke((wnd), (lmda), __FUNCTION__, __LINE__, note)
/*
* this: message날릴 윈도우 CWnd*
* i,sAstrR : 비동기라 스택에서 사라질 변수를 복사해 전달 한다.
	KwBeginInvoke(this, ([&, i, sAstrR]()-> void
		{
			UiForAsync(i, sAstrR);
		}));//?beginInvoke 4

*/


PWS KwOpenFileOpenDlg(HWND hwnd, CStringW& fname, PWS filter = NULL);


/////////////////////////////////////////////////////////////////////
//  AfxMessageBox("abcdefgh"); 형태의 메시지 박스에서 서식 포함 할때..
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
	int _i{ 0 };// 반복때 마다 1씩 증가. _maxCount -1 하고 멈춘다.
	int _maxCount{ 0 };//0이면 무한정
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


/// 백그라운드 콜백이 비동기로 불릴때는 
///		창이 닫힌지 모르고, 창 멤버에 접근하다 죽는걸 방지 하기 위해,
///		창을 ID로 등록(ViewRegister) 했다가, 살아 있는지 확인(ViewFind)할 수 있다.
/// 따라서, OnClose에서는 반드시 제거(ViewRemove)해야 한다.
class KCheckWnd
{
public:
	/// 여러 스레드가 이용 하므로
	CKCriticalSection _cs_mapVu;
	KStdMap<int, CWnd*> _mapWnd;
	/// ex: 사용예
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
public: // serialization에서만 만들어집니다.
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

class CDockablePaneExInvokable : public CDockablePane
	, public KLambdaTimer//?LbTimer 1
{
	DECLARE_DYNAMIC(CDockablePaneExInvokable)
public:
	CDockablePaneExInvokable() noexcept;
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
protected: // serialization에서만 만들어집니다.
	//CFormInvokable() noexcept;
	CFormInvokable(UINT nIDTemplate);
	DECLARE_DYNAMIC(CFormInvokable)

public:
	/// <summary>
	/// OnSize에서 늘릴때 따라 늘어 나는거 할떄 이전 크기 저장 해두는 거
	/// </summary>
	CSize m_sz;

	// 특성입니다.
public:
	// 작업입니다.
public:
	virtual CDocument* GetDoc()
	{
		return NULL;
	}
	// 재정의입니다.
public:
protected:
	//필요 하면 넣고, 자식의 것에서 아버지를 제대로 명칭 해야
	//virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	//virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

	// 구현입니다.
public:
	virtual ~CFormInvokable();
#ifdef _DEBUG
	//virtual void AssertValid() const;
	//virtual void Dump(CDumpContext& dc) const;
#endif

	BOOL OnSizeDefault(UINT nType, int cx, int cy, int nCtrl, int arIdc[]);

protected:

	// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg LRESULT OnBeginInvoke(WPARAM wParam, LPARAM lParam);//?beginInvoke 1
	afx_msg void OnTimer(UINT_PTR nIDEvent);//?LbTimer 3

};

/* CFormInvokable 계승 하는 법 4군데
* CFormView 를 CFormInvokable 로만 바꾸면 됨.
* 헤더에서 1
class CPostTestView : public CFormInvokable
* cpp에서 2
IMPLEMENT_DYNCREATE(CPostTestView, CFormInvokable)
* cpp에서 3
BEGIN_MESSAGE_MAP(CPostTestView, CFormInvokable)
* cpp에서 4
CPostTestView::CPostTestView() noexcept
	: CFormInvokable(IDD_MFCAPPPOSTTEST1_FORM) //할아버지 객체 것을 바로 부를수 없어서 아버지꺼 부름.

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
	PWS title; // 뒤에 comment 따라 오는것 편집 쉽게 뒤로 붙임
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

