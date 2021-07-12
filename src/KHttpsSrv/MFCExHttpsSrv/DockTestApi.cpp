// DockTestApi.cpp : implementation file
//

#include "pch.h"
#include "DockTestApi.h"
#include "Mainfrm.h"
#include "resource.h"
#include "Response1.h"
#include "ApiBase.h"

#ifndef SHARED_HANDLERS
#include "MFCExHttpsSrv.h"
#endif

IMPLEMENT_DYNCREATE(DockTestApiPane, CPaneForm)

// DockTestApi

IMPLEMENT_DYNCREATE(DockTestApi, DockClientBase)
DockTestApi* DockTestApi::s_me = NULL;

LPCWSTR s_ex1 = L"\
{\r\n\
   \"func\":\"ExGetApiList\",\r\n\
   \"params\": {\r\n\
      \"pr1\":\"val1\",\r\n\
      \"pr2\":10\r\n\
   }\r\n\
}";

DockTestApi::DockTestApi(UINT nID, CPaneForm* ppn)
	: DockClientBase(nID, ppn)
	, _Input(s_ex1)
	, _Output(_T("{ \"return\":0 }"))
{
	s_me = this;

}

DockTestApi::~DockTestApi()
{
}

void DockTestApi::DoDataExchange(CDataExchange* pDX)
{
	DockClientBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Input, c_Input);
	DDX_Control(pDX, IDC_Output, c_Output);
	DDX_Text(pDX, IDC_Input, _Input);
	DDX_Text(pDX, IDC_Output, _Output);
	DDX_Control(pDX, IDC_ExList, c_ExList);
}

BEGIN_MESSAGE_MAP(DockTestApi, DockClientBase)
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_BN_CLICKED(IDC_RunApi, &DockTestApi::OnBnClickedRunapi)
	ON_BN_CLICKED(IDC_Select, &DockTestApi::OnBnClickedSelect)
END_MESSAGE_MAP()


// DockTestApi diagnostics

#ifdef _DEBUG
void DockTestApi::AssertValid() const
{
	DockClientBase::AssertValid();
}

#ifndef _WIN32_WCE
void DockTestApi::Dump(CDumpContext& dc) const
{
	DockClientBase::Dump(dc);
}
#endif
#endif //_DEBUG


// DockTestApi message handlers
void DockTestApi::OnInitialUpdate()
{
	if(m_bInited)
		return;
	__super::OnInitialUpdate();
	m_bInited = TRUE;



	AdjustLayout();
}


void DockTestApi::AdjustLayout()
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
	int mg = 4;
	if(::IsWindow(c_Input.GetSafeHwnd()))// && _cList.IsWindowVisible()
	{
		c_Input.GetClientRect(rcL);
		c_Input.GetWindowRect(rcLw);
		//+	rcL { LT(0, 0)      RB(241 , 80 )[241 x 80] }	CRect
		//+ rcLw{ LT(1506, 660) RB(1751, 744)[245 x 84] }	CRect 모니터(screen)에서 위치, 크기는 4씩 커진다.
		ScreenToClient(rcLw);

		if(c_Input.GetSafeHwnd() && !c_Input.IsIconic())
			c_Input.SetWindowPos(nullptr, rc.left+mg, rc.top + rcLw.top, rc.Width() - (mg*2), rcLw.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}
	if(::IsWindow(c_Output.GetSafeHwnd()))// && _cProperty.IsWindowVisible()
	{
		c_Output.GetClientRect(rcP);
		c_Output.GetWindowRect(rcPw);
		ScreenToClient(rcPw);
		if(c_Output.GetSafeHwnd())
			c_Output.SetWindowPos(nullptr, rc.left+mg, rcPw.top, rc.Width() - (mg * 2), rc.Height() - (rcPw.top) - mg, SWP_NOACTIVATE | SWP_NOZORDER);
	}

}


void DockTestApi::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
	AdjustLayout();
}

void DockTestApi::OnSetFocus(CWnd* pOldWnd)
{
	DockClientBase::OnSetFocus(pOldWnd);
	//_cProperty.SetFocus();
}


