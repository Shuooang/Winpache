#pragma once
#include "string/string_utils.h" // CppCommon::StringUtils::ReplaceFirst

/// KHttp\pkg\CppServer\include\server
#include "server/kwadjust.h"
#include "server/asio/service.h" // CppServer::Asio::Service
#include "server/asio/ssl_session.h" // server override 몇개 하니 이게 필요해
#include "server/asio/ssl_server.h"
#include "server/http/https_server.h"

/// KHttp\src\KHttpsSrv\MFCAppServerEx2
#include "Cache.h"


//using namespace std;
using std::string;
using std::shared_ptr;
using std::function;

using CppServer::Asio::SSLServer;

using CppServer::HTTP::HTTPRequest;
using CppServer::HTTP::HTTPResponse;
using CppServer::Asio::SSLSession;
//using CppServer::HTTP::HTTPSServer;

//#define SPtr std::shared_ptr; // 오류나네 
#ifndef _break
#define _break {int ____i = 0;}
#endif

class KBinData
{
public:
	~KBinData()
	{
		if(m_p)
			delete m_p;
	}
	UINT_PTR m_len{ 0 };
	char* m_p{ 0 };
	void Attach(const char* p, UINT_PTR len)
	{
		if(len > 0)
		{
			if(m_p)
				delete m_p;

			m_p = new char[len + 1];
			m_len = len;
			memcpy(m_p, p, len);
			m_p[len] = '\0';
		}
	}
};

/// <summary>
/// 람다를 변수에 할당 할때 new function 해야 하는데, 함수의 파라미터를 생략할 수 있도록 template함수를 만든다.
/// 아래 예제 에서 'HTTPSCacheSession*, shared_ptr<KBinData>, HTTPResponse&' 부분을 생략할 수 있다.
/// KwLib64/KTemple.h KwCreateFuncValue 가 동일하게 있지만 외부 코드라 KTemple.h를 include하지 않기 위해 넣는다.
/// </summary>
/// <typeparam name="TFNC"></typeparam>
/// <typeparam name="TLAMBD"></typeparam>
/// <param name="rtval"></param>
/// <param name="lambda"></param>
template<typename TFNC, typename TLAMBD>
void TCreateFuncValue(shared_ptr<TFNC>& rtval, TLAMBD lambda, int bOvWR = 0)
{
/* ex
// 다음과 같은 변수가 선언 되었다고 전제
shared_ptr<function<TRAMBD>> _fncGET;

// 변수 선언 : TFNC == function<int(HTTPSCacheSession*, shared_ptr<KBinData>, HTTPResponse&)> 을 줄이기 위해
shared_ptr<function<int(HTTPSCacheSession*, shared_ptr<KBinData>, HTTPResponse&)>> _fncGET;

// ex
template<typename TFNC> void AddCallbackOnGET(TFNC fnc, int bOvWR = 0)
{
	TCreateFuncValue(_server->_fncGET, fnc);  아래와 동일
	//_server->_fncGET = std::make_shared<function<int(HTTPSCacheSession*, shared_ptr<KBinData>, HTTPResponse&)>>(fnc);
}*/

//rtval = shared_ptr<TFNC>(new TFNC(lambda)); 아래와 동일
	if(bOvWR || !rtval)
		rtval = std::make_shared<TFNC>(lambda);
}

class KSessionInfo
{
public:
	string _ssid;// TSESSION->id().string()
	string _ip;
	string _method;
	string _url;
	string _sparams;
	string _dir;
	string _stCached;//{"cached"/"NoCach"};
	KStrMap _urlparam;
	KStrMap _headers;

	/// return 할때 http code: error났을때 HTTPResponse::SetBegin(status)
	// ResponseForPost 에서 error 났을때 넣고, 
	int _status{ 200 };
	//+["Accept"]	"*/*"	
	//+["Accept-Encoding"]	"gzip, deflate"	
	//+["Accept-Language"]	"ko"	
	//+["Cache-Control"]	"no-cache"	
	//+["Connection"]	"Keep-Alive"	
	//+["Content-Length"]	"3921543"	
	//+["Content-Type"]	"image/jpeg"	
	//+["Host"]	"www.xxxx.co.kr:9480"	
	//+["UA-CPU"]	"AMD64"	
	//+["User-Agent"]	"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.2; Win64; x64; Trident/7.0; .NET4.0C; .NET4.0E; .NET CLR 2.0.50727; .NET CLR 3.0.30729; .NET CLR 3.5.30729)"	

