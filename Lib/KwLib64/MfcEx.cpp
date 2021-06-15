#include "pch.h"
#include "MfcEx.h"
#include "Kw_tool.h"
#include "KDebug.h"
//C:\Program Files(x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.28.29333\atlmfc\include\afxribbonbar.h

// CDockFormChild

IMPLEMENT_DYNCREATE(CDockFormChild, CFormInvokable)

CDockFormChild::~CDockFormChild()
{
}

void CDockFormChild::DoDataExchange(CDataExchange* pDX)
{
	CFormInvokable::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDockFormChild, CFormInvokable)
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


// CDockFormChild 진단입니다.

#ifdef _DEBUG
void CDockFormChild::AssertValid() const
{
	CFormInvokable::AssertValid();
}

#ifndef _WIN32_WCE
void CDockFormChild::Dump(CDumpContext& dc) const
{
	CFormInvokable::Dump(dc);
}
#endif
#endif //_DEBUG


// CDockFormChild 메시지 처리기입니다.
void CDockFormChild::OnInitialUpdate()
{
	CFormInvokable::OnInitialUpdate();

	GetParentFrame()->RecalcLayout();
	//SetScrollSizes( MM_TEXT, CSize( 205, 157 ) ); 
	ResizeParentToFit(FALSE);
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}

int CDockFormChild::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.


	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	int nResult = 0;
	CFrameWnd* pParentFrame = GetParentFrame();
	if(pParentFrame == pDesktopWnd)
	{
		// When this is docked 
		nResult = CFormInvokable::OnMouseActivate(pDesktopWnd, nHitTest, message);
	}
	else
	{
		// When this is not docked 
		BOOL isMiniFrameWnd = pDesktopWnd->IsKindOf(RUNTIME_CLASS(CMiniFrameWnd));
		BOOL isPaneFrameWnd = pDesktopWnd->IsKindOf(RUNTIME_CLASS(CPaneFrameWnd));
		BOOL isMultiPaneFrameWnd = pDesktopWnd->IsKindOf(RUNTIME_CLASS(CMultiPaneFrameWnd));
		// pDesktopWnd is the frame window for CDockablePane 
		nResult = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
	}
	return nResult;
	//return CFormInvokable::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void CDockFormChild::OnSizing(UINT fwSide, LPRECT pRect)
{
	CFormInvokable::OnSizing(fwSide, pRect);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

CDockFormChild* CDockFormChild::CreateInnerFormView(CWnd* pParent, CDockFormChild* pFormView)
{
	CCreateContext* pContext = NULL;
	if(!pFormView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 205, 157), pParent, 0, pContext))
	{
		AfxMessageBox(_T("Failed in creating CMyFormView"));
	}
	pFormView->OnInitialUpdate();
	return pFormView;
}

BOOL CDockFormChild::IsShown()
{
	auto b1 = ::IsWindow(this->GetSafeHwnd());
	auto b2 = this->IsWindowVisible();//뒤에 숨겨져 있으면 0
	BOOL b3 = FALSE;
	if(Pane())
		b3 = this->Pane()->IsVisible();//조금이라도 보이면 1
	if(b1)
	{
		return b2 && b3;
	}
	return FALSE;
}

/*


	2. 리소스 뷰에서 Form을 찾아 디자인한다.

		필요한 변수, 이벤트 처리를 하면 제대로 동작한다.



		3. Pane에 Form을 멤버 변수로 추가한다.

		COptionFormView* m_pOptionFormView;

*/




void CDockFormChild::OnStnClickedMfcpropertygrid1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDockFormChild::OnStnDblclickMfcpropertygrid1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}






















// CPaneForm

IMPLEMENT_DYNAMIC(CPaneForm, CDockablePane)


CPaneForm::CPaneForm()
	: m_pForm(NULL)
{
	m_sizeMin = CSize(50, 80);
}

CPaneForm::~CPaneForm()
{
}


