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
	CDockFormChild(int idd = 0, CPaneForm* ppn = NULL)           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
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
	UINT _nID{ 0 }; // �� pane�� �ƴϰ� child formview�� ID

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
			//_prpt = new CMFCPropertyGridProperty(title);//group�ΰ�츸 ����� �ش�
			_title = title;
		}
	}
	~KPrGrp()
	{
	}
	/// ���� ��ũ : free �ϸ� �ȵ�.
	KProperty* _prpObj{ nullptr };
	KPrGrp* _upper{ nullptr };
	// group title
	CStringA _key;
	wstring _title;
	// �ڱ� �ڽ� �׷� ������Ƽ : ���� ����. ���⿡ ��������Ƽ�� �߰� �ؾ�(SetField)
	CMFCPropertyGridProperty* _prpt{ nullptr };
	// root�� ������Ƽ�� ����. ����ü�� Ctrl�̴�.
	bool _bRoot{ false };

	// ���� �׷� - 2���� ������. 3�� �̻��� ��� �̰��� size() > 0 �̴�.
	KStdMapPtr<string, KPrGrp> _grps;// �׷�� vs �Ѿ�����
//	shared_ptr<KPrGrp> _shGrp;
		/// Ű vs item grid ctrl
	//KStdMap<string, CMFCPropertyGridProperty*> _subItems;
	// ����� ���� keyField, txt vs code and code vs txt
	KStdMapPtr<string, KStdMap<wstring, wstring>> _mapOptCode;
	KStdMapPtr<string, KStdMap<wstring, wstring>> _mapOptText;

	void Reset();
//void SetField(PAS keyField, PWS fname, PWS sHelp, COleVariant& defVal, BOOL bEdit = FALSE)
	/// defVal: default ���� ������. int�� long���� �־��
	/// AddGroup �� ���
	template<typename TVal>
	KPrGrp* SetField(PAS keyField, PWS fname, PWS sHelp, TVal defVal, BOOL bEdit = FALSE)
	{
		ASSERT(_prpt);
		if(!sHelp)
			sHelp = fname;

		auto subPr = new KPrGrp(_prpObj, this, fname);//1.���� �̾ȿ��� new CMFCPropertyGridProperty �Ѵ�
		CString sfname; sfname.Format(L"%s%s", fname, bEdit ? L" *" : L"");
		subPr->_prpt = new CMFCPropertyGridProperty(sfname, defVal, sHelp);

		_grps[keyField] = subPr;//2.������ ����
		_prpt->AddSubItem(subPr->_prpt);//3.��Ʈ�� ����
		subPr->_prpt->AllowEdit(bEdit);//4.�߰��Ӽ�
		return subPr;// ->_prpt;//subProperty return ����� SetData token�����
	}
	BOOL IsGroup()
	{
		return _prpt->IsGroup();//return _prpt->GetSubItemsCount() > 0;
	}

	void SetValue(PAS keyField, ShJVal sjv);

	/// � �ɼ��� ���õǾ���? code(DB�� ��¥ ��)�� �˾Ƴ���.
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
	//KStdMapPtr<string, KPrGrp> _grps;// �׷�� vs �Ѿ�����
	CMFCPropertyGridCtrl* _cProperty{ nullptr };

	/// ����� ������. ������
	ShJObj _sjoCp;
	ShJObj _sjo;

	/// �о�� ������
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
	// L""�� ������ �ε�, ������ �Ǽ��� �ν� ambigous ���Ƿ� ���� ����.
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
	//?����: int�� ���� long�� ������ cast �ؼ� �θ� ��
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

	/// �Ŀ� GetData -> _mapPrptDataKey -> fullkey �� ���� ����� �д�.
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

		_mapPrpt[(PAS)fkey] = newPr->_prpt;// fullkey�� KPr ã��
		_token++;/// 1,2,3...
		newPr->_prpt->SetData((DWORD_PTR)_token);
		_mapPrptDataKey[_token] = (PAS)fkey;// GetData -> fullkey
		_mapDataPr[_token] = newPr;          // GetData -> KPr
	}
	/// OnPropertyChanged ���� GetData �� ���� Ǯ�ʵ带 ã�´�.
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

	// enum option list�� �ٽ� ���� �Ϸ���
	void AddOption(ShJObj sjEnum);//PAS keyGrp, PAS keyField, PWS optxt, PWS opCode);
	void ToRefreshOption();
	wstring GetOptionTxt(PAS keyField, PWS fCode, PAS grpKey = NULL);
	void ClearOption(PAS keyField);
	void ClearAllOptions();

};

CMFCRibbonBaseElement* KwGetRibbonMenu(CMFCRibbonBar* bar, int idc);
CMFCRibbonBaseElement* KwGetRibbonMenu(CMFCRibbonBar* bar, int iCat, int iPan, int iMenu);
CMFCRibbonBaseElement* KwGetRibbonMenu(CMFCRibbonBar* bar, PWS sCat, int iPan, int iMenu);
