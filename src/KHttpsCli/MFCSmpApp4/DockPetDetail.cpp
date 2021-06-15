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


// DockPetDetail �����Դϴ�.

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


// DockPetDetail �޽��� ó�����Դϴ�.
// CBranchVu1 �޽��� ó�����Դϴ�.
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

	CMFCPropertyGridProperty* prop0 = new CMFCPropertyGridProperty(_T("�⺻"));

	//prop0->AddSubItem(new CMFCPropertyGridProperty(_T("BOOL ��"), (_variant_t)false, _T("True, False �� �ϳ�")));

	CMFCPropertyGridProperty* sub0 = new CMFCPropertyGridProperty(_T("�� ����"), _T("��"), _T("��, �����, ��, ��Ÿ �� �ϳ��� �����մϴ�."));
	sub0->SetData((DWORD_PTR)"fAnimal");
	sub0->AddOption(_T("��"));
	sub0->AddOption(_T("�����"));
	sub0->AddOption(_T("��"));
	sub0->AddOption(_T("��Ÿ"));
	int nop = sub0->GetOptionCount();
	sub0->AllowEdit(FALSE);
	prop0->AddSubItem(sub0);
	nop = sub0->GetOptionCount();

	CMFCPropertyGridProperty* sub1 = new CMFCPropertyGridProperty(_T("�� ǰ��"), _T("����"), _T("�� ǰ���� �ϳ��� �����մϴ�."));
	sub1->SetData((DWORD_PTR)"fBreed");
	sub1->AddOption(_T("����"));
	sub1->AddOption(_T("���Ϳ�"));
	sub1->AddOption(_T("Ǫ��"));
	sub1->AddOption(_T("������"));
	sub1->AddOption(_T("��Ÿ"));
	sub1->AllowEdit(FALSE);
	prop0->AddSubItem(sub1);

	CMFCPropertyGridProperty* sub2 = new CMFCPropertyGridProperty(_T("�� �̸�"), (_variant_t)_T("������"), _T("���� �̸��� �Է��մϴ�."));
	LPCSTR key0 = "fName";// (LPCSTR)CStringA("fName");
	sub2->SetData((DWORD_PTR)key0);//���� ��� ���ڿ� ��� �Ѵ�.
	prop0->AddSubItem(sub2);
	_cProperty.AddProperty(prop0);



	CMFCPropertyGridProperty* prop1 = new CMFCPropertyGridProperty(_T("��ü"));// , 0, TRUE);

	sub0 = new CMFCPropertyGridProperty(_T("����(����)"), (_variant_t)24l, _T("�¾�� �������� �Է��մϴ�."));
	sub0->EnableSpinControl(TRUE, 1, 200);
	prop1->AddSubItem(sub0);

	sub0 = new CMFCPropertyGridProperty(_T("ü��(Kg)"), (_variant_t)L"2.5", _T("ü���� Ű�α׷����� �Է��մϴ�."));
	//sub0->EnableSpinControl(TRUE, 0, 200); ������ ����
	prop1->AddSubItem(sub0);

	_cProperty.AddProperty(prop1);

	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("�۲�"));

	LOGFONT lf;
	CFont* font = CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT));
	font->GetLogFont(&lf);

	_tcscpy_s(lf.lfFaceName, _T("���� ���"));

	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("�۲�"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("â�� �⺻ �۲��� �����մϴ�.")));
	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("�ý��� �۲��� ����մϴ�."), (_variant_t)true, _T("â���� MS Shell Dlg �۲��� ����ϵ��� �����մϴ�.")));

	_cProperty.AddProperty(pGroup2);

	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("��Ÿ"));
	sub0 = new CMFCPropertyGridProperty(_T("(�̸�)"), _T("���ø����̼�"));
	sub0->Enable(FALSE);
	pGroup3->AddSubItem(sub0);

	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("â ����"), RGB(210, 192, 254), nullptr, _T("â�� �⺻ ������ �����մϴ�."));
	pColorProp->EnableOtherButton(_T("��Ÿ..."));
	pColorProp->EnableAutomaticButton(_T("�⺻��"), ::GetSysColor(COLOR_3DFACE));
	pGroup3->AddSubItem(pColorProp);

	static const TCHAR szFilter[] = _T("������ ����(*.ico)|*.ico|��� ����(*.*)|*.*||");
	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("������"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("â �������� �����մϴ�.")));

	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("����"), _T("c:\\")));

	_cProperty.AddProperty(pGroup3);

	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("����"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("ù��° ���� ����"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("�� ��° ���� ����"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("�׸� 1"), (_variant_t)_T("�� 1"), _T("�����Դϴ�.")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("�׸� 2"), (_variant_t)_T("�� 2"), _T("�����Դϴ�.")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("�׸� 3"), (_variant_t)_T("�� 3"), _T("�����Դϴ�.")));

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
			const CString& desc = si00->GetDescription();//���� �� ����
			int ilv = si00->GetHierarchyLevel();//1
			auto va01 = si00->GetValue();//
			const COleVariant& oval = si00->GetOriginalValue();//GetValue()�� ����.
			LPCSTR key = (LPCSTR)si00->GetData();// SetData�� �־�� ���̴�. ��� ���ڿ� ����.
			nop = si00->GetOptionCount();
			//auto op00 = si00->GetOption(0);//��
			//auto op01 = si00->GetOption(1);//�����
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
		sjoin = make_shared<JObj>();///?ex: ����â Refresh
		(*sjoin)("fUsrID") = u;
		(*sjoin)("fPetID") = p;
		DockOrder::s_me->AddPet("request", sjoin, eShowDock1);
		//DockOrder::s_me->Refresh("new_order", sjoin, eShowDock1);
	}
	// TODO: Add your control notification handler code here
}
