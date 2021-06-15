#pragma once
/// KHttp\src\KHttpsSrv\MFCAppServerEx2\HttpsSvr.h

//#include <boost/lexical_cast.hpp>

/// KHttp\pkg\CppCommon\include\string
#include "string/string_utils.h" // CppCommon::StringUtils::ReplaceFirst
//#include "utility/singleton.h"

/// KHttp\pkg\CppServer\include\server
#include "server/kwadjust.h"
#include "server/asio/service.h" // CppServer::Asio::Service
#include "server/asio/ssl_session.h" // server override � �ϴ� �̰� �ʿ���
#include "server/asio/ssl_server.h"
#include "server/http/https_server.h"

/// KHttp\src\KHttpsSrv\MFCAppServerEx2
#include "Cache.h"
// #include <memory>
// #include <functional>

//using namespace std;
using std::string;
using std::shared_ptr;
using std::function;

using CppServer::Asio::SSLSession;
using CppServer::Asio::SSLServer;

using CppServer::HTTP::HTTPRequest;
using CppServer::HTTP::HTTPResponse;
//using CppServer::HTTP::HTTPSSession;
//using CppServer::HTTP::HTTPSServer;

//#define SPtr std::shared_ptr; // �������� 



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
			m_p = new char[len + 1];
			m_len = len;
			memcpy(m_p, p, len);
			m_p[len] = '\0';
		}
	}
};

/// <summary>
/// ���ٸ� ������ �Ҵ� �Ҷ� new function �ؾ� �ϴµ�, �Լ��� �Ķ���͸� ������ �� �ֵ��� template�Լ��� �����.
/// �Ʒ� ���� ���� 'HTTPSCacheSession*, shared_ptr<KBinData>, HTTPResponse&' �κ��� ������ �� �ִ�.
/// KwLib64/KTemple.h KwCreateFuncValue �� �����ϰ� ������ �ܺ� �ڵ�� KTemple.h�� include���� �ʱ� ���� �ִ´�.
/// </summary>
/// <typeparam name="TFNC"></typeparam>
/// <typeparam name="TLAMBD"></typeparam>
/// <param name="rtval"></param>
/// <param name="lambda"></param>
template<typename TFNC, typename TLAMBD>
void TCreateFuncValue(shared_ptr<TFNC>& rtval, TLAMBD lambda)
{
/* ex
// ������ ���� ������ ���� �Ǿ��ٰ� ����
shared_ptr<function<TRAMBD>> _fncGET;

// ���� ���� : TFNC == function<int(HTTPSCacheSession*, shared_ptr<KBinData>, HTTPResponse&)> �� ���̱� ����
shared_ptr<function<int(HTTPSCacheSession*, shared_ptr<KBinData>, HTTPResponse&)>> _fncGET;

// ex
template<typename TFNC> void AddCallbackOnGET(TFNC fnc)
{
	TCreateFuncValue(_server->_fncGET, fnc);  �Ʒ��� ����
	//_server->_fncGET = std::make_shared<function<int(HTTPSCacheSession*, shared_ptr<KBinData>, HTTPResponse&)>>(fnc);
}*/

//rtval = shared_ptr<TFNC>(new TFNC(lambda)); �Ʒ��� ����
	rtval = std::make_shared<TFNC>(lambda);
}

class KSessionInfo
{
public:
	string _ip;
	string _method;
	string _url;
	string _sparams;
	string _dir;
	string _stCached;//{"cached"/"NoCach"};
	KStrMap _urlparam;
	KStrMap _headers;

	/// return �Ҷ� http code: error������ HTTPResponse::SetBegin(status)
	// ResponseForPost ���� error ������ �ְ�, 
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
		_url.clear(); //empty();�� �˻��Լ�
		_sparams.clear();
		_dir.clear();
		_stCached.clear();

		_urlparam.clear();
		_headers.clear();
	}
	void GatherBasicInfo(const HTTPRequest& request, string ips);//, KSessionInfo& sinf);//, string& url, string& sparams, string& dir, string& stCached, std::map<string, string>& urlparam, std::map<string, string>& headers);
};


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

	//?kdw virtual�� �ƴѰ� virtual�� ���� override�Ѵ�.
	// �̰��� ����ĳ�÷� �����Ҷ��� �ҷ�����.
	void onReceivedRequestInternal(const HTTPRequest& request) override;

