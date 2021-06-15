#include "pch.h"
#include "Requ.h"
#include "AppBase.h"
#include "KwLib64/TimeTool.h"
#include "MFCSmpApp4.h"


void AppBase::CallClientException(KException* se)
{
///	FOREGROUND(); BACKGROUND �ϼ��� �ִ�.
	//auto pfn = (CMainFrame*)AfxGetMainWnd();
	ShJObj sjo = ShJObj(new JObj);
	auto& jo = *sjo;
	
#ifdef _DEBUG
	jo("fDebug") = L"DEBUG";
#else
	jo("fDebug") = L"release";
#endif
	jo("fObject") = se->m_strStateNativeOrigin;
	jo("f02class") = se->_sExcept;
	jo("f10Err") = (int)se->_error;
	jo("fReturn") = se->m_nRetCode;
	jo("f12ToString") = se->m_strError;
	jo("fStack") = se->_func;
	jo("fLine") = se->_line;

	RM_ClientException(sjo);
}

void AppBase::RM_ClientException(ShJObj sjo, int iOp)
{
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		///	FOREGROUND(); BACKGROUND �ϼ��� �ִ�. QueueAsyncFunc ���� �߻��� ���
		/// ����� QueueAsyncFunc ���� �ȵȴ�. try catch ���ѷ��� ����
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud �۾� ť
			{
				RM_ClientException(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);/// (2) �ϼ��� �ִ�.
	try
	{
		JObj jin, jout;
		for(auto& [key, val] : *sjo)
			jin(key) = val;
		//jin("fUsrID") = L"staff-a021";/// ���߿� �α����� �����ID
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

		KRequ req;// (jin.S("fUsrID"));//��û�� ����� �α����� ����ڿ� ����.
		req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
		//if(!tchsame(jout.S("Return"), L"OK"))
		//	return;

		//_KwBeginInvoke(this, [&, jout]() /// foreground �۾� ť
		//	{	FOREGROUND();	});
	}
	catch(KException* e)/// ���� ������ ���� ���� ���⼭ �� ���� �Ѵ�. �ȱ׷��� ���� ����
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



void AppBase::AddWorkLog(PAS fObject, PAS fTitle, PWS fObjID, PAS fMode, PWS fObjName, PWS fObjName2, PAS fMemo, PWS fObjID2)
{
	//ShJObj sjo = ShJObj(new JObj);
	ShJObj sjo = make_shared<JObj>();
	auto& jo = *sjo;

	jo("fObject") = fObject;
	jo("fObjID") = fObjID;
	if(tchlen(fObjID2))
		jo("fObjID2") = fObjID2;
	if(tchlen(fObjName2))
		jo("fObjName2") = fObjName2;
	jo("fTitle") = fTitle;
	jo("fMode") = fMode;
	jo("fObjName") = fObjName;
	jo("fMemo") = fMemo;
	CKTime t = KwGetCurrentTime();
	CString st;
	KwCTimeToString(t, st);
	jo("fTmWork") = st;

	DockWorkLogList::s_me->WorkLog(sjo);//UI�� �Ѹ���.fTmWork
	
	RM_AddWorkLog(sjo);//������ ����
}

void AppBase::RM_AddWorkLog(ShJObj sjo, int iOp)
{
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud �۾� ť
			{
				RM_AddWorkLog(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);/// (2) �ϼ��� �ִ�.
	try
	{
		JObj jin, jout;
		for(auto& [key, val] : *sjo)
			jin.Copy(*sjo, key.c_str());

		CString fUsrID, fBizID;
// 		auto app = (CSmpApp4*)AfxGetApp();
// 		if(app->_login->Has("fUsrID"))
// 		{
// 			fUsrID = app->getLoginData("fUsrID");
// 			fBizID = app->getLoginData("fBizID");
// 		}
// 		else
// 		{
// 			fUsrID = L"(unkown)";
// 			fBizID = L"(unkown)";
// 		}
// 		jin("fUsrID") = fUsrID;
// 		jin("fBizID") = fBizID;
		KRequ req;// (jin.S("fUsrID"));//��û�� ����� �α����� ����ڿ� ����.
		jin("fUsrID") = req._UsrID;//������ ����� �α����� ���.
		jin("fBizID") = req._BizID;//������ ����� �α����� ���.
// 		jin.Copy(*sjo, "fObject");
// 		jin.Copy(*sjo, "fObjID");
// 		jin.Copy(*sjo, "fObjName");
// 		jin.Copy(*sjo, "fMode");
		jin("fDevice") = L"PC";
		//CRuntimeClass* rc = this->GetRuntimeClass();
		//jin("fClass") = rc->m_lpszClassName;

		req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
	}
	catch(KException* e)/// ���� ������ ���� ���� ���⼭ �� ���� �Ѵ�. �ȱ׷��� ���� ����
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









IMPLEMENT_DYNAMIC(CSafeViewBase, CFormInvokable)

BEGIN_MESSAGE_MAP(CSafeViewBase, CFormInvokable)
END_MESSAGE_MAP()


CSafeViewBase::CSafeViewBase(UINT nIDTemplate)
	: CFormInvokable(nIDTemplate)//�Ƶ��� �Ҿƹ��� ��ü ���� �ٷ� �θ��� ��� �ƹ����� ���� �� ��.
{
}
CSafeViewBase::~CSafeViewBase()
{
}


void CSafeViewBase::DoDataExchange(CDataExchange* pDX)
{
	CFormInvokable::DoDataExchange(pDX);
}

BOOL CSafeViewBase::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CFormInvokable::PreCreateWindow(cs);
}

void CSafeViewBase::OnInitialUpdate()
{
	CFormInvokable::OnInitialUpdate();
	//ResizeParentToFit();

}


LRESULT CSafeViewBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lr = 0;
	try
	{
		try
		{
			lr = CFormInvokable::WindowProc(message, wParam, lParam);
		} CATCH_APP;
	}
	catch(KException* e)/// �׸��� ���Ⱑ 2��°�� ���� ���⼭ ��� ���� ó�� �ؾ� �Ѵ�.
	{
		CRuntimeClass* rc = this->GetRuntimeClass();
		e->m_strStateNativeOrigin = rc->m_lpszClassName;
		TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTION__, __LINE__);
		AppBase::CallClientException(e);//DockClientBase
		e->Delete();
	}
	return lr;
}
