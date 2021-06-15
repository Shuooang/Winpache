// DockProducts.cpp : implementation file
//

#include "pch.h"
#include "MFCSmpApp4.h"
#include "DockProducts.h"
#include "DockOrder.h"
#include "Mainfrm.h"
#include "resource.h"
#include "Requ.h"

IMPLEMENT_DYNCREATE(DockProductsPane, CPaneForm)

// DockProducts

IMPLEMENT_DYNCREATE(DockProducts, DockClientBase)
DockProducts* DockProducts::s_me = NULL;

DockProducts::DockProducts(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn)
{
	s_me = this;

}

DockProducts::~DockProducts()
{
}

void DockProducts::DoDataExchange(CDataExchange* pDX)
{
	DockClientBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, _cList);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, _cProperty);
}

BEGIN_MESSAGE_MAP(DockProducts, DockClientBase)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &DockProducts::OnBnClickedButton1)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_BN_CLICKED(IDC_Read, &DockProducts::OnBnClickedRead)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &DockProducts::OnHdnItemclickList1)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &DockProducts::OnLvnColumnclickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &DockProducts::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_AddToOrder, &DockProducts::OnBnClickedAddtoorder)
END_MESSAGE_MAP()


// DockProducts 진단입니다.

#ifdef _DEBUG
void DockProducts::AssertValid() const
{
	DockClientBase::AssertValid();
}

#ifndef _WIN32_WCE
void DockProducts::Dump(CDumpContext& dc) const
{
	DockClientBase::Dump(dc);
}
#endif
#endif //_DEBUG

_STitleWidthField s_arlstProducts[] =
{
	{ 20, nullptr   , L"#"    ,},
	{ 140,"fName"   , L"상품명",},
	{ 70, "fKind"   , L"종류", },
	{ 70, "fElapsed", L"소요(분)",},
};
_STitleWidthField* DockProducts::getListFieldInfo(int* pnCols)
{
	if(pnCols) *pnCols = _countof(s_arlstProducts);
	return s_arlstProducts;
}

// DockProducts 메시지 처리기입니다.
// CBranchVu1 메시지 처리기입니다.
void DockProducts::OnInitialUpdate()
{
	if(m_bInited)
		return;
	DockClientBase::OnInitialUpdate();
	m_bInited = TRUE;


	KwSetListColumn(&_cList, s_arlstProducts, _countof(s_arlstProducts));
	InitPropList();

	AdjustLayout();
	KwSetListReportStyle(&_cList);
}

void DockProducts::AdjustLayout()
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

void DockProducts::OnSize(UINT nType, int cx, int cy)
{
	DockClientBase::OnSize(nType, cx, cy);
	AdjustLayout();
}

void DockProducts::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);
	//_cProperty.SetFocus();
}

void DockProducts::OnBnClickedButton1()
{
	//auto app = (CMfcSmpApp1App*)AfxGetApp();
	auto mfm = (CMainFrame*)AfxGetMainWnd();
	auto d1 = mfm->Dock(IDD_DockProducts);
	if(d1)
		AfxMessageBox(L"DockProducts ok");
}


void DockProducts::InitPropList(ShJObj sjo)
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
	dt.AddGroup("gUsr", L"상품 정보");
	dt.SetField("gUsr", "fName", L"상품명", L"상품명 입니다.");
	dt.SetField("gUsr", "fShow", L"노출여부", L"소비자앱에서 노출여부를 표시합니다.");
	dt.SetField("gUsr", "fKind", L"종류", L"상품의 종류 표시 합니다.");/// 0. 했다가, 소수 아래 0 이 많이 나와서 문자열로 사용 , 0.);
	dt.AddGroup("gEtc", L"기타정보");
	dt.SetField("gEtc", "fElapsed", L"작업시간", L"작업시간을 분단위로 표시합니다.");
	dt.SetField("gEtc", "fOrder", L"순서", L"순서를 소수점 있는 숫자로 표시합니다.", 0);
	dt.SetField("gEtc", "fDesc", L"설명", L"설명 표시 합니다.");

}

void DockProducts::Refresh(string eStep, ShJObj sjo, int iOp)
{
	//TRACE("%s (%d) \n", __FUNCTION__, eStep);
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
	FOREGROUND();

	if(eStep == "request")
	{
		ShJObj sjin = sjo;
		RM_SelectProductList(sjo, iOp);
		return;
	}
	else if(eStep == "list")
	{
		FOREGROUND();
		if(!tchsame(sjo->S("Return"), L"OK"))
		{
			InsertDataToList(false);//청소
			InitPropList();//청소
			return;
		}

		_sjaTable = sjo->Array("table");
		_sjaEnum = sjo->Array("tenum");

		if(_sjaTable && _sjaEnum)//sjout->Has("table"))
		{
			SetEnum(*_sjaEnum);

			if(_sortKey.empty())
				_sortKey = (PAS)CStringA(s_arlstProducts[1].field);// "fName";

			MakeSort(_sjaTable);
			InsertDataToList();

			InitPropList();
		}
	}
	else if(eStep == "output_detail")
	{
		CString fProdID = sjo->S("fProdID");
		auto sjaTable = _sjo->Array("table");
		if(sjaTable)
		{
			ShJObj sjoRow = sjaTable->FindByValue("fProdID", fProdID);
			if(sjoRow)
			{
				auto sjaEnum = _sjo->Array("tenum");
				_prData->SetJObj(sjoRow, sjaEnum);
				_cProperty.ExpandAll();
				if(KwAttr(iOp, eSyncByAction64))
				{
					CString fObjName = sjoRow->S("fName");
					AppBase::AddWorkLog("tproductbiz", "상품", fProdID, "상세보기", fObjName);
				}
			}
		}
	}
}