BEGIN_MESSAGE_MAP(CPaneForm, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

static int eTimer_ShowFirst = __LINE__;

// CPaneForm 메시지 처리기입니다.

void CPaneForm::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CString scl(GetRuntimeClass()->m_lpszClassName);
	//KTrace2(L"%s::%s\t%d\t%u\n", scl, __FUNCTIONT__, bShow, nStatus);
	__super::OnShowWindow(bShow, nStatus);

}
void CPaneForm::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	//_cProperty.SetFocus();
	//auto pfn = (CMainFrame*)AfxGetMainWnd();
	//pfn->ContextCategory(ID_CONTEXT_Order);
	m_pForm->OnDockFocus();
	
}

int CPaneForm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	//	m_wndChild.Create(_T("STATIC"),_T("DockingFormTest"),WS_CHILD|WS_VISIBLE|WS_BORDER,CRect(0,0,0,0),this,1234);

	CRuntimeClass* pNewViewClass;
	pNewViewClass = RUNTIME_CLASS(CFormInvokable);
	CCreateContext context;
	context.m_pNewViewClass = pNewViewClass;

	m_pForm = CDockFormChild::CreateInnerFormView(this, _CreateForm());
	return 0;
}

void CPaneForm::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if(m_pForm->GetSafeHwnd())
	{
		m_pForm->SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOZORDER);
		m_pForm->Invalidate(1);
	}
}

void CPaneForm::AdjustLayout()
{
	if(!GetSafeHwnd()) return;
	CRect rcClient;
	GetClientRect(rcClient);
	//	m_wndForm.SetWindowPos(NULL,rcClient.left, rcClient.top, rcClient.Width(),rcClient.Height(),SWP_NOZORDER|SWP_NOACTIVATE);
}

void CPaneForm::AdjustPaneToPaneContainer(CPaneDivider* pSlider)
{
	__super::AdjustPaneToPaneContainer(pSlider);
	AdjustLayout();
}

void CPaneForm::OnTimer(UINT_PTR nIDEvent)
{
	// 	if(nIDEvent == eTimer_ShowFirst)
	// 	{
	// 		KillTimer(eTimer_ShowFirst);
	// 		if( m_pForm )
	// 			m_pForm->_OnShowPane();
	// 	}
	// 	else 
	if(nIDEvent == 100)
	{
		CString s;
		if(m_nBlink > 10)
		{
			KillTimer(100);
			s = m_sTitle;
		}
		else
		{
			if(m_nBlink % 2 == 1)
				s = m_sTitle;
		}
		SetWindowText(s);// SetWindowText에서 오류
		m_nBlink++;
	}

	__super::OnTimer(nIDEvent);
}

void CPaneForm::StartTitleBlinking()
{
	return;// SetWindowText에서 오류
	m_nBlink = 0;
	GetWindowText(m_sTitle);
	SetTimer(100, 400, NULL);
}

void CPaneForm::PopPane(BOOL bFocus/*=TRUE*/, BOOL bTitleBlink/*=TRUE*/, BOOL bCloseAndOpen/*=TRUE*/)
{
	if(bCloseAndOpen)
		ShowDock(FALSE);//ShowPane(bShow, TRUE, TRUE);
	ShowDock(TRUE);
	if(bTitleBlink)
		StartTitleBlinking();
	if(bFocus)
	{
		SetActiveWindow();//not working
		SetFocus();//working
	}
}
void CPaneForm::OnMove(int x, int y)
{
	__super::OnMove(x, y);
	CRuntimeClass* prc = GetRuntimeClass();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CRect rc, rcc;
	GetWindowRect(rc);
	GetClientRect(rcc);
//	TRACE(L"%4d,%4d(%4d,%4d)(%4d,%4d) %s\r\n", x, y, rc.Width(), rc.Height(), rcc.Width(), rcc.Height(), CString(prc->m_lpszClassName));
}

