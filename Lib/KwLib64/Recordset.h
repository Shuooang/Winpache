#pragma once

#include <afxdb.h>

#include <vector>
#include <memory>
#include <functional>

#include "ktypedef.h"
#include "KBinary.h"
#include "tchtool.h"
#include "JSON/JSONValue.h"//?deprecated
#include "Dictionary.h"
#include "KVal.h"
#include "KTemple.h"
#include "SrvException.h"
#include "Kw_tool.h"
#include "RecQuat.h"
#include "KJson.h"
#include "TimeTool.h"

using namespace Kw;

using std::shared_ptr;
using std::vector;
using std::function;

typedef shared_ptr<CDBVariant> ShDBVar;
typedef vector<ShDBVar> VtRow;
typedef shared_ptr<VtRow> ShRow;

/// <summary>
/// KDatabase::ExecuteSQL 할때 파라미터 간단히
/// </summary>
class KDataPacket
{
public:
	
	// ex: KDataPacket(sErr, 0, SQL_C_CHAR, SQL_CHAR),
	// ex: KDataPacket(sErr, 0, SQL_C_BINARY, SQL_LONGVARBINARY),
	KDataPacket(PAS data, INT64 len = -1, int cType = SQL_C_BINARY, int sqlType = SQL_LONGVARBINARY)
		: _data((char*)data), _len(len), _cType(cType), _sqlType(sqlType)
	{
		if(len <= 0)// len이 생략 되면 null terminated string pointer로 간주
		{
			_len = tchlen((PAS)data);
		}
	}
	INT64 _len;//SQLLEN
	char* _data;//PTR : void*
	int _cType;//SQLSMALLINT
	int _sqlType;//SQLSMALLINT
};

//template <typename TJSON>
//class Quat<TJSON>;

class KDatabase : public CDatabase
{
public:
	KDatabase()
		: _tLastAcess(0)
		, _secReopenInterval(60000)
		, _mxDbAcess(FALSE, L"_mxDbAcess")
	{
	}
	virtual ~KDatabase()
	{
		TRACE("~KDatabase()\n");
	}
	CMutex _mxDbAcess;

	/// <summary>
	/// 마지막 접속이나 쿼리 시각
	/// </summary>
	ULONGLONG _tLastAcess;//GetTickCount64

	/// <summary>
	/// 이값이 60초라면 60초간 어떤 access도 없었으면 gone away발생 할수도 있으니 다시 연결 한다.
	/// </summary>
	DWORD _secReopenInterval;

	/// <summary>
	/// 기존 거에 _tLastAcess만 갱신 한다.
	/// </summary>
	virtual BOOL OpenEx(LPCTSTR lpszConnectString, DWORD dwOptions = 0);

	/// TRACE가 지저분해서 덮어쓴다.
	virtual BOOL Check(RETCODE nRetCode) const;
	BOOL PASCAL CheckHstmt(RETCODE, HSTMT hstmt) const;

	/// <summary>
	/// Close()하고 다시 OpenEx 하는데 기존 GetConnect()에서 'ODBC;'만 떼고 다시 부른다.
	/// 주의점은 딸린 CRecordset의 m_pDatabase가 NULL되므로 다시 할당 해줘야 한다.
	/// </summary>
	void Reopen();
	bool IsReopenable();
	string TimeCheckReopen();


	/// 로그용 DB
	shared_ptr<KDatabase> _dbLog;

	/// this에 query가 불렸을때 로그를 쌓기 위해 불려지는 람다함수 변수
	shared_ptr<function<int(PWS, DWORD)>> _fncDbLog;
	
	/// 로그용 DB를 붙이지 않고 생성할 때 
	void CreateDbLog(PWS dsnLog)
	{
		ASSERT(!_dbLog);
		//_dbLog = shared_ptr<KDatabase>(new KDatabase());
		_dbLog = std::make_shared<KDatabase>();
		_dbLog->OpenEx(dsnLog);
		/// 부른 쪽에서 _dbLog->ExecuteSQL(L"use `winpache`"); 할수도 있다.
	}

