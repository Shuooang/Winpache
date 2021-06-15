#pragma once

#ifdef _Use_HTTPS

#include <afxcmn.h>
#include <wininet.h>

#include "OpenLib/zlib/zlib.h"

#pragma comment(lib, "wininet.lib")

class MLIBTODLLCLASS CHTTPS
{
public:
	CHTTPS();
	virtual ~CHTTPS(void);

private:
	/// http통신 성공여부 
	bool	m_bSuccess;

	/// http통신 취소여부 
	bool	m_bAbort;
	
	bool	m_bHttps;

	/// 다운로드시에 저장될 파일
	CFile    m_FileToWrite;

	/// http통신 받은 데이터
	LPBYTE	m_lpRecvBuff;

	/// http통신 요청 데이터
	LPBYTE	m_lpRequestBuff;

	/// 에러 정보
	CString	m_sError;

	/// 요청서버정보
	CString	m_strSever;

	/// 요청 서버오브젝트
	CString	m_strObject;

	/// 파일 다운로드시 파일 정보
	CString	m_sFileToDownloadInto;
	
	/// 서버 커넷션
	HINTERNET	m_hHttpConnection;

	/// 서버 섹션
	HINTERNET	m_hInternetSession;
	
	/// 서버 요청 파일
	HINTERNET	m_hHttpFile;

	/// 서버 포트
	INTERNET_PORT	m_nPort;

	/// 요청 쓰레드
	CWinThread*		m_pThread;

	/// 받은 총 사이즈
	unsigned long	m_nRecvTotalSize;

	/// 받은 현재크기
	unsigned long	m_nRecvSize;
	/// 요청 사이즈
	unsigned long	m_nRequestSize;

public:
	/// 에러코드 리턴
	int	GetError();
	
	/// 런처 죽이기 (아직 결정안됨 업데이트 관련)
	bool	KillLauncher(CString szLauncher);

	/// 소프트웨어 다운로드 (업데이트 관련 아직 결정됨)
	bool	DownloadSW(CString url , CString DnFileName);
	
	/// 데이터 요청 함수
	bool	RequestData(CString url ,LPCTSTR strInput, CString& strOutput);

private:

	/// URL 파서
	bool	URLParse(CString url);
	/// GZip 압축풀기
	bool	GZipDeCompress (LPBYTE lpInput, DWORD dwInputSize,LPBYTE *ppOutput, DWORD *pdwOutputSize);
	/// GZip 압축하기
	bool	GZipCompress (LPBYTE lpInput, DWORD dwInputSize, LPBYTE *ppOutput, DWORD *pdwOutputSize);
	/// 데이터요청쓰레드
	void	RequestThread();
	/// 다운로드쓰레드
	void	DownLoadThread();
	/// 인터넷 상태 콜백
	void	InternetStatusCallback(HINTERNET hInternet,	DWORD dwContext,	DWORD dwInternetStatus,	LPVOID lpvStatusInformation,	DWORD dwStatusInformationLength);
	static	UINT APIENTRY _RequestThread(void *pParm);
	static	UINT APIENTRY _DownLoadThread(void *pParm);
	static	void CALLBACK _InternetStatusCallback(HINTERNET hInternet,	DWORD dwContext,	DWORD dwInternetStatus,	LPVOID lpvStatusInformation,	DWORD dwStatusInformationLength);

};
#endif //_Use_HTTPS
