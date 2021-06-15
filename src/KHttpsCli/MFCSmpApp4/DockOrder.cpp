// DockOrder.cpp : implementation file
//

#include "pch.h"
#include "MFCSmpApp4.h"
#include "DockOrder.h"
#include "Mainfrm.h"
#include "Requ.h"
#include "KwLib64/KDebug.h"
#include "DockTool.h"///?kdw
#include "DockMyPetList.h"
#include "DockProducts.h"

IMPLEMENT_DYNCREATE(DockOrderPane, CPaneForm)
BEGIN_MESSAGE_MAP(DockOrderPane, CPaneForm)
	ON_WM_SETFOCUS()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()
//ON_WM_CONTEXTMENU()

// DockOrder

IMPLEMENT_DYNCREATE(DockOrder, DockClientBase)
DockOrder* DockOrder::s_me = NULL;

DockOrder::DockOrder(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn)
{
	s_me = this;
}

DockOrder::~DockOrder()
{
}

void DockOrder::DoDataExchange(CDataExchange* pDX)
{
	DockClientBase::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_LIST1, _cList1);
	//DDX_Control(pDX, IDC_LIST2, _cList2);
	//DDX_Control(pDX, IDC_LIST3, _cList3);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, _cProperty);
	if(pDX->m_bEditLastControl)
		_cProperty._dock = this;
}

BEGIN_MESSAGE_MAP(DockOrder, DockClientBase)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_ACTIVATE()
	ON_WM_SETTINGCHANGE()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY(HDN_ITEMCLICK, 0, &DockOrder::OnHdnItemclickList1)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &DockOrder::OnLvnColumnclickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &DockOrder::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_Read, &DockOrder::OnBnClickedRead)
	ON_BN_CLICKED(IDC_AddPet, &DockOrder::OnBnClickedAddpet)
	ON_BN_CLICKED(IDC_AddProd, &DockOrder::OnBnClickedAddprod)
	ON_BN_CLICKED(IDC_StartOrder, &DockOrder::OnBnClickedStartorder)
	ON_COMMAND(ID_PopOrder_AddPet, &DockOrder::OnPopOrderAddPet)
	ON_COMMAND(ID_PopOrder_DelPet, &DockOrder::OnPopOrderDelPet)
	ON_COMMAND(ID_PopOrder_AddProd, &DockOrder::OnPopOrderAddProd)
	ON_COMMAND(ID_PopOrder_DelProd, &DockOrder::OnPopOrderDelProd)
	ON_COMMAND(ID_PopOrder_AddOpt, &DockOrder::OnPopOrderAddOpt)
	ON_COMMAND(ID_PopOrder_DelOpt, &DockOrder::OnPopOrderDelOpt)
	ON_COMMAND(ID_AddPet, &DockOrder::OnAddPetRb)
	ON_COMMAND(ID_AddProd, &DockOrder::OnAddProdRb)
	ON_COMMAND(ID_AddOption, &DockOrder::OnAddOptionRb)
	ON_COMMAND(ID_AddSchedule, &DockOrder::OnAddScheduleRb)
	ON_COMMAND(ID_Payment, &DockOrder::OnPaymentRb)
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_DelPet, &DockOrder::OnDelPet)
	ON_COMMAND(ID_DelProd, &DockOrder::OnDelProd)
	ON_COMMAND(ID_DelOption, &DockOrder::OnDelOption)
	ON_UPDATE_COMMAND_UI(ID_PopOrder_DelPet, &DockOrder::OnUpdatePopOrderDelPet)
	ON_UPDATE_COMMAND_UI(ID_PopOrder_AddProd, &DockOrder::OnUpdatePopOrderAddProd)
	ON_UPDATE_COMMAND_UI(ID_PopOrder_DelProd, &DockOrder::OnUpdatePopOrderDelProd)
	ON_UPDATE_COMMAND_UI(ID_PopOrder_AddOpt, &DockOrder::OnUpdatePopOrderAddOpt)
	ON_UPDATE_COMMAND_UI(ID_PopOrder_DelOpt, &DockOrder::OnUpdatePopOrderDelOpt)
	ON_UPDATE_COMMAND_UI(ID_AddPet, &DockOrder::OnUpdateAddPet)
	ON_UPDATE_COMMAND_UI(ID_PopOrder_AddPet, &DockOrder::OnUpdatePopOrderAddPet)
END_MESSAGE_MAP()


// DockOrder �����Դϴ�.

#ifdef _DEBUG
void DockOrder::AssertValid() const
{
	DockClientBase::AssertValid();
}

#ifndef _WIN32_WCE
void DockOrder::Dump(CDumpContext& dc) const
{
	DockClientBase::Dump(dc);
}
#endif
#endif //_DEBUG

_STitleWidthField s_arlstOrder1[] =
{
	{ 20, nullptr     , L"#"    ,},
	{ 60, "fName"     , L"�����",},
	{ 70, "fNameBreed", L"ǰ��", },
	{ 40, "fWeight"   , L"ü��",},
};
_STitleWidthField* DockOrder::getListFieldInfo(int* pnCols)
{
	if(pnCols) *pnCols = _countof(s_arlstOrder1);
	return s_arlstOrder1;
}
_STitleWidthField s_arlstOrder2[] =
{
	{ 20, nullptr     , L"#"    ,},
	{ 60, "fName"     , L"�����",},
	{ 70, "fNameBreed", L"ǰ��", },
	{ 40, "fWeight"   , L"ü��",},
};
_STitleWidthField s_arlstOrder3[] =
{
	{ 20, nullptr     , L"#"    ,},
	{ 60, "fName"     , L"�����",},
	{ 70, "fNameBreed", L"ǰ��", },
	{ 40, "fWeight"   , L"ü��",},
};

// DockOrder �޽��� ó�����Դϴ�.
// CBranchVu1 �޽��� ó�����Դϴ�.
void DockOrder::OnInitialUpdate()
{
	if(m_bInited)
		return;
	DockClientBase::OnInitialUpdate();
	m_bInited = TRUE;


	//KwSetListColumn(&_cList1, s_arlstOrder1, _countof(s_arlstOrder1));
	//KwSetListColumn(&_cList2, s_arlstOrder2, _countof(s_arlstOrder2));
	//KwSetListColumn(&_cList3, s_arlstOrder3, _countof(s_arlstOrder3));
//	KwSetListReportStyle(&_cList1);
	ShJObj sjo;
	InitPropList();

	AdjustLayout();
}

