// DlgParallel.cpp : implementation file
//

#include "pch.h"
#include "MFCExHttpsSrv.h"
#include "DlgParallel.h"
#include "afxdialogex.h"
#include "CmnDoc.h"


// DlgParallel dialog

IMPLEMENT_DYNAMIC(DlgParallel, CDialogEx)

DlgParallel::DlgParallel(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_Clustering, pParent)
	, _url(_T(""))
	, _mode(_T(""))
	, _action(_T(""))
{

}

DlgParallel::~DlgParallel()
{
}

void DlgParallel::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	auto doc = dynamic_cast<CmnDoc*>(_doc);
	JObj& jbj = doc->_jdata;


	DDX_Text(pDX, IDC_url, _url);
	DDX_CBString(pDX, IDC_mode, _mode);
	DDX_CBString(pDX, IDC_action, _action);

	DDX_Control(pDX, IDC_Clustering, _cList);
}


BEGIN_MESSAGE_MAP(DlgParallel, CDialogEx)
	ON_CBN_SELCHANGE(IDC_mode, &DlgParallel::OnSelchangeMode)
	ON_CBN_SELCHANGE(IDC_action, &DlgParallel::OnSelchangeAction)
	ON_EN_CHANGE(IDC_url, &DlgParallel::OnChangeUrl)
	ON_BN_CLICKED(IDOK, &DlgParallel::OnBnClickedOk)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_Clustering, &DlgParallel::OnItemchangedClustering)
	ON_BN_CLICKED(IDC_Add, &DlgParallel::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_Del, &DlgParallel::OnBnClickedDel)
	ON_BN_CLICKED(IDC_Apply, &DlgParallel::OnBnClickedApply)
END_MESSAGE_MAP()


// DlgParallel message handlers

_STitleWidthField s_arlstProducts[] =
{
	{ 20, nullptr   , L"#"    ,},
	{ 235,"url"   , L"URL",},
	{ 70, "mode"   , L"Mode", },
	{ 70, "action", L"Action",},
	{ 50, "error", L"Error",},
};
BOOL DlgParallel::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	KwSetListColumn(&_cList, s_arlstProducts, _countof(s_arlstProducts));
	KwSetListReportStyle(&_cList);

	///auto doc1 = (CmnDoc*)_doc; 이건 안되. 폴리모피즘이야.
	auto doc = dynamic_cast<CmnDoc*>(_doc);
	auto sjdata = doc->GetJData();/// 다중작업이라 복사해서 쓴다.
	_arSvr = sjdata->Array("Clustering");
	if(!_arSvr || _arSvr->size() == 0)//펜딩서버 있고
		return TRUE;

	Refresh();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void DlgParallel::Refresh()
{
	auto doc = dynamic_cast<CmnDoc*>(_doc);
	_cList.DeleteAllItems();
	for(int i = 0; i < _arSvr->size(); i++)
	{
		_cList.InsertItem(i, L"");// 먼저 해야, 빈칸이라도 들어 가지.
	
		auto sitm = _arSvr->GetAt(i); if(!sitm->IsObject()) continue;
		auto svr = sitm->AsObject(); if(!svr) continue;

		// error는 따로 저장 하므로 가져와서 넣어 준다.
		int error = 0;
		doc->_mpCntError.Lookup(svr->SA("url"), error);
		(*svr)("error") = error;
		for(int j = 1; j < _countof(s_arlstProducts); j++)
		{
			_cList.SetItem(i, j, LVIF_TEXT, svr->Str(s_arlstProducts[j].field), 0, 0, 0, NULL, 0);
			//_cList.SetItemData(i, (DWORD_PTR)(PWS)_data.GetAt(i));// sjo->S("fPetID"));//상세창이 없으므로 쓸일이 없다
		}
	}

}