void KRibbon::ContextCategory(CFrameWnd* pfm, CMFCRibbonBar* rbr, bool bShow, int idd)
{
	//auto pfm = this;// (CMainFrame*)AfxGetMainWnd();
	if(bShow)
	{
		// 순서 지켜야 함
		rbr->ShowContextCategories(idd, TRUE);
		int ret = rbr->ActivateContextCategory(idd);
	}
	else
	{
		//	비활성화		// 단일 Context
		rbr->ShowContextCategories(idd, FALSE);
		// 모든 Context
		rbr->HideAllContextCategories();
	}
	// 이후 반드시 호출
	rbr->RecalcLayout();
	rbr->RedrawWindow();

	pfm->SendMessage(WM_NCPAINT, 0, 0);
}

void KProperty::SetJObj(ShJObj sjo, PAS table, PAS tenum)
{
	//if(!_tbl)
	//	_tbl = make_shared<JArr>();
	//else
	//	_tbl->clear();

	//if(!sjo->Has(table))
	//	table = "table";// 잘못 준경우 디폴트로
	ShJArr jarTable;
	ShJArr jarEnum;
	if(sjo->Has(table) && sjo->IsArray(table))
		jarTable = sjo->Array(table);//데이터 가져온다. 원본.

	if(!_enum)
		_enum = make_shared<JArr>();
	else
		_enum->clear();
	if(sjo->Has(tenum) && sjo->IsArray(tenum))
		jarEnum = sjo->Array(tenum);//필드별 코드와 출력용문자열 데이터 챙긴다.
	SetJObj(jarTable, jarEnum);
}
void KProperty::SetJObj(ShJArr jarTable, ShJArr jarEnum)
{
	ShJArr tbl;
	tbl = jarTable;
	if(tbl->size())
	{
		auto& sjv = (*tbl)[0];
		SetJObj(sjv->AsObject(), jarEnum);
	}
}
void KProperty::SetJObj(ShJObj sjo, ShJArr sjaEnum)
{
	if(!sjo || !sjaEnum)
		return;
	if(_sjo.get())
		_sjo->clear();
	else
		_sjo = make_shared<JObj>();
	if(
		_sjoCp.get())
		_sjoCp->clear();
	else
		_sjoCp = make_shared<JObj>();

	
	if(_enum.get())
		_enum->clear();
	else
		_enum = make_shared<JArr>();

#define _CloneSjoX

#ifdef _CloneSjo
	_sjo = sjo;/// Clone안하고 이렇게 하면 값이 날라 간다. 어디서 날라 가지? 다 shared_ptr로 했는데.
	_enum = sjaEnum;
	_sjoCp->Clone(sjo, true);/// 복사본으로 작업한다.
#else
	_sjo->Clone(sjo, true);//원본
	_enum->Clone(sjaEnum, true);
	_sjoCp->Clone(sjo, true);/// 복사본으로 작업한다.
#endif
	CKTime now = KwGetCurrentTime();
	auto sp = now - _tDoneOption;
#ifdef DEBUG
	int mExpire = 1;
#else
	int mExpire = 15;
#endif
	if(sp.GetTotalMinutes() > mExpire)// _bOptionDataDone)
	{
		ToRefreshOption();

		for(auto& sjv : *_enum)
		{
			ASSERT(sjv->IsObject());
			auto& sjo = sjv->AsObject();
			AddOption(sjo);// fGrp, fField, sjo->S("fKR"), sjo->S("fCode"));
		}
	}

	for(auto& [k, sjv] : *_sjoCp)//복사본을 화면에 뿌린다.
	{
		CStringA ka(k.c_str());
		KPrGrp* grp = nullptr;
		if(_grp->_grps.Lookup((PAS)ka, grp))
		{
			//auto grp = _grp->_grps[(PAS)ka];
			grp->SetValue(ka, sjv);
		}
		else // 없으면 그룹이 Object로 분리 안되어 있나 보다. 그러면 다 찾아 보는 수밖에. 2차원 이라 가정 하고
		{
			for(auto& [kGrp, grpSub] : _grp->_grps)
			{
				KPrGrp* prSub = nullptr;
				if(grpSub->_grps.Lookup((PAS)ka, prSub))
				{
					prSub->SetValue(ka, sjv);
					break;
				}
			}
		}
	}

}

