#include "pch.h"

#include "ApiBase.h"
#include "KwLib64/Kw_tool.h"


HMODULE CApiBase::s_hDllExtra{ NULL };


//?deprecated
string CApiBase::CheckDB(PWS sDSN, SHP<KDatabase> dbLog)
{
	ASSERT(0);
	KDatabase _db;
	AUTOLOCK(_mxDbLog);//'Packets out of order' 이걸 해도 이 오류가 계속 나네
	string rs;
	try 
	{
		bool bDsnChanged = sDSN && !tchsame(sDSN, (PWS)_ODBCDSN);

		bool bDbMustOpen = false;
		if(bDsnChanged)
		{
			_db.Close();
			rs += "Close,";
			_ODBCDSN = sDSN;
			bDbMustOpen = true;
			//Server Information: MariaDB 10.04.000012
			//             DRIVER={MariaDB ODBC 3.1 Driver};TCPIP=1;SERVER=localhost;UID=myuser;PWD=********;DATABASE=maindb;PORT=3306
			//"DSN=odisoDB;DRIVER={[MariaDB ODBC 3.1 Driver]};UID=odisou;PWD=********;DATABASE=kdbase;PORT=3306;SERVER=api.domain.co.kr;TCPIP=1;";
				//[v] Enable automatic reconnect
				//[v] Allow multille statements
					//이거는 제어판/관리도구/ODBC설정에서 단지 이름과 DB종류(MariaDB)만 등록 한 경우
		}
		else
		{
			//+strMsg	L"MySQL server has gone away" << 이것을 방지 하기 위해 시간이 좀 겸과 하면 다시 DB연결 한다.
			//+strState	L"08S01,Native:2006,Origin:[ma-3.1.10][10.5.8-MariaDB]\n"	
			if(_db.IsOpen())
			{
				string rtc = _db.TimeCheckReopen();
				if(rtc.length())
					rs += rtc + ",";
			}
			else
				bDbMustOpen = true;
		}
		if(bDbMustOpen)
		{
			if(tchlen(sDSN) > 0 && _ODBCDSN.GetLength() > 0)
			{
				//CString dsn; dsn.Format(L"DSN=%s", _ODBCDSN);//이거는 모든 필수 항목이 다 들어가 있고.
				//_db.SetLoginTimeout(30);
				//_db.SetQueryTimeout(15);
				_db.OpenEx(_ODBCDSN);//_T("DSN=UserInfo"));
				rs += "Open,";
			}
		}

		/// 여기서 부터 로그 DB: 메인 DB를 붙이면 그만. 메인DB가 없으면 _ODBCDSNlog, 그것도 없으면 _ODBCDSN 생성.
		if (dbLog)
		{
			//ASSERT(!_db._dbLog); // 메인 디비를 초기화 해주는데 비어 있어야지.
			_db._dbLog = dbLog;

			if (_db._dbLog->IsOpen())
			{
				string rtc = _db._dbLog->TimeCheckReopen();// 연결 안끊기게 시간 지났으면 다시 접속
				if(rtc.length())//"reopen"
					rs += rtc + ":log,";
				TRACE("The '_db._dbLog' is already opened but reopened.");
			}
			else
			{
				if (_db._dbLog->IsReopenable())
				{
					_db._dbLog->Reopen();
					rs += "Reopen:log,";
					TRACE("The '_db._dbLog' is reopened.");
				}
				else
				{
					if (_ODBCDSNlog.IsEmpty())// dbLog도 안주고, DsnLog도 안준 경우 _ODBCDSN을 쓴다.
					{
						//_ODBCDSNlog = _ODBCDSN;
						// main DB도 없고 _ODBCDSNlog도 없으면 안만들어야지.
						TRACE("The '_db._dbLog' is not initialized. _ODBCDSNlog is empty.");
						rs = "The main dbLog is not connected";
					}
					else // _ODBCDSNlog 에 ODBC 셋팅에 비번까지 들어 있어야지. 안그러면 ODBC관리자 뜬다.
					{
						CString dsnLog; dsnLog.Format(L"DSN=%s", _ODBCDSNlog);//이거는 모든 필수 항목이 다 들어가 있고.
						_db._dbLog->OpenEx(dsnLog);
						rs += "Open:log,";
						//_db._dbLog->ExecuteSQL(L"use `winpache`");
						TRACE("The '_db._dbLog' is initialized. (%s)", CStringA(dsnLog));
					}
				}
			}
		}

		/// _db에 query가 불렸을때 로그를 쌓기 위해 불려지는 람다함수 등록
		_db.InitDbLog([&](PWS sql, DWORD elapsed)->int {
				return DoDbLog(sql, elapsed);
			});
	}CATCH_DB;
	return rs;
}

string CApiBase::CheckDbThread(PWS sDSN, SHP<KDatabase>& db, BOOL bSetDbLog)
{
	AUTOLOCK(_mxDbLog);//'Packets out of order' 이걸 해도 이 오류가 계속 나네
	string rs;
	try
	{
		if(!db)
			db = make_shared<KDatabase>();

		bool bDsnChanged = sDSN && !tchsame(sDSN, (PWS)_ODBCDSN);
		if(tchlen(sDSN) > 0 && _ODBCDSN.GetLength() > 0)
		{
			//_db.SetLoginTimeout(30);
			//_db.SetQueryTimeout(15);
			db->OpenEx(sDSN);//_T("DSN=UserInfo")); //_ODBCDSNlog
			rs += "Open,";
		}
		

		/// _db에 query가 불렸을때 로그를 쌓기 위해 불려지는 람다함수 등록
		if(bSetDbLog)
			InitDbLogProc(db.get());
	}CATCH_DB;
	return rs;
}

