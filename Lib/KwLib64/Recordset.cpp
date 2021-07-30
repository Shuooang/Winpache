#include "pch.h"
#include "Recordset.h"

#include "inlinefnc.h"
#include "KTemple.h"
#include "Kw_tool.h"
#include "DlgTool.h"
#include "KBinary.h"
#include "tchtool.h"
#include "KDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL KDatabase::Check(RETCODE nRetCode) const
{
	return CheckHstmt(nRetCode, SQL_NULL_HSTMT);
}

BOOL KDatabase::CheckHstmt(RETCODE nRetCode, HSTMT hstmt) const
{
	ASSERT_VALID(this);
	UNUSED(hstmt);

	switch (nRetCode)
	{
	case SQL_SUCCESS_WITH_INFO:
#ifdef _DEBUGx
	{
		CDBException e(nRetCode);
		TRACE(traceDatabase, 0, _T("Warning: ODBC Success With Info, "));
		e.BuildErrorString((CDatabase*)this, hstmt);
	}
#endif // _DEBUG

	// Fall through

	case SQL_SUCCESS:
	case SQL_NO_DATA_FOUND:
		return TRUE;
	}

	return FALSE;
}

BOOL KDatabase::OpenEx(LPCTSTR lpszConnectString, DWORD dwOptions)
{
	AUTOLOCK(_mxDbAcess);
	BOOL rv = CDatabase::OpenEx(lpszConnectString, dwOptions);
	_tLastAcess = GetTickCount64();

	return rv;
}
void KDatabase::Reopen()
{
	AUTOLOCK(_mxDbAcess);
	if(IsOpen())
		Close();//?중요: Close하면 딸린 CRecordset은 모두 리셋 되므로 m_pDatabase = NULL이 된다.

	//L"ODBC;DSN=odisoDB;DRIVER={[MariaDB ODBC 3.1 Driver]};UID=odisou;PWD=odisopwd;DATABASE=odiso;PORT=3306;SERVER=192.168.35.175;TCPIP=1;"	ATL::CStringT<wchar_t,StrTraitMFC_DLL<wchar_t,ATL::ChTraitsCRT<wchar_t>>>
	CString sDSN = GetConnect();
	if (sDSN.GetLength() == 0)
		throw "DSN is null.";// 아직 한번도 연결 시도 하지 않은 개발 오류 : 처리되지 않는 오류.
	CString sDSN1;
	if(sDSN.GetLength() > 5 && sDSN.Left(5) == L"ODBC;")
		sDSN1 = sDSN.Mid(5);
	else
		sDSN1 = sDSN;
	OpenEx(sDSN1);
	//CRecordset::Open DB Error:데이터 원본 이름이 없고 기본 드라이버를 지정하지 않았습니다.
}

bool KDatabase::IsReopenable()
{
	AUTOLOCK(_mxDbAcess);
	if (IsOpen())
		return true;
	//L"ODBC;DSN=odisoDB;DRIVER={[MariaDB ODBC 3.1 Driver]};UID=odisou;PWD=odisopwd;DATABASE=odiso;PORT=3306;SERVER=192.168.35.175;TCPIP=1;"	ATL::CStringT<wchar_t,StrTraitMFC_DLL<wchar_t,ATL::ChTraitsCRT<wchar_t>>>
	CString sDSN = GetConnect();
	if (sDSN.GetLength() == 0)
		return false;
	if(sDSN.Left(5) == L"ODBC;")
	{
		CString sDSN1 = sDSN.Mid(5);
		if (sDSN1.Find(L"DSN=", 0) >= 0)
			return true;
	}
	return false;
}

// 쓰는법: KRecset을 하나도 할당하지 않은 상태에서 불러야 한다. 예: CheckDB()
string KDatabase::TimeCheckReopen()
{
	auto tick = GetTickCount64();
	auto intv = tick - _tLastAcess;
	if(intv > _secReopenInterval)
	{
		Reopen();//reopen 하면 KRecordset의 CDatabase가 리셋 된다.
		return "Reopen";
	}
	return "";
}




// called from _GetType()
CStringA KDatabase::GetTypeToStr(int typeSql)
{
	CStringA psErr;
	switch(typeSql) //,SQL_COLUMN_TYPE
	{
		CASE_STR0(SQL_DATE);
		CASE_STR0(SQL_TIME);
		CASE_STR0(SQL_TIMESTAMP);
		CASE_STR0(SQL_VARCHAR);
		CASE_STR0(SQL_CHAR);
		CASE_STR0(SQL_NUMERIC);
		CASE_STR0(SQL_DECIMAL);
		CASE_STR0(SQL_INTEGER);
		CASE_STR0(SQL_SMALLINT);
		CASE_STR0(SQL_TINYINT);
		CASE_STR0(SQL_BIGINT);
		CASE_STR0(SQL_FLOAT);
		CASE_STR0(SQL_REAL);
		CASE_STR0(SQL_DOUBLE);
		CASE_STR0(SQL_LONGVARCHAR);
		CASE_STR0(SQL_BINARY);
		CASE_STR0(SQL_VARBINARY);
		CASE_STR0(SQL_LONGVARBINARY);
		CASE_STR0(SQL_GUID);//-11
		case -9: // VARCHAR 이건 sqlext.h 에 없지만 이상하게 mdb TEXT 가 이거로 걸리네
			psErr = "MySQL_VARCHAR"; break;
		case -8: // CHAR 이건 mysql 에서 enum type인데 string인것 같은데...
			psErr = "MySQL_CHAR"; break;
		case -10: // TEXT 이건 access 에서 ctype == SQL_C_CHAR == 1인데 SQL_WLONGVARCHAR
			psErr = "MySQL_TEXT"; break;

		default:
			psErr.Format("SQL_UNKNOWN_%d", typeSql);
			ASSERT(0); // -8(enum)
	}
	return psErr;
}


// text를 ?로 바이너리로 보낼때는 UTF8로 바꿔서 저장해야 한글이 보인다. KwWcharToUTF8
void KDatabase::ExecuteSQL(LPCWSTR sql, KDataPacket dts[], int numVar, int iOp)
{
	PTR* pData = new PTR[numVar];
	SQLLEN* cbRv = new SQLLEN[numVar];
	SQLSMALLINT* cType = new SQLSMALLINT[numVar];
	SQLSMALLINT* sqlType = new SQLSMALLINT[numVar];
	KAtEnd d_delete([&]() {
		DeleteMeSafe(pData);
		DeleteMeSafe(cbRv);
		DeleteMeSafe(cType);
		DeleteMeSafe(sqlType);
    });
	for(int i=0;i<numVar;i++)
	{
		pData[i] = (PTR)dts[i]._data;
		cbRv[i] = (SQLLEN)dts[i]._len;
		cType[i] = (SQLSMALLINT)dts[i]._cType;
		sqlType[i] = (SQLSMALLINT)dts[i]._sqlType;
	}

	ExecuteSQL(sql, numVar, pData, cbRv, cType, sqlType, iOp);
}
/// <summary>
/// 전송할 바이너리 필드가 ? 가 하나 일떄
/// </summary>
void KDatabase::ExecuteSQL(LPCWSTR sql, PTR pData, SQLLEN cbRv, SQLSMALLINT cType, SQLSMALLINT sqlType, int iOp)
{
	KDataPacket dts((char*)pData, cbRv, cType, sqlType);
	ExecuteSQL(sql, &dts, 1, iOp);
}

