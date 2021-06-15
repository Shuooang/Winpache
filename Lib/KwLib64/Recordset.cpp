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
		Close();//?�߿�: Close�ϸ� ���� CRecordset�� ��� ���� �ǹǷ� m_pDatabase = NULL�� �ȴ�.

	//L"ODBC;DSN=odisoDB;DRIVER={[MariaDB ODBC 3.1 Driver]};UID=odisou;PWD=odisopwd;DATABASE=odiso;PORT=3306;SERVER=192.168.35.175;TCPIP=1;"	ATL::CStringT<wchar_t,StrTraitMFC_DLL<wchar_t,ATL::ChTraitsCRT<wchar_t>>>
	CString sDSN = GetConnect();
	if (sDSN.GetLength() == 0)
		throw "DSN is null.";// ���� �ѹ��� ���� �õ� ���� ���� ���� ���� : ó������ �ʴ� ����.
	CString sDSN1 = sDSN.Mid(5);
	ASSERT(sDSN.Left(5) == L"ODBC;");
	OpenEx(sDSN1);
	//CRecordset::Open DB Error:������ ���� �̸��� ���� �⺻ ����̹��� �������� �ʾҽ��ϴ�.
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

// ���¹�: KRecset�� �ϳ��� �Ҵ����� ���� ���¿��� �ҷ��� �Ѵ�. ��: CheckDB()
void KDatabase::TimeCheckReopen()
{
	auto tick = GetTickCount64();
	auto intv = tick - _tLastAcess;
	if(intv > _secReopenInterval)
		Reopen();//reopen �ϸ� KRecordset�� CDatabase�� ���� �ȴ�.
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
		case -9: // VARCHAR �̰� sqlext.h �� ������ �̻��ϰ� mdb TEXT �� �̰ŷ� �ɸ���
			psErr = "MySQL_VARCHAR"; break;
		case -8: // CHAR �̰� mysql ���� enum type�ε� string�ΰ� ������...
			psErr = "MySQL_CHAR"; break;
		case -10: // TEXT �̰� access ���� ctype == SQL_C_CHAR == 1�ε� SQL_WLONGVARCHAR
			psErr = "MySQL_TEXT"; break;

		default:
			psErr.Format("SQL_UNKNOWN_%d", typeSql);
			ASSERT(0); // -8(enum)
	}
	return psErr;
}


// text�� ?�� ���̳ʸ��� �������� UTF8�� �ٲ㼭 �����ؾ� �ѱ��� ���δ�. KwWcharToUTF8
void KDatabase::ExecuteSQL(LPCWSTR sql, KDataPacket dts[], int numVar)
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

	ExecuteSQL(sql, numVar, pData, cbRv, cType, sqlType);
}
/// <summary>
/// ������ ���̳ʸ� �ʵ尡 ? �� �ϳ� �ϋ�
/// </summary>
void KDatabase::ExecuteSQL(LPCWSTR sql, PTR pData, SQLLEN cbRv, SQLSMALLINT cType, SQLSMALLINT sqlType)
{
	KDataPacket dts((char*)pData, cbRv, cType, sqlType);
	ExecuteSQL(sql, &dts, 1);
}

