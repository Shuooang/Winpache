#include "pch.h"
#include "Mainfrm.h"
#include "DockScheduleDetail.h"
#include "DockCustomDetail.h"

#include "resource.h"
//?kdw 

IMPLEMENT_DYNCREATE(DockScheduleDetailPane, CPaneForm)

// DockScheduleDetail

IMPLEMENT_DYNCREATE(DockScheduleDetail, DockClientBase)
DockScheduleDetail* DockScheduleDetail::s_me = NULL;

DockScheduleDetail::DockScheduleDetail(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn) //DockScheduleDetail::IDD IDD_DockScheduleDetail
{
	s_me = this;
}

DockScheduleDetail::~DockScheduleDetail()
{
	DeleteMeSafe(_prData);
}

void DockScheduleDetail::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, _cProperty);
}

BEGIN_MESSAGE_MAP(DockScheduleDetail, DockClientBase)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &DockScheduleDetail::OnBnClickedButton1)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_NOTIFY(NM_THEMECHANGED, IDC_MFCPROPERTYGRID1, &DockScheduleDetail::OnNMThemeChangedMfcpropertygrid1)
	ON_STN_CLICKED(IDC_MFCPROPERTYGRID1, &DockScheduleDetail::OnStnClickedMfcpropertygrid1)
	ON_STN_DBLCLK(IDC_MFCPROPERTYGRID1, &DockScheduleDetail::OnStnDblclickMfcpropertygrid1)
END_MESSAGE_MAP()


// DockScheduleDetail 진단입니다.

#ifdef _DEBUG
void DockScheduleDetail::AssertValid() const
{
	__super::AssertValid();
}

#ifndef _WIN32_WCE
void DockScheduleDetail::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}
#endif
#endif //_DEBUG


// DockScheduleDetail 메시지 처리기입니다.
// CBranchVu1 메시지 처리기입니다.
void DockScheduleDetail::OnInitialUpdate()
{
	if(m_bInited)
		return;
	__super::OnInitialUpdate();
	m_bInited = TRUE;

	InitPropList();// "init");

	AdjustLayout();

}

