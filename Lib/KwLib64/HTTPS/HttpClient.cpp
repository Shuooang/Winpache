#include "pch.h"
//#include "..\DllAttr.h"
#include "../inlinefnc.h"
#include "../KBinary.h"
#include "../KTemple.h"
#include "../KDebug.h"
#include "../tchtool.h"


#include "HttpClient.h"

#include <msxml6.h>
#include <afxinet.h>
#include <comutil.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//SETBUILDTIME2(DEBUGCOMPILETIME);// 이 파일의 컴파일 시각이 Serialize 에 사용 되도록 한다.

#if defined(_BETA) || defined(_DEBUG)
#define _BETAD
#endif


CHttpClient::CHttpClient(void)
: m_byteSend(0)
, m_byteResv(0)
, m_nMilSecTimeoutResolve(2000) // 연결
, m_nMilSecTimeoutConnect(3000)// 이건 파일 다운 로드 받을떄도 있으니. 안쓴다.
, m_nMilSecTimeoutSend(30000)
, m_nMilSecTimeoutRcv(30000)
, m_pPrgress(NULL)
, m_nGrowSize(10240)
, m_bEncode(false)
, m_dwError(0)
, m_dwRet(0)
, m_nVerb(CHttpConnection::HTTP_VERB_POST)
, m_pReqProgress(NULL)
{
	//m_buf.Alloc(eBUFSIZE);
}

CHttpClient::~CHttpClient(void)
{
}
void CHttpClient::Test()
{
	CStringA strData = "Some very long data to be POSTed here!";
	DWORD sz = strData.GetLength()+1;
	KBinary bin, binr;
	bin.Attach((LPSTR)(LPCSTR)strData, sz);
	RequestPost(L"192.168.10.70", 7510, L"/gps/?", &bin, &binr);
	bin.Detach();
	// http://192.168.10.70:7510/gps/
}

CStringW CHttpClient::HttpErrorStr(DWORD dwRet)
{
	CStringW psErr;
	switch (dwRet)
	{
		CASE_STR0(HTTP_STATUS_OK                 );
		CASE_STR0(HTTP_STATUS_CREATED            );
		CASE_STR0(HTTP_STATUS_ACCEPTED           );
		CASE_STR0(HTTP_STATUS_PARTIAL            );
		CASE_STR0(HTTP_STATUS_NO_CONTENT         );
		CASE_STR0(HTTP_STATUS_RESET_CONTENT      );
		CASE_STR0(HTTP_STATUS_PARTIAL_CONTENT    );
		CASE_STR0(HTTP_STATUS_AMBIGUOUS          );
		CASE_STR0(HTTP_STATUS_MOVED              );
		CASE_STR0(HTTP_STATUS_REDIRECT           );
		CASE_STR0(HTTP_STATUS_REDIRECT_METHOD    );
		CASE_STR0(HTTP_STATUS_NOT_MODIFIED       );
		CASE_STR0(HTTP_STATUS_USE_PROXY          );
		CASE_STR0(HTTP_STATUS_REDIRECT_KEEP_VERB );
		CASE_STR0(HTTP_STATUS_BAD_REQUEST        );
		CASE_STR0(HTTP_STATUS_DENIED             );
		CASE_STR0(HTTP_STATUS_PAYMENT_REQ        );
		CASE_STR0(HTTP_STATUS_FORBIDDEN          );
		CASE_STR0(HTTP_STATUS_NOT_FOUND          );
		CASE_STR0(HTTP_STATUS_BAD_METHOD         );
		CASE_STR0(HTTP_STATUS_NONE_ACCEPTABLE    );
		CASE_STR0(HTTP_STATUS_PROXY_AUTH_REQ     );
		CASE_STR0(HTTP_STATUS_REQUEST_TIMEOUT    );
		CASE_STR0(HTTP_STATUS_CONFLICT           );
		CASE_STR0(HTTP_STATUS_GONE               );
		CASE_STR0(HTTP_STATUS_LENGTH_REQUIRED    );
		CASE_STR0(HTTP_STATUS_PRECOND_FAILED     );
		CASE_STR0(HTTP_STATUS_REQUEST_TOO_LARGE  );
		CASE_STR0(HTTP_STATUS_URI_TOO_LONG       );
		CASE_STR0(HTTP_STATUS_UNSUPPORTED_MEDIA  );
		CASE_STR0(HTTP_STATUS_RETRY_WITH         );
		CASE_STR0(HTTP_STATUS_SERVER_ERROR       );
		CASE_STR0(HTTP_STATUS_NOT_SUPPORTED      );
		CASE_STR0(HTTP_STATUS_BAD_GATEWAY        );
		CASE_STR0(HTTP_STATUS_SERVICE_UNAVAIL    );
		CASE_STR0(HTTP_STATUS_GATEWAY_TIMEOUT    );
		CASE_STR0(HTTP_STATUS_VERSION_NOT_SUP    );
	default: 
		psErr.Format(L"HTTP_UnknownError(%u)", dwRet);
	}
	return psErr;
}
hres CHttpClient::_ReqProgress(EReqStep erq)
{
	m_eReqStep = erq;
	if(m_pReqProgress)
	{
		if(m_pReqProgress->_ReqProgress(erq) == 1)
			Sleep(1);// UI변경 했으니 thread 여유 주고
	}
	return S_OK;
}

hres CHttpClient::RequestPost( LPCTSTR pszServerName, INTERNET_PORT nPort, LPCTSTR sUrlTail, KBinary* pBin, KBinary* pBinr, int iOp)
{
	// SmoBranch가 로그인 할때
	// 	+		pszServerName	"daree.gugose.co.kr"	const wchar_t *
// 			nPort	18470	unsigned short
// 	+		sUrlTail	"/gps/?Req_20Login=0"	const wchar_t *
	// push 보낼떄
// 	+		pszServerName	0x00a27200 "push.gugose.co.kr"	const wchar_t *
// 	nPort	80	unsigned short
// 	+		sUrlTail	0x014beda8 "/daree/pushdaree3.php"	const wchar_t *
	hres hr = S_OK;
	CInternetSession session(m_sSession);
	CHttpConnection* pServer = NULL;
	//CAutoPtrClose<CHttpConnection> _pServer(pServer, this);//여기서 아직 NULL
	KAtEnd d_stream([&]() {
		if(pServer)
		{
			session.Close();
			pServer->Close();
		}
	});
	CHttpFile* pFile = NULL;
	//CAutoPtrClose<CHttpFile> _pFile(pFile);
	const int csz = 1024;

	CString strServerName;
	DWORD dwRet = 0;
	_ReqProgress(eRsStart);

	/// //////////////////////////////////////////////////////////////////////////
	try
	{// 이전에 goto EndLabel; 를 대신 하여 throwLINE 로 바꿈.
		try
		{
			session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 3);
		if(m_nMilSecTimeoutResolve > 0)// 60sec
			session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, m_nMilSecTimeoutResolve);//5sec
		if(m_nMilSecTimeoutSend > 0)// 60sec
			session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, m_nMilSecTimeoutSend);
		if(m_nMilSecTimeoutRcv > 0)// 이게 타임아웃/응답없음 모두 영향 있네. 
			session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, m_nMilSecTimeoutRcv);
		if(m_nMilSecTimeoutConnect > 0)// 60sec
			session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, m_nMilSecTimeoutConnect);
#ifdef _before_edit
	// 		session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, m_nMilSecTimeout);
			// 		session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 3);
			if(m_nMilSecTimeoutRcv > 0)
			{
				//session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, m_nMilSecTimeoutRcv);//1000*60*10);
				session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, m_nMilSecTimeoutRcv);//1000*60*10);
			}