/// <summary>
/// insert or update���� ��� �Ѵ�.
/// </summary>
/// <param name="lpszSQL"></param>
/// <param name="numVar"></param>
/// <param name="pData"></param>
/// <param name="cbRv"></param>
/// <param name="cType"></param>
/// <param name="sqlType"></param>
void KDatabase::ExecuteSQL(LPCWSTR sql, int numVar, // '?'�� ���� :  BIND�ؾ��� PARAM ����
							PTR* pData,			// Data Point ARRAY 
							SQLLEN* cbRv,			// ���� ���޵� length ARRAY
							SQLSMALLINT* cType,		// array of c type
							SQLSMALLINT* sqlType)
{
	AUTOLOCK(_mxDbAcess);
	ULONGLONG tick = GetTickCount64();
	//auto intv = tick - this->_tLastAcess;
	//if(intv > this->_secReopenInterval)
	//	Reopen();//ReopenDb();

	KAtEnd d_sql([&, sql, tick]()->void {
		auto elapsed = GetTickCount64() - tick;
		if (_fncDbLog)
		{
			auto rv = (*_fncDbLog)(sql, (DWORD)elapsed);
			// rv < 0 �̸� ����
		}
	});

	if(pData == NULL)
	{
		__super::ExecuteSQL(sql);
		return;
	}
	//ASSERT(tchstri(lpszSQL, L"select") == NULL); ���뿡 ��������� �ֳ�.
	RETCODE nRetCode;
	HSTMT hstmt;

	ENSURE_VALID(this);
	ENSURE_ARG(AfxIsValidString(sql));

	AFX_SQL_SYNC(::SQLAllocStmt(m_hdbc, &hstmt));
	if(!CheckHstmt(nRetCode, hstmt))
		AfxThrowDBException(nRetCode, this, hstmt);

	TRY
	{
		OnSetOptions(hstmt);

		// Give derived CDatabase classes option to use parameters
		//BindParameters(hstmt); override�϶�� ��� �ְ�, �װ� �Ʒ����� �Ѵ�.

		for(int i=0;i<numVar;i++)
		{
			SQLLEN len = cbRv[i];
			SQLLEN lLength = len;

			lLength = SQL_LEN_DATA_AT_EXEC(lLength);
			nRetCode = SQLBindParameter(hstmt, i+1, SQL_PARAM_INPUT, cType[i], sqlType[i],
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
			for(int i=0;i<numVar;i++)
			{
				AFX_ODBC_CALL(::SQLParamData(hstmt, (SQLPOINTER*)&pTarget)); // get the pointer that will receive the data.
				if(!CheckHstmt(nRetCode, hstmt) && nRetCode != SQL_NEED_DATA)
					AfxThrowDBException(nRetCode, this, hstmt);

				SQLLEN lenSent = 0, dw=0;
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
			// Make final SQLParamData call. �̰� �����ָ� �ȵ�� ����.
			AFX_ODBC_CALL(::SQLParamData(hstmt, (SQLPOINTER*)&pTarget)); 
			if(!CheckHstmt(nRetCode, hstmt))
				AfxThrowDBException(nRetCode, this, hstmt);
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
/// ���⼭ reopen �ϸ� �̹� database�� �Ҵ�� krecset�� �ٽ� ���� �ؾ� �Ѵ�. �� null�� ���� ����.
/// </summary>
void KRecordset::ReopenDb()
{
	ASSERT(0);
	auto pdb = (KDatabase*)m_pDatabase;
	AUTOLOCK(pdb->_mxDbAcess);
	pdb->Reopen();//reopen �ϸ� KRecordset�� CDatabase�� ���� �ȴ�.
	this->m_pDatabase = pdb;//�� ���� �ָ� �Ʒ� Open���� ���� ODBCâ�� ��������.
}

BOOL KRecordset::OpenSelect(PWS sql)
{
	/// select �� �������� �ص� �������� ���°� ������
	/// �׷��� ���� Reopen ���� �����忡�� Close ���� �𸣴� 
	auto pdb = (KDatabase*)m_pDatabase;
	AUTOLOCK(pdb->_mxDbAcess);

	if(m_hstmt == SQL_NULL_HSTMT)
	{
		/// DB�� ���� �ȵǾ� �־ ���̴�, �׳� ���� ġ�� CDBExceptions�� �߻� �ϴµ�, 
		/// ���� ���⼭ throw �ϴ°� ����?
		throw_str("Database is not ready.");// ion goon away.");
		if(m_pDatabase == NULL)
		{
			//ReopenDb();//KTrace(L"m_pDatabase == NULL try ReopenDb()\n");
		}
		//�������� reopen�Ҷ� �Լ��� Rec(rec1); Rec(rec2); �̷��� ������ �̸� rec1�� Reopen�ϸ� rec2�� null�� �ǹ�����.
//		ASSERT(m_pDatabase != NULL);//KRecordset rs(&_db)�ϱ� ������ NULL�� �ƴ���.
		//ASSERT(pdb->_tLastAcess == 0);//���� ���� �������� 0�ʱⰪ�̰���.
		// ���� ������ �ȵ� ���
		//KwMessageBoxError(L"���� DB������ �ȵǾ���.");
	}

	auto tick = GetTickCount64();
	//auto intv = tick - pdb->_tLastAcess;
	//if(intv > pdb->_secReopenInterval)
	//	ReopenDb();//���⼭ reopen�ϸ� rec2�� �� �ǹ�����.

	KAtEnd d_sql([&, sql, tick]()->void {
		auto elapsed = GetTickCount64() - tick;
		if (pdb->_fncDbLog.get())
		{
			auto rv = (*pdb->_fncDbLog)(sql, (DWORD)elapsed);
			// rv < 0 �̸� ����
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
			pdb->_tLastAcess = GetTickCount64();//���� GetTickCount() �ؾ�. Reopen�ҋ��� �ϰŵ�

			break;//while���� Ż��
		}
		catch(CDBException* e)
		{
			KTrace(L"CRecordset::Open DB Error:%s", e->m_strError);
			KTrace(e->m_strStateNativeOrigin);
#ifdef _DEBUG1
			CDumpContext dc;
			this->Dump(dc);//CRecordset::Open Exception:�߸��� �μ��� �߰ߵǾ����ϴ�.
			e->ReportError();//Dialog���.
#endif
			if(KDatabase::IsGoneAway(e))
			{
				//IsGoneAway �϶��� �ٽ� ���� �ϴµ�, �ٸ� ��쵵 �ұ�?
				//ReopenDb();//�̹� �Ҵ�� KRecset�� m_pDatabase �� NULL ����� �����Ƿ� Recset���� reopen�ϸ� �ȵǰڴ�.
				throw_str("Database connection goon away.");
				//SetLoginTimeout SetQueryTimeout���� ���谡 ������.
				//continue;//CDBException�� state=08S01�ΰ�츸 
				//ASSERT(nReTry > 1);//gone away �� �� ���� ���� �� ���
			}
#ifdef _DEBUG2
			e->ReportError();//Dialog���.
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
		{//GetFieldValue ���� field info �̸� �˾� �ѷ���
			auto finfo = new CODBCFieldInfo;
			this->GetODBCFieldInfo(iCol, *finfo);
			_finfos.push_back(shared_ptr<CODBCFieldInfo>(finfo));
			_mapField.SetAt(finfo->m_strName, iCol);
		}

		//vector<ShRow> _tbl;
		for(this->MoveFirst(); !this->IsEOF(); this->MoveNext())//������ IsBOF
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
					case DBVT_NULL:	//�׼����� ����� �� �ִ� union ����� �����ϴ�.
						//ASSERT(0); //type�� ��� ����, NULL�ΰ��
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
				if(eCutomFetched == 1)// �̸� ���� �ѰŴ�. �׷��� ~~
				{
					//TODO: ���� ���ؾ� �Ѵ�. _tbl
					// �̹� �� cell�� ��� �Ͽ����Ƿ� ���� �뷮�� �ʹ� ū��� *vcol�� �׳� free�Ѵ�.
					delete vcol;
					vcol = NULL;//�̰� ���� �߿�
				}

				rowV->push_back(ShDBVar(vcol));//���� shared_ptr�� ��� ������ �ִ�.
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
/// <param name="point">�Ҽ��� �Ʒ� ����</param>
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
		TRACE("�𸣴� DBVT Type(%d)\n", v->m_dwType);//ASSERT(0);
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
		TRACE("�𸣴� DBVT Type(%d)\n", v->m_dwType);//ASSERT(0);
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
	
	case DBVT_NULL:	//	return NaN; nulalble�ʵ带 �׳� �а� �ߴٸ� ���� �Ǽ�
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
		return CTime();//1970-01-01 09:00:00 �� 9����?

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

// row�� �������ΰ��� ���� �ϰ� array �� hsr(key) = �� �ִ´�.

void KRecordset::MakeRecsetToJson(JObj& hsr, PS keyRecset)
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
				});// type�� ���� quato ����
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

// row 0�� �ϳ��� object �μ� hsr�� �ִ´�.

void KRecordset::MakeRecsetOneRowToJson(JObj& hsr, PS keyRecset)
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
	hsr(keyRecset) = robj;//ownership�� false �ϰ� hsr�� �ִ´�.
						  //}CATCH_GE;
}
// @valriable �� '%s' �ٲ��ش�.
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
		// 'abc \' "	\r\n123������. ` ; ABC'
		//if(pc != '\\') // \n => \\n ���� ���� �ʰ� �ϴ� ������ �ε�
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

PS Quat::Qs(PS val, BOOL bQuat, BOOL bNullIfEmpty)
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
			return L"[obj]";//�̷��� SQL������ ������ ���� ����.
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

PS Quat::Qs(int k, BOOL bNullIfMinusOne)
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

PS Quat::Qs(double k, BOOL bNullIfMinusOne, int afterPoint)
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
	Apply(); // ������ GetSQL �ؾ� Apply �ϴµ�
	_bApplied = true; // debug�߿� ���� SQL�� ���� �;
#else
	_bApplied = false;
#endif
}

PS Quat::GetSQL()
{
	if(!_bApplied)
		Apply(); // ����� _sqlv ��.
	return (PWS)_sqlv;
}
/*
bool Quat::Field(CJsonPbj& json, LPCSTR ka, BOOL bNecessary, BOOL bQuat, BOOL bNullIfEmpty)
{
	CString k(ka);
	PWS pval = json.QS(k, bNullIfEmpty, bQuat, bNecessary);//quat ''�� ���߿� ���̰� FALSE
	if(bNecessary && pval == nullptr)
		throw_field(ka);
		//return false;
	_fields[(PWS)k] = pval;
	return true;
}*/

/// <summary>
/// �̹� �ִ� �ٸ� Quat�� �ʵ� ������ ���� �Ѵ�.
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
	_fields[L"NULL"] = L"NULL"; // �� '' �δµ� NULL �׾� ���� SET = COL1 = NULL ó�� �ʵ尪�� ���� �ȴ�. ���� �ƴϰ�.

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
			else// ������ '\0'�� ����� �´�.
			{
				CString value;
				//wstring val = _fields.find(var);
				auto it = _fields.find((PWS)var);
				if(it != _fields.end())
				{
					//value = Qs(it->second.c_str());// ���⼭ 'xxx' quat �� ��������. (�̹� ���� �� �����Ƿ�)
					value = it->second.c_str();// ���⼭ 'xxx' quat �� ������ �����Ƿ� �׷��� �ִ´�. NULL ����
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


/// update sql������ set �ڿ� f1 = v1, f2 = v2, f3 = v3 ���� �� �����.
/// �ǵڿ��� , �� ����.
/// _fields[L"SetField"]
bool Quat::AddSetField(PWS kw, JSONValue& jsv, BOOL bLast)
{
	CString value;
	auto it = _fields.find((PWS)kw);
	if(it != _fields.end())
	{
		value = it->second.c_str();// ���⼭ 'xxx' quat �� ��������.
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


int KDatabase::RegODBCMySQL(LPCWSTR sDSN, KWStrMap& kmap)// LPCTSTR sServer, LPCTSTR sDriver, LPCTSTR sDatabase, PS sPort)
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

	//KwRegSetValue(reg, L"utf8", _T("CHARSET"));// euckr �̸� KRecset::s_charSet = CP_ACP, CP_UTF8 �̸� 
	// ODBC�¾����� ������ 2�� 'Charcter Set:'�� �ִ�. �ű� ecukr, utf8... �� ���δ�.
	// ���ϸ� utf8�� �ȰͰ���.
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
				else //if (lType == REG_DWORD) �� �ʿ� �ϸ� �߰� �Ѵ�.
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

int KDatabase::RegGetODBCMySQL(LPCWSTR sDSN, KWStrMap& kmap)// LPCTSTR sServer, LPCTSTR sDriver, LPCTSTR sDatabase, PS sPort)
{
	CString sKey0(L"SOFTWARE\\ODBC\\ODBC.INI\\");
	CString sKey = sKey0 + sDSN; //SOFTWARE\\ODBC\\ODBC.INI\\Winpache

	CStringA val;
	ULONG nChr = 200;
	ULONG nVal = 200;

	//_testmain();
	KwEnumurateKey(HKEY_CURRENT_USER, sKey, kmap);
/// �̰� ����Ű
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
		/// ������ Ű ��, ���� ���� �Ŵ�. value�� ���ϴ� �Ŵ�.
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
	db.ExecuteSQL(sqlw);
}
