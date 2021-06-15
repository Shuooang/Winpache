#include "pch.h"

#include "SslConfig.h"
#ifdef _Use_SSLConfig


CSslConfig::CSslConfig(void)
{
}


CSslConfig::~CSslConfig(void)
{
}
static const GUID AppId = {0xAAAABBBB, 0xCCCC, 0xDDDD, 0xEE, 0xEE, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};


/***************************************************************************++
Routine Description:
    Get certificate hash from the certificate subject name.
Arguments:
    CertSubjectName - Subject name of the certificate to find.
    StoreName       - Name of the store under Local Machine where certificate
                     is present.
    CertHash        - Buffer to return certificate hash.
    CertHashLength  - Buffer length on input, hash length on output (element
                     count).
Return Value:
    Status.
--***************************************************************************/
DWORD CSslConfig::GetCertificateHash(
    _In_ PCWSTR CertSubjectName,
    _In_ PCWSTR StoreName,
    //_Out_writes_bytes_to_(*CertHashLength, *CertHashLength) 
	PBYTE CertHash,
    _Inout_ PDWORD CertHashLength
    )
{
    HCERTSTORE SystemStore = NULL;
    PCCERT_CONTEXT CertContext = NULL;
    DWORD Error = ERROR_SUCCESS;

    RtlZeroMemory(CertHash, *CertHashLength);

    //
    // Open the store under local machine.
    //

    SystemStore = CertOpenStore(CERT_STORE_PROV_SYSTEM,
                                0,
                                (HCRYPTPROV_LEGACY)NULL,
                                CERT_SYSTEM_STORE_LOCAL_MACHINE,
                                StoreName);

    if (SystemStore == NULL)
    {
        Error = GetLastError();
        goto exit;
    }

    //
    // Find the certificate from the subject name and get the hash.
    //

    CertContext = CertFindCertificateInStore(SystemStore,
                                             X509_ASN_ENCODING,
                                             0,
                                             CERT_FIND_SUBJECT_STR,
                                             CertSubjectName,
                                             NULL);

    if (CertContext == NULL)
    {
        Error = GetLastError();
        goto exit;
    }

    if (!CertGetCertificateContextProperty(CertContext,
                                           CERT_HASH_PROP_ID,
                                           CertHash,
                                           CertHashLength))
    {

        Error = GetLastError();
        goto exit;
    }

exit:

    //
    // Free the certificate context.
    //

    if (CertContext != NULL)
    {
        CertFreeCertificateContext(CertContext);
    }

    //
    // Close the certificate store if it was opened.
    //

    if (SystemStore != NULL)
    {
        CertCloseStore(SystemStore, 0);
    }

    return Error;
}

/***************************************************************************++
Routine Description:
    Get certificate hash and set the SNI configuration.
Arguments:
    SniKey          - SSL endpoint key: host and port.
    CertSubjectName - Subject name of the certificate to find.
    StoreName       - Name of the store under Local Machine where certificate
                      is present.
Return Value:
    Status.
--***************************************************************************/
DWORD  CSslConfig::SetSniConfiguration(
    _In_ PHTTP_SERVICE_CONFIG_SSL_SNI_KEY SniKey,
    _In_ PCWSTR CertSubjectName,
    _In_ PCWSTR StoreName
    )
{
    DWORD Error = ERROR_SUCCESS;
    BYTE CertHash[50] = {};
    DWORD CertHashLength = ARRAYSIZE(CertHash);
    HTTP_SERVICE_CONFIG_SSL_SNI_SET SniConfig = {};

    Error = GetCertificateHash(CertSubjectName,
                               StoreName,
                               CertHash,
                               &CertHashLength);

    if (Error != ERROR_SUCCESS)
    {
        goto exit;
    }

    SniConfig.KeyDesc = *SniKey;
    SniConfig.ParamDesc.pSslHash = CertHash;
    SniConfig.ParamDesc.SslHashLength = CertHashLength;
    SniConfig.ParamDesc.pSslCertStoreName = (PWSTR)StoreName;
    SniConfig.ParamDesc.AppId = AppId;

    Error = HttpSetServiceConfiguration(NULL,
                                        HttpServiceConfigSslSniCertInfo,
                                        &SniConfig,
                                        sizeof(SniConfig),
                                        NULL);

exit:

    return Error;
}

