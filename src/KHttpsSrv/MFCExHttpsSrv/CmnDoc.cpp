#include "pch.h"
#include "CmnDoc.h"


#include <direct.h> //_getcwd


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
	InitData();
	/// API 초기화
	auto sApi = make_shared<ApiSite1 >();// CApiBase
	_svr = new CResponse1(sApi);// new ApiSite1());//CApiBase를 계승한 ApiSite1 가 이번 프로젝트 사이트 이다.
	_svr->InitReponser(FALSE);

}





PAS CmnDoc::_cMagicKey = "55EEBBDD44559944770088339992728974";


void CmnDoc::InitData()
{
	AUTOLOCK(_csJdata);
	//auto& _jdata = *_jdata;
	_jdata("_MagicKey") = _cMagicKey;
	_jdata("_GUID") = "";
	_jdata("_port") = 0;
	_jdata("_CacheLife") = 2000;
	_jdata("_bSSL") = FALSE;
	_jdata("_bStaticCache") = TRUE;
	_jdata("_Title") = "(UNNAMED)";
	_jdata("_cachedPath") = "c:\\svr";
	_jdata("_cachedUrl") = "/";
	_jdata("_certificate") = "";
	_jdata("_privatekey") = "";
	_jdata("_dhparam") = "";
	_jdata("_prvpwd") = "";
	_jdata("_ODBCDSN") = "";
	_jdata("_note") = "sample HTTP server";
	_jdata("_defFile") = "index.html";
	_jdata("_rootLocal") = "c:\\svr";
	_jdata("_uploadLocal") = "c:\\svr\\upload";
	_jdata("_rootURL") = "/";
	_jdata("_ApiURL") = "/api";
	_jdata("_SQL") = "select * from tuser";
	_jdata("_SrcImagePath") = "C:\\svr\\upload";

// 	IPv4 myIP{0};
// 	bool bIP = getMyIP(myIP);
	CString localIP, macAddr;
	hres hr = KwGetMacInfo(localIP, macAddr);
	/// 내 주 컴
	// 1. 192.168.35.103, D0:50:99:66:DC:79, Intel(R) Ethernet Connection I217-V
	/// HP 노트북
	// 1. 0.0.0.0, 8C:DC:D4:78:B0:85, Realtek PCIe GBE Family Controller
	bool bMe = macAddr == L"D0:50:99:66:DC:79";


	/// 여기서 auto에 붙는 '&' 는 아주 중요 하다. 복사냐, 참조냐. 여기서는 참조여야 데이터 들어 갖지
	auto& arClst = *_jdata.AMake("Clustering");
	/// 아래는 regitry에 진짜 있는 값인데, ODBC셋팅이 없는 경우 이걸 초기값으로 쓴다.
	{
		auto jobcl0 = JObj();
		CString url = L"http://localhost:80";
		jobcl0("mode") = "main";// 이건 상황에 따라
		jobcl0("action") = "open";//설정해 둔 값이고
		/// "listening" 이면 자기도 response 하면서 분배도 하고, 
		/// "distribute" 이면 분배만 한다. 분배는 if(svr->_fncCluster) 따로 해줄께 없다.
		jobcl0("url") = url;
		arClst.Add(jobcl0);// 자기 자신은 삭제가 안된다.

		if(bMe)// 내자리면 디버깅을 위해서 넣는다.
		{
#ifdef _DEBUG
			auto jobcl = JObj();
			jobcl("mode") = "support";
			url = L"http://192.168.35.175:80";//HP 노트북에 병렬 처리 테스트
			jobcl("action") = "open";//병렬처리
			jobcl("url") = url;
			arClst.Add(jobcl);

			auto jobcle = JObj();/// Error 유발 시키는 디버그 항
			url = L"http://192.168.35.175:8888";//HP 노트북에 병렬 처리 테스트
			jobcle("mode") = "support";// error test 하기 위해 일부러
			jobcle("action") = "open";//병렬처리
			jobcle("url") = url;
			arClst.Add(jobcle);
#endif // _DEBUG
		}

		{
			auto jobclR = JObj();
			url = L"http://192.168.0.123:80";
			jobclR("mode") = "support";//사용안함
			jobclR("action") = "closed";//사용안함
			jobclR("url") = url;
			arClst.Add(jobclR);
		}
		{
			auto jobclR = JObj();
			url = L"http://example.mycompany.com:8081";
			jobclR("mode") = "support";//사용안함
			jobclR("action") = "closed";//사용안함
			jobclR("url") = url;
			arClst.Add(jobclR);
		}

	}
}




