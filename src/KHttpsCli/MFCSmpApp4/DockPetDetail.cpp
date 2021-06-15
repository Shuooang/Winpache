#include "pch.h"
#include "Mainfrm.h"
#include "DockPetDetail.h"
#include "DockOrder.h"

#include "resource.h"
//?kdw 

IMPLEMENT_DYNCREATE(DockPetDetailPane, CPaneForm)

// DockPetDetail

IMPLEMENT_DYNCREATE(DockPetDetail, DockClientBase)
DockPetDetail* DockPetDetail::s_me = NULL;

DockPetDetail::DockPetDetail(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn) //DockPetDetail::IDD IDD_DockPetDetail
{
	s_me = this;
}

DockPetDetail::~DockPetDetail()
{
}

void DockPetDetail::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, _cProperty);
}

BEGIN_MESSAGE_MAP(DockPetDetail, DockClientBase)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &DockPetDetail::OnBnClickedButton1)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_NOTIFY(NM_THEMECHANGED, IDC_MFCPROPERTYGRID1, &DockPetDetail::OnNMThemeChangedMfcpropertygrid1)
	ON_STN_CLICKED(IDC_MFCPROPERTYGRID1, &DockPetDetail::OnStnClickedMfcpropertygrid1)
	ON_STN_DBLCLK(IDC_MFCPROPERTYGRID1, &DockPetDetail::OnStnDblclickMfcpropertygrid1)
	ON_BN_CLICKED(IDC_StartOrder, &DockPetDetail::OnBnClickedStartOrder)
END_MESSAGE_MAP()


// DockPetDetail 진단입니다.

#ifdef _DEBUG
void DockPetDetail::AssertValid() const
{
	__super::AssertValid();
}

#ifndef _WIN32_WCE
void DockPetDetail::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}
#endif
#endif //_DEBUG


// DockPetDetail 메시지 처리기입니다.
// CBranchVu1 메시지 처리기입니다.
void DockPetDetail::OnInitialUpdate()
{
	if(m_bInited)
		return;
	__super::OnInitialUpdate();
	m_bInited = TRUE;


	InitPropList();

	AdjustLayout();

}

void DockPetDetail::AdjustLayout()
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

void DockPetDetail::OnSize(UINT nType, int cx, int cy)
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

void DockPetDetail::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);
	_cProperty.SetFocus();
}

void DockPetDetail::OnBnClickedButton1()
{
	//auto app = (CMfcSmpApp1App*)AfxGetApp();
	auto mfm = (CMainFrame*)AfxGetMainWnd();
	auto d1 = mfm->Dock(IDD_DockPetDetail);
	if(d1)
		AfxMessageBox(L"DockPetDetail ok");
}



void DockPetDetail::InitPropList(ShJObj sjo)
{
	//SetPropListFont();
	//CMFCPropertyGridProperty pp;
	//pp.AllowEdit(TRUE);
//	_cProperty.AllowEdit(TRUE);
	_cProperty.EnableHeaderCtrl(FALSE);
	_cProperty.EnableDescriptionArea();
	_cProperty.SetVSDotNetLook();
	_cProperty.MarkModifiedProperties();

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

}


void DockPetDetail::OnNMThemeChangedMfcpropertygrid1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// This feature requires Windows XP or greater.
	// The symbol _WIN32_WINNT must be >= 0x0501.
	// TODO: Add your control notification handler code here
	TRACE("%s \n", __FUNCTION__);
	*pResult = 0;
}


void DockPetDetail::OnStnClickedMfcpropertygrid1()
{
	TRACE("%s \n", __FUNCTION__);
}


void DockPetDetail::OnStnDblclickMfcpropertygrid1()
{
	TRACE("%s \n", __FUNCTION__);
}


void DockPetDetail::OnBnClickedStartOrder()
{
	CString u, p;// = _sjo->S("fUsrID");
	if(_sjo->LenS("fUsrID", u) && _sjo->LenS("fPetID", p))
	{
		ShJObj sjoin;
		sjoin = make_shared<JObj>();///?ex: 고객상세창 Refresh
		(*sjoin)("fUsrID") = u;
		(*sjoin)("fPetID") = p;
		DockOrder::s_me->AddPet("request", sjoin, eShowDock1);
		//DockOrder::s_me->Refresh("new_order", sjoin, eShowDock1);
	}
	// TODO: Add your control notification handler code here
}
