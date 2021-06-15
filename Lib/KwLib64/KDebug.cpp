#include "pch.h"

#include "inlinefnc.h"
#include "Kw_tool.h"
#include "KDebug.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void KwOutputA(LPCSTR str)
{
	OutputDebugStringA(str);
}
void KwOutput(LPCWSTR str)
{
	KwOutputA(CStringA(str));
}
/*
void KDbTrace(int im, CString& str_)
{
	try
	{
		if(ITraceDb::g_KTrace)
		{
			CString str = KwReturnMySqlString(str_);
			ITraceDb::g_KTrace->_KTrace(im, str);
		}
	}
	catch(CException* e)
	{
		DelException(e);
	}
}*/

int g_opTrace = (1<<8 | 1<<2); // 256 KTraceDb(8)
//int g_opTrace = (1<<3); // CReqPak 디버그 위해 자꾸 죽어서 

void KTraceA(LPCSTR lpFormat_, ...)
{
	if(!KwAttr(g_opTrace, 1 << 0)) // g_opTrace |= 1<<0; 해야 한다.
		return;
	int i = 0;
	try
	{
		CStringA str_;
		CSTRINGA_SPRINTF(lpFormat_, str_);
		KwOutputA(str_);
		//	KDbTrace(str_);
	}
	catch(CException* e)
	{
		DelException(e);
	}
}
void KTrace(LPCTSTR lpFormat_, ...)
{
	if(!KwAttr(g_opTrace, 1<<0)) // g_opTrace |= 1<<0; 해야 한다.
		return;
	int i=0;
	try
	{
		CString str_;
		// 	ATLASSERT( AtlIsValidString( lpFormat_ ) );
		// 	va_list argList;
		// 	va_start( argList, lpFormat_ );
		// 	str_.FormatV( lpFormat_, argList );
		// 	va_end( argList );
		CSTRING_SPRINTF(lpFormat_, str_);
		KwOutput(str_);
		//	KDbTrace(str_);
	}
	catch(CException* e)
	{
		DelException(e);
	}
}
void KTrace1(LPCTSTR lpFormat_, ...)
{
	if(!KwAttr(g_opTrace, 1<<1))
		return;
	try
	{
		CString str_;
		CSTRING_SPRINTF(lpFormat_, str_);
		if(str_.Left(2) == L"5." || str_.Find(L"3.1.2.1.1.1") >= 0)
			OutputDebugString(str_);
		//KDbTrace(1, str_);
	}
	catch(CException* e)
	{
		DelException(e);
	}
}
//이건 kdb로 가야 할듯..
//void KTraceErr(LPCTSTR lpFormat_, ...)
//{
//	try
//	{
//		CString str_;
//		CSTRING_SPRINTF(lpFormat_, str_);
//		OutputDebugString(str_);
//		KDbTrace(10, str_);
//	}
//	catch(CException* e)
//	{
//		DelException(e);
//	}
//}
void KTraceDb(LPCTSTR str_)
{
	if(!KwAttr(g_opTrace, 1<<8))
		return;
	try
	{
		OutputDebugString(str_);
		///?todo 파일로도 저장 하려면 날짜시각으로 파일 만들고(1,2,3) 붙이려면 
		/// KwNextFileName(PWS sFull) 이거 쓰면 디고.
	}
	catch(CException* e)
	{
		DelException(e);
	}
	//	KDbTrace(10, str_);
}
void KTraceDbV(LPCTSTR lpFormat_, ...)
{
	if(!KwAttr(g_opTrace, 1<<8))
		return;
	try
	{
		CString str_;
		CSTRING_SPRINTF(lpFormat_, str_);
		OutputDebugString(str_);
	}
	catch(CException* e)
	{
		DelException(e);
	}
	//	KDbTrace(10, str_);
}
#define KTRACEFUNC(n) void KTrace##n(LPCTSTR lpFormat_, ... )\
{	if(!KwAttr(g_opTrace, 1<<n))		return;\
	try\
	{	CString str_;\
		CSTRING_SPRINTF(lpFormat_, str_)	;\
		OutputDebugString(str_);\
	}catch (CException* e){	DelException(e);}\
}