/// group을 gOrer, gPrd.. 리커시브하게 찾는다.
/// 키로 시작하는 그룹을 찾는다. 뒤에 번호 빼고 그룹을 찾는다.
KPrGrp* KPrGrp::FindGroup(PAS fGrp)
{
	KPrGrp* grp = nullptr; // _grps[fGrp]; // 하면 gPet1 뒤에 1 때문에 못찾는다.
	for(auto& [kg, krp] : _grps)//모든 그룹을 다 디져서 필드 subProperty item을 찾아 간다.
	{
		if(tchbegin(kg.c_str(), fGrp))
			return krp;
		if(krp->_grps.size() > 0)
		{
			auto grp = krp->FindGroup(fGrp);
			if(grp)
				return grp;
		}
	}
	return nullptr;
}
/// 이건 2차원 일때 쓰던거
void KPrGrp::SetValue(PAS keyField, ShJVal sjv)
{
	//auto sub = GetSubItem(keyField);
	//if(!sub)
	//	return;//여기 그룹 필드가 아니네
	if(!_prpt)
		return;
	auto var0 = _prpt->GetValue();

	COleVariant var;
	CString sval;
	if(sjv->IsString())
	{
		var = sjv->AsString().c_str();
		sval = sjv->Str();
		KwTextToVar(var0, sval);
	}
	else if(sjv->IsInt())
	{
		var = (long)sjv->AsInt();
		sval = sjv->Str();
		KwTextToVar(var0, sval);
	}
	else if(sjv->IsDouble())
	{
		var = sjv->AsDouble();
		sval = sjv->Str(1);
		KwTextToVar(var0, sval);
	}
	else if(sjv->IsNull())
	{
		KwNullToVar(var0);
	}
	else if(sjv->IsObject())
	{
		auto sjo = sjv->AsObject();
		for(auto& [fld, sjvSub] : *sjo)
		{
			//CStringA key; key.Format("%s/%s", keyField, CStringA(fld.c_str()));
			CStringA fldA(fld.c_str());
			if(fldA == "fKind")
				_break;
			KPrGrp* grp = nullptr;
			if(_grps.Lookup((PAS)fldA, grp))/// 만들어놓은 프로퍼티가 있을 경우만. 필드별로 다 있는건 아니다.
			{
				//auto grp = _grps[(PAS)fldA];
				grp->SetValue(fldA, sjvSub);
			}
	/*		int nsub = _prpt->GetSubItemsCount();
			for(int i = 0; i < nsub; i++)
			{
				auto* sub = _prpt->GetSubItem(i);
				auto itok = (int)sub->GetData();
				if(itok > 0)
				{
					auto fullKey = this->_prpObj->_mapPrptDataKey[itok];
					auto grp = _prpObj ->FindGroup(
					auto prpt = _prpObj->_mapPrpt[fullKey];
					prpt->SetValue(
					//auto data = (PAS)sub->GetData();
					//if(tchsame(data.c_str(), keyField))
					//	sub;
				}
			}*/
		}
		//auto grp = _grps[keyField];
		//grp->SetValue(keyField, sjv);
		//for(auto& [k, v] : *sjo)
		//{
		//	for(auto& [k1, v1] : _grps)
		//	{
		//	}
		//}
	}
	else if(sjv->IsArray())
	{
		throw_str("아직 Array는 프로퍼티에 지원 안함.");
	}
	else
		throw_str("Property unknown type");

	//if(var0.vt == VT_BSTR)
	//	_prpt->SetValue(var);
	if(var.vt == VT_BSTR)
	{
		auto var1 = _prpObj->GetOptionTxt(keyField, var.bstrVal);/// 옵션데이터는 위쪽에서 갖고 있다.
		//PWS var1 = _upper->GetOptionTxt(keyField, var.bstrVal);/// 옵션데이터는 위쪽에서 갖고 있다.
		if(var1.length() > 0)
			var = var1.c_str();
		_prpt->SetValue(var);
	}
	else
		_prpt->SetValue(var0);
	//_prpt->AddSubItem(sub); SetField할때 이미 했어.
}
// enum option list를 다시 갱신 하려면
void KProperty::ToRefreshOption()
{
	ClearAllOptions();
	//for(auto& [kg, krp] : _grps)//모든 그룹을 다 디져서 필드 subProperty item을 찾아 간다.
	//{
	//	KPrGrp* grp = _grps[kg];
	//	grp->ClearAllOptions();
	//}
}

