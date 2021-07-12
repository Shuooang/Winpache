#pragma once

/// 
/// ����� ��Ī http_server.h�� �����Ƿ� https_server.h �� �ִ� ���� ���⿡ ����� �Ѵ�.
/// ����, https_server.h�� ���� �и� �ؼ� �������. ���⵵ ������ �ڴ�.

/// KHttp\pkg\CppServer\include\server
#include "server/kwadjust.h"
#include "server/http/http_server.h"

/// KHttp\pkg\CppServer\examples
// #include "asio_service.h" // AsioService => CAsioService �� ��ü

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


// extern CKTrace std_cout;// Debug �� �߰� ��� ���ÿ� �� ������
// extern CKTrace std_coutD;// Debug�� ����������


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

	//?kdw virtual�� �ƴѰ� virtual�� ���� override�Ѵ�.
	// �̰��� ����ĳ�÷� �����Ҷ��� �ҷ�����.
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
	/// url params���� �� ����� ���� �ٸ� 'notKey'�� ���� ĳ�ÿ� Ű�� �����.
	/// </summary>
	std::string getCacheKey(const char* notKey, const char* notKey2 = nullptr)
	{
		Tas ss;
		for(auto it:_sinfo._urlparam)
		{
			if(it.first != notKey && (!notKey2 || it.first != notKey2))//"uuid"=notKey �� srl�� ���� ������ �� Ű�� �����Ѵ�.
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
	/// _eStopMode = shutdown �̸� _service�� Stop�Ѵ�.
	//char* _eStopMode{"none"};// "stop", "shutdown", "restart",
		// ����: CAsiService�� start �Ҷ� ���� ���� �ʱ�ȭ �ϴ� �� �ذ� �ȵ�.
	// ���� ���� CAsiService�� ���� ���°� ������

	void onConnected(shared_ptr<TCPSession>& session) override;

	void onDisconnected(shared_ptr<TCPSession>& session) override;

	//kdw added 
public: // �Ʒ��� �������� ����.
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
	// ������ ���� �Ǹ� _server->_fncOnTrace = _fncOnTrace;

public:
	void restart_server();
	void stop_server();
	void shutdown_server();
	int start_server();

	template<typename TFNC> void AddCallbackOnReceivedRequest(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnReceivedRequest, fnc, bOvWR);//�Լ��� �Ķ���͸� ������ �� �ֵ���
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