	void Clear()
	{
		_ip.clear();
		_method.clear();
		_url.clear(); //empty();는 검사함수
		_sparams.clear();
		_dir.clear();
		_stCached.clear();

		_urlparam.clear();
		_headers.clear();
	}
	void GatherBasicInfo(const HTTPRequest& request, string ips);//, KSessionInfo& sinf);//, string& url, string& sparams, string& dir, string& stCached, std::map<string, string>& urlparam, std::map<string, string>& headers);
};



/// <summary>
/// 130 :
/// </summary>
class HTTPSCacheSession : public CppServer::HTTP::HTTPSSession
{
public:
	using CppServer::HTTP::HTTPSSession::HTTPSSession;

protected:
	void onError(int error, const string& category, const string& message) override;
	void onConnected() override;
	void onHandshaked() override;
	void onDisconnected() override;
	void onReceived(const void* buffer, size_t size) override;
	void onSent(size_t sent, size_t pending) override;
	void onSentKw(uint8_t* data, size_t szAll) override ;

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
	/// "uuid"는 client사용자 구분이고, "srl"은 캐시가 안되게 테스트 용이다.
	/// </summary>
	std::string getCacheKey(const char* notKey, const char* notKey2 = nullptr);

	shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&)>> _fncOnReceivedRequestInternal;
	shared_ptr<function<int(HTTPSCacheSession*, const void*, size_t)>> _fncOnReceived;
	shared_ptr<function<int(HTTPSCacheSession*, size_t, size_t)>>      _fncOnSent;
	shared_ptr<function<int(HTTPSCacheSession*, uint8_t*, size_t)>>    _fncOnSentKw;
};

typedef HTTPSCacheSession THCSsn; //너무 길어서 썼는데, 정의/선언 만들기 기능에서 안되어서 안쓰기로 했다.

/// <summary>
/// SSL server
/// </summary>
class HTTPSCacheServer : public CppServer::HTTP::HTTPSServer
{
public:
	using CppServer::HTTP::HTTPSServer::HTTPSServer;

	~HTTPSCacheServer()
	{
	}
	bool _bChache{true};
protected:
	shared_ptr<SSLSession> CreateSession(const shared_ptr<SSLServer>& server) override
	{
		auto ss = std::make_shared<HTTPSCacheSession>(std::dynamic_pointer_cast<HTTPSServer>(server));
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
	
	//C:\Dropbox\Proj\STUDY\boostEx\CppServer\CppServer-master\include\server\asio\ssl_session.h
	void onConnected(shared_ptr<SSLSession>& session) override;
	void onHandshaked(shared_ptr<SSLSession>& session) override;
	void onDisconnected(shared_ptr<SSLSession>& session) override;

	//kdw added 
public: // 아래는 소유권이 없다.
	shared_ptr<function<void()>>                                              _fncOnStarted;
	shared_ptr<function<void()>>                                              _fncOnStopped;
	shared_ptr<function<int(int, const string&, const string&)>>              _fncOnError;
	shared_ptr<function<int(HTTPSCacheSession*, shared_ptr<KBinData>, HTTPResponse&)>> _fncOnReceivedRequest;
	shared_ptr<function<int(HTTPSCacheSession*, HTTPResponse&)>> _fncGET; //HTTPRequest&,
	shared_ptr<function<int(HTTPSCacheSession*, shared_ptr<KBinData>, HTTPResponse&)>> _fncPOST;//
	shared_ptr<function<int(HTTPSCacheSession&)>>                              _fncOnConnected;
	shared_ptr<function<int(HTTPSCacheSession&)>>                              _fncOnHandshaked;
	shared_ptr<function<int(HTTPSCacheSession&)>>                              _fncOnDisconnected;
	shared_ptr<function<int(HTTPSCacheSession*, size_t, size_t)>>              _fncOnSent;
	shared_ptr<function<int(HTTPSCacheSession*, uint8_t*, size_t)>>            _fncOnSentKw;
	shared_ptr<function<int(HTTPSCacheSession*, const void*, size_t)>>         _fncOnReceived;
	shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&)>> _fncOnReceivedRequestInternal;
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

