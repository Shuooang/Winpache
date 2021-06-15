#include "pch.h"
#include "Mainfrm.h"
#include "DockOrder.h"
#include "DockMyPetList.h"
#include "resource.h"
#include "Requ.h"
//?kdw 

IMPLEMENT_DYNCREATE(DockMyPetListPane, CPaneForm)

// DockMyPetList

IMPLEMENT_DYNCREATE(DockMyPetList, DockClientBase)
DockMyPetList* DockMyPetList::s_me = NULL;

DockMyPetList::DockMyPetList(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn) //DockMyPetList::IDD IDD_DockMyPetList
{
	s_me = this;
}

DockMyPetList::~DockMyPetList()
{
}

void DockMyPetList::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, _cList);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, _cProperty);
}


BEGIN_MESSAGE_MAP(DockMyPetList, DockClientBase)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &DockMyPetList::OnBnClickedButton1)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_BN_CLICKED(IDC_Read, &DockMyPetList::OnBnClickedRead)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &DockMyPetList::OnHdnItemclickList1)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &DockMyPetList::OnLvnColumnclickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &DockMyPetList::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_AddToOrder, &DockMyPetList::OnBnClickedAddToOrder)
	ON_BN_CLICKED(IDC_AddMyPet, &DockMyPetList::OnBnClickedAddMyPet)
	ON_BN_CLICKED(IDC_NewOrder, &DockMyPetList::OnBnClickedNeworder)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// DockMyPetList �����Դϴ�.

#ifdef _DEBUG
void DockMyPetList::AssertValid() const
{
	__super::AssertValid();
}

#ifndef _WIN32_WCE
void DockMyPetList::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}
#endif
#endif //_DEBUG

_STitleWidthField s_arlstMyPet[] =
{
	{ 20, nullptr     , L"#"    ,},
	{ 60, "fName"     , L"�����",},
	{ 70, "fNameBreed", L"ǰ��", },
	{ 40, "fWeight"   , L"ü��",},
	{ 50, "fBrdSzCD"  , L"ũ��",},
//	{ 0, "fOrder"  , NULL,},
};
_STitleWidthField* DockMyPetList::getListFieldInfo(int* pnCols)
{
	if(pnCols) *pnCols = _countof(s_arlstMyPet);
	return s_arlstMyPet;
}

// DockMyPetList �޽��� ó�����Դϴ�.
// CBranchVu1 �޽��� ó�����Դϴ�.
void DockMyPetList::OnInitialUpdate()
{
	if(m_bInited)
		return;
	__super::OnInitialUpdate();
	m_bInited = TRUE;


	KwSetListColumn(&_cList, s_arlstMyPet, _countof(s_arlstMyPet));
	InitPropList();

	AdjustLayout();
	KwSetListReportStyle(&_cList);
}

void DockMyPetList::AdjustLayout()
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

	if(::IsWindow(_cList.GetSafeHwnd()))// && _cList.IsWindowVisible()
	{
		_cList.GetClientRect(rcL);
		_cList.GetWindowRect(rcLw);
		//+	rcL { LT(0, 0)      RB(241 , 80 )[241 x 80] }	CRect
		//+ rcLw{ LT(1506, 660) RB(1751, 744)[245 x 84] }	CRect �����(screen)���� ��ġ, ũ��� 4�� Ŀ����.
		ScreenToClient(rcLw);

		if(_cList.GetSafeHwnd() && !_cList.IsIconic())
			_cList.SetWindowPos(nullptr, rc.left, rc.top + rcLw.top, rc.Width(), rcLw.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}
	if(::IsWindow(_cProperty.GetSafeHwnd()))// && _cProperty.IsWindowVisible()
	{
		_cProperty.GetClientRect(rcP);
		_cProperty.GetWindowRect(rcPw);
		ScreenToClient(rcPw);
		if(_cProperty.GetSafeHwnd())
			_cProperty.SetWindowPos(nullptr, rc.left, rcPw.top, rc.Width(), rc.Height() - (rcPw.top), SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void DockMyPetList::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	AdjustLayout();
}

void DockMyPetList::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);
}
void DockMyPetList::OnDockFocus()
{
	//_cProperty.SetFocus();
	auto pfn = (CMainFrame*)AfxGetMainWnd();
	pfn->ContextCategory(ID_CONTEXT_MyPets);
}

void DockMyPetList::OnBnClickedButton1()
{
	//auto app = (CMfcSmpApp1App*)AfxGetApp();
	auto mfm = (CMainFrame*)AfxGetMainWnd();
	auto d1 = mfm->Dock(IDD_DockMyPetList);
	if(d1)
		AfxMessageBox(L"DockMyPetList ok");
}


