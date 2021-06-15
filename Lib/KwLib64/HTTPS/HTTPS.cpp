#include "pch.h"

#include "../ktypedef.h"

#ifdef _Use_HTTPS

#include "../KTempl.h"
//#include "../KObject.h"
#include "HTTPS.h"



#ifdef _WIN32_WCE
#include <Wininetui.h>
#endif // _WIN32_WCE

#define		BUFF_SIZE					102400
#define		MEMSIZE						0xFA000	// 1024 * 1000
#define		COMPRESS_TRADEOFF_SIZE	0x2000	// 8192
#define		ACCEPT_ENCODING_HEADER _T("Content-Type: application/octet-stream")


/**
 * CHTTPS::CHTTPS
 * HTTP 함수 초기화 
 * Dependencies : None
 * Side Effects : None
 * 
 * @return			
 * @exception		
 * @throws		
 */
CHTTPS::CHTTPS():
	m_lpRecvBuff(NULL),
	m_nRecvSize(0),
	m_lpRequestBuff(NULL),
	m_nRequestSize(0),
	m_nRecvTotalSize(0),
	m_bAbort(false),
	m_bHttps(false),
	m_pThread(NULL),
	m_hHttpConnection(NULL),
	m_hInternetSession(NULL),
	m_hHttpFile(NULL)
{
}

/**
 * CHTTPS::~CHTTPS
 * HTTP 함수 소멸
 * Dependencies : None
 * Side Effects : None
 * 
 * @param		void	
 * @return			
 * @exception		
 * @throws		
 */
CHTTPS::~CHTTPS(void)
{
	if (m_pThread)
	{
		WaitForSingleObject(m_pThread->m_hThread, INFINITE);
		delete m_pThread;
		m_pThread = NULL;
	}

	if (NULL != m_lpRecvBuff) {
		HANDLE hHeap = GetProcessHeap();
		HeapFree (hHeap, 0, m_lpRecvBuff);
		m_lpRecvBuff = NULL;
		m_nRecvSize = 0;

		m_lpRecvBuff = NULL;
		m_nRecvSize = 0;
	}

	if (NULL != m_lpRequestBuff) {
		free(m_lpRequestBuff);
		m_lpRequestBuff = NULL;
		m_nRequestSize = 0;
	
		m_lpRequestBuff = NULL;
		m_nRequestSize = 0;
	}

	if (m_hHttpFile)
	{
		::InternetCloseHandle(m_hHttpFile);
		m_hHttpFile = NULL;
	}
	if (m_hHttpConnection)
	{
		::InternetCloseHandle(m_hHttpConnection);
		m_hHttpConnection = NULL;
	}
	if (m_hInternetSession)
	{
		::InternetCloseHandle(m_hInternetSession);
		m_hInternetSession = NULL;
	}

}


/**
 * CHTTPS::GetError
 * Last에러 코드 리턴
 * Dependencies : None
 * Side Effects : None
 * 
 * @return		int	
 * @exception		
 * @throws		
 */
int CHTTPS::GetError()
{
	return ::GetLastError();
}



/**
 * CHTTPS::KillLauncher
 * 런처 종료하기
 * Dependencies : None
 * Side Effects : None
 * 
 * @param		CString szLauncher	
 * @return		bool	
 * @exception		
 * @throws		
 */
bool CHTTPS::KillLauncher(CString szLauncher)
{
	return 0;
}


/**
 * CHTTPS::GZipCompress
 * GZip 압축 함수
 * Dependencies : None
 * Side Effects : None
 * 
 * @param		LPBYTE lpInput	
 * @param		DWORD dwInputSize	
 * @param		LPBYTE *ppOutput	
 * @param		DWORD *pdwOutputSize	
 * @return		bool CHTTPS::GZipCompress	
 * @exception		
 * @throws		
 */
bool CHTTPS::GZipCompress (LPBYTE lpInput, DWORD dwInputSize, LPBYTE *ppOutput, DWORD *pdwOutputSize)
{
	ASSERT(lpInput != NULL );
	ASSERT(dwInputSize != 0 );

	int nError =0;
	ULONG uOutSize = *pdwOutputSize;
	nError = compress(*ppOutput, &uOutSize, lpInput, dwInputSize);
	if (Z_OK != nError)	
		return false;

	*pdwOutputSize = (int)uOutSize;
	return true;
}
///////////////////////////////////////////////////////////////////////////////////////
BOOL zlib_WriteToFile (LPBYTE lpBuff, DWORD dwSize, LPCSTR pszFilePath)
{
	DWORD		dwWritten =0;
	FILE		*fpWrite = NULL;

	HANDLE		hFile = NULL;
	FILETIME	ft_ModifyTime;

	if (NULL == lpBuff || NULL == pszFilePath)	return FALSE;

	fpWrite = fopen (pszFilePath, "wb");
	if (NULL == fpWrite) return FALSE;

	dwWritten = fwrite (lpBuff, 1, dwSize, fpWrite);
	fclose (fpWrite);
	if (dwWritten < dwSize) return FALSE;


	hFile = CreateFile ((LPCWSTR)pszFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)	return FALSE;
	SetFileTime (hFile, &ft_ModifyTime, &ft_ModifyTime, &ft_ModifyTime);
	CloseHandle(hFile);

	return TRUE;
}

