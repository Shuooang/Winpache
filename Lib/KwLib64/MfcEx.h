#pragma once
#include <afxdockablepane.h>
#include <afxpropertygridctrl.h>
#include <afxribbonbar.h>

#include <afxribboncategory.h>
#include <afxribbonpanel.h>

#include "DlgTool.h"
#include "TimeTool.h"
/*
class CLogOutput// : public ILogOutput
{
public:
	virtual CString& Log(LPCTSTR lpFormat, ...);
	virtual CString& LogErr(LPCTSTR lpFormat, ...);
	virtual CString& LogBox(LPCTSTR lpFormat, ...);
	virtual void LogFailedGoogle(CString s);
	virtual CString& LogTip(LPCTSTR lpFormat, ...);
};
*/

class CPaneForm;

class CDockFormChild : public CFormInvokable // CFormView//, public CLogOutput
	//, public IProjTime
{
	DECLARE_DYNCREATE(CDockFormChild)

protected:
	CDockFormChild(int idd = 0, CPaneForm* ppn = NULL)           // 동적 만들기에 사용되는 protected 생성자입니다.
		: CFormInvokable(idd), m_ppn(ppn)
		, m_bInited(FALSE)
	{
	}
	virtual ~CDockFormChild();
	CKTime m_tRefresh;
	//virtual CKTime _GetNow(ETmArea e = eCtLocal)
	//{
	//	return CLocalTime::GetNow__(e);//
	//}

public:
	BOOL m_bInited;
	CPaneForm* m_ppn;
	CPaneForm* Pane() { return m_ppn; }
	//enum { IDD = IDD_FORMPANE };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	static CDockFormChild* CreateInnerFormView(CWnd* pParent, CDockFormChild* pFormView);
	static CDockFormChild* CreateOne(CWnd* pParent);
	virtual void _OnShowPane()
	{
	}
	BOOL IsShown();
	virtual void OnDockFocus()
	{
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate();

	DECLARE_MESSAGE_MAP()
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	void OnSizing(UINT fwSide, LPRECT pRect);
public:
	afx_msg void OnStnClickedMfcpropertygrid1();
	afx_msg void OnStnDblclickMfcpropertygrid1();
};



class CPaneForm : public CDockablePane //: CPane : CBasePane
{
	DECLARE_DYNAMIC(CPaneForm)

public:
	CPaneForm();
	virtual ~CPaneForm();
	CDockFormChild* m_pForm;
	virtual CDockFormChild* _CreateForm() = NULL;
	void ShowDock(BOOL bShow = TRUE)
	{
		ShowPane(bShow, TRUE, TRUE);
	}
	CString m_sTitle;
	int m_nBlink;
	UINT _nID{ 0 }; // 이 pane이 아니고 child formview의 ID

	void StartTitleBlinking();

	void PopPane(BOOL bFocus = TRUE, BOOL bTitleBlink = TRUE, BOOL bCloseAndOpen = TRUE);
protected:
	DECLARE_MESSAGE_MAP()

	virtual void AdjustPaneToPaneContainer(CPaneDivider* pSlider);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);

	void AdjustLayout();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSetFocus(CWnd* pOldWnd);

public:
	virtual HDWP MoveWindow(CRect& rect, BOOL bRepaint = TRUE, HDWP hdwp = NULL)
	{
		return __super::MoveWindow(rect, bRepaint, hdwp);
	}
	virtual HDWP SetWindowPos(const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags, HDWP hdwp = NULL)
	{
		return __super::SetWindowPos(pWndInsertAfter, x, y, cx, cy, nFlags, hdwp);
	}
	virtual void ShowPane(BOOL bShow, BOOL bDelay, BOOL bActivate)
	{
		__super::ShowPane(bShow, bDelay, bActivate);
	}

};


class KRibbon
{
public:
	static void ContextCategory(CFrameWnd* pfm, CMFCRibbonBar* rbr, bool bShow, int idd);
};



#include "KJson.h"

using namespace Kw;
class KProperty;

class KPrGrp
{
public:
	KPrGrp(KProperty* prp, KPrGrp* uper, PWS title = nullptr)
		: _prpObj(prp)
		, _upper(uper)
	{
		if(title)
		{
			//_prpt = new CMFCPropertyGridProperty(title);//group인경우만 여기로 준다
			_title = title;
		}
	}
	~KPrGrp()
	{
	}
	/// 상위 링크 : free 하면 안되.
	KProperty* _prpObj{ nullptr };
	KPrGrp* _upper{ nullptr };
	// group title
	CStringA _key;
	wstring _title;
	// 자기 자신 그룹 프로퍼티 : 값은 없다. 여기에 값프로퍼티를 추가 해야(SetField)
	CMFCPropertyGridProperty* _prpt{ nullptr };
	// root는 프로퍼티가 없다. 그자체가 Ctrl이다.
	bool _bRoot{ false };

