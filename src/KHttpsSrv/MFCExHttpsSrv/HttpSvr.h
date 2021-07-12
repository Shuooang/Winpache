#pragma once

/// 
/// 여기는 속칭 http_server.h가 없으므로 https_server.h 에 있는 것을 여기에 해줘야 한다.
/// 아참, https_server.h는 내가 분리 해서 만들었지. 여기도 만들어야 겠다.

/// KHttp\pkg\CppServer\include\server
#include "server/kwadjust.h"
#include "server/http/http_server.h"

/// KHttp\pkg\CppServer\examples
// #include "asio_service.h" // AsioService => CAsioService 로 대체

/// KHttp\pkg\CppCommon\include\string
//#include "string/string_utils.h"
//#include "utility/singleton.h"

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


// extern CKTrace std_cout;// Debug 와 추가 출력 동시에 내 보낼때
// extern CKTrace std_coutD;// Debug만 내보낼려면


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

	//?kdw virtual이 아닌걸 virtual로 돌려 override한다.
	// 이것은 내부캐시로 접근할때도 불려진다.
	void onReceivedRequestInternal(const HTTPRequest& request) override;

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
	std::string getCacheKey(const char* notKey, const char* notKey2 = nullptr)
	{
		Tas ss;
		for(auto it:_sinfo._urlparam)
		{
			if(it.first != notKey && (!notKey2 || it.first != notKey2))//"uuid"=notKey 와 srl만 빼고 나머지 다 키로 생성한다.
				ss << it.first << it.second;
		}
		if(ss.str().length() == 0)
			ss << "/";
		return ss.str();
	}

	shared_ptr<function<int(HTTPCacheSession*, HTTPRequest&)>> _fncOnReceivedRequestInternal;
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
		auto ip = server->address();
		ss->_fncOnReceivedRequestInternal = _fncOnReceivedRequestInternal;
		ss->_fncOnReceived = _fncOnReceived;
		ss->_fncOnSent = _fncOnSent;
		ss->_fncOnSentKw = _fncOnSentKw;
		return ss;
	}

protected:
	void onError(int error, const string& category, const string& message) override;
	void onStarted() override;
	void onStopped() override;
	/// _eStopMode = shutdown 이면 _service도 Stop한다.
	//char* _eStopMode{"none"};// "stop", "shutdown", "restart",
		// 보류: CAsiService도 start 할때 마다 완전 초기화 하니 좀 해결 된둣.
	// 오류 나면 CAsiService가 맛이 가는거 때문에

	void onConnected(shared_ptr<TCPSession>& session) override;

	void onDisconnected(shared_ptr<TCPSession>& session) override;

	//kdw added 