/**
 * CHTTPS::GZipDeCompress 
 * GZip 압축 해제 함수
 * Dependencies : None
 * Side Effects : None
 * 
 * @param		LPBYTE lpInput	
 * @param		DWORD dwInputSize	
 * @param		LPBYTE *ppOutput	
 * @param		DWORD *pdwOutputSize	
 * @return		bool CHTTPS::GZipDeCompress	
 * @exception		
 * @throws		
 */
bool CHTTPS::GZipDeCompress (LPBYTE lpInput, DWORD dwInputSize, LPBYTE *ppOutput, DWORD *pdwOutputSize)
{
	ASSERT(lpInput != NULL );
	ASSERT(dwInputSize != 0 );

	int nError =0;
	ULONG uOutSize = *pdwOutputSize;
	nError = uncompress(*ppOutput, &uOutSize, lpInput, dwInputSize);
	if (Z_OK != nError)	
		return false;

	*pdwOutputSize = (int)uOutSize;
	return true;
}



/**
 * CHTTPS::URLParse
 * URL 을 파싱해서 m_strSever ,m_strObject,m_nPort를 구성한다.
 * Dependencies : None
 * Side Effects : None
 * 
 * @param		CString url	
 * @return		bool	
 * @exception		
 * @throws		
 */
bool CHTTPS::URLParse(CString url)
{
	ASSERT(!url.IsEmpty());

	TCHAR hostName[INTERNET_MAX_PATH_LENGTH] = _T("");
	TCHAR strPathName[INTERNET_MAX_PATH_LENGTH] = _T("");
	URL_COMPONENTS urlComponents;
	memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
	urlComponents.dwStructSize		= sizeof(URL_COMPONENTS);
	urlComponents.lpszHostName		= hostName;
	urlComponents.dwHostNameLength	= INTERNET_MAX_PATH_LENGTH;
	urlComponents.lpszUrlPath	  = strPathName;
	urlComponents.dwUrlPathLength = INTERNET_MAX_PATH_LENGTH;

	if (!InternetCrackUrl(url, url.GetLength(), 0, &urlComponents))
	{
		m_bSuccess = false;
		m_bAbort = true;
		KTrace(_T("Failed in call to InternetCrackUrl, Error:%d\n"), ::GetLastError());
	}

	/// 호스트 이름
	m_strSever	= hostName;

	ASSERT(!m_strSever.IsEmpty());


	/// 포트를 지정 하지 않았을경우 기본 포트값 http = 80 , https = 443
	if (urlComponents.nPort != 0) 
		m_nPort		= urlComponents.nPort;
	else
	{
		m_nPort = urlComponents.nPort;
	}

	m_strObject	= strPathName;

	/// http or https 인지 
	if(urlComponents.nScheme == INTERNET_SCHEME_HTTPS)
		m_bHttps  = true;


	return true;
}

/**
 * CHTTPS::DownloadSW
 * 서버에서 직접 파일을 다운받는다.
 * Dependencies : None
 * Side Effects : None
 * 
 * @param		CString url	
 * @param		CString DnFileName	
 * @return		bool	
 * @exception		
 * @throws		
 */