/// DB open은 기정 사실화 하고, 다시 체크 하는 거다.
void CApiBase::InitDbLogProc(KDatabase* db)
{
	db->InitDbLog([&](PWS sql, DWORD elapsed)->int {
		return DoDbLog(sql, elapsed);
		});
}


//?deprecated
string CApiBase::CheckDB()
{
	AUTOLOCK(_mxDbLog);//'Packets out of order' 이걸 해도 이 오류가 계속 나네
	string rs;
	ASSERT(0);
	KDatabase _db;
	if (_db.IsOpen())
	{
		string rtc = _db.TimeCheckReopen();
		if(rtc.length())
			rs += rtc + ",";
	}
	else if (_db.IsReopenable())     /// DB connect 없이 CheckDB() 들어 오면 IsReopenable로 연결되었는지 체크 하고
	{
		_db.Reopen();                /// 다시 연결 한다.
		rs += "Reopen,";
	}
	else                             /// 그리 하여 DB연결 안된 경우도 response 하도록 CDBException을 방지 한다.
	{
		rs += "FailConnect,"; /// 문자열 오류 리턴 하여, 메시지로 내보낸다.
		//return rs;
	}
	///?Site DB는 Open안되어 있어도, _dbLog는 Open일수도 있다.
	if(_db._dbLog)
	{
		if(_db._dbLog->IsOpen())
		{
			string rtc = _db._dbLog->TimeCheckReopen();// 연결 안끊기게 시간 지났으면 다시 접속
			if(rtc.length())//"reopen"
				rs += rtc + ":log,";
		}
		else
		{
			if(_db._dbLog->IsReopenable())
			{
				_db._dbLog->Reopen();
				rs += "Reopen:log,";
			}
			else
			{
				rs += "FailConnect:log"; /// 문자열 오류 리턴 하여, 메시지로 내보낸다.
				TRACE("%s\n", rs);
			}
		}
	}
	return rs;
}

/// 실제 쿼리가 있을때 기록 하는 루틴으로 불린다.
/// 주로 한꺼번에 로그를 쌓기 위해 매번 부르지 않고 버퍼링 한다.
int CApiBase::DoDbLog(PWS sql, DWORD elapsed)
{
	/// query외 elapsed 를 쌓아 놓았다가, ResponseForPost의 KAtEnd에서 DbQueryLog로 기록 한다. 
	/// 이부분은 KRecordset::OpenSelect 와 KDatabase::ExecuteSQL 끝날때 불려 진다.
	DWORD thID = ::GetCurrentThreadId();
	// 나중에 세션의 threadId로 검색 해야 한다. 로그 쌓는 두번째 스레드 아님.
	AUTOLOCK(_csDbQuery);//쿼리 쌓는 람다
	KPtrArray<KQueryLog>* arQry = NULL;
	if (!_dbQuery.Lookup(thID, arQry)) {
		arQry = new KPtrArray<KQueryLog>();
		_dbQuery.SetAt(thID, arQry);
	}
	arQry->Add(new KQueryLog(sql, elapsed));

	return 0;
}


