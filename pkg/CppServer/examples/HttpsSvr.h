#pragma once




#include "server/kwadjust.h"

#include "asio_service.h"

#include "server/asio/ssl_session.h" // server override � �ϴ� �̰� �ʿ���
#include "server/http/https_server.h"

// CppCommon
#include "string/string_utils.h"
#include "utility/singleton.h"

#include <memory>
#include <functional>

#include <server\asio\ssl_server.h>
#include <server\asio\ssl_session.h>

#include "Cache.h"

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

// KwLib64/KTemple.h KwCreateFuncValue �� �����ϰ� ������ �ܺ� �ڵ�� KTemple.h�� include���� �ʱ� ���� �ִ´�.
template<typename TFNC, typename TRAMBD>
void TCreateFuncValue(shared_ptr<TFNC>& rtval, TRAMBD lambda, int bOvWr = 0)
{
/* ex
// ������ ���� ������ ���� �Ǿ��ٰ� ����
shared_ptr<function<TRAMBD>> _fncGET;

// ���� ���� : TFNC == function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)> �� ���̱� ����
shared_ptr<function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)>> _fncGET;

// �Ҵ�
template<typename TFNC> void AddCallbackOnGET(TFNC fnc, int bOvWR = 0)
{
	TCreateFuncValue(_fncGET, fnc, bOvWR);
	//_fncGET = shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>>(new function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>(fnc));
}*/
	//rtval = shared_ptr<TFNC>(new TFNC(lambda)); �Ʒ��� ����
	if(bOvWr || !rtval)
		rtval = std::make_shared<TFNC>(lambda);
}
class KSessionInfo
{
public:
	string _method;
	string _url;
	string _sparams;
	std::map<string, string> _urlparam;
	std::map<string, string> _headers;
	//+["Accept"]	"*/*"	
	//+["Accept-Encoding"]	"gzip, deflate"	
	//+["Accept-Language"]	"ko"	
	//+["Cache-Control"]	"no-cache"	
	//+["Connection"]	"Keep-Alive"	
	//+["Content-Length"]	"3921543"	
	//+["Content-Type"]	"image/jpeg"	
	//+["Host"]	"www.parent.co.kr:19480"	
	//+["UA-CPU"]	"AMD64"	
	//+["User-Agent"]	"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.2; Win64; x64; Trident/7.0; .NET4.0C; .NET4.0E; .NET CLR 2.0.50727; .NET CLR 3.0.30729; .NET CLR 3.5.30729)"	

	string _dir;
	string _stCached;//{"cached"/"NoCach"};
	void Clear()
	{
		_method.clear();
		_url.clear(); //empty();�� �˻��Լ�
		_sparams.clear();
		_urlparam.clear();
		_dir.clear();
		_stCached.clear();
	}
	void GatherBasicInfo(const HTTPRequest& request);//, KSessionInfo& sinf);//, string& url, string& sparams, string& dir, string& stCached, std::map<string, string>& urlparam, std::map<string, string>& headers);

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

public:
	KSessionInfo _sinfo;

	//void MakeJsonError(HTTPResponse& res1, char* errmsg, int err = -1);
	//void ResponseError(char* errmsg, int err = -1);

	/// <summary>
	/// url params���� �� ����� ���� �ٸ� 'notKey'�� ���� ĳ�ÿ� Ű�� �����.
	/// </summary>
	std::string getCacheKey(const char* notKey)
	{
		Tas ss;
		for(auto it: _sinfo._urlparam)
		{
			if(it.first != notKey)//"uuid")
				ss << it.first << it.second;
		}
		return ss.str();
	}

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
		ss->_fncOnReceived = _fncOnReceived;
		ss->_fncOnSent = _fncOnSent;
		ss->_fncOnSentKw = _fncOnSentKw;
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
	shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>> _fncGET;
	shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>> _fncPOST;
	shared_ptr<function<int(HTTPSCacheSession&)>>                              _fncOnConnected;
	shared_ptr<function<int(HTTPSCacheSession&)>>                              _fncOnHandshaked;
	shared_ptr<function<int(HTTPSCacheSession&)>>                              _fncOnDisconnected;
	shared_ptr<function<int(HTTPSCacheSession*, size_t, size_t)>>              _fncOnSent;
	shared_ptr<function<int(HTTPSCacheSession*, uint8_t*, size_t)>>            _fncOnSentKw;
	shared_ptr<function<int(HTTPSCacheSession*, const void*, size_t)>>         _fncOnReceived;

