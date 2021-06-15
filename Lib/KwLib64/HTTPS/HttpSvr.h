#pragma once
//#define     NOGDI
//#define     NOMINMAX
#include    <winsock2.h>
#include    <ws2tcpip.h>
#include    <objbase.h>
#include    <wtypes.h>
#include <afxmt.h>
#include    "http.h"

#include "../Kw_tool.h"
#include "../ThreadPool.h"
#include "../JSON/JSONValue.h"


#define INITIALIZE_HTTP_RESPONSE( resp, status, reason )\
do \
{	RtlZeroMemory( (resp), sizeof(*(resp)) );           \
	(resp)->StatusCode = (status);                      \
	(resp)->pReason = (reason);                         \
	(resp)->ReasonLength = (USHORT) strlen(reason);     \
} while(FALSE)

#define ADD_KNOWN_HEADER(Response, HeaderId, RawValue)  \
do \
{	(Response).Headers.KnownHeaders[(HeaderId)].pRawValue = (RawValue);\
	(Response).Headers.KnownHeaders[(HeaderId)].RawValueLength =(USHORT) strlen(RawValue);\
} while(FALSE)


class CReqPack
	//: public ISynchronized
{
public:
	CReqPack(HANDLE hReqQ = NULL, PHTTP_REQUEST pReq = NULL, int serial = 0);
	virtual ~CReqPack();

	HANDLE m_hReqQ;
	PHTTP_REQUEST m_pReq;// to be deleted

	CBufArchive m_arc;

	CCriticalSection m_sec;
	CCriticalSection* _GetCS() { return &m_sec; }

	CKRbVal<CString,CString> m_params;

	KBinary m_binr;

	int _resultReceived;
	int _resultSent;
};


typedef shared_ptr<CReqPack> RfReqPack;

class CHttpSvr
{
public:
	CHttpSvr(void);
	virtual ~CHttpSvr(void);

	HANDLE _httpSvr;
	CStringW _uri;
	int InitSvr(LPCWSTR sUri = NULL);
	ULONG Stop(IN PWS sUri = NULL);
	ULONG Shutdown();
	DWORD DoReceiveRequests(HANDLE hReqQueue);

	DWORD HttpPost_ReadBody(RfReqPack pak);
	//DWORD HttpPost_ReadBody(IN HANDLE hReqQueue, IN PHTTP_REQUEST pRequest, CBufArchive& arc);

	int _AsynchResponsePost(RfReqPack pak);
	int _AsynchResponseGet(RfReqPack pak);

	DWORD SendHttpResponse(RfReqPack pak);//IN HANDLE hReqQueue, IN PHTTP_REQUEST pRequest, BYTE* pBufToSend = NULL, ULONG szToSend = 0);
	static DWORD SendHttpResponse(IN HANDLE hReqQueue, IN PHTTP_REQUEST pRequest, IN USHORT StatusCode, IN PSTR pReason, IN PSTR pEntity);


	//DWORD SendHttpResponse2(IN HANDLE hReqQueue, IN PHTTP_REQUEST pRequest);
	virtual int _StartHttpPost(IN HANDLE hReqQ, IN PHTTP_REQUEST pReq);
	virtual int _StartHttpGet(IN HANDLE hReqQ, IN PHTTP_REQUEST pReq, CString& output);
	int ResponseDefault(RfReqPack pak);
	HRESULT _OnHttpReceivePostBuf(RfReqPack pak);

	shared_ptr<function<int(RfReqPack)>> _fncGET;
	shared_ptr<function<int(RfReqPack)>> _fncPOST;
	shared_ptr<function<int(RfReqPack)>> _fncOnConnected;
	shared_ptr<function<int(RfReqPack)>> _fncOnStarted;
	shared_ptr<function<int(RfReqPack)>> _fncOnStopped;
	shared_ptr<function<int(RfReqPack)>> _fncOnError;
	shared_ptr<function<int(RfReqPack)>> _fncOnDisconnected;
	shared_ptr<function<int(RfReqPack)>> _fncOnReceived;
	shared_ptr<function<int(RfReqPack)>> _fncOnSent;

#ifdef _DEBUG
#define HTTP_CB(fnc) shared_ptr<function<int(RfReqPack)>>(DEBUG_NEW function<int(RfReqPack)>(fnc))
#else
#define HTTP_CB(fnc) shared_ptr<function<int(RfReqPack)>>(new function<int(RfReqPack)>(fnc))
#endif // _DEBUG

	template<typename TFNC> void AddCallback(PAS sEvent, TFNC fnc)
	{
		if(tchsame(sEvent, "GET"))
			_fncGET  = HTTP_CB(fnc);
		else if(tchsame(sEvent, "POST"))
			_fncPOST  = HTTP_CB(fnc);
		else if(tchsame(sEvent, "Started"))
			_fncOnStarted = HTTP_CB(fnc);
		else if(tchsame(sEvent, "Stopped"))
			_fncOnStopped  = HTTP_CB(fnc);
		else if(tchsame(sEvent, "Error"))
			_fncOnError  = HTTP_CB(fnc);
		else if(tchsame(sEvent, "Connected"))
			_fncOnConnected = HTTP_CB(fnc);
		else if(tchsame(sEvent, "Disconnected"))
			_fncOnDisconnected  = HTTP_CB(fnc);
		else if(tchsame(sEvent, "Received"))
			_fncOnReceived  = HTTP_CB(fnc);
		else if(tchsame(sEvent, "Sent"))
			_fncOnSent  = HTTP_CB(fnc);
		else {
			ASSERT(0);
		}
	}
	/*
	// callback Lambda함수 파라미터가 똑같아 매크로로 간략히 한다.
	template<typename TFNC> void AddCallbackOnGET(TFNC fnc)
	{	_fncGET  = HTTP_CB(fnc);
	}
	template<typename TFNC> void AddCallbackOnPOST(TFNC fnc)
	{	_fncPOST  = HTTP_CB(fnc);
	}
	template<typename TFNC> void AddCallbackOnConnected(TFNC fnc)
	{	_fncOnConnected  = HTTP_CB(fnc);
	}
	template<typename TFNC> void AddCallbackOnStarted(TFNC fnc)
	{	_fncOnStarted  = HTTP_CB(fnc);
	}
	template<typename TFNC> void AddCallbackOnStopped(TFNC fnc)
	{	_fncOnStopped  = HTTP_CB(fnc);
	}
	template<typename TFNC> void AddCallbackOnError(TFNC fnc)
	{	_fncOnError  = HTTP_CB(fnc);
	}
	template<typename TFNC> void AddCallbackOnDisconnected(TFNC fnc)
	{	_fncOnDisconnected  = HTTP_CB(fnc);
	}
	template<typename TFNC> void AddCallbackOnReceived(TFNC fnc)
	{	_fncOnReceived  = HTTP_CB(fnc);
	}
	template<typename TFNC> void AddCallbackOnSent(TFNC fnc)
	{	_fncOnSent  = HTTP_CB(fnc);
	}
	*/
};


