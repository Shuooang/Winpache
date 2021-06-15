#include "pch.h"
#include "Requ.h"
#include "AppBase.h"
#include "KwLib64/TimeTool.h"
#include "MFCSmpApp4.h"


void AppBase::CallClientException(KException* se)
{
///	FOREGROUND(); BACKGROUND 일수도 있다.
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
		///	FOREGROUND(); BACKGROUND 일수도 있다. QueueAsyncFunc 에서 발생한 경우
		/// 여기는 QueueAsyncFunc 쓰면 안된다. try catch 무한루프 돌라
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud 작업 큐
			{
				RM_ClientException(sjo, iOp);
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

		KRequ req;// (jin.S("fUsrID"));//요청한 사람은 로그인한 사용자와 같다.
		req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
		//if(!tchsame(jout.S("Return"), L"OK"))
		//	return;

		//_KwBeginInvoke(this, [&, jout]() /// foreground 작업 큐
		//	{	FOREGROUND();	});
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

	DockWorkLogList::s_me->WorkLog(sjo);//UI에 뿌린다.fTmWork
	
	RM_AddWorkLog(sjo);//서버에 저장
}

void AppBase::RM_AddWorkLog(ShJObj sjo, int iOp)
{
	if(!KwAttrXor(iOp, eNotInBG256))
	{
		CMainPool::QueueFunc([&, sjo, iOp]() ///backgroud 작업 큐
			{
				RM_AddWorkLog(sjo, iOp);
			});
		return;
	}

	BACKGROUND(1);/// (2) 일수도 있다.
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
		KRequ req;// (jin.S("fUsrID"));//요청한 사람은 로그인한 사용자와 같다.
		jin("fUsrID") = req._UsrID;//변경한 사람은 로그인한 사람.
		jin("fBizID") = req._BizID;//변경한 사람은 로그인한 사람.
// 		jin.Copy(*sjo, "fObject");
// 		jin.Copy(*sjo, "fObjID");
// 		jin.Copy(*sjo, "fObjName");
// 		jin.Copy(*sjo, "fMode");
		jin("fDevice") = L"PC";
		//CRuntimeClass* rc = this->GetRuntimeClass();
		//jin("fClass") = rc->m_lpszClassName;

		req.RequestPost(__FUNCTION__, jin, jout);//"GetTodaySchdule"
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









IMPLEMENT_DYNAMIC(CSafeViewBase, CFormInvokable)

BEGIN_MESSAGE_MAP(CSafeViewBase, CFormInvokable)
END_MESSAGE_MAP()


CSafeViewBase::CSafeViewBase(UINT nIDTemplate)
	: CFormInvokable(nIDTemplate)//아들이 할아버지 객체 것을 바로 부를수 없어서 아버지가 전달 해 줌.
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
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

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
	catch(KException* e)/// 그리고 여기가 2번째로 오니 여기서 모든 오류 처리 해야 한다.
	{
		CRuntimeClass* rc = this->GetRuntimeClass();
		e->m_strStateNativeOrigin = rc->m_lpszClassName;
		TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTION__, __LINE__);
		AppBase::CallClientException(e);//DockClientBase
		e->Delete();
	}
	return lr;
}
