#pragma once

#include "HttpsSvr.h"
#include "HttpSvr.h"
#include "Response1.h"

class CmnDoc
{
public:

	CmnDoc();
	void InitApi();


	shared_ptr<CMyHttps> _bodySSL;
	shared_ptr<CMyHttp> _body;

	CResponse1* _svr{ nullptr };//CApiBase 
	KAtEnd _d_svr;

	CStringA _GUID;
	int      _port{ 0 };//0 �̾�� SampleData�� ���� �ȴ�.
	int      _CacheLife{ 3000 };
	BOOL     _bSSL{ TRUE };
	BOOL     _bStaticCache{ TRUE };
	CStringA _Title{ "(UNNAMED)" };
	CStringA _cachedPath{ "c:\\svr" };
	CStringA _cachedUrl{ "/" }; // ������ ���Ϸ� ���� ĳ�õǴ� ���� ��Ʈ
	CStringA _certificate;//server certificate pem file path
	CStringA _privatekey;//server private key pem file path
	CStringA _dhparam; //Diffie Hellman key pem file path
	CStringA _prvpwd; //private key password:�̰� ���� ���ؾ� �ϴµ�
	CStringA _ODBCDSN;
	CString _note{ L"sample HTTP server" };
	CStringA _defFile{ "index.html" };
	CStringA _rootLocal{ "c:\\svr" };
	CStringA _uploadLocal{ "c:\\svr\\upload" };
	CStringA _rootURL{ "/" }; // _rootURL �� �׻� '/' http://v2.petme.kr:19479/files   �̰��� ��Ʈ ���� ���� �Ѵٴ� �ǹ�. �� ���ð�� 'c:\\svr' == URL '/' �� ���� �ȴ�.
	CStringA _ApiURL{ "/api" }; //api call �Ҷ� �������� �տ� �ٴ� dir �̰� �ָ� ���� �Լ� http://v2.petme.kr:19479/api?key=xxx&func=call

	CString _SQL{ L"select * from t_uuid" };
	CString _SrcImagePath{ L"C:\\svr\\upload" };

	CStringA _UdpSvr;
	int _portUDP;



	//KUdpSvr _udp;
	int InitServerValue();


	void Serialize(CArchive& ar);








	CMyHttps* GetServer()
	{
		if(!_bodySSL)
		{
			//s_pBody = new CMyHttps();
			_bodySSL = shared_ptr<CMyHttps>(new CMyHttps());
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
			_body = shared_ptr<CMyHttp>(new CMyHttp());
		}
		return _body.get();
	}

	// 
	void StartServer()
	{
		//_svr->_api->_fileKey = FILE name;
	}

	void StopServer()
	{
		if(_bSSL)
		{
			CMyHttps* https = GetServer();
			https->stop_server();
		}
		else
		{
			CMyHttp* http = GetServerNoSSL();
			http->stop_server();
		}
	}
	void ShutdownServer()//?Shutdown 
	{
		if(_bSSL)
		{
			CMyHttps* https = GetServer();
			https->shutdown_server();
		}
		else
		{
			CMyHttp* http = GetServerNoSSL();
			http->shutdown_server();
		}
	}
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
			if(https)
				return https->_server->IsStarted();
		}
		else
		{
			if(!_body)
				return false;
			CMyHttp* http = GetServerNoSSL();
			if(http)
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
				_bodyWin = shared_ptr<CHttpSvr>(new CHttpSvr());
			}
			return _bodyWin.get();
		}
	*/
};