/// <summary>
/// insert or update에만 써야 한다.
/// </summary>
/// <param name="lpszSQL"></param>
/// <param name="numVar"></param>
/// <param name="pData"></param>
/// <param name="cbRv"></param>
/// <param name="cType"></param>
/// <param name="sqlType"></param>
void KDatabase::ExecuteSQL(LPCWSTR sql, int numVar, // '?'의 갯수 :  BIND해야할 PARAM 갯수
							PTR* pData,			// Data Point ARRAY 
							SQLLEN* cbRv,			// 실제 전달된 length ARRAY
							SQLSMALLINT* cType,		// array of c type
							SQLSMALLINT* sqlType, int iOp)
{
	AUTOLOCK(_mxDbAcess);
	ULONGLONG tick = GetTickCount64();
	//auto intv = tick - this->_tLastAcess;
	//if(intv > this->_secReopenInterval)
	//	Reopen();//ReopenDb();

	KAtEnd d_sql([&, sql, tick, iOp]()->void {
		auto elapsed = GetTickCount64() - tick;
		if (_fncDbLog && !KwAttr(iOp, eNoLog))/// KDatabase::eNoLog를 주면 로그를 안쌓는다.
		{
			auto rv = (*_fncDbLog)(sql, (DWORD)elapsed);// rv < 0 이면 실패
		}
	});

	if(pData == NULL)
	{
		__super::ExecuteSQL(sql);
		return;
	}
	//ASSERT(tchstri(lpszSQL, L"select") == NULL); 내용에 들어있을수 있네.
	RETCODE nRetCode;
	HSTMT hstmt;

	ENSURE_VALID(this);
	ENSURE_ARG(AfxIsValidString(sql));
	int nTry = 1;
	while(1)
	{
		AFX_SQL_SYNC(::SQLAllocStmt(m_hdbc, &hstmt));
		if(!CheckHstmt(nRetCode, hstmt))
			AfxThrowDBException(nRetCode, this, hstmt);

		TRY
		{
			OnSetOptions(hstmt);

		// Give derived CDatabase classes option to use parameters
		//BindParameters(hstmt); override하라고 비어 있고, 그걸 아래에서 한다.

			for(int i = 0; i < numVar; i++)
			{
				SQLLEN len = cbRv[i];
				SQLLEN lLength = len;

				lLength = SQL_LEN_DATA_AT_EXEC(lLength);
				nRetCode = SQLBindParameter(hstmt, i + 1, SQL_PARAM_INPUT, cType[i], sqlType[i],
												len, //ColumnSize 
												0, //DecimalDigits 
												(PTR)pData[i], //ParameterValuePtr 
												0,	//BufferLength 
												&lLength);//StrLen_or_IndPtr 
				if(!CheckHstmt(nRetCode, hstmt))
					AfxThrowDBException(nRetCode, this, hstmt);
			}

			LPTSTR pszSQL = const_cast<LPTSTR>(sql);
			AFX_ODBC_CALL(::SQLExecDirect(hstmt, reinterpret_cast<SQLTCHAR*>(pszSQL), SQL_NTS));
			if(!CheckHstmt(nRetCode, hstmt) && nRetCode != SQL_NEED_DATA)
				AfxThrowDBException(nRetCode, this, hstmt);

			CHAR* pTarget = NULL;
			if(nRetCode == SQL_NEED_DATA)
			{
				for(int i = 0; i < numVar; i++)
				{
					AFX_ODBC_CALL(::SQLParamData(hstmt, (SQLPOINTER*)&pTarget)); // get the pointer that will receive the data.
					if(!CheckHstmt(nRetCode, hstmt) && nRetCode != SQL_NEED_DATA)
						AfxThrowDBException(nRetCode, this, hstmt);

					SQLLEN lenSent = 0, dw = 0;
					while(lenSent != cbRv[i])
					{
						SQLLEN dwSend = cbRv[i] - lenSent;
						if(dwSend > 0x8000) dwSend = 0x8000; // each time maximum block size

						AFX_ODBC_CALL(::SQLPutData(hstmt, (PTR)pTarget, (SQLLEN)dwSend));
						if(!CheckHstmt(nRetCode, hstmt) && nRetCode != SQL_NEED_DATA)
							AfxThrowDBException(nRetCode, this, hstmt);

						pTarget += dwSend;
						lenSent += dwSend;
					}
				}
				// Make final SQLParamData call. 이거 안해주면 안들어 간다.
				AFX_ODBC_CALL(::SQLParamData(hstmt, (SQLPOINTER*)&pTarget));
				if(!CheckHstmt(nRetCode, hstmt))
				{
					// 				(-1) : Unknown command
					// 				(State : 08S01, Native:1047
					///	AfxThrowDBException(nRetCode, this, hstmt);
					/// 여기서 throw db 안하고 재시도 해보자.
					CDBException* pException = new CDBException(nRetCode);
					KAtEnd d_sql([&]()->void {	if(pException)
						pException->Delete();	});
					if(nRetCode == SQL_ERROR)// && pdb != NULL)
					{
						pException->BuildErrorString(this, hstmt);
						if(pException->m_strError.Find(L"Got packets out of order") >= 0 && 
							pException->m_strStateNativeOrigin.Find(L"State:08S01") >= 0)
						{
							::SQLCancel(hstmt);
							AFX_SQL_SYNC(::SQLFreeStmt(hstmt, SQL_DROP));
							nTry++;
							continue;
						}
					}
				}
			}
		}
		CATCH_ALL(e)
		{
			::SQLCancel(hstmt);
			AFX_SQL_SYNC(::SQLFreeStmt(hstmt, SQL_DROP));
			THROW_LAST();
		}
		END_CATCH_ALL

		AFX_SQL_SYNC(::SQLFreeStmt(hstmt, SQL_DROP));
		break;
	}//while(1)
	//_dbLog->ExecuteSQL(
}






// Screen for errors.
BOOL KRecordset::Check(RETCODE nRetCode) const
{
	ASSERT_VALID(this);

	switch (nRetCode)
	{
	case SQL_SUCCESS_WITH_INFO:
#ifdef _DEBUGx
	{
		CDBException e(nRetCode);
		TRACE(traceDatabase, 0, _T("Warning: ODBC Success With Info, "));
		e.BuildErrorString(m_pDatabase, m_hstmt);
	}
#endif

	// Fall through

	case SQL_SUCCESS:
	case SQL_NO_DATA_FOUND:
	case SQL_NEED_DATA:
		return TRUE;
	}

	return FALSE;
}




/// <summary>
/// 여기서 reopen 하면 이미 database가 할당된 krecset은 다시 연결 해야 한다. 다 null로 날라 간다.
/// </summary>
void KRecordset::ReopenDb()
{
	ASSERT(0);
	auto pdb = (KDatabase*)m_pDatabase;
	AUTOLOCK(pdb->_mxDbAcess);
	pdb->Reopen();//reopen 하면 KRecordset의 CDatabase가 리셋 된다.
	this->m_pDatabase = pdb;//를 안해 주면 아래 Open에서 연결 ODBC창이 떠버린다.
}