	//shared_ptr<CacheBin> _cache;
	//shared_ptr<Cache> _cacheOld;
// 	void setCache(shared_ptr<CacheBin> cache) {
// 		if(!_cache)
// 			_cache = cache;
// 	}
	CacheBin& getCache() {
		return CacheBin::GetInstance();
// 		if (!_cache)
// 			_cache = shared_ptr<CacheBin>(new CacheBin());
// 		return *_cache.get();
	}
// 	Cache& getCacheOld() {
// 		//return Cache::GetInstance();
// 		if (!_cacheOld)
// 			_cacheOld = shared_ptr<Cache>(new Cache());
// 		return *_cacheOld.get();
// 	}
};

/// KHttp\pkg\CppServer\examples
// #include "asio_service.h" // AsioService => CAsioService 로 대체
class CAsioService : public CppServer::Asio::Service
{
public:
	using CppServer::Asio::Service::Service; 
	/// 이거 지우면 아래 오류난다. 
	/// 아마 std::enable_shared_from_this<Service> 이기 때문 인듯
	
	shared_ptr<function<int(int, const string&, const string&)>>               _fncOnErrorAsio;
	template<typename TFNC> void AddCallbackOnErrorAsio(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(this->_fncOnErrorAsio, fnc);
	}

protected:
	void onError(int error, const std::string& category, const std::string& message) override;
};

/// CppServer::Asio::Service의 derived class이며, 
/// App전체에서 Server가 여러개 일지라도 하나만 시작하면 되므로 static으로 처리한다.
///?deprecated
class CAsioSvcInst
{
public:
	~CAsioSvcInst()
	{
		ShutdownService();
	}

	shared_ptr<CAsioService> _service;

	void ShutdownService();
	
	// ASIO IO Service가 thread 수가 4개를 사용 한다.
	static CAsioSvcInst* getAsioService(int nThread = 4)
	{
		static CAsioSvcInst g_asioService;
		if(!g_asioService._service)
			g_asioService._service = std::make_shared<CAsioService>(nThread);
		return &g_asioService;
	}

	static CKTrace& getTrace()
	{
		static CKTrace s_std_cout;
		return s_std_cout;
	}
	static CKTrace& getTraceD()
	{
		static CKTrace s_std_cout(true);
		return s_std_cout;
	}
};
// extern CKTrace std_cout;// Debug 와 추가 출력 동시에 내 보낼때
// extern CKTrace std_coutD;// Debug만 내보낼려면
#define std_cout CAsioSvcInst::getTrace()
#define std_coutD CAsioSvcInst::getTraceD()


inline void CAsioService::onError(int error, const std::string& category, const std::string& message)
{
	/// 서버 StartServer 중 포트 충돌나면 여기서 오류 난다. 하지만 부른쪽으로 전달이 안된다.
	///         "Asio service caught an error with code      10048     and category       'system'       : 각 소켓 주소(프로토콜/네트워크 주소/포트)는 하나만 사용할 수 있습니다"	std::string

	std_cout << "Asio service error code: " << error << " error category: '" << category << "'" << std_endl;
	std_cout << "   " << message << std_endl;
	if(_fncOnErrorAsio.get())
		(*_fncOnErrorAsio)(error, category, message);
}
inline void CAsioSvcInst::ShutdownService()
{
	if(_service && _service->IsStarted())
	{
		// Stop the Asio service
		///?error: static객체가 가 삭제 될때 이니 로그 쓸 필요 없고, 죽는다.
		/// CKTrace 객체가 먼저 사라져서. CSingleLock::CSingleLock 에서 죽는다.
		///std_coutD << "Asio service stopping..." << std_endl;
		_service->Stop();
		///std_coutD << "Done!" << std_endl;
	}
}