bool CHTTPS::DownloadSW(CString url , CString DnFileName)
{

	bool				bSucc = false;
	unsigned long	ulSize = 0;

	ASSERT(!DnFileName.IsEmpty());
	

	bSucc = URLParse(url);
	if(bSucc ==  false)
	{
		m_sError = _T("Failed to URLParse\n");
		KTrace(m_sError);
		return false;

	}	
	m_sFileToDownloadInto = DnFileName;
	//Check to see if the file we are downloading to exists and if
	//it does, then ask the user if they were it overwritten
	CFileStatus fs;
	ASSERT(m_sFileToDownloadInto.GetLength());

	//Try and open the file we will download into
	if (!m_FileToWrite.Open(m_sFileToDownloadInto, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite))
	{
		m_sError = _T("Failed to open the file to download into\n");
		KTrace(m_sError);
		return TRUE;
	}

	m_bSuccess = false;


// 	CWinThread* AFXAPI AfxBeginThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam, int nPriority = THREAD_PRIORITY_NORMAL, UINT nStackSize = 0,DWORD dwCreateFlags = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);
// 	CWinThread* AFXAPI AfxBeginThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam, int nPriority = THREAD_PRIORITY_NORMAL, UINT nStackSize = 0,DWORD dwCreateFlags = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);

	m_pThread = AfxBeginThread((AFX_THREADPROC)CHTTPS::_DownLoadThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, CREATE_SUSPENDED);

	ASSERT(m_pThread != NULL);

	if (m_pThread == NULL)
	{
		m_sError = _T("Failed to create download thread, dialog is aborting\n");
		KTrace(m_sError);
		return false;
	}
	m_pThread->m_bAutoDelete = FALSE;
	m_pThread->ResumeThread();

	if (NULL != m_pThread) {
		DWORD dRet;
		bool  bWait = true;
		while (bWait)
		{
			dRet  = ::WaitForSingleObject(m_pThread->m_hThread, INFINITE);
			switch(dRet)
			{
			case WAIT_ABANDONED:
				bWait = false;
				m_bAbort =false;
				break;
			case WAIT_OBJECT_0:
				bWait = false;
				break;
			case WAIT_TIMEOUT:
				bWait = false;
				m_bAbort =false;
				break;
			}


		}
		::CloseHandle(m_pThread);
		m_pThread = NULL;
	}

	if(m_bAbort)
	{
	
		return false;
	}
	return true;
}

/**
 * CHTTPS::_DownLoadThread
 * 
 * Dependencies : None
 * Side Effects : None
 * 
 * @param		void *pParm	
 * @return		UINT APIENTRY	
 * @exception		
 * @throws		
 */
UINT APIENTRY  CHTTPS::_DownLoadThread(void *pParm)
{
	CHTTPS *pCtx = reinterpret_cast<CHTTPS *> (pParm);
	if (NULL == pCtx)
	{
		return 0;
	}
	pCtx->DownLoadThread();
	return 0;
}

/**
 * CHTTPS::DownLoadThread
 * 서버에서 데이터를 다운로드하고 파일로 저장한다.
 * Dependencies : None
 * Side Effects : None
 * 
 * @return		void		
 * @exception		
 * @throws		
 */
