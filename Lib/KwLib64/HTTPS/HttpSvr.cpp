#include "pch.h" /// 이 파일은 .. 에 있지만 여기서  "../pch.h" 로 주지 않는다. pre compiled header는 안한다.

#include <winsock2.h>
#include <ws2tcpip.h>

#include "../ThreadPool.h"
#include "../Lock.h"
#include "../tchtool.h"
#include "../KBinary.h"
#include "HttpSvr.h"

#pragma comment(lib, "Httpapi.lib")
//#pragma comment (lib, "Ws2_32.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


CHttpSvr::CHttpSvr(void)
{
}

CHttpSvr::~CHttpSvr(void)
{
}


int  CHttpSvr::InitSvr(LPCWSTR sUri)
{
	BACKGROUND(1);

	ULONG           retCode = 0;
	HANDLE          hReqQueue      = NULL;
	HTTPAPI_VERSION ver1 = HTTPAPI_VERSION_1;
	HTTPAPI_VERSION ver2 = HTTPAPI_VERSION_2;
	if(sUri == NULL)
		sUri = (PWS)_uri;
	ASSERT(tchlen(sUri));
	try
	{
		retCode = HttpInitialize(ver1, HTTP_INITIALIZE_SERVER, NULL);
		if(retCode != NO_ERROR)
		{
			TRACE(L"HttpInitialize failed with %u \n", retCode);
			return retCode;
		}
		retCode = HttpCreateHttpHandle(&hReqQueue, 0); // Req Queue Reserved
		if(retCode != NO_ERROR)
		{
			TRACE(L"HttpCreateHttpHandle failed with %u \n", retCode);
			throw retCode;
		}

		//   "http://www.adatum.com:80/vroot/"
		//   "https://adatum.com:443/secure/database/"
		//   "http://+:80/vroot/"
		_uri = sUri;
		retCode = HttpAddUrl(hReqQueue, sUri, NULL);// "http://+:19670/gps/" Req Queue// Fully qualified URL// Reserved
		if(retCode == NO_ERROR)
		{
			_httpSvr = hReqQueue;
		}
		else if(retCode == ERROR_ACCESS_DENIED)//5L
		{	// 이미 소켓이 사용 되는 경우다. 관리자 모드로 VS를 실행 해보라.
			TRACE(L"ERROR_ACCESS_DENIED %u\r\n", retCode);
			AfxMessageBox(L"ERROR_ACCESS_DENIED. Excute with admin access right.");
			throw retCode;
		}
		else
		{	// 이미 소켓이 사용 되는 경우다. 관리자 모드로 VS를 실행 해보라.
			TRACE(L"HttpAddUrl failed with %u\r\n", retCode);
			AfxMessageBox(L"HttpAddUrl Error!.");
			throw retCode;
		}

		ULONG result = -1;
		result = DoReceiveRequests(hReqQueue);// CloseHandle 해도 떨어 진다. ExitInstance
		TRACE(L"DoReceiveRequests failed with %u\r\n", result);
		retCode = result;
	}
	catch(CException* e)
	{
		e->Delete();// delete e;
	}
	catch(ULONG err)
	{
		TRACE(L"error retCode(%u)\n", err);
	}
	catch(...) {}

	//  Cleanup the HTTP Server API
	HttpRemoveUrl(hReqQueue, sUri);// Req Queue// Fully qualified URL
	ULONG ru = HttpTerminate(HTTP_INITIALIZE_SERVER, NULL);

	return retCode;
}