/// Server하나당 이거 하나씩 사용 하며, CAsioService는 각 Server마다 공유 하지만
///	Server는 각 리스너 마다 하나씩 갖는다.
class CMyHttps
{
public:
	void SampleData(int port = 9480
		, const char* vpath = NULL // "/api"
		, const char* path = NULL);// "C:/Dropbox/Proj/STUDY/boostEx/CppServer/CppServer-master/www/api");
		~CMyHttps();
	
	int _port;
	BOOL _bStaticCache{ TRUE };
	string _cachedPath;
	string _cachedUrl{ "/" };

	string _certificate;//server certificate pem file path
	string _privatekey;//server private key pem file path
	string _dhparam; //Diffie Hellman key pem file path
	string _prvpwd; //private key password

	shared_ptr<CAsioService> _service;
	shared_ptr<HTTPSCacheServer> _server;

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
	template<typename TFNC> void AddCallbackOnHandshaked(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnHandshaked, fnc, bOvWR);
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

/// SSL class와 NoSSL class 함수들이 SSL부분 말고는 워낙 흡사 하여, 
/// 여기 template 함수를 모아 둔다.
class HttpCmn
{
public:
	static void GatherBasicInfo(const HTTPRequest& request, KSessionInfo& sinf);//, string& url, string& sparams, string& dir, string& stCached, std::map<string, string>& urlparam, std::map<string, string>& headers);

	// 이건 이미 utf8로 변환 된거로 전제.
	static HTTPResponse& MakeJsonResponse(HTTPResponse& res, std::string_view js, int status = 200)
	{
		if(status != 200)
			res.SetBegin(status);
		res.MakeGetResponse(js, "application/json; charset=UTF-8");// "text/plain" default
		return res;
	}

	// js는 multi bytes지 utf8이 아니므로 변환 한다.
	static HTTPResponse& MakeStrErrorToJsonResponse(HTTPResponse& res, std::string_view sErr, int status = 400)
	{
		string jerror = JError(sErr.data(), -1, status); // status를 json에 넣기 위해 = 400
		return MakeJsonErrorResponse(res, jerror, status);
	}
	// 
	static HTTPResponse& MakeJsonErrorResponse(HTTPResponse& res, std::string_view jerror, int status = 400)
	{
// 		std::wstring wjerror;// error는 영문이 대부분이지만 한글이 있을지 모르니, UTF8로 변경해 보낸다.
// 		wjerror.assign(jerror.begin(), jerror.end());
// 		string utf8 = CppCommon::Encoding::ToUTF8(wjerror);
//		string utf8 = jerror.data();/// 이미 utf8로 온다.
		res.MakeErrorResponse(jerror, status);
		return res;
	}


	static string JError(const char* msg, int rv = -1, int status = 400)
	{
		// S_FALSE = 1  FAILED = -1  S_OK = 0
		std::stringstream ss;
		//ss << "{\"error\":\"" << msg << "\", \"return\":" << rv << "}";
		ss << "{\"error\":\"" << msg << "\", \"return\":" << rv << "\", \"status\":" << status << "}";
		return ss.str();
	}

	template<typename TSVC, typename TSVR>
	static void restart_server(TSVC service, TSVR server)
	{
		if (service.get() == nullptr || server.get() == nullptr)
			return;
		std_cout << "Server restarting..." << std_endl;
		if(server->IsStarted())
			server->Restart();
		else
			server->Start();
		//std_cout << "Done!" << std_endl;
	}


	template<typename TSVC, typename TSVR>
	static void stop_server(TSVC& service, TSVR& server)
	{
		// CMFCServerView::OnDestroy() 에서 제일 먼저 불려진다.
		if (!service || !server || !server->IsStarted())
			return;
		std_cout << "Server stopping..." << std_endl;
		server->Stop();
		//std_cout << "Done!" << std_endl;
	}
	template<typename TSVC, typename TSVR>
	static void shutdown_server(TSVC& service, TSVR& server)//?Shutdown 
	{
		// CMFCServerView::OnDestroy() 에서 제일 먼저 불려진다.
		//server->ResetEvent();
		if(!service || !server)
			return;
		std_cout << "Server shutdowning..." << std_endl;
		if(server->IsStarted())
			server->Stop();//?destroy 3
		//std_cout << "Done!" << std_endl;
	}

	
	//	static void onReceivedRequest(HTTPSCacheSession* session, const HTTPRequest& request);