void CmnDoc::SampleServer()
{
	CmnDoc* doc = this;// GetDocument();
	if(doc == NULL)
		return;
	AUTOLOCK(doc->_csJdata);

	char curDir[1000];
	_getcwd(curDir, 1000);
	TRACE("Cur dir is %s\n", curDir);
	//Cur dir is C:\Dropbox\Proj\STUDY\boostEx\CppServer\MFCAppServerEx1
	// At debugging time, curdir is project folder.
	//https->main_https(0, NULL);
	doc->_jdata("_ApiURL") = "/api";
	CStringA svrkeyIIS = "certificate.pem";//server certificate 
	CStringA prikeyIIS = "private.pem";
	//	CStringA chainIIS = "ca_bundle.pem";//Error key values mismatch
	CStringA dhparam = "dh4096.pem";

	WCHAR my_documents[MAX_PATH];//CSIDL_PERSONAL
	HRESULT result = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, my_documents);
	CStringW flPrj = my_documents;
	PWS tdir = L"\\Winpache\\cert\\";
	doc->_jdata("_SrcImagePath") = "C:\\svr\\upload";
	doc->_jdata("_port") = 80;

#ifdef _DEBUGx
	doc->_ODBCDSN = "DSN=FatUs";
	doc->_cachedPath = "C:\\Dropbox\\Proj\\ODISO\\Src\\IIS";
	doc->_rootLocal = "C:\\Dropbox\\Proj\\ODISO\\Src\\IIS";

	CStringA fldDhparam = "C:\\Dropbox\\Proj\\KHttpData\\cert";
	CStringA fldKey = "C:\\Dropbox\\Proj\\KHttpData\\cert";
#else
	doc->_jdata("_ODBCDSN") = "DSN=MyOdbcDsn";
	doc->_jdata("_cachedPath") = "C:\\svr";
	doc->_jdata("_rootLocal") = "C:\\svr";

	/// for dhparam
	CStringA fldDhparam(flPrj + tdir);// = "C:\\svr\\certi\\";// 혼자 크지 잘커라
	/// for key
	CStringA fldKey(flPrj + tdir);
#endif

	doc->_jdata("_certificate") = fldKey + svrkeyIIS;
	doc->_jdata("_privatekey") = fldKey + prikeyIIS;
	doc->_jdata("_dhparam") = fldDhparam + dhparam;
	doc->_jdata("_prvpwd") = "";
}


//안씀. UI부분 떄문에
int CmnDoc::CheckData()
{
	auto doc = this;
	return 0;
}



bool CmnDoc::IsStarted()
{
	AUTOLOCK(_csJdata);
	if(_jdata.I("_bSSL"))
	{
		if(!_bodySSL)
			return false;
		CMyHttps* https = GetServer();
		if(https && https->_server)
			return https->_server->IsStarted();
	}
	else
	{
		if(!_body)
			return false;
		CMyHttp* http = GetServerNoSSL();
		if(http && http->_server)
			return http->_server->IsStarted();
	}
	return false;
}



//서버 열기 전에 API함수 쓸기전에. image migration...
// CMainFrame.OnCreate 보다 여기 OnInitalUpdate가 먼저 불려 진다.
void CmnDoc::InitApi()
{
	AUTOLOCK(_csJdata);
	auto doc = this;// GetDocument();
	/// 캐시 폴더는 그냥 루트로 고정. 대신 StaticCache bool을 추가
	_jdata("_cachedPath") = _jdata("_rootLocal");
	_jdata("_cachedUrl") = "/";

	_svr->_api->_GUID = _jdata.SA("_GUID");
	_svr->_api->_ODBCDSN = _jdata.S("_ODBCDSN");
	_svr->_api->_rootLocal = _jdata.S("_rootLocal");
	_svr->_api->_defFile = _jdata.SA("_defFile");
	_svr->_api->_uploadLocal = _jdata.S("_uploadLocal");
	_svr->_api->_rootURL = _jdata.SA("_rootURL");
	_svr->_api->_ApiURL = _jdata.SA("_ApiURL");
}

int CmnDoc::InitServerValue()
{
	AUTOLOCK(_csJdata);
	if(_jdata.I("_bSSL"))
	{
		CMyHttps* https = GetServer();
		https->_port = _jdata.I("_port");
		https->_bStaticCache = _jdata.I("_bStaticCache");
		https->_cachedPath = _jdata.SA("_cachedPath");
		https->_cachedUrl = _jdata.SA("_cachedUrl");
		https->_certificate = _jdata.SA("_certificate");
		https->_privatekey = _jdata.SA("_privatekey");
		https->_dhparam = _jdata.SA("_dhparam");
		https->_prvpwd = _jdata.SA("_prvpwd");
	}
	else
	{
		CMyHttp* http = GetServerNoSSL();
		http->_port = _jdata.I("_port");
		http->_bStaticCache = _jdata.I("_bStaticCache");
		http->_cachedPath = _jdata.SA("_cachedPath");
		http->_cachedUrl = _jdata.SA("_cachedUrl");
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
	if(GetSSL())
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
	if(GetSSL())
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

void CmnDoc::RestartServer()
{
	if(GetSSL())
	{
		CMyHttps* https = GetServer();
		https->restart_server();
	}
	else
	{
		CMyHttp* http = GetServerNoSSL();
		http->restart_server();
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
