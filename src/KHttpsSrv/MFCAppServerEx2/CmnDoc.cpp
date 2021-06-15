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

void CmnDoc::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
	{
		//CJsonPbj js;
		JObj js;

		js("_port") = _port;

		if (_GUID.IsEmpty())
			_GUID = KwGetFormattedGuid();
		KJSPUT(_GUID);
		KJSPUT(_port);
		KJSPUT(_bSSL);
		KJSPUT(_bStaticCache);
		KJSPUT(_CacheLife);
//		KJSPUT(_Title);
// 		KJSPUT(_cachedPath);
// 		KJSPUT(_cachedUrl);
		KJSPUT(_certificate);
		KJSPUT(_privatekey);
		KJSPUT(_prvpwd);
		KJSPUT(_dhparam);
		KJSPUT(_ODBCDSN);
		KJSPUT(_rootLocal);
		KJSPUT(_defFile);
		KJSPUT(_uploadLocal);
		KJSPUT(_rootURL);
		KJSPUT(_ApiURL);
// 		KJSPUT(_UdpSvr);
// 		KJSPUT(_portUDP);
		KJSPUT(_SQL);
		KJSPUT(_SrcImagePath);
		KJSPUT(_note);
		//	JSPUT(_SrcTable);
		//	JSPUT(_SrcKeyField);
		CFile* fr = ar.GetFile();

		CStringA sUtf8 = js.ToJsonStringUtf8();
		fr->Write((PAS)sUtf8, sUtf8.GetLength());
	}
	else
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
		auto& js = *jdoc->AsObject().get();

		KJSGETS(_GUID);
		if (_GUID.IsEmpty())
			_GUID = KwGetFormattedGuid();
		KJSGETI(_port);// 숫자인 경우
		KJSGETI(_bSSL);// 숫자인 경우
		KJSGETI(_bStaticCache);// 숫자인 경우
		KJSGETI(_CacheLife);// 숫자인 경우
// 		KJSGETS(_Title);
// 		KJSGETS(_cachedPath);
// 		KJSGETS(_cachedUrl);
		KJSGETS(_certificate);
		KJSGETS(_privatekey);
		KJSGETS(_dhparam);
#ifdef _DEBUG
		KJSGETS(_prvpwd);
#endif
		KJSGETS(_ODBCDSN);
		KJSGETS(_rootLocal);
		KJSGETS(_defFile);
		KJSGETS(_uploadLocal);
		//if (_rootURL.IsEmpty())
		KJSGETS(_rootURL);
		KJSGETS(_ApiURL);

		KJSGETS(_SQL);
		KJSGETS(_SrcImagePath);
		KJSGETS(_note);
// 		KJSGETS(_UdpSvr);
// 		KJSGETI(_portUDP);// 숫자인 경우
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
