#pragma once

/// 
/// 여기는 속칭 http_server.h가 없으므로 https_server.h 에 있는 것을 여기에 해줘야 한다.
/// 아참, https_server.h는 내가 분리 해서 만들었지. 여기도 만들어야 겠다.
#include "server/kwadjust.h"

#include "asio_service.h"

#include "server/http/http_server.h"


// CppCommon
#include "string/string_utils.h"
#include "utility/singleton.h"

#include "Cache.h"


#include "HttpsSvr.h"



//using namespace std;
using std::string;
using std::shared_ptr;
using std::function;




using CppServer::HTTP::HTTPRequest;
using CppServer::HTTP::HTTPResponse;
using CppServer::Asio::TCPSession;
using CppServer::Asio::TCPServer;





class HTTPCacheSession : public CppServer::HTTP::HTTPSession
{
public:
	using CppServer::HTTP::HTTPSession::HTTPSession;

protected:
	void onError(int error, const string& category, const string& message) override;
	void onConnected() override;

	void onDisconnected() override;
	void onReceived(const void* buffer, size_t size) override;
	void onSent(size_t sent, size_t pending) override;
	void onSentKw(uint8_t* data, size_t szAll) override;

	void onEmpty() override;

	void onReceivedRequest(const HTTPRequest& request) override;
	void onReceivedRequestError(const HTTPRequest& request, const string& error) override;
	void onReceivedRequestHeader(const HTTPRequest& request) override;
public:
	KSessionInfo _sinfo;

	//string _url;
	//string _sparams;
	//std::map<string, string> _urlparam;
	//std::map<string, string> _headers;
	//string _dir;
	//string _stCached;//{"cached"/"NoCach"};

	//void MakeJsonError(HTTPResponse& res1, char* errmsg, int err = -1);
	//void ResponseError(char* errmsg, int err = -1);

	/// <summary>
	/// url params에서 각 사용자 마다 다른 'notKey'만 빼고 캐시용 키를 만든다.
	/// </summary>
	std::string getCacheKey(const char* notKey)
	{
		Tas ss;
		for(auto it:_sinfo._urlparam)
		{
			if(it.first != notKey)//"uuid")
				ss << it.first << it.second;
		}
		return ss.str();
	}

	shared_ptr<function<int(HTTPCacheSession*, const void*, size_t)>> _fncOnReceived;
	shared_ptr<function<int(HTTPCacheSession*, size_t, size_t)>>      _fncOnSent;
	shared_ptr<function<int(HTTPCacheSession*, uint8_t*, size_t)>>    _fncOnSentKw;
	//shared_ptr<function<int(const void*, size_t)>> _fncOnReceived;
	//shared_ptr<function<int(size_t, size_t)>>      _fncOnSent;
	//shared_ptr<function<int(uint8_t*, size_t)>>    _fncOnSentKw;
};


class HTTPCacheServer : public CppServer::HTTP::HTTPServer
{
public:
	using CppServer::HTTP::HTTPServer::HTTPServer;

	~HTTPCacheServer() {    }

protected:
	shared_ptr<CppServer::Asio::TCPSession> CreateSession(const shared_ptr<CppServer::Asio::TCPServer>& server) override
	{
		auto ss = std::make_shared<HTTPCacheSession>(std::dynamic_pointer_cast<HTTPServer>(server));
		ss->_fncOnReceived = _fncOnReceived;
		ss->_fncOnSent = _fncOnSent;
		ss->_fncOnSentKw = _fncOnSentKw;
		return ss;
	}

protected:
	void onError(int error, const string& category, const string& message) override;
	void onStarted() override;
	void onStopped() override;

	void onConnected(shared_ptr<TCPSession>& session) override;

	void onDisconnected(shared_ptr<TCPSession>& session) override;