void DockTestApi::OnBnClickedRunapi()
{
	UpdateData();
	auto mfm = (CMainFrame*)AfxGetMainWnd();
	auto& appd = ((CMFCExHttpsSrvApp*)AfxGetApp())->_docApp;

	auto cvu = dynamic_cast<CmnView*>(mfm->GetActiveCmnView());
	if(!cvu)
		return;//throw "There is no active Site Server view.";
	auto doc = cvu->GetDocument();	if(!doc) return;
	auto api = doc->_svr->_api;

	CString smsg;
	SHP<KDatabase> sdb = KDatabase::getDbConnected((PWS)api->_ODBCDSN);
	SHP<KDatabase> sdbLog = KDatabase::getDbConnected((PWS)api->_ODBCDSNlog);

	try
	{
		auto jdoc = ShJVal(Json::Parse((PWS)_Input));
		if(!jdoc)
			throw "The input JSON syntax is wrong.";

		//api->CheckDB(NULL, appd._dbMain);

		//throw new KException("JSON Parse Error.", GetLastError(), 0, L"JSON syntax is wrong.", NULL, __FUNCTION__, __LINE__);
		auto& jbj = *jdoc->AsObject().get();
		CStringW func = jbj.S("func");
		if(func.GetLength() == 0)
			throw "The field \"func\" is required.";
		CStringW uuidW = jbj.S("uuid");//POST 인 경우는 JSON 데이터 안에 있다. GET과 ImageUpload는 URL에 있어서 ss->_sinfo._urlparam["uuid"]로 얻는다.
		CStringA fncA(func);

		if(CApiBase::s_hDllExtra == NULL)
		{
			CApiBase::s_hDllExtra = ::LoadLibraryW(api->getExLibName());// L"MFCLibrary1.dll");
			if(CApiBase::s_hDllExtra == NULL)
			{
				CStringA s; s.Format("Library is not found including (%s).", func);
				throw (PAS)s;
			}
		}
		API_SITE ExDllApiFunc = (API_SITE)::GetProcAddress(CApiBase::s_hDllExtra, (PAS)fncA);
		if(ExDllApiFunc)
		{
			JObj jsr;
			JObj jpa;
			int rv = 0;
			KDatabase* pdb = sdb.get();
			if(jbj.IsObject("params"))//throw "The field \"params\" is required.";
			{
				jpa = jbj.O("params");
				
				if(jpa.SameS("database", L"winpache") && sdbLog)
					pdb = sdbLog.get();
				rv = ExDllApiFunc(*pdb, jpa, jsr, 0);
			}
			else // params 없어도 빈것으로 부른다.
			{
				rv = ExDllApiFunc(*pdb, jpa, jsr, 0);
			}
			JObj jres;
			if(rv == 0)
			{
				//if(jsr.size() > 0)
				jres("response") = jsr;// {Return:OK/Exists/No Data, Desc}
				jres("return") = rv;//S_OK 나 S_FALSE
			}
			else if(rv > 0)// jsr안에 Return:why 들어 있겠고, 더불어 error, return 도 넣는다.
			{///여기 올일이 없다. throw_response 하기 때문
				jres("error") = "API Error.";
				jres("return") = rv;//S_OK 나 S_FALSE
			}// rv < 0 이면 call한 쪽에서 Unkown Error를 넣는다.

			auto Output1 = jres.ToJsonStringW();
			_Output = KwReplaceStr(Output1, L"\n", L"\r\n");//CEdit 이 \n만 있으면 줄을 안바꾼다.
			UpdateData(0);

			if(func == L"ExGetApiList")
			{/// 위 콤보박스에 샘플 함수 넣는다.
				//CComboBox* cb = (CComboBox*)GetDlgItem(IDC_ExList);
				c_ExList.ResetContent();
				ShJArr jr = jsr.Array("ApiList");
				int n = (int)jr->size();
				for(int i = 0; i < n; i++)
				{
					ShJVal jv = jr->GetAt(i);
					CString sfn = jv->S();
					c_ExList.AddString(sfn);
				}
				auto ifs = c_ExList.FindString(0, func);
				if(ifs >= 0)
					c_ExList.SetCurSel(ifs);
			}
		}
	}
	catch(CDBException* e)
	{
		if(e->m_nRetCode == -2 && e->m_strError.IsEmpty() && e->m_strStateNativeOrigin.IsEmpty())
			smsg.Format(L"CDBException: Disconnected.\n");
		else
			smsg.Format(L"CDBException:%d %s - %s\n", e->m_nRetCode, e->m_strError, e->m_strStateNativeOrigin);
	}
	catch(KException* e)
	{
		smsg.Format(L"KException:%s - %s\n", e->m_strError, e->m_strStateNativeOrigin);
	}
	catch(_com_error* e)
	{
		smsg.Format(L"_com_error:%s\n", e->ErrorMessage());
	}
	catch(CException* e)
	{
		auto buf = new TCHAR(1024);
		KAtEnd d_buf([&]() { delete buf; });
		e->GetErrorMessage(buf, 1000);
		TRACE(L"CException:%s - %s %d\n", buf, __FUNCTION__, __LINE__);
		KwMessageBox(buf);
	}
	catch(TException ex)
	{
		TErrCode ec = ex.code();
		smsg.Format(L"catch boost::systemerror %s  - %s %d\n", ec.message().c_str(), __FUNCTIONW__, __LINE__); 
	}
	catch(std::exception& e)
	{	smsg.Format(L"catch std::exception %s  - %s %d\n", e.what(), __FUNCTIONW__, __LINE__);
	}
	catch(PWS e)
	{	smsg.Format(L"catch LPCWSTR %s  - %s %d\n", e, __FUNCTIONW__, __LINE__);
	}
	catch(PAS e)
	{	CString sw(e);
	smsg.Format(L"catch LPCSTR %s  - %s %d\n", sw, __FUNCTIONW__, __LINE__);
	}
	catch(...)
	{	smsg.Format(L"catch ...  - %s %d\n", __FUNCTIONW__, __LINE__);
	}
	if(smsg.GetLength())
		AfxMessageBox(smsg);
}