KPrGrp* KProperty::FindGroup(PAS keyGrp)
{
	CArray<CStringA, PAS> ar;
	KwCutStrByCharT('/', CStringA(keyGrp), ar);
	auto* grps = &_grp->_grps;
	KPrGrp* grp = nullptr;
	for(int i = 0; i < ar.GetCount(); i++)
	{
		auto k = (PAS)ar[i];
		grp = (*grps)[k];		ASSERT(grp);
		grps = &grp->_grps;
		if(grps->size() == 0)
			break;
	}
	return grp;
}

void KProperty::AddOption(ShJObj sjEnum)//PAS keyGrp, PAS keyField, PWS optxt, PWS opCode)
{
	/// "fTable":"torderschd"
	/// "fGrp" :"gOrder",
	/// "fField":"fWorkStat",
	/// "fCode" :"pause",
	/// "fKR"   :"잠시멈춤",
	CStringA fTable(sjEnum->S("fTable"));
	CStringA fField(sjEnum->S("fField"));
	CStringW fCode(sjEnum->S("fCode"));
	CStringW fKR(sjEnum->S("fKR"));
	CStringA fGrp(sjEnum->S("fGrp"));
	CStringA fTableG = fTable;
	if(fGrp.GetLength() > 0)
		fTableG = fGrp;
	AddOption(fTableG, fField, fCode, fKR);

	//KPrGrp* grp = _grp; // default
	//if(sjEnum->Has("fGrp"))//만약 서버에서 그룹을 지정 해 준경우(예:오더)
	//{
	//	CStringA fGrp(sjEnum->S("fGrp"));
	//	grp = _grp->FindGroup(fGrp);
	//}
	//else
	//{
	//	for(auto& [kGrp, grpSub] : _grp->_grps)// 첫번째 그룹 다 디진다. 데이터가 1차원 JObj일때
	//	{
	//		if(grpSub->_grps.Has((PAS)fField))
	//		{
	//			grp = grpSub;
	//			break;
	//		}
	//		//KPrGrp* prSub = nullptr; 최종값 바로 위에 그룹이 가지고 있어야지. 말단을 주면 안되지.
	//		//if(grpSub->_grps.Lookup((PAS)fField, prSub))
	//		//{
	//		//	grp = prSub;
	//		//	break;
	//		//}
	//	}
	//}
	//if(grp)
	//	grp->AddOption(fField, fCode, fKR);
}

