// DockOptions.cpp : implementation file
//

#include "pch.h"
#include "MFCSmpApp4.h"
#include "DockOptions.h"
#include "Mainfrm.h"
#include "resource.h"
#include "Requ.h"


IMPLEMENT_DYNCREATE(DockOptionsPane, CPaneForm)

// DockOptions dialog

IMPLEMENT_DYNCREATE(DockOptions, DockClientBase)
DockOptions* DockOptions::s_me = NULL;

DockOptions::DockOptions(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn)
{
	s_me = this;
}

DockOptions::~DockOptions()
{
}

void DockOptions::DoDataExchange(CDataExchange* pDX)
{
	DockClientBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, _cList);
	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, _cProperty);
}


BEGIN_MESSAGE_MAP(DockOptions, DockClientBase)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &DockOptions::OnBnClickedButton1)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_BN_CLICKED(IDC_Read, &DockOptions::OnBnClickedRead)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &DockOptions::OnHdnItemclickList1)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST1, &DockOptions::OnLvnColumnclickList1)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &DockOptions::OnLvnItemchangedList1)
END_MESSAGE_MAP()


// DockOptions 진단입니다.

#ifdef _DEBUG
void DockOptions::AssertValid() const
{
	__super::AssertValid();
}

#ifndef _WIN32_WCE
void DockOptions::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}
#endif
#endif //_DEBUG

_STitleWidthField s_arlstOptions[] =
{
	{ 20, nullptr     , L"#"    ,},
	{ 60, "fName"     , L"펫네임",},
	{ 70, "fNameBreed", L"품종", },
	{ 40, "fWeight"   , L"체중",},
};
_STitleWidthField* DockOptions::getListFieldInfo(int* pnCols)
{
	if(pnCols) *pnCols = _countof(s_arlstOptions);
	return s_arlstOptions;
}

// DockOptions 메시지 처리기입니다.
// CBranchVu1 메시지 처리기입니다.
void DockOptions::OnInitialUpdate()
{
	if(m_bInited)
		return;
	__super::OnInitialUpdate();
	m_bInited = TRUE;


	KwSetListColumn(&_cList, s_arlstOptions, _countof(s_arlstOptions));
	InitPropList();

	AdjustLayout();
	KwSetListReportStyle(&_cList);
}

void DockOptions::AdjustLayout()
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

void DockOptions::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	AdjustLayout();
}

void DockOptions::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);
	//_cProperty.SetFocus();
}

void DockOptions::OnBnClickedButton1()
{
	//auto app = (CMfcSmpApp1App*)AfxGetApp();
	auto mfm = (CMainFrame*)AfxGetMainWnd();
	auto d1 = mfm->Dock(IDD_DockOptions);
	if(d1)
		AfxMessageBox(L"DockOptions ok");
}


void DockOptions::InitPropList(ShJObj sjo)
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

void DockOptions::Refresh(string eStep, ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, eStep);

	if(eStep == "request")
	{
		ShJObj sjin = sjo;
		RM_SelectOptions(sjin);
		return;
	}
	else if(eStep == "list")
	{
		FOREGROUND();
		ShJObj sjout = sjo;/// _sjo 와 같다.

		if(sjout->Has("tenum"))
			_sjaEnum = sjo->Array("tenum");//필드별 코드와 출력용문자열 데이터 챙긴다.

		if(sjout->Has("table"))
		{
			if(_sortKey.empty())
				_sortKey = "fName";
			_sjaTable.reset();
			//_sjaTable->clear();
			_sjaTable = sjout->Array("table");/// _sjo 와 같다.
			MakeSort(_sjaTable);
			InsertDataToList();
			
			InitPropList();//청소
		}
	}
	else if(eStep == "output_detail")
	{
		CString fPetID = sjo->S("fPetID");
		for(auto& sjv : *_sjaTable)
		{
			auto sjoRow = sjv->AsObject();
			if(sjoRow->size() == 0)
				_break;
			if(fPetID == sjoRow->S("fPetID"))
			{
				//InitPropList();
				_prData->SetJObj(sjoRow, _sjaEnum);
				_cProperty.ExpandAll();
				break;
			}
		}
	}
}


void DockOptions::InsertDataToList(bool bHasData)
{
	ClearList();
	if(!bHasData)
		return;
	InsertDataToListImpl("fPrdOptID", "tprodoptbiz");

	//int i = 0;
	//for(auto& [k, sjo] : _sort)//| std::views::reverse 는 c++20
	//{
	//	auto item = _cList.InsertItem(i, L"");
	//	i++;
	//}
	//i = _sortDirection == 1 ? 0 : _cList.GetItemCount() - 1;
	//for(auto& [k, sjo] : _sort)//| std::views::reverse 는 c++20
	//{
	//	//auto item = _cList.InsertItem(i, L"");
	//	_cList.SetItem(i, 1, LVIF_TEXT, sjo->S("fName"), 0, 0, 0, NULL, 0);
	//	_cList.SetItem(i, 2, LVIF_TEXT, sjo->S("fNameBreed"), 0, 0, 0, NULL, 0);
	//	_cList.SetItem(i, 3, LVIF_TEXT, sjo->Str("fWeight", 1), 0, 0, 0, NULL, 0);
	//	_cList.SetItem(i, 4, LVIF_TEXT, sjo->S("fBrdSzCD"), 0, 0, 0, NULL, 0);
	//	_data.Add(sjo->S("fPetID"));
	//	const CString& data = _data.GetAt(i);
	//	_cList.SetItemData(i, (DWORD_PTR)(PWS)data);// sjo->S("fPetID"));//상세창이 없으므로 쓸일이 없다
	//	CString fPetID = (PWS)_cList.GetItemData(i);// , (DWORD_PTR)sjo->S("fUsrID"));
	//	if(_sortDirection == 1)
	//		i++;
	//	else
	//		i--;
	//}

}
void DockOptions::RM_SelectOptions(ShJObj sjo, int iOp)
{
	TRACE("%s (%d) \n", __FUNCTION__, iOp);
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		FOREGROUND();
		CMainPool::QueueFunc([&, sjo, iOp]() { RM_SelectOptions(sjo, iOp); });
		return;
	}

	BACKGROUND(1);

	JObj jin, jout;

	jin.Copy(*sjo, "fUsrID");//선택한 사람
	KRequ req;//요청한 사람
	jin("fBizID") = req._BizID;//선택한 사람은 당연 이 샵 소속 이겠지

	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if(!tchsame(jout.S("Return"), L"OK"))
		return;
	ShJObj sjout = make_shared<JObj>(jout);
	_KwBeginInvoke(this, [&, sjout]() /// foreground 작업 큐
		{
			FOREGROUND();
			Refresh("list", sjout);
		});
}

void DockOptions::OnBnClickedRead()
{
	auto sjo = make_shared<JObj>();
#ifdef DEBUG
	(*sjo)("fBizID") = L"biz-0002"; /// test용#
#else
	ASSERT(0);
#endif
	Refresh("request", sjo);
}

void DockOptions::OnHdnItemclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	//TRACE("%s \n", __FUNCTION__);
	*pResult = 0;
}

void DockOptions::OnLvnColumnclickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	TRACE("%s (%d, %d)\n", __FUNCTION__, pNMLV->iItem, pNMLV->iSubItem);
	SetListSort(_cList, pNMLV, s_arlstOptions);

	*pResult = 0;
}


void DockOptions::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
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
			this->Refresh("output_detail", sjin, ePostAsync8);
		}
	}
	*pResult = 0;
}



// DockOptions message handlers
