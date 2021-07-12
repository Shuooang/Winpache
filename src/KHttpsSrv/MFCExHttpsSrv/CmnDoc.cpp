#include "pch.h"
#include "CmnDoc.h"




#include "KwLib64/DlgTool.h"
#include "Response1.h"
#include "ApiSite1.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CmnDoc::CmnDoc()
	: _svr(NULL)
	, _d_svr([&]()->void
		{
			DeleteMeSafe(_svr);
		}) // ~CMFCAppServerEx2() 에서 해도 되는데. 이 방법 한번 써봄.
{
	/// API 초기화
	auto sApi = make_shared<ApiSite1 >();// CApiBase
	_svr = new CResponse1(sApi);// new ApiSite1());//CApiBase를 계승한 ApiSite1 가 이번 프로젝트 사이트 이다.
	_svr->InitReponser(FALSE);

}



//서버 열기 전에 API함수 쓸기전에. image migration...
// CMainFrame.OnCreate 보다 여기 OnInitalUpdate가 먼저 불려 진다.
void CmnDoc::InitApi()
{
	auto doc = this;// GetDocument();
	/// 캐시 폴더는 그냥 루트로 고정. 대신 StaticCache bool을 추가
	_cachedPath = _rootLocal;
	_cachedUrl = "/";

	doc->_svr->_api->_GUID = doc->_GUID;
	doc->_svr->_api->_ODBCDSN = doc->_ODBCDSN;
	doc->_svr->_api->_rootLocal = doc->_rootLocal;
	doc->_svr->_api->_defFile = doc->_defFile;
	doc->_svr->_api->_uploadLocal = doc->_uploadLocal;
	doc->_svr->_api->_rootURL = doc->_rootURL;
	doc->_svr->_api->_ApiURL = doc->_ApiURL;
}

int CmnDoc::InitServerValue()
{
	auto doc = this;
	if(doc->_bSSL)
	{
		CMyHttps* https = doc->GetServer();
		https->_port = doc->_port;
		https->_bStaticCache = doc->_bStaticCache;
		https->_cachedPath = (PAS)doc->_cachedPath;
		https->_cachedUrl = (PAS)doc->_cachedUrl;
		https->_certificate = (PAS)doc->_certificate;
		https->_privatekey = (PAS)doc->_privatekey;
		https->_dhparam = (PAS)doc->_dhparam;
		https->_prvpwd = (PAS)doc->_prvpwd;
	}
	else
	{
		CMyHttp* http = doc->GetServerNoSSL();
		http->_port = doc->_port;
		http->_bStaticCache = doc->_bStaticCache;
		http->_cachedPath = (PAS)doc->_cachedPath;
		http->_cachedUrl = (PAS)doc->_cachedUrl;
	}
	return 0;
}

void CmnDoc::JsonToData(ShJObj& sjobj, bool bToJson)
{
	//SrvDoc::JsonToData 에: UI에서 읽어 와야 하므로
}


void CmnDoc::Serialize(CArchive& ar)
{
	if(ar.IsStoring())//write
	{
		ShJObj sjobj = std::make_shared<JObj>();
		JsonToData(sjobj, ar.IsStoring());

		JObj& js = *sjobj;
		js.DeleteKey("_prvpwd");//각문서에는 저장 하지 않는다.

		CStringA sUtf8 = js.ToJsonStringUtf8();
		CFile* fr = ar.GetFile();
		fr->Write((PAS)sUtf8, sUtf8.GetLength());
	}
	else //read
	{
		CFile* fr = ar.GetFile();
		int len = (int)fr->GetLength();
		CStringA sa;
		char* buf = sa.GetBufferSetLength(len);
		fr->Read(buf, len);
		CString sWstr;
		KwUTF8ToWchar(buf, sWstr);

		auto jdoc = ShJVal(Json::Parse((PWS)sWstr));
		if(jdoc.get() == nullptr)
		{
			AfxMessageBox(L"파일 포맷 오류.");
			return;
		}
		ShJObj sjobj = jdoc->AsObject();
		(*sjobj)("_bDbConnected") = FALSE;//파일 열면 초기화 하고, 연결되면 TRUE. 서버크러시떄 복구 위해 하는 거
		JsonToData(sjobj, ar.IsStoring());
	}
}

// 

void CmnDoc::StartServer()
{
	/// view에서 이미 커다란 함수 StartServerT 라는 템플릿 함수를 불러야 하므로
	/// 여기서는 추가 적인 부분만 한다.
}

void CmnDoc::StopServer()
{
	if(_bSSL)
	{
		CMyHttps* https = GetServer();
		https->stop_server();
	}
	else
	{
		CMyHttp* http = GetServerNoSSL();
		http->stop_server();
	}
}

void CmnDoc::ShutdownServer()//?Shutdown 
{
	if(_bSSL)
	{
		CMyHttps* https = GetServer();
		https->shutdown_server();
	}
	else
	{
		CMyHttp* http = GetServerNoSSL();
		http->shutdown_server();
	}
}

int CmnDoc::FreeDllLibrary()
{
	CString smsg;
	//auto app = (CMFCExHttpsSrvApp*)AfxGetApp();
	auto api = (ApiSite1*)&_svr->_api;
	if(CApiBase::s_hDllExtra)
	{
		BOOL b = ::FreeLibrary(CApiBase::s_hDllExtra);
		if(b)
		{
			CApiBase::s_hDllExtra = NULL;
			return 0;
			//KwMessageBox(L"Now, the API library [%s] can be changed.", _svr->_api->getExLibName());
		}
		else
			return -1;
			//KwMessageBox(L"Error, while free library [%s].", _svr->_api->getExLibName());
	}
	//else KwMessageBox(L"You can replace the API library [%s].", _svr->_api->getExLibName());

	return 0;
	// 	Sleep(1000);
// 	CApiBase::s_hDllExtra = ::LoadLibraryW(L"MFCLibrary1.dll");

}