/// <summary>
/// 이건 DB log가 아니고, request로그 이다.
/// </summary>
/// <param name="method">ex: POST, GET</param>
/// <param name="ip">192.68.12.23</param>
/// <param name="uuid"></param>
/// <param name="uparams"></param>
/// <param name="func"></param>
/// <param name="requUtf8"></param>
/// <param name="respUtf8"></param>
/// <param name="elapsed"></param>
/// <param name="retnv"></param>
/// <param name="UA_CPU"></param>
/// <param name="UserAgent"></param>
void CApiBase::LogRequest(CStringW method, CStringW ip, CStringW uuid, CStringW uparams, CStringW func, 
	string requUtf8, string respUtf8, ULONGLONG elapsed, int retnv, string UA_CPU, string UserAgent)//std::stringstream& rpost)
{
	BACKGROUND(2);
	//'Packets out of order' CCriticalSection 해도 이 오류가 계속 나네
	///CMutex로 바꾸니 더 안난다.
	AUTOLOCK(_mxDbLog);
	//shared_ptr<KDatabase> dbLog;// = make_shared<KDatabase>();
	///string rs = CheckDbThread(_ODBCDSNlog, dbLog, FALSE);///자신이 로그DB이므로 다시 로그를 설정하지 않는다.
	SHP<KDatabase> sdbLog = KDatabase::getDbConnected((PWS)_ODBCDSNlog);
	CString dbName = sdbLog->GetDatabaseName();
	CString dbCont = sdbLog->GetConnect();
	/// InitDbLogProc는 안한다. 로그를 또 로그 하냐?
	//dbLog->OpenEx(_ODBCDSNlog);//_T("DSN=UserInfo")); //_ODBCDSNlog

	//CStringA sReqA(sWrequ), sResA(sWresp);
	//KwWcharToUTF8(sWrequ, sReqA); /// 한글은 깨진다.
	//KwWcharToUTF8(sWresp, sResA);
	//
	// SQL_C_CHAR, SQL_CHAR
	// frequest MEDUUMTEXT SQL_C_CHAR, SQL_LONGVARCHAR
	/// fresponse LONGTEXT SQL_C_BINARY, SQL_LONGVARBINARY utf8mb4_bin : OK
	// frequest MEDUUMTEXT SQL_C_BINARY, SQL_LONGVARBINARY utf16_bin : fail
	/// frequest MEDUUMTEXT SQL_C_BINARY, SQL_LONGVARBINARY utf8mb4_bin : 한글까지 OK

	KDataPacket dts[2] ={
		KDataPacket((PAS)requUtf8.c_str(), -1, SQL_C_BINARY, SQL_LONGVARBINARY),// SQL_C_CHAR, SQL_CHAR),//TEXT  request는 JSON이 아니고 GET인 경우 url일 수 있다.
		KDataPacket((PAS)respUtf8.c_str(), -1, SQL_C_CHAR, SQL_LONGVARCHAR),//JSON은 바이너리 취급 한다.SQL_C_BINARY 가 디폴트 값이라 생략한다.
	};
	/// KDataPacket(PAS data, INT64 len = -1, int cType = SQL_C_BINARY, int sqlType = SQL_LONGVARBINARY)
	/// (PAS)(PWS)sWrequ 방법은 실패 { 말고 안것도 안나온다.
	int n= 1;
	CString sql;
#ifdef _DEBUG
	CString fTest = L"DEBUG";
#else
	CString fTest = L"release";
#endif
	CStringW sUA_CPU(UA_CPU.c_str());
	CStringW sUserAgent(UserAgent.c_str());
	CString fOS = L"";
	CStringW guidW(_GUID);

	auto wrtofile = [&]() {
		CFile flog;
		PWS fname = L"..\\..\\..\\log\\t_reqlog.log";
		if(!_bCheckLogfileDirectory)
		{
			int rd = KwCheckTargetDir(fname, FALSE, TRUE);
			if(rd == 0)
				_bCheckLogfileDirectory = true;
			else
				throw_str("Can not create directory for 'log'.");
		}
		if(!_bCheckLogfile_t_reqlog)
			_bCheckLogfile_t_reqlog = KwIfFileExist(fname);
		flog.Open(fname, (!_bCheckLogfile_t_reqlog ? CFile::modeCreate : 0) | CFile::modeReadWrite);
		flog.SeekToEnd();

		CStringA now = CStringA(KwGetCurrentTimeFullString());
		flog.Write((LPVOID)(PAS)now, now.GetLength());	flog.Write((LPVOID)"\r\n", 2);
		CStringA sqla(sql);
		flog.Write((LPVOID)(PAS)sqla, sqla.GetLength());	flog.Write((LPVOID)"\r\n", 2);
		CStringA reqA;
		KwUTF8ToChar(requUtf8.c_str(), reqA);
		CStringA resA;
		KwUTF8ToChar(respUtf8.c_str(), resA);
		flog.Write((LPVOID)(PAS)reqA, reqA.GetLength());	flog.Write((LPVOID)"\r\n", 2);// 20자로 맞춤
		flog.Write((LPVOID)(PAS)resA, resA.GetLength());	flog.Write((LPVOID)"\r\n\r\n", 4);
		flog.Close();
	};
	while(n--)
	{
		try {
			sql.Format(L"INSERT INTO t_reqlog (fSiteId, fTest, fip, fmethod, fuuid, fuparams, freturn, ffunc, felapsed, fUA_CPU, fUserAgent, frequest, fresponse) VALUES \r\n\
	('%s', '%s', '%s', '%s', '%s', '%s', %d, '%s', %I64u, '%s', '%s', ?, ?)"
				, guidW, fTest, ip, method, uuid, uparams, retnv, func, elapsed, sUA_CPU, sUserAgent);
			
			if (!sdbLog || !sdbLog->IsOpen())
			{
				// 위에 AUTOLOCK(_mxDbLog); 있으니 동기화 안해도 된다.
				wrtofile();
			}
			else
				sdbLog->ExecuteSQL(sql, dts, _countof(dts), KDatabase::eNoLog);
			break;
			/*
CREATE TABLE `treqlog` (
	`fidx` INT(11) NOT NULL AUTO_INCREMENT,
	`frating` ENUM('standard','good','error') NULL DEFAULT NULL COMMENT 'good, standard:설명넣어' COLLATE 'utf16_general_ci',
	`ffunc` VARCHAR(50) NULL DEFAULT NULL COMMENT '원격함수명' COLLATE 'utf16_general_ci',
	`fmethod` VARCHAR(8) NULL DEFAULT NULL COMMENT 'GET, POST...' COLLATE 'utf16_general_ci',
	`freturn` INT(11) NULL DEFAULT NULL,
	`felapsed` FLOAT(12) NULL DEFAULT NULL,
	`fuuid` VARCHAR(50) NULL DEFAULT NULL COLLATE 'utf16_general_ci',
	`frequest` TEXT(32767) NULL DEFAULT NULL COMMENT 'JSON은 LONGTEXT utf8mb4_bin으로 바뀐다.' COLLATE 'utf16_bin',
	`fresponse` LONGTEXT NULL DEFAULT NULL COLLATE 'utf8mb4_bin',
	`fexplain` VARCHAR(1024) NULL DEFAULT NULL COMMENT '원격함수 설명' COLLATE 'utf16_general_ci',
	`ftimestamp` TIMESTAMP NULL DEFAULT current_timestamp(),
	UNIQUE INDEX `fidx` (`fidx`) USING BTREE,
	INDEX `fuuid` (`fuuid`) USING BTREE
)
COMMENT='db query log'
COLLATE='utf16_general_ci'
ENGINE=MyISAM
AUTO_INCREMENT=2458
;
			*/
		}
		catch(CDBException* e)
		{
			wrtofile();
			CString s; s.Format(L"CException(%d):%s(%s) - %s %d\n", e->m_nRetCode, e->m_strError, e->m_strStateNativeOrigin, __FUNCTIONW__, __LINE__);
			TRACE(s);
		}
		catch(CException* e)
		{
			auto buf = new TCHAR(1024);//
			KAtEnd d_buf([&]() { delete buf;});//"Got packets out of order\n" Lost connection to MySQL server during query
			e->GetErrorMessage(buf, 1000);
			DWORD err = GetLastError();
			TRACE(L"CException(%u):%s - %s %d\n", err, buf, __FUNCTIONW__, __LINE__);
			if(tchcmp(buf, L"MySQL server has gone away\n") == 0)
				continue;
			else
				break;
		}
		catch(...)
		{
			TRACE("catch ...  - %s %d\n", __FUNCTION__, __LINE__);
			break;
		}
	}
}
/*
void CApiBase::LogException(CException* e, PWS func, int line)
void CApiBase::LogException(CException* e, PWS uuid)
{
	BACKGROUND(2);
	if(_db._dbLog.get() == NULL)
		return;
//	QueueFUNCN(([&]() -> void { e를 전달해줄 방법이 없고, 자주 발생 하는거 아니라 동기로 한다.
		LogExceptionAsync(e, uuid);
	//}), "LogExceptionAsync");
}*/
void CApiBase::LogException(CException* e, PWS uuid, PWS func, int line)
{
	//BACKGROUND(3);
	AUTOLOCK(_mxDbLog);//'Packets out of order' 이걸 해도 이 오류가 계속 나네
//	ASSERT(0);
	SHP<KDatabase> sdbLog = KDatabase::getDbConnected((PWS)_ODBCDSNlog);
	bool bFileLog = (!sdbLog || !sdbLog->IsOpen());
	PWS fname = L"..\\..\\..\\log\\t_excepsvr.log";

	try {

		CFile flog;
		if (bFileLog)
		{
			if (!_bCheckLogfileDirectory)
			{
				int rd = KwCheckTargetDir(fname, FALSE, TRUE);
				if (rd == 0)
					_bCheckLogfileDirectory = true;
				else
					throw_str("Can not create directory for 'log'.");
			}
		}
		if (!_bCheckLogfile_t_excepsvr)
			_bCheckLogfile_t_excepsvr = KwIfFileExist(fname);

		CString sql;
		auto* se = dynamic_cast<KException*>(e);
		CStringW guidW(_GUID);
		if (se)// rethrow 된거
		{
			CStringA sErr;
			CStringA sSta;
			KwWcharToUTF8(se->m_strError, sErr);//데이터 ? 저장은 utf8로 바꿔서.
			KwWcharToUTF8(se->m_strStateNativeOrigin, sSta);//데이터 ? 저장은 utf8로 바꿔서.
			KDataPacket dts[] ={
				KDataPacket(sErr, sErr.GetLength(), SQL_C_CHAR, SQL_CHAR),
				KDataPacket(sSta, sSta.GetLength(), SQL_C_CHAR, SQL_CHAR),
			};
			/// _GUID : site key
			/// uuid : cleint user key
			sql.Format(L"INSERT INTO t_excepsvr (fSiteId, fuuid, fexption, fstatus, ferror, fretcode, fserror, fstatedb, ffunc, fline) \
VALUES ('%s', '%s', '%s', '%u', '%d','%d', ?, ?, '%s', '%d')", guidW
	, uuid ? uuid : L"", CStringW(se->_sExcept), se->_status, se->_error, se->m_nRetCode, CStringW(se->_func), se->_line);
			if(!bFileLog)
				sdbLog->ExecuteSQL(sql, dts, _countof(dts), KDatabase::eNoLog);
			else
			{
				// 위에 AUTOLOCK(_mxDbLog); 있으니 동기화 안해도 된다.
				flog.Open(fname, (!_bCheckLogfile_t_excepsvr ? CFile::modeCreate : 0) | CFile::modeReadWrite);
				flog.SeekToEnd();

				CStringA now = CStringA(KwGetCurrentTimeFullString());
				flog.Write((LPVOID)(PAS)now, now.GetLength());	flog.Write((LPVOID)"\r\n", 2);
				CStringA sqla(sql);
				flog.Write((LPVOID)(PAS)sqla, sqla.GetLength());	flog.Write((LPVOID)"\r\n", 2);
				CStringA sErrA1(se->m_strError);//KwUTF8ToChar(sErrUtf8, sErrA);
				flog.Write((LPVOID)(PAS)sErrA1, sErrA1.GetLength());	flog.Write((LPVOID)"\r\n", 2);
				CStringA sErrA2(se->m_strStateNativeOrigin);//KwUTF8ToChar(sErrUtf8, sErrA);
				flog.Write((LPVOID)(PAS)sErrA2, sErrA2.GetLength());	flog.Write((LPVOID)"\r\n\r\n", 4);
				flog.Close();
			}
		} else {
			DWORD lastError = GetLastError();
			CString sErrorW;
			e->GetErrorMessage(sErrorW.GetBuffer(1024), 1024); sErrorW.ReleaseBuffer();
			CStringA sErrUtf8;
			KwWcharToUTF8(sErrorW, sErrUtf8);//데이터 ? 저장은 utf8로 바꿔서.
			KDataPacket dts[] ={
				KDataPacket(sErrUtf8, sErrUtf8.GetLength(), SQL_C_CHAR, SQL_CHAR),
				//KDataPacket(sSta, sSta.GetLength(), SQL_C_CHAR, SQL_CHAR),
			};
			CRuntimeClass* rc = e->GetRuntimeClass();
			CString sExcept(rc->m_lpszClassName);
			TRACE(L"CException(%u):%s - %s %d\n", lastError, sErrorW, func, line);
			sql.Format(L"INSERT INTO t_excepsvr (fSiteId, fuuid, fexption, fstatus, ferror, fserror, ffunc, fline) VALUES \r\n\
	('%s', '%s', '%s', '%u','%d', ?, '%s', %d)", guidW
				, uuid ? uuid : L"", CStringW(rc->m_lpszClassName), 0, lastError, func, line);
			if (!bFileLog)
				sdbLog->ExecuteSQL(sql, dts, _countof(dts), KDatabase::eNoLog);
			else
			{
				// 위에 AUTOLOCK(_mxDbLog); 있으니 동기화 안해도 된다.
				flog.Open(fname, (!KwIfFileExist(fname) ? CFile::modeCreate : 0) | CFile::modeReadWrite);
				flog.SeekToEnd();
				_bCheckLogfile_t_excepsvr = true;
				
				CStringA now = CStringA(KwGetCurrentTimeFullString());
				flog.Write((LPVOID)(PAS)now, now.GetLength());	flog.Write((LPVOID)"\r\n", 2);
				CStringA sqla(sql);
				flog.Write((LPVOID)(PAS)sqla, sqla.GetLength());	flog.Write((LPVOID)"\r\n", 2);
				CStringA sErrA(sErrorW);//KwUTF8ToChar(sErrUtf8, sErrA);
				flog.Write((LPVOID)(PAS)sErrA, sErrA.GetLength());	flog.Write((LPVOID)"\r\n\r\n", 4);
				flog.Close();
			}
		}
		SvrMessageBeep(4);
	}
	catch(CDBException* e)
	{
		TRACE(L"CException(%d):%s(%s) - %s %d\n", e->m_nRetCode, e->m_strError, e->m_strStateNativeOrigin, __FUNCTION__, __LINE__);
		SvrMessageBeep(4);
	}
	catch(CException* e)
	{
		auto buf = new TCHAR(1024);
		KAtEnd d_buf([&]() { delete buf;});
		e->GetErrorMessage(buf, 1000);
		DWORD err = GetLastError();
		TRACE(L"CException(%u):%s - %s %d\n", err, buf, __FUNCTION__, __LINE__);
		SvrMessageBeep(4);
	}
	catch(...)
	{
		TRACE("catch ...  - %s %d\n", __FUNCTION__, __LINE__);
		SvrMessageBeep(4);
	}

}


