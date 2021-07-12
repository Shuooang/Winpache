#pragma once

#include "KwLib64/Dictionary.h"
#include "KwLib64/JSON/JSON.h"
#include "KwLib64/Recordset.h"
#include "KwLib64/Lock.h"

#include "HttpsSvr.h"

/*
	// step 1 : app.h��
	CResponse1* _svr; 

	 // step 2 : app.cpp InitInstance��
	auto api = new CApiBase();
	_svr = new CResponse1(api);//CApiBase
	api->MapRemoteFunctions();

*/




#ifdef _DEBUG
#define KWMESSAGEBOXERROR KwMessageBoxError
#else
#define KWMESSAGEBOXERROR TRACE
#endif


/// CApiBase ������ ����.

/// ����: Cexception, "xxx", (...) 3���� �� KException���� rethrow �ϱ� �����̴�.
#define CATCH_GE catch(KException* e)\
	{	TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTIONW__, __LINE__);\
		throw e;\
	} catch(CException* e)\
	{	auto buf = new TCHAR(1024);\
		KAtEnd d_buf([&]() { delete buf;});\
		e->GetErrorMessage(buf, 1000);\
		TRACE(L"CException:%s - %s %d\n", buf, __FUNCTIONW__, __LINE__);\
		throw new KException("CException", GetLastError(), 0, buf, NULL, __FUNCTION__, __LINE__);\
	} catch(TException ex)\
	{	TErrCode ec = ex.code();\
		TRACE("catch boost::systemerror %s  - %s %d\n", ec.message().c_str(), __FUNCTION__, __LINE__);\
		throw new KException("boost::systemerror", ec.value(), 0, CStringW(ec.message().c_str()), NULL, __FUNCTION__, __LINE__);\
	} catch(std::exception &e)\
	{	TRACE("catch std::exception %s  - %s %d\n", e.what(), __FUNCTION__, __LINE__);\
		throw new KException("std::exception", -1, 0, CStringW(e.what()), NULL, __FUNCTION__, __LINE__);\
	} catch(PWS e)\
	{	TRACE("catch LPCWSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__);\
		throw new KException("LPCWSTR", GetLastError(), 0, e, NULL, __FUNCTION__, __LINE__);\
	} catch(PAS e)\
	{	TRACE("catch LPCSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__);\
		throw new KException("LPCSTR", GetLastError(), 0, CStringW(e), NULL, __FUNCTION__, __LINE__);\
	} catch(...)\
	{	TRACE("catch ...  - %s %d\n", __FUNCTION__, __LINE__);\
		throw new KException("Unknown", GetLastError(), 0, L"Unknown catch(...) Error.", NULL, __FUNCTION__, __LINE__);\
	}
		//KWMESSAGEBOXERROR(CStringW(e));
		//KWMESSAGEBOXERROR(L"catch ... �𸣴� ����");
/// ����: DB�� ODBC ���� �޽����� ì��� ����.
// NO transaction Rollback + rethrow
#define CATCH_DB catch(CDBException* e)\
	{	TRACE("CDBException:%s - %s %d\n", CStringA(e->m_strError), __FUNCTION__, __LINE__);\
		throw new KException("CDBException", GetLastError(), e->m_nRetCode, e->m_strError, e->m_strStateNativeOrigin, __FUNCTION__, __LINE__);\
	} CATCH_GE

/// _CRT_WIDE

#define CATCH_GETR catch(KException* e)\
	{	TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTIONW__, __LINE__);\
		try{_db.TransRollback();} catch(...){ e->m_strError += L" + exception in TransRollback";}\
		throw e;\
	} catch(CException* e)\
	{	auto buf = new WCHAR(1024);\
		try{_db.TransRollback();} catch(...){}\
		KAtEnd d_buf([&]() { delete buf;});\
		e->GetErrorMessage(buf, 1000);\
		TRACE(L"CException:%s - %s %d\n", buf, __FUNCTIONW__, __LINE__);\
		throw new KException("CException", GetLastError(), 0, buf, NULL, __FUNCTION__, __LINE__);\
	} catch(TException ex)\
	{	TErrCode ec = ex.code();\
		try{_db.TransRollback();} catch(...){}\
		TRACE("catch boost::systemerror %s  - %s %d\n", ec.message().c_str(), __FUNCTION__, __LINE__);\
		throw new KException("boost::systemerror", ec.value(), 0, CStringW(ec.message().c_str()), NULL, __FUNCTION__, __LINE__);\
	} catch(std::exception &e)\
	{	TRACE("catch std::exception %s  - %s %d\n", e.what(), __FUNCTION__, __LINE__);\
		try{_db.TransRollback();} catch(...){}\
		throw new KException("std::exception", -1, 0, CStringW(e.what()), NULL, __FUNCTION__, __LINE__);\
	} catch(PWS e)\
	{	TRACE("catch LPCWSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__);\
		try{_db.TransRollback();} catch(...){}\
		throw new KException("LPCWSTR", GetLastError(), 0, e, NULL, __FUNCTION__, __LINE__);\
	} catch(PAS e)\
	{	TRACE("catch LPCSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__);\
		try{_db.TransRollback();} catch(...){}\
		throw new KException("LPCSTR", GetLastError(), 0, CStringW(e), NULL, __FUNCTION__, __LINE__);\
	} catch(...)\
	{	TRACE("catch ...  - %s %d\n", __FUNCTION__, __LINE__);\
		try{_db.TransRollback();} catch(...){}\
		throw new KException("Unknown", GetLastError(), 0, L"Unknown catch(...) Error.", NULL, __FUNCTION__, __LINE__);\
	}