void DockTestApi::deprecated_CreateDatabase()
{
	ASSERT(0);
	// 콤보박스에 함수 리스트 초기화
	if (c_ExList.GetCount() == 0)
	{
		SetApiFunc(L"ExGetApiList");
		OnBnClickedRunapi();
	}

	int isel = c_ExList.FindString(0, L"ExCreateServerDatabase");
	ASSERT(isel >= 0);
	c_ExList.SetCurSel(isel);
	OnBnClickedSelect();
	//KwMessageBox(L"After the check")
	OnBnClickedRunapi();
}

void DockTestApi::OnBnClickedSelect()
{
	auto isel = c_ExList.GetCurSel();
	if (isel < 0)
		return;
	CString sfnc;
	c_ExList.GetLBText(isel, sfnc);
	SetApiFunc(sfnc);
}

void DockTestApi::SetApiFunc(PWS pfnc)
{
	CString sfnc(pfnc);
CString params = L"{\r\n\
	  \"pr1\":\"val1\",\r\n\
      \"pr2\":10\r\n\
}\r\n";
	JObj jpr;
	if(sfnc == L"ExSelectUser")
	{
		//jpr("limit") = 5;
		params = L"\
   {\r\n\
	  \"database\":\"winpache\",\r\n\
	  \"limit\":5\r\n\
   }\r\n";
	}
	else if (sfnc == L"ExSelectUserQS")
	{
		//jpr("limit") = 10;
		params = L"\
   {\r\n\
	  \"database\":\"winpache\",\r\n\
	  \"limit\":10\r\n\
   }\r\n";
	}
	else if (sfnc == L"ExSelectBizToJson")
	{
		// tbiz
		params = L"\
   {\r\n\
	  \"database\":\"winpache\",\r\n\
	  \"fBizID\":\"biz-0001\"\r\n\
   }\r\n";
	}
	else if (sfnc == L"ExNewBusiness")
	{
		//\"fBizID\":\"biz-0001\"\r\n\
		// tbiz
		params = L"\
   {\r\n\
	  \"database\":\"winpache\",\r\n\
	  \"fUsrIdReg\":\"user-0001\",\r\n\
	  \"fTitle\":\"sample shop data\",\r\n\
	  \"fForm\":\"shop\",\r\n\
	  \"fTel\":\"01012345678\",\r\n\
	  \"fState\":\"close\",\r\n\
	  \"fBegin\":\"11:00:00\",\r\n\
	  \"fEnd\":\"19:00:00\",\r\n\
	  \"fMemo\":\"test insert record\"\r\n\
   }\r\n";
	}
	else if (sfnc == L"ExUpdateBusiness")
	{
		params = L"\
   {\r\n\
	  \"database\":\"winpache\",\r\n\
	  \"fUsrIdReg\":\"user-0001\",\r\n\
	  \"fTitle\":\"sample shop data\",\r\n\
	  \"fForm\":\"shop\",\r\n\
	  \"fTel\":\"01012345679\",\r\n\
	  \"fState\":\"quit\",\r\n\
	  \"fBegin\":\"11:30:00\",\r\n\
	  \"fEnd\":\"19:30:00\",\r\n\
	  \"fMemo\":\"test update record\"\r\n\
   }\r\n";
	}
	else if (sfnc == L"ExRemoveBizClass")
	{
		params = L"\
   {\r\n\
	  \"database\":\"winpache\",\r\n\
	  \"fBizID\":\"biz-0001\",\r\n\
	  \"fBIzClsCD\":\"petBeautyShop\"\r\n\
   }\r\n";
	}
/*
	else if(sfnc == L"ExCreateServerDatabase")
	{
		params = L"\
   {\r\n\
	  \"database\":\"database1\",\r\n\
	  \"collate\":\"utf16_unicode_ci\"\r\n\
   }\r\n";
	}
*/

// 	if(jpr.size() > 0)
// 		params = jpr.ToJsonStringW();

	CString jfnc;
	jfnc.Format(L"\
{\r\n\
   \"func\":\"%s\",\r\n\
   \"params\":\
%s\
}", sfnc, params);

	_Input = jfnc;
	UpdateData(0);
}