ULONG CHttpSvr::Stop(IN PWS sUri)
{
	try
	{
		if(sUri == NULL)
			sUri = (PWS)_uri;
		ULONG ru = HttpRemoveUrl(_httpSvr, sUri);// Req Queue// Fully qualified URL
		ru = HttpShutdownRequestQueue(_httpSvr);
		return ru;//NO_ERROR
	}
	catch(CException* e)
	{
		e->Delete();//delete e;
	}
	return 0;
}
ULONG CHttpSvr::Shutdown()
{
	try
	{
		ULONG ru = HttpTerminate(HTTP_INITIALIZE_SERVER, NULL);
		return ru;
	}
	catch(CException* e)
	{
		e->Delete();//delete e;
	}
	return 0;
}
DWORD CHttpSvr::DoReceiveRequests(IN HANDLE hReqQueue)
{
	BACKGROUND(1);

	ULONG              result = 0;
	HTTP_REQUEST_ID    requestId = 0;
	DWORD              bytesRead = 0;
	//PCHAR              pReqBuf;
	ULONG              uReqBufLen = 0;
	// Allocate a 2 KB buffer. This size should work for most 
	// requests. The buffer size can be increased if required. Space
	// is also required for an HTTP_REQUEST structure.
	const int c_uLen = sizeof(HTTP_REQUEST) + (1024*4);// 2kb 만 하라고 했는데 4k 한다.
	HTTP_SET_NULL_ID(&requestId);
	// Receive a Request
	for(;;)
	{
		uReqBufLen = c_uLen;

		PHTTP_REQUEST pReq = (PHTTP_REQUEST)new char[uReqBufLen];//?see reqBuf.Detatch() => CTaskHttp => CHttpSmo::_Response
		if(pReq == NULL) return ERROR_NOT_ENOUGH_MEMORY;
		RtlZeroMemory((PHTTP_REQUEST)pReq, uReqBufLen);
		//KAtEnd d_Req([&](){	DeleteMeSafe(pReq);	 });
		CAutoFreePtr<HTTP_REQUEST> reqBuf(pReq);//중간에 다시 realloc할수 있으니 반드시 reference로 넘겨 주는 auto free를 써야 한다.


		result = HttpReceiveHttpRequest(hReqQueue, requestId, 0, pReq, uReqBufLen, &bytesRead, NULL);
		if(NO_ERROR == result)
		{
			switch(pReq->Verb)// POST or GET
			{
				case HttpVerbPOST:
				{
					int rv = _StartHttpPost(hReqQueue, reqBuf.Detatch());
					if(rv < 0)
						continue;
					pReq = NULL;
				} break;
				case HttpVerbGET:
				{
					CString output;
					int rv = _StartHttpGet(hReqQueue, pReq, output);
					CStringA sa(output);//"Hey! You hit the server \r\n");
					result = SendHttpResponse(hReqQueue, reqBuf.Detatch(), 200, (PSTR)"OK", (PSTR)(LPCSTR)sa);
				}	break;
				default:
					TRACE(L"Got a unknown request for %ws \n", pReq->CookedUrl.pFullUrl);
					result = SendHttpResponse(hReqQueue, pReq, 503, (PSTR)"Not Implemented", NULL);
					break;
			}

			HTTP_SET_NULL_ID(&requestId);
			if(result != NO_ERROR)
				continue;
		}
		else if(result == ERROR_MORE_DATA)
		{
			//위에 HttpReceiveHttpRequest 를 몇번 더한다.
			requestId = pReq->RequestId;
			uReqBufLen = bytesRead;
			delete pReq;
			pReq = (PHTTP_REQUEST)new char[uReqBufLen];
			if(pReq == NULL)
			{
				result = ERROR_NOT_ENOUGH_MEMORY;
				break;
			}
		}
		else if(ERROR_CONNECTION_INVALID == result && !HTTP_IS_NULL_ID(&requestId))
		{
			HTTP_SET_NULL_ID(&requestId);
		}
		else if(result == ERROR_INVALID_HANDLE)
		{
			HTTP_SET_NULL_ID(&requestId);
		}
		else
		{
			break;// CloseHandle 해도 떨어 진다. ExitInstance
		}
	}
	return result;
}


DWORD CHttpSvr::HttpPost_ReadBody(RfReqPack pak)
{
	SyncFnc(pak);
	if((pak->m_pReq->Flags & HTTP_REQUEST_FLAG_MORE_ENTITY_BODY_EXISTS) == 0)
		return 0;
	DWORD           result = NO_ERROR;
	UCHAR pEBuf[10240] ={0,};
	ULONG uLenEBuf = sizeof(pEBuf);// 10240;

	try
	{
		do // 량이 많은 경우 여러번 읽는다.
		{
			ULONG uRead = 0;
			result = HttpReceiveRequestEntityBody(pak->m_hReqQ, pak->m_pReq->RequestId, 0, pEBuf, uLenEBuf, &uRead, NULL);
			if(result == NO_ERROR)
			{
				if(uRead > 0)
					pak->m_arc.Write(pEBuf, uRead);
			}
			else if(result != ERROR_MORE_DATA)// 234 , ERROR_HANDLE_EOF 38
				throw result;//아래 catch로
		} while(TRUE);
	}
	catch(DWORD err)
	{
		switch(err)
		{
			case ERROR_HANDLE_EOF://38
				TRACE(L"Reading has been Completed! ERROR_HANDLE_EOF(%lu) \n", err);
				break;
			case ERROR_INVALID_PARAMETER:
				break;
			default:
				TRACE(L"HttpPost_ReadBody failed with %lu \n", err);
				break;
		}
	}
	pak->_resultReceived = result;
	if(_fncOnReceived.get())
		(*_fncOnReceived.get())(pak);
	return result;
}