	// 서브 그룹 - 2단을 전제로. 3단 이상인 경우 이것이 size() > 0 이다.
	KStdMapPtr<string, KPrGrp> _grps;// 그룹명 vs 한아이템
//	shared_ptr<KPrGrp> _shGrp;
		/// 키 vs item grid ctrl
	//KStdMap<string, CMFCPropertyGridProperty*> _subItems;
	// 양방향 매핑 keyField, txt vs code and code vs txt
	KStdMapPtr<string, KStdMap<wstring, wstring>> _mapOptCode;
	KStdMapPtr<string, KStdMap<wstring, wstring>> _mapOptText;

	void Reset();
//void SetField(PAS keyField, PWS fname, PWS sHelp, COleVariant& defVal, BOOL bEdit = FALSE)
	/// defVal: default 값을 넣을대. int는 long으로 넣어야
	/// AddGroup 와 비슷
	template<typename TVal>
	KPrGrp* SetField(PAS keyField, PWS fname, PWS sHelp, TVal defVal, BOOL bEdit = FALSE)
	{
		ASSERT(_prpt);
		if(!sHelp)
			sHelp = fname;

		auto subPr = new KPrGrp(_prpObj, this, fname);//1.생성 이안에서 new CMFCPropertyGridProperty 한다
		CString sfname; sfname.Format(L"%s%s", fname, bEdit ? L" *" : L"");
		subPr->_prpt = new CMFCPropertyGridProperty(sfname, defVal, sHelp);

		_grps[keyField] = subPr;//2.데이터 저장
		_prpt->AddSubItem(subPr->_prpt);//3.컨트롤 조작
		subPr->_prpt->AllowEdit(bEdit);//4.추가속성
		return subPr;// ->_prpt;//subProperty return 해줘야 SetData token만든다
	}
	BOOL IsGroup()
	{
		return _prpt->IsGroup();//return _prpt->GetSubItemsCount() > 0;
	}

	void SetValue(PAS keyField, ShJVal sjv);

	/// 어떤 옵션이 선택되었나? code(DB에 진짜 값)를 알아낸다.
	//PWS GetOptionCode(PAS keyField, int i);

	CMFCPropertyGridProperty* GetSubItem(PAS keyField);
	KPrGrp* AddGroup(KProperty* prp, PAS keyGrp, PWS title);

	void ClearAllOption();
	void ClearOption(PAS key);
	PWS GetOptionTxt(PAS keyField, PWS fCode);
	KPrGrp* FindGroup(PAS fGrp);
	void AddOption(PAS keyField, PWS opCode, PWS optxt);
};


class KProperty
{
public:
	KProperty(CMFCPropertyGridCtrl* cProperty)
		: _cProperty(cProperty)
	{
		_sjoCp = make_shared<JObj>();
		_grp->_bRoot = true;
		//_grps._token = 313409;
	}
	~KProperty()
	{
		RemoveAll();
		DeleteMeSafe(_grp);
	}

	KPrGrp* _grp{ new KPrGrp(this, nullptr) };
	//KStdMapPtr<string, KPrGrp> _grps;// 그룹명 vs 한아이템
	CMFCPropertyGridCtrl* _cProperty{ nullptr };

	/// 복사된 데이터. 편집중
	ShJObj _sjoCp;
	ShJObj _sjo;

	/// 읽어온 데이터
	//ShJArr _tbl;
	ShJArr _enum;
	///        fTable             fField          fCode    fKR
	KStdMapPtr<string, KStdMapPtr<string, KStdMap<wstring, wstring>>> _mapOptCodeEx;
	KStdMapPtr<string, KStdMapPtr<string, KStdMap<wstring, wstring>>> _mapOptTextEx;
	void AddOption(PAS fTable, PAS fField, PWS fCode, PWS fKR);

	//bool _bOptionDataDone{ false };
	CKTime _tDoneOption;

	void RemoveAll();

	KPrGrp* FindGroup(PAS keyGrp);