//void DockProducts::ClearList(int iOp)
//{
//	if(KwAttrDel(iOp, ePostAsync8))
//	{
//		_KwBeginInvoke(this, [&, iOp]()
//			{
//				ClearList(iOp);
//			});
//		return;
//	}
//	_cList.DeleteAllItems();
//	_data.RemoveAll();
//	InitPropList();
//}

void DockProducts::InsertDataToList(bool bHasData)
{
	ClearList(0);
	if(!bHasData)
		return;
	InsertDataToListImpl("fProdID", "tproductbiz");
	//auto LdSetItem = [&](int i, int subi, ShJObj sjo)
	//{
	//	if(subi == 1)
	//	{
	//		_data.Add(sjo->S("fProdID"));
	//		_cList.SetItemData(i, (DWORD_PTR)(PWS)_data.GetAt(i));// sjo->S("fPetID"));//상세창이 없으므로 쓸일이 없다
	//	}
	//	PAS k = s_arlstProducts[subi].field;
	//	CString str = sjo->Str(k);
	//	auto tenumpr = _mapEnum[L"tproductbiz"];
	//	wstring fKR;
	//	CStringW stru;
	//	if(tenumpr && tenumpr->Lookup((PWS)str, fKR))
	//		stru = fKR.c_str();
	//	else
	//		stru = str;
	//	_cList.SetItem(i, subi, LVIF_TEXT, stru, 0, 0, 0, NULL, 0);
	//};
	//InsertAllItemT(_cList, _countof(s_arlstProducts), LdSetItem);
}

void DockProducts::RM_SelectProductList(ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]() { RM_SelectProductList(sjo, iOp); });
		return;
	}

	BACKGROUND(1);

	JObj jin, jout;

	//jin.Copy(*sjo, "fUsrID");
	KRequ req;//요청한 사람
	//jin("fUsrID") = req._UsrID;//로그인 사용자
	//jin("fBizID") = req._BizID;//현재 펫(로그인)
	jin.Copy(*sjo, "fBizID");

	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if(!tchsame(jout.S("Return"), L"OK"))
		return;

	_sjo = make_shared<JObj>(jout);//최초복사: 로컬변수 이므로 _KwBeginInvoke에 넘겨 주려면
	//auto sjout = _sjo;
	Refresh("list", _sjo, iOp | ePostAsync8);
}

void DockProducts::OnBnClickedRead()
{
	auto sjo = make_shared<JObj>();
#ifdef DEBUG
	(*sjo)("fBizID") = L"biz-0002"; /// test용#
#else
	ASSERT(0);
#endif
	Refresh("request", sjo);
}

void DockProducts::OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	//TRACE("%s \n", __FUNCTION__);
	*pResult = 0;
}

void DockProducts::OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	TRACE("%s (%d, %d)\n", __FUNCTION__, pNMLV->iItem, pNMLV->iSubItem);
	SetListSort(_cList, pNMLV, s_arlstProducts, _sjo->Array("table")); // ShJArr을 직접 줘 본다.
	//SetListSort(_cList, pNMLV, s_arlstProducts); _sjo->Array(table) 사용 한다.
	*pResult = 0;
}

void DockProducts::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	//                                          8                                          2    |    1 = 3 // 상태변화가 있고 선택되었거나, 포커스가 가있을 경우
	BOOL bUpdate = FALSE;
	CString fProdID;
	if(pNMLV->iItem >= 0 && pNMLV->uChanged == LVIF_STATE && pNMLV->uNewState == (LVIS_SELECTED | LVIS_FOCUSED))
	{
		fProdID = (PWS)_cList.GetItemData(pNMLV->iItem);// , (DWORD_PTR)sjo->S("fUsrID"));
		if(fProdID.GetLength() > 0)
		{
			ShJObj sjin = make_shared<JObj>();///?ex: 고객상세창 Refresh
			(*sjin)("fProdID") = fProdID;
			Refresh("output_detail", sjin, eSyncByAction64 | ePostAsync8);
			bUpdate = TRUE;
		}
		else
			fProdID = L"NULL";
	}
	TRACE("%s (%d, %d, %s, %s, %d, %s)\n", __FUNCTION__, pNMLV->iItem, pNMLV->uChanged,
		KwAttr(pNMLV->uNewState, LVIS_SELECTED) ? "select" : "",
		KwAttr(pNMLV->uNewState, LVIS_FOCUSED) ? "focus" : "", bUpdate, CStringA(fProdID));
	*pResult = 0;
}



void DockProducts::OnBnClickedAddtoorder()
{
	int nSel = _cList.GetNextItem(-1, LVNI_SELECTED);
	if(nSel < 0)
	{
		AfxMessageBox(L"예약에 추가할 상품 하나를 선택 하세요.");
		return;
	}
	CString fProdID = (PWS)_cList.GetItemData(nSel);
	if(fProdID.GetLength() > 0)
	{
		ShJObj sjoRow = FindByID(fProdID, "fProdID");
		ShJObj sjoin = make_shared<JObj>();///?ex: 고객상세창 Refresh
		sjoin->Copy(*sjoRow, "fProdID");

		DockOrder::s_me->AddProd("request", sjoin, eShowDock1);
	}
}