	/// this에 query가 불렸을때 로그를 쌓기 위해 불려지는 람다함수 등록
	// 주로 한꺼번에 로그를 쌓기 위해 매번 부르지 않고 버퍼링 한다.
	template<typename TFNC> void InitDbLog(TFNC fnc)
	{
		//_fncDbLog = shared_ptr<function<int(PWS, DWORD)>>(new function<int(PWS, DWORD)>(fnc));
		if(!_fncDbLog)
			_fncDbLog = std::make_shared<function<int(PWS, DWORD)>>(fnc);
	}

	CKCriticalSection _csTransaction;//트랜잭션 있는 함수 연거푸 불려 질때 트랜잭션이 중첩 된다.
	//CSyncAutoLock __lock##n(&(sobj), TRUE, __FUNCTION__, __LINE__, #sobj)
	void TransBegin()
	{
		_csTransaction.Lock();
		this->BeginTrans();
	}
	void TransCommit()
	{
		this->CommitTrans();
		_csTransaction.Unlock();
	}
	void TransRollback()
	{
		this->Rollback();
		_csTransaction.Unlock();
	}

	/// 
	/// <summary>
	/// m_strStateNativeOrigin에서 State:08S01 의 값을 뽑아 리턴 한다.
	/// </summary>
	/// <param name="e">CDBException*</param>
	/// <returns>CString State</returns>
	static CString GetState(CDBException* e)
	{
		//[CDBException] 
		//	m_strError=L"MySQL server has gone away\n" 
		//	m_strStateNativeOrigin=L"State:08S01,Native:2006,Origin:[ma-3.1.10][10.4.12-MariaDB]\n"
		auto sno = e->m_strStateNativeOrigin;
		CString stt;
		int i0 = sno.Find(',');
		if(i0 >= 0)
		{
			if(sno.Left(6) == L"State:")
				stt = sno.Mid(6, i0 -6);
		}
		return stt;
	}


	static BOOL IsGoneAway(CDBException* e)
	{
		CString stt = GetState( e);
		return stt == L"08S01";
	}


	static CStringA GetTypeToStr(int typeSql);

	struct CDbBinary
	{
		PTR pData;			// Data Point ARRAY 
			SQLLEN cbRv;			// 실제 전달된 length ARRAY
			SQLSMALLINT cType;		// array of c type
			SQLSMALLINT sqlType;
	};
	enum {
		eNone = 0, 
		eNoLog = 1 << 3 };

	void ExecuteSQL(LPCWSTR lpszSQL,
					PTR pData,			// Data Point ARRAY 
					SQLLEN cbRv,			// 실제 전달된 length ARRAY
					SQLSMALLINT cType = SQL_C_BINARY,		// array of c type
					SQLSMALLINT sqlType = SQL_LONGVARBINARY, int iOp = 0);
	void ExecuteSQL(LPCWSTR lpszSQL, KDataPacket dts[], int numVar, int iOp = 0);
	void ExecuteSQL(LPCWSTR lpszSQL, int numVar = 0, // '?'의 갯수 :  BIND해야할 PARAM 갯수
					PTR* pData = NULL, SQLLEN* cbRv = NULL, SQLSMALLINT* cType = NULL, SQLSMALLINT* sqlType = NULL, int iOp = 0);
	void ExecuteSQL(Quat& qs, int iOp = 0);

	static int RegODBCMySQL(LPCWSTR sDSN, KWStrMap& kmap);

	static int RegGetODBCMySQL(LPCWSTR sDSNA, KWStrMap& kmap);


	shared_ptr<function<void(CString, CString, CDBException*)>> _fncExceptionCTF;

	template<typename FNC>
	void CreateTablesInFolderLD(PWS folder, FNC fnc)
	{//default parameter가 있어도 안주면 못찾네. template라 그런가?
		_fncExceptionCTF = make_shared<function<void(CString, CString, CDBException*)>>(fnc);
		CreateTablesInFolder(folder);
	}
	void CreateTablesInFolder(PWS folder);