void KProperty::AddOption(PAS fTable, PAS fField, PWS fCode, PWS fKR)
{
	KStdMapPtr<string, KStdMap<wstring, wstring>>* mapField = nullptr;
	KStdMapPtr<string, KStdMap<wstring, wstring>>* mapFieldX = nullptr;
	KStdMap<wstring, wstring>* mapCode = nullptr;
	KStdMap<wstring, wstring>* mapCodeX = nullptr;

	if(!_mapOptCodeEx.Lookup(fTable, mapField))
	{
		mapField = new KStdMapPtr<string, KStdMap<wstring, wstring>>();
		_mapOptCodeEx[fTable] = mapField;
	}
	if(!mapField->Lookup(fField, mapCode))
	{
		mapCode = new KStdMap<wstring, wstring>();
		mapField->SetAt(fField, mapCode);
	}
	mapCode->SetAt(fCode, fKR);

	if(!_mapOptTextEx.Lookup(fTable, mapFieldX))
	{
		mapFieldX = new KStdMapPtr<string, KStdMap<wstring, wstring>>();
		_mapOptTextEx[fTable] = mapFieldX;
	}
	if(!mapFieldX->Lookup(fField, mapCodeX))
	{
		mapCodeX = new KStdMap<wstring, wstring>();
		mapFieldX->SetAt(fField, mapCodeX);
	}
	mapCodeX->SetAt(fKR, fCode);//반대 방향
}

//?deprecated
void KPrGrp::AddOption(PAS keyField, PWS opCode, PWS optxt)
{
	if(!_grps.Has(keyField))/// 만들어놓은 프로퍼티가 있을 경우만. 필드별로 다 있는건 아니다.
		return;

	auto subPr = _grps[keyField];//없을수도 있다.
	if(!subPr || !subPr->_prpt)
		return;

	subPr->_prpt->AddOption(optxt);/// 이텍스트를 선택 하면 _mapOptText 에서 코드를 알아야 저장 한다.

	if(!_mapOptCode.Has(keyField))
		_mapOptCode[keyField] = new KStdMap<wstring, wstring>();
	auto dicc = _mapOptCode[keyField];
	dicc->SetAt(opCode, optxt);/// code vs text 

	/// 키가 2개니 중복될일은 없겠지.
	if(!_mapOptText.Has(keyField))
		_mapOptText[keyField] = new KStdMap<wstring, wstring>();
	auto dict = _mapOptText[keyField];
	dict->SetAt(optxt, opCode);/// text vs code 반대방향도 매핑
}


/// SetField와 비슷
KPrGrp* KPrGrp::AddGroup(KProperty* prp, PAS keyGrp, PWS title)
{
	auto* subPr = new KPrGrp(prp, this, title);
	subPr->_prpt = new CMFCPropertyGridProperty(title);
	_grps[keyGrp] = subPr;
	if(_bRoot)
	{
		_prpObj->_cProperty->AddProperty(subPr->_prpt);
	}
	else
		_prpt->AddSubItem(subPr->_prpt);

	return subPr;
}


void KProperty::AddGroup(PAS keyGrp, PWS title)
{
	//if(!tchchr(keyGrp, '/'))
	//{
	//	auto* pkgr = new KPrGrp(title);
	//	_grps[keyGrp] = pkgr;
	//	_cProperty->AddProperty(pkgr->_prpt);
	//}
	//else
	CArray<CStringA, PAS> ar;
	KwCutStrByCharT('/', CStringA(keyGrp), ar);
	auto grp = _grp;
	for(int i = 0; i < ar.GetCount(); i++)
	{
		auto k = (PAS)ar[i];
		//KPrGrp* prp = nullptr;
		if(!grp->_grps.Has(k))
		{
			auto subPr = grp->AddGroup(this, k, title);
			SetFullKeyIndex(keyGrp, nullptr, subPr);
			grp = subPr;
		}
		else
		{
			grp = grp->_grps[k];
		}
	}
//	KPrGrp* grp = FindGroup(keyGrp);
}