	//kdw added 
public: // 아래는 소유권이 없다.
	shared_ptr<function<void()>>                                              _fncOnStarted;
	shared_ptr<function<void()>>                                              _fncOnStopped;
	shared_ptr<function<int(int, const string&, const string&)>>              _fncOnError;
	shared_ptr<function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)>> _fncGET;
	shared_ptr<function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)>> _fncPOST;
	shared_ptr<function<int(HTTPCacheSession&)>>                              _fncOnConnected;
	//	shared_ptr<function<int(HTTPCacheSession&)>>                              _fncOnHandshaked;
	shared_ptr<function<int(HTTPCacheSession&)>>                              _fncOnDisconnected;
	shared_ptr<function<int(HTTPCacheSession*, size_t, size_t)>>              _fncOnSent;
	shared_ptr<function<int(HTTPCacheSession*, uint8_t*, size_t)>>            _fncOnSentKw;
	shared_ptr<function<int(HTTPCacheSession*, const void*, size_t)>>         _fncOnReceived;

	//shared_ptr<function<int(const HTTPRequest&, HTTPResponse&)>> _fncGET;
	//shared_ptr<function<int(const HTTPRequest&, HTTPResponse&)>> _fncPOST;
	//shared_ptr<function<int(TCPSession&)>>                       _fncOnConnected;
	//shared_ptr<function<int(TCPSession&)>>                       _fncOnDisconnected;
	//shared_ptr<function<void()>>                                 _fncOnStarted;
	//shared_ptr<function<void()>>                                 _fncOnStopped;
	//shared_ptr<function<int(size_t, size_t)>>                    _fncOnSent;
	//shared_ptr<function<int(uint8_t*, size_t)>>  _fncOnSentKw;
	//shared_ptr<function<int(const void*, size_t)>>               _fncOnReceived;
	//shared_ptr<function<int(int, const string&, const string&)>> _fncOnError;

	shared_ptr<Cache> _cache;
	Cache& getCache() {
		//return Cache::GetInstance();
		if(_cache.get() == nullptr)
			_cache = shared_ptr<Cache>(new Cache());
		return *_cache.get();
	}

};



class CMyHttp
{
public:
	// 서버를 유일하게 하나 쓸떄는 이걸 쓴다.
	// 여러개 쓸때는 shared_ptr<CMyHttp> _bodySSL; 처럼 변수를 가진다.
	static shared_ptr<CMyHttp> s_body;
	static CMyHttp* Instance()
	{
		if(s_body.get() == nullptr)
		{
			s_body = shared_ptr<CMyHttp>(new CMyHttp());
		}
		return s_body.get();
	}


	void SampleData(int port = 19479
		, const char* vpath = "/api"
		, const char* path = "C:/Dropbox/Proj/STUDY/boostEx/CppServer/CppServer-master/www/api");
	~CMyHttp();

	int _port;
	string _path;
	string _vpath;




	shared_ptr<AsioService> _service;
	shared_ptr<HTTPCacheServer> _server;

public:
	int main_http(int argc, char** argv);
	void restart_server();
	void stop_server();
	int start_server();
	void shutdown_server();


	template<typename TFNC> void AddCallbackOnGET(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncGET, fnc);

	}
	template<typename TFNC> void AddCallbackOnPOST(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncPOST, fnc);
	}
	template<typename TFNC> void AddCallbackOnStarted(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnStarted, fnc);
	}
	template<typename TFNC> void AddCallbackOnStopped(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnStopped, fnc);
	}
	template<typename TFNC> void AddCallbackOnError(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnError, fnc);
	}
	template<typename TFNC> void AddCallbackOnConnected(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnConnected, fnc);
	}



	template<typename TFNC> void AddCallbackOnDisconnected(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnDisconnected, fnc);
	}
	template<typename TFNC> void AddCallbackOnReceived(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnReceived, fnc);
	}
	template<typename TFNC> void AddCallbackOnSent(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnSent, fnc);
	}
	template<typename TFNC> void AddCallbackOnSentKw(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnSentKw, fnc);
	}

};