void  CHTTPS::DownLoadThread()
{

	bool bRet = false;

	//Create the Internet session handle
	ASSERT(m_hInternetSession == NULL);
	m_hInternetSession = ::InternetOpen(AfxGetAppName(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInternetSession == NULL)
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError = _T("Failed in call to InternetOpen, Error\n");
		KTrace(m_sError);
		return;
	}

	//Setup the status callback function
	if (::InternetSetStatusCallback(m_hInternetSession, CHTTPS::_InternetStatusCallback ) == INTERNET_INVALID_STATUS_CALLBACK)
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError = _T("Failed in call to InternetSetStatusCallback, Error\n");
		KTrace(m_sError);
		return;
	}

	//Make the connection to the HTTP server          
	ASSERT(m_hHttpConnection == NULL);
	m_hHttpConnection = ::InternetConnect(m_hInternetSession, m_strSever, m_nPort, NULL,NULL,INTERNET_SERVICE_HTTP, 0 , (DWORD) this);
	if (m_hHttpConnection == NULL)
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError = _T("Failed in call to InternetConnect, Error\n");
		KTrace(m_sError);
		return;
	}


	//Issue the request to read the file
	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*");  //We support accepting any mime file type since this is a simple download of a file
	ppszAcceptTypes[1] = NULL;
	ASSERT(m_hHttpFile == NULL);

	DWORD dwFlags = 
		 (m_bHttps? INTERNET_FLAG_SECURE:0) | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;

	m_hHttpFile = ::HttpOpenRequest(m_hHttpConnection, L"POST", m_strObject, NULL, NULL, ppszAcceptTypes, dwFlags, (DWORD) this);
	if (m_hHttpFile == NULL)
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError = _T("Failed in call to HttpOpenRequest, Error\n");
		KTrace(m_sError);
		return;
	}


	//Issue the request
	BOOL bSend = ::HttpSendRequest(m_hHttpFile, NULL, 0, NULL, 0);
	if (!bSend)
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError = _T("Failed in call to HttpSendRequest, Error\n");
		KTrace(m_sError);
		return;
	}

	//Check the HTTP status code
	TCHAR szStatusCode[32];
	DWORD dwInfoSize = 32;
	if (!::HttpQueryInfo(m_hHttpFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwInfoSize, NULL))
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError = _T("Failed in call to HttpQueryInfo HTTP_QUERY_STATUS_CODE, Error\n");
		KTrace(m_sError);
		return;
	}
	else
	{
		long nStatusCode = _ttol(szStatusCode);

		//Handle any authentication errors
		if (nStatusCode == HTTP_STATUS_PROXY_AUTH_REQ || nStatusCode == HTTP_STATUS_DENIED)
		{
			// We have to read all outstanding data on the Internet handle
			// before we can resubmit request. Just discard the data.
			char szData[51];
			DWORD dwSize;
			do
			{
				::InternetReadFile(m_hHttpFile, (LPVOID)szData, 50, &dwSize);
			}
			while (dwSize != 0);
		}
		else if (nStatusCode != HTTP_STATUS_OK)
		{
			m_bSuccess = false;
			m_bAbort = true;
			m_sError = _T("Failed to retrieve a HTTP 200 status, Error\n");
			KTrace(m_sError);
			return;
		}
	}


	// Get the length of the file.            
	TCHAR szContentLength[32];
	dwInfoSize = 32;
	DWORD dwFileSize = 0;
	BOOL bGotFileSize = FALSE;
	if (::HttpQueryInfo(m_hHttpFile, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL))
	{
		//Set the progress control range
		bGotFileSize = TRUE;
		dwFileSize = (DWORD) _ttol(szContentLength);
		m_nRecvTotalSize = dwFileSize;
	}else
	{
		m_sError = _T("Failed to HttpQueryInfo HTTP_QUERY_CONTENT_LENGTH, Error\n");
		KTrace(m_sError);
	}

	//Now do the actual read of the file
	DWORD dwStartTicks = ::GetTickCount();
	DWORD dwCurrentTicks = dwStartTicks;
	DWORD dwBytesRead = 0;
	char szReadBuf[1024];
	DWORD dwBytesToRead = 1024;
	DWORD dwTotalBytesRead = 0;
	DWORD dwLastTotalBytes = 0;
	DWORD dwLastPercentage = 0;

	HANDLE hHeap = GetProcessHeap();

	//	int nBuffSize = BUFF_SIZE;	// 할당된 Buffer의 사이즈.
	m_lpRecvBuff = NULL;
	m_lpRecvBuff = (LPBYTE)HeapAlloc (hHeap, HEAP_ZERO_MEMORY, BUFF_SIZE +1);	// 받은 전체데이타.

	if(!m_lpRecvBuff)
	{
		// 메모리 할당을 못함 !!  어떻게 해야 되나...
		return;
	}
	do
	{
		if (!::InternetReadFile(m_hHttpFile, szReadBuf, dwBytesToRead, &dwBytesRead))
		{
			m_bSuccess = false;
			m_bAbort = 0;
			m_sError = _T("Failed to InternetReadFileH, Error\n");
			KTrace(m_sError);
			return;
		}
		else if (dwBytesRead )
		{

			TRY
			{
				m_FileToWrite.Write(szReadBuf, dwBytesRead);
			}
			CATCH(CFileException, e);                                          
			{
				KTrace(_T("An exception occured while writing to the download file\n"));
				DelException(e);
				return;
			}
			END_CATCH
			//Increment the total number of bytes read
			dwTotalBytesRead += dwBytesRead;  


		}
	} 
	while (dwBytesRead);
	m_FileToWrite.Close();
	if (m_bAbort)
		::DeleteFile(m_sFileToDownloadInto);

	m_bSuccess = true;

	return;
}


/**
 * CHTTPS::RequestData
 * 서버에 데이터를 요청하고 데이터를 버퍼로 넘긴다.
 * Dependencies : None
 * Side Effects : None
 * 
 * @param		CString url	
 * @param		LPCTSTR strInput	
 * @param		CString& strOutput	
 * @return		bool	
 * @exception		
 * @throws		
 */