void DockOrder::AdjustLayout()
{
	if(GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rc, rcL, rcP;
	GetClientRect(rc);

	int cyTlb = 30;// m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;
	//m_wndObjectCombo.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	//m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	CRect rcLw, rcPw;
	/*
	if(::IsWindow(_cList1.GetSafeHwnd()))// && _cList1.IsWindowVisible()
	{
		_cList1.GetClientRect(rcL);
		_cList1.GetWindowRect(rcLw);
		//+	rcL { LT(0, 0)      RB(241 , 80 )[241 x 80] }	CRect
		//+ rcLw{ LT(1506, 660) RB(1751, 744)[245 x 84] }	CRect �����(screen)���� ��ġ, ũ��� 4�� Ŀ����.
		ScreenToClient(rcLw);

		if(_cList1.GetSafeHwnd() && !_cList1.IsIconic())
			_cList1.SetWindowPos(nullptr, rc.left, rc.top + rcLw.top, rc.Width(), rcLw.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}*/
	if(::IsWindow(_cProperty.GetSafeHwnd()))// && _cProperty.IsWindowVisible()
	{
		_cProperty.GetClientRect(rcP);
		_cProperty.GetWindowRect(rcPw);
		ScreenToClient(rcPw);
		if(_cProperty.GetSafeHwnd())
			_cProperty.SetWindowPos(nullptr, rc.left, rcPw.top, rc.Width(), rc.Height() - (rcPw.top), SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void DockOrder::OnSize(UINT nType, int cx, int cy)
{
	DockClientBase::OnSize(nType, cx, cy);
	AdjustLayout();
}

void DockOrder::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);
}
void DockOrder::OnDockFocus()
{
	//_cProperty.SetFocus();
	auto pfn = (CMainFrame*)AfxGetMainWnd();
	pfn->ContextCategory(ID_CONTEXT_Order);
}

/*
void DockOrder::OnBnClickedButton1()
{
	//auto app = (CMfcSmpApp1App*)AfxGetApp();
	auto mfm = (CMainFrame*)AfxGetMainWnd();
	InitPropList();
	auto d1 = mfm->Dock(IDD_DockOrder);
	//if(d1)
	//	AfxMessageBox(L"DockOrder ok");#ifdef DEBUG
	auto sjo = make_shared<JObj>();
#ifdef DEBUG
	(*sjo)("fOrderID") = L"order1-0001"; /// test��#
#else
	ASSERT(0);
#endif
	Refresh("request", sjo);
}
*/


void DockOrder::InitPropList(ShJObj sjo)
{
	_cProperty.EnableHeaderCtrl(FALSE);
	_cProperty.EnableDescriptionArea();
	_cProperty.SetVSDotNetLook();
	_cProperty.MarkModifiedProperties();
	if(_prData)
		_prData->RemoveAll();
	else
		_prData = new KProperty(&_cProperty);
	KProperty& dt = *_prData;
	dt.AddGroup("gOrder", L"�����( * ǥ�ô� ���� ����)");
	dt.SetField("gOrder", "fTel", L"����ó", L"����� ����ó �Դϴ�.", L"");
	dt.SetField("gOrder", "fNickName", L"������", L"������ �г��� �Դϴ�.", L"");
	dt.SetField("gOrder", "fState", L"�����Ȳ", L"�����Ȳ �Դϴ�.", L"");
	dt.SetField("gOrder", "fEditing", L"��������", L"������ ���� ������ �˷��ݴϴ�.", L"");
	dt.SetField("gOrder", "fPriceTotal", L"���� �հ�", L"��� ��ǰ�� �ɼ� ������ ��ģ �ݾ��Դϴ�.", L"");/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
	dt.SetField("gOrder", "fPriceAgree", L"���� ���Ǳ�", L"���� ������ ���� ������ �ݾ��Դϴ�.", L"", TRUE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);

	if(!sjo)
	{
		InitPropertyPet(dt, 1);
		//dt.AddGroup("gPet1", L"�� (1)");
		//dt.SetField("gPet1", "fNamePet", L"�� �̸�", L"����� �Դϴ�.", L"", FALSE);
		//dt.SetField("gPet1", "fNameBr", L"ǰ��", L"������ �ݷ����� �����Դϴ�.", L"", FALSE);
		//dt.SetField("gPet1", "fWeight", L"����(Kg)", L"ü���� Kg���� ǥ�� �մϴ�.", 0., FALSE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
		//dt.SetField("gPet1", "fAgeMonth", L"����(����)", L"�¾�� �������� ǥ�� �մϴ�.", (long)0, FALSE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
		//dt.SetField("gPet1", "fGender", L"����", L"������ ǥ�� �մϴ�.", L"", FALSE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
		//dt.SetField("gPet1", "fNeuSurg", L"�߼�ȭ", L"�������θ� ǥ�� �մϴ�.", L"", FALSE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
		//dt.SetField("gPet1", "fBrdSzCD", L"������", L"����� ǥ�� �մϴ�.", L"", FALSE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);

		InitPropertyProduct(dt, 1, 1);

		InitPropertyOption(dt, 1, 1, 1);
		//InitPropertyOption(dt, 1, 1, 2);

		//InitPropertyProduct(dt, 1, 2);
		//dt.AddGroup("gPet1/gPrd2", L"��ǰ(2)");
		//dt.SetField("gPet1/gPrd2", "fNamePrd", L"��ǰ��", L"��ǰ������ ǥ�� �մϴ�.", L"", FALSE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
		//dt.SetField("gPet1/gPrd2", "fProdTpCD", L"��ǰ����", L"��ǰ������ ǥ�� �մϴ�.", L"", FALSE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
		//dt.SetField("gPet1/gPrd2", "fKind", L"�з�", L"��ǰ������ ǥ�� �մϴ�.", L"", FALSE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
		//dt.SetField("gPet1/gPrd2", "fElapsed", L"�ɸ��½ð�", L"��ǰ������ ǥ�� �մϴ�.", (long)0, FALSE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
		//dt.SetField("gPet1/gPrd2", "fDesc", L"��", L"��ǰ������ ǥ�� �մϴ�.", L"", FALSE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);

		//InitPropertyOption(dt, 1, 2, 1);
		//InitPropertyOption(dt, 1, 2, 2);
		//dt.AddGroup("gPet1/gPrd2/gOpt2", L"�ɼ�(2)");
		//dt.SetField("gPet1/gPrd2/gOpt2", "fNameOpt", L"�ɼǸ�", L"ü���� Kg���� ǥ�� �մϴ�.", L"", TRUE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
		//dt.SetField("gPet1/gPrd2/gOpt2", "fType", L"Ÿ��", L"ü���� Kg���� ǥ�� �մϴ�.", (long)0, TRUE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
		//dt.SetField("gPet1/gPrd2/gOpt2", "fExtraTime", L"�߰��ð�(��)", L"ü���� Kg���� ǥ�� �մϴ�.", L"", TRUE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
		//dt.SetField("gPet1/gPrd2/gOpt2", "fPrice", L"�߰�����", L"ü���� Kg���� ǥ�� �մϴ�.", 0., TRUE);/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
	}
	else
	{
		for(int i = 1;; i++)
		{
			CStringA k2; k2.Format("gPet%d", i);
			if(sjo->Has((PAS)k2))
			{
				InitPropertyPet(dt, i);
				auto sPet = sjo->O(k2);
				for(int j = 1;; j++)
				{
					CStringA k3; k3.Format("gPrd%d", j);
					if(sPet->Has((PAS)k3))
					{
						InitPropertyProduct(dt, i, j);
						auto sPrd = sPet->O(k3);
						for(int k = 1;; k++)
						{
							CStringA k4; k4.Format("gOpt%d", k);
							if(sPrd->Has((PAS)k4))
							{
								InitPropertyOption(dt, i, j, k);
								//auto sOpt = sPrd->O(k4);
							}
							else break;
						}
					}
					else break;
				}
			}
			else break;
		}
	}
	_cProperty.ExpandAll();
}
void DockOrder::InitPropertyPet(KProperty& dt, int iPet)
{
	CStringA kgr; kgr.Format("gPet%d", iPet);
	CStringW title; title.Format(L"��(%d)", iPet);
	dt.AddGroup(kgr, title);
	dt.SetField(kgr, "fNamePet", L"�� �̸�", L"����� �Դϴ�.", L"");
	dt.SetField(kgr, "fNameBr", L"ǰ��", L"������ �ݷ����� �����Դϴ�.", L"");
	dt.SetField(kgr, "fWeight", L"����(Kg)", L"ü���� Kg���� ǥ�� �մϴ�.", L"");/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
	dt.SetField(kgr, "fAgeMonth", L"����(����)", L"�¾�� �������� ǥ�� �մϴ�.", (long)0);
	dt.SetField(kgr, "fGender", L"����", L"������ ǥ�� �մϴ�.", L"");
	dt.SetField(kgr, "fNeuSurg", L"�߼�ȭ", L"�������θ� ǥ�� �մϴ�.", L"");
	dt.SetField(kgr, "fBrdSzCD", L"������", L"����� ǥ�� �մϴ�.", L"");
}

void DockOrder::InitPropertyProduct(KProperty& dt, int iPet, int iPrd)
{
	CStringA kgr; kgr.Format("gPet%d/gPrd%d", iPet, iPrd);
	CStringW title; title.Format(L"��ǰ(%d)", iPrd);
	dt.AddGroup(kgr, title);
	dt.SetField(kgr, "fNamePrd", L"��ǰ��", L"��ǰ������ ǥ�� �մϴ�.", L"");
	dt.SetField(kgr, "fProdTpCD", L"��ǰ����", L"��ǰ���� ǥ�� �մϴ�.", L"");
	dt.SetField(kgr, "fKind", L"�з�", L"�з� ǥ�� �մϴ�.", L"");
	dt.SetField(kgr, "fElapsed", L"�۾��ð�(��)", L"�۾��ð��� ������ ǥ�� �մϴ�.", (long)0);
	dt.SetField(kgr, "fPricePrd", L"����", L"������� ü���� ����� ������ ǥ�� �մϴ�.", L"");
	dt.SetField(kgr, "fDesc", L"��", L"�� ǥ�� �մϴ�.", L"");
}

void DockOrder::InitPropertyOption(KProperty& dt, int iPet, int iPrd, int iOpt)
{
	CStringA kgr; kgr.Format("gPet%d/gPrd%d/gOpt%d", iPet, iPrd, iOpt);
	CStringW title; title.Format(L"�ɼ�(%d)", iOpt);
	dt.AddGroup(kgr, title);
	dt.SetField(kgr, "fNameOpt", L"�ɼǸ�", L"�ɼǸ��� ǥ�� �մϴ�.", L"");
	dt.SetField(kgr, "fType", L"Ÿ��", L"�ɼ�Ÿ�� ǥ�� �մϴ�.", L"");
	dt.SetField(kgr, "fExtraTime", L"�߰��ð�(��)", L"�߰��ð��� ������ ǥ�� �մϴ�.", (long)0);
	dt.SetField(kgr, "fPriceOpt", L"�߰�����", L"�߰������� ǥ�� �մϴ�.", L"");
}




void DockOrder::Refresh(string eStep, ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, eStep);
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
		ShJObj sjin = sjo;
		RM_Function("SelectFullOrder", sjin);
		return;
	}
	else if(eStep == "new_order")
	{
		ASSERT(0);//?deprecated see AddPet("request"..)
		//StartBlink();
		VERIFY(DefaultSync(iOp));
		_eStatEdit = "new";
		RM_Function("AddOrder1stNew", sjo, iOp);
	}
	else if(eStep == "request")
	{
		RM_Function("SelectFullOrder", sjo);
	}
	else if(eStep == "list")
	{
		FOREGROUND();
		ShJObj sjout = sjo;/// _sjo �� ����.

		//if(sjout->Has("tenum"))
		//	_sjaEnum = sjo->Array("tenum");//�ʵ庰 �ڵ�� ��¿빮�ڿ� ������ ì���.

		if(sjout->Has("gOrder"))
		{
			if(_sortKey.empty())
				_sortKey = "fName";
			//_sjaTable.reset();
			auto sjaTable = sjout->Array("table");/// _sjo �� ����.
			MakeSort(sjaTable);
			InsertDataToList();

			InitPropList();//û��
		}
	}
	else if(eStep == "output_detail")
	{
		//if(sjo->Has("tenum"))
		//	_sjaEnum = sjo->Array("tenum");//�ʵ庰 �ڵ�� ��¿빮�ڿ� ������ ì���.
		ShJObj torder = sjo->O("torder");
		//ShJArr sjaEnum = sjo->OA("torder", "tenum");
		//InitPropList(torder);
		ShJArr sjaEnum = sjo->Array("tenum");
		InitPropList(torder);
		_prData->SetJObj(torder, sjaEnum);
		_cProperty.ExpandAll();
		//CString fPetID = sjo->S("fPetID");
		//for(auto& sjv : *_sjaTable)
		//{
		//	auto sjoRow = sjv->AsObject();
		//	if(sjoRow->size() == 0)
		//		_break;
		//	if(fPetID == sjoRow->S("fPetID"))
		//	{
		//		//InitPropList();
		//		_prData->SetJObj(sjoRow, _sjaEnum);
		//		_cProperty.ExpandAll();
		//		break;
		//	}
		//}
	}
}


