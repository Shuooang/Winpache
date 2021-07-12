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
		}) // ~CMFCAppServerEx2() ���� �ص� �Ǵµ�. �� ��� �ѹ� �ẽ.
{
	/// API �ʱ�ȭ
	auto sApi = make_shared<ApiSite1 >();// CApiBase
	_svr = new CResponse1(sApi);// new ApiSite1());//CApiBase�� ����� ApiSite1 �� �̹� ������Ʈ ����Ʈ �̴�.
	_svr->InitReponser(FALSE);

}



//���� ���� ���� API�Լ� ��������. image migration...
// CMainFrame.OnCreate ���� ���� OnInitalUpdate�� ���� �ҷ� ����.
void CmnDoc::InitApi()
{
	auto doc = this;// GetDocument();
	/// ĳ�� ������ �׳� ��Ʈ�� ����. ��� StaticCache bool�� �߰�
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
	//SrvDoc::JsonToData ��: UI���� �о� �;� �ϹǷ�
}


void CmnDoc::Serialize(CArchive& ar)
{
	if(ar.IsStoring())//write
	{
		ShJObj sjobj = std::make_shared<JObj>();
		JsonToData(sjobj, ar.IsStoring());

		JObj& js = *sjobj;
		js.DeleteKey("_prvpwd");//���������� ���� ���� �ʴ´�.

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
			AfxMessageBox(L"���� ���� ����.");
			return;
		}
		ShJObj sjobj = jdoc->AsObject();
		(*sjobj)("_bDbConnected") = FALSE;//���� ���� �ʱ�ȭ �ϰ�, ����Ǹ� TRUE. ����ũ���Ë� ���� ���� �ϴ� ��
		JsonToData(sjobj, ar.IsStoring());
	}
}

// 

void CmnDoc::StartServer()
{
	/// view���� �̹� Ŀ�ٶ� �Լ� StartServerT ��� ���ø� �Լ��� �ҷ��� �ϹǷ�
	/// ���⼭�� �߰� ���� �κи� �Ѵ�.
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