void KTrace2(LPCTSTR lpFormat_, ...)
{
	if(!KwAttr(g_opTrace, 1<<2))
		return;
	try
	{
		CString str_;
		CSTRING_SPRINTF(lpFormat_, str_);
		OutputDebugString(str_);
	}
	catch(CException* e)
	{
		DelException(e);
	}
	//	KDbTrace(2, str_);
}
void KTrace3(LPCTSTR lpFormat_, ...)
{
	if(!KwAttr(g_opTrace, 1<<3))
		return;
	try
	{
		CString str_;
		CSTRING_SPRINTF(lpFormat_, str_);
		OutputDebugString(str_);
	}
	catch(CException* e)
	{
		DelException(e);
	}
}
KTRACEFUNC(4)
KTRACEFUNC(5)
KTRACEFUNC(6)
KTRACEFUNC(7)
KTRACEFUNC(8)

void KTraceN(int mode, LPCTSTR lpFormat_, ...)
{
	if(mode != 14)
		return;
	try
	{
		CString str_;
		CSTRING_SPRINTF(lpFormat_, str_);
		OutputDebugString(str_);
		//	KDbTrace(10, str_);
	}
	catch(CException* e)
	{
		DelException(e);
	}
}



#ifdef E_CLOCKRUNNING
// Custom error codes
#define E_CLOCKRUNNING          MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 100)
#define E_CITYINROM             MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 101)
#define E_FIELDTOOLARGE         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 102)
#define E_INVALIDREMINDERTIME   MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 103)
#define E_INVALIDDATES          MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 104)
#define E_ALLDAYMEETING         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 105)
#define E_OVERLAPPINGEXCEPTION  MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 106)
#define E_CANTCHANGEDATE        MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 107)
#define E_EXCEPTIONSAMEDAY      MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 108)
#define E_UNWANTEDITEM          MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 109)
#define S_AUTO_CLOSED           MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_ITF, 150)
#endif // _DEBUG
#include <winerror.h>

bool KwCheckHRESULT(HRESULT hr, PWS sFunc, int nLine, PWS s2)
{
	PWS sErr = L"";
	if(FAILED(hr))
	{
		switch(hr)
		{/*
			CASE_ERR(E_CLOCKRUNNING);break;
			CASE_ERR(E_CITYINROM);break;
			CASE_ERR(E_FIELDTOOLARGE);break;
			CASE_ERR(E_INVALIDREMINDERTIME);break;
			CASE_ERR(E_INVALIDDATES);break;
			CASE_ERR(E_ALLDAYMEETING);break;
			CASE_ERR(E_OVERLAPPINGEXCEPTION);break;
			CASE_ERR(E_CANTCHANGEDATE);break;
			CASE_ERR(E_EXCEPTIONSAMEDAY);break;
			CASE_ERR(E_UNWANTEDITEM);break;
			CASE_ERR(S_AUTO_CLOSED);break;
			*/
			CASE_ERR(E_ACCESSDENIED);
			break;
			CASE_ERR(E_INVALIDARG);//hr	0x80070057 {E_INVALIDARG}	HRESULT
			break;
		case 0x8004006e:
			sErr = L"Authentication Read Timeout";//HXR_AUTH_READ_TIMEOUT
			break;
		default:
			_break;
			break;
		}
		//CHResultLog hlog;
		_Formatl(L"hr = %x in %s (%d) : %s, %s", hr, sFunc, nLine, sErr, s2!=NULL?s2:L"");
		//hlog.Log(_s);
		KTrace(L"%s\n", _s);
		return false;
	}
	return true;
	//	hr	0x80070057 {E_INVALIDARG}	HRESULT
		//{0x8004006e, i18n("Authentication Read Timeout")}, 	CHsh20Contact::_FromItem(IDispatch* pCobj = 0x01a42df0) 


}

void DeleteException(CException* e)
{//DelException(e);
	if(e)
	{
		//		PAS pti = typeid(e).name();//항상 class CException * 이라 base type몰라
		PAS pti = e->GetRuntimeClass()->m_lpszClassName;//CSrcException
		if(strstr(pti, "CSrcException") == NULL)
			e->Delete();//DeleteMeSafe(e); see except.cpp  CException::operator delete
	}
}

void CAutoDelException::Free()
{
	if(m_pObj)
	{
		//if(!m_pObj->m_bAutoDelete)//m_pObj->m_bReadyForDelete && 
		DeleteException(m_pObj);
	}
}
