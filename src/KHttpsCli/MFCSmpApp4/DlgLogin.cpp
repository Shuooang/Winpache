// DlgLogin.cpp: 구현 파일
//

#include "pch.h"
#include "MFCSmpApp4.h"
#include "DlgLogin.h"
#include "afxdialogex.h"
#include "Requ.h"


// DlgLogin 대화 상자

IMPLEMENT_DYNAMIC(DlgLogin, CDialogEx)

DlgLogin::DlgLogin(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LogIn, pParent)
	, _sLoginID(_T(""))
	, _sPcPwd(_T(""))
	, _CertCode(_T(""))
{

}

DlgLogin::~DlgLogin()
{
}

void DlgLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LogInID, _sLoginID);
	DDX_Text(pDX, IDC_PcPwd, _sPcPwd);
	DDX_Text(pDX, IDC_CertCode, _CertCode);
}


BEGIN_MESSAGE_MAP(DlgLogin, CDialogEx)
	ON_BN_CLICKED(IDOK, &DlgLogin::OnBnClickedOk)
END_MESSAGE_MAP()


// DlgLogin 메시지 처리기


void DlgLogin::OnBnClickedOk()
{
	UpdateData();
	if(_sLoginID.GetLength() > 0)
	{
		auto sjo = make_shared<JObj>();
		(*sjo)("fLogInID") = _sLoginID;
		if(_sPcPwd.GetLength() > 0)
			(*sjo)("fPcPwd") = _sPcPwd;
		if(_CertCode.GetLength() > 0)
			(*sjo)("fCertCode") = _CertCode;
		///RM_PcLogin(sjo, 0);
	}
	//CDialogEx::OnOK();
}

void DlgLogin::RM_RequestLogin(ShJObj sjo, int iOp)
{
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		///	FOREGROUND(); BACKGROUND 일수도 있다. QueueAsyncFunc 에서 발생한 경우
		/// 여기는 QueueAsyncFunc 쓰면 안된다. try catch 무한루프 돌라
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud 작업 큐
			{
				RM_RequestLogin(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);/// (2) 일수도 있다.
	try
	{
		JObj jin, jout;
		for(auto& [key, val] : *sjo)
			jin(key) = val;
		//jin("fUsrID") = L"staff-a021";/// 나중에 로그인한 사용자ID
		jin("fPakageApp") = L"com.petme2.smp";
		jin("fOsMachine") = KwGetWindowVersion();//L"6.2 (9200)\n"
		jin("fSysInfo") = KwSystemInfo();//L"6.2 (9200)\n"
		jin("fModel") = L"IBM convertible";
		jin("fComName") = KwComputerName();//L"DWKANG_PC" 
		jin("fTimeOcurd") = KwGetCurrentTime(); //CTime(); //L"1970-01-01 09:00:00"	
		jin("fAppVer") = KwGetProductVersion();//L"2"
		jin("fMachine") = L"PC";
		jin("fNetStat") = KwGetConnectedState();//L""

		CString fUsrID;
		auto app = (CSmpApp4*)AfxGetApp();
		if(app->_login->Has("fUsrID"))
			fUsrID = app->getLoginData("fUsrID");
		else
			fUsrID = L"(unkown)";

		KRequ req(fUsrID);// (jin.S("fUsrID"));//요청한 사람은 로그인한 사용자와 같다.
		req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
		if(!tchsame(jout.S("Return"), L"OK"))
		{
			AfxMessageBox(L"로그인에 실패 했습니다.");
			return;
		}
		ShJObj sjout = make_shared<JObj>(jout);

		_KwBeginInvoke(this, [&, sjout]() /// foreground 작업 큐
			{	
				FOREGROUND();	
				CDialogEx::OnOK();
			});
	}
	catch(KException* e)/// 오류 보내다 오류 나면 여기서 다 감수 한다. 안그러면 무한 루프
	{
		TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTION__, __LINE__);
		e->Delete();
	}
	catch(CException* e)
	{
		auto buf = new TCHAR(1024);
		KAtEnd d_buf([&]() { delete buf; });
		e->GetErrorMessage(buf, 1000);
		TRACE(L"CException:%s - %s %d\n", buf, __FUNCTION__, __LINE__);
		e->Delete();
	}
	catch(...)\
	{	TRACE("catch ...  - %s %d\n", __FUNCTION__, __LINE__);
	}

}