void DockMyPetList::InitPropList(ShJObj sjo)
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
	dt.AddGroup("gUsr", L"�� ����");
	dt.SetField("gUsr", "fName", L"�� �̸�", L"����� �Դϴ�.");
	dt.SetField("gUsr", "fNameBreed", L"ǰ��", L"������ �ݷ����� �����Դϴ�.");
	dt.SetField("gUsr", "fWeight", L"����(Kg)", L"ü���� Kg���� ǥ�� �մϴ�.");/// 0. �ߴٰ�, �Ҽ� �Ʒ� 0 �� ���� ���ͼ� ���ڿ��� ��� , 0.);
	dt.AddGroup("gEtc", L"��Ÿ����");
	dt.SetField("gEtc", "fBrdSzCD", L"������", L"�̸��� �ּ� �Դϴ�.");
	dt.SetField("gEtc", "fAgeMonth", L"����(��������)", L"�¾�� ���� ���� ǥ�� �մϴ�.", 0);
	dt.SetField("gEtc", "fGender", L"����", L"���� ������ ǥ�� �մϴ�.");
	dt.SetField("gEtc", "fNeuSurg", L"�߼�ȭ", L"�߼�ȭ ���� ���� �Դϴ�.");
	dt.SetField("gEtc", "fRegNo", L"��Ϲ�ȣ", L"��������� ����� �Ϸù�ȣ �Դϴ�.");
	dt.SetField("gEtc", "fMemo", L"�� �޸�", L"������ ������ �޸� �Դϴ�.");/// �Ϲ� �ۿ� ������ ū�ϳ�

}
/*
void DockMyPetList::Refresh(string eStep, CString fPetID)
{
	ShJObj sjin = make_shared<JObj>();///?ex: ����â Refresh
	(*sjin)("fPetID") = fPetID;
	this->Refresh("output_detail", sjin);
}
*/

void DockMyPetList::Refresh(string eStep, ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, eStep);

	if(KwAttrDel(iOp, ePostAsync8))//tchsame(tchbehind(eStep.c_str(), '/').Pas(), "post"))
	{/// �ڰ� /post �̸� �񵿱� �޽��� ť�� �־� ���� ȣ�� �Ѵ�.
		_KwBeginInvoke(this, [&, eStep, sjo, iOp]()
			{/// /post�� �� �� �κи� ����� �Ѵ�.
				Refresh(eStep, sjo, iOp); //tchprecede(eStep.c_str(), '/').Pas()
			});
		return;
	}
	else if(!DefaultSync(iOp))
		return;
	FOREGROUND();

	if(tchbegin(eStep.c_str(), "sync"))
	{
		CString s;
		if(!sjo)
			ClearList();
		else
		{
			s.Format(L"�����: %s (%s)", sjo->S("fNickName"), sjo->S("fTel"));
			SetDlgItemText(IDC_STATIC_top, s);
		
			ShJObj sjin = sjo;
			RM_SelectPetList(sjin, iOp);
		}
		return;
	}
	else if(tchbegin(eStep.c_str(), "request"))
	{
		ShJObj sjin = sjo;
		if(eStep == "request by CustomDetail")
			iOp |= eReqNoErrBox4;
		RM_SelectPetList(sjin, iOp);
		return;
	}
	else if(eStep == "list")
	{
		FOREGROUND();
		if(!tchsame(sjo->S("Return"), L"OK"))
			SetDlgItemText(IDC_STATIC_top, L"�����:");

		DoList(_sjo); // == sjo

		if(_cList.GetItemCount() > 0)
		{
			_bNoLogForItemChange = true;
			KwSelectListItemEx(&_cList, 0);// �̰� �ϸ� eSyncByAction64 �� �߻� �Ͽ� WorkLog �ع�����.
		}
				//KwFocusListItem(&_cList, 0);
		//auto isel = KwGetSelectedListItem(&_cList);
		//if(isel >= 0)
		//{
		//	CString fPetID = (PWS)_cList.GetItemData(isel);// , (DWORD_PTR)sjo->S("fUsrID"));
		//	if(fPetID.GetLength() > 0)
		//	{
		//		ShJObj sjin = make_shared<JObj>();
		//		(*sjin)("fPetID") = fPetID;
		//		Refresh("output_detail", sjin, ePostAsync8);
		//	}
		//}
	}
	else if(eStep == "output_detail")
	{
		//auto sjaTable = _sjo->Array("table");
		CString fPetID = sjo->S("fPetID");
		auto sjaTable = _sjo->Array("table");
		ShJObj sjoRow = sjaTable->FindByValue("fPetID", fPetID);
		//ShJObj sjoRow = FindByID(fPetID);
		if(sjoRow)
		{
			auto sjaEnum = _sjo->Array("tenum");
			_prData->SetJObj(sjoRow, sjaEnum);
			_cProperty.ExpandAll();
			if(KwAttr(iOp, eSyncByAction64))
			{
				CString fObjName = sjoRow->S("fName");
				AppBase::AddWorkLog("tmypets", "�� ��", fPetID, "�󼼺���", fObjName);
			}
		}

		//for(auto& sjv : *sjaTable)// ���Ͼ� ��ġ
		//{
		//	auto sjoRow = sjv->AsObject();
		//	if(sjoRow->size() == 0)
		//		_break;
		//	if(fPetID == sjoRow->S("fPetID"))//�߰�
		//	{
		//		_prData->SetJObj(sjoRow, sjaEnum);
		//		_cProperty.ExpandAll();
		//		break;
		//	}
		//}
	}
}