	shared_ptr<Cache> _cache;
	Cache& getCache() {
		//return Cache::GetInstance();
		if(_cache.get() == nullptr)
			_cache = shared_ptr<Cache>(new Cache());
		return *_cache.get();
	}
};


class CMyHttps
{
public:
	/*// ������ �����ϰ� �ϳ� ������ �̰� ����.
	// ������ ������ shared_ptr<CMyHttps> _bodySSL; ó�� ������ ������.
	static shared_ptr<CMyHttps> s_body;
	static CMyHttps* Instance()
	{
		if(s_body.get() == nullptr)
		{
			s_body = shared_ptr<CMyHttps>(new CMyHttps());
		}
		return s_body.get();
	}*/

	void SampleData(int port = 19480
		, const char* vpath = "/api"
					, const char* path = "C:/Dropbox/Proj/STUDY/boostEx/CppServer/CppServer-master/www/api");
	~CMyHttps();
	
	int _port;
	string _path;
	string _vpath;
	string _certificate;//server certificate pem file path
	string _privatekey;//server private key pem file path
	string _dhparam; //Diffie Hellman key pem file path
	string _prvpwd; //private key password

	shared_ptr<AsioService> _service;
	shared_ptr<HTTPSCacheServer> _server;

public:
	int main_https(int argc, char** argv);
	void restart_server();
	void stop_server();
	int start_server();
	void shutdown_server();
	
	template<typename TFNC> void AddCallbackOnGET(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncGET, fnc, bOvWR);
		//_server->_fncGET  = shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>>(new function<int(HTTPSCacheSession*,HTTPRequest&, HTTPResponse&)>(fnc));
	}
	template<typename TFNC> void AddCallbackOnPOST(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncPOST, fnc, bOvWR);
		//_server->_fncPOST  = TCreateFuncValue(fnc);
		//_server->_fncPOST  = shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>>(new function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>(fnc));
	}
	template<typename TFNC> void AddCallbackOnStarted(TFNC fnc, int bOvWR = 0)
	{
		//function<int()> f1 = function<int()>(fnc);
		//function<int()>* pf1 = new function<int()>(fnc);
		// _server->_fncOnStarted = fnc;//error C2679: ���� '=': ������ �ǿ����ڷ� 'TFNC' ������ ����ϴ� �����ڰ� ���ų� ���Ǵ� ��ȯ�� �����ϴ�.
		// shared_ptr�� �ٷ� ���ú����� �翬�� ���ִ´�.
		//auto pfnc = new function<void()>(fnc);
		//_server->_fncOnStarted = pfnc;//error C2679: ���� '=': ������ �ǿ����ڷ� 'function<void (void)> *' ������ ����ϴ� �����ڰ� ���ų� ���Ǵ� ��ȯ�� �����ϴ�.
		// shared_ptr �����͸� ���� attach���Ѵ�.

		///_server->_fncOnStarted = Clone_FncNone(fnc);/// shared_ptr�� ���Ŀ� struct�� struct�� ���� ����� construct�� �ҷ�����.
		//_server->_fncOnStarted = shared_ptr<function<void()>>(pfnc);
		/// �̰� �������� ����.
		TCreateFuncValue(_server->_fncOnStarted, fnc, bOvWR);
		//	_server->_fncOnStarted = shared_ptr<function<void()>>(new function<void()>(fnc));
		/// >> ���ΰ� �ΰ� �پ ������.
	}
	template<typename TFNC> void AddCallbackOnStopped(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnStopped, fnc, bOvWR);
		//_server->_fncOnStopped = shared_ptr<function<void()>>(new function<void()>(fnc));///lambda���������� '->void'�� ��������
		//_server->_fncOnStopped = shared_ptr<function<()>>(new function<()>(fnc)); '<void()>'���� void�� ���� �ϸ� ����
	}
	template<typename TFNC> void AddCallbackOnError(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnError, fnc, bOvWR);
		//_server->_fncOnError = shared_ptr<function<int(int, const string&, const string&)>>(new function<int(int, const string&, const string&)>(fnc));
	}
	template<typename TFNC> void AddCallbackOnConnected(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnConnected, fnc, bOvWR);
		//_server->_fncOnConnected = shared_ptr<function<int(HTTPSCacheSession&)>>(new function<int(HTTPSCacheSession&)>(fnc));
	}
	template<typename TFNC> void AddCallbackOnHandshaked(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnHandshaked, fnc, bOvWR);
		//_server->_fncOnHandshaked = shared_ptr<function<int(HTTPSCacheSession&)>>(new function<int(HTTPSCacheSession&)>(fnc));
	}
	template<typename TFNC> void AddCallbackOnDisconnected(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnDisconnected, fnc, bOvWR);
		//_server->_fncOnDisconnected = shared_ptr<function<int(HTTPSCacheSession&)>>(new function<int(HTTPSCacheSession&)>(fnc));
	}
	template<typename TFNC> void AddCallbackOnReceived(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnReceived, fnc, bOvWR);
		//_server->_fncOnReceived = shared_ptr<function<int(HTTPSCacheSession*,const void*,size_t)>>(new function<int(HTTPSCacheSession*, const void*,size_t)>(fnc));
	}
	template<typename TFNC> void AddCallbackOnSent(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnSent, fnc, bOvWR);
		//_server->_fncOnSent = shared_ptr<function<int(HTTPSCacheSession*,size_t, size_t)>>(new function<int(HTTPSCacheSession*,size_t, size_t)>(fnc));
	}
	template<typename TFNC> void AddCallbackOnSentKw(TFNC fnc, int bOvWR = 0)
	{
		TCreateFuncValue(_server->_fncOnSentKw, fnc, bOvWR);
		//_server->_fncOnSentKw = shared_ptr<function<int(HTTPSCacheSession*, uint8_t*, size_t)>>(new function<int(HTTPSCacheSession*, uint8_t*, size_t)>(fnc));
	}

};

