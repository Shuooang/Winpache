#pragma once
#include "KwLib64/Kw_tool.h"
#include "KwLib64/Recordset.h"
#include "Response1.h"

class CApiBase;// 선언 전에 쓰일떄는 이렇게 declare만 하고, 아래 에서 define한다.
/// <summary>
/// 원격 함수 Member function pointer 형식을 선언한다.
/// - const를 안붙인이유는 변경 해봐야 별일 안생기므로
/// </summary>
//typedef int(CApiBase::* FuncApi)(JObj&, JObj&, int);
typedef function<int(KDatabase& _db, JObj&, JObj&, int)> FuncApi;
typedef SHP<FuncApi> SFuncApi;


/// <summary>
/// 원격함수 dic에서 value위치에 들어갈 type
/// </summary>


class CResponse1;

class CFuncItem
{
public:
	CFuncItem(SFuncApi func1 = NULL, int iOp = 0)
		: _func(func1), _iOp(iOp)
	{
	}
	SFuncApi _func;//Member function pointer
	int _iOp;
};

enum EOp
{
	eFncApp = 1 << 0, // 앱전용
	eFncBiz = 1 << 1, // PC앱 전용
	eFncAdmin = 1 << 2, // PC앱 슈퍼관리자 전용
	eFncPolicy = 1 << 3,// 리턴할때 사용자 정책도 함께 한다.
	eFncContract = 1 << 4,// 업데이트된 약관을 가지고 간다.
	eFncAccess = 1 << 5,
	eFncMsg = 1 << 6,// 사용자에게 앱 시작시 보여줄 메시지 있을떄
	eFncHttpGet = 1 << 7, // HTTP GET 도 지원 한다.
};
//eFncNoDB = 1 << 7,        // DB를 사용 하지 않은 static한 응답 할때 CheckDB를 안해도 괸다.

/// <summary>
/// 모든 api가 들어 있는 곳
/// </summary>
//template<typename FuncApi>
class CApiBase
{
public:

	CApiBase()
		: _maxFileSize(7000000)
		, _resp(nullptr)
		, _mxDbLog(FALSE, L"_mxDbLog")
	{
	}
	static HMODULE s_hDllExtra;

	//KDatabase _db;

	/// <summary>
	/// request 입력 jpa 파라미터에 따라 site db를 쓸건지, main db를 쓸건지 정한ㄷ.
	/// </summary>
	/// <param name="jpa">request 입력 jpa 파라미터</param>
	/// <returns></returns>
// 	virtual KDatabase* getDB(JObj& jpa)
// 	{
// 		return &_db;
// 	}


	CResponse1* _resp;//상호참조
	CStringA _GUID;
	CString _fileKey;// doc에서 스타트 할때 초기화 되어서, DB에 로그 쌓일때 쓰인다.
	CString _ODBCDSN, // DSN=DsnName;UID=id;PWD=pwd;database=dbname
		_ODBCDSNlog;
	CString _rootLocal;
	CString _uploadLocal{ L"c:\\svr\\upload" };
	CStringA _defFile{ "index.html" };

	CStringA _rootURL{ "/" };//이건 나중에 고정 된다.
	CStringA _ApiURL{ "/api" }; // api call 할때 가상으로 앞에 붙는 dir 이거 주면 원격 함수

	bool _bCheckLogfileDirectory{ false };
	bool _bCheckLogfile_t_reqlog{ false };
	bool _bCheckLogfile_t_excepsvr{ false };
	bool _bCheckLogfile_t_ldblog{ false };
	
	virtual string CheckDB(PWS sDSN, SHP<KDatabase> dbLog);
	virtual string CheckDB(PWS sDSN)
	{
		SHP<KDatabase> dbLog;
		return CheckDB(sDSN, dbLog);
	}
	///스레드별로 따로 컨넥션을 갖기로
	virtual string CheckDbThread(PWS sDSN, SHP<KDatabase>& db, BOOL bSetDbLog = TRUE);

	void InitDbLogProc(KDatabase* db);
	/// <summary>
	/// 내부에서 에러 메시지 내 보낼때, output창 등으로 내보낼때 이 람다를 부른다.
	/// </summary>
	SHP<function<void(string, int)>> _fncOutput;
	template<typename TFNC> void AddCallbackOutput(TFNC fnc)
	{
		_fncOutput = std::make_shared<function<void(string, int)>>(fnc);
	}
	virtual string CheckDB();

	virtual int DoDbLog(PWS sql, DWORD elapesed);

	virtual void GetPolicy(JObj& jpara, JObj& jpolicy);

	virtual PWS getExLibName()
	{
		return NULL;
	}
		
//	void CellToJson(KRecordset& rs, int r, int c, JObj& robj);


	//void MakeRecsetToJson(KRecordset& rs, JObj& hsr, PWS keyRecset = L"table");

	//void MakeRecsetOneRowToJson(KRecordset& rs, JObj& hsr, PWS keyRecset);