bool CHTTPS::RequestData(CString url , LPCTSTR strInput, CString& strOutput)
{

	bool				bSucc = false;
	unsigned long	ulSize = 0;

	bSucc = URLParse(url);
	if(bSucc ==  false)
	{
		m_sError = _T("Failed to URLParse\n");
		KTrace(m_sError);
		return false;

	}


	//+ 데이터 압축
	ulSize =strlen(CTsToAs(strInput).GetAp());
	if(ulSize == 0)
		return false;

	m_nRequestSize = ulSize + COMPRESS_TRADEOFF_SIZE;
	m_lpRequestBuff = (LPBYTE)malloc(m_nRequestSize);
	memset (m_lpRequestBuff, 0, m_nRequestSize);
	bSucc = GZipCompress ((LPBYTE)CTsToAs(strInput).GetAp() , ulSize, &m_lpRequestBuff, &m_nRequestSize);

	ASSERT(m_nRequestSize > 0);

	if(!bSucc)
	{
		m_sError = _T("Failed to GZipCompress\n");
		KTrace(m_sError);
		return false;
	}


	m_bSuccess = false;
	m_pThread = AfxBeginThread((AFX_THREADPROC)CHTTPS::_RequestThread, (LPVOID)this, THREAD_PRIORITY_NORMAL, CREATE_SUSPENDED);

	ASSERT(m_pThread != NULL);

	if (m_pThread == NULL)
	{
		m_sError = _T("Failed to create download thread, dialog is aborting\n");
		KTrace(m_sError);
		return false;
	}
	m_pThread->m_bAutoDelete = FALSE;
	m_pThread->ResumeThread();


	if (NULL != m_pThread) {
		DWORD dRet;
		bool  bWait = true;
		while (bWait)
		{
			dRet  = ::WaitForSingleObject(m_pThread->m_hThread, INFINITE);
			switch(dRet)
			{
				case WAIT_ABANDONED:
					bWait = false;
					m_bAbort =true;
					break;
				case WAIT_OBJECT_0:
					m_bAbort =false;
					bWait = false;
					break;
				case WAIT_TIMEOUT:
					bWait = false;
					m_bAbort =true;
					break;
			}

			KTrace(_T("download : %d/%d \n" ) ,m_nRecvSize,m_nRecvTotalSize);

		}
		m_pThread->Delete();//::CloseHandle(m_pThread);
		m_pThread = NULL;
	}

	if(m_bAbort|| (!m_bSuccess))
	{
		if (NULL != m_lpRecvBuff) {

			HANDLE hHeap = GetProcessHeap();
			HeapFree (hHeap, 0, m_lpRecvBuff);
			m_lpRecvBuff = NULL;
			m_nRecvSize = 0;

		}

		if (NULL != m_lpRequestBuff) {

			free(m_lpRequestBuff);
			m_lpRequestBuff = NULL;
			m_nRequestSize = 0;
		}
		m_sError = _T("Failed SendThread is aborting\n");
		KTrace(m_sError);
		return false;
	}


	LPBYTE			lpUncompressed = NULL;
	unsigned long	ulUncompressedSize = 0;

	// 데이터 압축풀기
	ulUncompressedSize = MEMSIZE;	// 원본이 MEMSIZE를 넘을수는없다...서버에서 제한에 걸렸을테니...
	lpUncompressed = (LPBYTE)malloc(ulUncompressedSize);
	memset (lpUncompressed, 0, ulUncompressedSize);

	bSucc = GZipDeCompress (m_lpRecvBuff, m_nRecvSize, &lpUncompressed, &ulUncompressedSize);

	ASSERT(ulUncompressedSize > 0 );
	
	if (false == bSucc)
	{
		free (lpUncompressed);
		m_sError = _T("Failed to GZipDeCompress is false\n");
		KTrace(m_sError);
		return false;
	}
	


	strOutput = CAsToTs((LPCSTR)lpUncompressed).GetTp();

	ASSERT(!strOutput.IsEmpty() );

	if (NULL != m_lpRecvBuff) {

		HANDLE hHeap = GetProcessHeap();
		HeapFree (hHeap, 0, m_lpRecvBuff);
		m_lpRecvBuff = NULL;
		m_nRecvSize = 0;

	}

	if (NULL != m_lpRequestBuff) {

		free(m_lpRequestBuff);
		m_lpRequestBuff = NULL;
		m_nRequestSize = 0;
	}



	free (lpUncompressed);

	return true;
}


/**
 *  CHTTPS::_RequestThread
 * 
 * Dependencies : None
 * Side Effects : None
 * 
 * @param		void *pParm	
 * @return		UINT APIENTRY	
 * @exception		
 * @throws		
 */
UINT APIENTRY  CHTTPS::_RequestThread(void *pParm)
{
	CHTTPS *pCtx = reinterpret_cast<CHTTPS *> (pParm);
	if (NULL == pCtx)
	{
		return 0;
	}
	pCtx->RequestThread();
	return 0;
}

/**
 * CHTTPS::RequestThread
 * 요청된 데이터를 실제 http 통신을 통해 받는다.
 * Dependencies : None
 * Side Effects : None
 * 
 * @return		void		
 * @exception		
 * @throws		
 */
