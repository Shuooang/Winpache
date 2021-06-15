#pragma once


#include "stdecl.h"


typedef	LPCWSTR PWS;
typedef	LPCSTR PAS;
typedef POSITION POS;
typedef HRESULT hres;







// pragma once가 안먹고 여러번 출력 된다
#ifndef _Solution_Define

#define _Solution_Define

/// ////////////////////////////////////////////////////////////////
/// 각프로젝트에 Sln$(SolutionName)를 전처리기에 넣어서 특정 기능 코드를
/// 컴파일 할건지 정한다. 자주 안쓰는 경우에 한다.


#if defined(SlnKHttpsSrv)
#pragma message("defined(SlnKHttpsSrv) HTTPS server")
#define _Use_SSL_Http
#define _Use_GUID
#endif

#if defined(SlnCppServer)
#pragma message("defined(SlnCppServer) HTTPS 테스트")
#define _Use_SSL_Http
#define _Use_GUID
#endif

#if defined(SlnPetmeSrv)
#pragma message("defined(SlnPetmeSrv) HTTPS PetmeSrv 서버")
#define _Use_SSL_Http
#define _Use_GUID
#endif
#if defined(SlnPetMeSMP)
#pragma message("defined(SlnPetMeSMP) 샵관리앱")
#define _Use_SSL_Http
#define _Use_GUID
#endif

/// ////////////////////////////////////////////////////////////////
/// 솔루션에 따라 정의 된것을 컴파일시 출력 하여 알게 한다.
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