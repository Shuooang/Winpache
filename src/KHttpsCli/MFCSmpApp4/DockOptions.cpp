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


// DockOptions �����Դϴ�.

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
	{ 60, "fName"     , L"�����",},
	{ 70, "fNameBreed", L"ǰ��", },
	{ 40, "fWeight"   , L"ü��",},
};
_STitleWidthField* DockOptions::getListFieldInfo(int* pnCols)
{
	if(pnCols) *pnCols = _countof(s_arlstOptions);
	return s_arlstOptions;
}

// DockOptions �޽��� ó�����Դϴ�.
// CBranchVu1 �޽��� ó�����Դϴ�.
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
		ShJObj sjout = sjo;/// _sjo �� ����.

		if(sjout->Has("tenum"))
			_sjaEnum = sjo->Array("tenum");//�ʵ庰 �ڵ�� ��¿빮�ڿ� ������ ì���.

		if(sjout->Has("table"))
		{
			if(_sortKey.empty())
				_sortKey = "fName";
			_sjaTable.reset();
			//_sjaTable->clear();
			_sjaTable = sjout->Array("table");/// _sjo �� ����.
			MakeSort(_sjaTable);
			InsertDataToList();
			
			InitPropList();//û��
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
	//for(auto& [k, sjo] : _sort)//| std::views::reverse �� c++20
	//{
	//	auto item = _cList.InsertItem(i, L"");
	//	i++;
	//}
	//i = _sortDirection == 1 ? 0 : _cList.GetItemCount() - 1;
	//for(auto& [k, sjo] : _sort)//| std::views::reverse �� c++20
	//{
	//	//auto item = _cList.InsertItem(i, L"");
	//	_cList.SetItem(i, 1, LVIF_TEXT, sjo->S("fName"), 0, 0, 0, NULL, 0);
	//	_cList.SetItem(i, 2, LVIF_TEXT, sjo->S("fNameBreed"), 0, 0, 0, NULL, 0);
	//	_cList.SetItem(i, 3, LVIF_TEXT, sjo->Str("fWeight", 1), 0, 0, 0, NULL, 0);
	//	_cList.SetItem(i, 4, LVIF_TEXT, sjo->S("fBrdSzCD"), 0, 0, 0, NULL, 0);
	//	_data.Add(sjo->S("fPetID"));
	//	const CString& data = _data.GetAt(i);
	//	_cList.SetItemData(i, (DWORD_PTR)(PWS)data);// sjo->S("fPetID"));//��â�� �����Ƿ� ������ ����
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

	jin.Copy(*sjo, "fUsrID");//������ ���
	KRequ req;//��û�� ���
	jin("fBizID") = req._BizID;//������ ����� �翬 �� �� �Ҽ� �̰���

	req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	if(!tchsame(jout.S("Return"), L"OK"))
		return;
	ShJObj sjout = make_shared<JObj>(jout);
	_KwBeginInvoke(this, [&, sjout]() /// foreground �۾� ť
		{
			FOREGROUND();
			Refresh("list", sjout);
		});
}

void DockOptions::OnBnClickedRead()
{
	auto sjo = make_shared<JObj>();
#ifdef DEBUG
	(*sjo)("fBizID") = L"biz-0002"; /// test��#
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
	//                                          8                                          2    |    1 = 3 // ���º�ȭ�� �ְ� ���õǾ��ų�, ��Ŀ���� ������ ���
	if(pNMLV->iItem >= 0 && pNMLV->uChanged == LVIF_STATE && pNMLV->uNewState == (LVIS_SELECTED | LVIS_FOCUSED))
	{
		CString fPetID = (PWS)_cList.GetItemData(pNMLV->iItem);// , (DWORD_PTR)sjo->S("fUsrID"));
		if(fPetID.GetLength() > 0)
		{
			ShJObj sjin = make_shared<JObj>();///?ex: ����â Refresh
			(*sjin)("fPetID") = fPetID;
			this->Refresh("output_detail", sjin, ePostAsync8);
		}
	}
	*pResult = 0;
}



// DockOptions message handlers