#endif // _before_edit
			pServer = session.GetHttpConnection(pszServerName, nPort);//	ThrowIfNull(pServer);

			_ReqProgress(eRsSetOp);
		}
		catch (CInternetException* e)
		{	//서버안열려있는 경우여기 안옴.
	//		pServer = NULL;		// 서버가 죽은 경우 여기를 지난다.
			e->GetErrorMessage(m_sError.GetBuffer(csz), csz);m_sError.ReleaseBuffer();
			KTrace(_T("CInternetException while GetHttpConnection %s"), m_sError);
			DelException(e);
			hr = -1;
			_Formatl(L"접속 오류.(%s)", m_sError);
			HttpLog(_s);
			throwLINE;
		}
		catch(CException* e)
		{
			e->GetErrorMessage(m_sError.GetBuffer(csz), csz);m_sError.ReleaseBuffer();
			DelException(e);
			throwLINE;
		}

		HttpLog(L"서버에 연결.");
		try
		{
			pFile = pServer->OpenRequest(m_nVerb, sUrlTail);
			_ReqProgress(eRsOpenReq);

			//         CHttpConnection::HTTP_VERB_POST     
			KTrace(_T("OpenRequest: %s%s\n"), (PWS)pszServerName, (PWS)sUrlTail);
	#ifdef _DEBUG
			_Formatl(L"데이터 요청.(%s)", sUrlTail);
	#else
			_Formatl(L"데이터 요청.");
	#endif // _DEBUG
			HttpLog(_s);
			//서버안열린경우도 여기 까지 온다.
			//pFile->AddRequestHeaders();
			//ThrowIfNull(pFile);//HTTP_VERB_GET
	//		CHttpFile* OpenRequest(int nVerb, LPCTSTR pstrObjectName, LPCTSTR pstrReferer = NULL, DWORD_PTR dwContext = 1, LPCTSTR* ppstrAcceptTypes = NULL, LPCTSTR pstrVersion = NULL, DWORD dwFlags = INTERNET_FLAG_EXISTING_CONNECT)
		
		}
		catch (CInternetException* e)//EndRequest
		{
			pFile = NULL;
			// CAutoPtrClose 가 있어서 안해도 된다.
// 			if(pServer)
// 			{
// 				pServer->Close();
// 				DeleteMeSafe(pServer);
// 			}
			e->GetErrorMessage(m_sError.GetBuffer(csz), csz);m_sError.ReleaseBuffer();
			KTrace(_T("CInternetException while OpenRequest %s"), m_sError);
			DelException(e);
			hr = e->m_dwError;//eHrErr_Timeout; 
			_Formatl(L"응답 오류.(%s)", m_sError);
			HttpLog(_s);
			throwLINE;
		}
		catch(CException* pe)
		{
// 			if(pServer)
// 			{
// 				pServer->Close();
// 				DeleteMeSafe(pServer);
// 			}
			pe->GetErrorMessage(m_sError.GetBuffer(csz), csz);m_sError.ReleaseBuffer();
			DelException(pe);
			_Formatl(L"일반 오류.(%s)", m_sError);
			HttpLog(_s);
			throwLINE;
		}






		ASSERT(!m_bEncode);// 지원 하지 않는다.
	/// //////////////////////////////////////////////////////////////////////////
		// 여기서 encode 한다.
		//KBinary binE;//, binD;
		//CXmlEncode xenc;
		//if(m_bEncode)
		//{
		//	CStringA key = xenc.CreatKey();
		//	xenc.EncodeData( (LPCSTR)pBin->m_p, pBin->m_len, key, binE ); // xml(utf8) => encode binary
		//	pBin = &binE;
		//}


		try
		{
	#ifdef _DEBUGx
			// AddRequestHeaders 사용예
			// 		pFile->AddRequestHeaders("Keep-Alive: 300",                       HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD);
			// 		pFile->AddRequestHeaders("Connection: keep-alive",                HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD);
			// 		pFile->AddRequestHeaders("Content-Length: 50",                    HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD);
			// 		pFile->AddRequestHeaders("user=uname&password=upass&submit=LogIn",HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD);

	=====================================
	POST /Default.aspx HTTP/1.1\r\n
		Accept: image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, 
		application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, applicati
	Referer: http://203.225.22.74/Default.aspx\r\n
	Accept-Language: ko\r\n
	User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0; InfoPath.2; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)\r\n
	Content-Type: application/x-www-form-urlencoded\r\n
	Accept-Encoding: gzip, deflate\r\n
	Host: 203.225.22.74\r\n
	Content length: 40
	Connection: Keep-Alive\r\n
	Cache-Control: no-cache\r\n
	\r\n
	=====================================
	L"Accept: text, */*\r\n"
	L"Accept-Language: ko\r\n"
	L"User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0; InfoPath.2; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)\r\n"
	L"Content-Type: application/x-www-form-urlencoded\r\n"
	L"http.accept_encoding == \"gzip, deflate\""
	L"Host: 203.225.22.74\r\n"
	L"Content length: 40"
	L"Connection: Keep-Alive\r\n"
	L"Cache-Control: no-cache\r\n"
	L"\r\n"
	//		L"image/gif, image/jpeg, image/pjpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-ms-application, application/x-ms-xbap, application/vnd.ms-xpsdocument, application/xaml+xml, * / *"
			hd.Format(L"Accept: TEXT */*\r\n"
				L"Content-Type:\ multipart/form-data;boundary=---------------------------7d13a23b368\r\n
				L"User-Agent: VCApp\r\n"
			//pFile->AddRequestHeaders(L"Accept: text/ *\r\n"
			//	L"Content-Type:\ multipart/form-data;boundary=---------------------------7d13a23b368\r\n
			//	L"User-Agent: VCApp\r\n"
			//	); 

	//		AddRequestHeaders("Accept: */*rn" )

	//		괄호 부분에 "User-Agent: Test....", "Content-type..." 이렇게도 있을 때 있고요.

	#endif // _DEBUG
	#ifdef _DEBUGx
			strHeaders.Format(//L"POST %s HTTP/1.1\r\n" // /Default.aspx
				L"Accept: text, * / *\r\n"
				L"Referer: http://%s%s\r\n"
				L"Accept-Language: ko\r\n"
				L"User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; Trident/4.0; InfoPath.2; .NET CLR 2.0.50727; .NET CLR 3.0.04506.648; .NET CLR 3.5.21022; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729)\r\n"
				L"Content-Type: application/x-www-form-urlencoded\r\n"
				L"Accept-Encoding: gzip, deflate\r\n"
				L"Host: %s\r\n"
				L"Content length: %d\r\n"
				L"Connection: Keep-Alive\r\n"
				L"Cache-Control: no-cache\r\n"
				L"\r\n", 
				//sUrlTail, 
				pszServerName, sUrlTail, pszServerName, pBin->m_len);
				//m_sError	"The requested header was not found

	#endif // _DEBUGx
			if(KwOp(ePtForm))
			{
				CString strHeaders;
				strHeaders.Format(L"Content-Type: application/x-www-form-urlencoded");
				if(m_header.GetLength())
				{
					if(strHeaders.Right(2) != L"\r\n")
						strHeaders += L"\r\n";
					strHeaders += m_header;
				}
				BOOL b = pFile->SendRequest(strHeaders, (LPVOID)pBin->m_p, (DWORD)pBin->m_len);
				_ReqProgress(eRsSendReq);
			}
			else if(KwOp(ePtBinary))
			{
				CString strHeaders;
				strHeaders.Format(L"Content-Type: application/binary");
				if(m_header.GetLength())
				{
					if(strHeaders.Right(2) != L"\r\n")
						strHeaders += L"\r\n";
					strHeaders += m_header;
				}
				BOOL b = pFile->SendRequest(strHeaders, (LPVOID)pBin->m_p, (DWORD)pBin->m_len);
				_ReqProgress(eRsSendReq);
			}
			else
			{
				if(m_header.GetLength())
				{
					CString strHeaders;
					strHeaders += m_header;
					BOOL b = pFile->SendRequest(strHeaders, (LPVOID)pBin->m_p, (DWORD)pBin->m_len);
					_ReqProgress(eRsSendReq);
				}
				else
				{
					BOOL b = pFile->SendRequestEx((DWORD)pBin->m_len, HSR_INITIATE);// 서버가 죽은 경우 여기 서 exception발생
					_ReqProgress(eRsSendReq);
					pFile->Write((const void*)(LPCSTR)pBin->m_p, (DWORD)pBin->m_len);   
					_ReqProgress(eRsWrite);//여기 까지온 경우 적어도 서버는 살아 있다는 의미.
					pFile->EndRequest();/// 응답지연 될 경우 여기서 멈춰 있다.
					_ReqProgress(eRsEndReq);
				}
			}
			CString sb1;sb1.Format(L"Sent %u bytes", (DWORD)pBin->m_len);
			HttpLog(sb1);

			HttpLog(L"다운로드 시작...");
			KTrace(_T("%s\n"), (PWS)sb1);

			pFile->QueryInfoStatusCode(dwRet);
			m_dwRet = dwRet;
			m_sReturn = HttpErrorStr(m_dwRet);

			if (dwRet == HTTP_STATUS_OK || dwRet == HTTP_STATUS_CREATED 
				|| dwRet == HTTP_STATUS_ACCEPTED 
				|| dwRet == HTTP_STATUS_BAD_REQUEST 
				|| dwRet == HTTP_STATUS_NONE_ACCEPTABLE)
			{ // HTTP_STATUS_CREATED 는 우이사 톰켓서버랑 카드취소시 리턴된 거
				CBufArchive arc; 
				//arc.m_nGrowSize = m_nGrowSize;
				UINT urTotal = 0;
				KBinary bbuf;
				bbuf.Alloc(eBUFSIZE);

				while (1)
				{
					UINT ur = pFile->Read(bbuf.m_p, eBUFSIZE);
					if(ur == 0)
						break;
					_ReqProgress(eRsReadResp);
					urTotal += ur;

					CString sb;
					if(urTotal < 1000)
						sb.Format(L"Recved. %u Bytes", urTotal);
					else
						sb.Format(L"Recved. %u KBytes", urTotal/1000);
					HttpLog(sb, IProgressHttp::ePhStatic);

					arc.Write(bbuf.m_p, ur);
				}

				//arc.Terminate(); Write에서 한다.
				UINT len = (DWORD)arc.GetCurPos();
				if(len > 0)
				{
					//if(m_bEncode && xencR.CheckEncodeData((LPCSTR)arc.GetPtr(), arc.GetLength()))
					//{	CXmlEncode xencR;
					//    pBinr->SetPtr((LPSTR)xencR.m_bin.GetPA(), xencR.m_bin.m_len);
					//}
					//else		
						pBinr->Attach((LPSTR)arc.Detach(), len);//복사 하는데 Detach 하면 안되지.
				}
			}
			else if(dwRet == HTTP_STATUS_NOT_FOUND)
			{
				HttpLog(L"해당 파일 없음.");
				_break;
			}
			else if(dwRet == HTTP_STATUS_BAD_REQUEST)
			{
				HttpLog(L"전송 오류.");
				_break;
			}
			else if(dwRet == HTTP_STATUS_SERVER_ERROR)
			{
				HttpLog(L"전송 오류.");
				_break;
			}
		}
		catch (CInternetException* e)
		{ // 서버가 죽은(안열린) 경우 여기를 지난다.
			DelException(e);
			// http://support.microsoft.com/kb/193625/ko
			try
			{
				e->GetErrorMessage(m_sError.GetBuffer(csz), csz);m_sError.ReleaseBuffer();
				//12029 ERROR_INTERNET_CANNOT_CONNECT     The attempt to connect to the server failed.
				//12030 ERROR_INTERNET_CONNECTION_ABORTED The connection with the server has been terminated.
				KTrace(L"CInternetException %s, %u", (m_sError), e->m_dwError);
				hr = e->m_dwError;//eHrErr_Timeout;  +	12002	m_sError	"작업 시간을 초과했습니다.
				_Formatl(L"서버 응답 없음.(%s)", m_sError);
				HttpLog(_s); // 작업이 취소되었습니다.(외부에서 CHttpConnection.Close할 경우)
			}
			catch (CException* )
			{
			}
			throwLINE;
		}
		catch (CMemoryException* e)	
		{
			e->GetErrorMessage(m_sError.GetBuffer(csz), csz);m_sError.ReleaseBuffer();
			DelException(e);hr = -1;
			_Formatl(L"MemoryException.(%s)", m_sError);
			HttpLog(_s);
			throwLINE;
		}
		catch (CFileException* e)
		{
			e->GetErrorMessage(m_sError.GetBuffer(csz), csz);m_sError.ReleaseBuffer();
			DelException(e);hr = -2;
			_Formatl(L"FileException.(%s)", m_sError);
			HttpLog(_s);
			throwLINE;
		}
		catch(CException* pe)
		{
			pe->GetErrorMessage(m_sError.GetBuffer(csz), csz);m_sError.ReleaseBuffer();
			DelException(pe);
			_Formatl(L"Exception.(%s)", m_sError);
			HttpLog(_s);
			throwLINE;
		}
		catch(int ln)
		{
			hr = ln;
			throwLINE;
		}
		catch(...)
		{
			_break;
			throwLINE;
		}
	}// remove throwLINE
	catch(int line) // goto EndLabel 대치
	{
		line;// to return with hr
		_break;//throwLINE;
	}
	/// //////////////////////////////////////////////////////////////////////////