/// <summary>
/// 같은 url이 있으면 true 리턴
/// </summary>
bool DlgParallel::CheckDuplicate(PWS newUrl, int iExcept)
{
	for(int i = 0; i < _arSvr->size(); i++)
	{
		if(i == iExcept)
			continue;
		auto sitm = _arSvr->GetAt(i); if(!sitm->IsObject()) continue;
		auto svr = sitm->AsObject(); if(!svr) continue;
		if(svr->SameS("url", newUrl))
			return true;
	}
	return false;
}
ShJObj DlgParallel::GetCurJObj()
{
	ShJObj svr;
	int isel = KwGetSelectedListItem(&_cList);
	if(isel >= 0)
	{
		auto sitm = _arSvr->GetAt(isel);  if(!sitm->IsObject()) return svr;
		svr = sitm->AsObject(); if(!svr) return svr;
	}
	return svr;
}
void DlgParallel::OnSelchangeMode()
{
// 	UpdateData();
// 	auto svr = GetCurJObj();
// 	if(svr)
// 		(*svr)("mode") = _mode;
}


void DlgParallel::OnSelchangeAction()
{
// 	UpdateData();
// 	auto svr = GetCurJObj();
// 	if(svr)
// 		(*svr)("action") = _action;
}


void DlgParallel::OnChangeUrl()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
// 	UpdateData();
// 	auto svr = GetCurJObj();
// 	if(svr)
// 		(*svr)("url") = _url;
}


void DlgParallel::OnBnClickedOk()
{
	auto doc = dynamic_cast<CmnDoc*>(_doc);
	{
		AUTOLOCK(doc->_csJdata);
		JObj& jbj = doc->_jdata;
		auto sjarr = jbj.Array("Clustering");
		sjarr->Clone(_arSvr, true);/// shared_pointer에 의한 참조 이므로 내부에도 적용 된다.
	}
	CDialogEx::OnOK();
}


void DlgParallel::OnItemchangedClustering(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	auto svr = GetCurJObj();
	if(svr)
	{
		_url = svr->S("url");
		_mode = svr->S("mode");
		_action = svr->S("action");

		UpdateData(0);
	}
	
	bool bMain = pNMLV->iItem == 0;
	GetDlgItem(IDC_mode)->EnableWindow(!bMain);
	GetDlgItem(IDC_url)->EnableWindow(!bMain);
	GetDlgItem(IDC_Del)->EnableWindow(!bMain); /// main 0번 항목은 자신이므로 삭제 안된다.
	
	GetDlgItem(IDC_Apply)->EnableWindow(pNMLV->iItem >= 0);// 선택된게 있어야. 가끔 하나도 선택 안된 경우 있어.
	*pResult = 0;
}


void DlgParallel::OnBnClickedAdd()
{
	UpdateData();
	bool bok = _url.Find(L"http") == 0 && _url.Find(L"://") >= 4;
	if(!bok)
	{
		AfxMessageBox(L"The URL must start with 'http://'.\n(ex: 'http://192.168.0.100:8080')");
		return;
	}
	if(CheckDuplicate(_url))
	{
		AfxMessageBox(L"The same URL already exists.");
		return;
	}
	ShJObj sjo = make_shared<JObj>();
	(*sjo)("url") = _url;
	(*sjo)("mode") = _mode;
	(*sjo)("action") = _action;
	_arSvr->Add(sjo);
	Refresh();
}


void DlgParallel::OnBnClickedDel()
{
	int isel = KwGetSelectedListItem(&_cList);
	if(isel > 0)
	{
		_arSvr->RemoveAt(isel);
		Refresh();
	}
}


void DlgParallel::OnBnClickedApply()
{
	UpdateData();
	auto i0 = _url.Find(L"http");
	auto i1 = _url.Find(L"://");
	bool bok = i0 == 0 && i1 > 3;
	if(!bok)
	{
		AfxMessageBox(L"The URL must start with 'http://'.\n(ex: 'http://192.168.0.100:8080')");
		return;
	}
	auto svr = GetCurJObj();
	if(!svr)
	{
		AfxMessageBox(L"Select one item from the list.");
		return;
	}
	int isel = KwGetSelectedListItem(&_cList);
	if(CheckDuplicate(_url, isel))
	{
		AfxMessageBox(L"The same URL already exists.");
		return;
	}
	/// 이미 disabled 되어 있음.
 	if(!svr->SameS("mode", L"main") && _mode == L"main")
 	{
 		AfxMessageBox(L"The support server mode can't be 'main'.");
 		return;
 	}

	if(svr)
	{
		(*svr)("url") = _url;
		(*svr)("mode") = _mode;
		(*svr)("action") = _action;
		Refresh();
	}
}