class HttpCmn
{
public:

	//static void MakeJsonError(HTTPResponse& res1, char* errmsg, int err);

	static void GatherBasicInfo(const HTTPRequest& request, KSessionInfo& sinf);//, string& url, string& sparams, string& dir, string& stCached, std::map<string, string>& urlparam, std::map<string, string>& headers);


	//void MakeJsonResponse(HTTPResponse& res, CJsonPbj& jres)
	//{
	//	string jresU8 = jres.ToJsonStringUtf8();//���⼭ �̹��� �۾��� ������ ���� �ϹǷ� utf8�� �ٲ۴�.
	//	MakeJsonResponse(res, jresU8);
	//}
	static HTTPResponse& MakeJsonResponse(HTTPResponse& res, std::string_view js)
	{
		res.MakeGetResponse(js, "application/json; charset=UTF-8");// "text/plain" default
		return res;
	}
	static HTTPResponse& MakeJErrorResponse(HTTPResponse& res, std::string_view js, int err = -1)
	{
		string jerror = JError(js.data(), err);
		std::wstring wjerror;
		wjerror.assign(jerror.begin(), jerror.end());
		// error�� ������ ��κ������� �ѱ��� ������ �𸣴�, UTF8�� ������ ������.
		string utf8 = CppCommon::Encoding::ToUTF8(wjerror);
		return MakeJsonResponse(res, utf8);
	}
	static string JError(const char* msg, int rv = -1)
	{
		// S_FALSE = 1  FAILED = -1  S_OK = 0
		std::stringstream ss;
		ss << "{\"error\":\"" << msg << "\", \"return\":" << rv << "}";
		return ss.str();
	}



	template<typename TSVC, typename TSVR>
	static void restart_server(TSVC service, TSVR server)
	{
		if(service.get() == nullptr)
			return;
		if(server.get() == nullptr)
			return;
		std_cout << "Server restarting...";
		server->Restart();
		std_cout << "Done!" << std_endl;
	}

	template<typename TSVC, typename TSVR>
	static void shutdown_server(TSVC service, TSVR server)
	{
		if(service.get() == nullptr)
			return;
		// Stop the server
		std_cout << "Server stopping...";
		if(server.get() == nullptr)
			return;
		if (!server->IsStarted())
		{
			std_cout << "TCP server is not started!" << std_endl;
			return;
		}

		server->Stop();
		std_cout << "Done!" << std_endl;

		// Stop the Asio service
		std_cout << "Asio service stopping...";
		if(!service->IsStarted())
			service->Stop();
		std_cout << "Done!" << std_endl;

		server.reset();
		service.reset();
	}

	template<typename TSVC, typename TSVR>
	static void stop_server(TSVC service, TSVR server)
	{
		if(service.get() == nullptr)
			return;
		if(server.get() == nullptr)
			return;
		if(!server->IsStarted())
			return;
		// Stop the server
		std_cout << "Server stopping...";
		server->Stop();
		std_cout << "Done!" << std_endl;

		server.reset();
	}


//	static void onReceivedRequest(HTTPSCacheSession* session, const HTTPRequest& request);

	//HTTPSCacheSession or HTTPCacheSession
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
		Host: www.parent.co.kr:19480
		Content-Length: 71
		Connection: Keep-Alive
		Cache-Control: no-cache