//EndLabel:

	if(pFile)
	{
		pFile->Close();
		DeleteMeSafe(pFile);
	}
	return hr;
}

// Usage:
// {
// 	//http://www.onstylei.com/prk/prk_list.sty?menu=13&pg=1&ipp=10
// 	KBinary bin;
// 	phc->RequestGet(L"www.onstylei.com", 80, L"/prk/prk_list.sty?menu=13&pg=1&ipp=10", &bin);
// }


hres CHttpClient::RequestGet( LPCTSTR pszServerName, INTERNET_PORT nPort, LPCTSTR sUrlTail, KBinary* pBinr, UINT szBuf)
{
	hres hr = S_OK;
	CInternetSession session(m_sSession,1,PRE_CONFIG_INTERNET_ACCESS, NULL,NULL, INTERNET_FLAG_DONT_CACHE);

	CHttpConnection* pServer = NULL;
	//CAutoPtrClose<CHttpConnection> _pServer(pServer);//Warning: Disconnecting CInternetConnection handle 00CC0008 in context 00000001 at destruction.
	KAtEnd d_pServer([&]() {
		if(pServer)
			pServer->Close();
	});

	CHttpFile* pFile = NULL;
	//CAutoPtrClose<CHttpFile> _pFile(pFile);
	//		CHttpFile CInternetFile::AssertValid 에서 오류 dwResult == INTERNET_HANDLE_TYPE_HTTP_REQUEST
	KAtEnd d_pFile([&]() {
		if(pFile)
			pFile->Close();
	});
	try
	{
		CString strServerName;
		DWORD dwRet = 0;
		session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 3);

		if(m_nMilSecTimeoutResolve > 0)// 60sec
			session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, m_nMilSecTimeoutResolve);//5sec

		if(m_nMilSecTimeoutSend > 0)// 60sec
			session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, m_nMilSecTimeoutSend);
		if(m_nMilSecTimeoutRcv > 0)// 이게 타임아웃/응답없음 모두 영향 있네. 
			session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, m_nMilSecTimeoutRcv);
		if(m_nMilSecTimeoutConnect > 0)// 60sec
			session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, m_nMilSecTimeoutConnect);
		