//?deprecated
/*
void DockOrder::StartOrder(ShJObj sjo, int iOp)
{
	StartBlink();

	_eStatEdit = "new";

	auto sjoin = make_shared<JObj>();
	sjoin->Copy(*sjo, "fUsrIdOrd", "fUsrID");
	sjoin->CopyIf(*sjo, "fPetID");
	//(*sjoin)("fUsrID") = fUsrID;// L"user-0002";
	ASSERT(!fUsrID.IsEmpty());
	RM_AddOrder1stNew(sjoin, iOp);
	// RM_AddOrder1stNew �ϰ� �о� ���� "edit"���� �ٲ��
	// OnPropertyChanged ���� �ʵ忡 ���� RM_UpdateOrder �� �Ѵ�.
}
*/
void DockOrder::NewOrder(PAS eStep, ShJObj sjoin, int iOp)
{
	if(KwAttrDel(iOp, ePostAsync8))
	{
		_KwBeginInvoke(this, [&, eStep, sjoin, iOp]()
			{	NewOrder(eStep, sjoin, iOp);	});
		return;
	}
	else if(!DefaultSync(iOp))
		return;

	if(tchsame(eStep, "request"))
	{
		if(_sjo)///�ٸ��� ������ �̸� û���ϰ� ���� �Ѵ�. ���� ������ �α׿� �ִ�.
		{
			InitPropList();
			_sjo.reset();
		}

		_eStatEdit = "new";
		ShJObj sjoIn = make_shared<JObj>();
		sjoIn->CopyIf(*sjoin, "fPetID");
		sjoIn->Copy(*sjoin, "fUsrID");
		RM_Function("AddOrder1stNew", sjoIn, iOp);
	}
}