/***************************************************************************++
Routine Description:
    Query the SNI configuration.
Arguments:
    SniKey - SSL endpoint key: host and port.
Return Value:
    Status.
--***************************************************************************/
DWORD CSslConfig::QuerySniConfiguration(
    _In_ PHTTP_SERVICE_CONFIG_SSL_SNI_KEY SniKey
    )
{
    DWORD Error = ERROR_SUCCESS;
    PHTTP_SERVICE_CONFIG_SSL_SNI_SET SniConfig = NULL;
    HTTP_SERVICE_CONFIG_SSL_SNI_QUERY SniQuery = {};
    DWORD ReturnLength = 0;

//	SniQuery.KeyDesc = *SniKey;
#ifdef _Error
	SniQuery.KeyDesc.pIpPort->sa_data = &SniKey->IpPort->__ss_pad1;
#endif // _Error
    SniQuery.QueryDesc = HttpServiceConfigQueryExact;

    //
    // Get the size of the buffer required to query the config.
    //
//	HttpServiceConfigSSLCertInfo
    Error = HttpQueryServiceConfiguration(NULL,
                                          HttpServiceConfigSslSniCertInfo,
                                          &SniQuery,
                                          sizeof(SniQuery),
                                          NULL,
                                          0,
                                          &ReturnLength,
                                          NULL);

    if (Error != ERROR_INSUFFICIENT_BUFFER)
    {
        goto exit;
    }

    //
    // Allocate buffer and query again.
    //

    SniConfig = (PHTTP_SERVICE_CONFIG_SSL_SNI_SET)malloc(ReturnLength);

    if (SniConfig == NULL)
    {
        Error = ERROR_NOT_ENOUGH_MEMORY;
        goto exit;
    }

    Error = HttpQueryServiceConfiguration(NULL,
                                          HttpServiceConfigSslSniCertInfo,
                                          &SniQuery,
                                          sizeof(SniQuery),
                                          SniConfig,
                                          ReturnLength,
                                          &ReturnLength,
                                          NULL);

    if (Error != ERROR_SUCCESS)
    {
        goto exit;
    }

exit:

    if (SniConfig != NULL)
    {
        free(SniConfig);
        SniConfig = NULL;
    }

    return Error;
}

/***************************************************************************++

Routine Description:

    Delete the SNI configuration.

Arguments:

    SniKey - SSL endpoint key: host and port.

Return Value:

    Status.

--***************************************************************************/
DWORD CSslConfig::DeleteSniConfiguration(
    _In_ PHTTP_SERVICE_CONFIG_SSL_SNI_KEY SniKey
    )
{
    DWORD Error = ERROR_SUCCESS;
    HTTP_SERVICE_CONFIG_SSL_SNI_SET SniConfig = {};

    SniConfig.KeyDesc = *SniKey;

    Error = HttpDeleteServiceConfiguration(NULL,
                                           HttpServiceConfigSslSniCertInfo,
                                           &SniConfig,
                                           sizeof(SniConfig),
                                           NULL);
    return Error;
}

/***************************************************************************++
Routine Description:
    Demonstrate how to set, query, and delete the hostname based SSL configuration.
Arguments:
    Port - Port for the hostname based SSL binding.
    Hostname - Hostname for the SSL binding.
    CertSubjectName - Subject name of the certificate using in the SSL binding.
    StoreName       - Name of the store under Local Machine where certificate
                      is present.
Return Value:
    Status.
--***************************************************************************/
DWORD CSslConfig::SniConfiguration(
    USHORT Port,
    _In_ PCWSTR Hostname,
    _In_ PCWSTR CertSubjectName,
    _In_ PCWSTR StoreName
    )
{
    DWORD Error = ERROR_SUCCESS;
    HTTP_SERVICE_CONFIG_SSL_SNI_KEY SniKey = {};

    //
    // Create SniKey.
    // N.B. An SNI binding is IP version agnostic but for API purposes we are
    // required to specify the IP address to be (IPv4) 0.0.0.0 in the key.
    //

    SniKey.Host = (PWSTR)Hostname;
    IN4ADDR_SETANY((PSOCKADDR_IN)&SniKey.IpPort);
    SS_PORT(&SniKey.IpPort) = htons(Port);
    
	// Create the SNI binding.
    Error = SetSniConfiguration(&SniKey, CertSubjectName, StoreName);
    if (Error != ERROR_SUCCESS)
        goto exit;

	// Query the SNI configuration.
    Error = QuerySniConfiguration(&SniKey);
    if (Error != ERROR_SUCCESS)
        goto exit;

	// Delete the SNI configuration.
    Error = DeleteSniConfiguration(&SniKey);
    if (Error != ERROR_SUCCESS)
        goto exit;
exit:
    return Error;
}