// 			session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, m_nMilSecTimeout);
// 			session.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, m_nMilSecTimeoutRcv);

		pServer = session.GetHttpConnection(pszServerName, nPort);	ThrowIfNull(pServer);
		if ((HINTERNET)(*pServer) != NULL)
		{
			//if(pServer)
			//	_pServer.SetClose(true);
			DWORD dw = AfxGetInternetHandleType((HINTERNET)(*pServer));
			if(dw != INTERNET_HANDLE_TYPE_CONNECT_HTTP)
				throw -10;
		}

		pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET, sUrlTail); 	ThrowIfNull(pFile);//HTTP_VERB_GET

		DWORD dwResult = AfxGetInternetHandleType((HINTERNET)(*pFile));
		if (pFile->IsKindOf(RUNTIME_CLASS(CHttpFile)))
		{
			if(dwResult != INTERNET_HANDLE_TYPE_HTTP_REQUEST)
				throw -12;
		}

		pFile->SendRequest();

		pFile->QueryInfoStatusCode(dwRet);
		if (dwRet == HTTP_STATUS_OK || dwRet == HTTP_STATUS_CREATED 
			|| dwRet == HTTP_STATUS_ACCEPTED 
			|| dwRet == HTTP_STATUS_BAD_REQUEST 
			|| dwRet == HTTP_STATUS_NONE_ACCEPTABLE)
//		if (dwRet == HTTP_STATUS_OK || dwRet == HTTP_STATUS_CREATED )//HTTP_STATUS_SERVER_ERROR        500 // internal server error
		{//HTTP_STATUS_NOT_FOUND 이면 경로가 바뀐거다. directory가
			//_pFile.SetClose();
			const int csz = 10240;//40960 해봤자.. 최대 8992

			CBufArchive arc;
			//arc.m_nGrowSize = szBuf;

			auto sbuf = new shared_ptr<char>(new char[csz]);
			CHAR *szBuff = sbuf->get();
			//CAutoFreePtr<CHAR> au(szBuff);
			
			while (1)
			{
				UINT ur = pFile->Read(szBuff, csz);
				if(ur == 0)
					break;
				arc.Write(szBuff, ur);
			}

			arc.Terminate();
			UINT len = (DWORD)arc.GetCurPos();
			//KTrace(L"===============Total len: %u\n", len);
			if(len > 0 && arc.GetPtr()) //
				pBinr->Attach((LPSTR)arc.Detach(), len);
		}
	}
	catch (CInternetException* e)
	{
		//catch errors from WinInet//AfxMessageBox(szError);  dwError:12002 작업 시간을 초과했습니다.
		e->GetErrorMessage(m_sError.GetBuffer(1024), 1024);m_sError.ReleaseBuffer();
		KTrace2(L"CInternetException %d, %s\n", e->m_dwError, m_sError); 
		m_dwError = e->m_dwError;
		hr = -1;
		DelException(e);
	}
	catch (CException* e)
	{
		DelException(e);
		hr = -2;
	}
	catch(int ln)
	{
		hr = ln;
	}
	if(pServer)
	{
		pServer->Close();
		delete pServer;
	}

	session.Close();

	return hr;
}


//CS_STATIC_INIT3(CHttpClient,CsHttpSSL);

// http://www.getcodesamples.com/src/E9D9BFB8
hres CHttpClient::RequestGetSSL( LPCTSTR surl, KBinary* pBinr, KBinary* pBin, PAS sMethod, std::map<string, string>* httpHdr)
{
	hres hr = S_OK;
	//CS_STATIC_LOCK5(CsHttpSSL); // == SynchLockP(CsHttpSSL());
#ifdef _Use_SSL_Http
	try
	{
		CAutoCoInit _auco;/// 이거 안해줘서 thread 에서 안불려 짐
		// http://stackoverflow.com/questions/1107862/http-client-example-on-win32
		HRESULT hr;
		CComPtr<IXMLHTTPRequest> request;
		for(int n = 1;n>0;n--)
		{
			hr = request.CoCreateInstance(CLSID_XMLHTTP60);
			//if(request.p == NULL)
			//	hr = request.CoCreateInstance(CLSID_XMLHTTP40);
			//if(request.p == NULL)
			//	hr = request.CoCreateInstance(CLSID_XMLHTTP30);
			//if(request.p == NULL)
			//	hr = request.CoCreateInstance(CLSID_XMLHTTP26);
			//if(request.p == NULL)
			//	hr = request.CoCreateInstance(CLSID_XMLHTTP);
			if(request.p == NULL)
			{
				Sleep(10);///?중요 CHttpClient.RequestPostSSL.try 에 MTA 설명
				if(n == 1)/// CAutoCoInit 빠트림
					return -1000;
				continue;
			}
			//request-> setTimeouts(3000, 3000, 3000, 3000);
			hr = request->open(
				_bstr_t(sMethod), //Method "GET", "POST"
				_bstr_t(surl), //"https://maps.googleapis.com/maps/api/geocode/xml?address=%EC%9D%B4%EC%88%98&sensor=false&key=AIzaSyBSHdO1t0z0XWsEVBrhTMgnubnSDYnadWs"),
				_variant_t(VARIANT_FALSE),//Async
				_variant_t(),            // User
				_variant_t());          //Password

			n = 0;
			//https://developer.mozilla.org/ko/docs/Web/HTTP/Headers/Cache-Control
				//request->setRequestHeader(_bstr_t("Cache-Control"), _bstr_t("max-age=2")); //seconds
			if(httpHdr)
			{
				for(auto kv : *httpHdr)
				{
					request->setRequestHeader(_bstr_t(kv.first.c_str()), _bstr_t(kv.second.c_str()));
				}
				//request->setRequestHeader(_bstr_t("Cache-Control"), _bstr_t("no-cache"));
			}
			if(tchsame(sMethod, "GET"))
			{
				//이거 해도 안되네. URL 뒤에 파라미터라도 조금씩 바꿔야
				//이건 그냥 http header에 추가 되어서, 서버 쪽에서 참고 하는듯
				hr = request->send(_variant_t());
			}
			else if(tchsame(sMethod, "POST"))
			{
				// http://stackoverflow.com/questions/17629266/byte-array-to-variant
				_variant_t var;
				var.vt = VT_ARRAY | VT_UI1;
				SAFEARRAYBOUND rgsabound[1];
				rgsabound[0].cElements = (ULONG)pBin->m_len;
				rgsabound[0].lLbound = 0;
				var.parray = SafeArrayCreate(VT_UI1,1,rgsabound);
				void * pArrayData = NULL;    
				SafeArrayAccessData(var.parray,&pArrayData);
				memcpy(pArrayData, pBin->m_p, pBin->m_len); 
				SafeArrayUnaccessData(var.parray);
				hr = request->send(var);
			}

			// get status - 200 if succuss
			long status;
			hr = request->get_status(&status);
			m_dwRet = status;
			// load image data (if url points to an image)
			VARIANT responseVariant;
			hr = request->get_responseStream(&responseVariant);
			IStream* stream = (IStream*)responseVariant.punkVal;
			KAtEnd d_stream([&]() {	if(stream)
						stream->Release();
			});

			if(stream == NULL) // www.google.com 으로 GET 했더니 NULL이 온다.
				break;

			CBufArchive arc;
			//arc.m_nGrowSize = szBuf;

			const int csz = 10240;//40960 해봤자.. 최대 8992
			CHAR *szBuff = new CHAR[csz];
			//CAutoFreePtr<CHAR> au(szBuff);
			KAtEnd d_szBuff([szBuff]() {
				if(szBuff)
					delete szBuff;
			});

			while (1)
			{
				ULONG ur = 0;
				hr = stream->Read(szBuff, csz, &ur);
				if(ur < 1)
					break;
				arc.Write(szBuff, ur);
			}

			arc.Terminate();
			UINT_PTR len = arc.GetCurPos();
			if(len > 0 && arc.GetPtr()) //
				pBinr->Attach((LPSTR)arc.Detach(), len);

			//stream->Release();
			break;
		}
	
	}
	catch (CInternetException* e)
	{
		//catch errors from WinInet
		e->GetErrorMessage(m_sError.GetBuffer(1024), 1024);m_sError.ReleaseBuffer();
		//AfxMessageBox(szError);  dwError:12002
		KTrace(L"CInternetException %s\n", (m_sError));
		hr = -1;
		DelException(e);
	}
	catch (CException* e)
	{
		DelException(e);
		hr = -2;
	}
	catch(int ln)
	{
		hr = ln;
	}
#endif // _Use_SSL_Http
	return hr;
}