		{"func":"SampleSelect", "params": {"table":"tapppolicy", "limit":"20"}}
		*/
		HTTPResponse& res1 = ssn->response();
		HTTPRequest& req1 = (HTTPRequest&)request; // this->request(); kdw�� �߰�

		/// �⺻ ������ ����. _url, _dir, _sparams, _urlparam
		ssn->_sinfo.GatherBasicInfo(request);//_url, _sparams, _dir, _stCached, _urlparam, _headers);

		// Process HTTP request methods
		if(request.method() == "HEAD")
		{
			ssn->SendResponseAsync(res1.MakeHeadResponse());
		}
		else if(request.method() == "GET")
		{
			// �̰� ��� �Ǵµ� �� ���ǿ��� �� http�� �ٳ�?
			int rv = -1;
			string keyRes = ssn->getCacheKey("uuid");//_urlparam�� �̹� ��� �־�� ����. _url
			string value, valueRes;
			if (ssn->_sinfo._url.empty())
			{
				HttpCmn::MakeJErrorResponse(res1, "URL is empth!");
				ssn->SendResponseAsync(res1);// MakeJsonResponse(res1, JError("URL is empth.")));
				return;
			}
			if (keyRes.size() > 0)
			{
				if (ssn->_sinfo._urlparam["noCache"] == "true")
					svr->getCache().DeleteCacheValue(keyRes, valueRes);
				else if (svr->getCache().GetCacheValue(keyRes, valueRes))
				{/// ĳ�ð� ������ ���� ������ �ʴ´�. ��� ó�� ����? ���ο��� ������ ��� �о� �鿩 ��ü ĳ�ÿ��� ���� �Ѵ�.
					ssn->_sinfo._stCached = "cached";// onSent ���� �α� ���� �ϱ� ���� �־� �д�.
					ssn->SendResponseAsync(MakeJsonResponse(res1, valueRes));
					return;
				}
			}

			if(svr->_fncGET.get())
			{
				ssn->_sinfo._stCached = "NoCach";

				rv = (int)(*svr->_fncGET.get())(ssn, req1, res1);
				
				if(rv == 0)	{//������ ��츸 ĳ�ÿ� �ִ´�.
					string value(res1.body());
					svr->getCache().PutCacheValue(keyRes, value);
				}
				else if (rv < 0) // + �̸� �̹� �����޽��� ��� �ִ�.
					HttpCmn::MakeJErrorResponse(res1, "Unknown Remote GET API Error!");
				ssn->SendResponseAsync(res1);//�̹� reponse JSON packet�� ����.
			}
			assert(svr->_fncGET.get());//_fncGET �� �ݵ�� �ִ�.
		}
		else if((request.method() == "POST") || (request.method() == "PUT"))
		{
			int rv = 0;
			string keyRes = ssn->getCacheKey("uuid");//_urlparam�� uuid�� ������ Ű. _url > map
			keyRes += request.body();
			string valueRes;
			// �ٷ����� ���浥���͸� ĳ�� ���� DB�и��� �о�� �� �ʿ䰡 �������� ĳ�� Ű �����ϰ� DB������ �����Ѵ�.
			if (keyRes.size() > 0)
			{
				if (ssn->_sinfo._urlparam["noCache"] == "true")
					svr->getCache().DeleteCacheValue(keyRes, valueRes);
				else if (svr->getCache().GetCacheValue(keyRes, valueRes))
				{
					ssn->_sinfo._stCached = "cached";
					ssn->SendResponseAsync(MakeJsonResponse(res1, valueRes));
					return;
				}
			}
			/// ////////////////////////////////////////////////////////
			if(svr->_fncPOST.get())//�տ� const �� �־
			{
				ssn->_sinfo._stCached = "NoCach";

				rv = (int)(*svr->_fncPOST.get())((TSESSION*)ssn, req1, res1);
				
				if(rv == 0)	{//������ ��츸 ĳ�ÿ� �ִ´�.
					string value(res1.body());
					svr->getCache().PutCacheValue(keyRes, value); // ������ ���� �ɽÿ� ����
				}
				else if(rv < 0) // + �̸� �̹� �����޽��� ��� �ִ�.
					HttpCmn::MakeJErrorResponse(res1, "Unknown Remote POST API Error!");
	
				ssn->SendResponseAsync(res1);//�̹� reponse JSON packet�� ����.
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

			// Delete the cache value
			if(svr->getCache().DeleteCacheValue(key, value))
			{
				// Response with the cache value
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

