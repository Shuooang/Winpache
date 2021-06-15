#pragma once

#include <afx.h>
#include "ktypedef.h"


/// <summary>
/// 서버에서 주로 발생 하는 exception을 처리 하는 객체.
/// 
/// </summary>
class KException : public CException
{
	DECLARE_DYNAMIC(KException)

	// Attributes
public:
	BOOL IsAutoDelete()
	{
#ifdef _DEBUG
		return m_bReadyForDelete;
#else
		return TRUE;
#endif
	}
	KException(PAS sExcept, DWORD error, int rcde, PWS sErr, PWS sState, PAS func = NULL, int line = 0)
		//: _sExcept(sExcept), _error(error)
		//, m_nRetCode(rcde), m_strError(sErr), m_strStateNativeOrigin(sState)
		//, _func(func), _line(line)
	{
		Init(sExcept, error, rcde, sErr, sState, func, line);
	}
	KException(PAS sExcept, DWORD error, int rcde, PAS sErrA, PWS sState, PAS func = NULL, int line = 0)
	{
		Init(sExcept, error, rcde, CString(sErrA), sState, func, line);
	}
	void Init(PAS sExcept, DWORD error, int rcde, PWS sErr, PWS sState, PAS func = NULL, int line = 0)
	{
		_sExcept = (sExcept);
		_error = (error); ////GetLastError() trow_response(err, "sErr") 시스템에서 얻은 값
		m_nRetCode = (rcde); // client에서 이값에 따라 무슨 짓을 구분 하기 위함
		m_strError = (sErr);
		m_strStateNativeOrigin = (sState);
		_func = (func);
		_line = (line);
	}

	/// HTTP Request 일때 사용
	int _status{ 200 };// OK HTTP status 400(Bad Request)

	int m_nRetCode{ -1 };//return -1; 이 앱애서 의미있는 구분 - 이면 오류. + 이면 상황

	DWORD _error{ 0 };//GetLastError()시스템에서 얻은 값

	CString m_strError;//e->GetErrorMessage
	CString m_strStateNativeOrigin;//ODBC에러는 추가 오류 문자열이 있다.

	CStringA _sExcept;//CException GetRuntimeClass();의 CRuntimeClass::m_lpszClassName
	CStringA _func;
	int _line;
	// Implementation (use AfxThrowDBException to create)
public:

};


/// throw_??? 시리지가 발생 하면 output창에 출력이 되고 
///		TRACE 윗 라인을 더블클릭 하면 소스로 바로 가기 위함 이다.
#define throw_common() 	TRACE("<< 여기를 더블클릭 하면 소스로 이동. DB t_excepsvr 에 기록됨.\n\n\t\t#### EXCEPTION #### %s: %u, %d, %s, %s, - %s(%d)\n\n",\
	_ke->_sExcept, _ke->_error, _ke->m_nRetCode, CStringA(_ke->m_strError), \
		CStringA(_ke->m_strStateNativeOrigin), _ke->_func, _ke->_line ); throw _ke

//GetLastError() 값을 직접 줄때
#define throw_gen(n, s) {auto _ke = new KException("throw_gen", n, 0, s   , NULL, __FUNCTION__, __LINE__);\
	throw_common(); }


#define throw_num(n)    {auto _ke = new KException("throw_num", n, 0, NULL, NULL, __FUNCTION__, __LINE__);\
	throw_common(); }

// return 값(음수도가능)을 줄때
#define throw_rcd(r, s) {auto _ke = new KException("throw_gen", 0, r, s   , NULL, __FUNCTION__, __LINE__);\
	throw_common(); }
	
	// 메시지만 줄때
#define throw_str(s)  {auto _ke = new KException("throw_str", 0, 0, s   , NULL, __FUNCTION__, __LINE__);\
	throw_common(); }

#define throw_response(status, s) {auto _ke = new KException("throw_response", 0, 0, s   , NULL, __FUNCTION__, __LINE__);\
	_ke->_status = status;\
	throw_common();}

#define throw_response_code(status, rcode, s) {auto _ke = new KException("throw_response", rcode, rcode, s   , NULL, __FUNCTION__, __LINE__);\
	_ke->_status = status;\
	throw_common();}

#define throw_field(field) {auto _ke = new KException("Field exception", 400, 100, field, NULL, __FUNCTION__, __LINE__);\
	throw_common(); }


#define throw_BadRequest(err, s) {auto _ke = new KException("throw_response", err, 0, s   , NULL, __FUNCTION__, __LINE__);\
	_ke->_status = 400;\
	throw_common();}
