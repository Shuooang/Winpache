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

	//CStringA _GUID;
	//int      _port{ 0 };//0 �̾�� SampleData�� ���� �ȴ�.
// 	int      _CacheLife{ 3000 };
// 	BOOL     _bSSL{ FALSE };
// 	BOOL     _bStaticCache{ TRUE };
// 	CString _Title{ "(UNNAMED)" };
// 	CStringA _cachedPath{ "c:\\svr" };
// 	CStringA _cachedUrl{ "/" }; // ������ ���Ϸ� ���� ĳ�õǴ� ���� ��Ʈ
// 	CStringA _certificate;//server certificate pem file path
// 	CStringA _privatekey;//server private key pem file path
// 	CStringA _dhparam; //Diffie Hellman key pem file path
// 	CStringA _prvpwd; //private key password:�̰� ���� ���ؾ� �ϴµ�
// 	CString _ODBCDSN;
// 	CString _note{ L"sample HTTP server" };
// 	CStringA _defFile{ "index.html" };
// 	CStringA _rootLocal{ "c:\\svr" };
// 	CStringA _uploadLocal{ "c:\\svr\\upload" };
// 	CStringA _rootURL{ "/" }; // _rootURL �� �׻� '/' http://v2.petme.kr:19479/files   �̰��� ��Ʈ ���� ���� �Ѵٴ� �ǹ�. �� ���ð�� 'c:\\svr' == URL '/' �� ���� �ȴ�.
// 	CStringA _ApiURL{ "/api" }; //api call �Ҷ� �������� �տ� �ٴ� dir �̰� �ָ� ���� �Լ� http://v2.petme.kr:19479/api?key=xxx&func=call
// 
// 	CString _SQL{ L"select * from tuser" };
// 	CString _SrcImagePath{ L"C:\\svr\\upload" };
// 
// 	BOOL     _bDbConnected{FALSE};//DB�� disconnect�� ����. ����Stop�ϸ� ~CDatabase���� �ڿ� ������ ������
// 	BOOL _bRecover{FALSE};// ������ �ȵǰ�, �о� ���϶� ������ ������(TRUE), ����� ������(FALSE)
// 	CStringA _tLastRunning;// 5�и��� üũ �Ѵ�. "2021-06-27 12:16:12" 
// 	CString _fullPath;//������






	//ShJObj _jdata{make_shared<JObj>()};
	//JObj& _jd{*_jdata};
	CKCriticalSection _csJdata;
	JObj _jdata;

	/// lock ��� ���¿� �ȵ�� ������ ���� �ؼ� ������.
	ShJObj GetJData()
	{
		AUTOLOCK(_csJdata);
		return make_shared<JObj>(_jdata);/// JObj::Clone(doc->_jdata, true); �� ����
	}
	/// ���� ���� �����ʹ� ���� �־ ����� �θ� ���� �� �ȿ��� ��ŷ �ȴ�.
	int GetInt(PAS k)
	{
		AUTOLOCK(_csJdata);
		return _jdata.I(k);
	}
	int GetSSL(){	return GetInt("_bSSL");	}

	CString GetStr(PAS k)
	{
		AUTOLOCK(_csJdata);
		return _jdata.S(k);
	}
	CStringA GetStrA(PAS k)
	{
		AUTOLOCK(_csJdata);
		return _jdata.SA(k);
	}
	void SetStr(PAS k, PWS val)
	{
		AUTOLOCK(_csJdata);
		_jdata(k) = val;
	}
	void SetClusteringServer(PAS kurl, PAS key, PWS value)
	{
		AUTOLOCK(_csJdata);
		ShJArr arSvr1 = _jdata.Array("Clustering");
		for(int c = 0; c < arSvr1->size(); c++)
		{
			auto sitm1 = arSvr1->GetAt(c); if(!sitm1->IsObject()) continue;
			auto svr1 = sitm1->AsObject(); if(!svr1) continue;
			if(svr1->SameSA("url", kurl))
			{
				(*svr1)(key) = value;// "action": L"closed";
				break;
			}
		}
	}

	void InitData();
// 	CStringA _UdpSvr;
// 	int _portUDP;
	void SampleServer();


	//?deprecated/ clustering : ���� ó���� Ƚ��
	KStdMap<string, int> _mpCntError;

	/// _cntReq % (Clustering.size + main)
	/// 0: main, 1,2,3...
	UINT64 _cntReq{0};
	// InterlockedIncrement(&_cntReq);

	int CheckData();
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
		if(_jdata.I("_bSSL"))
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
	void RestartServer();
	
	bool IsStarted();
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