void CApiBase::SvrMessageBeep(int ncount)
{
	for(int i = 0; i < ncount; i++)
	{
		MessageBeep(MB_ICONERROR);
	}
}
void CApiBase::DbQueryLog(DWORD sessionThreadID, PWS func, PWS uuid)
{
	BACKGROUND(2);
	AUTOLOCK(_mxDbLog);//'Packets out of order' 이걸 해도 이 오류가 계속 나네
// 	if(this->_db._dbLog.get() == NULL)
// 		return;

	CString sql;
	DWORD thID = ::GetCurrentThreadId();
	//CSyncAutoLock lk(&this->_csDbQuery); //다른 스레드 에서도 쓴다.
	KPtrArray<KQueryLog> arq;
	CStringA qrA;//UNICODE를 byte로
	try
	{
		{// lock하고 복사한 후 처리 한다. 
			AUTOLOCK(_csDbQuery);//그사이 쌓아놓은 쿼리 소모 할때
			KPtrArray<KQueryLog>* arQry = NULL;
			//sessionThreadID는 BACKGROUND(1)의 세션의Thread ID이다.
			if (_dbQuery.Lookup(sessionThreadID, arQry))
			{
				for (int i = 0; i < arQry->GetCount(); i++)
					arq.Add(arQry->GetAt(i));
				arQry->clear(); // DeleteAll은 value인 pointer를 모두 free한다. 여기는 안한다.
			}
			this->_dbQuery.DeleteKey(thID);
		}

		SHP<KDatabase> sdbLog = KDatabase::getDbConnected((PWS)_ODBCDSNlog);
		bool bFileLog = (!sdbLog || !sdbLog->IsOpen());
		CFile flog;
		if (bFileLog)
		{
			PWS fname = L"..\\..\\..\\log\\t_ldblog.log";
			if (!_bCheckLogfileDirectory)
			{
				int rd = KwCheckTargetDir(fname, FALSE, TRUE);
				if (rd == 0)
					_bCheckLogfileDirectory = true;
				else
					throw_str("Can not create directory for 'log'.");
			}
			if (!_bCheckLogfile_t_ldblog)
				_bCheckLogfile_t_ldblog = KwIfFileExist(fname);
			flog.Open(fname, (!_bCheckLogfile_t_ldblog ? CFile::modeCreate : 0) | CFile::modeReadWrite);
			flog.SeekToEnd();
		}
		CStringW guidW(_GUID);
		for (int i = 0; i < arq.GetCount(); i++)
		{
			auto qlg = arq.GetAt(i);
			//qrA = CStringA(qlg->_sql);//UNICODE를 byte로
			KwWcharToUTF8(qlg->_sql, qrA);
			KDataPacket dts((PAS)qrA, qrA.GetLength(), SQL_C_CHAR, SQL_CHAR);
			sql.Format(L"INSERT INTO t_ldblog (fSiteId, fApp, fClass, fCalled, fTitle, fElpsed, fDetail) VALUES \r\n\
	('%s', 'Winpache', 'query', '%s', '%s', %u, ?)"
				, guidW, func, uuid ? uuid : L"", qlg->_elapsed);

			if (bFileLog)
			{
				//CBufArchive ar; 어차피 CFile::Write도 안에서 메모리 버퍼링 하므로 메모리에 모았다고 쓸필요 없다.
				/// 위에 AUTOLOCK(_mxDbLog); 있으니 동기화 안해도 된다.
				CStringA now = CStringA(KwGetCurrentTimeFullString());
				flog.Write((LPVOID)(PAS)now, now.GetLength());	flog.Write((LPVOID)"\r\n", 2);
				CStringA sqla(sql);
				flog.Write((LPVOID)(PAS)sqla, sqla.GetLength());	flog.Write((LPVOID)"\r\n", 2);
				CStringA dts1(qlg->_sql);
				flog.Write((LPVOID)(PAS)dts1, dts1.GetLength());	flog.Write((LPVOID)"\r\n\r\n", 4);
			}
			else
				sdbLog->ExecuteSQL(sql, &dts, 1, KDatabase::eNoLog);
		}
		if (bFileLog)
			flog.Close();
	}
	catch (CDBException* e)
	{
		CString qr(qrA);
		TRACE(L"DbQueryLog CDBException(%d):%s(%s)\n%s - %s %d\n", e->m_nRetCode, e->m_strError, e->m_strStateNativeOrigin, (PWS)qr, __FUNCTIONW__, __LINE__);
	}
	catch (CException* e)
	{
		auto ke = dynamic_cast<KException*>(e);
		if(ke)
		{
			TRACE(L"DbQueryLog error. %s\n", ke->m_strError);
		} else
			TRACE("DbQueryLog error.....................................\n");
	}
}
/*
void CApiBase::CellToJson(KRecordset& rs, int r, int c, JObj& robj)
{
	int eType = rs._finfos[c]->m_nSQLType;
	CDBVariant* v = rs.GetCell(r, c);
	if(v->m_dwType == DBVT_NULL)//DB에 값이 NULL인 경우 JSON에서 "ID":null, 처럼 null을 지원한다.
	{
		robj((PWS)rs._finfos[c]->m_strName) = (PWS)NULL;//type = JSONType_Null JSONValue::JSONValue(NULL)
		return;
	}


	switch(eType)
	{
		case SQL_GUID:
		case SQL_LONGVARCHAR:
		case SQL_VARCHAR:
		case SQL_CHAR:
		case -8://CHAR1
		case -9://VARCHAR1
		case -10://TEXT1
			robj((PWS)rs._finfos[c]->m_strName) = rs.Cell(r, c);
			break;
		case SQL_DECIMAL://이거는 문자열 상태 숫자
			robj((PWS)rs._finfos[c]->m_strName) = rs.Cell(r, c);
			break;
		case SQL_LONGVARBINARY://이거는 "0x1234566" 문자열 상태 숫자
		case SQL_VARBINARY:
		case SQL_BINARY:
			robj((PWS)rs._finfos[c]->m_strName) = rs.Cell(r, c);
			break;
			//case SQL_INTERVAL: == SQL_TIME
		case SQL_TIME:// "2020-10-26 02:16:00" 문자열로
		case SQL_TIMESTAMP:// "2020-10-26 02:16:00" 문자열로
		case SQL_DATETIME:// "2020-10-26 02:16:00" 문자열로
			robj((PWS)rs._finfos[c]->m_strName) = rs.Cell(r, c);
			break;
		case SQL_BIGINT:
			TRACE("SQL_BIGINT Type(%d)\n", eType);//ASSERT(0);
		case SQL_INTEGER:
		case SQL_SMALLINT:
		case SQL_TINYINT:
		case SQL_BIT:
			robj((PWS)rs._finfos[c]->m_strName) = rs.CellI(r, c);
			break;
		case SQL_FLOAT:
		case SQL_REAL:
		case SQL_DOUBLE:
			robj((PWS)rs._finfos[c]->m_strName) = rs.CellD(r, c);
			break;
		case SQL_NUMERIC:
			robj((PWS)rs._finfos[c]->m_strName) = rs.CellD(r, c);
			break;
		default:
			robj((PWS)rs._finfos[c]->m_strName) = rs.Cell(r, c);
			TRACE("모르는 SQL Type(%d)\n", eType);//ASSERT(0);
			break;
	}
	//throw "Cell Unknow Type";
}
*/