#pragma comment (lib, "msxml6.lib")
#ifdef _DEBUG
#pragma comment (lib, "comsuppwd.lib")
#else
#pragma comment (lib, "comsuppw.lib")
#endif // _DEBUG

int CHttpClient::RequestPostSSL(LPCTSTR surl, KBinary* pBinr, KBinary* pBin, std::map<string, string>* httpHdr)
{
	return RequestPostSSL(surl, pBinr, pBin->m_p, (DWORD)pBin->m_len, httpHdr);
}
//int CHttpClient::RequestPostSSL(LPCTSTR surl, KBinary* pBinr, LPCSTR buf, UINT szBuf, CKRbVal<CStringW, CStringW>* httpHdr)
int CHttpClient::RequestPostSSL( LPCTSTR surl, KBinary* pBinr, LPCSTR buf, UINT szBuf, std::map<string,string>* httpHdr)
{
	int hr = S_OK;
	CString sUrl(surl);
	BOOL bData = buf != NULL && szBuf > 0;
#ifdef _Use_SSL_Http
	try
	{
		CAutoCoInit _auco(COINIT_APARTMENTTHREADED, !_coInit.m_bInit);
		// http://stackoverflow.com/questions/1107862/http-client-example-on-win32
		//https://msdn.microsoft.com/en-us/library/cc507432(v=vs.85).aspx
		//https://www.microsoft.com/ko-KR/download/details.aspx?id=3988 msxml6.msi
		// http://stackoverflow.com/questions/12487282/com-object-methods-are-not-executed-on-the-thread-that-coinitialize-d-and-create
		// MTA/STA multithread 지원. 
		// UI thread 에서 CoInitialize 했다면, STA 모델의 경우 다른 thread에서 CoCreateInstance 안된다.
		/// MTA 로 하려면 HKEY_LOCAL_MACHINE\SOFTWARE\Classes\CLSID\{Your CLSID}\InprocServer32@ThreadingModel=Apartment 인데... ?
		// Apartment 로 설정 해 줘야 
		// CAutoCoInit 로 어디선가 CoInitializeEx 를 불러야 한다.
		HRESULT hr;
		//CComPtr<IXMLHTTPRequest> request;
		if(!_sxrq)
			hr = _sxrq.CoCreateInstance(CLSID_ServerXMLHTTP60);//CLSID_XMLHTTP60
		CComPtr<IServerXMLHTTPRequest> request(_sxrq);
		//hr = request.CoCreateInstance(CLSID_ServerXMLHTTP60);//CLSID_XMLHTTP60
		//if(request.p == NULL)
		//	hr = request.CoCreateInstance(CLSID_XMLHTTP40);
		//if(request.p == NULL)
		//	hr = request.CoCreateInstance(CLSID_XMLHTTP30);
		//if(request.p == NULL)
		//	hr = request.CoCreateInstance(CLSID_XMLHTTP26);
		//if(request.p == NULL)
		//	hr = request.CoCreateInstance(CLSID_XMLHTTP);
		if(request.p == NULL)
		{
			//AfxMessageBox(L"msxml6.msi 설치해라");
			throw __LINE__;
		}
		//                dns->ip   socket에 연결     보내는                 첫 받음(데이터 아님)
#ifdef _DEBUGx
		request->setTimeouts(1000, 7000, 30000, 30000);
#else
		request->setTimeouts(m_nMilSecTimeoutResolve, m_nMilSecTimeoutConnect, m_nMilSecTimeoutSend, m_nMilSecTimeoutRcv);
#endif // _DEBUG
		/// 서버에 접속이 안되면 m_nMilSecTimeoutResolve + m_nMilSecTimeoutConnect 
		/// 성공 했을때: 94, 129, 
		/// 실패: 1000, 2000 일때 3031 만에 실패 응답 했다.
		/// 실패: 1000, 5000 일때 8000 만에 실패 응답 했다.
		/// 실패: 2000, 2000 일때 4485 만에 실패 응답 했다.
		/// 실패: 1000, 7000 일때 6828 만에 실패 응답 했다.
		/// 결론: 접속 안되면, resolve + connect + a 정도에 응답 한다.
		hr = request->open(
			_bstr_t("POST"), //Method "GET", "POST"
			_bstr_t(sUrl), //"https://maps.googleapis.com/maps/api/geocode/xml?address=%EC%9D%B4%EC%88%98&sensor=false&key=AIzaSyBSHdO1t0z0XWsEVBrhTMgnubnSDYnadWs"),
			_variant_t(VARIANT_FALSE),//Async
			_variant_t(),            // User
			_variant_t());          //Password
// 		var oReq = new XMLHttpRequest();
// 		oReq.open("POST", sURL, false);
// 		oReq.setRequestHeader("Content-Type", "text/xml");
// 		oReq.send(sRequestBody);
		if(httpHdr)
		{
			for(auto kv : *httpHdr)
			{
				request->setRequestHeader(_bstr_t(kv.first.c_str()), _bstr_t(kv.second.c_str()));
			}

			//CKRbVal<CStringW, CStringW>& lst = *httpHdr;
			//for(POSITION _pos = (lst).GetHeadPosition();_pos;)
			//{
			//	(lst).GetNext(_pos);//	for_list0(*httpHdr)
			//	//next_list0(*httpHdr);
			//	const CStringW& key = lst.GetKeyAt(_pos);
			//	CStringW& val = lst.GetValueAt(_pos);
			//	request->setRequestHeader((BSTR)(PWS)key, (BSTR)(PWS)val);
			//}
		}
		BSTR bstr1 = nullptr;
		request->getResponseHeader(L"Cache-Control", (BSTR*)&bstr1);

// 		else // Conformance mode : No 해야 됨
// 			request->setRequestHeader(L"Content-type", L"application/json");
		//error C2664: 'HRESULT IXMLHTTPRequest::setRequestHeader(BSTR,BSTR)': cannot convert argument 1 from 'const wchar_t [13]' to 'BSTR'
	//request->setRequestHeader(L"Content-type", L"application/x-www-form-urlencoded");
		//request->setRequestHeader(L"Content-type", L"text/xml");
		//request->setRequestHeader(_bstr_t("Cache-Control"), _bstr_t("no-cache"));
		CString s;
		CString smg;
		//if(strcmp(sMethod, "GET") == 0)
		//	hr = request->send(_variant_t());
		//else if(strcmp(sMethod, "POST") == 0)
		{
			LONGLONG tk = GetTickCount64();
			if(!bData)
				hr = request->send(_variant_t()); // S_OK
			else
			{
				// http://stackoverflow.com/questions/17629266/byte-array-to-variant
				_variant_t var;
				var.vt = VT_ARRAY | VT_UI1;
				SAFEARRAYBOUND rgsabound[1];
				rgsabound[0].cElements = szBuf;
				rgsabound[0].lLbound = 0;
				var.parray = SafeArrayCreate(VT_UI1,1,rgsabound);
				void * pArrayData = NULL;    
				SafeArrayAccessData(var.parray, &pArrayData);
				memcpy(pArrayData, buf, szBuf); 
				SafeArrayUnaccessData(var.parray);

				hr = request->send(var);
				/// WinRT originate error - 0x80072F45 : '전송 메서드가 호출되기 전에는 이 메서드를 호출할 수 없습니다.'
			}
			LONGLONG msel = GetTickCount64() - tk;

			// 0x800C0008 is INET_E_DOWNLOAD_FAILURE The download has failed (the connection was interrupted).
			m_dwRet = hr;
// 			s.Format(L"%#x", hr);// 			smg += s;
		}

		// get status - 200 if succuss
		long status = 0;
		hr = request->get_status(&status);//status == 200, 404(not found)
		_status = status; //m_dwRet S_OK . 서버 죽이면 12031
	
		s.Format(L" %ld", status);
		smg += s;
//		405 :message Request method 'GET' not supported
// 		HTTP 404 - Not Found(페이지가 아에 존재하지 않음)
// 		HTTP 405 - Method not allowed(페이지는 존재하나, 그걸 못보게 막거나 리소스를 허용안함)
// 		if(status != 200 && status != 404 && status != 405)
// 			throw __LINE__;
		// load image data (if url points to an image)
		VARIANT resvar;// response Variant;
		hr = request->get_responseStream(&resvar);		if(hr != S_OK)	throw __LINE__;

		IStream* stream = (IStream*)resvar.punkVal;	if(stream == NULL)	throw __LINE__;
		KAtEnd d_stream([&]() {	if(stream)	stream->Release();	});
		// www.google.com 으로 GET 했더니 NULL이 온다.

		const int csz = 10240;//40960 해봤자.. 최대 8992
 		CHAR *szBuff = new CHAR[csz + 1];
 		KAtEnd d_szBuff([szBuff]() { delete szBuff; });
		//std::shared_ptr<char> shbuf{ std::make_shared<char>(csz + 1) };
		// szBuff = shbuf.get();
#ifdef _DEBUG
		memset(szBuff, '_', csz);
		szBuff[csz] = '\0';
#else
		memset(szBuff, 0, csz + 1);
#endif // _DEBUG	

		CBufArchive arc;	//arc.m_nGrowSize = szBuf;
		ULONG ur = 0;
		do	{
			hr = stream->Read(szBuff, csz, &ur);
			if(ur == 0)
				break;
			arc.Write(szBuff, ur);
		} while(ur);

		arc.Terminate();
		UINT_PTR len = arc.GetCurPos();
		if(len > 0 && arc.GetPtr()) //
			pBinr->Attach((LPSTR)arc.Detach(), len);

		if(status != 200 //OK
			&& status != 404 //eHttp_Not_Found
			&& status != 405)//eHttp_Method_Not_Allowed
		{
			auto ln = __LINE__;
			throw ln;// release 에서는 LONG에 안걸리네.
		}
	}
	catch (CInternetException* e)
	{
		e->GetErrorMessage(m_sError.GetBuffer(1024), 1024); m_sError.ReleaseBuffer();
		KTrace(L"CInternetException %s\n", (m_sError));
		hr = -1;
		e->Delete();//DelException(e);
	}
	catch (CException* e)
	{
		hr = -2;
		e->Delete();//DelException(e);
	}
	catch(LONG ln) // __LINE__
	{
		hr = -3;
	}
	catch(int ln) // __LINE__
	{
		hr = -3;
	}
	catch(...) /// __LINE__이 catch LONG에서도 안걸리고, 부른 스레드 (...)으로 가버린다.
	{
		hr = -3;
	}
#else
	ASSERT(0); //_Use_SSL_Http
#endif // _Use_SSL_Http
	return hr;
}













