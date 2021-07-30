/*!
	\file https_server.cpp
	\brief HTTPS server example
	\author Ivan Shynkarenka
	\date 30.04.2019
	\copyright MIT License
*/

#include "pch.h"

#include <iostream>
#include <map>
#include <mutex>
#include <tuple>

#include "HttpsSvr.h" // ���� ����� ��������, �и� �Ͽ� �����.

// MFC windows �ۿ��� �޸� ���� ����µ�,
//�Ȱ��� �ڵ�� CRT�� ������ �ϸ� �޸� ���� ���µ�, �״�� MFC�� ��ġ�� ���� �����.
// �׷��� �Ʒ� �õ� �غ����� �ҿ� ����.
// ������ �����ΰ��� �����߿� �þ� ���� �ʰ�, ���� ��ŸƮ �ϸ� ����Ƿ� �����ص� �ȴ�.
//#ifdef _DEBUG
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
//// allocations to be of _CLIENT_BLOCK type
//#else
//#define DBG_NEW new
//#endif
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
//

//
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//shared_ptr<CMyHttps> CMyHttps::s_body;

// CKTrace std_cout;// Debug �� �߰� ��� ���ÿ� �� ������
// CKTrace std_coutD(true);// Debug�� ����������

void CMyHttps::SampleData(int port, const char* vpath, const char* path)// error :"../www/api")
{
	_port = port;
	if(path)
		_cachedPath = path;
	if(vpath)
		_cachedUrl = vpath;

// 	string keyFolder = "C:/Dropbox/Proj/STUDY/boostEx/CppServer/CppServer-master/tools/certificates/";
// 	string keyFolderIIS = "C:/Dropbox/Proj/ODISO/Src/IIS/.well-known/pki-validation/";
	string keyFolder    = "C:/svr/certificates/";
	string keyFolderIIS = "C:/svr/certificates/";

	string dh = "dh4096.pem";
	string svrkeyIIS = "certificate.pem";//server certificate 
	string prikeyIIS = "private.pem";
	string chainIIS = "ca_bundle.pem";//Error key values mismatch
	_certificate = keyFolderIIS + svrkeyIIS;
	_privatekey = keyFolderIIS + prikeyIIS;
	_dhparam = keyFolder + dh;
	_prvpwd = "";
	//�ֵ�� ���� ��� Ű��� ȥ�� ũ�� �� Ŀ��
}

CMyHttps::~CMyHttps()
{
	stop_server();// CMFCServerView::OnDestroy() ���� �̹� �ҷ�����, ������ ���
	std_cout << "~CMyHttps() destroyed." << std_endl;
}