/// ����: Ʈ����� ���� Rollback �ϱ� ����.
// transaction Rollback + rethrow. auto commit�� ���� try {}�� ������ ������ commit�Ǵ� ������ �ִ�.
#define CATCH_DBTR catch(CDBException* e)\
	{	TRACE("CDBException:%s - %s %d\n", CStringA(e->m_strError), __FUNCTION__, __LINE__);\
		try{_db.TransRollback();} catch(...){ e->m_strError += L" + exception in TransRollback";}\
		throw new KException("CDBException", GetLastError(), e->m_nRetCode, e->m_strError, e->m_strStateNativeOrigin, __FUNCTION__, __LINE__);\
	} CATCH_GETR

	/* ����׿�~~
	catch(CDBException* e)
	{	TRACE("CDBException:%s - %s %d\n", CStringA(e->m_strError), __FUNCTION__, __LINE__);
		throw new KException("CDBException", GetLastError(), e->m_nRetCode, e->m_strError, e->m_strStateNativeOrigin, __FUNCTION__, __LINE__);
	}
	catch(KException* e)
	{	TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTION__, __LINE__);
		throw e;
	}
	catch(CException* e)
	{	auto buf = new TCHAR(1024);
		KAtEnd d_buf([&]() { delete buf; });
		e->GetErrorMessage(buf, 1000);
		TRACE(L"CException:%s - %s %d\n", buf, __FUNCTION__, __LINE__); \
		throw new KException("CException", GetLastError(), 0, buf, NULL, __FUNCTION__, __LINE__);
	}
	catch(PAS e)
	{	TRACE("catch PAS %s  - %s %d\n", e, __FUNCTION__, __LINE__);
		KWMESSAGEBOXERROR(CStringW(e));
		throw new KException("string", GetLastError(), 0, CStringW(e), NULL, __FUNCTION__, __LINE__);
	}
	catch(...)
	{	TRACE("catch ...  - %s %d\n", __FUNCTION__, __LINE__);
		KWMESSAGEBOXERROR(L"catch ... �𸣴� ����");
		throw new KException("Unknown", GetLastError(), 0, L"Unknown ... Error.", NULL, __FUNCTION__, __LINE__);
	}
	*/


class CApiBase;// ���� ���� ���ϋ��� �̷��� declare�� �ϰ�, �Ʒ� ���� define�Ѵ�.


class CResponse1
{
public:
	CResponse1(SHP<CApiBase> api = NULL);//, BOOL bOwner = TRUE);
	~CResponse1();

	/// ���� ���� �Լ��� ����ü ��ü�� ������
	/// base type share_ptr�� �Ҵ� �Ҷ��� derived type��ü �ν��Ͻ��� ���� �´�.
	SHP<CApiBase> _api; //  = make_shared< ApiSite1 >();
	
	/// <summary>
	/// �� _api�� ������. TRUE�̸� �̰�ü ������ ���� ���� �Ѵ�.
	/// </summary>
	BOOL _bOwner;


	void InitReponser(BOOL bDB = TRUE);


	int ResponseFileForGet(string url, KBinary& fbuf);

	int ResponseForGet(KSessionInfo& sinfo, stringstream& rpost);
	int ResponseForPost(KSessionInfo& sinfo, SHP<KBinData> body, stringstream& rpost);
	int ResponseImageUpload(KSessionInfo& sinfo, string uuid, PAS data, size_t len, string fname, string contType, string contLeng, Tas& rpost);
};


//2020-12-08 14:35:57 ���� ������ ����
string JError(PAS msg, int rv = -1, int status = 400); // FAILED 400:Bad request
void JError(JObj& jres, PAS msg, int rv = -1, int status = 400);

//DB Error. rv = -100;//
//Unknown Error rv = -1000;


typedef int(_stdcall* API_SITE)(KDatabase&, JObj&, JObj&, int);