/***************************************************************************++
Routine Description:
    Create the port based SSL binding.
Arguments:
    CcsKey - CCS endpoint key.
Return Value:
    Status.
--***************************************************************************/
DWORD CSslConfig::SetCcsConfiguration(_In_ PHTTP_SERVICE_CONFIG_SSL_CCS_KEY CcsKey)
{
    DWORD Error = ERROR_SUCCESS;
    HTTP_SERVICE_CONFIG_SSL_CCS_SET CcsConfig = {};

    CcsConfig.KeyDesc = *CcsKey;
    CcsConfig.ParamDesc.AppId = AppId;

    Error = HttpSetServiceConfiguration(NULL,
                                        HttpServiceConfigSslSniCertInfo,
                                        &CcsConfig,
                                        sizeof(CcsConfig),
                                        NULL);
    return Error;
}

/***************************************************************************++
Routine Description:
    Query the port based SSL binding.
Arguments:
    CcsKey - CCS endpoint key: 0.0.0.0:Port.
Return Value:
    Status.
--***************************************************************************/
DWORD CSslConfig::QueryCcsConfiguration(_In_ PHTTP_SERVICE_CONFIG_SSL_CCS_KEY CcsKey)
{
    DWORD Error = ERROR_SUCCESS;
    PHTTP_SERVICE_CONFIG_SSL_CCS_SET CcsConfig = NULL;
    HTTP_SERVICE_CONFIG_SSL_CCS_QUERY CcsQuery = {};
    DWORD ReturnLength = 0;

    CcsQuery.KeyDesc = *CcsKey;
    CcsQuery.QueryDesc = HttpServiceConfigQueryExact;

    // Get the size of the buffer required to query the config.
    Error = HttpQueryServiceConfiguration(NULL,
                                          HttpServiceConfigSslSniCertInfo,
                                          &CcsQuery,
                                          sizeof(CcsQuery),
                                          NULL,
                                          0,
                                          &ReturnLength,
                                          NULL);

    if (Error != ERROR_INSUFFICIENT_BUFFER)
        goto exit;

    //
    // Allocate buffer and query again.
    CcsConfig = (PHTTP_SERVICE_CONFIG_SSL_CCS_SET)malloc(ReturnLength);
    if (CcsConfig == NULL)
    {
        Error = ERROR_NOT_ENOUGH_MEMORY;
        goto exit;
    }

    Error = HttpQueryServiceConfiguration(NULL,
                                          HttpServiceConfigSslSniCertInfo,
                                          &CcsQuery,
                                          sizeof(CcsQuery),
                                          CcsConfig,
                                          ReturnLength,
                                          &ReturnLength,
                                          NULL);

    if (Error != ERROR_SUCCESS)
        goto exit;
exit:
    if (CcsConfig != NULL)
    {
        free(CcsConfig);
        CcsConfig = NULL;
    }

    return Error;
}

/***************************************************************************++
Routine Description:
    Delete the port based SSL binding.
Arguments:
    CcsKey - CCS endpoint key.
Return Value:
    Status.
--***************************************************************************/
DWORD CSslConfig::DeleteCcsConfiguration(_In_ PHTTP_SERVICE_CONFIG_SSL_CCS_KEY CcsKey)
{
    DWORD Error = ERROR_SUCCESS;
    HTTP_SERVICE_CONFIG_SSL_CCS_SET CcsConfig = {};

    CcsConfig.KeyDesc = *CcsKey;

    Error = HttpDeleteServiceConfiguration(NULL,
                                           HttpServiceConfigSslSniCertInfo,
                                           &CcsConfig,
                                           sizeof(CcsConfig),
                                           NULL);
    return Error;
}

/***************************************************************************++
Routine Description:
    Demonstrate how to set, query, and delete a port based SSL binding.
Arguments:
    Port            - CCS binding port.
Return Value:
    Status.
--***************************************************************************/
DWORD CSslConfig::CcsConfiguration(USHORT Port)
{
    DWORD Error = ERROR_SUCCESS;
    HTTP_SERVICE_CONFIG_SSL_CCS_KEY CcsKey = {};

    //
    // Create CcsKey.
    // N.B. A CCS binding is IP version agnostic but for API purposes we are
    // required to specify the IP address to be (IPv4) 0.0.0.0 in the key.
    //

    IN4ADDR_SETANY((PSOCKADDR_IN)&CcsKey.LocalAddress);
    SS_PORT(&CcsKey.LocalAddress) = htons(Port);

    // Create the port based SSL binding.
    Error = SetCcsConfiguration(&CcsKey);
    if (Error == ERROR_SUCCESS)
        goto exit;

	// Query the port based SSL binding.
    //

    Error = QueryCcsConfiguration(&CcsKey);
    if (Error != ERROR_SUCCESS)
        goto exit;

	// Delete the port based SSL binding.
    Error = DeleteCcsConfiguration(&CcsKey);
    if (Error != ERROR_SUCCESS)
        goto exit;
exit:
    return Error;
}
#endif // _Use_SSLConfig