void DockScheduleDetail::AdjustLayout()
{
	if(GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = 30;// m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	//m_wndObjectCombo.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	//m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	if(_cProperty.GetSafeHwnd() && !_cProperty.IsIconic())
		_cProperty.SetWindowPos(nullptr, rectClient.left, rectClient.top + cyTlb, rectClient.Width(), rectClient.Height() - (cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

void DockScheduleDetail::OnSize(UINT nType, int cx, int cy)
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

void DockScheduleDetail::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);

}

void DockScheduleDetail::OnDockFocus()
{
	_cProperty.SetFocus();
	auto pfn = (CMainFrame*)AfxGetMainWnd();
	pfn->ContextCategory(ID_CONTEXT_ScheduleDetail);
}

void DockScheduleDetail::OnBnClickedButton1()
{
	//auto app = (CMfcSmpApp1App*)AfxGetApp();
	auto mfm = (CMainFrame*)AfxGetMainWnd();
	auto d1 = mfm->Dock(IDD_DockScheduleDetail);
	if(d1)
		AfxMessageBox(L"DockScheduleDetail ok");
}




void DockScheduleDetail::OnNMThemeChangedMfcpropertygrid1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// This feature requires Windows XP or greater.
	// The symbol _WIN32_WINNT must be >= 0x0501.
	// TODO: Add your control notification handler code here
	TRACE("%s \n", __FUNCTION__);
	*pResult = 0;
}


void DockScheduleDetail::OnStnClickedMfcpropertygrid1()
{
	TRACE("%s \n", __FUNCTION__);
}


void DockScheduleDetail::OnStnDblclickMfcpropertygrid1()
{
	TRACE("%s \n", __FUNCTION__);
}

void DockScheduleDetail::Refresh(string eStep, ShJObj sjo, int iOp)
{
	TRACE("%s \n", __FUNCTION__);
	if(KwAttrDel(iOp, ePostAsync8))
	{
		_KwBeginInvoke(this, [&, eStep, sjo, iOp]()
			{
				Refresh(eStep, sjo, iOp);
			});
		return;
	}
	else if(!DefaultSync(iOp))
		return;
	//int bRefresh = JObj::IsUpdated(_sjo, sjo, "fOrdShedID");
	/*
		"fNameOrd":"손님1","fNamePet":"견1","fNameStf":"샵관리자-2",
		"fNamePrd":"목욕","fNameOpt":"3mm",
		"fOrdOptID":"order4-00111","fOrdPetID":"order2-0001","fOrdPrdID":"order3-0011",
		"fState":"saved","fTimeUse":"occupy","fSchdType":"request","fWorkStat":"before",
		"fOrdShedID":"ordshed-0006","fOrder":1,"fOrderID":"order1-0001","fPetID":"mypet-0001",
		"fPrdOptID":"propt-5235faa5","fProdID":"prod-5235554e","fScore":4,
		"fUsrIdOrd":"user-0001","fUsrIdStf":"admin-0002",
	*/
// 	if(tchbegin(eStep.c_str(), "sync"))
// 	{
// 		if(!DefaultSync(iOp))
// 			return;
// 	}
	CStringA table;
	CStringA keyUsrID;
	if(sjo->Has("torderschd"))
	{
		table = "torderschd";
		keyUsrID = "fUsrIdOrd";
	}
	else if(sjo->Has("tbizschedule"))
	{
		table = "tbizschedule";
		keyUsrID = "fUsrID";
	}
	else
		return;
	
	_sjo = sjo;
	
	InitPropList(table);
	/// _prData->ToRefreshOption();; 콤보박스형 value에 옵션을 채울 려면
	_prData->SetJObj(sjo, table);
	_cProperty.ExpandAll();

	/// 사용자 상세 창 Refresh
	CString fUsrID;
	bool bOK = sjo->GetArrayItem(table, 0, keyUsrID, fUsrID);
	
	ShJObj sjin;
	if(fUsrID.GetLength() > 0)
	{
		sjin = make_shared<JObj>();///?ex: 고객상세창 Refresh
		(*sjin)("fUsrID") = fUsrID;
	}
	int iOp2 = KwAttrRemove(iOp, eSyncByAction64 | eShowDock1);
	DockCustomDetail::s_me->Refresh("sync", sjin, iOp2 | ePostAsync8);

	if(KwAttr(iOp, eSyncByAction64))
	{
		auto sjoOut = sjo->GetArrayItem(table, 0);
		CString fObjID;
		CString fObjName;
		CString fObjName2;
		CStringA fMemo;
		if(table == L"torderschd")
		{
			fObjName = sjoOut->S("fNameOrd");//Prd
			fObjName2 = sjoOut->S("fNamePet");//Prd
			fObjID = sjoOut->S("fOrdShedID");
			fMemo.Format("예약: %s", sjoOut->SA("fNamePrd"));//
		}
		else if(table == L"tbizschedule")
		{
			fObjName = sjoOut->S("fNickName");
			fObjName2 = sjoOut->S("fNote");
			fObjID = sjoOut->S("fSchdID");
			fMemo = "개인일정";
		}
		AppBase::AddWorkLog(table, "일정", fObjID, "상세", fObjName, nullptr, fMemo);
	}
}


void DockScheduleDetail::InitPropList(ShJObj sjo)
{
	//SetPropListFont();
	//CMFCPropertyGridProperty pp;
	//pp.AllowEdit(TRUE);
//	_cProperty.AllowEdit(TRUE);
	_cProperty.EnableHeaderCtrl(FALSE);
	_cProperty.EnableDescriptionArea();
	_cProperty.SetVSDotNetLook();
	_cProperty.MarkModifiedProperties();

	CStringA table = !sjo ? "init" : sjo->SA("table");
	if(table == _table)
		return;

	if(_prData)
		_prData->RemoveAll();
	else
		_prData = new KProperty(&_cProperty);

	if(table == "torderschd" || table == "init")
	{
		//_cProperty.RemoveAll();

		//KStdMapPtr<string, KPrGrp> _grp;// 그룹명 vs 한아이템

		KProperty& dt = *_prData;
		//dt._pr.SetAt(L"반려동물과사람", new KPrItem("fNameOrd", L"고객 닉네임", "강아지");

		dt.AddGroup("gPetMe", L"반려동물과사람");
		dt.SetField("gPetMe", "fNameOrd", L"닉네임", L"고객 닉네임 입니다.");
		dt.SetField("gPetMe", "fNamePet", L"팻 이름", L"반려동물의 이름 입니다.");
		dt.SetField("gPetMe", "fNameStf", L"담당자", L"예약을 받은 샵의 담당지 닉네임 입니다.");

		dt.AddGroup("gOrder", L"서비스");
		dt.SetField("gOrder", "fNamePrd", L"상품", L"예약한 서비스 상품 입니다.");
		dt.SetField("gOrder", "fNameOpt", L"옵션", L"예약한 서비스 상품의 옵션 입니다.");
		dt.SetField("gOrder", "fBeginTo", L"시작", L"작업 시작 예상 시각 입니다.");
		dt.SetField("gOrder", "fEndTo"  , L"종료", L"작업 종료 예상 시각입니다");

		dt.AddGroup("gState", L"상태/옵션");
		//dt.SetField("gState", "fClassPaid", L"결제방식", L"결제 방식을 나타냅니다.");
		dt.SetField("gState", "fState", L"상황", L"현재 예약 상황을 나타냅니다.");
		dt.SetField("gState", "fTimeUse", L"할당", L"일정 때문에 예약 가능 여부를 나타냅니다.");
		dt.SetField("gState", "fSchdType", L"상태", L"예약 진행 단계를 나타냅니다.");
		dt.SetField("gState", "fWorkStat", L"작업상황", L"작업 진행 상태를 나타냅니다.");
	}
	else if(table == "tbizschedule")
	{
		//KStdMapPtr<string, KPrGrp> _grp;// 그룹명 vs 한아이템
		KProperty& dt = *_prData;
		dt.AddGroup("gSched", L"일정 속성");
		dt.SetField("gSched", "fNote", L"일정내용", L"일정에 관한 내용입니다.");
		dt.SetField("gSched", "fNickName", L"닉네임", L"스탭 닉네임 입니다.");
		dt.SetField("gSched", "fAttr", L"종류", L"전체 또는 개인 일정 종류입니다.");

		dt.AddGroup("gTime", L"일정 시각");
		dt.SetField("gTime", "fOff", L"예약여부", L"시간이 점유되어 예약 가능여부 입니다.");
		dt.SetField("gTime", "fBeginTo", L"시작", L"작업 시작 예상 시각 입니다.");
		dt.SetField("gTime", "fEndTo", L"종료", L"작업 종료 예상 시각입니다");
		dt.SetField("gTime", "fRepeat", L"반복", L"반복 여부 입니다.");

	}
	else
	{
	}
	_table = table;
	/*
	CMFCPropertyGridProperty* prop0 = new CMFCPropertyGridProperty(_T("기본"));

	//prop0->AddSubItem(new CMFCPropertyGridProperty(_T("BOOL 값"), (_variant_t)false, _T("True, False 중 하나")));

	CMFCPropertyGridProperty* sub0 = new CMFCPropertyGridProperty(_T("펫 종류"), _T("개"), _T("개, 고양이, 새, 기타 중 하나를 지정합니다."));
	sub0->SetData((DWORD_PTR)"fAnimal");
	sub0->AddOption(_T("개"));
	sub0->AddOption(_T("고양이"));
	sub0->AddOption(_T("새"));
	sub0->AddOption(_T("기타"));
	int nop = sub0->GetOptionCount();
	sub0->AllowEdit(FALSE);
	prop0->AddSubItem(sub0);
	nop = sub0->GetOptionCount();

	CMFCPropertyGridProperty* sub1 = new CMFCPropertyGridProperty(_T("펫 품종"), _T("시츄"), _T("펫 품종을 하나를 지정합니다."));
	sub1->SetData((DWORD_PTR)"fBreed");
	sub1->AddOption(_T("시츄"));
	sub1->AddOption(_T("차와와"));
	sub1->AddOption(_T("푸들"));
	sub1->AddOption(_T("진돗개"));
	sub1->AddOption(_T("기타"));
	sub1->AllowEdit(FALSE);
	prop0->AddSubItem(sub1);

	CMFCPropertyGridProperty* sub2 = new CMFCPropertyGridProperty(_T("펫 이름"), (_variant_t)_T("주줄이"), _T("펫의 이름을 입력합니다."));
	LPCSTR key0 = "fName";// (LPCSTR)CStringA("fName");
	sub2->SetData((DWORD_PTR)key0);//원래 상수 문자열 써야 한다.
	prop0->AddSubItem(sub2);
	_cProperty.AddProperty(prop0);



	CMFCPropertyGridProperty* prop1 = new CMFCPropertyGridProperty(_T("신체"));// , 0, TRUE);

	sub0 = new CMFCPropertyGridProperty(_T("나이(개월)"), (_variant_t)24l, _T("태어난지 생월수로 입력합니다."));
	sub0->EnableSpinControl(TRUE, 1, 200);
	prop1->AddSubItem(sub0);

	sub0 = new CMFCPropertyGridProperty(_T("체중(Kg)"), (_variant_t)L"2.5", _T("체중을 키로그램으로 입력합니다."));
	//sub0->EnableSpinControl(TRUE, 0, 200); 정수만 가능
	prop1->AddSubItem(sub0);

	_cProperty.AddProperty(prop1);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("글꼴"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	_tcscpy_s(lf.lfFaceName, _T("맑은 고딕"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("글꼴"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("창의 기본 글꼴을 지정합니다.")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("시스템 글꼴을 사용합니다."), (_variant_t)true, _T("창에서 MS Shell Dlg 글꼴을 사용하도록 지정합니다.")));

	_cProperty.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("기타"));
	sub0 = new CMFCPropertyGridProperty(_T("(이름)"), _T("애플리케이션"));
	sub0->Enable(FALSE);
	pGroup3->AddSubItem(sub0);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("창 색상"), RGB(210, 192, 254), nullptr, _T("창의 기본 색상을 지정합니다."));
	pColorProp->EnableOtherButton(_T("기타..."));
	pColorProp->EnableAutomaticButton(_T("기본값"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static const TCHAR szFilter[] = _T("아이콘 파일(*.ico)|*.ico|모든 파일(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("아이콘"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("창 아이콘을 지정합니다.")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("폴더"), _T("c:\\")));

	_cProperty.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("계층"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("첫번째 하위 수준"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("두 번째 하위 수준"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 1"), (_variant_t)_T("값 1"), _T("설명입니다.")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 2"), (_variant_t)_T("값 2"), _T("설명입니다.")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("항목 3"), (_variant_t)_T("값 3"), _T("설명입니다.")));

	pGroup4->Expand(FALSE);
	_cProperty.AddProperty(pGroup4);

	//	int x = _cProperty.GetProperty(0)->GetSubItem(0)->GetValue().iVal;
	int npr = _cProperty.GetPropertyCount();
	for(int i = 0; i < npr; i++)
	{
		auto pr0 = _cProperty.GetProperty(i);
		int nsi = pr0->GetSubItemsCount();
		for(int j = 0; j < nsi; j++)
		{
			auto si00 = pr0->GetSubItem(j);
			const CString& desc = si00->GetDescription();//위에 쓴 설명
			int ilv = si00->GetHierarchyLevel();//1
			auto va01 = si00->GetValue();//
			const COleVariant& oval = si00->GetOriginalValue();//GetValue()와 같다.
			LPCSTR key = (LPCSTR)si00->GetData();// SetData로 넣어둔 값이다. 상수 문자열 가능.
			nop = si00->GetOptionCount();
			//auto op00 = si00->GetOption(0);//개
			//auto op01 = si00->GetOption(1);//고양이
		}
	}
	*/

}