BOOL DockOrder::AddPet(PAS eStep, ShJObj sjoin, int iOp)
{
	//if(!_sjo) �� ���� ���� ������ 
	//	return;
	if(KwAttrDel(iOp, ePostAsync8))
	{
		_KwBeginInvoke(this, [&, eStep, sjoin, iOp]()
			{
				AddPet(eStep, sjoin, iOp);
			});
		return 0;
	}
	else if(!DefaultSync(iOp))
		return 0;


	ShJObj gOrder;
	if(_sjo)
	{
		gOrder = _sjo->OO("torder", "gOrder");
		if(!gOrder)
			return 0;
		else
			if(KwAttr(iOp, eUpdateUI512))
				return TRUE;
	}
	if(KwAttr(iOp, eUpdateUI512))
		return FALSE;

	
	if(tchsame(eStep, "request"))/// ��¥ ���� ��û �ϱ�
	{
		/*	// pet�� � �ֳ�
		int i = 1;
		for(auto& [k, sjv] : *_sjo)
		{
			CString kp; kp.Format(L"gPet%d", i);
			auto sjoPet = sjv->AsObject();
			if(sjoPet->Has(kp))
				i++;
		}
		CString kp; kp.Format(L"gPet%d", i);
		(*_sjo)(kp) = sjoPet;
		*/

		if(gOrder)
		{
			auto sjoEdit = make_shared<JObj>();
			sjoEdit->Copy(*gOrder, "fOrderID");
			sjoEdit->CopyIf(*sjoin, "fUsrID");
			sjoEdit->CopyIf(*sjoin, "fPetID");
			RM_Function("AddOrder2ndPet", sjoEdit, iOp);///?����: ���� ���� �ȵ�� ����
		}
		else
		{
			AfxMessageBox(L"���� ���� ���� ���� �ƴմϴ�.\n�� ������ ���� ���� �ϰų� ���� ������ ���� �Ͻʽÿ�.");
			return 0;
		}
	}
	else if(tchsame(eStep, "open_list"))/// �� �� �߰� �ϱ� ���ؼ��� �긮��Ʈ â�� ����.
	{
		//auto mfm = (CMainFrame*)AfxGetMainWnd();
		//DockTool::ShowHide(mfm, *mfm->Pane(IDD_DockMyPetList), TRUE);
		CString fUsrID;
		if(gOrder->Has("fUsrIdOrd"))
		{
			ShJObj sjoIn = make_shared<JObj>();///?ex: ����â Refresh
			sjoIn->Copy(*gOrder, "fUsrID", "fUsrIdOrd");//������ ���
			sjoIn->Copy(*gOrder, "fNickName");
			sjoIn->Copy(*gOrder, "fTel");
			DockMyPetList::s_me->Refresh("request by Order", sjoIn, iOp | eShowDock1 | ePostAsync8);
		}
	}
	return 0;
}

BOOL DockOrder::AddProd(PAS eStep, ShJObj sjoin, int iOp)
{
	//if(!_sjo) �� ���� ���� ������ 
	//	return;
	if(KwAttrDel(iOp, ePostAsync8))
	{
		_KwBeginInvoke(this, [&, eStep, sjoin, iOp]()
			{	AddProd(eStep, sjoin, iOp);	});
		return 0;
	}
	else if(!DefaultSync(iOp))
		return 0;

	CStringA fk = GetPrGridFullKey(&_cProperty);
	CStringA gPetK;
	if(fk.GetLength() > 0)
	{
		if(tchbegin((PAS)fk, "gPet")) // 'gPet'���� ���� �ϸ�
		{
			if(KwAttr(iOp, eUpdateUI512))
				return TRUE;

			gPetK = tchprecede((PAS)fk, '/').Pas(); // '/'�տ� �κ��� �� ����.
		}
	}
	if(gPetK.IsEmpty())
	{
		if(KwAttr(iOp, eUpdateUI512))
			return FALSE;
		AfxMessageBox(L"��ǰ �߰��� ���� ���� �ϼ���.");
		return 0;
	}

	ShJObj gPet;
	if(_sjo)
	{
		gPet = _sjo->OO("torder", gPetK);
		if(!gPet)
			return 0;
	}

	if(tchsame(eStep, "request"))/// ��¥ ���� ��û �ϱ�
	{
		auto gOrder = _sjo->OO("torder", "gOrder");
		if(gOrder && gPet)
		{
			auto sjoEdit = make_shared<JObj>();
			sjoEdit->Copy(*gOrder, "fOrderID");
			sjoEdit->Copy(*gPet, "fOrdPetID");
			sjoEdit->CopyIf(*sjoin, "fProdID");
			RM_Function("AddOrder3rdProduct", sjoEdit, iOp);///?����: ���� ���� �ȵ�� ����
		}
		else
			AfxMessageBox(L"���� ���� ���� ���� �ƴմϴ�.\n�� ������ ���� ���� �ϰų� ���� ������ ���� �Ͻʽÿ�.");
	}
	else if(tchsame(eStep, "list"))/// �� �� �߰� �ϱ� ���ؼ��� �긮��Ʈ â�� ����.
	{
		//auto mfm = (CMainFrame*)AfxGetMainWnd();
		//DockTool::ShowHide(mfm, *mfm->Pane(IDD_DockMyPetList), TRUE);
		auto gOrder = _sjo->OO("torder", "gOrder");

		CString fUsrID;
		if(gOrder && gPet)//->Has("fOrdPetID"))
		{
			ShJObj sjoIn = make_shared<JObj>();///?ex: ����â Refresh
			sjoIn->Copy(*gOrder, "fBizID");//��ǰ�� �� Ű�� ����Ʈ
			//sjoIn->Copy(*gPet, "fOrdPetID");
			DockProducts::s_me->Refresh("request", sjoIn, iOp | eShowDock1);//ePostAsync8
		}
	}
	return 0;
}