public:
	KSessionInfo _sinfo;

	//void MakeJsonError(HTTPResponse& res1, char* errmsg, int err = -1);
	//void ResponseError(char* errmsg, int err = -1);

	/// <summary>
	/// url params���� �� ����� ���� �ٸ� 'notKey'�� ���� ĳ�ÿ� Ű�� �����.
	/// "uuid"�� client����� �����̰�, "srl"�� ĳ�ð� �ȵǰ� �׽�Ʈ ���̴�.
	/// </summary>
	std::string getCacheKey(const char* notKey, const char* notKey2 = nullptr)
	{
		Tas ss;
		for(auto it: _sinfo._urlparam)
		{
			if(it.first != notKey && (!notKey2 || it.first != notKey2))//"uuid"=notKey �� srl�� ���� ������ �� Ű�� �����Ѵ�.
				ss << it.first << it.second;
		}
		if(ss.str().length() == 0)
			ss << "/";
		return ss.str();
	}

	shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&)>> _fncOnReceivedRequestInternal;
	shared_ptr<function<int(HTTPSCacheSession*, const void*, size_t)>> _fncOnReceived;
	shared_ptr<function<int(HTTPSCacheSession*, size_t, size_t)>>      _fncOnSent;
	shared_ptr<function<int(HTTPSCacheSession*, uint8_t*, size_t)>>    _fncOnSentKw;
};
typedef HTTPSCacheSession THCSsn; //�ʹ� �� ��µ�, ����/���� ����� ��ɿ��� �ȵǾ �Ⱦ���� �ߴ�.


class HTTPSCacheServer : public CppServer::HTTP::HTTPSServer
{
public:
	using CppServer::HTTP::HTTPSServer::HTTPSServer;

	//HTTPSCacheServer() :_fncGET(0), _fncPOST(0)
	//{//error C2512: 'CppServer::HTTP::HTTPSServer': ����� �� �ִ� ������ �⺻ �����ڰ� �����ϴ�.
	//}
	~HTTPSCacheServer()
	{
		//if(_fncGET)//shared_ptr�� ���� �ʾұ� ������ free����� �Ѵ�.
		//	delete _fncGET;
		//if(_fncPOST)
		//	delete _fncPOST;
	}
protected:
	shared_ptr<SSLSession> CreateSession(const shared_ptr<SSLServer>& server) override
	{
		auto ss = std::make_shared<HTTPSCacheSession>(std::dynamic_pointer_cast<HTTPSServer>(server));
		ss->_fncOnReceivedRequestInternal = _fncOnReceivedRequestInternal;
		ss->_fncOnReceived = _fncOnReceived;
		ss->_fncOnSent = _fncOnSent;
		ss->_fncOnSentKw = _fncOnSentKw;
		auto ip = server->address();
		return ss;
	}

protected:
	void onError(int error, const string& category, const string& message) override;
	void onStarted() override;
	void onStopped() override;
	//C:\Dropbox\Proj\STUDY\boostEx\CppServer\CppServer-master\include\server\asio\ssl_session.h
	void onConnected(shared_ptr<SSLSession>& session) override;
	void onHandshaked(shared_ptr<SSLSession>& session) override;
	void onDisconnected(shared_ptr<SSLSession>& session) override;

	//kdw added 
public: // �Ʒ��� �������� ����.
	//�ΰ��� �Ϻη� �׳� �����ͷ� �غ���.
	//function<int(HTTPSCacheSession*, const HTTPRequest&, HTTPResponse&)>* _fncGET;
	//function<int(HTTPSCacheSession*, const HTTPRequest&, HTTPResponse&)>* _fncPOST;
	//�޸� ������������ shared_ptr�� �ִ´�.