DWORD CHttpSvr::SendHttpResponse(
	IN HANDLE        hReqQueue,
	IN PHTTP_REQUEST pReq,
	IN USHORT        StatusCode,
	IN PSTR          pReason,
	IN PSTR          pEntityString)
{
	HTTP_RESPONSE   response;
	HTTP_DATA_CHUNK dataChunk[1];
	DWORD           result = 0;
	DWORD           bytesSent = 0;
	memset(&response, 0, sizeof(HTTP_RESPONSE));
	memset(&dataChunk[0], 0, sizeof(HTTP_DATA_CHUNK));
	// Initialize the HTTP response structure.
	INITIALIZE_HTTP_RESPONSE(&response, StatusCode, pReason);
	// Add a known header.
	ADD_KNOWN_HEADER(response, HttpHeaderContentType, "text/html");
	if(pEntityString)
	{
		// Add an entity chunk.
		dataChunk[0].DataChunkType           = HttpDataChunkFromMemory;
		dataChunk[0].FromMemory.pBuffer      = pEntityString;
		dataChunk[0].FromMemory.BufferLength = (ULONG)strlen(pEntityString);
		response.EntityChunkCount         = 1;
		response.pEntityChunks            = dataChunk;
	}
	// 
	// Because the entity body is sent in one call, it is not
	// required to specify the Content-Length.
	result = HttpSendHttpResponse(
		hReqQueue,           // ReqQueueHandle
		pReq->RequestId, // Request ID
		0,                   // Flags
		&response,           // HTTP response
		NULL,                // pReserved1
		&bytesSent,          // bytes sent  (OPTIONAL)
		NULL,                // pReserved2  (must be NULL)
		0,                   // Reserved3   (must be 0)
		NULL,                // LPOVERLAPPED(OPTIONAL)
		NULL                 // pReserved4  (must be NULL)
	);
	if(result != NO_ERROR)
	{//87:ERROR_INVALID_PARAMETER
		TRACE(L"HttpSendHttpResponse failed with %lu \n", result);
	}
	return result;
}
/*
int CHttpSvr::_StartHttpPost(IN HANDLE hReqQ, IN PHTTP_REQUEST pReq)
{	//동기식 방법: 비동기식은 참조 CHSSmo::_StartHttpPost
	int result = -1;
	if(pReq->Flags & HTTP_REQUEST_FLAG_MORE_ENTITY_BODY_EXISTS)
	{
		CReqPack opak(this, hReqQ, pReq);
		SyncFnc(opak);
		result = HttpPost_ReadBody(&opak);
		result = ResponseDefault(&opak);// hReqQ, pReq, &arc);
	}
	return result;
}*/


//?see _CreateServer()
int CHttpSvr::_StartHttpPost(IN HANDLE hReqQ, IN PHTTP_REQUEST pReq)
{
	BACKGROUND(1);
	int result = 0;

	RfReqPack pak = shared_ptr<CReqPack>(new CReqPack(hReqQ, pReq));
	if(_fncOnConnected.get())
	{
		result = (*_fncOnConnected.get())(pak);
		if(result < 0)
			return -1;
	}
	if(KwGetUrlParams(pak->m_pReq->pRawUrl, pak->m_params) == 0)
	{
		//OpenSiteFile(file);
		return -1;
	}

	QueueFUNCN([=]()-> void
	{
		_AsynchResponsePost(pak);
	}, "비동기 POST 세션 분기");
	//CTaskHttp* ptk = new CTaskHttp(pak, bAutoDelPak);
	//CMainPool::QueueTask(ptk);// => _AsynchResponse(CTaskHttp* pTask)
	return 0;
}