hres CHttpClient::RequestFile( LPCTSTR pszServerName, INTERNET_PORT nPort, LPCTSTR sUrlTail, KBinary* pBinr)
{
	hres hr = S_OK;
#ifdef _DEBUGx
	CInternetSession session(m_sSession,1,PRE_CONFIG_INTERNET_ACCESS, NULL,NULL, INTERNET_FLAG_DONT_CACHE);

	//CHttpConnection* pServer = NULL;
	//CInternetConnection* pServer = NULL;
//	CAutoPtrClose<CHttpConnection> _pServer(pServer);

	CInternetFile* pFile = NULL;
	CAutoPtrClose<CInternetFile> _pFile(pFile);
	//		CHttpFile CInternetFile::AssertValid 에서 오류 dwResult == INTERNET_HANDLE_TYPE_HTTP_REQUEST
	try
	{
		CString strServerName;
		DWORD dwRet = 0;

//		pServer = session.GetHttpConnection(pszServerName, nPort);	ThrowIfNull(pServer);
		pFile = pServer->OpenUrl(CHttpConnection::HTTP_VERB_GET, sUrlTail); 	ThrowIfNull(pFile);//HTTP_VERB_GET

// 		pFile->SendRequest();
// 		pFile->QueryInfoStatusCode(dwRet);
		//if (dwRet == HTTP_STATUS_OK)
		{
			_pFile.SetClose();
			CBufArchive arc;
			const int csz = 1024;
			CHAR szBuff[csz];
			while (1)
			{
				UINT ur = pFile->Read(szBuff, csz);
				//KTrace(L"%s\n", szBuff);
				if(ur == 0)
				{
					break;
				}
				arc.Write(szBuff, ur);
			}

			UINT len = arc.GetCurPos();
			pBinr->Attach((LPSTR)arc.Detach(), len);
		}
	}
	catch (CInternetException* e)
	{
		//catch errors from WinInet
		DelException(e);
		TCHAR pszError[64];
		e->GetErrorMessage(pszError, 64);
		KTrace(_T("CInternetException %s"), pszError);
		hr = -1;
	}
	catch(int ln)
	{
		hr = ln;
	}
	session.Close();

#endif // _DEBUGx
	return hr;
}

hres CHttpClient::LoadFile(const TCHAR* strURL, KBinary* pBinr, IDownload* pii)
{
	CWaitCursor _wc;
	//KTrace(L"%s\n", strURL);
	CInternetSession session(NULL,1,PRE_CONFIG_INTERNET_ACCESS, NULL,NULL, INTERNET_FLAG_DONT_CACHE);	
	CInternetFile* pInternetFile = NULL;	
	try 
	{
		session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 120000);
		session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 3);
		pInternetFile = (CInternetFile*) session.OpenURL(strURL);
	} 
	catch (CInternetException* e) 
	{		
		e->GetErrorMessage(m_sError.GetBuffer(1024), 1024);m_sError.ReleaseBuffer();
		//AfxMessageBox(szError);
		pInternetFile = NULL;
		DelException(e);
		return -1;
	}	
	if(pInternetFile == NULL) return false;
	int data_size = 0;

	CBufArchive arc;
	const int csz = 4096;
	CHAR szBuff[csz];
	long szDown = 0;
	while (1)
	{
		UINT ur = pInternetFile->Read(szBuff, csz);
		//KTrace(L"%s\n", szBuff);
		if(ur == 0)
			break;
		arc.Write(szBuff, ur);
// <!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
// <html><head>
// <title>404 Not Found</title>
// </head><body>
// <h1>Not Found</h1>
// <p>The requested URL /btn2/center/160722/BtnStart.exe.new was not found on this server.</p>
// </body></html>
		szDown += ur;
		if(pii)
			pii->_Downloading(strURL, szDown, ur);
	}
	arc.Terminate();
	UINT_PTR len = arc.GetCurPos();
	pBinr->Attach((LPSTR)arc.Detach(), len);

	pInternetFile->Close();

	delete pInternetFile;
	pInternetFile = NULL;
	return S_OK;
}