public: // 아래는 소유권이 없다.
	shared_ptr<function<void()>>                                              _fncOnStarted;
	shared_ptr<function<void()>>                                              _fncOnStopped;
	shared_ptr<function<int(int, const string&, const string&)>>              _fncOnError;
	shared_ptr<function<int(HTTPCacheSession*, shared_ptr<KBinData>, HTTPResponse&)>> _fncOnReceivedRequest;
	shared_ptr<function<int(HTTPCacheSession*, shared_ptr<KBinData>, HTTPResponse&)>> _fncGET;//HTTPRequest&, 
	shared_ptr<function<int(HTTPCacheSession*, shared_ptr<KBinData>, HTTPResponse&)>> _fncPOST;
	shared_ptr<function<int(HTTPCacheSession&)>>                              _fncOnConnected;
	//	shared_ptr<function<int(HTTPCacheSession&)>>                              _fncOnHandshaked;
	shared_ptr<function<int(HTTPCacheSession&)>>                              _fncOnDisconnected;
	shared_ptr<function<int(HTTPCacheSession*, size_t, size_t)>>              _fncOnSent;
	shared_ptr<function<int(HTTPCacheSession*, uint8_t*, size_t)>>            _fncOnSentKw;
	shared_ptr<function<int(HTTPCacheSession*, const void*, size_t)>>         _fncOnReceived;
	shared_ptr<function<int(HTTPCacheSession*, HTTPRequest&)>> _fncOnReceivedRequestInternal;
	shared_ptr<function<void(string)>>                                          _fncOnTrace;

	void ResetEvent()
	{
		auto server = this;
		server->_fncOnStarted.reset();
		server->_fncOnStopped.reset();
		server->_fncOnError.reset();
		server->_fncOnReceivedRequest.reset();
		server->_fncGET.reset();
		server->_fncPOST.reset();
		server->_fncOnConnected.reset();
		server->_fncOnDisconnected.reset();
		server->_fncOnSent.reset();
		server->_fncOnSentKw.reset();
		server->_fncOnReceived.reset();
		server->_fncOnReceivedRequestInternal.reset();
		server->_fncOnTrace.reset();
	}
	shared_ptr<CacheBin> _cache;
	shared_ptr<Cache> _cacheOld;
	CacheBin& getCache() {
		//return Cache::GetInstance();
		if (_cache.get() == nullptr)
			_cache = shared_ptr<CacheBin>(new CacheBin());
		return *_cache.get();
	}
	Cache& getCacheOld() {
		//return Cache::GetInstance();
		if (_cacheOld.get() == nullptr)
			_cacheOld = shared_ptr<Cache>(new Cache());
		return *_cacheOld.get();
	}

};



class CMyHttp
{
public:
	void SampleData(int port = 19478
		, const char* vpath = NULL // "/api"
		, const char* path = NULL);// "C:/Dropbox/Proj/STUDY/boostEx/CppServer/CppServer-master/www/api");
	~CMyHttp();

	int _port;
	BOOL _bStaticCache{ TRUE };
	string _cachedPath;
	string _cachedUrl{ "/" };






	shared_ptr<CAsioService> _service;
	shared_ptr<HTTPCacheServer> _server;

	shared_ptr<function<void(string)>> _fncOnTrace;
	// 서버가 생성 되면 _server->_fncOnTrace = _fncOnTrace;

public:
	void restart_server();
	void stop_server();
	void shutdown_server();
	int start_server();

	template<typename TFNC> void AddCallbackOnReceivedRequest(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnReceivedRequest, fnc, bOvWR);//함수의 파라미터를 생략할 수 있도록
	}
	template<typename TFNC> void AddCallbackOnGET(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncGET, fnc, bOvWR);
	}
	template<typename TFNC> void AddCallbackOnPOST(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncPOST, fnc, bOvWR);
	}
	template<typename TFNC> void AddCallbackOnStarted(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnStarted, fnc, bOvWR);
	}
	template<typename TFNC> void AddCallbackOnStopped(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnStopped, fnc, bOvWR);
	}
	template<typename TFNC> void AddCallbackOnError(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnError, fnc, bOvWR);
	}
	template<typename TFNC> void AddCallbackOnConnected(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnConnected, fnc, bOvWR);
	}



	template<typename TFNC> void AddCallbackOnDisconnected(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnDisconnected, fnc, bOvWR);
	}
	template<typename TFNC> void AddCallbackOnReceived(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnReceived, fnc, bOvWR);
	}
	template<typename TFNC> void AddCallbackOnReceivedRequestInternal(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnReceivedRequestInternal, fnc, bOvWR);
	}
	template<typename TFNC> void AddCallbackOnSent(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnSent, fnc, bOvWR);
	}
	template<typename TFNC> void AddCallbackOnSentKw(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnSentKw, fnc, bOvWR);
	}
	void AddCallbackOnTrace(shared_ptr<function<void(string)>> fnc, int bOvWR = 0)//?ExTrace 3.5 MyHttp -> std_cout(KTrace)
	{
		if(bOvWR || !_fncOnTrace)
		{
			_fncOnTrace = fnc;
			std_cout.AddCallbackOnTrace(fnc);
			std_coutD.AddCallbackOnTrace(fnc);
		}
	}
};