int CHttpSvr::_StartHttpGet(IN HANDLE hReqQ, IN PHTTP_REQUEST pReq, CString& output)
{
	BACKGROUND(1);
	int result = 0;

	RfReqPack pak = shared_ptr<CReqPack>(new CReqPack(hReqQ, pReq));
	//CReqPack* pak = new CReqPack(hReqQ, pReq);
	if(_fncOnConnected.get())
	{
		result = (*_fncOnConnected.get())(pak);
		if(result < 0)
			return -1;
	}

	if(KwGetUrlParams(pak->m_pReq->pRawUrl, pak->m_params) == 0)
	{
		//OpenSiteFile(file);
		return -1;//파라미터가 없으면 파일 열어야지.
	}
	QueueFUNCN([=]()-> void
	{
		_AsynchResponseGet(pak);
	}, "비동기 GET 세션 분기");
	return 0;
}

//?주의: 위와 class 다름... 이름 비슷
//<= CTaskHttp::DoTask
int CHttpSvr::_AsynchResponsePost(RfReqPack pak)//CTaskHttp* pTask)
{
	BACKGROUND(2);//1차적으로 서버가 백그이고, 하나 접속이 있으면 백그2로 분기 한다.
	int rv = 0;
	int result = 0;
	try
	{
		if(pak->m_pReq->Flags & HTTP_REQUEST_FLAG_MORE_ENTITY_BODY_EXISTS)
		{
			result = HttpPost_ReadBody(pak);//pak->m_arc 에 POST data가 채워진다.
			if(result != NO_ERROR && result != ERROR_HANDLE_EOF) // 읽은게 없더라도 NO_ERROR 이다.
				throw result;
		}
		rv = ResponseDefault(pak);//_OnHttpReceivePostBuf + SendHttpPostResponse
		//DeleteMeSafe(pak);
	}
	catch(int erv)
	{
		TRACE("_AsynchResponse err(%d)", erv);
	}
	return rv;
}
int CHttpSvr::_AsynchResponseGet(RfReqPack pak)//CTaskHttp* pTask)
{
	BACKGROUND(2);//1차적으로 서버가 백그이고, 하나 접속이 있으면 백그2로 분기 한다.
	int rv = 0;
	int result = 0;
	try
	{
		ASSERT((pak->m_pReq->Flags & HTTP_REQUEST_FLAG_MORE_ENTITY_BODY_EXISTS) == 0);
		rv = ResponseDefault(pak);//_OnHttpReceivePostBuf + SendHttpPostResponse
		//DeleteMeSafe(pak);
	}
	catch(int erv)
	{
		TRACE("_AsynchResponse err(%d)", erv);
	}
	return rv;
}

/// <summary>
/// 여기서 드디어 요청의 리턴값을 만들어 응답한다.
/// </summary>
/// <param name="pak"></param>
/// <returns></returns>
int CHttpSvr::ResponseDefault(RfReqPack pak)
{
	SyncFnc(pak);
	int rv = 0;
	switch(pak->m_pReq->Verb)// POST or GET
	{
		case HttpVerbPOST:
		{
			HRESULT hr = _OnHttpReceivePostBuf(pak);
		} break;
		case HttpVerbGET:
		{
			try
			{
				if(_fncGET.get())
				{
					rv = (*_fncGET.get())(pak);
					return rv;
				}
			}
			catch(CException*)
			{
				//DelException(e);
			}
		}	break;
		default:
			break;
	}

	rv = SendHttpResponse(pak);
	return rv;
}



