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
	/// http��� �������� 
	bool	m_bSuccess;

	/// http��� ��ҿ��� 
	bool	m_bAbort;
	
	bool	m_bHttps;

	/// �ٿ�ε�ÿ� ����� ����
	CFile    m_FileToWrite;

	/// http��� ���� ������
	LPBYTE	m_lpRecvBuff;

	/// http��� ��û ������
	LPBYTE	m_lpRequestBuff;

	/// ���� ����
	CString	m_sError;

	/// ��û��������
	CString	m_strSever;

	/// ��û ����������Ʈ
	CString	m_strObject;

	/// ���� �ٿ�ε�� ���� ����
	CString	m_sFileToDownloadInto;
	
	/// ���� Ŀ�ݼ�
	HINTERNET	m_hHttpConnection;

	/// ���� ����
	HINTERNET	m_hInternetSession;
	
	/// ���� ��û ����
	HINTERNET	m_hHttpFile;

	/// ���� ��Ʈ
	INTERNET_PORT	m_nPort;

	/// ��û ������
	CWinThread*		m_pThread;

	/// ���� �� ������
	unsigned long	m_nRecvTotalSize;

	/// ���� ����ũ��
	unsigned long	m_nRecvSize;
	/// ��û ������
	unsigned long	m_nRequestSize;

public:
	/// �����ڵ� ����
	int	GetError();
	
	/// ��ó ���̱� (���� �����ȵ� ������Ʈ ����)
	bool	KillLauncher(CString szLauncher);

	/// ����Ʈ���� �ٿ�ε� (������Ʈ ���� ���� ������)
	bool	DownloadSW(CString url , CString DnFileName);
	
	/// ������ ��û �Լ�
	bool	RequestData(CString url ,LPCTSTR strInput, CString& strOutput);

private:

	/// URL �ļ�
	bool	URLParse(CString url);
	/// GZip ����Ǯ��
	bool	GZipDeCompress (LPBYTE lpInput, DWORD dwInputSize,LPBYTE *ppOutput, DWORD *pdwOutputSize);
	/// GZip �����ϱ�
	bool	GZipCompress (LPBYTE lpInput, DWORD dwInputSize, LPBYTE *ppOutput, DWORD *pdwOutputSize);
	/// �����Ϳ�û������
	void	RequestThread();
	/// �ٿ�ε徲����
	void	DownLoadThread();
	/// ���ͳ� ���� �ݹ�
	void	InternetStatusCallback(HINTERNET hInternet,	DWORD dwContext,	DWORD dwInternetStatus,	LPVOID lpvStatusInformation,	DWORD dwStatusInformationLength);
	static	UINT APIENTRY _RequestThread(void *pParm);
	static	UINT APIENTRY _DownLoadThread(void *pParm);
	static	void CALLBACK _InternetStatusCallback(HINTERNET hInternet,	DWORD dwContext,	DWORD dwInternetStatus,	LPVOID lpvStatusInformation,	DWORD dwStatusInformationLength);

};
#endif //_Use_HTTPS