BOOL KRecordset::OpenSelect(PWS sql)
{
	/// select 는 다중으로 해도 별문제가 없는거 같은데
	/// 그래도 언제 Reopen 다중 스레드에서 Close 될지 모르니 
	auto pdb = (KDatabase*)m_pDatabase;
	AUTOLOCK(pdb->_mxDbAcess);

	if(m_hstmt == SQL_NULL_HSTMT)
	{
		/// DB가 연결 안되어 있어도 널이니, 그냥 지나 치면 CDBExceptions이 발생 하는데, 
		/// 굳이 여기서 throw 하는게 좋나?
		CStringA sqla(sql);
		CStringA sqll;
		sqll.Format("Recordset is not ready(SQL_NULL_HSTMT). %s...", sqla.Left(20));
		/// 여기서 계속 진행 하면 아래 Open에서 [데이터원본선택]창이 떠버린다.
		throw_str((PAS)sqll);// ion goon away.");
		if(m_pDatabase == NULL)
		{
			//ReopenDb();//KTrace(L"m_pDatabase == NULL try ReopenDb()\n");
		}
		//오랫만에 reopen할때 함수에 Rec(rec1); Rec(rec2); 이렇게 여러개 이면 rec1이 Reopen하면 rec2는 null이 되버린다.
//		ASSERT(m_pDatabase != NULL);//KRecordset rs(&_db)하기 때문에 NULL은 아니지.
		//ASSERT(pdb->_tLastAcess == 0);//아직 연결 안했으면 0초기값이겠지.
		// 아직 연결이 안된 경우
		//KwMessageBoxError(L"아직 DB연결이 안되었다.");
	}

	auto tick = GetTickCount64();
	//auto intv = tick - pdb->_tLastAcess;
	//if(intv > pdb->_secReopenInterval)
	//	ReopenDb();//여기서 reopen하면 rec2는 널 되버린다.

	KAtEnd d_sql([&, sql, tick]()->void {
		auto elapsed = GetTickCount64() - tick;
		if (pdb->_fncDbLog.get())
		{
			auto rv = (*pdb->_fncDbLog)(sql, (DWORD)elapsed);
			// rv < 0 이면 실패
		}

	});

	KTrace(L"%s\n", sql);


	BOOL bOpen = FALSE;
	int nReTry = 3;
	while(nReTry--)
	{
		try
		{
			bOpen = Open(CRecordset::snapshot, sql);
			pdb->_tLastAcess = GetTickCount64();//새로 GetTickCount() 해야. Reopen할떄도 하거든

			break;//while에서 탈출
		}
		catch(CDBException* e)
		{
			KTrace(L"CRecordset::Open DB Error:%s", e->m_strError);
			KTrace(e->m_strStateNativeOrigin);
#ifdef _DEBUG1
			CDumpContext dc;
			this->Dump(dc);//CRecordset::Open Exception:잘못된 인수가 발견되었습니다.
			e->ReportError();//Dialog뜬다.
#endif
			if(KDatabase::IsGoneAway(e))
			{
				//IsGoneAway 일때만 다시 연결 하는데, 다른 경우도 할까?
				//ReopenDb();//이미 할당된 KRecset의 m_pDatabase 를 NULL 만들어 버리므로 Recset에서 reopen하면 안되겠다.
				throw_str("Database connection goon away.");
				//SetLoginTimeout SetQueryTimeout과는 관계가 없구만.
				//continue;//CDBException의 state=08S01인경우만 
				//ASSERT(nReTry > 1);//gone away 후 또 접속 실패 한 경우
			}
#ifdef _DEBUG2
			e->ReportError();//Dialog뜬다.
#endif
			throw e;
		}
		catch(CException* e)
		{	auto buf = new TCHAR(1024); 
			KAtEnd d_buf([&]() { delete buf; }); 
			e->GetErrorMessage(buf, 1000); 
			TRACE(L"CException:%s - %s %d\n", buf, __FUNCTION__, __LINE__); 
			throw new KException("CException", GetLastError(), 0, buf, NULL, __FUNCTION__, __LINE__); 
		}
		catch(std::exception& e)
		{	TRACE("catch std::exception %s  - %s %d\n", e.what(), __FUNCTION__, __LINE__); 
			throw new KException("std::exception", -1, 0, CStringW(e.what()), NULL, __FUNCTION__, __LINE__); 
		}
		catch(PWS e)
		{	TRACE("catch LPCWSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__); 
			throw new KException("LPCWSTR", GetLastError(), 0, e, NULL, __FUNCTION__, __LINE__); 
		}
		catch(PAS e)
		{	TRACE("catch LPCSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__); 
			throw new KException("LPCSTR", GetLastError(), 0, CStringW(e), NULL, __FUNCTION__, __LINE__); 
		}
		catch(...)
		{	TRACE("catch ...  - %s %d\n", __FUNCTION__, __LINE__); \
			throw new KException("Unknown", GetLastError(), 0, L"Unknown catch(...) Error.", NULL, __FUNCTION__, __LINE__); \
		}
	}
	return bOpen;
}
BOOL KRecordset::OpenSelectFetch(PWS sql)
{
	if(m_pDatabase == NULL)
		throw_str("Database is not connected! (OpenSelectFetch)");
	auto pdb = (KDatabase*)m_pDatabase;
	AUTOLOCK(pdb->_mxDbAcess);
	BOOL b = OpenSelect(sql);
	if(b)
		Fetch();
	return b;
}

CString KRecordset::GetFieldName(int col)
{
	ASSERT(_finfos.size() > col);
	auto field = _finfos[col];
	return field->m_strName;
}


int KRecordset::GetFieldNames(CStringArray& ar)
{
	for(auto field:_finfos)
		ar.Add(field->m_strName);
	return (int)ar.GetCount();
}


int KRecordset::DoFetch()
{
   // ASSERT(IsOpen());
	auto pdb = (KDatabase*)m_pDatabase;
	AUTOLOCK(pdb->_mxDbAcess);
	int iRow = 1;
	BOOL bIsEOF = this->IsEOF();
	DWORD dwSize = this->GetRowsetSize();
	if(!bIsEOF)
	{
		int iFieldCnt = this->GetODBCFieldCount();
		//CODBCFieldInfo fieldinfo;
		CString srow;// hd.Format(L"strName\tSQLType\tPrecision\tScale\tNullability\r\n");
//		TRACE("DBVT_Type\tSQLType\tPrecision\tScale\tNullability\tstrName\r\n");

		//vector<shared_ptr<CODBCFieldInfo>> finfos;
		for(int iCol = 0; iCol < iFieldCnt; iCol++)
		{//GetFieldValue 전에 field info 미리 알아 둘려고
			auto finfo = new CODBCFieldInfo;
			this->GetODBCFieldInfo(iCol, *finfo);
			_finfos.push_back(shared_ptr<CODBCFieldInfo>(finfo));
			_mapField.SetAt(finfo->m_strName, iCol);
		}

		//vector<ShRow> _tbl;
		for(this->MoveFirst(); !this->IsEOF(); this->MoveNext())//역순은 IsBOF
		{
			auto rowV = new VtRow;
			for(int iCol = 0; iCol < iFieldCnt; iCol++)
			{
				CString sItem;
				//CDBVariant var;
				auto vcol = new CDBVariant;

				this->SetAbsolutePosition(iRow);// 1,2,3,4,


				auto fi = _finfos[iCol].get();// &fieldinfo;
				if(fi->m_nSQLType == SQL_LONGVARBINARY)
					_break;
				this->GetFieldValue(iCol, *vcol);//sItem);

				int eCutomFetched = 0;
				if(_fncFetch)
					eCutomFetched = (*_fncFetch)(iRow -1, iCol, *vcol);

				if(iRow == 1)
				{
					//this->GetODBCFieldInfo(iCol, fieldinfo);
					switch(vcol->m_dwType)
					{
					case DBVT_NULL:	//액세스에 사용할 수 있는 union 멤버가 없습니다.
						//ASSERT(0); //type에 상관 없이, NULL인경우
						break;
					case DBVT_BOOL:		vcol->m_boolVal; break;
					case DBVT_UCHAR:	vcol->m_chVal; break;
					case DBVT_SHORT:	vcol->m_iVal; break;
					case DBVT_LONG:		vcol->m_lVal; break;
					case DBVT_SINGLE:	vcol->m_fltVal; break;
					case DBVT_DOUBLE:	vcol->m_dblVal; break;
					case DBVT_DATE:		vcol->m_pdate; break;
					case DBVT_STRING:	vcol->m_pstring; break;
					case DBVT_BINARY:	vcol->m_pbinary; break;
					case DBVT_ASTRING:	vcol->m_pstringA; break;
					case DBVT_WSTRING:	vcol->m_pstringW; break;
					default:
						ASSERT(0); break;
					}
#ifdef _DEBUGx
					srow.Format(L"%d\t%d\t%d\t%d\t%d\t%s\r\n", vcol->m_dwType, fi->m_nSQLType, fi->m_nPrecision, fi->m_nScale, fi->m_nNullability, fi->m_strName);
					TRACE(srow);
#endif
					//+	m_strName		L"fuuid"	CString
					//	m_nSQLType		-9			short
					//	m_nPrecision	50			unsigned __int64
					//	m_nScale		0			short
					//	m_nNullability	0			short

					//	DBVT_Type			SQLType	Precision	Scale	Nullability	strName
					//	DBVT_LONG		4	SQL_INTEGER			4	10			0	1	INT1
					//	DBVT_SHORT		3	SQL_SMALLINT		5	5			0	1	SMALLINT1
					//	DBVT_SINGLE		5	SQL_REAL			7	7			0	1	FLOAT1
					//	DBVT_DOUBLE		6	SQL_DOUBLE			8	15			0	1	DOUBLE1
					//	DBVT_WSTRING	11	SQL_DECIMAL			3	10			0	1	DECIMAL1
					//					11						-8	50			0	1	CHAR1
					//					11						-9	50			0	1	VARCHAR1
					//					11						-10	65535		0	1	TEXT1
					//					11						-10	1073741823	0	1	JSON1
					//	DBVT_BINARY		9	SQL_LONGVARBINARY	-4	65535		0	1	BLOB1
					//	DBVT_DATE		7	SQL_DATETIME		9	10			0	1	DATE1
					//					7						10	8			0	1	TIME1
					//					7						11	19			0	1	DATETIME1
					//					7						11	19			0	1	TIMESTAMP
				}

				if(vcol->m_dwType == DBVT_BINARY)//SQL_LONGVARBINARY)
				{
					//LPVOID pv = GlobalLock(vcol->m_pbinary->m_hData);
					//KAtEnd d_pv([&] { GlobalUnlock(vcol->m_pbinary->m_hData); });
					//UINT64 len = vcol->m_pbinary->m_dwDataLength;
					//char* rbuf = new char[2 + (len*2) + 1]; // 0x 65 \0
					//KAtEnd d_rbuf([&] { delete rbuf; });
					//KwBinToHexStr((LPSTR)pv, len, rbuf);
					//sData = rbuf;
				}
				if(eCutomFetched == 1)// 이면 뭔일 한거다. 그러니 ~~
				{
					//TODO: 저장 안해야 한다. _tbl
					// 이미 이 cell을 사용 하였으므로 굳이 용량이 너무 큰경우 *vcol을 그냥 free한다.
					delete vcol;
					vcol = NULL;//이게 아주 중요
				}

				rowV->push_back(ShDBVar(vcol));//따라서 shared_ptr이 비어 있을수 있다.
			}
			_tbl.push_back(ShRow(rowV));

			iRow++;
		}
	}



    return 0;
}


CDBVariant* KRecordset::GetCell(int r, int c)
{
	ASSERT(r < _tbl.size());
	auto shRow = _tbl[r];
	auto row = shRow.get();
	ASSERT(c < row->size());
	auto v = (*row)[c];
	return v.get();
}

int KRecordset::GetColNum(CString& sc)
{
	int c = -1;
	VERIFY(_mapField.Lookup(sc, c));
	return c;
}
int KRecordset::GetType(int r, int c)
{
	auto v = GetCell(r, c);
	if(v == NULL)
		return DBVT_NULL;
	return v->m_dwType;
}

BOOL KRecordset::IsNull(int r, int c)
{
	auto v = GetCell(r, c);
	if(v == NULL)
		return TRUE;
	return v->m_dwType == DBVT_NULL;
}
/// <summary>
/// 
/// </summary>
/// <param name="r"></param>
/// <param name="c"></param>
/// <param name="point">소수점 아래 숫자</param>
/// <returns></returns>
CStringW KRecordset::Cell(int r, int c, int point, int iOp)
{
	CStringW s;
	Cell(s, r, c, point, iOp);
	return s;
}

void KRecordset::Cell(CStringW& s, int r, int c, int point, int iOp)
{
	Cell(s, r, c, point, [&](CStringW& s, CDBVariant* v) -> int {
		return 0;
		});
}




CStringA KRecordset::CellSA(int r, int c, int point, int iOp)
{
	return CStringA(Cell(r,c,point,iOp));
}


int KRecordset::CellI(int r, int c)
{
	CDBVariant* v = GetCell(r, c);
	if(v == NULL)
		return 0;

	switch(v->m_dwType)
	{
	case DBVT_BOOL:
		return (int)v->m_boolVal;
	case DBVT_UCHAR:
		return (int)v->m_chVal;
	case DBVT_SHORT:
		return (int)v->m_iVal;
	case DBVT_LONG:
		return (int)v->m_lVal;
	case DBVT_SINGLE:
		return (int)v->m_fltVal;
	case DBVT_DOUBLE:
		return (int)v->m_dblVal;
	case DBVT_WSTRING:
	{
		PWS pws = (PWS)(*v->m_pstringW);
		return KwAtoi(pws);
	}

	case DBVT_DATE:
	case DBVT_BINARY:
	case DBVT_STRING:
	case DBVT_ASTRING:
	default://DBVT_NULL
		TRACE("모르는 DBVT Type(%d)\n", v->m_dwType);//ASSERT(0);
		ASSERT(0); break;
	}
	return -1;
}
float KRecordset::CellF(int r, int c)
{
	CDBVariant* v = GetCell(r, c);
	if(v == NULL)
		return 0;

	switch(v->m_dwType)
	{
	case DBVT_BOOL:
		return (float)v->m_boolVal;
	case DBVT_UCHAR:
		return (float)v->m_chVal;
	case DBVT_SHORT:
		return (float)v->m_iVal;
	case DBVT_LONG:
		return (float)v->m_lVal;
	case DBVT_SINGLE:
		return (float)v->m_fltVal;
	case DBVT_DOUBLE:
		return (float)v->m_dblVal;
	case DBVT_WSTRING:
	{
		PWS pws = (PWS)(*v->m_pstringW);
		return (float)KwAtof(pws);
	}

	case DBVT_DATE:
	case DBVT_BINARY:
	case DBVT_STRING:
	case DBVT_ASTRING:
	default://DBVT_NULL
		TRACE("모르는 DBVT Type(%d)\n", v->m_dwType);//ASSERT(0);
		ASSERT(0); break;
	}
	return -1;
}

/*
CStringW* KRecordset::PCell(int r, int c)
{
}
int* KRecordset::PCellI(int r, int c)
{
}
double* KRecordset::PCellD(int r, int c)
{
	CDBVariant* v = GetCell(r, c);
	if(v->m_dwType != DBVT_NULL)
		return &CellD(r, c);
	return nullptr;
}
float* KRecordset::PCellF(int r, int c)
{
}
CTime* KRecordset::PCellT(int r, int c)
{
}
*/



double KRecordset::CellD(int r, int c)
{
	CDBVariant* v = GetCell(r, c);
	if(v == NULL)
		return 0.;

	switch(v->m_dwType)
	{
	case DBVT_BOOL:
		return (double)v->m_boolVal;
	case DBVT_UCHAR:
		return (double)v->m_chVal;
	case DBVT_SHORT:
		return (double)v->m_iVal;
	case DBVT_LONG:
		return (double)v->m_lVal;
	case DBVT_SINGLE:
		return (double)v->m_fltVal;
	case DBVT_DOUBLE:
		return (double)v->m_dblVal;
	
	case DBVT_NULL:	//	return NaN; nulalble필드를 그냥 읽게 했다면 개발 실수
	case DBVT_DATE:
	case DBVT_BINARY:
	case DBVT_STRING:
	case DBVT_WSTRING:
	case DBVT_ASTRING:
	default:
		ASSERT(0); break;
	}
	return -1;
}

CKTime KRecordset::CellT(int r, int c)
{
	CDBVariant* v = GetCell(r, c);
	if(v == NULL)
		return CTime();//1970-01-01 09:00:00 왜 9시지?

	switch(v->m_dwType)
	{
	case DBVT_DATE:
		return CTime(v->m_pdate->year, v->m_pdate->month,
			v->m_pdate->day, v->m_pdate->hour, v->m_pdate->minute, v->m_pdate->second);
	case DBVT_BOOL:	case DBVT_UCHAR:	case DBVT_SHORT:	case DBVT_LONG:
	case DBVT_SINGLE:	case DBVT_DOUBLE:	case DBVT_BINARY:	case DBVT_STRING:
	case DBVT_WSTRING:	case DBVT_ASTRING:
	default:
		ASSERT(0); break;
	}
	return CTime();
}


HGLOBAL KRecordset::CellBinH(int r, int c, UINT64& len)
{
	CDBVariant* v = GetCell(r, c);
	if(v == NULL)
		return NULL;
	switch(v->m_dwType)
	{
	case DBVT_BINARY: {
		HGLOBAL h = v->m_pbinary->m_hData;
		len = (int)v->m_pbinary->m_dwDataLength;
		return h;
	}
	case DBVT_NULL:
		return NULL;
	case DBVT_STRING:
	case DBVT_WSTRING:
	case DBVT_BOOL:
	case DBVT_UCHAR:
	case DBVT_SHORT:
	case DBVT_LONG:
	case DBVT_SINGLE:
	case DBVT_DOUBLE:
	case DBVT_DATE:
	case DBVT_ASTRING:
	default:
		ASSERT(0); break;
	}
	return NULL;
}

BOOL KRecordset::CellBin(int r, int c, KBinary* pBin)
{
	HGLOBAL h = NULL;
	UINT64 len = 0;
	h = CellBinH(r, c, len);
	if(h == NULL)
		return FALSE;

	LPVOID pv = GlobalLock(h);
	KAtEnd d_pv([&] { GlobalUnlock(h); });

	pBin->SetPtr((LPCSTR)pv, (int)len);
	return TRUE;
}


int KRecordset::ExtractTable(CStringW sql0, CStringW& tblS)
{
	CString sql = sql0;
	sql.MakeLower();
	auto ifr = sql.Find(L"from");
	if (ifr > 0)
	{
		sql = sql0.Mid(ifr + 4);
		CStringArray ar, ar1;
		KwCutByToken(sql, L" \t\r\n", ar);
		if (ar.GetCount() > 0) {
			CString tblL = ar[0];
			KwCutByToken(tblL, L".", ar1);
			if (ar1.GetCount() < 2)
				tblS = tblL;
			else
				tblS = ar1[1]; // petme.`table`
			tblS.Trim(L"`");
			return 0;
		}
	}
	return 1;
}

// row가 여러개인것을 전제 하고 array 를 hsr(key) = 에 넣는다.

void KRecordset::MakeRecsetToJson(JObj& hsr, PWS keyRecset)
{
	KRecordset& rs = *this;
	//try		{
	//CStringArray arHd;
	//rs.GetFieldNames(arHd);
	JArr ar;
	for(int r = 0; r < rs.RowSize(); r++)
	{
		JObj robj;
		for(int c = 0; c < rs.ColSize(); c++)
		{
			rs.CellToJson(r, c, robj, [&](JObj&, CDBVariant*) -> int
				{
					return 0;
				});// type에 따라 quato 여부
				   //CString colName = rs._finfos[c]->m_strName;
				   //CString cell = rs.Cell(r, c);
				   //robj((PWS)colName) = cell;
		}
		//robj.get()->_bValueOwner = false;
		ar.Add(robj);// new JVal(robj));
	}
	hsr(keyRecset) = ar;
	//}CATCH_GE;
}

// row 0번 하나만 object 로서 hsr에 넣는다.

void KRecordset::MakeRecsetOneRowToJson(JObj& hsr, PWS keyRecset)
{
	KRecordset& rs = *this;
	//try	{
	if(rs.RowSize() == 0)
		return;
	JObj robj;
	for(int c = 0; c < rs.ColSize(); c++)
	{
		rs.CellToJson(0, c, robj, [&](JObj&, CDBVariant*) -> int { return 0; });
	}
	hsr(keyRecset) = robj;//ownership을 false 하고 hsr에 넣는다.
						  //}CATCH_GE;
}
// @valriable 을 '%s' 바꿔준다.
/*
int KRecordset::SqlParams(CStringW sql0, CStringW& tblS)
{
}
*/
CString KwQuat(PWS content)
{
	CString stack;
	int len = (int)tchlen(content);
	TCHAR* tmp = (TCHAR*)stack.GetBuffer(len * 2 + 1);
	int j = 0;
	WCHAR pc = L'\0';
	for(int i = 0; i < len; i++)
	{
		TCHAR c = content[i];
		// 'abc \' "	\r\n123가나다. ` ; ABC'
		//if(pc != '\\') // \n => \\n 으로 가지 않게 일단 무리수 인듯
//		if(c == '\'')
		if(c == '\'' || c == '\\')
		{
			tmp[j++] = '\\'; tmp[j++] = c; // ' > \'
		}
		else
			tmp[j++] = c;
		pc = c;
	}
	tmp[j] = '\0';
	stack.ReleaseBuffer();
	return stack;
}
#ifdef _QUAT

PWS Quat::Qs(PWS val, BOOL bQuat, BOOL bNullIfEmpty)
{
	CString& sbuf = _buf.GetBuf();
	if (val != nullptr)
	{
		if (bNullIfEmpty && tchlen(val) == 0)
			return L"NULL";
		else
		{
			if(bQuat)
				sbuf.Format(L"'%s'", val);
			else
				sbuf = val;
			return sbuf;
		}
/*
else if (v->IsBool())
			return v->AsBool() ? L"1" : L"0";
		else if (v->IsNull())
			return L"NULL";
		else if (v->IsObject())
			return L"[obj]";//이래야 SQL에러가 나도록 유도 하지.
		else if (v->IsArray())
			return L"[array]";
		else
		{
			ASSERT(0);
		}
*/	}

	if(bNullIfEmpty)
		return L"NULL";
	else
		return nullptr;
}

PWS Quat::Qs(int k, BOOL bNullIfMinusOne)
{
	CString& sbuf = _buf.GetBuf();
	CString fmt;
	//double d = k;// v->AsNumber();
	if (bNullIfMinusOne && k == -1)// tchlen(val) == 0)
		return L"NULL";

	fmt = L"%d";
	sbuf.Format(L"%d", k);
	return sbuf;
}

PWS Quat::Qs(double k, BOOL bNullIfMinusOne, int afterPoint)
{
	CString& sbuf = _buf.GetBuf();
	CString fmt;
	//double d = k;// v->AsNumber();
	if (bNullIfMinusOne && k == -1.)// tchlen(val) == 0)
		return L"NULL";

	fmt.Format(L"%%.%df", afterPoint);
	sbuf.Format(fmt, k);
	return sbuf;
}

void Quat::SetSQL(PWS sql)
{
	_sql = sql;
	_sql.Trim();
	_sql.TrimRight(';');
#ifdef _DEBUG
	Apply(); // 원래는 GetSQL 해야 Apply 하는데
	_bApplied = true; // debug중에 최종 SQL이 보고 싶어서
#else
	_bApplied = false;
#endif
}

PWS Quat::GetSQL()
{
	if(!_bApplied)
		Apply(); // 결과가 _sqlv 로.
	return (PWS)_sqlv;
}
/*
bool Quat::Field(CJsonPbj& json, LPCSTR ka, BOOL bNecessary, BOOL bQuat, BOOL bNullIfEmpty)
{
	CString k(ka);
	PWS pval = json.QS(k, bNullIfEmpty, bQuat, bNecessary);//quat ''는 나중에 붙이게 FALSE
	if(bNecessary && pval == nullptr)
		throw_field(ka);
		//return false;
	_fields[(PWS)k] = pval;
	return true;
}*/

/// <summary>
/// 이미 있는 다른 Quat의 필드 정보를 복사 한다.
/// </summary>
/// <param name="qt"></param>
/// <returns></returns>
size_t Quat::CopyField(Quat& qt)
{
	this->_fields = qt._fields;
	return _fields.size();
}
/*
void Field(LPCSTR ka, TVal v)
{
	//auto uv = new CUbj();
	CUbj uv;
	//uv->Set(v);
	uv.Set(v);
	wstring sk = uv.GetTxtW();
	//_fields.SetAt(k, uv);
	//_fields[k] = uv;
	CString k(ka);
	_fields[(PWS)k] = sk;
}
*/
void Quat::Apply()
{
	_sqlv.Empty();
	_fields[L"NULL"] = L"NULL"; // 다 '' 싸는데 NULL 그야 말로 SET = COL1 = NULL 처럼 필드값이 삭제 된다. 행이 아니고.

	bool bAt = false;
	CString var;
	CString value;
	for(int i = 0; i < _sql.GetLength() + 1; i++)
	{
		WCHAR ch = i < _sql.GetLength() ? _sql.GetAt(i) : '\0';
		if(bAt)
		{
			if(KwIsAlNum(ch) || ch == '_')
			{
				var += ch;
			}
			else// 끝나고 '\0'도 여기로 온다.
			{
				CString value;
				//wstring val = _fields.find(var);
				auto it = _fields.find((PWS)var);
				if(it != _fields.end())
				{
					//value = Qs(it->second.c_str());// 여기서 'xxx' quat 가 씌워진다. (이미 씌워 져 있으므로)
					value = it->second.c_str();// 여기서 'xxx' quat 가 씌워져 있으므로 그래로 넣는다. NULL 에외
				}
				else
				{
					CString ser; ser.Format(L"SQL variable @%s is not found.(Apply error)", var);
					throw_str(ser);
				}

				_sqlv += value;
				_sqlv += ch;
				bAt = false;
				var.Empty();
			}
		}
		else
		{
			if(ch == '@')
				bAt = true;
			else
				_sqlv += ch;
		}

	}
	_bApplied = true;
}


/// update sql문에서 set 뒤에 f1 = v1, f2 = v2, f3 = v3 같이 다 만든다.
/// 맨뒤에는 , 를 제거.
/// _fields[L"SetField"]
bool Quat::AddSetField(PWS kw, JSONValue& jsv, BOOL bLast)
{
	CString value;
	auto it = _fields.find((PWS)kw);
	if(it != _fields.end())
	{
		value = it->second.c_str();// 여기서 'xxx' quat 가 씌워진다.
		CStringW sv;
		sv.Format(L"%s = %s", kw, (PWS)value);
		if(!bLast)
			sv += L", ";
		_setval += sv;
		if(bLast)
			_fields[L"SetField"] = _setval;
		// UPDATE table set @SetField where fBizID = @fBizID
		return true;
	}
	return false;
}

#endif // _QUAT


int KDatabase::SetKeyODBCMySQL(LPCWSTR sDSN, CString sval, CString val)
{
	return UpdateKeyODBCMySQL(sDSN, sval, val, false);
}
int KDatabase::RemoveKeyODBCMySQL(LPCWSTR sDSN, CString sval)
{
	return UpdateKeyODBCMySQL(sDSN, sval);
}
int KDatabase::UpdateKeyODBCMySQL(LPCWSTR sDSN, CString sval, CString val, bool bRemove)
{
	CString sKey0(_T("Software\\ODBC\\ODBC.INI\\"));
	CString sKey = sKey0 + sDSN;
	CRegKey reg;
	LSTATUS st = reg.Open(HKEY_CURRENT_USER, sKey);
	if(st != ERROR_SUCCESS)
		return -1;
	LSTATUS std = 0;
	if(bRemove)
		std = reg.DeleteValue(sval);
	else
		std = reg.SetStringValue(sval, val);
	reg.Flush();
	reg.Close();
	return 0;
}

int KDatabase::RegODBCMySQL(LPCWSTR sDSN, KWStrMap& kmap)// LPCTSTR sServer, LPCTSTR sDriver, LPCTSTR sDatabase, PWS sPort)
{
	CString sKey0(_T("Software\\ODBC\\ODBC.INI\\"));
	CString sKey = sKey0 + sDSN;

	CRegKey reg;
	reg.Create(HKEY_CURRENT_USER, sKey);
	// SERVER, UID, PORT, Driver, PORT, : TCPIP
	for(auto& [k, v] : kmap)
	{
		reg.SetStringValue(k.c_str(), v.c_str());
	}
	if(!kmap.Has(L"Driver"))
		reg.SetStringValue(L"Driver", L"MariaDB ODBC 3.1 Driver");
	if(!kmap.Has(L"SERVER"))
		reg.SetStringValue(L"SERVER", L"localhost");
	if(!kmap.Has(L"PORT"))
		reg.SetStringValue(L"PORT", L"3306");
	if(!kmap.Has(L"TCPIP"))
		reg.SetStringValue(L"TCPIP", L"1");
	reg.Flush();
	reg.Close();

	//KwRegSetValue(reg, L"utf8", _T("CHARSET"));// euckr 이면 KRecset::s_charSet = CP_ACP, CP_UTF8 이면 
	// ODBC셋업에서 페이지 2에 'Charcter Set:'이 있다. 거기 ecukr, utf8... 이 보인다.
	// 안하면 utf8로 된것같다.
// 	KwRegSetValue(reg, sDatabase, _T("DATABASE"));//kool
// 	KwRegSetValue(reg, sDriver, _T("Driver")); //C:\Program Files (x86)\MySQL\Connector ODBC 5.2\myodbc5w.dll
// 	KwRegSetValue(reg, sServer, _T("SERVER")); //call.gugose.co.kr
// 	KwRegSetValue(reg, sPort, _T("PORT")); //call.gugose.co.kr

	CRegKey reg1;
	CString sKey1 = sKey0 + L"ODBC Data Sources";
	reg1.Create(HKEY_CURRENT_USER, sKey1);
	reg1.SetStringValue(sDSN, L"MariaDB ODBC 3.1 Driver");
	reg1.Flush();
	reg1.Close();

	return 0;
}
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

void KwQueryKey(HKEY hKey, KWStrMap& kmap)
{
	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	TCHAR    achVal[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string 
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys = 0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cKeys;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 

	DWORD i, retCode;

	TCHAR  achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;
	DWORD cchKey = MAX_VALUE_NAME;

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

	// Enumerate the subkeys, until RegEnumKeyEx fails.

	if (cSubKeys)
	{
		TRACE("\nNumber of subkeys: %d\n", cSubKeys);
		for (i = 0; i < cSubKeys; i++)
		{
			cbName = MAX_KEY_LENGTH;
			retCode = RegEnumKeyEx(hKey, i,achKey,&cbName,NULL,NULL,NULL,&ftLastWriteTime);
			if (retCode == ERROR_SUCCESS)
				TRACE(TEXT("(%d) %s\n"), i + 1, achKey);
		}
	}
	// Enumerate the key values. 
	if (cValues)
	{
		TRACE("\nNumber of values: %d\n", cValues);
		for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
		{
			cchKey = MAX_VALUE_NAME;
			cchValue = MAX_VALUE_NAME;
			achKey[0] = '\0';
			achValue[0] = '\0';
// 			RegEnumValueW(
// 				_In_ HKEY hKey,
// 				_In_ DWORD dwIndex,
// 				_Out_writes_to_opt_(*lpcchValueName, *lpcchValueName + 1) LPWSTR lpValueName,
// 				_Inout_ LPDWORD lpcchValueName,
// 				_Reserved_ LPDWORD lpReserved,
// 				_Out_opt_ LPDWORD lpType,
// 				_Out_writes_bytes_to_opt_(*lpcbData, *lpcbData) __out_data_source(REGISTRY) LPBYTE lpData,
// 				_Inout_opt_ LPDWORD lpcbData
// 			);
			DWORD lType = 0;
			retCode = RegEnumValueW(hKey, i, achKey, &cchKey, NULL, &lType, (LPBYTE)achValue, &cchValue);
			if (retCode == ERROR_SUCCESS)
			{
				if (lType == REG_SZ)
				{
					TRACE(TEXT("(%d) %s: %s\n"), i + 1, achKey, achValue);
					kmap[achKey] = achValue;
				}
				else if (lType == REG_DWORD)
				{
					TRACE(TEXT("(%d) %s: %d\n"), i + 1, achKey, (int)(*achValue));
					wstring sval = std::to_wstring((int)(*achValue));
					kmap[achKey] = sval;
				}
				else //if (lType == REG_DWORD) 더 필요 하면 추가 한다.
				{//https://docs.microsoft.com/en-us/windows/win32/sysinfo/registry-value-types
					TRACE(TEXT("(%d) %s: type:%d excepted\n"), i + 1, achKey, lType);// (int)(*achValue));
				}
			}
		}
	}
}

void  KwEnumurateKey(HKEY hkey, PWS key, KWStrMap& kmap)
{
	HKEY hTestKey;
	if (RegOpenKeyEx(hkey, key, //TEXT("SOFTWARE\\Microsoft\\OneDrive"),
		0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
		KwQueryKey(hTestKey, kmap);
	RegCloseKey(hTestKey);
}

int KDatabase::RegGetODBCMySQL(LPCWSTR sDSN, KWStrMap& kmap)// LPCTSTR sServer, LPCTSTR sDriver, LPCTSTR sDatabase, PWS sPort)
{
	CString sKey0(L"SOFTWARE\\ODBC\\ODBC.INI\\");
	CString sKey = sKey0 + sDSN; //SOFTWARE\\ODBC\\ODBC.INI\\Winpache

	CStringA val;
	ULONG nChr = 200;
	ULONG nVal = 200;

	//_testmain();
	KwEnumurateKey(HKEY_CURRENT_USER, sKey, kmap);
/// 이건 서브키
// 	HKEY hTestKey;
// 	if (RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft", 0, KEY_READ, &hTestKey) == ERROR_SUCCESS)
// 		QueryKey(hTestKey);
// 	RegCloseKey(hTestKey);
	CRegKey reg;
	CStringW valw;
	/*
	auto ro = reg.Open(HKEY_CURRENT_USER, sKey, KEY_ALL_ACCESS);// KEY_READ | KEY_ENUMERATE_SUB_KEYS);
	if (ro != ERROR_SUCCESS)
		return -1;
	CString key;
	DWORD lType = 0;
	SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION;// { 0 };
	SECURITY_DESCRIPTOR sd = { 0 };
	DWORD szc = sizeof(si);
	reg.GetKeySecurity(si, &sd, &szc);
	for (int i = 0; i < 100; i++)
	{
		//auto rq2 = reg.EnumKey(i, val.GetBuffer(200), &nChr); val.ReleaseBuffer();
		/// 위에껀 키 즉, 폴더 같은 거다. value를 원하는 거다.
		auto rq2 = RegEnumValueW(reg.m_hKey, i, key.GetBuffer(200), (LPDWORD)&nChr, 0, 
			&lType, (LPBYTE)val.GetBuffer(200), (LPDWORD)&nVal); key.ReleaseBuffer(); val.ReleaseBuffer();
		if (rq2 == ERROR_SUCCESS)
			TRACE(L"%s: %s\n", key, val);
		else if (rq2 == ERROR_MORE_DATA)//ERROR_NO_MORE_ITEMS)//259  ERROR_MORE_DATA 234
			break;
	}
	PWS kes[] = { L"SERVER", L"UID", L"PORT", L"Driver", L"PORT", L"TCPIP", L"DESCRIPTION", };
	for (auto k : kes)
	{
		auto rq = reg.QueryStringValue(k, valw.GetBuffer(200), &nChr); valw.ReleaseBuffer();
		if (rq == ERROR_SUCCESS)
			kmap[k] = valw;
	}
	reg.Close();
	*/
	CString sKey1 = sKey0 + L"ODBC Data Sources";
	reg.Open(HKEY_CURRENT_USER, sKey1, KEY_ALL_ACCESS);
	auto rq1 = reg.QueryStringValue(sDSN, valw.GetBuffer(200), &nChr); valw.ReleaseBuffer();
	if (rq1 == ERROR_SUCCESS)
		kmap[L"ODBC Data Sources"] = valw;//L"MariaDB ODBC 3.1 Driver"
	reg.Close();

	return 0;
}



void KDatabase::CreateTablesInFolder(PWS folder)
{
	KDatabase& db = *this;
	CString path1 = folder;
	if(path1.Right(1) != '\\')
		path1 += '\\';
	WIN32_FIND_DATA wfd;
	HANDLE h = FindFirstFile(path1 + L"*.sql", &wfd);
	if(h == INVALID_HANDLE_VALUE)
		throw_str("SQL file not found int folder.");

	//CString allErr;
	KStdMap<wstring, wstring> artrg;//trigger는 table 다만든 후 한다.
	KStdMap<wstring, wstring> arvu;//trigger는 table 다만든 후 한다.
	for(;;)
	{
		CString fname = wfd.cFileName;
		if(!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			CString fnamew = path1 + fname;
			CStringA sql;//utf8
			if(!KwReadSmallTextFileA(fnamew, sql))
			{
				CStringA s; s.Format("%s file not found.", fname);
				throw_str(s);
			}
			CStringW sqlw;
			KwUTF8ToWchar(sql, sqlw);
			if(sqlw.Find(L"CREATE TRIGGER") >= 0)
				artrg.SetAt((PWS)fname, (PWS)sqlw);
			else if(sqlw.Find(L"CREATE ") >= 0 && sqlw.Find(L" VIEW ") >= 0)
				arvu.SetAt((PWS)fname, (PWS)sqlw);
			else
			{
				KArray<wstring> ar;
				SplitSQL(sqlw, ar);
				for(int i = 0; i < ar.GetCount(); i++)
				{
					try
					{
						db.ExecuteSQL(ar[i].c_str());
					}
					catch(CDBException* e)
					{
						(*_fncExceptionCTF)(fname, ar[i].c_str(), e);
// 						CString smsg; smsg.Format(L"Error! %s: %s, %s.", fname, e->m_strError, e->m_strStateNativeOrigin);
// 						allErr += smsg + L"\r\n";
					}
				}
			}
		}
		if(FindNextFile(h, &wfd) == FALSE)
			break;
	}
	FindClose(h);

	for(auto& [fn, wsql] : artrg)
	{
		KArray<wstring> ar;
		CStringW sqlw(wsql.c_str());
		SplitSQL(sqlw, ar);
		for(int i = 0; i < ar.GetCount(); i++)
		{
			try
			{
				db.ExecuteSQL(ar[i].c_str());
			}
			catch(CDBException* e)
			{
				/// CREATE TRIGGER 에서 위 아래 DELIMITER 문 오류 나서, 빼고 하니, 결국 지원 안하네.
				//+m_strError	L"This command is not supported in the prepared statement protocol yet\n"
				//+ m_strStateNativeOrigin	L"State:S1000,Native:1295,Origin:[ma-3.1.12][10.5.10-MariaDB]\n"
				(*_fncExceptionCTF)(fn.c_str(), ar[i].c_str(), e);
			}
		}
	}
	for(auto& [fn, wsqlv] : arvu)
	{
		KArray<wstring> ar;
		CStringW sqlw(wsqlv.c_str());
		SplitSQL(sqlw, ar);
		for(int i = 0; i < ar.GetCount(); i++)
		{
			try
			{
				db.ExecuteSQL(ar[i].c_str());
			}
			catch(CDBException* e)
			{
				(*_fncExceptionCTF)(fn.c_str(), ar[i].c_str(), e);
			}
		}
	}
// 	if(allErr.GetLength())
// 		throw_str(allErr);
}

/// <summary>
/// create table.. 이 있는 sql 파일이 있어야 하며, 
/// database는 지정되어있지 않았으므로 use `database`를 해줘야 한다.
/// </summary>
/// <param name="db">CDatabase dereved class</param>
/// <param name="fname">file name</param>
void KDatabase::CreateTable(KDatabase& db, PAS fname)
{
	CStringA sql;//utf8
	CStringW fnamew(fname);
	if (!KwReadSmallTextFileA(fnamew, sql))
	{
		CStringA s; s.Format("%s file not found.", fname);
		throw_str(s);
	}
	CStringW sqlw;
	KwUTF8ToWchar(sql, sqlw);
	KArray<wstring> ar;
	SplitSQL(sqlw, ar);
	for(int i = 0; i < ar.GetCount(); i++)
		db.ExecuteSQL(ar[i].c_str());
}

/// 
/// 이런문장은 OK =>    --테이블 데이터 winpache.tuser:~1, 093 rows(대략적) 내보내기
/// 이런문장은 Error => /*!40000 ALTER TABLE `tuser` DISABLE KEYS */;
///   Error문장은 제거 한다.
void KDatabase::SplitSQL(CStringW& sqlw, KArray<wstring>& ar)
{
	int irn = 0;
	CString sql;
	int nDelimiter = 0;
	CString delimiter(L";");
	for(; sqlw.GetLength() >= irn;)
	{
		auto i0 = sqlw.Find(L"\r\n", irn);
		CString ln;
		if(i0 >= 0)
		{
			ln = sqlw.Mid(irn, i0 - irn + 2);
			irn = i0 + 2;
		}
		else//마지막 개행이 없는 경우
		{
			ln = sqlw.Mid(irn);
			if(ln.GetLength() == 0)
				break;
			irn += ln.GetLength();
		}

		CString ln1 = ln;
		ln.Trim();
		bool bSkip = ln.GetLength() == 0; 
		bSkip |= ln.GetLength() >= 6 && tchbegin((PWS)ln, L"/*!") && tchend((PWS)ln, L"*/;");
		bSkip |= tchbegin((PWS)ln, L"-- ");

#define _DELIMITER_IS_SQL

		if(!bSkip)
		{
#ifdef _DELIMITER_IS_SQL
			if(tchbegin((PWS)ln, L"DELIMITER"))
			{
				sql += ln1;
				delimiter = ln.Mid(10);
				delimiter.Trim();
				///ar.Add((PWS)sql); DELIMITER 문은 실행 하지도 포함하지도 말아 보자.// https://forums.mysql.com/read.php?37,245058,247011#msg-247011
				sql.Empty();
				nDelimiter ^= 1; //스위치
			}
			else
			{
				//if(ln.Right(1) == L";")
				if(tchend((PWS)ln, (PWS)delimiter))
				{
					if(nDelimiter)// DELIMITER inside 에서는
					{
						CString ln2 = KwReplaceStr(ln1, (PWS)delimiter, L"");// END // 에서 '//'를 빼고 더한다.
						//ln1.TrimRight(delimiter);///안되네 이 함수
						sql += ln2;
					}
					else
						sql += ln1;
	
					ar.Add((PWS)sql);
					sql.Empty();
				}
				else
					sql += ln1;
			}
#else
			if(tchbegin((PWS)ln, L"DELIMITER"))
			{
				sql += ln1;
				delimiter = ln.Mid(10);
				delimiter.Trim();
				if(nDelimiter == 0) // "DELIMITER //" begin
				{
					nDelimiter++;
				}
				else // "DELIMITER ;" end
				{
					nDelimiter = 0;
					ar.Add((PWS)sql);
					sql.Empty();
				}
			}
			else
			{
				sql += ln1;
				if(nDelimiter)
				{
					// DELIMITER inside
				}
				else 
				{
					// DELIMITER outside
					if(tchend((PWS)ln, (PWS)delimiter))
					{
						ar.Add((PWS)sql);
						sql.Empty();
					}
				}
			}
#endif // _DELIMITER_IS_SQL

		}
	}
	TRACE("SplitSQL %d sqls.\n", ar.GetCount());
}


CKCriticalSection KDatabase::s_csDbConnect;

SHP<KDatabase> KDatabase::getDbConnected(wstring dsn, int sec)
{
	AUTOLOCK(s_csDbConnect);

	/// DSN : (openTick + KDatabase) map
	static KStdMapPtr<wstring, KList<SHP<KDatabaseOdbc>>> s_mapOdbc;
	SHP<KDatabase> rsdb;
	if(dsn.length() == 0)
		return rsdb;

	ULONGLONG tick = GetTickCount64();
	KList<SHP<KDatabaseOdbc>>* pldb = NULL;
	int ndbUsing = 0;
	int ndb = 0;

	if(!s_mapOdbc.Lookup(dsn, pldb))// 특정 dsn 연결중 놀고 있는 거 있나?
	{///1.dns의 것이 없으면 만들어서 리턴
		pldb = new KList<SHP<KDatabaseOdbc>>();
		auto sdbo = make_shared<KDatabaseOdbc>(dsn);
		sdbo->_sdb->OpenEx(sdbo->_dsn.c_str(), CDatabase::noOdbcDialog);

		pldb->push_back(sdbo);
		s_mapOdbc[dsn] = pldb;//하나의 dsn에 여러개 스레드별로 연결이 List에 넣어 둔다.
		sdbo->_tick = tick;
		rsdb = sdbo->_sdb;
	}
	else 
	{
		ndb = pldb->size();
		for(auto& sdbo : *pldb) // item: SHP<KDatabaseOdbc>
		{
			auto ou = sdbo.use_count();//이거는 상관 없다.
			auto du = sdbo->_sdb.use_count();///이거는 리턴 하기 때문에 use_count올라 간다.
			if(du == 1)// 1이면 shared된게 없어서 노는 중.
			{
				LONGLONG trm = tick - sdbo->_tick;
				if(trm < (1000L * (LONGLONG)sec))//연결만료 전이면
				{///2.1 놀고 있는 db가 있으면, 만료 되었나 확인 후 리턴
					if(!sdbo->_sdb->IsOpen())
						sdbo->_sdb->OpenEx(dsn.c_str(), CDatabase::noOdbcDialog);///never come. open 안된경우만
					sdbo->_tick = tick;
					rsdb = sdbo->_sdb;
				}
				else 
				{///2.2 만료된 db는 다시 연결 하여 리턴
					if(sdbo->_sdb->IsOpen())
						sdbo->_sdb->Close();
					sdbo->_sdb->OpenEx(dsn.c_str());
					sdbo->_tick = tick;
					rsdb = sdbo->_sdb;
				}
#ifndef _DEBUG
				break;
#endif // _DEBUG
			}
			else
				ndbUsing++;
		}

		if(!rsdb)
		{
			// 아직 리턴이 안되었다면 노는 db가 없다는 건데 그럼 추가 해야지.
			///3. 없거나 있더라도 사용 중이면 추가.
			auto sdbo = make_shared<KDatabaseOdbc>(dsn);
			sdbo->_sdb->OpenEx(dsn.c_str());
			pldb->push_back(sdbo);
			sdbo->_tick = tick;
			rsdb = sdbo->_sdb;
		}
	}
	//KTrace(L"getDbConnected: %d using: %d (%s)\r\n", ndb, ndbUsing, dsn.c_str());
	return rsdb;
}

void KDatabase::OpenOdbcSetting()
{
	WCHAR my_documents[MAX_PATH]{0,};
	::GetSystemDirectory(my_documents, MAX_PATH);//위랑 결과가 같다.
	CStringW flPrj = my_documents;
	flPrj += L"\\odbcad32.exe";
	::ShellExecute(0, 0, flPrj, 0, 0, SW_SHOW);
}