int CMyHttps::start_server()//shared_ptr<CAsioService>* pService)
{
// 	if(_service && _service->IsStarted() && _server && _server->IsStarted())
// 		return 1;
	// Create a new Asio service
	try 
	{
		if(_service)
		{/// ������ û��
			auto service = _service;
			if(service->IsStarted())
				service->Stop();
		
			_service.reset();
		}

		ASSERT(!_service);
		_service = std::make_shared<CAsioService>(4);
		std_cout << "Asio service starting...";
		if(_service->Start())
			std_cout << "Done!" << std_endl;
		else
			std_cout << "Asio service is failed!" << std_endl;
		///_service = CAsioSvcInst::getAsioService(1)->_service;
		/// site�� ��Ʈ ���� ���� _service�� ����� ����. �׷��� ���� ������ �־�� �ҵ�.
	
		// Start the Asio service
/*
		
		if(!_service->IsStarted())
		{
			if(_service->Start())
				std_cout << "Done!" << std_endl;
			else
				std_cout << "Asio service is failed!" << std_endl;
		}
		else
			std_cout << "Asio service is already started!!" << std_endl;
*/

		





		///[ Create and prepare a new SSL server context
		auto context = std::make_shared<CppServer::Asio::SSLContext>(BAsio::ssl::context::tlsv12);
		context->set_password_callback([&](size_t max_length, BAsio::ssl::context::password_purpose purpose) -> string
			{
				return this->_prvpwd;//�ֵ�� ���� ��� Ű��� ȥ�� ũ�� �� Ŀ��
			});
		context->use_certificate_chain_file(_certificate);// ���� ��� chain�� ca_bundle
		context->use_private_key_file(_privatekey, BAsio::ssl::context::pem);
		context->use_tmp_dh_file(_dhparam);
		///] No SSL �� �ٸ� ����

		if(_server)
		{/// ������ û��
			auto server1 = _server;
			if(server1->IsStarted())
				server1->Stop();

			_server.reset();
		}
		ASSERT(!_server);

		// Create a new HTTPS server
		_server = std::make_shared<HTTPSCacheServer>(_service, context, _port);
		// ������ ���� �Ǹ� 
		if(!_server->_fncOnTrace)
			_server->_fncOnTrace = _fncOnTrace;//?ExTrace 4 CMyHttps -> _server (���ʿ�)

		//_cachedPath = "C:/Dropbox/Proj/STUDY/boostEx/CppServer/CppServer-master/www/api";"C:\Dropbox\Proj\STUDY\boostEx\CppServer\CppServer-master\www\api";
		if(_bStaticCache && _cachedPath.size() > 0 && _cachedUrl.size() > 0)
		{
			string cachedPath = (PAS)KwReplaceStr(CStringA(_cachedPath.c_str()), "\\", "/");
			_server->AddStaticContent(cachedPath, _cachedUrl);
			/// ���⼭ ���� ����, Dropbox ����Ʈ����ȭ�� ���÷� �ȵȰ�� ������ ����� ���о �׷���.
		}

		// Start the server
		std_cout << "Server starting..." << std_endl;
		if(!_server->IsStarted())
		{
			if(_server->Start())
				std_cout << "Done! (SSL)" << std_endl;
			else
			{
				std_cout << "Server starting ERROR!!!!!!!!" << std_endl;
				return -1;
			}
		}
		else
			std_cout << "Server is already started!!" << std_endl;
	}
	catch(TException ex)
	{
		TErrCode ec = ex.code();
		std_cout << "Error " << ec.message() << std_endl;
		return -3;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}
	catch(std::exception& e)
	{
		std_cout << "Error " << e.what() << std_endl;
		return -4;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}
	catch(std::string& es)
	{
		std_cout << "Error " << es << std_endl;
		return -5;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}
	catch(int es)
	{
		std_cout << "Error " << es << std_endl;
		return -6;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}
	catch(PAS es)
	{
		std_cout << "Error " << es << std_endl;
		return -6;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}
	catch(...)
	{
		return -7;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}
	return 0;
}


void CMyHttps::restart_server()
{
	if(_service)
	{
		if(_server)
			HttpCmn::restart_server(_service, _server);
	}
}



void CMyHttps::stop_server()
{
	if(_service)
	{
		if(_server)
				HttpCmn::stop_server(_service, _server);
	}
}

void CMyHttps::shutdown_server()//?Shutdown 
{
	if(_service)
	{
		if(_server)
			HttpCmn::shutdown_server(_service, _server);
	}
}

void HTTPSCacheServer::onError(int error, const string& category, const string& message)
{
	std_cout << "HTTPS server caught an error with code " << error << " and category '" << category << "': " << message << std_endl;
	// 10038 system "message = "���� �̿��� ��ü�� �۾��� �õ��߽��ϴ�"
	if(_fncOnError.get())
		(*_fncOnError)(error, category, message);
}


void HTTPSCacheServer::onStarted() 
{
	std_cout << "���� ����. HTTPCacheServer::onStarted " << std_endl;
	if(_fncOnStarted.get())
		(*_fncOnStarted)();
}



void HTTPSCacheServer::onStopped() 
{
	std_cout << "HTTPCacheServer::onStopped " << std_endl;
	if(_fncOnStopped.get())
		(*_fncOnStopped)();

// 	string esm = _eStopMode;// {"none"};// "stop", "shutdown", "restart",
// 	if(esm == "shutdown")
// 	{
// 
// 	}
}


//C:\Dropbox\Proj\STUDY\boostEx\CppServer\CppServer-master\include\server\asio\ssl_session.h
void HTTPSCacheServer::onConnected(shared_ptr<SSLSession>& session) 
{
	std_coutD << "HTTPCacheServer::onConnected " << std_endl;
	if(_fncOnConnected.get())
		(*_fncOnConnected)((HTTPSCacheSession&)*session);
}


/// CMyHttp�� �Ͱ� �ٸ��κ�
void HTTPSCacheServer::onHandshaked(shared_ptr<SSLSession>& session)
{
	std_coutD << "HTTPCacheServer::onHandshaked " << std_endl;
	if(_fncOnHandshaked.get())
		(*_fncOnHandshaked)((HTTPSCacheSession&)*session);
}


void HTTPSCacheServer::onDisconnected(shared_ptr<SSLSession>& session) 
{
	std_coutD << "HTTPCacheServer::onDisconnected " << std_endl;
	if(_fncOnDisconnected.get())
		(*_fncOnDisconnected)((HTTPSCacheSession&)*session);
}