	static void CreateTable(KDatabase& _db, PAS fname);
	static void SplitSQL(CStringW& sqlw, KArray<wstring>& ar);

	// use CString GetConnect();
	CString GetConnectedDsn()
	{
		return m_strConnect;
	}
	static CKCriticalSection s_csDbConnect;
	static SHP<KDatabase> getDbConnected(wstring dsn, int sec = 60);

};
/// OpenEx한 tick과 KDatabase를 같이 가지고 있는 객체
class KDatabaseOdbc
{
public:
	KDatabaseOdbc()
	{
	}
	KDatabaseOdbc(wstring dsn)
	{
		if(dsn.length() > 0)
		{
			CreateDb(dsn);
			_dsn = dsn;
		}
	}
	~KDatabaseOdbc()
	{
		//DeleteMeSafe(_pdb);
	}
	SHP<KDatabase> _sdb;
	wstring _dsn;
	ULONGLONG _tick{0};
	void CreateDb(wstring dsn)
	{
		ASSERT(!_sdb);
		_sdb = make_shared<KDatabase>();
		_sdb->OpenEx(dsn.c_str());
	}
};

/// db log를 쌓으려면 threadId : RemoteFunction : query 이렇게 3차원 dic이 필요
class KQueryLog
{
public:
	CStringW _sql;
	DWORD _elapsed;
	KQueryLog(PWS sql = nullptr, DWORD elapsed = 0)
		: _sql(sql), _elapsed(elapsed)
	{}
};


class KRecordset : public CRecordset
{
public:
	KRecordset(CDatabase* db)
		: CRecordset(db)
		,  _fncFetch(NULL)
		, _point(6)
	{
	}
	~KRecordset()
	{
		DeleteMeSafe(_fncFetch);
	}
	
	CStrBufferT<CString, LPCTSTR> _buf;

	void ReopenDb();


	/// TRACE가 지저분해서 덮어쓴다.
	virtual BOOL Check(RETCODE nRetCode) const; // general error check

	/// <summary>
	/// 값이 소수점일때 Cell에서 소수점 아래 디폴드 값
	/// </summary>
	int _point;
	void SetFloatPoint(int point)	{
		_point = point;
	}


	/// <summary>
	/// 컬럼 정보를 저장해 둔다.
	/// </summary>
	vector<shared_ptr<CODBCFieldInfo>> _finfos;


	/// <summary>
	/// 테이블 데이터를 보관한다. CDBVariant
	/// </summary>
	vector<ShRow> _tbl;

	BOOL OpenSelectFetch(Quat& qs);
	BOOL OpenSelectFetch(PWS sql);
	BOOL OpenSelect(PWS sql);
	BOOL OpenSelect(Tss& ss);
	BOOL OpenSelect(Quat& qs);
	
	function<int(int,int, CDBVariant&)>* _fncFetch;

	template<typename FNC>
	int Fetch(FNC fnc = [](int, int, CDBVariant&){})
	{//default parameter가 있어도 안주면 못찾네. template라 그런가?
		_fncFetch = 
#ifdef _DEBUG
			DEBUG_NEW
#else
			new
#endif // _DEBUG
			function<int(int, int, CDBVariant&)>(fnc);
		DoFetch();
		return 0;
	}
	int Fetch()
	{
		_fncFetch = NULL;
		DoFetch();
		return 0;
	}
	int DoFetch();

	CString GetFieldName(int col);
	int GetFieldNames(CStringArray& ar);

	int ColSize()
	{
		return (int)_finfos.size();
	}
	int RowSize()
	{
		return (int)_tbl.size();
	}

	int GetColNum(CString& sc);
	int GetColNum(PAS sc)
	{
		return GetColNum(CString(sc));
	}

	CDBVariant* GetCell(int r, int c);
/*	CDBVariant* GetCell(int r, CString& sc)
	{
		int c = GetColNum(sc);
		return GetCell(r, c);
	}
	CDBVariant* GetCellP(int r, PWS sc)
	{
		return GetCell(r, CString(sc));
	}*/