void CApiBase::GetPolicy(JObj& jpara, JObj& jpolicy)
{
	// jpara에서 fuuid참조 하여 맞춤형 policy
	// jpolicy("someConfir") = 100;
}

// row가 여러개인것을 전제 하고 array 를 hsr(key) = 에 넣는다.
/*
void CApiBase::MakeRecsetToJson(KRecordset& rs, JObj& hsr, PS keyRecset)
{
	try
	{
		//CStringArray arHd;
		//rs.GetFieldNames(arHd);
		JArr ar;
		for(int r = 0;r<rs.RowSize();r++)
		{
			JObj robj;
			for(int c = 0;c<rs.ColSize();c++)
			{
				rs.CellToJson(r, c, robj, [&](JObj&, CDBVariant*) -> int {
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
	}CATCH_GE;
}

// row 0번 하나만 object 로서 hsr에 넣는다.
void CApiBase::MakeRecsetOneRowToJson(KRecordset& rs, JObj& hsr, PS keyRecset)
{
	try
	{
		if(rs.RowSize() == 0)
			return;
		JObj robj;
		for(int c = 0; c < rs.ColSize(); c++)
		{
			rs.CellToJson(0, c, robj, [&](JObj&, CDBVariant*) -> int { return 0;});
		}
		hsr(keyRecset) = robj;//ownership을 false 하고 hsr에 넣는다.
	}CATCH_GE;
}
*/