hres CHttpClient::LoadImage(PWS strURL, PWS sLocalFolder, OUT CString& sFile)
{
	
	CString siu0;
	CString sURL = strURL;
	if(sURL.GetLength() > 0)
	{

		int i0 = sURL.ReverseFind('/');
		siu0 = sURL.Mid(i0+1);
		if(siu0.IsEmpty())
			return -10;
		CString sloc = sLocalFolder;
		ASSERT(sloc.GetAt(sloc.GetLength()-1) == '\\');
		sloc += siu0;//L"temp\\" 폴더변경하면LoadImageFile 때폴더못찾는다.

		HRESULT hr = LoadImage(sURL, sloc);//bini);
		// http://image.onstylei.com/uploadfiles/service/mobile/file6601543139286127651.jpg
		if(hr == S_OK)
		{
			sFile = sloc;
			return hr;
		}
	}
	return -1;
}
hres CHttpClient::LoadImage(const TCHAR* strURL, const TCHAR* strSavePath)
{
	CWaitCursor _wc;
	//KTrace(L"\t%s\n", strURL);
	CInternetSession session(NULL,1,PRE_CONFIG_INTERNET_ACCESS, NULL,NULL, INTERNET_FLAG_DONT_CACHE);	
	CInternetFile* pInternetFile = NULL;	
	try {
		session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 50000);
		session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 2);
		pInternetFile = (CInternetFile*) session.OpenURL(strURL);
	} 
	catch (CInternetException* e) 
	{		
		e->GetErrorMessage(m_sError.GetBuffer(1024), 1024);m_sError.ReleaseBuffer();
		//AfxMessageBox(szError);
		pInternetFile = NULL;
		DelException(e);
		return -1;
	}
	catch(CException* e)
	{
		DelException(e);
		return -2;
	}
	catch(...)
	{
		return -3;
	}
	if(pInternetFile == NULL) 
		return false;

	BYTE* data = new BYTE[10240];
	int data_size = 0;

	KwFileSafeRemove(strSavePath);

	//FILE* fp = _tfopen(strSavePath, _T("w+b"));
	FILE* fp = NULL;
	errno_t err = _wfopen_s(&fp, strSavePath, L"w+b");
	
	bool bOK = true;
	while((data_size = pInternetFile->Read(data, 1024)) != 0) 		
	{
		if(tchstri((const char*)data, "<!DOCTYPE HTML"))
		{
			bOK = false;
			break;
		}
		fwrite(data, sizeof(unsigned char), data_size, fp);
	}
//<!DOCTYPE HTML PUBLIC "-//IETF//DTD HTML 2.0//EN">
//<HTML><HEAD>
//<TITLE>403 Forbidden</TITLE>
//</HEAD><BODY>
//<H1>Forbidden</H1>
//You don't have permission to access /attfiles//
//on this server.<P>
//<HR>
//<ADDRESS>Apache/1.3.37 Server at kway.koreab2b.com Port 80</ADDRESS>
//</BODY></HTML>
	fclose(fp);
	pInternetFile->Close();
	
	DeleteMeSafe(data);
	DeleteMeSafe(pInternetFile);

	if(!bOK)
		KwFileSafeRemove(strSavePath);
	return bOK ? S_OK : -10;
}

void CHttpClient::HttpLog( PWS sLog, int iOp )
{
	if(m_pPrgress == NULL)
		return;

	m_pPrgress->_UpdateProgressLog(sLog, iOp);
}


void CHttpClient::Reserved(PWS id)
{
	CString tmp;
	try
	{
		//CDllTest3
	}
	catch (CException* )
	{
	}

	_break;
}

// CHttpClient* CHttpClient::CreateHostApp( CHttpClient* pht, PWS func , LPARAM lp)
// {
// 	int port = 0;
// 	CString svr, sUri;
// //	FillHostInfo_(svr, sUri, port);
// 
// 	if(pht == NULL)
// 		pht = CreateHttpClient(lp);
// 	pht->InitHost(func, svr, sUri, port);//?zzz
// 	return pht;
// }
// 
// 
// 
/*
int CHttpClient::ReverseGeocode(cxaf* phsr, CString sRef, PWS apikey)
{
// 	m_sRefId = sRef;
// 	CHtClPush ht;

	try
	{
		CString sUrl;
#ifdef _DEBUGx
https://maps.googleapis.com/maps/api/place/details/json?reference=CoQBfAAAAPNJju6I_TAFDZLBeHuHsS4ZbBV4dDeP4GQsbkTnR_LzQnxhehWY2_rRodE5Ovc9vWKgcYOy7tdonIIDAASOwsk8vtqSI1viegpeSdUd1OI6zil4LZ_4_7h4fsPKvHIJYzEwCJbVUp8AYX67keYjKcHDLk8kspr3gTIQ-0saUuTGEhB5kR0tuX-WJOKlnzjksME_GhQcTuZCuABBo5eDymBcjlxWQpWpww&sensor=true&key=AIzaSyCZd-byvvkZTQkateyWBO01Xb85--YoRxs
#endif // _DEBUG
		CString sURef;
		KwWcharToUTF8ToHtmlUrl(sRef, sURef);

//		sUrl.Format(L"http://maps.google.co.kr/maps/api/geocode/json?address=%s&sensor=true", sRef);
		sUrl.Format(L"https://maps.google.co.kr/maps/api/geocode/xml?address=%s&sensor=true&key=%s",sURef, apikey);
//			, m_sRefId,	CRingCenter1App::s_apiKey);       //&sensor=true&key=
		// RingCenter : AIzaSyCZd-byvvkZTQkateyWBO01Xb85--YoRxs

		KBinary binr;
		hres hr = this->RequestGetSSL(sUrl, &binr, 40000); 	CheckthrowHR;

		CStringW sWstr;
		KwUTF8ToWchar(binr.m_p, sWstr);

		CKwXml xml(sWstr);// = new CKwXml(srv.toString());
		int is = 0;
		CString s;
		if(xml.getContent(L"status", is, s))
		{
			if(s != L"OK")
				return -1;
		}
		is = 0;  // 다시 첨부터
		while(true)
		{
//			CXmlProj hs;
// 			if(!xml.getContent(L"name", is, s)) break;
// 			hs(f0420Title) = s;

			if(!xml.getContent(L"formatted_address", is, s)) break;
			PWS korea = L"대한민국 ";
			if(s.Find(korea) == 0)
			{
				s.TrimLeft(korea);
			}
			is = xml.goTagEnd(L"geometry", is);
			if(is < 0)
				break;
			is = xml.goTagEnd(L"location", is);
			if(is < 0)
				break;
			double lat = KwAtof(xml.getContent(L"lat", is, s));
			double lng = KwAtof(xml.getContent(L"lng", is, s));
			phsr->SetAt(L"t04position.f09Addr2", s);
			phsr->SetAt(L"t04position.f02x", lng);
			phsr->SetAt(L"t04position.f03y", lat);
			break;
		}

	}
	catch (CException* e)
	{
		DelException(e);
	}
	return 0;// m_ar.GetCount();
}*/

#ifdef _DEBUGx
{
   "results" : [
      {
         "address_components" : [
            {
               "long_name" : "장안1동",
               "short_name" : "장안1동",
               "types" : [ "political", "sublocality", "sublocality_level_2" ]
            },
            {
               "long_name" : "동대문구",
               "short_name" : "동대문구",
               "types" : [ "political", "sublocality", "sublocality_level_1" ]
            },
            {
               "long_name" : "서울특별시",
               "short_name" : "서울특별시",
               "types" : [ "locality", "political" ]
            },
            {
               "long_name" : "대한민국",
               "short_name" : "KR",
               "types" : [ "country", "political" ]
            },
            {
               "long_name" : "130-100",
               "short_name" : "130-100",
               "types" : [ "postal_code" ]
            }
         ],
         "formatted_address" : "대한민국 서울특별시 동대문구 장안1동",
         "geometry" : {
            "bounds" : {
               "northeast" : {
                  "lat" : 37.5726988,
                  "lng" : 127.0780531
               },
               "southwest" : {
                  "lat" : 37.5596501,
                  "lng" : 127.0580344
               }
            },
            "location" : {
               "lat" : 37.5677585,
               "lng" : 127.0663775
            },
            "location_type" : "APPROXIMATE",
            "viewport" : {
               "northeast" : {
                  "lat" : 37.5726988,
                  "lng" : 127.0780531
               },
               "southwest" : {
                  "lat" : 37.5596501,
                  "lng" : 127.0580344
               }
            }
         },
         "place_id" : "ChIJAZ_kgja7fDUR-4UBeKcEy7E",
         "types" : [ "political", "sublocality", "sublocality_level_2" ]
      }
   ],
   "status" : "OK"
}

#endif // _DEBUGx



#ifdef _Use_SockClient

