#include "pch.h"
#include "KwLib64/Kw_tool.h"
#include "OpenSsl.h"

#ifdef _Use_OpenSsl
#include <openssl/conf.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/err.h>

void OSslEncrypt::OpenErrLog()
{
	FILE* fp = NULL;
	fopen_s(&fp, "opensslerr.log", "wt+");
	ERR_print_errors_fp(fp);//fseek 안해도 되나?
	fclose(fp);
}

/// @brief 데이터 암호화. 비번과 소금이 첨가 되야 한다.
/// @param orgBuf 
/// @param orgLen 
/// @param descBuf 
/// @param descLen 
/// @return 
BOOL OSslEncrypt::Encrypt(const unsigned char* orgBuf, int orgLen, unsigned char* descBuf, int* descLen)
{
	//Key, IV 만들기 // orgLen == 52
	BOOL rb = TRUE;
	EVP_CIPHER_CTX* ctx = nullptr;
	try
	{
		PUCHAR keyiv = new UCHAR[32];
		KAtEnd d_keyiv([&]() { delete[] keyiv; });

		int ret = PKCS5_PBKDF2_HMAC_SHA1((PAS)_pwd, _pwd.GetLength(), (PUCHAR)(PAS)_SALT, _SALT.GetLength(),
			ITER, 32, keyiv); // ret == 1
		if(ret != 1)
			throw (int)__LINE__;

		int len = 0;
		ctx = EVP_CIPHER_CTX_new();
		if(ctx == nullptr)
			throw (int)__LINE__;
		KAtEnd d_ctx([&]() { EVP_CIPHER_CTX_free(ctx); });
		EVP_CIPHER_CTX_init(ctx);

		if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, &keyiv[0], &keyiv[16]))
			throw (int)__LINE__;

		if(1 != EVP_EncryptUpdate(ctx, descBuf, &len, orgBuf, orgLen))
			throw (int)__LINE__;
		*descLen = len; // len == 48

		if(1 != EVP_EncryptFinal_ex(ctx, descBuf + len, &len))
			throw (int)__LINE__;
		*descLen += len; // desclen == 64, len == 16
	}
	catch(CException* e)
	{
		OpenErrLog();
		CString sErrorW;
		e->GetErrorMessage(sErrorW.GetBuffer(1024), 1024); sErrorW.ReleaseBuffer();
		CStringA err(sErrorW);
		auto rc = e->GetRuntimeClass();
		TRACE("CException<%s>(%s)\n", rc->m_lpszClassName, err);
		rb = FALSE;
	}
	catch(int ie)
	{
		OpenErrLog();
		TRACE("catch(int ie)(%d)\n", ie);
		rb = FALSE;
	}
	return rb;
}

BOOL OSslEncrypt::Decrypt(const unsigned char* orgBuf, int orgLen, unsigned char* descBuf, int* descLen)
{
	//Key, IV 만들기
	unsigned char keyiv[48]{0};// 32개만 사용
	BOOL rb = TRUE;
	EVP_CIPHER_CTX* ctx = nullptr;
	try
	{
		int ret = PKCS5_PBKDF2_HMAC_SHA1((PAS)_pwd, _pwd.GetLength(), (PUCHAR)(PAS)_SALT, tchlen((PAS)_SALT),
			ITER, 32, keyiv);//버퍼 48크기중 32만 사용

		int len = 0;

		ctx = EVP_CIPHER_CTX_new();
		if(ctx == nullptr)
			throw (int)__LINE__;
		KAtEnd d_ctx([&]() { EVP_CIPHER_CTX_free(ctx); });

		EVP_CIPHER_CTX_init(ctx);

		if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, &keyiv[0], &keyiv[16]))
			throw (int)__LINE__;

		if(1 != EVP_DecryptUpdate(ctx, descBuf, &len, orgBuf, orgLen))
			throw (int)__LINE__;
		*descLen = len;

		if(1 != EVP_DecryptFinal_ex(ctx, descBuf + len, &len))
			throw (int)__LINE__;
		*descLen += len;
	}
	catch(CException* e)
	{
		OpenErrLog();
		CString sErrorW;
		e->GetErrorMessage(sErrorW.GetBuffer(1024), 1024); sErrorW.ReleaseBuffer();
		CStringA err(sErrorW);
		auto rc = e->GetRuntimeClass();
		TRACE("CException<%s>(%s)\n", rc->m_lpszClassName, err);
		rb = FALSE;
	}
	catch(int ie)
	{
		OpenErrLog();
		TRACE("catch(int ie)(%d)\n", ie);
		rb = FALSE;
	}
	return rb;
}

#endif