void  CHTTPS::RequestThread()
{

	bool bRet = false;

	//Create the Internet session handle
	ASSERT(m_hInternetSession == NULL);
	m_hInternetSession = ::InternetOpen(AfxGetAppName(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (m_hInternetSession == NULL)
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError.Format(_T("Failed in call to InternetOpen, Error:%d\n" ), GetLastError());
		KTrace(m_sError);
		return;
	}

	//Setup the status callback function
	if (::InternetSetStatusCallback(m_hInternetSession, CHTTPS::_InternetStatusCallback ) == INTERNET_INVALID_STATUS_CALLBACK)
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError.Format(_T("Failed in call to InternetSetStatusCallback, Error:%d\n") , GetLastError());
		KTrace(m_sError);
		return;
	}

	//Make the connection to the HTTP server          
	ASSERT(m_hHttpConnection == NULL);
	m_hHttpConnection = ::InternetConnect(m_hInternetSession, m_strSever, m_nPort, NULL,NULL,INTERNET_SERVICE_HTTP, 0, (DWORD) this);
	if (m_hHttpConnection == NULL)
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError.Format( _T("Failed in call to InternetConnect,  Error:%d\n") , GetLastError());
		KTrace(m_sError);
		return;
	}


	//Issue the request to read the file
	LPCTSTR ppszAcceptTypes[2];
	ppszAcceptTypes[0] = _T("*/*");  //We support accepting any mime file type since this is a simple download of a file
	ppszAcceptTypes[1] = NULL;
	ASSERT(m_hHttpFile == NULL);
	DWORD dwFlags = 
		(m_bHttps? INTERNET_FLAG_SECURE:0) | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
//		INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
	m_hHttpFile = ::HttpOpenRequest(m_hHttpConnection, L"POST", m_strObject, NULL, NULL, NULL/*ppszAcceptTypes*/, dwFlags, (DWORD) this);
	if (m_hHttpFile == NULL)
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError.Format(_T("Failed in call to HttpOpenRequest,  Error:%d\n") , GetLastError());
		KTrace(m_sError);
		return;
	}


	if(!::HttpAddRequestHeaders(m_hHttpFile , ACCEPT_ENCODING_HEADER , -1,HTTP_ADDREQ_FLAG_ADD))
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError.Format( _T("Failed in call to HttpAddRequestHeaders,  Error:%d\n" ), GetLastError());
		KTrace(m_sError);
		return;

	}

///  인증무시 루틴 + 
	dwFlags  = 0;
	DWORD dwBuffLen = sizeof(dwFlags);
	InternetQueryOption (m_hHttpFile, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBuffLen);
	dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
	InternetSetOption (m_hHttpFile, INTERNET_OPTION_SECURITY_FLAGS,(LPVOID)&dwFlags, sizeof (dwFlags) );
