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

#include "HttpsSvr.h" // 원래 헤더가 없었지만, 분리 하여 만든다.

// MFC windows 앱에서 메모리 릭이 생기는데,
//똑같은 코드라도 CRT로 컴파일 하면 메모리 릭이 없는데, 그대로 MFC와 합치면 릭이 생긴다.
// 그래서 아래 시도 해봤지만 소용 없다.
// 하지만 다행인것은 교신중에 늘어 나지 않고, 서버 스타트 하면 생기므로 무시해도 된다.
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
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

//shared_ptr<CMyHttps> CMyHttps::s_body;

//CKTrace std_cout;

void CMyHttps::SampleData(int port, const char* vpath, const char* path)// error :"../www/api")
{
	_port = port;
	_path = path;
	_vpath = vpath;

	string keyFolder = "C:/Dropbox/Proj/STUDY/boostEx/CppServer/CppServer-master/tools/certificates/";
	string dh = "dh4096.pem";
	string keyFolderIIS = "C:/Dropbox/Proj/ODISO/Src/IIS/.well-known/pki-validation/";
	string svrkeyIIS = "certificate.pem";//server certificate 
	string prikeyIIS = "private.pem";
	string chainIIS = "ca_bundle.pem";//Error key values mismatch
	_certificate = keyFolderIIS + svrkeyIIS;
	_privatekey = keyFolderIIS + prikeyIIS;
	_dhparam = keyFolder + dh;
	_prvpwd = "hjkjjkl";
}

CMyHttps::~CMyHttps()
{
	std_cout << "~CMyHttps() destroyed." << std_endl;
}

/// 이건 기존 코드를 보존 하는것일뿐.
int CMyHttps::main_https(int argc, char** argv)
{
	// HTTPS server port
	int port = 19480;//8443;443;//
	if(argc > 1)
		port = std::atoi(argv[1]);
	// HTTPS server content path
	string www = "../www/api";
	if(argc > 2)
		www = argv[2];

	std_cout << "HTTPS server port: " << port << std_endl;
	std_cout << "HTTPS server static content path: " << www << std_endl;
	std_cout << "HTTPS server website: " << "https://localhost:" << port << "/api/index.html" << std_endl;

	std_cout << std_endl;
	return 0;
}


int CMyHttps::start_server()
{
	if(_service.get() && _server.get() && _server->IsStarted())
		return 1;
	//string www = "../www/api"; // == "api/" 로 
	// Create a new Asio service
	try 
	{
		_service = std::make_shared<AsioService>();
		if (_service.get())
			std_cout << "Asio service starting...";
		else
			return 2;
		// Start the Asio service
		_service->Start();
		std_cout << "Done!" << std_endl;

		// Create and prepare a new SSL server context
		auto context = std::make_shared<CppServer::Asio::SSLContext>(BAsio::ssl::context::tlsv12);
		context->set_password_callback([&](size_t max_length, BAsio::ssl::context::password_purpose purpose) -> string
			{
				return this->_prvpwd;//"hjkjjkl";//"qwerty"; //여기에 비번이
			});
		//string keyFolder = "C:/Dropbox/Proj/STUDY/boostEx/CppServer/CppServer-master/tools/certificates/";
		//string svrkey = "server.pem";//server certificate 
		//string prikey = "server.pem";
		//string dh = "dh4096.pem";

		//string keyFolderIIS = "C:/Dropbox/Proj/ODISO/Src/IIS/.well-known/pki-validation/";
		//string svrkeyIIS = "certificate.pem";//server certificate 
		//string prikeyIIS = "private.pem";
		//string chainIIS = "ca_bundle.pem";//Error key values mismatch
		////https://github.com/chronoxor/CppServer#openssl-certificates
		////C:\Dropbox\Proj\STUDY\SSL\openssl\홈서버 구축기.docx
		//context->use_certificate_chain_file(keyFolderIIS + svrkeyIIS);// 여기 통과 chain이 ca_bundle
		//context->use_private_key_file(keyFolderIIS + prikeyIIS, BAsio::ssl::context::pem);
		//context->use_tmp_dh_file(keyFolder + dh);
		context->use_certificate_chain_file(_certificate);// 여기 통과 chain이 ca_bundle
		context->use_private_key_file(_privatekey, BAsio::ssl::context::pem);
		context->use_tmp_dh_file(_dhparam);
	/*}//C2039: 'system_error': 'BAsio::error'의 멤버가 아닙니다.
	catch(TException ex) //    catch(const TException& ex) //OK
	{
		TErrCode ec = ex.code();       //boost::wrapexcept<boost::system::system_error>, 
		std_cout << "Error " << ec.message() << std_endl;//Error No such process : pem파일이 DropBox 스마트동기화 안되어있다.
		return 2;
	}

	try {*/
		// Create a new HTTPS server
		_server = std::make_shared<HTTPSCacheServer>(_service, context, _port);
		//_path = "C:/Dropbox/Proj/STUDY/boostEx/CppServer/CppServer-master/www/api";"C:\Dropbox\Proj\STUDY\boostEx\CppServer\CppServer-master\www\api";
		if(_path.size() > 0 && _vpath.size() > 0)
			_server->AddStaticContent(_path, _vpath);

		//kdw: shared_ptr을 쓰지 않은 경우 여기서 포인터는 초기화 해줘야 한다. 생성자 매개변수가 틀려서 기본생성자가 없다고 못만드므로, C2512오류
		//_server->_fncGET = nullptr;
		//_server->_fncPOST = nullptr;

		// Start the server
		std_cout << "Server starting...";
		if (_server->Start())
			std_cout << "Done!" << std_endl;
		else
		{
			std_cout << "Server starting ERROR!!!!!!!!" << std_endl;
			return -1;
		}
	}
	catch(TException ex)
	{
		TErrCode ec = ex.code();
		std_cout << "Error " << ec.message() << std_endl;
		return 3;
	}
	return 0;
}