	int GetType(int r, int c);

	BOOL IsNull(int r, int c);

	void Cell(CStringW& s, int r, int c, int point = 0, int iOp = 0);
	CStringW CellS(int r, int c, int point = 0, int iOp = 0)
	{
		return Cell(r, c, point, iOp);
	}
	CStringW CellS(int r, PAS pc, int point = 0, int iOp = 0)
	{
		return Cell(r, pc, point, iOp);
	}
	CStringW Cell(int r, int c, int point = 0, int iOp = 0);
	CStringW Cell(int r, PAS pc, int point = 0, int iOp = 0)
	{
		int c = GetColNum(pc);
		return Cell(r, c, point, iOp);
	}
	CStringA CellSA(int r, int c, int point = 0, int iOp = 0);

	int CellI(int r, int c);
	int CellI(int r, PAS pc)
	{
		int c = GetColNum(pc);
		return CellI(r, c);
	}


	double CellD(int r, int c);
	double CellD(int r, PAS pc)
	{
		int c = GetColNum(pc);
		return CellD(r, c);
	}
	float CellF(int r, int c);
	float CellF(int r, PAS pc)
	{
		int c = GetColNum(pc);
		return CellF(r, c);
	}
	CKTime CellT(int r, int c);
	CKTime CellT(int r, PAS pc)
	{
		int c = GetColNum(pc);
		return CellT(r, c);
	}
	HGLOBAL CellBinH(int r, int c, UINT64& len);
	BOOL CellBin(int r, int c, KBinary* pBin);







#define SetJsonStrKey(TYPE) \
	int c = GetColNum(srcCol);\
	if(nullptr == tarCol)\
		tarCol = srcCol;\
	return SetJson##TYPE(jsn, r, c, tarCol);


	template<typename TJSON>
	bool SetJsonS(TJSON& jsn, int r, PAS srcCol, PAS tarCol = nullptr)
	{
		SetJsonStrKey(S);
	}
	template<typename TJSON>
	bool SetJsonD(TJSON& jsn, int r, PAS srcCol, PAS tarCol = nullptr)
	{
		SetJsonStrKey(D);
	}
	template<typename TJSON>
	bool SetJsonI(TJSON& jsn, int r, PAS srcCol, PAS tarCol = nullptr)
	{
		SetJsonStrKey(I);
	}
	template<typename TJSON>
	bool SetJsonF(TJSON& jsn, int r, PAS srcCol, PAS tarCol = nullptr)
	{
		SetJsonStrKey(F);
	}
	template<typename TJSON>
	bool SetJsonT(TJSON& jsn, int r, PAS srcCol, PAS tarCol = nullptr)
	{
		SetJsonStrKey(T);
	}





	//bool SetJsonS(JSONObject& jsn, int r, int c, PAS key);
	//bool SetJsonD(JSONObject& jsn, int r, int c, PAS key);
	//bool SetJsonI(JSONObject& jsn, int r, int c, PAS key);
	//bool SetJsonF(JSONObject& jsn, int r, int c, PAS key);
	//bool SetJsonT(JSONObject& jsn, int r, int c, PAS key);

	//bool SetJsonS(CJsonPbj& jsn, int r, int c, PAS key);
	//bool SetJsonD(CJsonPbj& jsn, int r, int c, PAS key);
	//bool SetJsonI(CJsonPbj& jsn, int r, int c, PAS key);
	//bool SetJsonF(CJsonPbj& jsn, int r, int c, PAS key);
	//bool SetJsonT(CJsonPbj& jsn, int r, int c, PAS key);
	template<typename TJSON>
	bool SetJsonNull(TJSON& jso, int r, int c, PAS key)
	{
		CDBVariant* v = GetCell(r, c);
		if(v == NULL)
			return false;
		if(v->m_dwType == DBVT_NULL)
		{
			jso(key) = ONULL();
			return false;
		}
		return true;
	}




#define SetJsonTYPE(TYPE) \
	if(!SetJsonNull(jso, r, c, key))\
		return false;\
	jso(key) = Cell##TYPE(r, c);\
	return true;