/// ///////////////////////// Session ////////////////////////////////////////////
/// ���� ��ü�� HTTP�� HTTPS �� �޶� �Ȱ�����
void KSessionInfo::GatherBasicInfo(const HTTPRequest& request, string ips)//, KSessionInfo& sinf)//string& url, string& sparams, string& dir, string& stCached,							  std::map<string, string>& urlparam, std::map<string, string>& headers)
{
	///[ �⺻ ������ ����. _url, _dir, _sparams, _urlparam
	_ip = ips; // client IP �α� ������ ���δ�.
	_method = request.method();
	//string url0(request.url());//key	"/apix?key=xxx&srl=5111"	url0	"/images/xxxxx.jpeg"
	_url = CppCommon::Encoding::URLDecode(request.url());
	size_t iq = _url.find('?');//, 0Ui64);
	if(iq != string::npos) {
		_dir = _url.substr(0, iq);
		_sparams = _url.substr(iq + 1);
		KwUrlParamToMap(_sparams, _urlparam);
	}
	else
		_dir = _url;

	//http header�� map�� ����. Content-Type: image/jpeg
	auto szHd = request.headers();
	for(size_t i=0;i<szHd;i++) {
		auto hd = request.header(i);
		_headers[(string)std::get<0>(hd)] = std::get<1>(hd);
	}
	///] �⺻ ������ ����. _url, _dir, _sparams, _urlparam
}


void HTTPSCacheSession::onReceivedRequest(const HTTPRequest& request)
{
	std_coutD << "HTTPSCacheSession::onReceivedRequest" << std_endl;
	HTTPResponse& res1 = response();
	auto svr = server();	//+svr	shared_ptr<SSLServer>
	shared_ptr<SSLServer>& svr1 = server();	//+svr1	shared_ptr{_fncGET=empty _fncPOST=empty} [5 strong refs, 1 weak ref] [{_Storage={_Value={_fncGET=empty _fncPOST=empty }} }]	shared_ptr<CppServer::Asio::SSLServer>&
	auto psvr = svr1.get();
	HTTPSCacheServer* svr2 = dynamic_cast<HTTPSCacheServer*>(psvr);
	HttpCmn::onReceivedRequest(svr2, this, (HTTPRequest&)request);
}
#ifdef _DEBUGxxx
-_sinfo	{_ssid="982ca166-ec98-11eb-8dbb-001a7dda7113" _ip="127.0.0.1" _method="POST" ...}	KSessionInfo
+	_ssid	"982ca166-ec98-11eb-8dbb-001a7dda7113"
+	_ip	"127.0.0.1"
+	_method	"POST"
+	_url	"/api?func=ExSelectUserQS&srl=2"
+	_sparams	"func=ExSelectUserQS&srl=2"
+	_dir	"/api"
+	_stCached	""
	_status	200	int
-	_urlparam	{ size=2 }	KStrMap
+		["func"]	"ExSelectUserQS"
+		["srl"]	"2"
-	_headers	{ size=10 }	KStrMap
+		["Accept"]	"*/*"
+		["Accept-Encoding"]	"gzip, deflate"
+		["Accept-Language"]	"ko"
+		["Cache-Control"]	"no-cache"
+		["Connection"]	"Keep-Alive"
+		["Content-Length"]	"115"
+		["Content-Type"]	"application/json"
+		["Host"]	"localhost"
+		["UA-CPU"]	"AMD64"
+		["User-Agent"]	"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.2; Win64; x64; Trident/7.0; .NET4.0C; .NET4.0E; .NET CLR 2.0.50727; .NET CLR 3.0.30729; .NET CLR 3.5.30729)"
#endif // _DEBUGxxx


//?kdw virtual�� �ƴѰ� virtual�� ���� override�Ѵ�.
void HTTPSCacheSession::onReceivedRequestInternal(const HTTPRequest& request)
{
	std_coutD << "HTTPSCacheSession::onReceivedRequestInternal" << std_endl;
	//HttpCmn::onReceivedRequestInternal((HTTPRequest&)request);
	if(_fncOnReceivedRequestInternal)
		(*_fncOnReceivedRequestInternal)(this, (HTTPRequest&)request);
	__super::onReceivedRequestInternal(request);
}

void HTTPSCacheSession::onReceived(const void* buffer, size_t size) {
	std_coutD << "HTTPSCacheSession::onReceived " << size << std_endl;
	__super::onReceived(buffer, size);// �̰� �Ⱥθ��� onReceivedRequest�� �Ⱥҷ�
	if(_fncOnReceived.get())
		(*_fncOnReceived)(this, buffer, size);
}