/// 인증무시 루틴 -

	//Issue the request
	BOOL bSend = ::HttpSendRequest(m_hHttpFile, NULL, 0, (LPVOID)m_lpRequestBuff, m_nRequestSize);
	if (!bSend)
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError.Format(_T("Failed in call to HttpSendRequest,  Error:%d\n" ), GetLastError());
		DWORD dwError = GetLastError ();
		if (dwError == ERROR_INTERNET_INVALID_CA)
		{
			HWND hwnd = GetDesktopWindow();
			
			InternetErrorDlg (hwnd,
				m_hHttpFile,
				ERROR_INTERNET_INVALID_CA,
				FLAGS_ERROR_UI_FILTER_FOR_ERRORS |
				FLAGS_ERROR_UI_FLAGS_GENERATE_DATA |
				FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS,
				NULL);
				
		}

		KTrace(m_sError);
		return;
	}

	//Check the HTTP status code
	TCHAR szStatusCode[32];
	DWORD dwInfoSize = 32;
	if (!::HttpQueryInfo(m_hHttpFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwInfoSize, NULL))
	{
		m_bSuccess = false;
		m_bAbort = true;
		m_sError.Format( _T("Failed in call to HttpQueryInfo HTTP_QUERY_STATUS_CODE,  Error:%d\n") , GetLastError());
		KTrace(m_sError);
	


		return;
	}
	else
	{
		long nStatusCode = _ttol(szStatusCode);

		//Handle any authentication errors
		if (nStatusCode == HTTP_STATUS_PROXY_AUTH_REQ || nStatusCode == HTTP_STATUS_DENIED)
		{
			// We have to read all outstanding data on the Internet handle
			// before we can resubmit request. Just discard the data.
			char szData[51];
			DWORD dwSize;
			do
			{
				::InternetReadFile(m_hHttpFile, (LPVOID)szData, 50, &dwSize);
			}
			while (dwSize != 0);
		}
		else if (nStatusCode != HTTP_STATUS_OK)
		{
			m_bSuccess = false;
			m_bAbort = true;
			m_sError.Format( _T("Failed to retrieve a HTTP 200 status,  Error:%d\n") , GetLastError());
			KTrace(m_sError);
		return;
		}
	}

	// Get the length of the file.            
	TCHAR szContentLength[32];
	dwInfoSize = 32;
	DWORD dwFileSize = 0;
	BOOL bGotFileSize = FALSE;
	if (::HttpQueryInfo(m_hHttpFile, HTTP_QUERY_CONTENT_LENGTH, szContentLength, &dwInfoSize, NULL))
	{
		//Set the progress control range
		bGotFileSize = TRUE;
		dwFileSize = (DWORD) _ttol(szContentLength);
		m_nRecvTotalSize = dwFileSize;
	}else
	{
		m_sError.Format( _T("Failed to HttpQueryInfo HTTP_QUERY_CONTENT_LENGTH,  Error:%d\n") , GetLastError());
		KTrace(m_sError);
	}

	//Now do the actual read of the file
	DWORD dwBytesRead = 0;
	DWORD dwBytesToRead = 1024;
	DWORD dwTotalBytesRead = 0;

	HANDLE hHeap = GetProcessHeap();
	if (NULL != m_lpRecvBuff)
		HeapFree (hHeap, 0, m_lpRecvBuff);

	m_lpRecvBuff = NULL;
	int nBuffSize = BUFF_SIZE;	// 할당된 Buffer의 사이즈.
	m_lpRecvBuff = (LPBYTE)HeapAlloc (hHeap, HEAP_ZERO_MEMORY, nBuffSize +1);	// 받은 전체데이타.

	if(!m_lpRecvBuff)
	{
		// 메모리 할당을 못함 !!  어떻게 해야 되나...
		return;
	}

	LPBYTE lpChunk = (LPBYTE)HeapAlloc (hHeap, HEAP_ZERO_MEMORY, BUFF_SIZE +1);	// 한번에 받은데이타

	if(!lpChunk)
	{
		// 메모리 할당을 못함 !!  어떻게 해야 되나...
		return;
	}

	do
	{
		if (!::InternetReadFile(m_hHttpFile, lpChunk,BUFF_SIZE, &dwBytesRead))//dwBytesToRead, &dwBytesRead))  // jeromwolf 버그 수정
		{
			m_bSuccess = false;
			m_bAbort = 0;
			m_sError.Format( _T("Failed to InternetReadFileH,  Error:%d\n") , GetLastError());
			KTrace(m_sError);
			return;
		}
		else if (dwBytesRead )
		{

			if (dwBytesToRead < m_nRecvSize + dwBytesRead)
			{
				LPVOID pTmp = m_lpRecvBuff;
				dwBytesToRead += BUFF_SIZE;
				m_lpRecvBuff = (LPBYTE)HeapAlloc (hHeap, HEAP_ZERO_MEMORY, dwBytesToRead +1);

 				if(!m_lpRecvBuff)
				{
					// 메모리 할당을 못함 !!  어떻게 해야 되나...
					return;
				}

				memcpy (m_lpRecvBuff, pTmp, m_nRecvSize);

			}
			memcpy (m_lpRecvBuff + m_nRecvSize, lpChunk, dwBytesRead);
			m_nRecvSize += dwBytesRead;
			memset (lpChunk, 0, BUFF_SIZE +1);
			//Increment the total number of bytes read
			dwTotalBytesRead += dwBytesRead;  
		}
	} 
	while (dwBytesRead);
	HeapFree (hHeap, 0, lpChunk);

	m_bSuccess = true;

	return;
}

/**
* CHTTPS::_InternetStatusCallback
* 
* Dependencies : None
* Side Effects : None
* 
* @return		void		
* @exception		
* @throws		
*/
void CALLBACK  CHTTPS::_InternetStatusCallback(HINTERNET hInternet,
															  DWORD dwContext,
															  DWORD dwInternetStatus,
															  LPVOID lpvStatusInformation,
															  DWORD dwStatusInformationLength)
{

	CHTTPS *pCtx = reinterpret_cast<CHTTPS *> (dwContext);
	if (NULL == pCtx)
	{
		return ;
	}
	pCtx->InternetStatusCallback(hInternet,dwContext,dwInternetStatus,lpvStatusInformation,dwStatusInformationLength);

}