	shared_ptr<function<void()>>                                               _fncOnStarted;
	shared_ptr<function<void()>>                                               _fncOnStopped;
	shared_ptr<function<int(int, const string&, const string&)>>               _fncOnError;
	shared_ptr<function<int(HTTPSCacheSession*, shared_ptr<KBinData>,HTTPResponse&)>> _fncGET; //HTTPRequest&,
	shared_ptr<function<int(HTTPSCacheSession*, shared_ptr<KBinData>,HTTPResponse&)>> _fncPOST;//
	shared_ptr<function<int(HTTPSCacheSession&)>>                              _fncOnConnected;
	shared_ptr<function<int(HTTPSCacheSession&)>>                              _fncOnHandshaked;
	shared_ptr<function<int(HTTPSCacheSession&)>>                              _fncOnDisconnected;
	shared_ptr<function<int(HTTPSCacheSession*, size_t, size_t)>>              _fncOnSent;
	shared_ptr<function<int(HTTPSCacheSession*, uint8_t*, size_t)>>            _fncOnSentKw;
	shared_ptr<function<int(HTTPSCacheSession*, const void*, size_t)>>         _fncOnReceived;
	shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&)>> _fncOnReceivedRequestInternal;
	shared_ptr<function<void(string)>>                                          _fncOnTrace;
	
	void ResetEvent()
	{
		auto server = this;
		server->_fncOnStarted.reset();
		server->_fncOnStopped.reset();
		server->_fncOnError.reset();
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

/// KHttp\pkg\CppServer\examples
// #include "asio_service.h" // AsioService => CAsioService �� ��ü
class CAsioService : public CppServer::Asio::Service
{
public:
	using CppServer::Asio::Service::Service; 
	/// �̰� ����� �Ʒ� ��������. 
	/// �Ƹ� std::enable_shared_from_this<Service> �̱� ���� �ε�
protected:
	void onError(int error, const std::string& category, const std::string& message) override;
};

/// CppServer::Asio::Service�� derived class�̸�, 
/// App��ü���� Server�� ������ ������ �ϳ��� �����ϸ� �ǹǷ� static���� ó���Ѵ�.
class CAsioSvcInst
{
public:
	~CAsioSvcInst()
	{
		ShutdownService();
	}

	shared_ptr<CAsioService> _service;

	void ShutdownService();
	
	// ASIO IO Service�� thread ���� 4���� ��� �Ѵ�.
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
// extern CKTrace std_cout;// Debug �� �߰� ��� ���ÿ� �� ������
// extern CKTrace std_coutD;// Debug�� ����������
#define std_cout CAsioSvcInst::getTrace()
#define std_coutD CAsioSvcInst::getTraceD()


inline void CAsioService::onError(int error, const std::string& category, const std::string& message)
{
	std_cout << "Asio service caught an error with code " << error << " and category '" << category << "': " << message << std_endl;
}
inline void CAsioSvcInst::ShutdownService()
{
	if(_service && _service->IsStarted())
	{
		// Stop the Asio service
		///?error: static��ü�� �� ���� �ɶ� �̴� �α� �� �ʿ� ����, �״´�.
		/// CKTrace ��ü�� ���� �������. CSingleLock::CSingleLock ���� �״´�.
		///std_coutD << "Asio service stopping..." << std_endl;
		_service->Stop();
		///std_coutD << "Done!" << std_endl;
	}
}

/// Server�ϳ��� �̰� �ϳ��� ��� �ϸ�, CAsioService�� �� Server���� ���� ������
///	Server�� �� ������ ���� �ϳ��� ���´�.
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
	// ������ ���� �Ǹ� _server->_fncOnTrace = _fncOnTrace;

public:
	void restart_server();
	void stop_server();
	void shutdown_server();
	int start_server();
	
	template<typename TFNC> void AddCallbackOnGET(TFNC fnc)
	{
		//_server->_fncGET = std::make_shared<function<int(HTTPSCacheSession*, shared_ptr<KBinData>, HTTPResponse&)>>(fnc);
		TCreateFuncValue(_server->_fncGET, fnc);//�Լ��� �Ķ���͸� ������ �� �ֵ���
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
	template<typename TFNC> void AddCallbackOnHandshaked(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnHandshaked, fnc);
	}
	template<typename TFNC> void AddCallbackOnDisconnected(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnDisconnected, fnc);
	}
	template<typename TFNC> void AddCallbackOnReceived(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnReceived, fnc);
	}
	template<typename TFNC> void AddCallbackOnReceivedRequestInternal(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnReceivedRequestInternal, fnc);
	}
	template<typename TFNC> void AddCallbackOnSent(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnSent, fnc);
	}
	template<typename TFNC> void AddCallbackOnSentKw(TFNC fnc)
	{
		TCreateFuncValue(_server->_fncOnSentKw, fnc);
	}
	void AddCallbackOnTrace(shared_ptr<function<void(string)>> fnc)//?ExTrace 3.5 MyHttp -> std_cout(KTrace)
	{
		if(!_fncOnTrace)
		{
			_fncOnTrace = fnc;
			std_cout.AddCallbackOnTrace(fnc);
			std_coutD.AddCallbackOnTrace(fnc);
		}
	}

};

