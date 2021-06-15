#include "pch.h"
#include "Mainfrm.h"
#include "DockCustomDetail.h" //?dock declare: Formview resource를 만들고 class 를 등록 한다.
#include "resource.h"
#include "Requ.h"
#include "DockMyPetList.h"
#include "DockOrder.h"

IMPLEMENT_DYNCREATE(DockCustomDetailPane, CPaneForm)//?dock declare

// DockCustomDetail

IMPLEMENT_DYNCREATE(DockCustomDetail, DockClientBase)//?dock declare
DockCustomDetail* DockCustomDetail::s_me = NULL;

DockCustomDetail::DockCustomDetail(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn) //DockCustomDetail::IDD IDD_DockCustomDetail
{
	s_me = this;
}

DockCustomDetail::~DockCustomDetail()
{
}

void DockCustomDetail::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, _cProperty);
}

BEGIN_MESSAGE_MAP(DockCustomDetail, DockClientBase)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_NOTIFY(NM_THEMECHANGED, IDC_MFCPROPERTYGRID1, &DockCustomDetail::OnNMThemeChangedMfcpropertygrid1)
	ON_STN_CLICKED(IDC_MFCPROPERTYGRID1, &DockCustomDetail::OnStnClickedMfcpropertygrid1)
	ON_STN_DBLCLK(IDC_MFCPROPERTYGRID1, &DockCustomDetail::OnStnDblclickMfcpropertygrid1)
	ON_BN_CLICKED(IDC_BUTTON1, &DockCustomDetail::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &DockCustomDetail::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_StartOrder, &DockCustomDetail::OnBnClickedStartOrder)
	ON_BN_CLICKED(IDC_MyPetList, &DockCustomDetail::OnBnClickedMyPetList)
END_MESSAGE_MAP()


// DockCustomDetail 진단입니다.

#ifdef _DEBUG
void DockCustomDetail::AssertValid() const
{
	__super::AssertValid();
}

#ifndef _WIN32_WCE
void DockCustomDetail::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}
#endif
#endif //_DEBUG


// DockCustomDetail 메시지 처리기입니다.
// CBranchVu1 메시지 처리기입니다.
void DockCustomDetail::OnInitialUpdate()
{
	if(m_bInited)
		return;
	__super::OnInitialUpdate();
	m_bInited = TRUE;


	InitPropList();

	AdjustLayout();

}

void DockCustomDetail::AdjustLayout()
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

void DockCustomDetail::OnSize(UINT nType, int cx, int cy)
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

void DockCustomDetail::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);
}
void DockCustomDetail::OnDockFocus()
{
	_cProperty.SetFocus();
	auto pfn = (CMainFrame*)AfxGetMainWnd();
	pfn->ContextCategory(ID_CONTEXT_CustomDetail);
}
void DockCustomDetail::OnBnClickedButton1()
{
	//auto app = (CMfcSmpApp1App*)AfxGetApp();
	auto mfm = (CMainFrame*)AfxGetMainWnd();
	auto d1 = mfm->Dock(IDD_DockCustomDetail);
	if(d1)
		AfxMessageBox(L"CustomDetail ok");
}



void DockCustomDetail::InitPropList(ShJObj sjo)
{
	//SetPropListFont();
	//CMFCPropertyGridProperty pp;
	//pp.AllowEdit(TRUE);
//	_cProperty.AllowEdit(TRUE);
	_cProperty.EnableHeaderCtrl(FALSE);
	_cProperty.EnableDescriptionArea();
	_cProperty.SetVSDotNetLook();
	_cProperty.MarkModifiedProperties();

	if(_prData)
		_prData->RemoveAll();
	else
		_prData = new KProperty(&_cProperty);

	KProperty& dt = *_prData;
	dt.AddGroup("gUsr", L"고객");
	dt.SetField("gUsr", "fNickName", L"닉네임", L"닉네임 입니다.");
	dt.SetField("gUsr", "fTel", L"전화번호", L"핸드폰 번호 입니다.");
	dt.SetField("gUsr", "fTel2", L"전화번호2", L"두번째 전화번호 입니다.");
	dt.SetField("gUsr", "fEmail", L"이메일", L"이메일 주소 입니다.");
	dt.AddGroup("gEtc", L"기타정보");
	dt.SetField("gEtc", "fAppVer", L"앱 버전", L"핸드폰 앱 버전 입니다.");
	dt.SetField("gEtc", "fModel", L"폰 모델", L"앱을 설치한 폰의 모델명 입니다.");
	//select u.fUsrID, u.fTel, u.fEmail, u.fNickName, u.fAppVer, u.fModel, u.fPush, u.fProj from tuser u where u.fUsrID = @fUsrID;");

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


void DockCustomDetail::OnNMThemeChangedMfcpropertygrid1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// This feature requires Windows XP or greater.
	// The symbol _WIN32_WINNT must be >= 0x0501.
	// TODO: Add your control notification handler code here
	TRACE("%s \n", __FUNCTION__);
	*pResult = 0;
}


void DockCustomDetail::OnStnClickedMfcpropertygrid1()
{
	TRACE("%s \n", __FUNCTION__);
}