BOOL DockOrder::AddOption(PAS eStep, ShJObj sjoin, int iOp)
{
	if(KwAttrDel(iOp, ePostAsync8))
	{
		_KwBeginInvoke(this, [&, eStep, sjoin, iOp]()
			{
				AddOption(eStep, sjoin, iOp);
			});
		return 0;
	}
	else if(!DefaultSync(iOp))
		return 0;

	CStringA fk = GetPrGridFullKey(&_cProperty);
	CStringA gPetK;
	if(fk.GetLength() > 0)
	{
		if(tchbegin((PAS)fk, "gOpt")) // 'gPet'���� ���� �ϸ�
		{
			if(KwAttr(iOp, eUpdateUI512))
				return TRUE;

			gPetK = tchprecede((PAS)fk, '/').Pas(); // '/'�տ� �κ��� �� ����.
		}
	}
	if(gPetK.IsEmpty())
	{
		if(KwAttr(iOp, eUpdateUI512))
			return FALSE;
		AfxMessageBox(L"�ɼ� �߰��� ��ǰ�� ���� �ϼ���.");
		return 0;
	}

	ShJObj gPet;
	if(_sjo)
	{
		gPet = _sjo->OO("torder", gPetK);
		if(!gPet)
			return 0;
	}

	if(tchsame(eStep, "request"))/// ��¥ ���� ��û �ϱ�
	{
		auto gOrder = _sjo->OO("torder", "gOrder");
		if(gOrder && gPet)
		{
			auto sjoEdit = make_shared<JObj>();
			sjoEdit->Copy(*gOrder, "fOrderID");
			sjoEdit->Copy(*gPet, "fOrdPrdID");
			sjoEdit->CopyIf(*sjoin, "fProdID");
			RM_Function("AddOrder4thOption", sjoEdit, iOp);///?����: ���� ���� �ȵ�� ����
		}
		else
			AfxMessageBox(L"���� ���� ���� ���� �ƴմϴ�.\n�� ������ ���� ���� �ϰų� ���� ������ ���� �Ͻʽÿ�.");
	}
	else if(tchsame(eStep, "list"))/// �� �� �߰� �ϱ� ���ؼ��� �긮��Ʈ â�� ����.
	{
		//auto mfm = (CMainFrame*)AfxGetMainWnd();
		//DockTool::ShowHide(mfm, *mfm->Pane(IDD_DockMyPetList), TRUE);
		auto gOrder = _sjo->OO("torder", "gOrder");

		CString fUsrID;
		if(gOrder && gPet)//->Has("fOrdPetID"))
		{
			ShJObj sjoIn = make_shared<JObj>();///?ex: ����â Refresh
			sjoIn->Copy(*gOrder, "fBizID");//��ǰ�� �� Ű�� ����Ʈ
			//sjoIn->Copy(*gPet, "fOrdPetID");
			DockProducts::s_me->Refresh("request", sjoIn, iOp | eShowDock1);//ePostAsync8
		}
	}
	return 0;
}
/*BOOL DockOrder::DelPet(int iOp)
{
	TRACE("%s\n", __FUNCTION__);

	CPoint point;
	GetCursorPos(&point);
	CPoint pointc = point;
	ScreenToClient(&pointc);//{x=-3366 y=272}
	CRect rccPr;
	_cProperty.GetClientRect(&rccPr);
	CRect rcPr;
	_cProperty.GetWindowRect(&rcPr);//�������Ϳ��� ��ġ
	CMFCPropertyGridProperty* sel = _cProperty.GetCurSel();// const { return m_pSel; }
	if(sel)
	{
		int k = (int)sel->GetData();
		string fk = _prData->FindFullkey(k);
		auto kpr = _prData->FindKProperty(k);
		CString v = sel->GetValue();
		string kpet;
		if(tchbegin(fk.c_str(), "gPet"))
		{
			if(KwAttr(iOp, eUpdateUI512))
				return TRUE;
			auto ds = fk.find('/');
			if(ds != __1)//>= 0)
				kpet = fk.substr(0, ds);
			else
				kpet = fk;
		}
		else if(KwAttr(iOp, eUpdateUI512))
			return TRUE;

		if(kpet.length() > 0)
		{
			if(!KwAttr(iOp, eNoCheckMsgBox16))
			{
				if(AfxMessageBox(L"���࿡�� ���׸��� �����ϸ� �� �Ʒ� ��ǰ�� �ɼ� �׸��� ���� ���� ��� ���� �˴ϴ�.\n���� �Ͻðڽ��ϱ�?", MB_YESNO) != IDYES)
					return 0;
			}

			auto sjoGOrder = _sjo->OO("torder", "gOrder");
			auto sjoPet = _sjo->OO("torder", kpet);
			if(sjoPet && sjoGOrder)
			{
				auto sjoIn = make_shared<JObj>();
				sjoIn->Copy(*sjoGOrder, "fOrderID");
				sjoIn->Copy(*sjoPet, "fOrdPetID");
				RM_Function("DeleteOrder2ndPet", sjoPet);
			}
		}
	}

	CRect rctPr;
	KwGetCtrlRect(this, &_cProperty, &rctPr);///rctPr	{LT(0, 52) RB(370, 817)  [370 x 765]}
	return 0;
}*/
BOOL DockOrder::DelPet(int iOp)
{
	TRACE("%s\n", __FUNCTION__);
	return DelItem("pet", iOp);
}

BOOL DockOrder::DelProd(int iOp)
{
	return DelItem("product", iOp);
}

BOOL DockOrder::DelOption(int iOp)
{
	TRACE("%s\n", __FUNCTION__);
	return DelItem("option", iOp);
	//CMFCPropertyGridProperty* sel = _cProperty.GetCurSel();// const { return m_pSel; }
	//if(sel)
	//{
	//	int k = (int)sel->GetData();
	//	string fk = _prData->FindFullkey(k);
	//	auto kpr = _prData->FindKProperty(k);
	//	CString v = sel->GetValue();
	//	string kpet;
	//	if(tchbegin(fk.c_str(), "gOpt"))
	//	{
	//		if(KwAttr(iOp, eUpdateUI512))
	//			return TRUE;
	//		auto ds = fk.find('/');
	//		if(ds != __1)//>= 0)
	//			kpet = fk.substr(0, ds);
	//		else
	//			kpet = fk;
	//	}
	//	if(KwAttr(iOp, eUpdateUI512))
	//		return FALSE;//���̻� ���� ���ϰ�

	//	if(kpet.length() > 0)
	//	{
	//		if(!KwAttr(iOp, eNoCheckMsgBox16))
	//		{
	//			if(AfxMessageBox(L"���࿡�� ��ǰ�� �����ϸ� �� �Ʒ� �ɼǵ� ��� ���� �˴ϴ�.\n���� �Ͻðڽ��ϱ�?", MB_YESNO) != IDYES)
	//				return 0;
	//		}

	//		auto sjoGOrder = _sjo->OO("torder", "gOrder");
	//		auto sjoPet = _sjo->OO("torder", kpet);
	//		if(sjoPet && sjoGOrder)
	//		{
	//			auto sjoIn = make_shared<JObj>();
	//			//sjoIn->Copy(*sjoGOrder, "fOrderID");
	//			sjoIn->Copy(*sjoPet, "fOrdPetID");
	//			RM_Function("DeleteOrder3rdProduct", sjoPet);
	//		}
	//	}
	//}

	//CRect rctPr;
	//KwGetCtrlRect(this, &_cProperty, &rctPr);///rctPr	{LT(0, 52) RB(370, 817)  [370 x 765]}
	return 0;
}

