#pragma once

#include <afx.h>
#include "ktypedef.h"


/// <summary>
/// �������� �ַ� �߻� �ϴ� exception�� ó�� �ϴ� ��ü.
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
		_error = (error); ////GetLastError() trow_response(err, "sErr") �ý��ۿ��� ���� ��
		m_nRetCode = (rcde); // client���� �̰��� ���� ���� ���� ���� �ϱ� ����
		m_strError = (sErr);
		m_strStateNativeOrigin = (sState);
		_func = (func);
		_line = (line);
	}

	/// HTTP Request �϶� ���
	int _status{ 200 };// OK HTTP status 400(Bad Request)

	int m_nRetCode{ -1 };//return -1; �� �۾ּ� �ǹ��ִ� ���� - �̸� ����. + �̸� ��Ȳ

	DWORD _error{ 0 };//GetLastError()�ý��ۿ��� ���� ��

	CString m_strError;//e->GetErrorMessage
	CString m_strStateNativeOrigin;//ODBC������ �߰� ���� ���ڿ��� �ִ�.

	CStringA _sExcept;//CException GetRuntimeClass();�� CRuntimeClass::m_lpszClassName
	CStringA _func;
	int _line;
	// Implementation (use AfxThrowDBException to create)
public:

};


/// throw_??? �ø����� �߻� �ϸ� outputâ�� ����� �ǰ� 
///		TRACE �� ������ ����Ŭ�� �ϸ� �ҽ��� �ٷ� ���� ���� �̴�.
#define throw_common() 	TRACE("<< ���⸦ ����Ŭ�� �ϸ� �ҽ��� �̵�. DB t_excepsvr �� ��ϵ�.\n\n\t\t#### EXCEPTION #### %s: %u, %d, %s, %s, - %s(%d)\n\n",\
	_ke->_sExcept, _ke->_error, _ke->m_nRetCode, CStringA(_ke->m_strError), \
		CStringA(_ke->m_strStateNativeOrigin), _ke->_func, _ke->_line ); throw _ke

//GetLastError() ���� ���� �ٶ�
#define throw_gen(n, s) {auto _ke = new KException("throw_gen", n, 0, s   , NULL, __FUNCTION__, __LINE__);\
	throw_common(); }


#define throw_num(n)    {auto _ke = new KException("throw_num", n, 0, NULL, NULL, __FUNCTION__, __LINE__);\
	throw_common(); }

// return ��(����������)�� �ٶ�
#define throw_rcd(r, s) {auto _ke = new KException("throw_gen", 0, r, s   , NULL, __FUNCTION__, __LINE__);\
	throw_common(); }
	
	// �޽����� �ٶ�
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