/// SSL class�� NoSSL class �Լ����� SSL�κ� ����� ���� ��� �Ͽ�, 
/// ���� template �Լ��� ��� �д�.
class HttpCmn
{
public:
	static void GatherBasicInfo(const HTTPRequest& request, KSessionInfo& sinf);//, string& url, string& sparams, string& dir, string& stCached, std::map<string, string>& urlparam, std::map<string, string>& headers);

	// �̰� �̹� utf8�� ��ȯ �Ȱŷ� ����.
	static HTTPResponse& MakeJsonResponse(HTTPResponse& res, std::string_view js, int status = 200)
	{
		if(status != 200)
			res.SetBegin(status);
		res.MakeGetResponse(js, "application/json; charset=UTF-8");// "text/plain" default
		return res;
	}

	// js�� multi bytes�� utf8�� �ƴϹǷ� ��ȯ �Ѵ�.
	static HTTPResponse& MakeStrErrorToJsonResponse(HTTPResponse& res, std::string_view sErr, int status = 400)
	{
		string jerror = JError(sErr.data(), -1, status); // status�� json�� �ֱ� ���� = 400
		return MakeJsonErrorResponse(res, jerror, status);
	}
	// 
	static HTTPResponse& MakeJsonErrorResponse(HTTPResponse& res, std::string_view jerror, int status = 400)
	{
		std::wstring wjerror;// error�� ������ ��κ������� �ѱ��� ������ �𸣴�, UTF8�� ������ ������.
		wjerror.assign(jerror.begin(), jerror.end());

		string utf8 = CppCommon::Encoding::ToUTF8(wjerror);
		res.MakeErrorResponse(utf8, status);
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
		// CMFCServerView::OnDestroy() ���� ���� ���� �ҷ�����.
		if (!service || !server || !server->IsStarted())
			return;
		std_cout << "Server stopping..." << std_endl;
		server->Stop();
		//std_cout << "Done!" << std_endl;
	}
	template<typename TSVC, typename TSVR>
	static void shutdown_server(TSVC& service, TSVR& server)//?Shutdown 
	{
		// CMFCServerView::OnDestroy() ���� ���� ���� �ҷ�����.
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
		// �ٷ����� ���浥���͸� ĳ�� ���� DB�и��� �о�� �� �ʿ䰡 �������� ĳ�� Ű �����ϰ� DB������ �����Ѵ�.
		if (keyRes.size() > 0)
		{
			auto& che = svr->getCache();
			auto& m = ssn->_sinfo._urlparam;
			auto& h = ssn->_sinfo._headers;
			if (
				//h.Get("Cache-Control") == "no-cache" || POST�϶��� �̰� ���־��µ� ��� �־ POST cache�� ��� �� �� ����.
				m.Get("noCache") == "true"
				) 
			{// m["noCache"] �ϴ� ���� Pair�� ���������.
				che.DeleteCacheValue(keyRes.c_str(), cval);
			}
			else
			{
				auto b1 = che.GetCacheValue(keyRes.c_str(), cval);
				if(b1)
				{
					ssn->_sinfo._stCached = "cached";
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
		HTTPRequest& req1 = (HTTPRequest&)request; // this->request(); kdw�� �߰�
		//string body(req1.body());
		///[ IP address�� �˾� ���� �׽�Ʈ ����
		/// sck�� �Ѱ� �ַ� �ߴµ�, type�� �ʹ� �� ���⼭ �׳� IP�� �̾� ����.
		auto& sck = ssn->socket();//boost::asio::ip::tcp::socket& socket()
		auto ep = sck.remote_endpoint();
		auto ipa = ep.address();
		auto ips = ipa.to_string();

		/// �⺻ ������ ����. _url, _dir, _sparams, _urlparam
		ssn->_sinfo.GatherBasicInfo(request, ips);//_url, _sparams, _dir, _stCached, _urlparam, _headers);
		//-ep{ impl_ = {data_ = {base = {sa_family = 2 sa_data =  <> } v4 = {sin_family = 2 ...} ...} } }	
//	boost::asio::ip::basic_endpoint<boost::asio::ip::tcp>
//+		ipa	{type_=ipv4 (0) ipv4_address_={addr_={S_un={S_un_b={s_b1=127 '' s_b2=0 '\0' s_b3=0 '\0' ...} S_un_w=...} } } ...}	
//		boost::asio::ip::address
		//+ips	"127.0.0.1"	
		///] IP address�� �˾� ���� �׽�Ʈ ����
	//	string ssid = ss->id().string();

		int rv = 0;
		/*
		KAtEnd d_monitor([&, ssn]()
			{
				/// ���⼭ monitor UI ���� �ؾ�. ĳ�ñ��� ����� ����.
				/// ���� OnSentKw���� ������, �װ� ������ �ҷ� ����, 
				/// onPOST ���� �ϸ� ĳ�ø� �� ����ä��.
				/// �Ʒ� POST���� �� �񵿱� ȣ�� �ϴ� ���⼭ �غ��� �ҿ� ����.
				/// onSent ������ ȣ���߿� �Ǹ����� pending size�� sent > size �϶� ������ �̹Ƿ� �׶��ϸ� ���� ������
			});*/

		//string s = boost::lexical_cast<string>(sck.remote_endpoint());
		if(request.method() == "HEAD")
		{
			HttpCmn::MakeStrErrorToJsonResponse(res1, "HEAD is not supported.", 400);
			ssn->SendResponseAsync(res1);// .MakeHeadResponse());
		}
		else if(request.method() == "GET")
		{
			// �̰� ��� �Ǵµ� �� ���ǿ��� �� http�� �ٳ�?
			if (ssn->_sinfo._url.empty())
			{
				HttpCmn::MakeStrErrorToJsonResponse(res1, "URL is empty!");
				ssn->SendResponseAsync(res1);// MakeJsonResponse(res1, JError("URL is empth.")));
				return;
			}
			// uuid �� srl ���� 
			string keyRes = ssn->getCacheKey("uuid");//_urlparam�� �̹� ��� �־�� ����. _url
// 			if(keyRes.size() == 0)//?���� URL�Ķ���Ͱ� ������ 
// 				keyRes = "index.html";// ssn->_sinfo._url;// "/"�� ���´�. 

			string valueRes;
			CacheVal cval;
			rv = CheckCacheFirst(keyRes, cval, svr, ssn, request);
			if (rv == 1)	{
				valueRes.insert(0, cval._data.m_p, cval._data.m_len);
				res1.MakeGetResponse(valueRes, (PAS)cval._contentType);
				ssn->SendResponseAsync(res1);// MakeJsonResponse(res1, valueRes));
				return;
			}

			if(svr->_fncGET.get())
			{
				ssn->_sinfo._stCached = "NoCach";

				shared_ptr<KBinData> shbin = std::make_shared<KBinData>();//req1.body().size()
				shbin->Attach(req1.body().data(), req1.body().size());

				auto sh_ss = svr->FindSession(ssn->id());//�Ʒ� ���� ������ ���� �Ϸ���, ��ħ shared_ptr�� ���� �Ѵ�.
				auto sh_ss1(ssn->shared_from_this());// enable_shared_from_this<> ��� �޾Ƽ� ����. �̷��� �ص� �Ǵ��� Ȯ���� ����
				/// ssn == sh_ss == sh_ss1 == ssn1
				// rambda ���⼭ ���ʹ� ����Ͻ������� �Բ� response ���� �ٸ� �����忡�� �Ѵ�.
				auto bg_run = [svr, sh_ss, keyRes, shbin, &res1]()
				{
					auto ssn1 = (TSESSION*)sh_ss.get();
					auto fnc = svr->_fncGET.get();
					/// ///////////////////////////////////////////////////////////////
					int rv = (int)(*fnc)(ssn1, shbin, res1);
					/// ///////////////////////////////////////////////////////////////

					if(rv == 0)
					{//������ ��츸 ĳ�ÿ� �ִ´�.
						string value(res1.body());
						std::map<string, string> mapHd;
						GetResponseHeader(res1, mapHd);
						string cntype = mapHd["Content-Type"];
						svr->getCache().PutCacheValue(keyRes.c_str(), value.c_str(), value.size(), cntype.c_str()); // ������ ���� �ɽÿ� ����
						if(ssn1->_sinfo._status != 200)
							res1.SetBegin(ssn1->_sinfo._status);
					}
					else if(rv < 0) // + �̸� �̹� �����޽��� ��� �ִ�.
						HttpCmn::MakeStrErrorToJsonResponse(res1, "Unknown Remote GET API Error!");

					ssn1->SendResponseAsync(res1);//�̹� reponse JSON packet�� ����.
				};
				QueueFUNC(bg_run);
			}
			//assert(svr->_fncGET.get());//_fncGET �� �ݵ�� �ִ�.
		}
		else if((request.method() == "POST") || (request.method() == "PUT"))
		{
			int rv = 0;
			// srl ���� 
			string keyRes = ssn->getCacheKey("uuid");// uuid"�� ĳ�� ���� ����. ĳ�� ���Ϸ��� srl=1234�� ��� �Ѵ�.
			keyRes += request.body();
			
			string valueRes;
			CacheVal cval;
			rv = CheckCacheFirst(keyRes, cval, svr, ssn, request);
			if (rv == 1)	{
				valueRes.insert(0, cval._data.m_p, cval._data.m_len);
				res1.MakeGetResponse(valueRes, (PAS)cval._contentType);
				ssn->SendResponseAsync(res1);// MakeJsonResponse(res1, valueRes));
				return;
			}

			if(svr->_fncPOST.get())//�տ� const �� �־
			{
				ssn->_sinfo._stCached = "NoCach";

				auto sh_ss = svr->FindSession(ssn->id());
				auto sh_ss1(ssn->shared_from_this());// enable_shared_from_this<> ��� �޾Ƽ� ����. �̷��� �ص� �Ǵ��� Ȯ���� ����

				/// �Ʒ� ó�� ��ƾ�� �񵿱�� �ϸ� data�� ì�ܾ� �Ѵ�. �� �Լ� �ڿ��� �ٷ� request�� Clear �Ǳ� �����̴�.
				shared_ptr<KBinData> shbin = std::make_shared<KBinData>();
				shbin->Attach(req1.body().data(), req1.body().size());
				
				// rambda ���⼭ ���ʹ� ����Ͻ������� �Բ� response ���� �ٸ� �����忡�� �Ѵ�.
				auto bg_run = [svr, sh_ss, keyRes, shbin, &res1]() 
				{
					auto ssn1 = (TSESSION*)sh_ss.get();
					auto fnc = svr->_fncPOST.get();
					/// ///////////////////////////////////////////////////////////////
					int rv = (int)(*fnc)(ssn1, shbin, res1);// => CResponse1::ResponseForPost
					/// ///////////////////////////////////////////////////////////////
					/// //-1: error�޽��� �̹� ����. -2: No ���� �޽��� , 0:OK
					if(rv == 0)
					{//������ ��츸 ĳ�ÿ� �ִ´�.
						string value(res1.body());
						std::map<string, string> mapHd;
						GetResponseHeader(res1, mapHd);
						string cntype = mapHd["Content-Type"];
						svr->getCache().PutCacheValue(keyRes.c_str(), value.c_str(), value.size(), cntype.c_str()); // ������ ���� �ɽÿ� ����
						if(ssn1->_sinfo._status != 200)
							res1.SetBegin(ssn1->_sinfo._status);
					}
					else if(rv < 0) // + �̸� �̹� �����޽��� ��� �ִ�.
					{
						if(rv == -2)
							HttpCmn::MakeStrErrorToJsonResponse(res1, "Unknown Remote POST API Error!");
					}
					//((TSESSION*)sh_ss.get())
					ssn1->SendResponseAsync(res1);//�̹� reponse JSON packet�� ����.
				};
				QueueFUNC(bg_run);
			}
			assert(svr->_fncPOST.get());//_fncPOST �� �ݵ�� �ִ�.
		}
		else if(request.method() == "DELETE")
		{
			ssn->SendResponseAsync(res1.MakeGetResponse("{}", "application/json; charset=UTF-8"));
			return;

			string key(request.url());
			string value;

			// Decode the key value
			key = CppCommon::Encoding::URLDecode(key);
			CppCommon::StringUtils::ReplaceFirst(key, "/api/cache", "");
			CppCommon::StringUtils::ReplaceFirst(key, "?key=", "");
			CacheVal cval;
			// Delete the cache value
			if(svr->getCache().DeleteCacheValue(key.c_str(), cval))
			{
				// Response with the cache value
				value.insert(0, cval._data.m_p, cval._data.m_len);
				ssn->SendResponseAsync(res1.MakeGetResponse(value));
			}
			else
				ssn->SendResponseAsync(res1.MakeErrorResponse("Deleted cache value was not found for the key: " + key, 404));
		}
		else if(request.method() == "OPTIONS")
			ssn->SendResponseAsync(res1.MakeOptionsResponse());
		else if(request.method() == "TRACE")
			ssn->SendResponseAsync(res1.MakeTraceResponse(request.cache()));
		else
			ssn->SendResponseAsync(res1.MakeErrorResponse("Unsupported HTTP method: " + string(request.method())));
	}

};