//?deprecated
//ShJObj DockMyPetList::FindByID(PWS fPetID)
//{
//	auto sjaTable = _sjo->Array("table");
//	if(sjaTable)
//	{
//		for(auto& sjv : *sjaTable)// ���Ͼ� ��ġ
//		{
//			auto sjoRow = sjv->AsObject();
//			if(sjoRow->size() == 0)
//				_break;
//			if(sjoRow->SameS("fPetID", fPetID))//�߰�
//				return sjoRow;
//		}
//	}
//	return nullptr;
//}

void DockMyPetList::ClearList(int iOp)
{
	__super::ClearList(iOp);
	//if(KwAttrDel(iOp, ePostAsync8))
	//{
	//	_KwBeginInvoke(this, [&, iOp]()
	//		{
	//			ClearList(iOp);
	//		});
	//	return;
	//}
	//_cList.DeleteAllItems();
	//_data.RemoveAll();
	//InitPropList();
	SetDlgItemText(IDC_STATIC_top, L"�����:");
}

void DockMyPetList::InsertDataToList(bool bHasData)
{
	ClearList();
	if(!bHasData)
		return;
	InsertDataToListImpl("fPetID", "tmypets");
	//auto LdSetItem = [&](int i, int subi, ShJObj sjo)
	//{
	//	if(subi == 1)
	//	{
	//		_data.Add(sjo->S("fPetID"));
	//		_cList.SetItemData(i, (DWORD_PTR)(PWS)_data.GetAt(i));// sjo->S("fPetID"));//��â�� �����Ƿ� ������ ����
	//	}
	//	PAS k = s_arlstMyPet[subi].field;
	//	CString str = sjo->Str(k);
	//	auto tenumpr = _mapEnum[L"tmypets"];
	//	wstring fKR;
	//	CStringW stru;
	//	if(tenumpr && tenumpr->Lookup((PWS)str, fKR))
	//		stru = fKR.c_str();
	//	else
	//		stru = str;
	//	_cList.SetItem(i, subi, LVIF_TEXT, stru, 0, 0, 0, NULL, 0);
	//};

	//InsertAllItemT(_cList, _countof(s_arlstMyPet), LdSetItem);
}

void DockMyPetList::RM_SelectPetList(ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		if(!sjo)
			ClearList();
		else
			CMainPool::QueueFunc([&, sjo, iOp]() 
				{ 
					RM_SelectPetList(sjo, iOp); 
				});
		return;
	}
	BACKGROUND(1);

	JObj jin, jout;
	//_fUsrID = sjo->S("fUsrID");//������ ���
	jin.Copy(*sjo, "fUsrID");
	KRequ req;//��û�� ���
	if((iOp & eReqNoErrBox4))
		req._errBox = false; // cutomDetail ���� ����ȭ �Ұ�� ���� ���� �޽��� �ڽ� �ȶ��.

	jin("fBizID") = req._BizID;//������ ����� �翬 �� �� �Ҽ� �̰���

	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if(!tchsame(jout.S("Return"), L"OK"))
	{
		ClearList(iOp | ePostAsync8);
		return;
	}

	_sjo = make_shared<JObj>(jout);
	auto sjout = _sjo;
	Refresh("list", sjout, iOp | ePostAsync8);
}

void DockMyPetList::OnBnClickedRead()
{
	auto sjo = make_shared<JObj>();
	if(_fUsrID.GetLength() > 0)
	{
		(*sjo)("fUsrID") = _fUsrID;
		Refresh("request", sjo);
	}
}

void DockMyPetList::OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	//TRACE("%s \n", __FUNCTION__);
	*pResult = 0;
}

void DockMyPetList::OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	TRACE("%s (%d, %d)\n", __FUNCTION__, pNMLV->iItem, pNMLV->iSubItem);
	SetListSort(_cList, pNMLV, s_arlstMyPet);

	*pResult = 0;
}