// 여기에 API에 쓰일 함수를 맵핑 한다.
bool CApiBase::MapRemoteFunctions()
{
	if(m_mapRFncs.GetCount() > 0)
		return true;
	//?RemoteAPI 2: 매핑 (함수명과 실제 함수)

	int iOp1 = 0;
	AddApi("SampleApi1", [&](KDatabase& _db, JObj& jpa, JObj& jrs, int iOp) -> int {	return SampleApi1(_db, jpa, jrs, iOp);	}, iOp1);
	//AddLambdaApi(SampleApi1, iOp);

	//m_mapRFncs.SetAt(#fnc, CFuncItem(&ApiSite1::fnc, iOp));
	//m_mapRFncs.SetAt("SampleApi1", CFuncItem(&CApiBase::SampleApi1, iOp));
//	RemoteFunc(SampleApi1, CFuncItem::eFncNoDB);//CFuncItem::eFncPolicy);
	//RemoteFunc(SampleSelectUUID, 0);
	//RemoteFunc(SampleSelect, 0);
	//RemoteFunc(SampleInsert, 0);
	//RemoteFunc(SampleUpdate, 0);
	//RemoteFunc(SampleDelete, 0);
	return false;
}
// DB 미사용 예제
int CApiBase::SampleApi1(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	static int si2 = 0;

	jrs("Serial") = si2++;
	jrs("name") = "kdw";
	jrs("title") = "This is reponse of POST.";
	jrs("Return") = L"OK";
	// "{"Serial":123, "name":"kdw", "title":"This is reponse of POST.", "Return":"OK"}"
	return 0;
}