void DockCustomDetail::OnStnDblclickMfcpropertygrid1()
{
	TRACE("%s \n", __FUNCTION__);
}


void DockCustomDetail::OnBnClickedButton2()
{
	ShJObj sjo;// = ShJObj(new JObj);
	RM_SelectCustomDetail(sjo);

}
void DockCustomDetail::RM_SelectCustomDetail(ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]()	
			{
				RM_SelectCustomDetail(sjo, iOp);			
			});
		return;
	}

	BACKGROUND(1);

	JObj jin, jout;

	jin.Copy(*sjo, "fUsrID");

	KRequ req;//요청한 사람
	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if (!tchsame(jout.S("Return"), L"OK"))
		return;
	//auto sjoRow = jout.GetArrayItem("table", 0);// array를 구한 후 0번쨰 항
	_sjo = make_shared<JObj>(jout);
	Refresh("output", _sjo, iOp | ePostAsync8);
}

void DockCustomDetail::Refresh(string eStep, ShJObj sjo, int iOp)
{
	if(KwAttrDel(iOp, ePostAsync8))
	{
		_KwBeginInvoke(this, [&, eStep, sjo, iOp]()
			{	Refresh(eStep, sjo, iOp);	});
		return;
	}
	else if(!DefaultSync(iOp))
		return;

	FOREGROUND();
	if(eStep == "sync")
	{
		int bRefresh = JObj::IsUpdated(_sjo, sjo, "fUsrID");

		if(bRefresh == 1)//&& (!_sjo->Has("fUsrID") || )
		{
			_sjo = sjo;
			RM_SelectCustomDetail(sjo, iOp);
		}
		else
		{
			if(bRefresh < 0) // -1, -2 return 하면 지워 졌다는 얘기
			{
				TRACE("%s (%d) \n", __FUNCTION__, eStep);
				if(_sjo)
					_sjo.reset();
				InitPropList();
			
				DockMyPetList::s_me->Refresh("sync");// to clear list
			}
			else if(bRefresh == 0)// 이면 그대로 같다.
			{	
			}
		}
		return;
	}
	TRACE("%s (%d) \n", __FUNCTION__, eStep);
	
	if(eStep == "output")
	{
		InitPropList();
		/// _prData->ToRefreshOption();; 콤보박스형 value에 옵션을 채울 려면
		_prData->SetJObj(sjo, "table");
		_cProperty.ExpandAll();

		if(KwAttr(iOp, eSyncByAction64))
		{
			//CString fUsrID;
			auto sjoU = sjo->GetArrayItem("table", 0);// array를 구한 후 0번쨰 항
			//bool bOK = sjo->GetArrayItem("table", 0, "fUsrID", fUsrID);
			//bool bOK1 = sjo->GetArrayItem("table", 0, "fNickName", fNickName);
			//bool bOK2 = sjo->GetArrayItem("table", 0, "fTel", fTel);
			if(sjoU)
			{
				ShJObj sjoIn = make_shared<JObj>();///?ex: 고객상세창 Refresh
				//(*sjoIn)("fUsrID") = fUsrID;
				sjoIn->Copy(*sjoU, "fUsrID");
				sjoIn->Copy(*sjoU, "fNickName");
				sjoIn->Copy(*sjoU, "fTel");
				iOp = KwAttrRemove(iOp, eSyncByAction64) | eReqNoErrBox4;
				DockMyPetList::s_me->Refresh("sync", sjoIn, iOp | ePostAsync8);

				AppBase::AddWorkLog("tuser", "사용자", sjoIn->S("fUsrID"), "상세보기", sjoIn->S("fNickName"), sjoIn->S("fTel"));
			}
		}
	}
}



void DockCustomDetail::OnBnClickedStartOrder()
{
	auto sjoU = _sjo->GetArrayItem("table", 0);// array를 구한 후 0번쨰 항
	CString v;
	if(sjoU->LenS("fUsrID", v))
	{
		auto sjoin = make_shared<JObj>();
		(*sjoin)("fUsrID") = v;// L"user-0002";
		DockOrder::s_me->NewOrder("request", sjoin, eShowDock1 | ePostAsync8);
		//DockOrder::s_me->AddPet("request", sjoin, eShowDock1);
		//DockOrder::s_me->Refresh("new_order", sjoin, eShowDock1);
	}
}


void DockCustomDetail::OnBnClickedMyPetList()
{
	CString v;
	auto sjoU = _sjo->GetArrayItem("table", 0);// array를 구한 후 0번쨰 항
	if(sjoU)
	{
		if(sjoU->LenS("fUsrID", v))
		{
			auto sjoIn = make_shared<JObj>();
			(*sjoIn)("fUsrID") = v;// L"user-0002";
			sjoIn->Copy(*sjoU, "fNickName");
			sjoIn->Copy(*sjoU, "fTel");
			DockMyPetList::s_me->Refresh("sync", sjoIn, eShowDock1);
		}
	}
	//DockMyPetList::s_me->StartPetManage(v, eShowDock1);
}
