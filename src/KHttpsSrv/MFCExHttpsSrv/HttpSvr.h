#pragma once
#include "string/string_utils.h" // CppCommon::StringUtils::ReplaceFirst

/// KHttp\pkg\CppServer\include\server
#include "server/kwadjust.h"



#include "server/http/http_server.h"

/// KHttp\src\KHttpsSrv\MFCAppServerEx2
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










































































































/// <summary>
/// 130 :
/// </summary>
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

	/// <summary>
	/// url params에서 각 사용자 마다 다른 'notKey'만 빼고 캐시용 키를 만든다.
	/// 
	/// </summary>
	std::string getCacheKey(const char* notKey, const char* notKey2 = nullptr);

	shared_ptr<function<int(HTTPCacheSession*, HTTPRequest&)>> _fncOnReceivedRequestInternal;
	shared_ptr<function<int(HTTPCacheSession*, const void*, size_t)>> _fncOnReceived;
	shared_ptr<function<int(HTTPCacheSession*, size_t, size_t)>>      _fncOnSent;
	shared_ptr<function<int(HTTPCacheSession*, uint8_t*, size_t)>>    _fncOnSentKw;
};



/// <summary>
/// No SSL server
/// </summary>
class HTTPCacheServer : public CppServer::HTTP::HTTPServer
{
public:
	using CppServer::HTTP::HTTPServer::HTTPServer;

	~HTTPCacheServer()
	{
	}
	bool _bChache{true};
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
	shared_ptr<function<int(HTTPCacheSession*, HTTPResponse&)>> _fncGET;//HTTPRequest&,
	shared_ptr<function<int(HTTPCacheSession*, shared_ptr<KBinData>, HTTPResponse&)>> _fncPOST;
	shared_ptr<function<int(HTTPCacheSession&)>>                              _fncOnConnected;
	//shared_ptr<function<int(HTTPCacheSession&)>>                              _fncOnHandshaked;
	shared_ptr<function<int(HTTPCacheSession&)>>                              _fncOnDisconnected;
	shared_ptr<function<int(HTTPCacheSession*, size_t, size_t)>>              _fncOnSent;
	shared_ptr<function<int(HTTPCacheSession*, uint8_t*, size_t)>>            _fncOnSentKw;
	shared_ptr<function<int(HTTPCacheSession*, const void*, size_t)>>         _fncOnReceived;
	shared_ptr<function<int(HTTPCacheSession*, HTTPRequest&)>> _fncOnReceivedRequestInternal;
	shared_ptr<function<void(string)>>                                          _fncOnTrace;
	shared_ptr<function<int(KSessionInfo&, shared_ptr<KBinData>)>> _fncCluster;//

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
		server->_fncCluster.reset();
	}

// 	shared_ptr<CacheBin> _cache;
// 	//shared_ptr<Cache> _cacheOld;
// 	void setCache(shared_ptr<CacheBin> cache) {
// 		if(!_cache)
// 			_cache = cache;
// 	}
	CacheBin& getCache() {
		return CacheBin::GetInstance();
// 		if (_cache.get() == nullptr)
// 			_cache = shared_ptr<CacheBin>(new CacheBin());
// 		return *_cache.get();
	}
// 	Cache& getCacheOld() {
// 		//return Cache::GetInstance();
// 		if (_cacheOld.get() == nullptr)
// 			_cacheOld = shared_ptr<Cache>(new Cache());
// 		return *_cacheOld.get();
// 	}
};













/// Server하나당 이거 하나씩 사용 하며, CAsioService는 각 Server마다 공유 하지만
///	Server는 각 리스너 마다 하나씩 갖는다.








































































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
		//_server->_fncGET = std::make_shared<function<int(HTTPSCacheSession*, shared_ptr<KBinData>, HTTPResponse&)>>(fnc);
		TCreateFuncValue(_server->_fncGET, fnc, bOvWR);//함수의 파라미터를 생략할 수 있도록
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
	template<typename TFNC> void AddCallbackCluster(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncCluster, fnc, bOvWR);
	}

};

