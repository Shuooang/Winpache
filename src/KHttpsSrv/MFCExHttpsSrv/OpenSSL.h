#pragma once

#include "KwLib64/ktypedef.h"

#ifdef _Use_OpenSsl
//#define ITER     1 	

// from github src
#define OUR_SALT "fileenc_oursalt"
#define ITER 4096


class OSslEncrypt
{
public:
	OSslEncrypt(PAS pwd = NULL, PAS salt = NULL)
	{
		if(pwd)
			_pwd = pwd;
		if(salt)
			_SALT = salt;
	}
	void OpenErrLog();

	CStringA _pwd{"9570739"};
	CStringA _SALT{"dnflWksakt"};

	BOOL Encrypt(const unsigned char* orgBuf, int orgLen, unsigned char* descBuf, int* descLen);

	BOOL Decrypt(const unsigned char* orgBuf, int orgLen, unsigned char* descBuf, int* descLen);
};




#endif // _Use_OpenSsl
