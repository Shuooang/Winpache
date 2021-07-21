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
	///�̰� GetServer�� �ҷ����� ���� �ְ� �Ѵ�.
	SHP<CMyHttps> _bodySSL;
	SHP<CMyHttp> _body;

public:
	CResponse1* _svr{ nullptr };//CApiBase 
	KAtEnd _d_svr;

	CStringA _GUID;
	int      _port{ 0 };//0 �̾�� SampleData�� ���� �ȴ�.
	int      _CacheLife{ 3000 };
	BOOL     _bSSL{ FALSE };
	BOOL     _bStaticCache{ TRUE };
	CString _Title{ "(UNNAMED)" };
	CStringA _cachedPath{ "c:\\svr" };
	CStringA _cachedUrl{ "/" }; // ������ ���Ϸ� ���� ĳ�õǴ� ���� ��Ʈ
	CStringA _certificate;//server certificate pem file path
	CStringA _privatekey;//server private key pem file path
	CStringA _dhparam; //Diffie Hellman key pem file path
	CStringA _prvpwd; //private key password:�̰� ���� ���ؾ� �ϴµ�
	CString _ODBCDSN;
	CString _note{ L"sample HTTP server" };
	CStringA _defFile{ "index.html" };
	CStringA _rootLocal{ "c:\\svr" };
	CStringA _uploadLocal{ "c:\\svr\\upload" };
	CStringA _rootURL{ "/" }; // _rootURL �� �׻� '/' http://v2.petme.kr:19479/files   �̰��� ��Ʈ ���� ���� �Ѵٴ� �ǹ�. �� ���ð�� 'c:\\svr' == URL '/' �� ���� �ȴ�.
	CStringA _ApiURL{ "/api" }; //api call �Ҷ� �������� �տ� �ٴ� dir �̰� �ָ� ���� �Լ� http://v2.petme.kr:19479/api?key=xxx&func=call

	CString _SQL{ L"select * from tuser" };
	CString _SrcImagePath{ L"C:\\svr\\upload" };

	BOOL     _bDbConnected{FALSE};//DB�� disconnect�� ����. ����Stop�ϸ� ~CDatabase���� �ڿ� ������ ������
	BOOL _bRecover{FALSE};// ������ �ȵǰ�, �о� ���϶� ������ ������(TRUE), ����� ������(FALSE)
	CStringA _tLastRunning;// 5�и��� üũ �Ѵ�. "2021-06-27 12:16:12" 
	CString _fullPath;//������

// 	CStringA _UdpSvr;
// 	int _portUDP;

	/// �̺κ��� ���� �Ϸ��� ���� �κ��� �ʹ� ���� json���� ���� �ϴ� �Լ��� �����Ѵ�. JsonToData
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
			_bodySSL->SampleData();//InitServer()���� ���� �ѹ� �ҷ� ����.
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