void DockMyPetList::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	TRACE("%s (%d, %d, %d)\n", __FUNCTION__, pNMLV->iItem, pNMLV->uChanged, pNMLV->uNewState);
	//                                          8                                          2    |    1 = 3 // ���º�ȭ�� �ְ� ���õǾ��ų�, ��Ŀ���� ������ ���
	if(pNMLV->iItem >= 0 && pNMLV->uChanged == LVIF_STATE && pNMLV->uNewState == (LVIS_SELECTED | LVIS_FOCUSED))
	{
		CString fPetID = (PWS)_cList.GetItemData(pNMLV->iItem);// , (DWORD_PTR)sjo->S("fUsrID"));
		if(fPetID.GetLength() > 0)
		{
			ShJObj sjin = make_shared<JObj>();///?ex: ����â Refresh
			(*sjin)("fPetID") = fPetID;
			int iOp = ePostAsync8 | eNoBlinkBmp2;
			if(!_bNoLogForItemChange)
				iOp |= eSyncByAction64;
			else
				_bNoLogForItemChange = false;//�ڵ� ����. Refresh list ����

			this->Refresh("output_detail", sjin, iOp);
		}
	}
	*pResult = 0;
}



void DockMyPetList::OnBnClickedAddToOrder()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSel = _cList.GetNextItem(-1, LVNI_SELECTED);
	if(nSel < 0)
		return;
	CString fPetID = (PWS)_cList.GetItemData(nSel);
	if(fPetID.GetLength() > 0)
	{
		ShJObj sjoRow = FindByID(fPetID, "fPetID");
		ShJObj sjoin;
		sjoin = make_shared<JObj>();///?ex: ����â Refresh
		sjoin->Copy(*sjoRow, "fPetID");
		sjoin->Copy(*sjoRow, "fUsrID", "fUsrIdCare");

		DockOrder::s_me->AddPet("request", sjoin, eShowDock1);

		// ��â detail �ٲ�. ���� �Ҷ� �̹� item changed �����ٵ�
// 		_KwBeginInvoke(this, [&, fPetID]() // foreground �۾� ť
// 			{
// 				ShJObj sjin = make_shared<JObj>();///?ex: ����â Refresh
// 				(*sjin)("fPetID") = fPetID;
// 				Refresh("output_detail", sjin);// fPetID);
// 			});
	}
}


void DockMyPetList::OnBnClickedAddMyPet()
{
	if(_fUsrID.GetLength() > 0)
	{
		ShJObj sjo = make_shared<JObj>();
		(*sjo)("fUsrIdCare") = _fUsrID;
		RM_AddMyPet(sjo);
		// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�. AddMyPet
	}
}


void DockMyPetList::RM_AddMyPet(ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		CMainPool::QueueFunc([&, sjo, iOp]() 
			{ 
				RM_AddMyPet(sjo, iOp); 
			});
		return;
	}
	BACKGROUND(1);

	auto sjaTable = _sjo->Array("table");
	int ix = 0;
	PWS  newPet = L"�� ��";
	if(sjaTable)
	{
		for(auto& sjv : *sjaTable)// ���Ͼ� ��ġ
		{
			auto sjoRow = sjv->AsObject();
			if(sjoRow->BeginS("fName", newPet))
			{
				CString fName = sjoRow->S("fName");
				auto sival = fName.Mid(3);
				sival.Trim();
				int iv = _wtoi(sival);
				//swscanf_s(fName, L"�� �� %d", &iv);
				if(ix < iv)
					ix = iv;
			}
		}
	}
	ix++;
	CString fName;
	fName.Format(L"%s %d", newPet, ix);

	JObj jin, jout;
	KRequ req;
	jin("fBizID") = req._BizID;
	jin.Copy(*sjo, "fUsrIdCare");//������ ���
	jin("fName") = fName;

	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if(!jout.SameS("Return", L"OK"))
		return;
	ShJObj sjout = make_shared<JObj>(jout);
	Refresh("output_detail", sjout, iOp | ePostAsync8);
}
/*
void DockMyPetList::StartPetManage(CStringW fUsrID, int iOp)
{
	auto sjoin = make_shared<JObj>();
	(*sjoin)("fUsrID") = fUsrID;// L"user-0002";
	ASSERT(!fUsrID.IsEmpty());
	RM_SelectPetList(sjoin, iOp);
}
*/


void DockMyPetList::OnBnClickedNeworder()
{
	if(!_sjo)
		return;
	auto sjoU = _sjo->GetArrayItem("table", 0);// array�� ���� �� 0���� ��
	CString usr, pet;
	if(sjoU->LenS("fUsrIdCare", usr) && sjoU->LenS("fPetID", pet))
	{
		auto sjoin = make_shared<JObj>();
		//sjoin->Copy(*sjoU, "fUsrID", "fUsrIdCare");// L"user-0002";
		(*sjoin)("fUsrID") = usr;
		(*sjoin)("fPetID") = pet;
		DockOrder::s_me->NewOrder("request", sjoin, eShowDock1);
	}
}


void DockMyPetList::OnShowWindow(BOOL bShow, UINT nStatus)
{
	DockClientBase::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}