/**
* CHTTPS::InternetStatusCallback
* 인터넷 상태 콜백함수.
* Dependencies : None
* Side Effects : None
* 
* @return		void		
* @exception		
* @throws		
*/
void  CHTTPS::InternetStatusCallback(HINTERNET hInternet,
												 DWORD dwContext,
												 DWORD dwInternetStatus,
												 LPVOID lpvStatusInformation,
												 DWORD dwStatusInformationLength)
{

	switch(dwInternetStatus)
	{
		case INTERNET_STATUS_RESOLVING_NAME:m_sError = _T("INTERNET_STATUS_RESOLVING_NAME \n");break;
		case INTERNET_STATUS_NAME_RESOLVED:m_sError = _T("INTERNET_STATUS_NAME_RESOLVED \n");break;
		case INTERNET_STATUS_CONNECTING_TO_SERVER:m_sError = _T("INTERNET_STATUS_CONNECTING_TO_SERVER \n");break;
		case INTERNET_STATUS_CONNECTED_TO_SERVER:m_sError = _T("INTERNET_STATUS_CONNECTED_TO_SERVER \n");break;
		case INTERNET_STATUS_SENDING_REQUEST:m_sError = _T("INTERNET_STATUS_SENDING_REQUEST \n");break;
		case INTERNET_STATUS_REQUEST_SENT:m_sError = _T("INTERNET_STATUS_REQUEST_SENT \n");break;
		case INTERNET_STATUS_RECEIVING_RESPONSE:m_sError = _T("INTERNET_STATUS_RECEIVING_RESPONSE \n");break;
		case INTERNET_STATUS_RESPONSE_RECEIVED:m_sError = _T("INTERNET_STATUS_RESPONSE_RECEIVED \n");break;
		case INTERNET_STATUS_CTL_RESPONSE_RECEIVED:m_sError = _T("INTERNET_STATUS_CTL_RESPONSE_RECEIVED \n");break;
		case INTERNET_STATUS_PREFETCH:m_sError = _T("INTERNET_STATUS_PREFETCH \n");break;
		case INTERNET_STATUS_CLOSING_CONNECTION:m_sError = _T("INTERNET_STATUS_CLOSING_CONNECTION \n");break;
		case INTERNET_STATUS_CONNECTION_CLOSED:m_sError = _T("INTERNET_STATUS_CONNECTION_CLOSED \n");break;
		case INTERNET_STATUS_HANDLE_CREATED:m_sError = _T("INTERNET_STATUS_HANDLE_CREATED \n");break;
		case INTERNET_STATUS_HANDLE_CLOSING:m_sError = _T("INTERNET_STATUS_HANDLE_CLOSING \n");break;
		case INTERNET_STATUS_DETECTING_PROXY:m_sError = _T("INTERNET_STATUS_DETECTING_PROXY \n");break;
		case INTERNET_STATUS_REQUEST_COMPLETE:m_sError = _T("INTERNET_STATUS_REQUEST_COMPLETE \n");break;
		case INTERNET_STATUS_REDIRECT:m_sError = _T("INTERNET_STATUS_REDIRECT \n");break;
		case INTERNET_STATUS_INTERMEDIATE_RESPONSE:m_sError = _T("INTERNET_STATUS_INTERMEDIATE_RESPONSE \n");break;
		case INTERNET_STATUS_USER_INPUT_REQUIRED:m_sError = _T("INTERNET_STATUS_USER_INPUT_REQUIRED \n");break;
		case INTERNET_STATUS_STATE_CHANGE:m_sError = _T("INTERNET_STATUS_STATE_CHANGE \n");break;
		case INTERNET_STATUS_COOKIE_SENT:m_sError = _T("INTERNET_STATUS_COOKIE_SENT \n");break;
		case INTERNET_STATUS_COOKIE_RECEIVED:m_sError = _T("INTERNET_STATUS_COOKIE_RECEIVED \n");break;
#ifdef _WIN32_WCE
		case INTERNET_STATUS_COOKIE_STATE:m_sError = _T("INTERNET_STATUS_COOKIE_STATE \n");break;
		case INTERNET_STATUS_COOKIE_SUPPRESSED:m_sError = _T("INTERNET_STATUS_COOKIE_SUPPRESSED \n");break;
#endif // _DEBUG
		case INTERNET_STATUS_PRIVACY_IMPACTED:m_sError = _T("INTERNET_STATUS_PRIVACY_IMPACTED \n");break;
		case INTERNET_STATUS_P3P_HEADER:m_sError = _T("INTERNET_STATUS_P3P_HEADER \n");break;
		case INTERNET_STATUS_P3P_POLICYREF:m_sError = _T("INTERNET_STATUS_P3P_POLICYREF \n");break;
		case INTERNET_STATUS_COOKIE_HISTORY:m_sError = _T("INTERNET_STATUS_COOKIE_HISTORY \n");break;
		case INTERNET_STATE_CONNECTED:m_sError = _T("INTERNET_STATE_CONNECTED \n");break;
		case INTERNET_STATE_DISCONNECTED:m_sError = _T("INTERNET_STATE_DISCONNECTED \n");break;
		case INTERNET_STATE_DISCONNECTED_BY_USER:m_sError = _T("INTERNET_STATE_DISCONNECTED_BY_USER \n");break;
		case INTERNET_STATE_IDLE:m_sError = _T("INTERNET_STATE_IDLE \n");break;
		case INTERNET_STATE_BUSY:m_sError = _T("INTERNET_STATE_BUSY \n");break;
		default:
				KTrace(_T("CHTTPS::InternetStatusCallback - %d:\n"),dwInternetStatus);
				break;
	}

	KTrace(m_sError);


}

#endif //_Use_HTTPS
