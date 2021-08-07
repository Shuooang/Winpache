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

#pragma message( "\t\t\t  --- START definition --- ")

#if defined(SlnSuareSimul)
#pragma message("\t\t\t\t#define (SlnSuareSimul) HTTPS server")
#define _Use_SSL_Http
#define _Use_GUID
#define _Use_OpenSsl
#endif

#if defined(SlnKHttpsSrv)
#pragma message("\t\t\t\t#define (SlnKHttpsSrv) HTTPS server")
#define _Use_SSL_Http
#define _Use_GUID
#define _Use_OpenSsl
#endif

#if defined(SlnCppServer)
#pragma message("\t\t\t\t#define (SlnCppServer) HTTPS �׽�Ʈ")
#define _Use_SSL_Http
#define _Use_GUID
#endif

#if defined(SlnPetmeSrv)
#pragma message("\t\t\t\t#define (SlnPetmeSrv) HTTPS PetmeSrv ����")
#define _Use_SSL_Http
#define _Use_GUID
#endif
#if defined(SlnPetMeSMP)
#pragma message("\t\t\t\t#define (SlnPetMeSMP) ��������")
#define _Use_SSL_Http
#define _Use_GUID
#endif

/// ////////////////////////////////////////////////////////////////
/// �ַ�ǿ� ���� ���� �Ȱ��� �����Ͻ� ��� �Ͽ� �˰� �Ѵ�.
#if defined(_KwLib)
#pragma message("\t\t\t\t#define (_KwLib)")
#endif
#if defined(_DEBUG)
#pragma message("\t\t\t\t#define (_DEBUG)")
#endif
#if defined(_LIB)
#pragma message("\t\t\t\t#define (_LIB)")
#endif
#if defined(_UNICODE)
#pragma message("\t\t\t\t#define (_UNICODE)")
#endif
#if defined(UNICODE)
#pragma message("\t\t\t\t#define (UNICODE)")
#endif


#ifdef _Use_Compress
#pragma message( "\t\t\t\t#define _Use_Compress")
#endif

#ifdef _Use_WinInet
#pragma message( "\t\t\t\t#define _Use_WinInet")
#endif

#ifdef _Use_HTTPS
#pragma message( "\t\t\t\t#define _Use_HTTPS")
#endif

#ifdef _Use_Comutil
#pragma message( "\t\t\t\t#define _Use_Comutil")
#endif

#ifdef _Use_Version
#pragma message( "\t\t\t\t#define _Use_Version")
#endif

#ifdef _Use_OpenSsl
#pragma message( "\t\t\t\t#define _Use_OpenSsl")
#endif

#ifdef _Use_SSL_Http
#pragma message( "\t\t\t\t#define _Use_SSL_Http")
#endif

#ifdef _Use_DeviceID
#pragma message( "\t\t\t\t#define _Use_DeviceID")
#endif
#ifdef _Use_GUID
#pragma message( "\t\t\t\t#define _Use_GUID")
#endif

#pragma message( "\t\t\t  --- END --- ")

#endif//_Solution_Define