BOOL DockOrder::DelItem(CStringA item, int iOp)
{
	TRACE("%s\n", __FUNCTION__);
	//CPoint point;
	//GetCursorPos(&point);
	//CPoint pointc = point;
	//ScreenToClient(&pointc);//{x=-3366 y=272}
	//CRect rccPr;
	//_cProperty.GetClientRect(&rccPr);
	//CRect rcPr;
	//_cProperty.GetWindowRect(&rcPr);//�������Ϳ��� ��ġ
	CMFCPropertyGridProperty* sel = _cProperty.GetCurSel();// const { return m_pSel; }
	if(sel)
	{
		int k = (int)sel->GetData();
		string fk = _prData->FindFullkey(k);
		__int64 ik = -1;
		char* pth = nullptr;
		if(KwAttr(iOp, eUpdateUI512))
		{
			if(item == "pet")
				pth = "gPet";
			else if(item == "product")
				pth = "/gPrd";
			else if(item == "option")
				pth = "/gOpt";
			else ASSERT(0);

			ik = tchstrx(fk.c_str(), pth);
			return ik >= 0;//���̻� ���� ���ϰ�
		}
		
		//auto kpr = _prData->FindKProperty(k);
		//CString v = sel->GetValue();
		string kpet;
		string kprd;
		string kopt;

		auto dsPet = fk.find('/');
		auto dsPrd = fk.find('/', dsPet + 1);
		auto dsOpt = fk.find('/', dsPrd + 1);
		if(dsPet != __1)//>= 0)
		{
			kpet = fk.substr(0, dsPet);

			if(dsPrd != __1)//__1)
			{
				kprd = fk.substr(dsPet + 1, dsPrd - dsPet - 1);
				if(dsOpt != __1)
					kopt = fk.substr(dsOpt + 1, dsOpt - dsPrd - 1);
				else
					kopt = fk.substr(dsOpt + 1);
			}
			else
				kprd = fk.substr(dsPet + 1);
		}
		else
			kpet = fk;


		auto sjoGOrder = _sjo->OO("torder", "gOrder");
		if(!sjoGOrder)
			return 0;
		ShJObj sjoPet;
		ShJObj sjoPrd;
		ShJObj sjoOpt;

		if(tchbegin(kpet.c_str(), "gPet"))//.length() > 0)
		{
			sjoPet = _sjo->OO("torder", kpet);
			if(sjoPet)
			{
				if(tchbegin(kprd.c_str(), "gPrd"))//.length() > 0)
				{
					sjoPrd = sjoPet->O(kprd);
					if(sjoPrd)
					{
						if(tchbegin(kopt.c_str(), "gOpt")) // kopt.length() > 0)
							sjoOpt = sjoPrd->O(kopt);
					}
				}
			}
		}


		CStringW msg;
		if(item == "pet")
		{
			//if(sjoPet->Has("gPrd1"))
				msg = L"���࿡�� �� �׸��� �����ϸ� �� �Ʒ��� ��ǰ, �ɼǰ� �������� ��� ���� �˴ϴ�.";
		}
		else if(item == "product")
		{
			//if(sjoPrd->Has("gOpt1"))
				msg = L"���࿡�� ��ǰ�� �����ϸ� �� �Ʒ� �ɼǰ� ������ ��� ���� �˴ϴ�.";
		}
		else if(item == "option")
		{
			msg = L"���࿡�� �ɼ��� �����ϸ� �ɼǰ� ����� ������ ���� �˴ϴ�.";
		}

		if(!KwAttr(iOp, eNoCheckMsgBox16))
		{
			CString s1; s1.Format(L"%s\n���� �Ͻðڽ��ϱ�?", msg);
#ifndef _DEBUG
			if(AfxMessageBox(s1, MB_YESNO) != IDYES)
				return 0;
#endif
		}

		auto sjoIn = make_shared<JObj>();
		CStringA sFunc;
		sjoIn->Copy(*sjoGOrder, "fOrderID");
		if(item == "pet")
		{
			if(sjoPet)
			{
				sjoIn->Copy(*sjoPet, "fOrdPetID");
				sFunc = "DeleteOrder2ndPet";
			}
		}
		else if(item == "product")
		{
			if(sjoPrd)
			{
				sjoIn->Copy(*sjoPrd, "fOrdPrdID");
				sFunc = "DeleteOrder3rdProduct";
			}
		}
		else if(item == "option")
		{
			if(sjoOpt)
			{
				sjoIn->Copy(*sjoPrd, "fOrdOptID");
				sFunc = "DeleteOrder4thOption";
			}
		}
		else ASSERT(0);

		if(sFunc.GetLength() > 0)
			RM_Function(sFunc, sjoIn);
	}
	//CRect rctPr;
	//KwGetCtrlRect(this, &_cProperty, &rctPr);///rctPr	{LT(0, 52) RB(370, 817)  [370 x 765]}
	return 0;
}


void DockOrder::WorkLogOrder(ShJObj sjo, PAS fMode, PAS fMemo)
{
	auto gOrder = sjo->OO("torder", "gOrder");
	//auto sjo1 = sjo->GetArrayItem("torder", 0);// , "gOrder");
	//auto sjo2 = sjo1->O("gOrder");
	AppBase::AddWorkLog("torder1", "����", gOrder->S("fOrderID"), fMode, gOrder->S("fNickName"), gOrder->S("fTel"), fMemo);
}
/*
void DockOrder::RM_SelectFullOrder(ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]() { RM_SelectFullOrder(sjo, iOp); });
		return;
	}
	BACKGROUND(1);
	JObj jin, jout;
	KRequ req;
	jin.Copy(*sjo, "fOrderID");//������ ���
	jin("fBizID") = req._BizID;

	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if(!jout.SameS("Return", L"OK"))
		return;
	_eStatEdit = "edit";
	ShJObj sjout = make_shared<JObj>(jout);
	_sjo = sjout;
	Refresh("output_detail", sjout, iOp | ePostAsync8);
}

void DockOrder::RM_AddOrder1stNew(ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		CMainPool::QueueFunc([&, sjo, iOp]() { RM_AddOrder1stNew(sjo, iOp); });
		return;
	}
	BACKGROUND(1);
	JObj jin, jout;
	KRequ req;//��û�� ���
	jin("fBizID") = req._BizID;//������ ����� �翬 �� �� �Ҽ� �̰���
	jin("fUsrIdUpt") = req._UsrID;
	jin.Copy(*sjo, "fUsrIdOrd", "fUsrID");
	jin.CopyIf(*sjo, "fPetID");//������ ��

	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if(!jout.SameS("Return", L"OK"))
		return;

	_eStatEdit = "edit";
	ShJObj sjout = make_shared<JObj>(jout);
	_sjo = sjout;

	WorkLogOrder(_sjo, "���λ���");

	Refresh("output_detail", sjout, iOp | ePostAsync8);
}

void DockOrder::RM_AddOrder2ndPet(ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		CMainPool::QueueFunc([&, sjo, iOp]() { RM_AddOrder2ndPet(sjo, iOp); });
		return;
	}
	BACKGROUND(1);
	JObj jin, jout;
	KRequ req;
	jin("fUsrIdUpt") = req._UsrID;
	jin.Copy(*sjo, "fOrderID");//������ ���
	jin.Copy(*sjo, "fPetID");//������ ��

	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if(!jout.SameS("Return", L"OK"))
		return;
	//ShJObj sjout = make_shared<JObj>(jout);
	_sjo = make_shared<JObj>(jout);;
	
	WorkLogOrder(_sjo, "update", "�� �߰�");
	
	Refresh("output_detail", _sjo, iOp | ePostAsync8);
}


void DockOrder::RM_AddOrder3rdProduct(ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		CMainPool::QueueFunc([&, sjo, iOp]() { RM_AddOrder3rdProduct(sjo, iOp); });
		return;
	}
	BACKGROUND(1);
	JObj jin, jout;
	KRequ req;
	jin("fUsrIdUpt") = req._UsrID;
	
	//auto gOrder = _sjo->OO("torder", "gOrder");
	//jin.Copy(*gOrder, "fOrderID");//������ ���
	
	jin.Copy(*sjo, "fOrdPetID");//������ ���
	jin.Copy(*sjo, "fProdID");//������ ��

	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if(!jout.SameS("Return", L"OK"))
		return;
	//ShJObj sjout = make_shared<JObj>(jout);
	_sjo = make_shared<JObj>(jout);

	WorkLogOrder(_sjo, "����", "��ǰ �߰�");

	Refresh("output_detail", _sjo, iOp | ePostAsync8);
}



void DockOrder::RM_DeleteOrder2ndPet(ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		CMainPool::QueueFunc([&, sjo, iOp]() { RM_DeleteOrder2ndPet(sjo, iOp); });
		return;
	}
	BACKGROUND(1);
	JObj jin, jout;
	KRequ req;
	jin("fUsrIdUpt") = req._UsrID;
	//jin.Copy(*sjo, "fOrderID");//������ ���
	jin.Copy(*sjo, "fOrdPetID");//������ ��

	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if(!jout.SameS("Return", L"OK"))
		return;
	ShJObj sjout = make_shared<JObj>(jout);
	_sjo = sjout;
	Refresh("output_detail", sjout, iOp | ePostAsync8);
}

void DockOrder::RM_DeleteOrder3rdProduct(ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		CMainPool::QueueFunc([&, sjo, iOp]() { RM_DeleteOrder3rdProduct(sjo, iOp); });
		return;
	}
	BACKGROUND(1);
	JObj jin, jout;
	KRequ req;
	jin("fUsrIdUpt") = req._UsrID;
	//jin.Copy(*sjo, "fOrderID");//������ ���
	jin.Copy(*sjo, "fOrdProdID");//������ ��

	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if(!jout.SameS("Return", L"OK"))
		return;
	ShJObj sjout = make_shared<JObj>(jout);
	_sjo = sjout;
	Refresh("output_detail", sjout, iOp | ePostAsync8);
}
*/