///?warning onSent, onSentKw �� ������ �뷮�� ���� �ι� �Ҹ��� �ִ�.
void HTTPSCacheSession::onSent(size_t sent, size_t pending) {
	std_coutD << "HTTPSCacheSession::onSent " << sent << ", " << pending << std_endl;
	if(_fncOnSent.get())
		(*_fncOnSent)(this, sent, pending);
}
void HTTPSCacheSession::onSentKw(uint8_t* data, size_t szAll) {
	std_coutD << "HTTPSCacheSession::onSentKw " << szAll << std_endl;
	if(_fncOnSentKw.get())
		(*_fncOnSentKw)(this, data, szAll);
	//_sinfo.Clear(); no no ���⼭ �ع�����, onSent Lambda���� �Ⱥ�����. 
	/// ������ �Ҹ��µ�, �� �Ҹ��� ���� ���־���.
}

void HTTPSCacheSession::onReceivedRequestError(const HTTPRequest& request, const string& error)
{
	std_cout << "Session Request error: " << error << std_endl;
}

void HTTPSCacheSession::onReceivedRequestHeader(const HTTPRequest& request)
{// base�� �� �Լ��� __super�� �θ� �ʿ� ����.
	//std_cout << "HTTPSCacheSession::onReceivedRequestHeader- len: " << std_endl;//request.body_length <<  
}

void HTTPSCacheSession::onError(int error, const string& category, const string& message)
{
	std_cout << "HTTPSCacheSession::onError with code: " << error << " category: '" << category << "': " << message << std_endl;
}

void HTTPSCacheSession::onConnected() {
	std_coutD << "HTTPSCacheSession::onConnected " << std_endl;
}

void HTTPSCacheSession::onHandshaked() {
	std_coutD << "HTTPSCacheSession::onHandshaked " << std_endl;
}

void HTTPSCacheSession::onDisconnected() {
	std_coutD << "HTTPSCacheSession::onDisconnected" << std_endl << std_endl;
	__super::onDisconnected();
}


void HTTPSCacheSession::onEmpty() {
	//std_cout << "HTTPSCacheSession::onEmpty " << std_endl;
}

/*
Hello World!
HTTPS server port: 9480
HTTPS server static content path: ../www/api
HTTPS server website: https://localhost:9480/api/index.html

Asio service starting...Done!
Server starting...Done!
Press Enter to stop the server or '!' to restart the server...
HTTPSCacheServer::onStarted
HTTPSCacheSession::onConnected
HTTPSCacheServer::onConnected
HTTPSCacheSession::onConnected
HTTPSCacheServer::onConnected
HTTPSCacheSession::onError with code: 336151574 category: 'asio.ssl': sslv3 alert certificate unknown
HTTPSCacheSession::onDisconnected

HTTPSCacheServer::onHandshaked
HTTPSCacheSession::onError with code: 336151574 category: 'asio.ssl': sslv3 alert certificate unknown
HTTPSCacheSession::onDisconnected

HTTPSCacheServer::onHandshaked
HTTPSCacheSession::onConnected
HTTPSCacheServer::onConnected
HTTPSCacheSession::onHandshaked
HTTPSCacheServer::onHandshaked
HTTPSCacheSession::onEmpty
HTTPSCacheSession::onReceived >> super GET /api/?key=xxx HTTP/1.1
Host: 114.201.133.81:9480
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36
	 v��� ���� '/' �Ѱ���
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*//*;q=0.8, application/signed-exchange;v=b3;q=0.9
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br
Accept-Language: ko-KR, ko;q=0.9, en-US;q=0.8, en;q=0.7, und;q=0.6

605
HTTPSCacheSession::onReceivedRequest

Request method: GET
Request URL: /api/?key=xxx
Request protocol: HTTP/1.1
Request headers: 12
Host: 114.201.133.81:9480
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36
																									  v��� ���� '/' �Ѱ���
Accept: text/html, application/xhtml+xml, application/xml;q=0.9, image/avif, image/webp, image/apng, *//*;q=0.8,application/signed-exchange;v=b3;q=0.9
Sec-Fetch-Site: none
Sec-Fetch-Mode: navigate
Sec-Fetch-User: ?1
Sec-Fetch-Dest: document
Accept-Encoding: gzip, deflate, br
Accept-Language: ko-KR,ko;q=0.9,en-US;q=0.8,en;q=0.7,und;q=0.6
Request body:0

HTTPSCacheSession::onSent
HTTPSCacheSession::onEmpty
*/
