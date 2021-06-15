#pragma once

#ifdef _Use_SSLConfig

#pragma warning(disable:4201)   // nameless struct/union
#pragma warning(disable:4214)   // bit field types other than int
#include <ws2def.h>
#include <http.h>
#include <wincrypt.h>
#include <mstcpip.h>

// C:\Program Files\Microsoft SDKs\Windows\v7.0A\include\http.h
#ifndef HTTP_SERVICE_CONFIG_SSL_SNI_KEY
typedef struct _HTTP_SERVICE_CONFIG_SSL_SNI_KEY {
	SOCKADDR_STORAGE IpPort;
	PWSTR            Host;
} HTTP_SERVICE_CONFIG_SSL_SNI_KEY, *PHTTP_SERVICE_CONFIG_SSL_SNI_KEY;

typedef struct _HTTP_SERVICE_CONFIG_SSL_SNI_SET {
	HTTP_SERVICE_CONFIG_SSL_SNI_KEY KeyDesc;
	HTTP_SERVICE_CONFIG_SSL_PARAM   ParamDesc;
} HTTP_SERVICE_CONFIG_SSL_SNI_SET, *PHTTP_SERVICE_CONFIG_SSL_SNI_SET;

typedef struct _HTTP_SERVICE_CONFIG_SSL_SNI_QUERY {
	HTTP_SERVICE_CONFIG_QUERY_TYPE QueryDesc;
	HTTP_SERVICE_CONFIG_SSL_KEY    KeyDesc;
	DWORD                          dwToken;
} HTTP_SERVICE_CONFIG_SSL_SNI_QUERY, *PHTTP_SERVICE_CONFIG_SSL_SNI_QUERY;
#endif

#ifndef HTTP_SERVICE_CONFIG_SSL_CCS_KEY
typedef struct _HTTP_SERVICE_CONFIG_SSL_CCS_KEY
{
	SOCKADDR_STORAGE LocalAddress;
} HTTP_SERVICE_CONFIG_SSL_CCS_KEY, *PHTTP_SERVICE_CONFIG_SSL_CCS_KEY;
typedef struct _HTTP_SERVICE_CONFIG_SSL_CCS_QUERY
{
	UINT dwToken;
	HTTP_SERVICE_CONFIG_SSL_CCS_KEY KeyDesc;
	HTTP_SERVICE_CONFIG_QUERY_TYPE QueryDesc;
} HTTP_SERVICE_CONFIG_SSL_CCS_QUERY, *PHTTP_SERVICE_CONFIG_SSL_CCS_QUERY;
typedef struct _HTTP_SERVICE_CONFIG_SSL_CCS_SET
{
	HTTP_SERVICE_CONFIG_SSL_CCS_KEY KeyDesc;
	HTTP_SERVICE_CONFIG_SSL_PARAM ParamDesc;
} HTTP_SERVICE_CONFIG_SSL_CCS_SET, *PHTTP_SERVICE_CONFIG_SSL_CCS_SET;
// typedef struct _HTTP_SERVICE_CONFIG_SSL_KEY
// {
// 	INT_PTR pIpPort;
// } HTTP_SERVICE_CONFIG_SSL_KEY, *PHTTP_SERVICE_CONFIG_SSL_KEY;
#endif

#ifndef HttpServiceConfigSslSniCertInfo
// Windows 8 and later 에 사용 되므로 대치
#define HttpServiceConfigSslSniCertInfo HttpServiceConfigSSLCertInfo
#endif

class CSslConfig
{
public:
	CSslConfig(void);
	~CSslConfig(void);
	DWORD GetCertificateHash( _In_ PCWSTR CertSubjectName, _In_ PCWSTR StoreName, 
		//_Out_writes_bytes_to_(*CertHashLength, *CertHashLength) 
		PBYTE CertHash, _Inout_ PDWORD CertHashLength );
	DWORD SetSniConfiguration( _In_ PHTTP_SERVICE_CONFIG_SSL_SNI_KEY SniKey, _In_ PCWSTR CertSubjectName, _In_ PCWSTR StoreName );
	DWORD QuerySniConfiguration( _In_ PHTTP_SERVICE_CONFIG_SSL_SNI_KEY SniKey );
	DWORD DeleteSniConfiguration( _In_ PHTTP_SERVICE_CONFIG_SSL_SNI_KEY SniKey );
	DWORD SniConfiguration( USHORT Port, _In_ PCWSTR Hostname, _In_ PCWSTR CertSubjectName, _In_ PCWSTR StoreName );
	DWORD SetCcsConfiguration( _In_ PHTTP_SERVICE_CONFIG_SSL_CCS_KEY CcsKey );
	DWORD QueryCcsConfiguration( _In_ PHTTP_SERVICE_CONFIG_SSL_CCS_KEY CcsKey );
	DWORD DeleteCcsConfiguration( _In_ PHTTP_SERVICE_CONFIG_SSL_CCS_KEY CcsKey );
	DWORD CcsConfiguration( USHORT Port );
};

#endif // _Use_SSLConfig
