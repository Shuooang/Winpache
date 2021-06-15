/*!
	\file http_server.cpp
	\brief HTTP server example
	\author Ivan Shynkarenka
	\date 30.04.2019
	\copyright MIT License
*/

/// ���� include������ �־����� ����� ����鼭 �׸� �Ű� ����.

#include "pch.h"

#include <iostream>
#include <map>
#include <mutex>
#include <tuple>

#include "HttpSvr.h" // ���� ����� ��������, �и� �Ͽ� �����.
#include "HttpsSvr.h" // ���� �κ�





















shared_ptr<CMyHttp> CMyHttp::s_body;

//CKTrace std_cout;

void CMyHttp::SampleData(int port, const char* vpath, const char* path)// error :"../www/api")
{
	_port = port;
	_path = path;
	_vpath = vpath;
}

CMyHttp::~CMyHttp()
{
	std_cout << "~CMyHttp() destroyed." << std_endl;
}

/// �̰� ���� �ڵ带 ���� �ϴ°��ϻ�.
int CMyHttp::main_http(int argc, char** argv)
{
	// HTTP server port
	int port = 8080;
	if (argc > 1)
		port = std::atoi(argv[1]);
	// HTTP server content path
	std::string www = "../www/api";
	if (argc > 2)
		www = argv[2];

	std_cout << "HTTP server port: " << port << std_endl;
	std_cout << "HTTP server static content path: " << www << std_endl;
	std_cout << "HTTP server website: " << "http://localhost:" << port << "/api/index.html" << std_endl;

	std_cout << std_endl;

	// Create a new Asio service
	auto service = std::make_shared<AsioService>();

	// Start the Asio service
	std_cout << "Asio service starting...";
	service->Start();
	std_cout << "Done!" << std_endl;

	// Create a new HTTP server
	auto server = std::make_shared<HTTPCacheServer>(service, port);
	server->AddStaticContent(www, "/api");

	// Start the server
	std_cout << "Server starting...";
	server->Start();
	std_cout << "Done!" << std_endl;

	std_cout << "Press Enter to stop the server or '!' to restart the server..." << std_endl;

	// Perform text input
	std::string line;
	while (getline(std::cin, line))
	{
		if (line.empty())
			break;

		// Restart the server
		if (line == "!")
		{
			std_cout << "Server restarting...";
			server->Restart();
			std_cout << "Done!" << std_endl;
			continue;
		}
	}

	// Stop the server
	std_cout << "Server stopping...";
	server->Stop();
	std_cout << "Done!" << std_endl;

	// Stop the Asio service
	std_cout << "Asio service stopping...";
	service->Stop();
	std_cout << "Done!" << std_endl;

	return 0;
}


int CMyHttp::start_server()
{
	if(_service.get() && _server.get() && _server->IsStarted())
		return 1;
	// Create a new Asio service
	try {
		_service = std::make_shared<AsioService>();
		if (_service.get())
			std_cout << "Asio service starting...";
		else
			return 2;

		// Start the Asio service
		bool bStarted = _service->Start();
		if(bStarted)
			std_cout << "Done!" << std_endl;
		else
			std_cout << "Asio service is already started!!" << std_endl;


	///
	/// 
	/// https �� �ٸ� �κ� ���� �κ��� ������.
	/// 
	/// 





		// Create a new HTTP server
		_server = std::make_shared<HTTPCacheServer>(_service, _port);
		_server->AddStaticContent(_path, _vpath);//, "/api");

		//kdw: shared_ptr�� ���� ���� ��� ���⼭ �����ʹ� �ʱ�ȭ ����� �Ѵ�. ������ �Ű������� Ʋ���� �⺻�����ڰ� ���ٰ� ������Ƿ�, C2512����
		//_server->_fncGET = nullptr;
		//_server->_fncPOST = nullptr;

		// Start the server
		std_cout << "Server starting...";
		if(_server->Start())
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
		return 3;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}

	return 0;
}



/// CMyHttps�� �Ͱ� ����
void CMyHttp::restart_server()
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


/// CMyHttps�� �Ͱ� ����
void CMyHttp::stop_server()
{
	HttpCmn::stop_server(_service, _server);
}


/// CMyHttps�� �Ͱ� ����
void CMyHttp::shutdown_server()
{
	HttpCmn::shutdown_server(_service, _server);
}


/// CMyHttps�� �Ͱ� ����
void HTTPCacheServer::onError(int error, const string& category, const string& message)
{
	std_cout << "HTTPS server caught an error with code " << error << " and category '" << category << "': " << message << std_endl;
	if(_fncOnError.get())
		(*_fncOnError.get())(error, category, message);
}


/// CMyHttps�� �Ͱ� ����
void HTTPCacheServer::onStarted()
{
	std_cout << "HTTPSCacheServer::onStarted " << std_endl;
	if(_fncOnStarted.get())
		(*_fncOnStarted.get())();
}


