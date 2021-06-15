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


// DockMyPetList 진단입니다.

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
	{ 60, "fName"     , L"펫네임",},
	{ 70, "fNameBreed", L"품종", },
	{ 40, "fWeight"   , L"체중",},
	{ 50, "fBrdSzCD"  , L"크기",},
//	{ 0, "fOrder"  , NULL,},
};
_STitleWidthField* DockMyPetList::getListFieldInfo(int* pnCols)
{
	if(pnCols) *pnCols = _countof(s_arlstMyPet);
	return s_arlstMyPet;
}

// DockMyPetList 메시지 처리기입니다.
// CBranchVu1 메시지 처리기입니다.
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
		//+ rcLw{ LT(1506, 660) RB(1751, 744)[245 x 84] }	CRect 모니터(screen)에서 위치, 크기는 4씩 커진다.
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
	dt.AddGroup("gUsr", L"펫 정보");
	dt.SetField("gUsr", "fName", L"펫 이름", L"펫네임 입니다.");
	dt.SetField("gUsr", "fNameBreed", L"품종", L"견종등 반려동물 종류입니다.");
	dt.SetField("gUsr", "fWeight", L"무게(Kg)", L"체중을 Kg으로 표시 합니다.");/// 0. 했다가, 소수 아래 0 이 많이 나와서 문자열로 사용 , 0.);
	dt.AddGroup("gEtc", L"기타정보");
	dt.SetField("gEtc", "fBrdSzCD", L"사이즈", L"이메일 주소 입니다.");
	dt.SetField("gEtc", "fAgeMonth", L"나이(생월기준)", L"태어난지 개월 수를 표시 합니다.", 0);
	dt.SetField("gEtc", "fGender", L"성별", L"원래 성별을 표시 합니다.");
	dt.SetField("gEtc", "fNeuSurg", L"중성화", L"중성화 수술 여부 입니다.");
	dt.SetField("gEtc", "fRegNo", L"등록번호", L"공공기관에 등록한 일련번호 입니다.");
	dt.SetField("gEtc", "fMemo", L"샵 메모", L"샵에만 나오는 메모 입니다.");/// 일반 앱에 나오면 큰일나

}
/*
void DockMyPetList::Refresh(string eStep, CString fPetID)
{
	ShJObj sjin = make_shared<JObj>();///?ex: 고객상세창 Refresh
	(*sjin)("fPetID") = fPetID;
	this->Refresh("output_detail", sjin);
}
*/

void DockMyPetList::Refresh(string eStep, ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, eStep);

	if(KwAttrDel(iOp, ePostAsync8))//tchsame(tchbehind(eStep.c_str(), '/').Pas(), "post"))
	{/// 뒤가 /post 이면 비동기 메시지 큐에 넣어 예약 호출 한다.
		_KwBeginInvoke(this, [&, eStep, sjo, iOp]()
			{/// /post를 뺀 앞 부분만 재귀콜 한다.
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
			s.Format(L"사용자: %s (%s)", sjo->S("fNickName"), sjo->S("fTel"));
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
			SetDlgItemText(IDC_STATIC_top, L"사용자:");

		DoList(_sjo); // == sjo

		if(_cList.GetItemCount() > 0)
		{
			_bNoLogForItemChange = true;
			KwSelectListItemEx(&_cList, 0);// 이거 하면 eSyncByAction64 가 발생 하여 WorkLog 해버린다.
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
				AppBase::AddWorkLog("tmypets", "내 펫", fPetID, "상세보기", fObjName);
			}
		}

		//for(auto& sjv : *sjaTable)// 리니어 서치
		//{
		//	auto sjoRow = sjv->AsObject();
		//	if(sjoRow->size() == 0)
		//		_break;
		//	if(fPetID == sjoRow->S("fPetID"))//발견
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
//		for(auto& sjv : *sjaTable)// 리니어 서치
//		{
//			auto sjoRow = sjv->AsObject();
//			if(sjoRow->size() == 0)
//				_break;
//			if(sjoRow->SameS("fPetID", fPetID))//발견
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
	SetDlgItemText(IDC_STATIC_top, L"사용자:");
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
	//		_cList.SetItemData(i, (DWORD_PTR)(PWS)_data.GetAt(i));// sjo->S("fPetID"));//상세창이 없으므로 쓸일이 없다
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
	//_fUsrID = sjo->S("fUsrID");//선택한 사람
	jin.Copy(*sjo, "fUsrID");
	KRequ req;//요청한 사람
	if((iOp & eReqNoErrBox4))
		req._errBox = false; // cutomDetail 에서 동기화 할경우 서버 에러 메시지 박스 안뜬다.

	jin("fBizID") = req._BizID;//선택한 사람은 당연 이 샵 소속 이겠지

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
	//                                          8                                          2    |    1 = 3 // 상태변화가 있고 선택되었거나, 포커스가 가있을 경우
	if(pNMLV->iItem >= 0 && pNMLV->uChanged == LVIF_STATE && pNMLV->uNewState == (LVIS_SELECTED | LVIS_FOCUSED))
	{
		CString fPetID = (PWS)_cList.GetItemData(pNMLV->iItem);// , (DWORD_PTR)sjo->S("fUsrID"));
		if(fPetID.GetLength() > 0)
		{
			ShJObj sjin = make_shared<JObj>();///?ex: 고객상세창 Refresh
			(*sjin)("fPetID") = fPetID;
			int iOp = ePostAsync8 | eNoBlinkBmp2;
			if(!_bNoLogForItemChange)
				iOp |= eSyncByAction64;
			else
				_bNoLogForItemChange = false;//자동 선택. Refresh list 에서

			this->Refresh("output_detail", sjin, iOp);
		}
	}
	*pResult = 0;
}



void DockMyPetList::OnBnClickedAddToOrder()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSel = _cList.GetNextItem(-1, LVNI_SELECTED);
	if(nSel < 0)
		return;
	CString fPetID = (PWS)_cList.GetItemData(nSel);
	if(fPetID.GetLength() > 0)
	{
		ShJObj sjoRow = FindByID(fPetID, "fPetID");
		ShJObj sjoin;
		sjoin = make_shared<JObj>();///?ex: 고객상세창 Refresh
		sjoin->Copy(*sjoRow, "fPetID");
		sjoin->Copy(*sjoRow, "fUsrID", "fUsrIdCare");

		DockOrder::s_me->AddPet("request", sjoin, eShowDock1);

		// 이창 detail 바꿈. 선택 할때 이미 item changed 했을텐데
// 		_KwBeginInvoke(this, [&, fPetID]() // foreground 작업 큐
// 			{
// 				ShJObj sjin = make_shared<JObj>();///?ex: 고객상세창 Refresh
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
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다. AddMyPet
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
	PWS  newPet = L"새 펫";
	if(sjaTable)
	{
		for(auto& sjv : *sjaTable)// 리니어 서치
		{
			auto sjoRow = sjv->AsObject();
			if(sjoRow->BeginS("fName", newPet))
			{
				CString fName = sjoRow->S("fName");
				auto sival = fName.Mid(3);
				sival.Trim();
				int iv = _wtoi(sival);
				//swscanf_s(fName, L"새 펫 %d", &iv);
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
	jin.Copy(*sjo, "fUsrIdCare");//선택한 사람
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
	auto sjoU = _sjo->GetArrayItem("table", 0);// array를 구한 후 0번쨰 항
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

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