	bool SetJsonD(CJsonPbj& jso, int r, int c, PAS key)
	{
		SetJsonTYPE(D);
	}
	bool SetJsonF(CJsonPbj& jso, int r, int c, PAS key)
	{
		SetJsonTYPE(F);
	}
	bool SetJsonI(CJsonPbj& jso, int r, int c, PAS key)
	{
		SetJsonTYPE(I);
	}
	bool SetJsonS(CJsonPbj& jso, int r, int c, PAS key)
	{
		SetJsonTYPE(S);
	}
	bool SetJsonT(CJsonPbj& jso, int r, int c, PAS key)
	{
		//SetJsonTYPE(T);
		SetJsonTYPE(S);
		//if(!SetJsonNull(jso, r, c, key))
		//	return false; 
		//jso(key) = Cell(r, c); //CellT 대신 문자열 그대로 넣는다.
		//return true;
	}

#define SetCJsonTYPE(TYPE) \
	CJsonPbj jso(&jsn);\
	return SetJson##TYPE(jso, r, c, key);

	bool SetJsonF(JSONObject& jsn, int r, int c, PAS key)
	{
		SetCJsonTYPE(F);
	}
	bool SetJsonD(JSONObject& jsn, int r, int c, PAS key)
	{
		SetCJsonTYPE(D);
	}
	bool SetJsonI(JSONObject& jsn, int r, int c, PAS key)
	{
		SetCJsonTYPE(I);
	}
	bool SetJsonS(JSONObject& jsn, int r, int c, LPCSTR key)
	{
		SetCJsonTYPE(S);
	}
	bool SetJsonT(JSONObject& jsn, int r, int c, LPCSTR key)
	{
		SetCJsonTYPE(T);
	}

#define SetKwJObjTYPE(TYPE) \
	if(!SetJsonNull(jso, r, c, key))\
		return false;\
	jso(key) = Cell##TYPE(r, c);\
	return true;

	bool SetJsonD(JObj& jso, int r, int c, PAS key)
	{
		SetKwJObjTYPE(D);
	}
	bool SetJsonF(JObj& jso, int r, int c, PAS key)
	{
		SetKwJObjTYPE(F);
	}
	bool SetJsonI(JObj& jso, int r, int c, PAS key)
	{
		SetKwJObjTYPE(I);
	}
	bool SetJsonS(JObj& jso, int r, int c, PAS key)
	{
		SetKwJObjTYPE(S);
	}
	/*
	CStringW* PCell(int r, int c);
	double* PCellD(int r, int c);
	CTime* PCellT(int r, int c);
	float* PCellF(int r, int c);
	int* PCellI(int r, int c);
	*/


	bool NoData() { return this->RowSize() == 0; }//m_rcQuery == SQL_NO_DATA_FOUND || 

	static int ExtractTable(CStringW sql, CStringW& table);
	//CKRbVal
	CRBMap<CString, int> _mapField; // field name vs colnumber