		//HTTPSCacheSession or HTTPCacheSession
	template<typename TSESSION, typename TSVR>
	static int CheckCacheFirst(string& keyRes, CacheVal& cval, TSVR* svr, TSESSION* ssn, HTTPRequest& request)
	{
		// 바로직전 변경데이터를 캐시 없이 DB분명히 읽어야 할 필요가 있을때만 캐시 키 삭제하고 DB읽음을 강요한다.
		if (keyRes.size() > 0)
		{
			auto& che = svr->getCache();
			auto& m = ssn->_sinfo._urlparam;
			auto& h = ssn->_sinfo._headers;
			bool b1 = h.Get("Cache-Control") == "no-cache";
			///?주의: POST일때는 client에서 이걸 안주었는데 default로 붙어 버려서 POST cache는 기대 할 수 없다.   그래서 srl=로만 판독 한다.
			bool b2 = m.Get("noCache") == "true";
			if (b2) //b1 || 
			{// m["noCache"] 하는 순간 Pair가 만들어진다.
				che.DeleteCacheValue(keyRes, cval);
			}
			else
			{
				auto b1 = che.GetCacheValue(keyRes, cval);
				if(b1)
				{
					//ssn->_sinfo._stCached = "cached"; 부른쪽에서 해야지.
					//ssn->SendResponseAsync(MakeJsonResponse(res1, valueRes));
					return 1;
				}
			}
		}
		return 0;
	}

	static void GetResponseHeader(HTTPResponse& res1, std::map<string, string>& mapHd)
	{
		auto szHd = res1.headers();
		for (size_t i = 0; i < szHd; i++) {
			auto hd = res1.header(i);
			mapHd[(string)std::get<0>(hd)] = std::get<1>(hd);
		}
	}

