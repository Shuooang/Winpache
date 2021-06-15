// DockTestApi.cpp : implementation file
//

#include "pch.h"
#include "DockTestApi.h"
#include "Mainfrm.h"
#include "resource.h"
#include "Response1.h"
#include "ApiBase.h"

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
	auto cvu = dynamic_cast<CmnView*>(mfm->GetActiveCmnView());
	CString smsg;
	try
	{
		if(!cvu)
			throw "There is no active window.";

		auto doc = cvu->GetDocument();	if(!doc) return;
		auto api = doc->_svr->_api;
		auto jdoc = ShJVal(Json::Parse((PWS)_Input));
		if(!jdoc)
			throw "The input JSON syntax is wrong.";
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
			api->CheckDB();
			if(!jbj.Has("params"))
				throw "The field \"params\" is required.";
			JObj jpa = jbj.Oref("params");
			JObj jsr;

			int rv = ExDllApiFunc(api->_db, jpa, jsr, 0);
			_Output = jsr.ToJsonStringW();
			_Output = KwReplaceStr(_Output, L"\n", L"\r\n");//CEdit 이 \n만 있으면 줄을 안바꾼다.
			UpdateData(0);

			if(func == L"ExGetApiList")
			{
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



void DockTestApi::CreateDatabase()
{
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

void DockTestApi::SetApiFunc(PWS sfnc)
{

CString params = L"{\r\n\
	  \"pr1\":\"val1\",\r\n\
      \"pr2\":10\r\n\
}\r\n";
	JObj jpr;
	if(sfnc == L"ExSelectUUID")
	{
		jpr("limit") = 5;
		params = L"\
   {\r\n\
	  \"limit\":5\r\n\
   }\r\n";
	}
	else if(sfnc == L"ExSelectUUIDQS")
	{
		jpr("limit") = 10;
		params = L"\
   {\r\n\
	  \"limit\":10\r\n\
   }\r\n";
	}
	else if(sfnc == L"ExCreateServerDatabase")
	{
		params = L"\
   {\r\n\
	  \"database\":\"HttpsSvr\"\r\n\
	  \"collate\":\"utf16_unicode_ci\"\r\n\
   }\r\n";
	}

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


