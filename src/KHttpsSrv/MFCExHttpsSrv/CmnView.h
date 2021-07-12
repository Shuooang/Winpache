#pragma once


#include "KwLib64/DlgTool.h"
#include "KwLib64/HTTPS/HttpSvr.h"
#include "KwLib64/UdpSvr.h"
#include "KwLib64/stdecl.h"

#include "HttpsSvr.h"
#include "HttpSvr.h"
#include "CmnDoc.h"
//#include "MFCExHttpsSrv.h"



class CmnView
{
public:
	CmnView(CFormInvokable* view)
		: _vu(view)
	{
	}
	CmnDoc* GetDocument() const;

	void DoDataExchange(CDataExchange* pDX);

	CFormInvokable* _vu;
	int _id{ 0 };

	virtual int getId() { return _id; }

	//CmnView
	virtual _STitleWidthField* GetArListConf(int* nCols) = NULL;

	//OnDestroy 에서 CreateEvent 하고, OnStop에서 SetEvent
	HANDLE _hEventShutdown{nullptr};// = CreateEvent(NULL, TRUE, FALSE, L"EndView");
	HANDLE _hEventAsioError{nullptr};// = CreateEvent(NULL, TRUE, FALSE, L"EndView");


// 	CEditCtrlCV c_cachedPath;
// 	CEditCtrlCV c_cachedUrl;
	CEditCtrlCV c_port;
	CEditCtrlCV c_CacheLife;
	CEditCtrlCV c_path;
	CEditCtrlCV c_vpath;
	CEditCtrlCV c_ODBCDSN;
	CEditCtrlCV c_rootLocal;
	CEditCtrlCV c_defFile;
	CEditCtrlCV c_uploadLocal;
	CEditCtrlCV c_rootURL;
	CEditCtrlCV c_ApiURL;
	CEditCtrlCV c_Title;
	CEditCtrlCV c_note;


	void StartServer();

	void StopServer();

	void RestartServer();

	void Shutdown(PAS from = NULL);

	template<typename TMyserver, typename TSession>
	int StartServerHttpT(TMyserver* https, TSession* pss);//2020-12-08 14:00:38

	virtual void MonitorRequest(SHP<KArray<string>> shar) = NULL;

	int InitServer();
	void SampleServer();
	void MakeJsonResponse(HTTPResponse& res, JObj& jres);

	void SelectFolder(CStringA& target);


#ifdef xxxxxxxxxxxxxx
	SHP<function<void(string)>> _fncExtraTrace;

	template<typename TFNC> void AddCallbackExtraTrace(TFNC fnc)
	{
		_fncExtraTrace = std::make_shared<function<void(string)>>(fnc);//?ExTrace 2 CmnView에 넣어둔다.
	}
#endif // xxxxxxxxxxxxxx

	// 메뉴나 버튼 enable stat
	KStdMap<int, int> _mapUpt;
	void EnableCommand(int idc, int bEnable, int iOp = 0);
	int GetUpdate(int idc)
	{
		int bEnable = 0;
		if(_mapUpt.Lookup(idc, bEnable))
			return bEnable;
		return FALSE;
	}
	virtual void EnableCtrl(int idc, int bEnable);

	virtual void UpdateControl(CStringA stat, int iOp = 0);

	//void CallbackOnGET(TSession* ss, SHP<KBinData> shbin, HTTPResponse& res);
	virtual void CallbackOnStarted(int vuid);
	virtual int  CallbackOnStopped(HANDLE hev, int vuid);
	virtual int CallbackOnSent(KSessionInfo& inf, int vuid, size_t sent, size_t pending);
	virtual int CallbackOnReceived(const void* buffer, size_t size) { return 0;  }
	
	virtual int CallbackOnReceivedRequest(KSessionInfo& inf, int vuid, SHP<KBinData> shbin, HTTPResponse& res)
	{
		return 0;
	}
	int CallbackOnGET(KSessionInfo& inf, int vuid, SHP<KBinData> shbin, HTTPResponse& res);
	int CallbackOnPOST(KSessionInfo& inf, int vuid, SHP<KBinData> shbin, HTTPResponse& res);
	int CallbackOnErrorAsio(int e, string c, string m);
	void StoptAndStart(string step);
	/// restart 가 아니고 차례대로 Stop하고 Start 하는 것.
	string _stepStopStart;
};



