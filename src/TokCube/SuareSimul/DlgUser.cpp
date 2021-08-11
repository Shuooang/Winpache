// DlgUser.cpp : implementation file
//

#include "pch.h"
#include "SuareSimul.h"
#include "DlgUser.h"
#include "afxdialogex.h"


// DlgUser dialog

IMPLEMENT_DYNAMIC(DlgUser, CDlgInvokable)

DlgUser::DlgUser(CWnd* pParent /*=nullptr*/)
	: CDlgInvokable(IDD_DlgUser, pParent)
	, _email(_T(""))
	, _SNS(_T(""))
	, _Nickname(_T(""))
{

}

DlgUser::~DlgUser()
{
}

void DlgUser::DoDataExchange(CDataExchange* pDX)
{
	CDlgInvokable::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_email, _email);
	DDX_Text(pDX, IDC_SNS, _SNS);
	DDX_Text(pDX, IDC_Nickname, _Nickname);
}


BEGIN_MESSAGE_MAP(DlgUser, CDlgInvokable)
	ON_BN_CLICKED(IDOK, &DlgUser::OnBnClickedOk)
END_MESSAGE_MAP()


// DlgUser message handlers


void DlgUser::OnAfterSave(int rv, ShJObj sjUsr)
{
	BACKGROUND(1);
	_KwBeginInvoke(this, [&, rv, sjUsr]()-> void
		{
			auto app = (CSuareSimulApp*)GetMainApp();
			auto& appd = (app)->_docApp;
			FOREGROUND();
			if(rv == 0)
			{
				(*appd._json)("email") = _email;
				(*appd._json)("SNS") = _SNS;
				(*appd._json)("Nickname") = _Nickname;
				/// CubeRegisterUser 에서 sjUsr를 바꿨으므로 여기서 복사해 줘야
				(*appd._json)("stat") = sjUsr->S("stat");
				appd.SaveData();

				CDlgInvokable::OnOK();
			}
			else if(rv == -10)//Nickname duplicated
			{
				KwMessageBoxError(KwRsc(IDC_NicknameDuplicated));
			}
		});
}
void DlgUser::OnBnClickedOk()
{
	UpdateData();
	CMainPool::QueueFunc([&]()
		{ 
			BACKGROUND(1);
			auto app = (CSuareSimulApp*)GetMainApp();
			auto& appd = (app)->_docApp;
			AUTOLOCK(appd._csAppDoc);

			auto sjUsr = make_shared<JObj>(appd._json);
			(*sjUsr)("email") = _email;
			(*sjUsr)("SNS") = _SNS;
			(*sjUsr)("Nickname") = _Nickname;

			int rv = appd.CubeRegisterUser(sjUsr, false);
			
			OnAfterSave(rv, sjUsr);
		});
}
