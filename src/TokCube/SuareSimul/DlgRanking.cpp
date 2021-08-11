// DlgRanking.cpp : implementation file
//

#include "pch.h"
#include "SuareSimul.h"
#include "DlgRanking.h"
#include "afxdialogex.h"


// DlgRanking dialog

IMPLEMENT_DYNAMIC(DlgRanking, CDlgInvokable)

DlgRanking::DlgRanking(CWnd* pParent /*=nullptr*/)
	: CDlgInvokable(IDD_DlgRanking, pParent)
{

}

DlgRanking::~DlgRanking()
{
}

void DlgRanking::DoDataExchange(CDataExchange* pDX)
{
	CDlgInvokable::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, _cList);
}


BEGIN_MESSAGE_MAP(DlgRanking, CDlgInvokable)
	ON_BN_CLICKED(IDC_ReplaySample, &DlgRanking::OnBnClickedReplaysample)
	ON_BN_CLICKED(IDC_TrySame, &DlgRanking::OnBnClickedTrysame)
	ON_BN_CLICKED(IDCANCEL, &DlgRanking::OnBnClickedCancel)
END_MESSAGE_MAP()


// DlgRanking message handlers

_STitleWidthField s_arlstGames[] =
{
	{ 25, "rank"   , L"#"    ,0, 1,},
	{ 80, "fNickname"   , L"Nickname", IDS_Nickname,},
	{ 40, "fCount", L"Count",IDS_Count,1,},
	{ 75, "fSec", L"Time(sec)",IDS_TimeSec,},
	{ 120, "fTimeReg", L"Game Time",IDS_RegTime,},
};

BOOL DlgRanking::OnInitDialog()
{
	CDlgInvokable::OnInitDialog();
// 	s_arlstGames[1].title = KwRsc(IDS_Nickname);
// 	s_arlstGames[2].title = KwRsc(IDS_Count);
// 	s_arlstGames[3].title = KwRsc(IDS_TimeSec);
	KwSetListReportStyle(&_cList);
	KwSetListColumn(&_cList, s_arlstGames, _countof(s_arlstGames));

	CMainPool::QueueFunc([&]()
		{
			auto app = (CSuareSimulApp*)GetMainApp();
			auto& appd = (app)->_docApp;
			auto rv = appd.CubeGetRankingList(_sjTbl);
			KwBeginInvoke(this, ([&, rv]()-> void
				{
					if(rv == 0)
						Refresh();
					else
						KwMessageBoxError(KwRsc(IDS_CantConnect));
				}));//?beginInvoke 4
		});
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
void DlgRanking::Refresh()
{
	_cList.DeleteAllItems();
	int i = 0;
	CString sbuf; 
	PWS buf = sbuf.GetBuffer(50);
	if(_sjTbl)
	{
		for(auto& it : *_sjTbl)
		{
			auto sjo = it->AsObject();
			KwItoaAW(i + 1, (WCHAR*)buf, 40);
			_cList.InsertItem(i, buf);// ���� �ؾ�, ��ĭ�̶� ��� ����.
			for(int j = 1; j < _countof(s_arlstGames); j++)
				_cList.SetItem(i, j, LVIF_TEXT, sjo->Str(s_arlstGames[j].field, 3), 0, 0, 0, NULL, 0);
			i++;
		}
	}
	// SELECT g.fgguid, g.fuuid, (g.fSec7th/10000000) fSec, u.fNickname, g.fTimeReg FROM tgame g JOIN tuser u ON g.fuuid=u.fuuid ORDER BY g.fSec7th LIMIT 20;

}


void DlgRanking::OnBnClickedReplaysample()
{
	//_sjgame = ;
	/// ���� ���õȰ� ������ �ϳ��� ������ �׼��� �����ͼ� 
	/// ���� �ʱⰪ �� ���
	/// �ϳ��� �����̸� �ȴ�. 
	/// �Ź� Ÿ�̸Ӹ� ���� �ϵ�, ���� ���� �ش�. IDC_ReplaySample IDC_TrySame
	auto i = KwGetSelectedListItem(&_cList);
	if(i >= 0)
	{
		_iSel = i;
		EndDialog(IDC_ReplaySample);
	}
}


void DlgRanking::OnBnClickedTrysame()
{
	auto i = KwGetSelectedListItem(&_cList);
	if(i >= 0)
	{
		_iSel = i;
		EndDialog(IDC_TrySame);
	}
}


void DlgRanking::OnBnClickedCancel()
{
	OnCancel();
}