void CMyHttps::restart_server()
{
	HttpCmn::restart_server(_service, _server);
	/*if(_service.get() == nullptr)
		return;
	if(_server.get() == nullptr)
		return;
	std_cout << "Server restarting...";
	_server->Restart();
	std_cout << "Done!" << std_endl;*/
}



void CMyHttps::stop_server()
{
	HttpCmn::stop_server(_service, _server);
}


void CMyHttps::shutdown_server()
{
	HttpCmn::shutdown_server(_service, _server);
}

/*
Hello World!
HTTPS server port: 19480
HTTPS server static content path: ../www/api
HTTPS server website: https://localhost:19480/api/index.html

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
Host: 114.201.33.80:19480
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36
																								 v요기 원래 '/' 한개임
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
Host: 114.201.33.80:19480
Connection: keep-alive
Cache-Control: max-age=0
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/85.0.4183.121 Safari/537.36
																									  v요기 원래 '/' 한개임
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

void HTTPSCacheServer::onError(int error, const string& category, const string& message)
{
	std_cout << "HTTPS server caught an error with code " << error << " and category '" << category << "': " << message << std_endl;
	// 10038 system "message = "소켓 이외의 개체에 작업을 시도했습니다"
	if(_fncOnError.get())
		(*_fncOnError.get())(error, category, message);
}


void HTTPSCacheServer::onStarted() 
{
	std_cout << "HTTPSCacheServer::onStarted " << std_endl;
	if(_fncOnStarted.get())
		(*_fncOnStarted.get())();
}



void HTTPSCacheServer::onStopped() 
{
	std_cout << "HTTPSCacheServer::onStopped " << std_endl;
	if(_fncOnStopped.get())
		(*_fncOnStopped.get())();
}


//C:\Dropbox\Proj\STUDY\boostEx\CppServer\CppServer-master\include\server\asio\ssl_session.h
void HTTPSCacheServer::onConnected(shared_ptr<SSLSession>& session) 
{
	std_cout << "HTTPSCacheServer::onConnected " << std_endl;
	if(_fncOnConnected.get())
		(*_fncOnConnected.get())((HTTPSCacheSession&)*session.get());
}


/// CMyHttp의 것과 다른부분
void HTTPSCacheServer::onHandshaked(shared_ptr<SSLSession>& session)
{
	std_cout << "HTTPSCacheServer::onHandshaked " << std_endl;
	if(_fncOnHandshaked.get())
		(*_fncOnHandshaked.get())((HTTPSCacheSession&)*session.get());
}


void HTTPSCacheServer::onDisconnected(shared_ptr<SSLSession>& session) 
{
	std_cout << "HTTPSCacheServer::onHandshaked " << std_endl;
	if(_fncOnDisconnected.get())
		(*_fncOnDisconnected.get())((HTTPSCacheSession&)*session.get());
}



/// ///////////////////////// Session ////////////////////////////////////////////
/// 
void KSessionInfo::GatherBasicInfo(const HTTPRequest& request)//, KSessionInfo& sinf)//string& url, string& sparams, string& dir, string& stCached,							  std::map<string, string>& urlparam, std::map<string, string>& headers)
{
	///[ 기본 데이터 수집. _url, _dir, _sparams, _urlparam
	_method = request.method();
	string url0(request.url());//key	"/apix?key=xxx&srl=5111"	url0	"/images/xxxxx.jpeg"
	_url = CppCommon::Encoding::URLDecode(url0);
	size_t iq = _url.find('?');//, 0Ui64);
	if(iq != string::npos) {
		_dir = _url.substr(0, iq);
		_sparams = _url.substr(iq + 1);
		KwUrlParamToMap(_sparams, _urlparam);
	}
	else
		_dir = _url;

	//http header를 map에 수집. Content-Type: image/jpeg
	auto szHd = request.headers();
	for(int i=0;i<szHd;i++) {
		auto hd = request.header(i);
		_headers[(string)std::get<0>(hd)] = std::get<1>(hd);
	}
	///] 기본 데이터 수집. _url, _dir, _sparams, _urlparam
}


void HTTPSCacheSession::onReceivedRequest(const HTTPRequest& request)
{
	HTTPResponse& res1 = response();
	auto svr = server();	//+svr	shared_ptr<SSLServer>
	shared_ptr<SSLServer>& svr1 = server();	//+svr1	shared_ptr{_fncGET=empty _fncPOST=empty} [5 strong refs, 1 weak ref] [{_Storage={_Value={_fncGET=empty _fncPOST=empty }} }]	shared_ptr<CppServer::Asio::SSLServer>&
	auto psvr = svr1.get();
	HTTPSCacheServer* svr2 = dynamic_cast<HTTPSCacheServer*>(psvr);

	HttpCmn::onReceivedRequest(svr2, this, (HTTPRequest&)request);
}

void HTTPSCacheSession::onReceived(const void* buffer, size_t size) {
	std_cout << "HTTPSCacheSession::onReceived >> super " << size << std_endl;
	__super::onReceived(buffer, size);// 이거 안부르면 onReceivedRequest가 안불려
	if(_fncOnReceived.get())
		(*_fncOnReceived.get())(this, buffer, size);
}

void HTTPSCacheSession::onSent(size_t sent, size_t pending) {
	//std_cout << "HTTPSCacheSession::onSent " << std_endl;
	if(_fncOnSent.get())
		(*_fncOnSent.get())(this, sent, pending);
}
void HTTPSCacheSession::onSentKw(uint8_t* data, size_t szAll) {
	//std_cout << "HTTPSCacheSession::onSent " << std_endl;
	if(_fncOnSentKw.get())
		(*_fncOnSentKw.get())(this, data, szAll);
	_sinfo.Clear();
}

void HTTPSCacheSession::onReceivedRequestError(const HTTPRequest& request, const string& error)
{
	std_cout << "HTTPSCacheSession::onReceivedRequestError: " << error << std_endl;
}

void HTTPSCacheSession::onReceivedRequestHeader(const HTTPRequest& request)
{// base가 빈 함수라 __super를 부를 필요 없다.
	std_cout << "HTTPSCacheSession::onReceivedRequestHeader- len: " << std_endl;//request.body_length <<  
}

void HTTPSCacheSession::onError(int error, const string& category, const string& message)
{
	std_cout << "HTTPSCacheSession::onError with code: " << error << " category: '" << category << "': " << message << std_endl;
}

void HTTPSCacheSession::onConnected() {
	std_cout << "HTTPSCacheSession::onConnected " << std_endl;
}

void HTTPSCacheSession::onHandshaked() {
	std_cout << "HTTPSCacheSession::onHandshaked " << std_endl;
}

void HTTPSCacheSession::onDisconnected() {
	std_cout << "HTTPSCacheSession::onDisconnected >> super" << std_endl << std_endl;
	__super::onDisconnected();
}


void HTTPSCacheSession::onEmpty() {
	//std_cout << "HTTPSCacheSession::onEmpty " << std_endl;
}