	// odiso에서 MakeRecsetToJson2 형식을 참조
	template<typename Func>
	void MakeRecsetToJson(KRecordset& rs, JObj& hsr, PWS keyRecset = L"table", Func lmda = [](KRecordset&, int, int, JObj&) -> int {})
	{
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


	/// <summary>
	/// 요청 들어오면 응답과 함께 DB에 쌓는다. 전송문은 JSON포맷이다.
	/// </summary>
	/// <param name="sWstr">CStringW 요청</param>
	/// <param name="rpost">CStringW 응답</param>
	void LogRequest(CStringW method, CStringW ip, CStringW uuid, CStringW uparams, CStringW func, string requUtf8, string respUtf8, 
		ULONGLONG elapsed, int retnv, string UA_CPU, string UserAgent);
	//CCriticalSection _csLogReq;//'Packets out of order' 오류 나서 뮤텍스로 교체후 오류가 안난다.
	CMutex _mxDbLog;
	/// <summary>
	/// 서버 오류를 DB에 기록한다.
	/// </summary>
	/// <param name="e"></param>
	void LogException(CException* e, PWS uuid = nullptr, PWS func = nullptr, int line = 0);
	//void LogException(CException* e, PWS func = nullptr, int line = 0)	{
	//	LogException(e, nullptr, func, line);
	//}
	//CCriticalSection _csLogExcep;
	//CMutex _mxLogExcep;



	/// db log를 쌓으려면 threadId : array<query,elapsed> 이렇게 3차원 dic이 필요
	CKCriticalSection _csDbQuery;//여러 스레드가 이용 하므로
	CKRbPtr<DWORD, KPtrArray<KQueryLog>> _dbQuery;
	void SvrMessageBeep(int ncount = 1);
	void DbQueryLog(DWORD sessionThreadID, PWS func, PWS uuid);

	ULONGLONG _maxFileSize;


	//파일도 직접 응답 하려 했는데, CppServer에서 GET으로 파일 요청 하면 내부처리된 바람에 이게 필요 없게 되었다.
	template<typename FNCRESP>
	void ResponseFile(PAS pfull, FNCRESP fnc)
	{
		try {
			CStringW sfile(pfull);
			CFile fl(sfile, CFile::modeRead);
			KAtEnd _fl([&]()->void {//동기 람다라 fl을 그대로 참조
				fl.Close();
			});
			auto flen = (int)fl.GetLength();
			if(flen <= _maxFileSize) //7메가
			{
				CStringA sbuf;
				auto fbuf = sbuf.GetBuffer(flen+1);// SHP<char>(new char[flen + 1]);
				*(fbuf + flen) = '\0';
				fl.Read(fbuf, flen);

				fnc(fbuf, flen);//res.MakeGetResponse(fbuf.get());
				sbuf.ReleaseBuffer();
			}
			else
				fnc(nullptr, flen);
		}
		catch(CException* e)
		{
			CString sbuf;
			LPTSTR p = sbuf.GetBuffer(512);
			e->GetErrorMessage(p, 512);
			TRACE("File (%s) %s. open error!", pfull, p);
		}
	}

	/// <summary>
	/// 원격함수명 vs 실제함수 맵
	/// </summary>
	//CRBMap0<CStringA, CFuncItem> m_mapRFncs; //FuncApi
	CKRbPtr<CStringA, CFuncItem> m_mapRFncs; //FuncApi
	//CRBMap0<CStringA, SHP<function<int(JObj&,JObj&,int)>>> m_mapRFncs; //FuncApi


	template<typename TFNC>//, typename TRAMBD>
	void AddApi(PAS sFnc, TFNC lambda, int iOp)
	{
		auto sfnc = SHP<FuncApi>(new FuncApi(lambda));//memory leak
		auto fapi = new CFuncItem(sfnc, iOp);
		m_mapRFncs.SetAt(sFnc, fapi);
	}
	/*
	template<typename FNCRESP>
	void AddApi1(PAS sFnc, FNCRESP lambda)
	{
		auto fnc = SHP<function<int(JObj&, JObj&, int)>>(new function<int(JObj&, JObj&, int)>(lambda));
		m_mapRFncs.SetAt(sFnc, fnc);
		//m_mapRFncs.SetAt(sFnc, std::make_shared<function<FNCRESP>>(lambda));
		//_server->_fncOnSentKw = SHP<function<int(HTTPSCacheSession*, uint8_t*, size_t)>>(new function<int(HTTPSCacheSession*, uint8_t*, size_t)>(fnc));
	}
	*/
	/// <summary>
	/// 원격함수명 vs 실제함수를 연결한다.
	/// 함수명이 같다는 전제에 매크로 이용 하며, 옵션도 포함.
	/// </summary>
	virtual bool MapRemoteFunctions();

	//?RemoteAPI 1:선언

	int FileOpen(string url, KBinary& fbuf);

	int ImageUpload(PAS data, size_t len, string orgname, PAS contType, PAS contLeng, JObj& jsr, 
		PAS tableSrc = nullptr, int idx = 0, PAS sOp = nullptr);// bool bTransaction = true);


	CString DevGuid(PWS title);






	int SampleApi1(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp);
	int SampleSelect(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp);
	int SampleInsert(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp);
	int SampleUpdate(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp);
	int SampleDelete(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp);

};

#define LogExcepFN(e) LogException(e, nullptr, __FUNCTIONW__, __LINE__)

#define AddLambdaApi(fnc, iOp1) AddApi(#fnc, [&](KDatabase& _db, JObj& jpa, JObj& jrs, int iOp) -> int {	return fnc(_db, jpa, jrs, iOp);	}, iOp1)
//예: AddLambdaApi(SampleSelect, 0);