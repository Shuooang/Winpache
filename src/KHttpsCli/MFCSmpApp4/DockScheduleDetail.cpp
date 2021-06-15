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


// DockScheduleDetail �����Դϴ�.

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


// DockScheduleDetail �޽��� ó�����Դϴ�.
// CBranchVu1 �޽��� ó�����Դϴ�.
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
		"fNameOrd":"�մ�1","fNamePet":"��1","fNameStf":"��������-2",
		"fNamePrd":"���","fNameOpt":"3mm",
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
	/// _prData->ToRefreshOption();; �޺��ڽ��� value�� �ɼ��� ä�� ����
	_prData->SetJObj(sjo, table);
	_cProperty.ExpandAll();

	/// ����� �� â Refresh
	CString fUsrID;
	bool bOK = sjo->GetArrayItem(table, 0, keyUsrID, fUsrID);
	
	ShJObj sjin;
	if(fUsrID.GetLength() > 0)
	{
		sjin = make_shared<JObj>();///?ex: ����â Refresh
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
			fMemo.Format("����: %s", sjoOut->SA("fNamePrd"));//
		}
		else if(table == L"tbizschedule")
		{
			fObjName = sjoOut->S("fNickName");
			fObjName2 = sjoOut->S("fNote");
			fObjID = sjoOut->S("fSchdID");
			fMemo = "��������";
		}
		AppBase::AddWorkLog(table, "����", fObjID, "��", fObjName, nullptr, fMemo);
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

		//KStdMapPtr<string, KPrGrp> _grp;// �׷�� vs �Ѿ�����

		KProperty& dt = *_prData;
		//dt._pr.SetAt(L"�ݷ����������", new KPrItem("fNameOrd", L"�� �г���", "������");

		dt.AddGroup("gPetMe", L"�ݷ����������");
		dt.SetField("gPetMe", "fNameOrd", L"�г���", L"�� �г��� �Դϴ�.");
		dt.SetField("gPetMe", "fNamePet", L"�� �̸�", L"�ݷ������� �̸� �Դϴ�.");
		dt.SetField("gPetMe", "fNameStf", L"�����", L"������ ���� ���� ����� �г��� �Դϴ�.");

		dt.AddGroup("gOrder", L"����");
		dt.SetField("gOrder", "fNamePrd", L"��ǰ", L"������ ���� ��ǰ �Դϴ�.");
		dt.SetField("gOrder", "fNameOpt", L"�ɼ�", L"������ ���� ��ǰ�� �ɼ� �Դϴ�.");
		dt.SetField("gOrder", "fBeginTo", L"����", L"�۾� ���� ���� �ð� �Դϴ�.");
		dt.SetField("gOrder", "fEndTo"  , L"����", L"�۾� ���� ���� �ð��Դϴ�");

		dt.AddGroup("gState", L"����/�ɼ�");
		//dt.SetField("gState", "fClassPaid", L"�������", L"���� ����� ��Ÿ���ϴ�.");
		dt.SetField("gState", "fState", L"��Ȳ", L"���� ���� ��Ȳ�� ��Ÿ���ϴ�.");
		dt.SetField("gState", "fTimeUse", L"�Ҵ�", L"���� ������ ���� ���� ���θ� ��Ÿ���ϴ�.");
		dt.SetField("gState", "fSchdType", L"����", L"���� ���� �ܰ踦 ��Ÿ���ϴ�.");
		dt.SetField("gState", "fWorkStat", L"�۾���Ȳ", L"�۾� ���� ���¸� ��Ÿ���ϴ�.");
	}
	else if(table == "tbizschedule")
	{
		//KStdMapPtr<string, KPrGrp> _grp;// �׷�� vs �Ѿ�����
		KProperty& dt = *_prData;
		dt.AddGroup("gSched", L"���� �Ӽ�");
		dt.SetField("gSched", "fNote", L"��������", L"������ ���� �����Դϴ�.");
		dt.SetField("gSched", "fNickName", L"�г���", L"���� �г��� �Դϴ�.");
		dt.SetField("gSched", "fAttr", L"����", L"��ü �Ǵ� ���� ���� �����Դϴ�.");

		dt.AddGroup("gTime", L"���� �ð�");
		dt.SetField("gTime", "fOff", L"���࿩��", L"�ð��� �����Ǿ� ���� ���ɿ��� �Դϴ�.");
		dt.SetField("gTime", "fBeginTo", L"����", L"�۾� ���� ���� �ð� �Դϴ�.");
		dt.SetField("gTime", "fEndTo", L"����", L"�۾� ���� ���� �ð��Դϴ�");
		dt.SetField("gTime", "fRepeat", L"�ݺ�", L"�ݺ� ���� �Դϴ�.");

	}
	else
	{
	}
	_table = table;
	/*
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
	*/

}