	template<typename TSESSION, typename TSVR>
	static void onReceivedRequest(TSVR* svr, TSESSION* ssn, HTTPRequest& request)
	{
		/*
		POST /api?key=xxx&srl=30363 HTTP/1.1
		Accept: *\/*
		Accept-Language: ko
		UA-CPU: AMD64
		Accept-Encoding: gzip, deflate
		User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.2; Win64; x64; Trident/7.0;.NET4.0C;.NET4.0E)
		Host: www.xxxx.co.kr:9480
		Content-Length: 71
		Connection: Keep-Alive
		Cache-Control: no-cache

		{"func":"SampleSelect", "params": {"table":"tapppolicy", "limit":"20"}}
		*/
 		HTTPResponse& res1 = ssn->response();
		HTTPRequest& req1 = (HTTPRequest&)request; // this->request(); kdw가 추가
		//string body(req1.body());
		///[ IP address를 알아 내는 테스트 영역
		/// sck을 넘겨 주려 했는데, type이 너무 길어서 여기서 그냥 IP를 뽑아 낸다.
		auto& sck = ssn->socket();//boost::asio::ip::tcp::socket& socket()
		auto ep = sck.remote_endpoint();
		auto ipa = ep.address();
		auto ips = ipa.to_string();

		/// 기본 데이터 수집. _url, _dir, _sparams, _urlparam
		ssn->_sinfo.GatherBasicInfo(request, ips);//_url, _sparams, _dir, _stCached, _urlparam, _headers);
		ssn->_sinfo._ssid = ssn->id().string();
		//-ep{ impl_ = {data_ = {base = {sa_family = 2 sa_data =  <> } v4 = {sin_family = 2 ...} ...} } }	
//	boost::asio::ip::basic_endpoint<boost::asio::ip::tcp>
//+		ipa	{type_=ipv4 (0) ipv4_address_={addr_={S_un={S_un_b={s_b1=127 '' s_b2=0 '\0' s_b3=0 '\0' ...} S_un_w=...} } } ...}	
//		boost::asio::ip::address
		//+ips	"127.0.0.1"	
		///] IP address를 알아 내는 테스트 영역
	//	string ssid = ss->id().string();

		int rv = 0;
		/*
		KAtEnd d_monitor([&, ssn]()
			{
				/// 여기서 monitor UI 예약 해야. 캐시까지 모니터 가능.
				/// 전엔 OnSentKw에서 했지만, 그건 여러번 불려 지니, 
				/// onPOST 에서 하면 캐시를 못 가로채니.
				/// 아래 POST에서 또 비동기 호출 하니 여기서 해봐야 소용 없다.
				/// onSent 여러번 호출중에 맨마지막 pending size가 sent > size 일때 마지막 이므로 그때하면 되지 않을까
			});*/
		/// // BG에서 쓰려면 이걸 써야 한다.////////////////////////////////////////
		auto sh_ss1(ssn->shared_from_this());// enable_shared_from_this<> 계승 받아서 가능. 이렇게 해도 되는지 확인해 보자
		/// //////////////////////////////////////////

		shared_ptr<KBinData> shbinDumy;
		if(svr->_fncOnReceivedRequest)
			(*svr->_fncOnReceivedRequest)((TSESSION*)sh_ss1.get(), shbinDumy, res1);

		//string s = boost::lexical_cast<string>(sck.remote_endpoint());
		if(request.method() == "HEAD")
		{
			ssn->SendResponseAsync(res1.MakeErrorResponse("Unsupported HTTP method: " + string(request.method())));
			//ssn->SendResponseAsync(res1);// .MakeHeadResponse());
		}
		else if(request.method() == "GET")
		{
			// 이게 통신 되는데 왜 주의요함 즉 http로 붙냐?
			if (ssn->_sinfo._url.empty())
			{
				HttpCmn::MakeStrErrorToJsonResponse(res1, "URL is empty!");
				ssn->SendResponseAsync(res1);// MakeJsonResponse(res1, JError("URL is empth.")));
				return;
			}
			else
			{
#ifdef _DEBUG
				string s = ssn->_sinfo._url;
				string ext;
				int length = 3;// ssn->_sinfo._url.size();
				if(length >= s.size()) { ext = s; }
				else
					ext = s.substr(s.size() - length);
				if(ext == "png")
				{
					ext = ext;
				}
#endif // _DEBUG
			}
			// uuid 와 srl 빼고 
			string keyRes = ssn->getCacheKey("uuid");//_urlparam이 이미 들어 있어야 리턴. _url
// 			if(keyRes.size() == 0)//?이후 URL파라미터가 없으면 
// 				keyRes = "index.html";// ssn->_sinfo._url;// "/"만 남는다. 

			string valueRes;
			//CacheVal cval;
			shared_ptr<CacheVal> scval = make_shared<CacheVal>();
			int rchche = 0;
			if(svr->_bChache)
			{
				rchche = CheckCacheFirst(keyRes, *scval, svr, ssn, request);
				TRACE("%s : %s\n", scval->_contentType.c_str(), keyRes.c_str());
				if(rchche == 1) {
					ssn->_sinfo._stCached = "cached";
					valueRes.insert(0, scval->_data.m_p, scval->_data.m_len);
					res1.MakeGetResponse(valueRes, scval->_contentType);
					ssn->SendResponseAsync(res1);// MakeJsonResponse(res1, valueRes));
					return;
				}
			}

			if(svr->_fncGET.get())
			{
				ssn->_sinfo._stCached = "NoCach";

				shared_ptr<KBinData> shbin = std::make_shared<KBinData>();//req1.body().size()
				shbin->Attach(req1.body().data(), req1.body().size());

				auto sh_ss = svr->FindSession(ssn->id());//아래 람다 안으로 복사 하려고, 마침 shared_ptr을 리턴 한다.
				auto sh_ss1(ssn->shared_from_this());// enable_shared_from_this<> 계승 받아서 가능. 이렇게 해도 되는지 확인해 보자
				/// ssn == sh_ss == sh_ss1 == ssn1
				// lambda 여기서 부터는 비즈니스로직과 함께 response 까지 다른 쓰레드에서 한다.
				auto bg_run = [svr, sh_ss, keyRes, shbin, &res1, scval, rchche]()
				{
					auto ssn1 = (TSESSION*)sh_ss.get();
					auto fnc = svr->_fncGET.get();
					if(rchche == 1)
					{
						_break;
					}
					/// ///////////////////////////////////////////////////////////////
					/// res1에 담아 올거면서 shbin은 왜주는 거냐?
					int rv = (int)(*fnc)(ssn1, res1); // => CmnView::CallbackOnGET
					/// ///////////////////////////////////////////////////////////////

					if(rv == 0)
					{//성공한 경우만 캐시에 넣는다.
						if(svr->_bChache)
						{
#ifdef _DEBUG
							int CompareData(string url, string_view resbody, shared_ptr<KBinData> shbin, shared_ptr<CacheVal> scval, int rchche);
							int rcv = CompareData(ssn1->_sinfo._url, res1.body(), shbin, scval, rchche);

#endif // _DEBUG
							std::map<string, string> mapHd;
							GetResponseHeader(res1, mapHd);
							string cntype = mapHd["Content-Type"];
							svr->getCache().PutCacheValue(keyRes, res1.body(), cntype); // 성공한 값은 케시에 넣음
						}
						if(ssn1->_sinfo._status != 200)
							res1.SetBegin(ssn1->_sinfo._status);
					}
					else if(rv < 0) // + 이면 이미 오류메시지 들어 있다.
						HttpCmn::MakeStrErrorToJsonResponse(res1, "Unknown Remote GET API Error!");

					ssn1->SendResponseAsync(res1);//이미 reponse JSON packet을 넣음.
				};
				QueueFUNC(bg_run);
			}
			//assert(svr->_fncGET.get());//_fncGET 는 반드시 있다.
		}
		else if((request.method() == "POST") || (request.method() == "PUT"))
		{
			int rv = 0;
			// srl 빼고 
			string keyRes = ssn->getCacheKey("uuid");// uuid"는 캐시 에서 제외. 캐시 안하려면 srl=1234를 줘야 한다.
			keyRes += request.body();
			
			string valueRes;
			CacheVal cval;
			rv = CheckCacheFirst(keyRes, cval, svr, ssn, request);
			if (rv == 1)	{
				ssn->_sinfo._stCached = "cached";
				valueRes.insert(0, cval._data.m_p, cval._data.m_len);
				res1.MakeGetResponse(valueRes, cval._contentType);
				ssn->SendResponseAsync(res1);// MakeJsonResponse(res1, valueRes));
				return;
			}

			std::map<string, string> mapHd;
			GetResponseHeader(res1, mapHd);
			string cntype = mapHd["Content-Type"];

			/// auto sh_ss = svr->FindSession(ssn->id()); 이것도 되지만, shred 안되어 있으므로 sh_ss1 을 사용

			auto& ainfo = ssn->_sinfo;
			/// URL에서 SSL, 주소, 포트 뺀 _dir과 parameter를 그대로
			/// request 한다. POST 데이터는 그대로
			/// 그것은 ainfo._url 이다.
			/// 재요청할때 ainfo._headers 를 전부 헤더로 붙여 줘야 한다.
#ifdef _DEBUG
// 			shared_ptr<KBinData> shbin = std::make_shared<KBinData>();
// 			shbin->Attach(req1.body().data(), req1.body().size());// 진짜 데이터 부분만 이다.
			string_view sprt = req1.protocol();	//	"HTTP/1.1"
			string turl = ssn->_sinfo._url;
#endif // _DEBUG
#define _CLUSTER
#ifdef _CLUSTER
			if(svr->_fncCluster)//CSrvView::CallbackCluster
			{
				//auto bg_run = [svr, sh_ss, keyRes, shbin, cntype, &res1]() {
				shared_ptr<KBinData> shbin = std::make_shared<KBinData>();
				shbin->Attach(req1.body().data(), req1.body().size());
				int rv = (int)(*svr->_fncCluster)(ssn->_sinfo, shbin);// => CResponse1::ResponseForPost
				if(rv == 1) {//1:분산 처리됨. 2: localhost 지금 이거 이므로 통과
					ssn->_sinfo._stCached = "distrib";
					valueRes.insert(0, shbin->m_p, shbin->m_len);
					res1.MakeGetResponse(valueRes, cntype);
					ssn->SendResponseAsync(res1);
					return;
				}
				//}; QueueFUNC(bg_run);
			}
#endif // _CLUSTER
			if(svr->_fncPOST)//앞에 const 가 있어서
			{
				ssn->_sinfo._stCached = "NoCach";
				/// 아래 처리 루틴을 비동기로 하면 data를 챙겨야 한다. 이 함수 뒤에서 바로 request는 Clear 되기 때문이다.
				shared_ptr<KBinData> shbin = std::make_shared<KBinData>();
				shbin->Attach(req1.body().data(), req1.body().size());

 				if(ssn->_sinfo._ip.length() == 0)
 					TRACE("");
				// lambda 여기서 부터는 비즈니스로직과 함께 response 까지 다른 쓰레드에서 한다.
				auto bg_run = [svr, sh_ss1, keyRes, shbin, cntype, &res1]()
				{
					auto ssn1 = (TSESSION*)sh_ss1.get();
					if(ssn1->_sinfo._ip.length() == 0)
						TRACE("");///?error: onSentKw에서 _sinfo.Clear(); 해버리니. onSentKw 는 여러번 불리는데.
					auto sh_ss2(ssn1->shared_from_this());// enable_shared_from_this<> 계승 받아서 가능. 이렇게 해도 되는지 확인해 보자
					auto ssn2 = (TSESSION*)sh_ss2.get();

 					if(ssn2->_sinfo._ip.length() == 0)
 						TRACE("");

					auto fnc = svr->_fncPOST.get();
					/// ///////////////////////////////////////////////////////////////
					int rv = (int)(*fnc)(ssn1, shbin, res1);// => CResponse1::ResponseForPost
					/// ///////////////////////////////////////////////////////////////
					/// //-1: error메시지 이미 생성. -2: No 오류 메시지 , 0:OK
					if(rv == 0)
					{//성공한 경우만 캐시에 넣는다.
						string value(res1.body());
						svr->getCache().PutCacheValue(keyRes, value, cntype); // 성공한 값은 케시에 넣음
						if(ssn1->_sinfo._status != 200)
							res1.SetBegin(ssn1->_sinfo._status);
					}
					else if(rv < 0) // + 이면 이미 오류메시지 들어 있다.
					{
						if(rv == -2)
							HttpCmn::MakeStrErrorToJsonResponse(res1, "Unknown Remote POST API Error!");
					}
					//((TSESSION*)sh_ss.get())
					ssn1->SendResponseAsync(res1);//이미 reponse JSON packet을 넣음.
				};
				QueueFUNC(bg_run);
			}
			assert(svr->_fncPOST.get());//_fncPOST 는 반드시 있다.
		}
		else if(request.method() == "DELETE")
		{
			ssn->SendResponseAsync(res1.MakeErrorResponse("Unsupported HTTP method: " + string(request.method())));
			return;

#ifdef _DEBUGxx
			ssn->SendResponseAsync(res1.MakeGetResponse("{}", "application/json; charset=UTF-8"));

			string key(request.url());
			string value;

			// Decode the key value
			key = CppCommon::Encoding::URLDecode(key);
			CppCommon::StringUtils::ReplaceFirst(key, "/api/cache", "");
			CppCommon::StringUtils::ReplaceFirst(key, "?key=", "");
			CacheVal cval;
			// Delete the cache value
			if(svr->getCache().DeleteCacheValue(key, cval))
			{
				// Response with the cache value
				value.insert(0, cval._data.m_p, cval._data.m_len);
				ssn->SendResponseAsync(res1.MakeGetResponse(value));
			}
			else
				ssn->SendResponseAsync(res1.MakeErrorResponse("Deleted cache value was not found for the key: " + key, 404));
#endif // _DEBUGxx
		}
		else if(request.method() == "OPTIONS")
		{
			ssn->SendResponseAsync(res1.MakeErrorResponse("Unsupported HTTP method: " + string(request.method())));
			//ssn->SendResponseAsync(res1.MakeOptionsResponse());
		}
		else if(request.method() == "TRACE")
		{
			ssn->SendResponseAsync(res1.MakeErrorResponse("Unsupported HTTP method: " + string(request.method())));
			//ssn->SendResponseAsync(res1.MakeTraceResponse(request.cache()));
		}
		else
			ssn->SendResponseAsync(res1.MakeErrorResponse("Unsupported HTTP method: " + string(request.method())));
	}

};

