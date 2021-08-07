#pragma once
#include <afxinet.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <msxml6.h>

#include "..\Kw_tool.h"
#include "..\Dictionary.h"
//#include "..\XmlMap.h"

#define WM_APP_CM (WM_APP+331)

#ifdef _Use_CM
#include "KwLib32\mobile\Connection.h"
#endif // UNDER_CE


#define XREQUEST(req) \
	hres req(cxaf& hs, cxaf& hsr){return Req_Post(__FUNCTIONT__, &hs, &hsr);}

enum EHttpError
{
	eHrErr_Timeout = -100,
	eHrErr_ServerNotOpen = eHrErr_Timeout,
	eHrErr_NoRespData = -41, // Req_?? �Լ��� ���� ��쵵 �̰� ����
	eHrErr_BadPacket = -51,
};

class IProgressHttp
{
public:
	enum { ePhNone, ePhStatic};
	virtual void _UpdateProgressLog(PWS sLog, int iOp = 0) = NULL;
//	virtual void _UpdateProgress(int reqId, int nByte, int nByteAll) = NULL;
};

/*
// �̰� �ܺο��� thread ���� session Close �õ� �Ϸ��� ������ٰ�
// �ϼ� ����... ���ֵ� ����...
class CServerPtr : public IAutoClosePtr<CHttpConnection>
{
public:
	CServerPtr() : m_pServer(NULL)
	{
	}
	//CKRbStrPtr, 
	//CKRbMapPtrAuto
	//CKRbMapSimple<CString, CHttpConnection*> m_mapHC;//����� �־� �ѱ� ���� �ϴٰ�... ������ �ϰ��
	CHttpConnection* m_pServer;
	virtual int _Begin(CHttpConnection*& p)
	{
		m_pServer = p;
		return 0;
	}
	virtual int _End(CHttpConnection*& p)
	{
		m_pServer = NULL;
		return 0;
	}
	CHttpConnection* GetCurServer()
	{
		return m_pServer;
	}
};
*/


class IDownload
{
public: 
	virtual hres _Downloading(PWS file, long sizeSum, long size) { return S_OK;}
};

enum EReqStep { eRsStart, eRsSetOp, eRsOpenReq, eRsSendReq, eRsWrite, eRsEndReq, eRsReadResp};
// requeset �Ҷ� ���� ��Ȳ�� ó�� �Ѵ�.
class IReqProgress
{
public:
	virtual hres _ReqProgress(EReqStep erq) = NULL;
};

//CS_DEF_CLASS1(CsHttpSSL);

class CHttpClient// : public CServerPtr
{
public:
	CHttpClient(void);
	virtual ~CHttpClient(void);
	CString m_sSession; // GpsMate or "Prk2"
	CString m_ipSvr;// "www.onstylei.com"
	CString m_sUri; // "/gps/?"
	int m_port;		// 80
	int m_nVerb; //CHttpConnection::HTTP_VERB_POST
	CString m_header;//http header�� Content-Type: application/x-www-form-urlencoded ó���� ':'�α��� \r\n ���� ����
	//CKSection m_crByte;//for m_byteSend, m_byteResv
	//CS_DEF_MEMBER_M(m_crByte);

	//CS_STATIC_INSTANCE2(CsHttpSSL)

	float m_byteSend;
	float m_byteResv;

	enum { eBUFSIZE = 10240,};
	CString m_sError;
	DWORD m_dwError;// Exception
	DWORD m_dwRet;// QueryInfoStatusCode
	int _status{ 200 };// HTTP status Code eHttp_OK = 200,
	CString m_sReturn;
	EReqStep m_eReqStep;
//	KBinary m_buf;
	void InitHost(PWS sSs, PWS sIP, PWS sUri, int iPort = 0)
	{
		m_sSession = sSs;
		m_ipSvr = sIP;
		m_sUri = sUri;
		m_port = iPort;
	}

	// �ٿ�ε� �޴� ���� ��Ȳ�� UI�� ǥ�� �ϰ� ������...
	IProgressHttp* m_pPrgress;

	UINT m_nGrowSize;

	bool m_bEncode;

	CComPtr<IServerXMLHTTPRequest2> _sxrq;
	CAutoCoInit _coInit;
// 	static CHttpClient* CreateHostApp(CHttpClient* pht, PWS func, LPARAM lp = NULL);
// 	static void FillHostInfo_(CString& svr, CString& sUri, int& port);
// 
// 	static CHttpClient* CreateHttpClient_(LPARAM lp = NULL)
// 	{
// 		return new CHttpClient;
// 	}


	void HttpLog(PWS sLog, int iOp = 0);
	void Test();
	int m_nMilSecTimeoutResolve;// connect
	int m_nMilSecTimeoutSend;// send
	int m_nMilSecTimeoutRcv;//rcv : �̰� ���� Ÿ�Ӿƿ� ������ �Ѵ�.
	int m_nMilSecTimeoutConnect;// connect
protected:
public:
	void SetTimeout(int timeout)
	{
		m_nMilSecTimeoutResolve = timeout;
	}