//?RemoteAPI 3: 정의 (끝)


int CApiBase::FileOpen(string url, KBinary& fbuf)
{
	int rv = 0;
	CStringA surl(url.c_str());// '/images/org/8D23D27A-4286-4308-B8F1-EF17916B19EF.jpeg'
	CStringA surlr = KwReplaceStr(surl, "/", "\\");// '\\images\\org\\8D23D27A-4286-4308-B8F1-EF17916B19EF.jpeg'

// 	if(surlr == L"\\")
// 		surlr += _defFile;
	CStringA local;
	CString rootLocal = KwReplaceStr(_rootLocal, L"/", L"\\");
	if (_rootURL == "/")
		local = CStringA(rootLocal) + surlr;
	else
		local = CStringA(rootLocal) + surlr.Mid(_rootURL.GetLength());
	/*vector<string> arp;// http://v2.petme.kr:19479/files/images/org/8D23D27A-4286-4308-B8F1-EF17916B19EF.jpeg
	auto np = KwCutByToken(url, "/", arp, 0);// [0]'', [1]files, [2]images
	CStringA local = _rootLocal + "\\";
	for (int i = 2; i < (np - 1); i++)//[2]부터 해야 한다.
	{
		local += arp[i].c_str();
		local += "\\";
	}
	local += arp[np-1].c_str();*/
	try
	{
		CStringW lcw(local);
		CFile f(lcw, CFile::modeRead | CFile::shareDenyNone);/// 동시에 보내면 파일 공유위반 exception이 나서 shareDenyNone 해본다.
		KAtEnd d_f([&]() { f.Close(); });
		
		auto len = f.GetLength();
		fbuf.Alloc((UINT_PTR)len);
		//fbuf = std::make_shared<char>(len);
		f.Read(fbuf.m_p, (UINT)len);
	}
	catch (CException* e)
	{
		throw e;
	}
	//}CATCH_GE;
	return rv;
}

int CApiBase::ImageUpload(PAS data, size_t len, string orgname, PAS contType, PAS contLeng, JObj& jsr, 
							PAS tableSrc, int idx, PAS sOp)//bool bTransaction)
{
	CStringW guid;
	// _rootLocal;//(L"C:\\Temp\\images");
	// _rootURL;//("http://211.253.36.217/images/");
	int rv = 0;
	bool bTransaction = tchstr(sOp, "transaction") != NULL;
	bool bSaveToJpeg = tchstr(sOp, "jpeg") != NULL;
	CString sExt, sExt1;
	CStringA sFitSize;
	try
	{
		string sContType = contType;
		guid = KwGetFormattedGuid(true);
		string ext = sContType.substr(6);// "images/" 뒤에 부분
		sExt = CString(ext.c_str());
		sExt.MakeLower();
		sExt1 = sExt;
		CStringArray arPath;//1024,1536x2048, 1440x2560, 1080x1920
		CSize arSize[] = { CSize(1440,1440), CSize(512,512), CSize(256,256), CSize(128,128)};
		sFitSize.Format("%dx%d", arSize[0].cx, arSize[0].cy);
		CString path;
		path.Format(L"%s\\images\\org\\%s.%s", _uploadLocal, guid, sExt);//org는 확장자도 그대로
		arPath.Add(path);
		if (bSaveToJpeg)
			sExt1 = L"jpg";
		for(int i=0;i<_countof(arSize);i++)
		{
			path.Format(L"%s\\images\\%dx%d\\%s.%s", _uploadLocal, arSize[i].cx, arSize[i].cy, guid, sExt1);
			arPath.Add(path);
		}
		CStringA extSave("large");
		if (bSaveToJpeg)
			extSave += ",jpg";// : nullptr; //?todo: 사용자가 올릴때는 "jpg" 로 저장하고 sExt 도 바꾸어야 한다.
		rv = KwSaveImageMultiSize((LPVOID)data, len, arPath, arSize, extSave);
		if(rv == 1)
		{
			throw_rcd(rv, L"Image Stream Error.");
		}
		else if(rv == 2)
			throw_rcd(rv, L"Image Format Error.");
	}CATCH_GE;

	SHP<KDatabase> sdb = KDatabase::getDbConnected((PWS)_ODBCDSN);

	try
	{
		//CheckDB();
		bool bFileLog = (!sdb || !sdb->IsOpen());
		
		if(bTransaction)
			sdb->TransBegin();
		//KAutoTransaction _dbtr(_db); // 정의 만으로 자동 commit 되는 객체.
		// 트랜색션은 try, CATCH_DBTR 를 써야 한다.
		CStringA rootURL = _rootURL;
		if (_rootURL.Right(1) == "/")
			rootURL.TrimRight("/");
		CStringA imgURL = rootURL + "/images";
		CStringA sqlA;
		CStringA oldSrc; 
		if (tableSrc)
			oldSrc.Format("'%s', %d", tableSrc, idx);//구 버전1테이블 참조
		else
			oldSrc = "null, null";
		sqlA.Format("insert into t_images (fImgID, fExt, fUrlDir, fFitSize, fContentType, fContentLength, fTableSrc, _idx) values \
('%s', '%s', '%s', '%s', '%s', %s, %s)", CStringA(guid), CStringA(sExt1), imgURL, sFitSize, contType, contLeng, oldSrc);
		CString sql(sqlA);
		sdb->ExecuteSQL(sql);

		jsr("fImgID") = guid;
		jsr("Return") = L"OK";

		if (bTransaction)
			sdb->TransCommit();
		return 0;
	}//CATCH_DBTR;// Rollback한다. rethrow한다. bTransaction때문에 CDBException 부분 따온다.
	catch (CDBException* e)
	{	TRACE("CDBException:%s - %s %d\n", CStringA(e->m_strError), __FUNCTION__, __LINE__);
		if (bTransaction)//이것(선택적 트랜잭션)때문에 CATCH_DBTR 안쓰고 override한 이유 이다.
			sdb->TransRollback();
		throw new KException("CDBException", GetLastError(), e->m_nRetCode, e->m_strError, e->m_strStateNativeOrigin, __FUNCTION__, __LINE__);
	}CATCH_GE
	return rv;//rethrow하므로 여기 안오고 부른쪽에서 catch해야 한다.

}