template<typename TMyserver, typename TSession>
int CmnView::StartServerHttpT(TMyserver* https, TSession* pss)
{
	BACKGROUND(1);
	/// 1. start_server 전에 InitServer 해 줘야 server가 staret한다.
	int rv = InitServer();
	if(rv != 0)
		return rv;

	CmnDoc* doc = GetDocument();
	/// 2. start_server 하여 내부의 server를 생성 한다.
	try
	{
		// moved to SrvView
// 		if(_fncExtraTrace)
// 			https->AddCallbackOnTrace(_fncExtraTrace);//?ExTrace 3 CmnView -> MyHttps + std_cout(KTrace)

		/// /////////////////////////////////
		rv = https->start_server();// 캐시폴더 지정시 안에 파일이 Dropbox smart동기화가 안되어 있으면 crush
		/// /////////////////////////////////
		if(rv != 0)
		{
			AfxMessageBox(L"Server Start Error! Make sure this computer is HTTPS certified.");
			return rv;
		}
		ASSERT(https->_server);
		https->_server->getCache()._maxFresh = doc->_CacheLife;//순서 _server ?
	}
	catch(std::exception&)
	{
		rv = -1;
		TRACE("std::exception occured when https->start_server()\n");
	}
	catch(CException* e)
	{
		rv = -2;
		auto buf = new TCHAR(1024);		KAtEnd d_buf([&]() { delete buf; });
		e->GetErrorMessage(buf, 1000);
		AfxMessageBox(buf, MB_OK | MB_ICONEXCLAMATION);
	}
	catch(...)
	{
		rv = -3;
		TRACE("catch(...) occured when https->start_server()\n");
		//auto buf = new TCHAR(1024);	KAtEnd d_buf([&]() { delete buf; });
	}


	if(rv != 0)
		return rv;

	auto app = AfxGetApp();
	auto ivc0 = dynamic_cast<KCheckWnd*>(app);
	int vuid = getId();// this 나 windows는 사라질수 있으니 반드시 [&,...]에 전달 해야 한다.

	// 이걸 OnStarted에서 하지 않고 멸령 내리자 마자 해야 중복 Start않지
	/// 3. start_server후에 불러야 만들어진 서버에 다음에 오는 람다식을 저장해 둘수 있다.
	//auto server = https->_ HTTPSCacheSession
	https->AddCallbackOnReceivedRequest([&, vuid](TSession* ss, SHP<KBinData> shbin, HTTPResponse& res) -> int
		{
			KSessionInfo& inf = ss->_sinfo;
			return CallbackOnReceivedRequest(inf, vuid, shbin, res);
		});
	https->AddCallbackOnGET([&, vuid](TSession* ss, SHP<KBinData> shbin, HTTPResponse& res) -> int
		{
			//string ssid = ss->id().string();
			KSessionInfo& inf = ss->_sinfo;
			return CallbackOnGET(inf, vuid, shbin, res);
		});
	https->AddCallbackOnPOST([&, vuid](TSession* ss, SHP<KBinData> shbin, HTTPResponse& res) -> int
		{
			KSessionInfo& inf = ss->_sinfo;
			return CallbackOnPOST(inf, vuid, shbin, res);
		});

	https->AddCallbackOnStarted([&, vuid]() -> void
		{
			CallbackOnStarted(vuid);
		});

	// 계속 죽으니 BG에서 Stopped된 시점을 알수 없어서 그냥 안끊고 나간다. ~ 에서 다할거라 믿고
	if(!_hEventShutdown)
		_hEventShutdown = ::CreateEvent(NULL, TRUE, FALSE, L"ShutdownServer");//?destroy 1 여기서 해줘야 람다에 전달 가능
	else
		::ResetEvent(_hEventShutdown);

	auto hev = _hEventShutdown;//?중요: 반드시 필요한 값을 람다로 복사해야 하는 경우 명심.
	https->AddCallbackOnStopped([&, hev, vuid]() -> int //?Shutdown 
		{// void 인경우 '-> void'를 생략해도 된다.
			return CallbackOnStopped(hev, vuid);
		});
	
	auto cbErr = [&, vuid](int e, const string& c, const string& m) -> int
	{
		//TRACE("AddCallbackOnError: %d, %s, %s\n", e, c, m);
		auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
		bool bVu = !ivc ? false : ivc->ViewFind(vuid);
		if(bVu)
			UpdateControl("error");
		return 0;
	};
	https->AddCallbackOnError(cbErr);

	// Asio가 오류나면 다시 살아 나지 못한다. 그래서 죽였다가 살려야.
	https->_service->AddCallbackOnErrorAsio([&, vuid](int e, const string& c, const string& m) -> int
		{
			//TRACE("AddCallbackOnError: %d, %s, %s\n", e, c, m);
			// 2021-06-30 13:08:22 	   잘못된 파일 핸들입니다
/*
2021-06-30 13:08:22 	Asio service error code: 10009 error category: 'system' 잘못된 파일 핸들입니다
2021-06-30 13:08:22 	Asio service error code: 10008 error category: 'system' 각 소켓 주소(프로토콜/네트워크 주소/포트)는 하나만 사용할 수 있습니다
2021-06-30 13:08:05 	HTTPCacheServer::onConnected 
2021-06-30 12:35:30 	HTTPCacheServer::onHandshaked 
;
;
2021-06-30 09:32:23 	HTTPCacheServer::onHandshaked 
2021-06-30 09:18:43 	HTTPCacheServer::onHandshaked 
2021-06-30 09:17:02 	HTTPCacheServer::onConnected 
2021-06-30 09:16:46 	HTTPCacheServer::onConnected 
2021-06-30 09:16:08 	HTTPCacheServer::onConnected 
2021-06-30 09:15:54 	HTTPCacheServer::onStarted 
2021-06-30 09:15:54 	Server starting...
2021-06-30 09:15:51 	Asio service starting...Done!
2021-06-30 09:15:32 	Main Dababase is connected! (DSN: Winpache).
*/
			auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
			bool bVu = !ivc ? false : ivc->ViewFind(vuid);
			if(bVu)
			{
				UpdateControl("error");
				CallbackOnErrorAsio(e, c, m);///Asio system error나면 StopAndStart한다.
			}
			return 0;
		});
	
#ifdef _Dont
	https->AddCallbackOnConnected([&](TSession& session) -> int
		{
			//TRACE("AddCallbackOnConnected\n");
			return 0;
		});
	https->AddCallbackOnHandshaked([&](HTTPSCacheSession& session) -> int {
		TRACE("AddCallbackOnHandshaked\n");
		return 0;
		});

#endif // _Dont
	/// 이것도 용량이 많으면, 여러번 불릴거 같은데.
	https->AddCallbackOnReceived([&](TSession* ss, const void* buffer, size_t size) -> int
		{
			CallbackOnReceived(buffer, size);
			//TRACE("AddCallbackOnReceived(size:%ld)\n", size);
			return 0;
		});
	
	https->AddCallbackOnSent([&, vuid](TSession* ss, size_t sent, size_t pending) -> int
		{
			KSessionInfo& inf = ss->_sinfo;
			return CallbackOnSent(inf, vuid, sent, pending);
		});
	/*
//SSLSession HTTPSCacheSession* ss SSLSession* ss
	https->AddCallbackOnReceivedRequestInternal([&](TSession* ss, HTTPRequest& request) -> int
		{
			BACKGROUND(1);
			//TRACE("AddCallbackOnReceived(size:%ld)\n", size);
			KSessionInfo& inf = ss->_sinfo;
			SHP<KArray<string>> shar = make_shared<KArray<string>>();
			CStringA now(KwGetCurrentTimeFullString());
			//string url0(request.url());//key	"/apix?key=xxx&srl=5111"	url0	"/images/xxxxx.jpeg"
			string url1 = CppCommon::Encoding::URLDecode(request.url());
			//string_view sv;			sv.data()
			shar->Add("I");
			shar->Add((PAS)now);
			shar->Add(string(request.method()));
			shar->Add(url1);
			shar->Add("internal");
			KwBeginInvoke(this,
				([&, shar]()-> void
					{
						this->MonitorRequest(shar);
					}));
			return 0;
		});*/

		/*https->AddCallbackOnSentKw([&](TSession* ss, uint8_t* data, size_t szAll) -> int
			{
				BACKGROUND(1);
				string ssid = ss->id().string();
							auto svr = ss->server();
				HTTPResponse& res1 = ss->response();
				HTTPRequest& req1 = ss->request();
				KSessionInfo& inf = ss->_sinfo;

				CStringA now(KwGetCurrentTimeFullString());
				PAS pas = (char*)data;
				//모든 http패킷은 위에 text header영역이 있다. 비록 바이너리 일지라도. 그 구분이 "\r\n\r\n"이다.
				auto ie = tchstrx(pas, "\r\n\r\n");
				if(ie < 0)
					return -1;

				char* buf = new char[ie + 1];//http헤더 영역만 복사.
				KAtEnd _buf([&]() { delete buf; });//끝날때 실행: 메모리 해제.
				tchncpy(buf, pas, ie);
				buf[ie] = '\0';//두줄개행 구분 자리로 문자열 끊기 위해 \0을 넣는다.

				KStrMap kv;
				KwKeyValueStrToMap(buf, "\r\n", ": ", kv);

				auto app = (CMFCAppServerEx2*)AfxGetApp();
				CStringA sa;
				SHP<KArray<string>> shar = make_shared<KArray<string>>();
				//std::string_view mthd = req1.method(); 이거 비어 있네

				shar->Add("E");
				shar->Add((PAS)now);
				shar->Add(inf._method);
				shar->Add(inf._url);
				shar->Add(inf._stCached);
				shar->Add(kv.Get("Content-Type"));
				shar->Add(kv.Get("Content-Length"));
				if(inf._method.length() == 0)
					_break;
	#ifdef _DEBUGx
				// html 문서를 열람할때 내부적으로 불리는 이미지파일 같은 것을 접근 할때는
				// request의 method, url, header 값이 부실하다.
				if( inf._method.length() == 0 ||
					inf._url.length() == 0 ||
					inf._stCached.length() == 0)
				{
					auto mt = req1.method();
					auto ur = req1.url();
					auto szHd = req1.headers();
					KStrMap hdrs;
					for(int i = 0; i < szHd; i++)
					{
						auto hd = req1.header(i);
						hdrs[(string)std::get<0>(hd)] = std::get<1>(hd);
					}
					auto v1 = res1.body();
					auto szHd2 = res1.headers();
					KStrMap hdrs2;
					for(int i = 0; i < szHd2; i++)
					{
						auto hd = res1.header(i);
						hdrs2[(string)std::get<0>(hd)] = std::get<1>(hd);
						//+["Content-Length"]	"6346"
						//+["Content-Type"]	"text/html"
					}
					auto v3 = res1.protocol(); // "HTTP/1.1"
					auto v4 = res1.cache();
					_break;
				}
				sa.Format("%s %s %s >> %s %s : %s", now,
					inf._method.c_str(),
					inf._url.c_str(),
					inf._stCached.c_str(),
					kv["Content-Type"].c_str(),
					kv["Content-Length"].c_str());
	#endif // _DEBUG
			// UI작업 main thread에 Despatch한다.
			KwBeginInvoke(this,
				([&, sa, shar]()-> void {
					this->MonitorRequest(shar);
					//this->MonitorRequest(sa);
				 }));
			//HTTP/1.1 200 OK
			//Content-Type: image/jpeg
			//Cache-Control: max-age=3600
			//Content-Length: 155856
			//TRACE(L"AddCallbackOnSent(sent:%ld, pending:%ld) %s\n", sent, pending, now);
			return 0;//(sent:107, pending:0) 2020-10-11 07:35:56
			});*/

	https->AddCallbackOnDisconnected([&, vuid](TSession& session) -> int
		{
#ifdef _DEBUG
			auto ivc = dynamic_cast<KCheckWnd*>(AfxGetApp());
			bool bVu = !ivc ? false : ivc->ViewFind(vuid);
			{
				CString now = KwGetCurrentTimeFullString();
				TRACE(L"AddCallbackOnDisconnected %s\n", now);
			}
#endif // _DEBUG
			return 0;//2020-10-11 07:37:46 :보낸후 110초 후 끝어 진다.
		});

	return rv;
}