/// CMyHttps�� �Ͱ� ����
void HTTPCacheServer::onStopped()
{
	std_cout << "HTTPSCacheServer::onStopped " << std_endl;
	if(_fncOnStopped.get())
		(*_fncOnStopped.get())();
}


/// CMyHttps�� �Ͱ� ����
void HTTPCacheServer::onConnected(shared_ptr<TCPSession>& session)
{
	std_cout << "HTTPSCacheServer::onConnected " << std_endl;
	if(_fncOnConnected.get())
		(*_fncOnConnected.get())(*(HTTPCacheSession*)session.get());
}


/// CMyHttps::onHandshaked �� SSL�� �ʿ� ������ ���⼭ ������.







/// CMyHttps�� �Ͱ� ����
void HTTPCacheServer::onDisconnected(shared_ptr<TCPSession>& session)
{
	std_cout << "HTTPCacheServer::onHandshaked " << std_endl;
	if(_fncOnDisconnected.get())
		(*_fncOnDisconnected.get())(*(HTTPCacheSession*)session.get());
}




/// ///////////////////////// Session ////////////////////////////////////////////
/*
/// CMyHttps�� �Ͱ� ����
//void HTTPCacheSession::MakeJsonError(HTTPResponse& res1, char* errmsg, int err)
//{
//	HttpCmn::MakeJsonError(res1, errmsg, err);
//}
//
/// CMyHttps�� �Ͱ� ����
void HTTPCacheSession::ResponseError(char* errmsg, int err)
{
	HTTPResponse& res1 = response();
	MakeJsonError(res1, errmsg, err);
	SendResponseAsync(res1);
}*/
void HTTPCacheSession::onReceivedRequestHeader(const HTTPRequest& request)
{// base�� �� �Լ��� __super�� �θ� �ʿ� ����.
	//std_cout << "HTTPCacheSession::onReceivedRequestHeader- len: " << std_endl;//request.body_length <<  
}
void HTTPCacheSession::onReceivedRequest(const HTTPRequest& request)
{
//	HTTPRequest& req1 = (HTTPRequest&)request();
	HTTPRequest& req1 = (HTTPRequest&)request; // this->request(); kdw�� �߰�
	auto svr = server();	//+svr	shared_ptr<SSLServer>
	auto svr2 = dynamic_cast<HTTPCacheServer*>(svr.get());

	HttpCmn::onReceivedRequest(svr2, this, req1);
}


void HTTPCacheSession::onReceived(const void* buffer, size_t size)
{
	__super::onReceived(buffer, size);// �̰� �Ⱥθ��� onReceivedRequest�� �Ⱥҷ�
	if (_fncOnReceived.get())
		(*_fncOnReceived.get())(this, buffer, size);
}

void HTTPCacheSession::onSent(size_t sent, size_t pending)
{
	if (_fncOnSent.get())
		(*_fncOnSent.get())(this, sent, pending);
}
void HTTPCacheSession::onSentKw(uint8_t* data, size_t szAll)
{
	if (_fncOnSentKw.get())
		(*_fncOnSentKw.get())(this, data, szAll);
	_sinfo.Clear();
}

/*
	// Show HTTP request content
	std_cout << std_endl << request.string();

	HTTPResponse& res1 = response();

	shared_ptr<TCPServer>& svr1 = server();
	HTTPCacheServer* server = dynamic_cast<HTTPCacheServer*>(svr1.get());

	_sinfo.GatherBasicInfo(_request);


	// Process HTTP request methods
	if(request.method() == "HEAD")
	{
		SendResponseAsync(response().MakeHeadResponse());
	}
	else if(request.method() == "GET")
	{
		int rv = -1;
		string keyRes = getCacheKey("uuid");//_urlparam�� �̹� ��� �־�� ����. _url
		string value, valueRes;
		if(_sinfo._url.empty())
		{
			// Response with all cache values
			//SendResponseAsync(res1.MakeGetResponse(server->getCache().GetAllCache(), "application/json; charset=UTF-8"));
			SendResponseAsync(res1.MakeGetResponse("{}", "application/json; charset=UTF-8"));
		}
		else if(_sinfo._urlparam["noCache"] == "true")
			server->getCache().DeleteCacheValue(keyRes, valueRes);
		else if(server->getCache().GetCacheValue(keyRes, valueRes))
		{// ĳ�ð� ������ ���� ������ �ʴ´�. ��� ó�� ����?
			// Response with the cache value
			_sinfo._stCached = "cached";
			res1.MakeGetResponse(value, "application/json; charset=UTF-8");
			SendResponseAsync(res1);
			return;
		}


		if(server->_fncGET)
		{
			_sinfo._stCached = "NoCach";

			rv = (int)(*server->_fncGET)(request, res1);
			if(rv == 0)
			{
				string value(res1.body());
				server->getCache().PutCacheValue(keyRes, value);
				SendResponseAsync(res1);
				return;
			}
		}
		else {
			//HTTPSCacheServer* server = dynamic_cast<HTTPSCacheServer*>(svr);
			SendResponseAsync(response().MakeErrorResponse("Required cache value was not found for the key: " + _url, 404));
			// ���� ó�� GET https�� ������ ���� 2020-10-04 03:15
		}*/
		/*
		std::string key(request.url());
		std::string value;

		// Decode the key value
		key = CppCommon::Encoding::URLDecode(key);
		CppCommon::StringUtils::ReplaceFirst(key, "/api/cache", "");
		CppCommon::StringUtils::ReplaceFirst(key, "?key=", "");

		if(key.empty())
		{
			// Response with all cache values .Cache::GetInstance().GetAllCache()
			SendResponseAsync(response().MakeGetResponse("{}", "application/json; charset=UTF-8"));
		}
		// Get the cache value by the given key
		else if(Cache::GetInstance().GetCacheValue(key, value))
		{
			// Response with the cache value
			SendResponseAsync(response().MakeGetResponse(value));
		}
		else
			SendResponseAsync(response().MakeErrorResponse("Required cache value was not found for the key: " + key, 404));*/