	/*template<typename Func>
	void Cell(CStringW& s, int r, PWS pc, int point = 0, Func lmda = [](CStringW&, CDBVariant*) -> int { return 0; })
	{
		CStringW sc(pc);
		int c = GetColNum(pc);
		Cell(s, r, c, point, lmda);
	}*/
	template<typename Func>
	void Cell(CStringW& s, int r, CString& sc, int point = 0, Func lmda = [](CStringW&, CDBVariant*) -> int { return 0; })
	{
		int c = GetColNum(sc);
		Cell(s, r, c, point, lmda);
	}
	template<typename Func>
	void Cell(CStringW& s, int r, int c, int point = 0, Func lmda = [](CStringW&, CDBVariant*) -> int {return 0;})
	{
		CDBVariant* v = GetCell(r, c);
		if (v == NULL) {
			lmda(s, v);
			return;
		}

		switch (v->m_dwType)
		{
		case DBVT_SINGLE:
		case DBVT_DOUBLE:
			if (point == 0)
				point = _point; //미리 셋팅한 값
			break;
		}
		CStringW fmt;
		if (point > 0)
			fmt.Format(L"%%.%df", point); // %.6f

		if (lmda(s, v) == 1)  // 1이면 예외적으로 처리 했으니 무시.
			return;

		switch (v->m_dwType)
		{
		case DBVT_NULL: 
			break;
		case DBVT_STRING:
		case DBVT_WSTRING: {
			s = *v->m_pstringW;
			break;
		}
		case DBVT_BOOL:
			s.Format(L"%s", v->m_boolVal ? L"True" : L"False"); break;
		case DBVT_UCHAR:
			s.Format(L"%c", v->m_chVal); break;
		case DBVT_SHORT:
			s.Format(L"%d", v->m_iVal); break;
		case DBVT_LONG:
			s.Format(L"%ld", v->m_lVal); break;
		case DBVT_SINGLE:
			s.Format(fmt, v->m_fltVal); break;
		case DBVT_DOUBLE:
			s.Format(fmt, v->m_dblVal); break;
		case DBVT_DATE:
			s.Format(L"%04d-%02d-%02d %02d:%02d:%02d",
				v->m_pdate->year,
				v->m_pdate->month,
				v->m_pdate->day,
				v->m_pdate->hour,
				v->m_pdate->minute,
				v->m_pdate->second); break;
		case DBVT_ASTRING:
			s = CStringW(*v->m_pstringA); break;
		case DBVT_BINARY://바이너리
			{
				//ASSERT(0); v->m_pbinary;
				if (lmda(s, v) == 0) { // 0이면 default 문자열 방식으로 처리.
					LPVOID pv = GlobalLock(v->m_pbinary->m_hData);
					KAtEnd d_pv([&] { GlobalUnlock(v->m_pbinary->m_hData); });
					UINT64 len = v->m_pbinary->m_dwDataLength;
					char* rbuf = new char[2 + (len * 2) + 1]; // 0x 65 \0
					KAtEnd d_rbuf([&] { delete rbuf; });
					KwBinToHexStr((LPSTR)pv, (int)len, rbuf);
					//s = L"0x";// 19af8s9b876e0c 형식으로 리턴 한다. '0x' 안 붙이기로
					s = CString(rbuf);
				}
				break;//바이너리는 바로 리턴
			}
		default:
			s = L"(error)";
			ASSERT(0); break;
		}
		return;// CStringW(L"(error)");
	}