/// �̰� ȭ�� ���� ��
void DockOrder::OnBnClickedRead()
{
	if(_sjo)
	{
		auto gOrder = _sjo->OO("torder", "gOrder");
		if(gOrder)
			Refresh("request", gOrder);
	}
// 	auto sjo = make_shared<JObj>();
// #ifdef DEBUG
// 	(*sjo)("fOrderID") = L"order1-0001"; /// test��#
// #else
// 	ASSERT(0);
// #endif
// 	Refresh("request", sjo);
}

void DockOrder::OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	//TRACE("%s \n", __FUNCTION__);
	*pResult = 0;
}

void DockOrder::OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	TRACE("%s (%d, %d)\n", __FUNCTION__, pNMLV->iItem, pNMLV->iSubItem);
	SetListSort(_cList1, pNMLV, s_arlstOrder1);

	*pResult = 0;
}




void DockOrder::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	TRACE("%s (%d, %d, %d)\n", __FUNCTION__, pNMLV->iItem, pNMLV->uChanged, pNMLV->uNewState);
	//                                          8                                          2    |    1 = 3 // ���º�ȭ�� �ְ� ���õǾ��ų�, ��Ŀ���� ������ ���
	if(pNMLV->iItem >= 0 && pNMLV->uChanged == LVIF_STATE && pNMLV->uNewState == (LVIS_SELECTED | LVIS_FOCUSED))
	{
		CString fPetID = (PWS)_cList1.GetItemData(pNMLV->iItem);// , (DWORD_PTR)sjo->S("fUsrID"));
		if(fPetID.GetLength() > 0)
		{
			ShJObj sjin = make_shared<JObj>();///?ex: ����â Refresh
			(*sjin)("fPetID") = fPetID;
			Refresh("output_detail", sjin, ePostAsync8);
		}
	}
	*pResult = 0;
}


BOOL KPropertyCtrl::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	//TRACE("%s(%u) (%d)\n", __FUNCTION__, nID, nCode);
	return CMFCPropertyGridCtrl::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
BOOL KPropertyCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	//TRACE("%s(%d) (%d : %#X)\n", __FUNCTION__, wParam, lParam, lParam);
	return CMFCPropertyGridCtrl::OnNotify(wParam, lParam, pResult);
}



void KPropertyCtrl::OnPropertyChanged(CMFCPropertyGridProperty* pProp) const
{
	// TODO: Add your specialized code here and/or call the base class
	const COleVariant& var = pProp->GetValue();
	CString val = pProp->FormatProperty();
	/// TextToVar
	/// IsValueChanged
	/// CreateInPlaceEdit
	///"gOrder"
	///  "gPet1/gPrd2/gOpt2", "fOptName22"
	//if(var.vt == VT_BSTR)
	//	val = var.bstrVal;
	//else if(var.vt == VT_I2 || var.vt == VT_I4)
	//	val.Format(L"%d", var.intVal);
	KTrace(L"%s(%d) (%s)\n", __FUNCTIONW__, var.vt, val);
	//TRACE("%s(%d) (%s)\n", __FUNCTION__, var.vt, val);
	_dock->_prData->_sjoCp;

	return CMFCPropertyGridCtrl::OnPropertyChanged(pProp);
}




void DockOrder::OnBnClickedAddpet()
{
	AddPet("open_list");
}

void DockOrder::OnBnClickedAddprod()
{
	TRACE("%s\n", __FUNCTION__);
	AddProd("list");
}


void DockOrder::OnBnClickedStartorder()
{
	//StartOrder(L"user-0002");
	AfxMessageBox(L"���󼼳� �� ��Ͽ��� ���� �� ���� ���� �Ͻʽÿ�.");
}


void DockOrder::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//TRACE("%s\n", __FUNCTION__);
	/// app ���� �̸� �־� ���
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_Order, point.x, point.y, this, TRUE);
	DockClientBase::OnContextMenu(pWnd, point);
}
//void DockOrderPane::OnContextMenu(CWnd* pWnd, CPoint point)
//{
//	//TRACE("%s\n", __FUNCTION__);
//	CPaneForm::OnContextMenu(pWnd, point);
//}


void DockOrder::OnPopOrderAddPet()
{
	TRACE("%s\n", __FUNCTION__);
	AddPet("open_list");
}
void DockOrder::OnPopOrderAddProd()
{
	TRACE("%s\n", __FUNCTION__);
	AddProd("list");
}
void DockOrder::OnPopOrderAddOpt()
{
	TRACE("%s\n", __FUNCTION__);
}

void DockOrder::OnPopOrderDelPet()
{
	DelPet();
}