DWORD CHttpSvr::SendHttpResponse(RfReqPack pak)
{
	//	SyncFnc(pak);
	UINT_PTR len = 0;
	BYTE* pBufToSend = (BYTE*)pak->m_binr.GetPtr(len);
	if(len == 0)
		return 0;
	DWORD           result = 0;
	HTTP_RESPONSE   response;
	DWORD           bytesSent = 0;
	CStringA        szContentLength;//[MAX_ULONG_STR];
	HTTP_DATA_CHUNK dataChunk[1];

	try
	{
		INITIALIZE_HTTP_RESPONSE(&response, 200, "OK");
		//ASSERT(pReq->Flags & HTTP_REQUEST_FLAG_MORE_ENTITY_BODY_EXISTS);
		// Mobile에서 보내니 이 Flags 가 0이네.
		szContentLength.Format("%d", len);
		ADD_KNOWN_HEADER(response, HttpHeaderContentLength, (LPCSTR)szContentLength);
		//pak->m_tic2 = GetTickCount64(); //응답 보내기 직전 까지 시간

		result = HttpSendHttpResponse(
			pak->m_hReqQ,           // ReqQueueHandle
			pak->m_pReq->RequestId, // Request ID
			HTTP_SEND_RESPONSE_FLAG_MORE_DATA,
			&response,       // HTTP response
			NULL,            // pReserved1
			&bytesSent,      // bytes sent-optional
			NULL, 0, NULL, NULL);
		if(result != NO_ERROR)
			throw result;

		if(pBufToSend && len > 0)
		{
			dataChunk[0].DataChunkType = HttpDataChunkFromMemory;
			dataChunk[0].FromMemory.pBuffer = pBufToSend;//arcR.GetPtr();
			dataChunk[0].FromMemory.BufferLength = (ULONG)len;//arcR.GetLength();

			result = HttpSendResponseEntityBody(pak->m_hReqQ, pak->m_pReq->RequestId,
				0,           // This is the last send.
				1,           // Entity Chunk Count.
				dataChunk, NULL, NULL, 0, NULL, NULL);
			
			pak->_resultSent = result;

			if(_fncOnSent.get())
			{
				result = (*_fncOnSent.get())(pak);
				if(result < 0)
					return -1;
			}

			if(result != NO_ERROR)
				throw result;
		}
	}
	catch(ULONG ue)
	{
		//pak->m_bResponsed = -1;
		switch(ue)
		{
			case ERROR_NOT_ENOUGH_MEMORY:
				TRACE(L"Insufficient resources \n");
				break;
			default:
				TRACE(L"HttpSendHttpResponse failed with %lu \n", ue);
				break;
		}
	}

	return result;
}

CReqPack::CReqPack(HANDLE hReqQ, PHTTP_REQUEST pReq, int serial)
	: m_hReqQ(hReqQ)
	, m_pReq(pReq)
{
}

CReqPack::~CReqPack()//?pak
{
	DeleteMeSafe(m_pReq);
}


HRESULT CHttpSvr::_OnHttpReceivePostBuf(RfReqPack pak)
{
	SyncFnc(pak);
	LPCSTR pUrl = pak->m_pReq->pRawUrl;
	HRESULT hr = S_OK;
	CString sUrl(pUrl);
	try
	{
		if(_fncPOST.get())
		{
			hr = (*_fncPOST.get())(pak);
			return hr;
		}

		//		CStringA sFuncA(pak->m_func);
		CString sType;
		pak->m_params.Lookup(L"type", sType);
		if(sType == L"JSON")
		{
			KBinary bin;
			bin.Wrap((LPCSTR)pak->m_arc.GetPtr(), pak->m_arc.GetLength());
			pak->m_binr.SetPtr(bin.m_p, bin.m_len); // 샘플: 고대로 에코 복사
			/*
			//CStringA keyFunc = CStringA("CHttpSmo::") + sFuncA;
			MapRemoteFunctions();// 함수매핑 체크
									//FuncODI jproc = NULL;
			CFuncItem funcItm;
			if(m_mapRFncs.Lookup(sFuncA, funcItm))//jproc);
			{
				CString sWstr;
				KwUTF8ToWchar(bin.GetPA(), sWstr);
				JSONValue* jdoc = JSON::Parse((PWS)sWstr);

				CJsonPbj jbj(jdoc->AsObject());
				CString fc = jbj.S("Func");
				CString guid = jbj.S("GUID");

				CJsonPbj jpa = jbj.O("Params");
				JSONObject* jobj = NULL;
				{
					SyncPbj(pak);
					CJsonPbj jsr;
					/// //////////////////////////////////////////
					hr = (this->*(funcItm.func))(*pak, jpa, jsr);//ok
					CJsonPbj jres;
					//AddPolicy(jsr, sFuncA);// jsr에 policy를 붙인다. 전에는 더 위층에 했으나

					if(jsr->size() > 0)
						jres("Response") = jsr;

					PackToResponse(pak, jres);// indent xml Req_KeepAlive
				}// Sync
			}// loopup
			*/
		}//json
		else
		{
			KBinary bin;
			bin.Wrap((LPCSTR)pak->m_arc.GetPtr(), pak->m_arc.GetLength());
			pak->m_binr.SetPtr(bin.m_p, bin.m_len); // 샘플: 고대로 에코 복사
		}
	}
	catch(CException*)
	{
		//DelException(e);
	}
	catch(...)
	{
		//PushToNotifyException__();
	}
	return hr;
}