	enum 
	{
		ePtNormal, 
		ePtForm   = 1 << 0, 
		ePtXml    = 1 << 1,
		ePtBinary = 1 << 4,
		ePtGoogleAuto  = 1 << 16,
	};
	hres RequestPost( LPCTSTR pszServerName, INTERNET_PORT nPort, LPCTSTR sUrlTail, KBinary* pBin, KBinary* pBinr, int iOp = ePtNormal);
#ifdef _Use_old_hashmap
	hres RequestPost( LPCTSTR sFunc, CHashBase* phs, CHashBase* phsr );
#endif // _Use_old_hashmap
//	hres RequestPost( LPCTSTR pFunc, CXmlMapObj* phs, CXmlMapObj* phsr , PWS url = NULL);
	hres RequestGet( LPCTSTR pszServerName, INTERNET_PORT nPort, LPCTSTR sUrlTail, KBinary* pBinr, UINT szBuf = 1024);
	hres RequestFile( LPCTSTR pszServerName, INTERNET_PORT nPort, LPCTSTR sUrlTail, KBinary* pBinr);
	hres LoadImage(const TCHAR* strURL, const TCHAR* strSavePath);
	hres LoadImage(PWS strURL, PWS sLocalFolder, OUT CString& sFile);

	hres LoadFile(const TCHAR* strURL, KBinary* pBinr, IDownload* pii = NULL);

	// internet ���ܼ� �ٽ� ���� ��� �Ʒ��� �ҷ��� �ٸ��� �϶��
	virtual void _CheckReconnected(){}
	hres RequestGetSSL( LPCTSTR surl, KBinary* pBinr, KBinary* pBin, PAS sMethod = "GET", std::map<string, string>* httpHdr = NULL);//UINT szBuf, 
	int RequestPostSSL(LPCTSTR surl, KBinary* pBinr, KBinary* pBin, std::map<string, string>* httpHdr = NULL);
	int RequestPostSSL(LPCTSTR surl, KBinary* pBinr, LPCSTR buf, UINT szBuf, std::map<string, string>* httpHdr = NULL);
	//int RequestPostSSL( LPCTSTR surl, KBinary* pBinr, LPCSTR buf, UINT szBuf, CKRbVal<CStringW, CStringW>* httpHdr = NULL);
	
	virtual void Reserved(PWS id);
	static CStringW HttpErrorStr(DWORD dwRet);
	//int ReverseGeocode(cxaf* phsr, CString sRef, PWS apikey);

	IReqProgress* m_pReqProgress;
	virtual hres _ReqProgress(EReqStep erq);
};







#define DEFAULT_BUFLEN 512


#ifdef _Use_SockClient


// ���� ���� Ŭ���̾�Ʈ, ������ ���� base class
class CSockClient
{
public:
	CSockClient(PWS ip = L"localhost", int port = 20001);
	TCHAR m_spcLen;
	virtual ~CSockClient();
	CString m_IP;// = "localhost";
	int m_port;// = "10000";
	int m_lenHead;
	hres m_hErr;
	CString m_sErr;

	SOCKET m_socket;// = INVALID_SOCKET;
	KBinary m_bin;
	KBinary m_binr;
	virtual hres _OnError(int line, PWS fFunc, PWS sErr1, PWS sErr2, int err = -1, LPARAM lp = NULL)
	{
		return S_OK;
	}

	// len ������ �տ� �������� ä���� char type ��Ŷũ���� ���� ���ڿ�
	virtual PWS _LengthFormat(int len = 0);
	void LenFormat(int len = 10, TCHAR spc = ' ');



#ifdef _Usage
	void Usage()
	{
		CSckCl_Bank bk(L"localhost", 20001); // WSAStartup
		hr = bk.Connect();
		int rv = bk.SendLen(len);
		rv = bk.Send(pk1, len);
		hr = bk.CloseSend();

		int lenPk = bk.RecvLen();
		CStringA sbuf;
		LPSTR rbuf = sbuf.GetBuffer(lenPk);
		bk.Recv(lenPk, rbuf);
	}
#endif // _Usage

	// m_bin �� ������ m_binr �� �޴´�.
	int Request();

	int Send()
	{
		if(m_bin.m_p == NULL)
			return -1;
		return Send(m_bin.m_p, m_bin.m_len);
	}

	int Send(PAS buf, int len);

	hres Connect();
	hres Close(int mode);
	hres CloseSend()
	{
		return Close(SD_SEND);
	}
	hres CloseReceive()
	{
		return Close(SD_RECEIVE);
	}
	hres Recv(int lenToRd)
	{
		m_binr.Alloc(lenToRd);
		return Recv(lenToRd, m_binr.m_p);
	}
	hres Recv(int lenToRd, LPSTR recvbuf);
	int RecvLen();
	int SendLen(int len);
};

#endif //_Use_SockClient