/*
void DockOrder::DoOrderAddPet(PAS eStep, ShJObj sjo, int iOp)
{
	if(!_sjo)
		return;
	auto gOrder = _sjo->OO("torder", "gOrder");
	if(!gOrder)
		return;

	if(tchsame(eStep, "open_list"))
	{
		auto mfm = (CMainFrame*)AfxGetMainWnd();
		DockTool::ShowHide(mfm, *mfm->Pane(IDD_DockMyPetList), TRUE);

		CString fUsrID;
		if(gOrder->Has("fUsrIdOrd"))
		{
			ShJObj sjoIn = make_shared<JObj>();///?ex: ����â Refresh
			sjoIn->Copy(*gOrder, "fUsrID", "fUsrIdOrd");//������ ���
			_KwBeginInvoke(this, [&, sjoIn]() // foreground �۾� ť
				{
					DockMyPetList::s_me->Refresh("request by Order", sjoIn);
				});
		}
	}
	else if(tchsame(eStep, "request"))
	{
		ShJObj sjoIn = make_shared<JObj>();///?ex: ����â Refresh
		sjoIn->Copy(*gOrder, "fOrderID");
		RM_AddOrder2ndPet(sjoIn);

	}

}*/

void DockOrder::OnPopOrderDelProd()
{
	TRACE("%s\n", __FUNCTION__);
	DelProd();
}
void DockOrder::OnPopOrderDelOpt()
{
	TRACE("%s\n", __FUNCTION__);
}


void DockOrder::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	DockClientBase::OnActivate(nState, pWndOther, bMinimized);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void DockOrderPane::OnSetFocus(CWnd* pOldWnd)
{
	TRACE("%s\n", __FUNCTION__);
	CPaneForm::OnSetFocus(pOldWnd);
}

void DockOrderPane::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	TRACE("%s\n", __FUNCTION__);
	CPaneForm::OnActivate(nState, pWndOther, bMinimized);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void DockOrderPane::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	TRACE("%s\n", __FUNCTION__);
	CPaneForm::OnActivateApp(bActive, dwThreadID);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

void DockOrder::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	TRACE("%s\n", __FUNCTION__);
	DockClientBase::OnActivateApp(bActive, dwThreadID);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void DockOrder::OnAddPetRb()
{
	TRACE("%s\n", __FUNCTION__);
	AddPet("open_list");
}


void DockOrder::OnAddProdRb()
{
	TRACE("%s\n", __FUNCTION__);
	AddProd("list");
}


void DockOrder::OnAddOptionRb()
{
	TRACE("%s\n", __FUNCTION__);
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void DockOrder::OnAddScheduleRb()
{
	TRACE("%s\n", __FUNCTION__);
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}


void DockOrder::OnPaymentRb()
{
	TRACE("%s\n", __FUNCTION__);
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
}




void DockOrder::OnDelPet()
{
	DelPet();
}


void DockOrder::OnDelProd()
{
	DelProd();
}


void DockOrder::OnDelOption()
{
	DelOption();
}
void DockOrder::OnUpdateAddPet(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


void DockOrder::OnUpdatePopOrderAddPet(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


void DockOrder::OnUpdatePopOrderDelPet(CCmdUI* pCmdUI)
{
	BOOL b = DelPet(eUpdateUI512 | eNoBlinkBmp2);
	//CMFCPropertyGridProperty* sel = _cProperty.GetCurSel();// const { return m_pSel; }
	//if(sel)
	//{
	//	int k = (int)sel->GetData();
	//	string fk = _prData->FindFullkey(k);
	//	b = tchbegin(fk.c_str(), "gPet");
	//}
	pCmdUI->Enable(b);
}


void DockOrder::OnUpdatePopOrderAddProd(CCmdUI* pCmdUI)
{
	BOOL b = AddProd(nullptr, nullptr, eUpdateUI512 | eNoBlinkBmp2);
	pCmdUI->Enable(b);
}


void DockOrder::OnUpdatePopOrderDelProd(CCmdUI* pCmdUI)
{
	BOOL b = DelProd(eUpdateUI512 | eNoBlinkBmp2);
	pCmdUI->Enable(b);
}


void DockOrder::OnUpdatePopOrderAddOpt(CCmdUI* pCmdUI)
{
	BOOL b = AddOption(nullptr, nullptr, eUpdateUI512 | eNoBlinkBmp2);
	pCmdUI->Enable(b);
}


void DockOrder::OnUpdatePopOrderDelOpt(CCmdUI* pCmdUI)
{
	BOOL b = DelOption(eUpdateUI512 | eNoBlinkBmp2);
	pCmdUI->Enable(b);
}


void DockOrder::RM_Function(CStringA sFunc, ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sFunc, sjo, iOp]() { RM_Function(sFunc, sjo, iOp); });
		return;
	}
	BACKGROUND(1);
	JObj jin, jout;
	KRequ req;
	jin("fUsrIdUpt") = req._UsrID;

	if(sFunc == "SelectFullOrder")
	{
		jin.Copy(*sjo, "fOrderID");
		jin("fBizID") = req._BizID;
	}
	else if(sFunc == "AddOrder1stNew")
	{
		jin("fBizID") = req._BizID;//������ ����� �翬 �� �� �Ҽ� �̰���
		jin.Copy(*sjo, "fUsrIdOrd", "fUsrID");
		jin.CopyIf(*sjo, "fPetID");//������ ��. ���� ���� �ִ�. so CopyIf
	}
	else if(sFunc == "AddOrder2ndPet")
	{
		jin.Copy(*sjo, "fOrderID");
		jin.Copy(*sjo, "fPetID");//������ ��
	}
	else if(sFunc == "AddOrder3rdProduct")
	{
		jin.Copy(*sjo, "fOrderID");//��ü �о� �ö� �ʿ�
		jin.Copy(*sjo, "fOrdPetID");//������ ��
		jin.Copy(*sjo, "fProdID");//������ ��ǰ
	}
	else if(sFunc == "AddOrder4thOption")
	{
		jin.Copy(*sjo, "fOrderID");//��ü �о� �ö� �ʿ�
		jin.Copy(*sjo, "fOrdPetID");//������ ��
		jin.Copy(*sjo, "fProdID");//������ ��ǰ
	}
	else if(sFunc == "DeleteOrder2ndPet")/// Ű�� ����ũ �ϴϱ� �������� �߰� Ű�� �ʿ� ����.
	{
		jin.Copy(*sjo, "fOrderID");//��ü �о� �ö� �ʿ�
		jin.Copy(*sjo, "fOrdPetID");//������ ��
	}
	else if(sFunc == "DeleteOrder3rdProduct")
	{
		jin.Copy(*sjo, "fOrderID");//��ü �о� �ö� �ʿ�
		jin.Copy(*sjo, "fOrdPrdID");//������ ��ǰ
	}
	else if(sFunc == "DeleteOrder4thOption")
	{
		jin.Copy(*sjo, "fOrderID");//��ü �о� �ö� �ʿ�
		jin.Copy(*sjo, "fOrdOptID");//������ �ɼ�
	}
	else
	{
		ASSERT(0);
	}

	req.RequestPost(sFunc, jin, jout);//"GetTodaySchdule"
	if(!jout.SameS("Return", L"OK"))
		return;

	_eStatEdit = "edit";
	ShJObj sjout = make_shared<JObj>(jout);
	_sjo = sjout;

	Refresh("output_detail", sjout, iOp | ePostAsync8);
}