int CApiBase::SampleSelect(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		// 1. DB사용 초기화
		KRecordset rs(&_db);//getDB(jpa));
//		KRecordset rs(&_db);
		CString table;
		if (tchsame(jpa.S("uuid"), L"9470777"))
			table = jpa.S("table");
		else
			table = L"tuser";

		Quat qs;//bNecessary, bQuat, bNullIfEmpty
		//qs.Field(jpa, "table", TRUE);
		//qs.Field(jpa, "limit");
		qs_Field2(qs, table, 1, 0);// 필수,노''

		// 2. request parmameter 분석
		CString where1;
		CString limit1;
		CString sql;
		if(jpa.Has("where"))
		{
			where1.Format(L"where %s", jpa.S("where"));
			qs_Field3(qs, where1, 0, 0, 0);
		}
		else
			qs.Field("where1", L"", 0,0,0);
		//sql.Format(L"select * from `%s` %s limit %d", table, where1, jpa.I("limit"));
		if(jpa.Has("limit"))
		{
			limit1.Format(L"limit %d", jpa.I("limit"));
			qs_Field3(qs, limit1, 0, 0, 0);
		}
		else
			qs.Field("limit1", L"limit 100", 0, 0, 0);

		sql = L"select * from `@table` @where1 @limit1";
		qs.SetSQL(sql);

		// 3. SQL query실행
		BOOL bOpen = rs.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.

		// 4. SQL Query결과를 JSON으로 리턴
		rs.MakeRecsetToJson(jrs, table);
		jrs("Return") = L"OK";
	}CATCH_DB;
	return 0;
}

int CApiBase::SampleInsert(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		CString sErr("abcd가나다라");
		CStringA sErrA(sErr);
		CStringA sErrU8;
		KwWcharToUTF8(sErr, sErrU8);
		KDataPacket dts[] = {
	KDataPacket(sErrU8, sErrU8.GetLength(), SQL_C_CHAR, SQL_CHAR),
	//KDataPacket(sSta, sSta.GetLength(), SQL_C_CHAR, SQL_CHAR),
		};
		//sql.Format(L"INSERT INTO texcepsvr (fuuid, fexption, ferror, fserror) VALUES ('%s', '%s', '%u', ?)"
		//									, uuid ? uuid : L"", CStringW(rc->m_lpszClassName), lastError);

		// 트랜색션은 try, CATCH_DBTR 를 써야 한다.
		_db.TransBegin();// Query가 2개 이상일때 부터 사용. 여기 예제는 1개 일때 사용 예.
		CStringW guidW(_GUID);
		CString sql;
		sql.Format(L"INSERT INTO t_excepsvr (fSiteId, fuuid, fexption, ferror, fserror) VALUES \r\n\
	('%s', '%s', '%s', '%u', ?)"
			, guidW, L"CApiBase::SampleInsert", L"CException", 10);
		_db._dbLog->ExecuteSQL(sql, dts, _countof(dts), KDatabase::eNoLog);

		//sql.Format(L"insert into tuuid (fuuid, fsrcuuid) values ('%s', '%s')", jpa.S("fuuid"), jpa.S("fsrcuuid"));
		//_db.ExecuteSQL(sql);
		jrs("Return") = L"OK";
		_db.TransCommit();
	}CATCH_DBTR;
	return 0;
}

int CApiBase::SampleUpdate(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp) {
	try
	{
		//KAutoTransaction _dbtr(_db); // 정의 만으로 자동 commit 되면 안되. exception에서 Rollback 하애 하는데.
		if(!jpa.Has("fuuid"))
			throw_field("fuuid");
		
		if(!jpa.Len("fuuid"))
			throw_BadRequest(-1, "Field 'fuuid' is too short.");

		CString sql;
		sql.Format(L"update tuser set fname = '%s' where fuuid = '%s'"
			, jpa.S("fname"), jpa.S("fuuid"));
		_db.ExecuteSQL(sql);
		jrs("Return") = L"OK";
	}CATCH_DBTR;
	return 0;
}

int CApiBase::SampleDelete(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp) {

	//CheckDB(); 부른쪽에서 해준다.
	KRecordset rs(&_db);//getDB(jpa));
	//KAtEnd d_rs([&] {	rs.Close();}); // ~CRecordset()에 있는데 왜또해
	jrs("Return") = L"OK";
	return 0;
}




CString CApiBase::DevGuid(PWS title)
{
	CString uuid = KwGetFormattedGuid(true, title, 8);
//#ifdef _DEBUG
//	CString dvguid;
//	if(title)
//		dvguid = CString(title) + L"-" + uuid;
//	else
//		dvguid = uuid;
//	return dvguid;
//#else
	return uuid;
//#endif
}