	void AddGroup(PAS keyGrp, PWS title);
	// L""는 포인터 인데, 정수나 실수로 인식 ambigous 나므로 따로 뺀다.
	void SetField(PAS keyGrp, PAS keyField, PWS fname, PWS sHelp)
	{
		SetField(keyGrp, keyField, fname, sHelp, L"", FALSE);
	}
	void SetField(PAS keyGrp, PAS keyField, PWS fname, PWS sHelp, int val)
	{
		SetField(keyGrp, keyField, fname, sHelp, (long)val, FALSE);
	}
	//void SetEditField(PAS keyGrp, PAS keyField, PWS fname, PWS sHelp)
	//{
	//	SetField(keyGrp, keyField, fname, sHelp, TRUE);
	//}
	//?주의: int는 없고 long만 있으니 cast 해서 부를 것
	template<typename TVal>
	void SetField(PAS keyGrp, PAS keyField, PWS fname, PWS sHelp, TVal defltVal, BOOL bEdit = FALSE)
	{
		KPrGrp* grp = FindGroup(keyGrp);
		auto subPr = grp->SetField(keyField, fname, sHelp, defltVal, bEdit);

		SetFullKeyIndex(keyGrp, keyField, subPr);
		//CStringA fkey; fkey.Format("%s/%s", keyGrp, keyField);
		//_mapPrpt[(PAS)fkey] = grp->_prpt;
		//_token++;/// 1,2,3...
		//_mapPrptDataKey[_token] = (PAS)fkey;
		//subPrpt->SetData((DWORD_PTR)_token);
	}

	/// 후에 GetData -> _mapPrptDataKey -> fullkey 를 위해 만들어 둔다.
	void SetFullKeyIndex(PAS keyGrp, PAS keyField, KPrGrp* newPr)
	{
		CStringA fkey; 
		if(keyField)
		{
			fkey.Format("%s/%s", keyGrp, keyField);
			newPr->_key = keyField;
		}
		else
		{
			fkey = keyGrp;
			newPr->_key = keyGrp;
		}

		_mapPrpt[(PAS)fkey] = newPr->_prpt;// fullkey로 KPr 찾기
		_token++;/// 1,2,3...
		newPr->_prpt->SetData((DWORD_PTR)_token);
		_mapPrptDataKey[_token] = (PAS)fkey;// GetData -> fullkey
		_mapDataPr[_token] = newPr;          // GetData -> KPr
	}
	/// OnPropertyChanged 에서 GetData 로 관련 풀필드를 찾는다.
	KStdMap<string, CMFCPropertyGridProperty*> _mapPrpt;
	KStdMap<int, string> _mapPrptDataKey;
	KStdMap<int, KPrGrp*> _mapDataPr;
	int _token{ 0 };

	CMFCPropertyGridProperty* FindProperty(int token)
	{
		ASSERT(token > 0);
		string fkey = _mapPrptDataKey[token];
		CMFCPropertyGridProperty* prpt = _mapPrpt[fkey];
		return prpt;
	}
	string FindFullkey(int token)
	{
		ASSERT(token > 0);
		string fkey = _mapPrptDataKey[token];
		return fkey;
	}

	KPrGrp* FindKProperty(int token)
	{
		ASSERT(token > 0);
		auto kpr = _mapDataPr[token];
		return kpr;
	}





	void SetValue(PAS keyGrp, PAS keyField, ShJVal sjv)
	{
		KPrGrp* grp = FindGroup(keyField);
		grp->SetValue(keyField, sjv);// _prpt->AddSubItem(sub);
	}

	void SetJObj(ShJObj sjo, PAS table = "table", PAS tenum = "tenum");
	void SetJObj(ShJArr sjoTable, ShJArr arEnum);
	void SetJObj(ShJObj joRow, ShJArr arEnum);

	// enum option list를 다시 갱신 하려면
	void AddOption(ShJObj sjEnum);//PAS keyGrp, PAS keyField, PWS optxt, PWS opCode);
	void ToRefreshOption();
	wstring GetOptionTxt(PAS keyField, PWS fCode, PAS grpKey = NULL);
	void ClearOption(PAS keyField);
	void ClearAllOptions();

};

CMFCRibbonBaseElement* KwGetRibbonMenu(CMFCRibbonBar* bar, int idc);
CMFCRibbonBaseElement* KwGetRibbonMenu(CMFCRibbonBar* bar, int iCat, int iPan, int iMenu);
CMFCRibbonBaseElement* KwGetRibbonMenu(CMFCRibbonBar* bar, PWS sCat, int iPan, int iMenu);