/// 특정키 필드의 옵션만 모두 삭제 한다.
void KPrGrp::ClearOption(PAS key)
{
	if(_grps.size() > 0)
	{
		for(auto& [k, grp] : _grps)
			grp->ClearOption(key);
	}
	else
	{
		auto sub = GetSubItem(key);
		if(sub)
			sub->RemoveAllOptions();
	}
}
void KPrGrp::ClearAllOption()
{
	if(_grps.size() > 0)
	{
		for(auto& [k, grp] : _grps)
			grp->ClearAllOption();
	}
	else
	{
		//::IsWindow(_prpt)
		int nsub = _prpt->GetSubItemsCount();
		for(int i = 0; i < nsub; i++)
		{
			auto* sub = _prpt->GetSubItem(i);
			if(sub->GetOptionCount() > 0)
				sub->RemoveAllOptions();
		}
		_mapOptCode.clear();
	}
}

void KProperty::ClearOption(PAS keyField)
{
	KPrGrp* grp = FindGroup(keyField);
	grp->ClearOption(keyField);
	auto sub = grp->GetSubItem(keyField);
	if(sub)
		sub->RemoveAllOptions();
	auto& arr = grp->_mapOptCode[keyField];
	arr->clear();
}

/// 시간이 지나서 옵션을 갱신 하고자 할떄 : 필요할까
void KProperty::ClearAllOptions()
{
	for(auto& [k, grp] : _grp->_grps)
		grp->ClearAllOption();
}

wstring KProperty::GetOptionTxt(PAS keyField, PWS fCode, PAS grpKey)
{
	//KPrGrp* grp = FindGroup(keyField);
	//return grp->GetOptionTxt(keyField, fCode);
	KStdMapPtr<string, KStdMap<wstring, wstring>>* mapField = nullptr;
	KStdMap<wstring, wstring>* mapCode = nullptr;
	if(grpKey)
	{
		if(_mapOptCodeEx.Lookup(grpKey, mapField))/// Lookup 은 value type이 너무 길때 auto를 쓰기 위해 안쓴다.
		{
			if(mapField->Lookup(keyField, mapCode))/// Lookup 은 value type이 너무 길때 auto를 쓰기 위해 안쓴다.
			{
				wstring val;
				if(mapCode->Lookup(fCode, val))
					return val;
			}
		}
	}
	else
	{
		for(auto& [gk, mf] : _mapOptCodeEx)
		{
			if(mf->Lookup(keyField, mapCode))/// Lookup 은 value type이 너무 길때 auto를 쓰기 위해 안쓴다.
			{
				wstring val;
				if(mapCode->Lookup(fCode, val))
					return val.c_str();
			}

		}
	}
	return L"";
}
PWS KPrGrp::GetOptionTxt(PAS keyField, PWS fCode)
{
	if(_mapOptCode.Has(keyField))/// Lookup 은 value type이 너무 길때 auto를 쓰기 위해 안쓴다.
	{
		auto dmap = _mapOptCode[keyField];
		return (*dmap)[fCode].c_str();
	}
	return nullptr;
}

//PWS KPrGrp::GetOptionCode(PAS keyField, int i)
//{
//	if(_mapOptCode.Has(keyField))
//	{
//		auto& arr = _optCode[keyField];
//		return arr[i];
//	}
//	return L"";
//}
/// 찾은 서브 아이탬이 그룹일수도 있다.
CMFCPropertyGridProperty* KPrGrp::GetSubItem(PAS keyField)
{
	if(_prpt)
	{
		int nsub = _prpt->GetSubItemsCount();
		for(int i = 0; i < nsub; i++)
		{
			auto* sub = _prpt->GetSubItem(i);
			auto itok = (int)sub->GetData();
			if(itok > 0)
			{
				auto data = this->_prpObj->_mapPrptDataKey[itok];
				//auto data = (PAS)sub->GetData();
				if(tchsame(data.c_str(), keyField))
					return sub;
			}
		}
	}
	return nullptr;
}