/*
	}
	else if((request.method() == "POST") || (request.method() == "PUT"))
	{
		int rv = 0;
		if(server->_fncPOST)              //�տ� const �� �־
			rv = (int)(*server->_fncPOST)(request, res1);///(HTTPRequest&)

		if(rv == 0)
		{
			SendResponseAsync(res1);
			return;
		}

		std::string key(request.url());
		std::string value(request.body());

		// Decode the key value
		key = CppCommon::Encoding::URLDecode(key);
		CppCommon::StringUtils::ReplaceFirst(key, "/api/cache", "");
		CppCommon::StringUtils::ReplaceFirst(key, "?key=", "");

		// Put the cache value
		Cache::GetInstance().PutCacheValue(key, value);

		// Response with the cache value
		SendResponseAsync(response().MakeOKResponse());
	}
	else if(request.method() == "DELETE")
	{
		std::string key(request.url());
		std::string value;

		// Decode the key value
		key = CppCommon::Encoding::URLDecode(key);
		CppCommon::StringUtils::ReplaceFirst(key, "/api/cache", "");
		CppCommon::StringUtils::ReplaceFirst(key, "?key=", "");

		// Delete the cache value
		if(Cache::GetInstance().DeleteCacheValue(key, value))
		{
			// Response with the cache value
			SendResponseAsync(response().MakeGetResponse(value));
		}
		else
			SendResponseAsync(response().MakeErrorResponse("Deleted cache value was not found for the key: " + key, 404));
	}
	else if(request.method() == "OPTIONS")
		SendResponseAsync(response().MakeOptionsResponse());
	else if(request.method() == "TRACE")
		SendResponseAsync(response().MakeTraceResponse(request.cache()));
	else
		SendResponseAsync(response().MakeErrorResponse("Unsupported HTTP method: " + std::string(request.method())));
}*/

void HTTPCacheSession::onReceivedRequestError(const CppServer::HTTP::HTTPRequest& request, const std::string& error)
{
	std_cout << "Request error: " << error << std_endl;
}

void HTTPCacheSession::onError(int error, const std::string& category, const std::string& message)
{
	std_cout << "HTTP session caught an error with code " << error << " and category '" << category << "': " << message << std_endl;
}

void HTTPCacheSession::onConnected()
{
	std_cout << "HTTPCacheSession::onConnected " << std_endl;
}

void HTTPCacheSession::onDisconnected()
{
	std_cout << "HTTPCacheSession::onDisconnected >> super" << std_endl << std_endl;
	__super::onDisconnected();
}

void HTTPCacheSession::onEmpty()
{
	//std_cout << "HTTPCacheSession::onEmpty >> super" << std_endl << std_endl;
}


/*
std::string Cache::GetAllCache()
{
	std::scoped_lock locker(_cache_lock);
	std::string result;
	result += "[\n";
	for(const auto& item : _cache)
	{
		result += "  {\n";
		result += "    \"key\": \"" + item.first + "\",\n";
		result += "    \"value\": \"" + item.second + "\",\n";
		result += "  },\n";
	}
	result += "]\n";
	return result;
}

bool Cache::GetCacheValue(std::string_view key, std::string& value)
{
	std::scoped_lock locker(_cache_lock);
	auto it = _cache.find(key);
	if(it != _cache.end())
	{
		value = it->second;
		return true;
	}
	else
		return false;
}

void Cache::PutCacheValue(std::string_view key, std::string_view value)
{
	std::scoped_lock locker(_cache_lock);
	auto it = _cache.emplace(key, value);
	if(!it.second)
		it.first->second = value;
}

bool Cache::DeleteCacheValue(std::string_view key, std::string& value)
{
	std::scoped_lock locker(_cache_lock);
	auto it = _cache.find(key);
	if(it != _cache.end())
	{
		value = it->second;
		_cache.erase(it);
		return true;
	}
	else
		return false;
}
*/