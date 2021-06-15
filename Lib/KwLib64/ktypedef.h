#pragma once


#include "stdecl.h"


typedef	LPCWSTR PWS;
typedef	LPCSTR PAS;
typedef POSITION POS;
typedef HRESULT hres;







// pragma once�� �ȸ԰� ������ ��� �ȴ�
#ifndef _Solution_Define

#define _Solution_Define

/// ////////////////////////////////////////////////////////////////
/// ��������Ʈ�� Sln$(SolutionName)�� ��ó���⿡ �־ Ư�� ��� �ڵ带
/// ������ �Ұ��� ���Ѵ�. ���� �Ⱦ��� ��쿡 �Ѵ�.


#if defined(SlnKHttpsSrv)
#pragma message("defined(SlnKHttpsSrv) HTTPS server")
#define _Use_SSL_Http
#define _Use_GUID
#endif

#if defined(SlnCppServer)
#pragma message("defined(SlnCppServer) HTTPS �׽�Ʈ")
#define _Use_SSL_Http
#define _Use_GUID
#endif

#if defined(SlnPetmeSrv)
#pragma message("defined(SlnPetmeSrv) HTTPS PetmeSrv ����")
#define _Use_SSL_Http
#define _Use_GUID
#endif
#if defined(SlnPetMeSMP)
#pragma message("defined(SlnPetMeSMP) ��������")
#define _Use_SSL_Http
#define _Use_GUID
#endif

/// ////////////////////////////////////////////////////////////////
/// �ַ�ǿ� ���� ���� �Ȱ��� �����Ͻ� ��� �Ͽ� �˰� �Ѵ�.
#if defined(_KwLib)
#pragma message("defined(_KwLib)")
#endif
#if defined(_DEBUG)
#pragma message("defined(_DEBUG)")
#endif
#if defined(_LIB)
#pragma message("defined(_LIB)")
#endif
#if defined(_UNICODE)
#pragma message("defined(_UNICODE)")
#endif
#if defined(UNICODE)
#pragma message("defined(UNICODE)")
#endif


#ifdef _Use_Compress
#pragma message( "#define _Use_Compress")
#endif

#ifdef _Use_WinInet
#pragma message( "#define _Use_WinInet")
#endif

#ifdef _Use_HTTPS
#pragma message( "#define _Use_HTTPS")
#endif

#ifdef _Use_Comutil
#pragma message( "#define _Use_Comutil")
#endif

#ifdef _Use_Version
#pragma message( "#define _Use_Version")
#endif


#ifdef _Use_SSL_Http
#pragma message( "#define _Use_SSL_Http")
#endif

#ifdef _Use_DeviceID
#pragma message( "#define _Use_DeviceID")
#endif
#ifdef _Use_GUID
#pragma message( "#define _Use_GUID")
#endif


#endif//_Solution_Define