#pragma once

#include "HttpsSvr.h"
#include "HttpSvr.h"
#include "Response1.h"

class CmnDoc
{
public:

	CmnDoc();
	void InitApi();

protected:
	///이건 GetServer를 불러서만 쓸수 있게 한다.
	SHP<CMyHttps> _bodySSL;
	SHP<CMyHttp> _body;

public:
	CResponse1* _svr{ nullptr };//CApiBase 
	KAtEnd _d_svr;

	CStringA _GUID;
	int      _port{ 0 };//0 이어야 SampleData가 적용 된다.
	int      _CacheLife{ 3000 };
	BOOL     _bSSL{ FALSE };
	BOOL     _bStaticCache{ TRUE };
	CString _Title{ "(UNNAMED)" };
	CStringA _cachedPath{ "c:\\svr" };
	CStringA _cachedUrl{ "/" }; // 고정된 파일로 완전 캐시되는 곳의 루트
	CStringA _certificate;//server certificate pem file path
	CStringA _privatekey;//server private key pem file path
	CStringA _dhparam; //Diffie Hellman key pem file path
	CStringA _prvpwd; //private key password:이건 저장 안해야 하는데
	CString _ODBCDSN;
	CString _note{ L"sample HTTP server" };
	CStringA _defFile{ "index.html" };
	CStringA _rootLocal{ "c:\\svr" };
	CStringA _uploadLocal{ "c:\\svr\\upload" };
	CStringA _rootURL{ "/" }; // _rootURL 는 항상 '/' http://v2.petme.kr:19479/files   이것은 루트 부터 지원 한다는 의미. 위 로컬경로 'c:\\svr' == URL '/' 와 같게 된다.
	CStringA _ApiURL{ "/api" }; //api call 할때 가상으로 앞에 붙는 dir 이거 주면 원격 함수 http://v2.petme.kr:19479/api?key=xxx&func=call

	CString _SQL{ L"select * from tuser" };
	CString _SrcImagePath{ L"C:\\svr\\upload" };

	BOOL     _bDbConnected{FALSE};//DB는 disconnect가 없다. 서버Stop하면 ~CDatabase에서 자연 스럽게 없애지
	BOOL _bRecover{FALSE};// 저장은 안되고, 읽어 들일때 복구로 읽혔나(TRUE), 사람이 열었나(FALSE)
	CStringA _tLastRunning;// 5분마다 체크 한다. "2021-06-27 12:16:12" 
	CString _fullPath;//이파일

// 	CStringA _UdpSvr;
// 	int _portUDP;

	/// 이부분은 구현 하려다 수정 부분이 너무 많아 json으로 변경 하는 함수를 지원한다. JsonToData
// 	ShJObj _json{std::make_shared<JObj>()};
// 	void InitDoc()
// 	{
// 		auto& jobj = *_json;
// 		//jobj("DSN") = L"Winpache";
// 	}


	//KUdpSvr _udp;
	int InitServerValue();


	virtual void JsonToData(ShJObj& sjobj, bool bToJson);


	void Serialize(CArchive& ar);





	void AddCallbackOnTrace(SHP<function<void(string)>> srfnc)
	{
		if(_bSSL)
			GetServerNoSSL()->AddCallbackOnTrace(srfnc);//?ExTrace 3 CmnView -> MyHttps + std_cout(KTrace)
		else
			GetServer()->AddCallbackOnTrace(srfnc);//?ExTrace 3 CmnView -> MyHttps + std_cout(KTrace)

	}


	CMyHttps* GetServer()
	{
		if(!_bodySSL)
		{
			//s_pBody = new CMyHttps();
			_bodySSL = SHP<CMyHttps>(new CMyHttps());
#ifdef _DEBUG
			_bodySSL->SampleData();//InitServer()최초 여기 한번 불려 진다.
#endif
		}
		return _bodySSL.get();
	}
	CMyHttp* GetServerNoSSL()
	{
		if(!_body)
		{
			//s_pBody = new CMyHttps();
			_body = SHP<CMyHttp>(new CMyHttp());
		}
		return _body.get();
	}

	// 
	void StartServer();

	void StopServer();
	void ShutdownServer();
	void RestartServer()
	{
		if(_bSSL)
		{
			CMyHttps* https = GetServer();
			https->restart_server();
		}
		else
		{
			CMyHttp* http = GetServerNoSSL();
			http->restart_server();
		}
	}
	
	bool IsStarted()
	{
		if(_bSSL)
		{
			if(!_bodySSL)
				return false;
			CMyHttps* https = GetServer();
			if(https && https->_server)
				return https->_server->IsStarted();
		}
		else
		{
			if(!_body)
				return false;
			CMyHttp* http = GetServerNoSSL();
			if(http && http->_server)
				return http->_server->IsStarted();
		}
		return false;
	}
	int FreeDllLibrary();
	/*
		CHttpSvr* GetServerWin()
		{
			if(_bodyWin.get() == nullptr)
			{
				//s_pBody = new CMyHttps();
				_bodyWin = SHP<CHttpSvr>(new CHttpSvr());
			}
			return _bodyWin.get();
		}
	*/
};