	/// <summary>
	/// 여러 타입을 받을수 있는, CJsonPbj에 받아 온다.
	/// </summary>
	/// <typeparam name="Func">lambda 함수</typeparam>
	/// <param name="r"></param>
	/// <param name="c"></param>
	/// <param name="robj">여러 타입을 받을수 있는</param>
	/// <param name="lmda">예외 처리시에</param>
	template<typename Func, typename TJSON>
	void CellToJson(int r, int c, TJSON& robj, Func lmda = [](CJsonPbj&, CDBVariant*) -> int {return 0; })
	{
		int eType = this->_finfos[c]->m_nSQLType;
		CDBVariant* v = this->GetCell(r, c);

		if (lmda(robj, v) == 1)  // 1이면 예외적으로 처리 했으니 무시.
			return;

		PWS field = (PWS)this->_finfos[c]->m_strName;
		if (v->m_dwType == DBVT_NULL)//DB에 값이 NULL인 경우 JSON에서 "ID":null, 처럼 null을 지원한다.
		{
			robj(field) = (PWS)NULL;//type = JSONType_Null JSONValue::JSONValue(NULL)
			return;
		}
		switch (eType)
		{
		case SQL_GUID:
		case SQL_LONGVARCHAR:
		case SQL_VARCHAR:
		case SQL_CHAR:
		case -8://CHAR1
		case -9://VARCHAR1
		case -10://TEXT1
			robj(field) = this->Cell(r, c);
			break;
		case SQL_DECIMAL://이거는 문자열 상태 숫자
			robj(field) = this->Cell(r, c);
			break;
		case SQL_LONGVARBINARY://이거는 "0x1234566" 문자열 상태 숫자
		case SQL_VARBINARY:
		case SQL_BINARY:
			robj(field) = this->Cell(r, c);
			break;
			//case SQL_INTERVAL: == SQL_TIME
		case SQL_TIME:// "2020-10-26 02:16:00" 문자열로
		case SQL_TIMESTAMP:// "2020-10-26 02:16:00" 문자열로
		case SQL_DATETIME:// "2020-10-26 02:16:00" 문자열로
			robj(field) = this->Cell(r, c);
			break;
		case SQL_BIGINT:
			TRACE("SQL_BIGINT Type(%d)\n", eType);//ASSERT(0);
		case SQL_INTEGER:
		case SQL_SMALLINT:
		case SQL_TINYINT:
		case SQL_BIT:
			robj(field) = this->CellI(r, c);
			break;
		case SQL_FLOAT:
		case SQL_REAL:
		case SQL_DOUBLE:
			robj(field) = this->CellD(r, c);
			break;
		case SQL_NUMERIC:
			robj(field) = this->CellD(r, c);
			break;
		default:
			robj(field) = this->Cell(r, c);
			TRACE("모르는 SQL Type(%d)\n", eType);//ASSERT(0);
			break;
		}
		//throw "Cell Unknow Type";
	}

	// row가 여러개인것을 전제 하고 array 를 hsr(key) = 에 넣는다.
	void MakeRecsetToJson(JObj& hsr, PS keyRecset);

	// row 0번 하나만 object 로서 hsr에 넣는다.
	void MakeRecsetOneRowToJson(JObj& hsr, PS keyRecset);

	template<typename Func>
	void MakeRecsetToJson(JObj& hsr, PS keyRecset = L"table", Func lmda = [](KRecordset&, int, int, JObj&) -> int {})
	{
		KRecordset& rs = *this;
		try
		{
			JSONArray ar;
			for(int r = 0; r < rs.RowSize(); r++)
			{
				JObj robj;
				for(int c = 0; c < rs.ColSize(); c++)
				{
					//CString colName = rs._finfos[c]->m_strName;
					//int eType = rs._finfos[c]->m_nSQLType
					//CString cell = rs.Cell(r, c);
					if(lmda(r, c, robj) == 0) // cell마저도 넣어 줘야 한다. 0이면 암것도 안함
					{
						CellToJson(rs, r, c, robj);// type에 따라 quato 여부
					}
				}
				ar.push_back(new JSONValue(*robj.get()));
			}
			hsr(keyRecset) = ar;
		}CATCH_GE;
	}

};

/*
// try블럭 때문에 커밋 해버린다. catch에서 Rollback할때 transaction pernding 상태가 아니다.
//  rmsid Begin commit 해야 겠다.
class KAutoTransaction
{
public:
	KDatabase& _db;
	KAutoTransaction(KDatabase& db)
		: _db(db)
	{
		_db.BeginTrans();
	}
	BOOL _bOK {TRUE};
	void Fail()
	{
		_bOK = FALSE;
	}
	~KAutoTransaction()
	{
		if(_bOK)
			_db.CommitTrans();
		else
			_db.Rollback();
	}
};*/


inline void KDatabase::ExecuteSQL(Quat& qs, int iOp)
{
	ExecuteSQL(qs.GetSQL(), iOp);
}


inline BOOL KRecordset::OpenSelectFetch(Quat& qs)
{
	return OpenSelectFetch(qs.GetSQL());
}



inline BOOL KRecordset::OpenSelect(Tss& ss)
{
	CString sql(Psr(ss));
	return OpenSelect(sql);
}
inline BOOL KRecordset::OpenSelect(Quat& qs)
{
	return OpenSelect(qs.GetSQL());
}

CString KwQuat(PWS content);


#define Rec(rs) KRecordset rs(&_db)