void KProperty::RemoveAll()
{
	ClearAllOptions();

	//if(_grp->_prpt)
	//	_cProperty->DeleteProperty(_grp->_prpt);
	//for(auto& [k, grpSub] : _grp->_grps)
	//{
	//	if(grpSub->_prpt)
	//		_cProperty->DeleteProperty(grpSub->_prpt);
	//}
	if(_grp)
		_grp->Reset();

	if(_cProperty && _cProperty->GetSafeHwnd())/// 이게 나중이어야. 내부적으로 갖고 있는 pointer는 null로 최기화 해야한다.
		_cProperty->RemoveAll();

	_mapPrpt.clear();
	_mapPrptDataKey.clear();
	_mapDataPr.clear();
	_token = 0;
}

void KPrGrp::Reset()
{
	for(auto& [k, grp] : _grps)
		grp->Reset();
	_grps.clear();
	//if(_prpt)// 이건 안해도, _cProperty->RemoveAll(); 에서 다 하는데.
	//	_prpt->RemoveAllOptions();
	_mapOptCode.DeleteAll();
	_mapOptText.DeleteAll();
	if(_prpt)
	{
		//_prpt->RemoveAllOptions();
		//_prpt->RemoveSubItem();
		if(_prpObj && _prpObj->_cProperty && _prpObj->_cProperty->GetSafeHwnd())/// 이게 나중이어야. 내부적으로 갖고 있는 pointer는 null로 최기화 해야한다.
			_prpObj->_cProperty->DeleteProperty(_prpt);
		//_prpt = nullptr;
	}
	//DeleteMeSafe(_prpt);
}


/// 전체 다 디져서 찾는 거
CMFCRibbonBaseElement* KwGetRibbonMenu(CMFCRibbonBar* bar, int idc)
{
	for(int nCategory = 0; nCategory < bar->GetCategoryCount(); nCategory++)
	{
		CMFCRibbonCategory* pCategory = bar->GetCategory(nCategory);
		CString strCategory = pCategory->GetName();
		for(int nPanel = 0; nPanel < pCategory->GetPanelCount(); nPanel++)
		{
			CMFCRibbonPanel* pPanel = pCategory->GetPanel(nPanel);
			CArray <CMFCRibbonBaseElement*, CMFCRibbonBaseElement*> ar;
			pPanel->GetElements(ar);
			CString spn = pPanel->GetName();
			for(int i = 0; i < ar.GetCount(); i++)
			{
				CMFCRibbonBaseElement* pel = ar[i];
				int id = pel->GetID();
				//KTrace(L"%d(%s)/%d(%s)/%d(id:%d)%s\n", nCategory, strCategory, nPanel, spn, i, id, CStringW(pel->GetRuntimeClass()->m_lpszClassName));
				if(id == idc)
					return pel;
			}
		}
	}
	return NULL;
}

/// catagory/pannel/menu순서 로 찾는 함수
CMFCRibbonBaseElement* KwGetRibbonMenu(CMFCRibbonBar* bar, int iCat, int iPan, int iMenu)
{
	CMFCRibbonCategory* pCategory = bar->GetCategory(iCat);
	if(pCategory)
	{
		CMFCRibbonPanel* pPanel = pCategory->GetPanel(iPan);
		if(pPanel)
		{
			CMFCRibbonBaseElement* pm = (CMFCRibbonBaseElement*)pPanel->GetElement(iMenu);
			return pm;
		}
	}
	return NULL;
}

/// catagory(이름)/pannel/menu순서 로 찾는 함수
CMFCRibbonBaseElement* KwGetRibbonMenu(CMFCRibbonBar* bar, PWS sCat, int iPan, int iMenu)
{
	int iCat = -1;
	for(int nCategory = 0; nCategory < bar->GetCategoryCount(); nCategory++)
	{
		CMFCRibbonCategory* pCategory = bar->GetCategory(nCategory);
		CString strCategory = pCategory->GetName();
		if(strCategory == sCat)
		{
			iCat = nCategory;
			break;
		}
	}

	if(iCat < 0)
		return NULL;

	return KwGetRibbonMenu(bar, iCat, iPan, iMenu);
}