CSockClient::CSockClient( PWS ip /*= L"localhost"*/, int port /*= 20001*/ ) : m_IP(ip)
	, m_port(port)
	, m_hErr(S_OK)
	, m_socket(INVALID_SOCKET)
	, m_lenHead(4)
	, m_spcLen('0')
{
	/// 1 start up.  Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h 
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData = {0,};
	int rv=0;
	rv = WSAStartup(wVersionRequested, &wsaData);
	// 		wVersion	514	unsigned short
	// 		wHighVersion	514	unsigned short
	// +	szDescription	0x0039ea78 "WinSock 2.0"	char [257]
	// +	szSystemStatus	0x0039eb79 "Running"	char [129]
	// 		iMaxSockets	0	unsigned short
	// 		iMaxUdpDg	0	unsigned short
	// +	lpVendorInfo	0x00000000 <Ptr>이 잘못되었습니다.	char *
	if (rv != 0) {
		_Formatl(L"WSAStartup failed: %d\n", rv);
		throwlinemsg(_s);
	}
}

CSockClient::~CSockClient()
{
	if(m_socket != INVALID_SOCKET)
		closesocket(m_socket);
	WSACleanup();
}

PWS CSockClient::_LengthFormat( int len /*= 0*/ )
{
	if(len == 0)
		len = m_lenHead;//초기값 4: 10 아니니 주의
	StrBuf(fmtLen);
	if(m_spcLen == ' ')
		fmtLen.Format(L"%%%dd", len); // %10d 앞이 공백으로 채워지는 문자열 GW java 앱과 통신할때 앞에 패킷 크기 약속
	else
		fmtLen.Format(L"%%0%dd", len); // %10d 앞이 공백으로 채워지는 문자열 GW java 앱과 통신할때 앞에 패킷 크기 약속
	return fmtLen;
}

void CSockClient::LenFormat( int len /*= 10*/, TCHAR spc /*= ' '*/ )
{
	ASSERT(spc == ' ' || spc == '0');
	m_lenHead = len;
	m_spcLen = spc;
}

// m_bin 을 보내고 m_binr 에 받는다.
int CSockClient::Request()
{
	hres hr = Connect();
	if(FAILED(hr))
		return -1;
	int rv = SendLen(m_bin.m_len);

	if(rv != SOCKET_ERROR)
		rv = Send(m_bin.m_p, m_bin.m_len);
	else
		return -10;

	hr = CloseSend();
	if(FAILED(hr))
	{
		//AfxMessageBox(L"CloseSend failed");
		rv = -20;
	}

	int len = RecvLen();
	if(len > 0)
	{
		m_binr.Alloc(len);
		Recv(m_binr.m_len, m_binr.m_p);
		CString sbufw(m_binr.m_p);
		_Formatl(L"%d bytes returned(%s...)\n", m_binr.m_len, sbufw.Left(40));
		KTrace2(_s);
	}
	return rv;
}

int CSockClient::Send( PAS buf, int len )
{
	ASSERT(m_socket != INVALID_SOCKET);
	int flag = 0;
	int rv = send( m_socket, (PAS)buf, len, flag );
	if (rv == SOCKET_ERROR) 
		_OnError(__LINE__, __FUNCTIONT__, L"Send Error", FMTS(L"%ld, len(%d)", WSAGetLastError(), len));
	return rv;
}

hres CSockClient::Connect()
{
	hres hr = S_OK;

	CStringA IP(m_IP);// = "localhost";
	CStringA port; port.Format("%d", m_port);

	struct addrinfo *result = NULL, *ptr = NULL, hints;
	ZeroMemory( &hints, sizeof(hints) );
	int rv=0;
//	int recvbuflen = DEFAULT_BUFLEN;

	CString sErr;
	try
	{
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		/// 2 Resolve the server address and port
		rv = getaddrinfo(IP, port, &hints, &result);
		if ( rv != 0 ) {
			_Formatl(L"getaddrinfo failed: %d\n", rv);
			throwlinemsg(_s);
		}

		/// 3 Attempt to connect to an address until one succeeds
		SOCKET sck = INVALID_SOCKET;
		for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) 
		{
			/// 3.1 Create a SOCKET for connecting to server
			sck = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);//1040, 
			if (sck == INVALID_SOCKET) {
				_Formatl(L"INVALID_SOCKET %ld\n", WSAGetLastError());
				throwlinemsg(_s);
			}
			/// 3.2 Connect to server.
			rv = connect( sck, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (rv == SOCKET_ERROR) 
			{
				closesocket(sck);
				sck = INVALID_SOCKET;
				_OnError(__LINE__, __FUNCTIONT__, L"connect", L"SOCKET_ERROR");
				continue;
			}
			break;
		}
		freeaddrinfo(result);		result = NULL;
		if (sck == INVALID_SOCKET) 
		{
			_Formatl(L"No Address to connect to server!");
			throwlinemsg(_s);
		}
		else
			m_socket = sck;
	}
	catch (CSrcException* e)
	{
		_OnError(e->m_iLine, __FUNCTIONT__, L"CSrcException", e->m_sErrorMsg);
		m_sErr = e->m_sErrorMsg;
		hr = -100;
	}
	catch (CException* e)
	{
		_Formatl(L"KMotionDec Send");
		CString s;e->GetErrorMessage(s.GetBuffer(400), 400);s.ReleaseBuffer();
		_OnError(__LINE__, __FUNCTIONT__, L"CSrcException", s);
		hr = -101;
	}
	return hr;
}

hres CSockClient::Close( int mode )
{
	// shutdown the connection since no more data will be sent
	int rv = shutdown(m_socket, mode);// SD_RECEIVE 더보낼게 SD_SEND 없으면 recv 하기도 전에 닫아도 되나?
	if (rv == SOCKET_ERROR) 
	{
		_OnError(__LINE__, __FUNCTIONT__, L"shutdown", FMTS(L"%s %d\n", 
			mode==SD_SEND ? L"SD_SEND":L"SD_RECEIVE", WSAGetLastError()));
		return -1;
	}
	return S_OK;
}

hres CSockClient::Recv( int lenToRd, LPSTR recvbuf )
{
	ASSERT(lenToRd > 0);
	CString rslt;
	// Receive data until the server closes the connection
	int rv = 0;
	int nMaxErr = 5;
	int lenRead = 0;
	while(1)
	{
		rv = recv(m_socket, recvbuf, lenToRd, 0);
		if (rv < 0)
		{
			nMaxErr--;
			_OnError(__LINE__, __FUNCTIONT__, L"CSockClient::Recv", FMTS(L"%d.%d\n", nMaxErr, WSAGetLastError()));
			if(nMaxErr == 0)
				break;//error 5번이면 포기
		}
		else if (rv > 0)
		{
			KTrace3(L"Bytes received: %d\n", rv);
			lenRead += rv;
			if(lenRead == lenToRd)
				break;//다읽어서 마침
		}
		else 
		{
			rslt.Format(L"Connection closed\n");
			nMaxErr = 0;
			break;
		}
	} //while (rv > 0 && nMaxErr > 0 || lenRead < lenToRd);
	if(lenRead == 0)
	{
		if(rv == 0)// closed
			return 1;
	}
	else if(lenRead == lenToRd)
		return S_OK;
	else if(lenRead < lenToRd)
		return 2;
	else if(lenRead == 0)// && rv < 0)
		return -1;
	//ASSERT(0);
	return S_OK;
}

int CSockClient::RecvLen()
{
	CString rslt;
	// Receive data until the server closes the connection
	int iResult = 0;
	CStringA sbuf;
	LPSTR recvbuf = sbuf.GetBuffer(m_lenHead);

	hres hr = Recv(m_lenHead, recvbuf);
	if(hr == S_OK)
	{
		sbuf.ReleaseBuffer();
		CString slen(sbuf);
		if(slen.GetLength() > 0)
			return KwAtoi(slen);
		else
			return 0;
	}
	return -1;
}

int CSockClient::SendLen( int len )
{
	CStringA fmtLen(_LengthFormat(m_lenHead));//; fmtLen.Format(L"%%%dd", s_sizeLen); // %10d
	CStringA slen;slen.Format(fmtLen, len); // "%4d"
	return Send((PAS)slen, m_lenHead);      // " 280"
}
#endif //_Use_SockClient