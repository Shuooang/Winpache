#pragma once
#include "KwLib64/Kw_tool.h"
#include "KwLib64/Recordset.h"
#include "Response1.h"

class CApiBase;// ���� ���� ���ϋ��� �̷��� declare�� �ϰ�, �Ʒ� ���� define�Ѵ�.
/// <summary>
/// ���� �Լ� Member function pointer ������ �����Ѵ�.
/// - const�� �Ⱥ��������� ���� �غ��� ���� �Ȼ���Ƿ�
/// </summary>
//typedef int(CApiBase::* FuncApi)(JObj&, JObj&, int);
typedef function<int(KDatabase& _db, JObj&, JObj&, int)> FuncApi;
typedef SHP<FuncApi> SFuncApi;


/// <summary>
/// �����Լ� dic���� value��ġ�� �� type
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
	eFncApp = 1 << 0, // ������
	eFncBiz = 1 << 1, // PC�� ����
	eFncAdmin = 1 << 2, // PC�� ���۰����� ����
	eFncPolicy = 1 << 3,// �����Ҷ� ����� ��å�� �Բ� �Ѵ�.
	eFncContract = 1 << 4,// ������Ʈ�� ����� ������ ����.
	eFncAccess = 1 << 5,
	eFncMsg = 1 << 6,// ����ڿ��� �� ���۽� ������ �޽��� ������
	eFncHttpGet = 1 << 7, // HTTP GET �� ���� �Ѵ�.
};
//eFncNoDB = 1 << 7,        // DB�� ��� ���� ���� static�� ���� �Ҷ� CheckDB�� ���ص� ����.

/// <summary>
/// ��� api�� ��� �ִ� ��
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
	/// request �Է� jpa �Ķ���Ϳ� ���� site db�� ������, main db�� ������ ���Ѥ�.
	/// </summary>
	/// <param name="jpa">request �Է� jpa �Ķ����</param>
	/// <returns></returns>
// 	virtual KDatabase* getDB(JObj& jpa)
// 	{
// 		return &_db;
// 	}


	CResponse1* _resp;//��ȣ����
	CStringA _GUID;
	CString _fileKey;// doc���� ��ŸƮ �Ҷ� �ʱ�ȭ �Ǿ, DB�� �α� ���϶� ���δ�.
	CString _ODBCDSN, // DSN=DsnName;UID=id;PWD=pwd;database=dbname
		_ODBCDSNlog;
	CString _rootLocal;
	CString _uploadLocal{ L"c:\\svr\\upload" };
	CStringA _defFile{ "index.html" };

	CStringA _rootURL{ "/" };//�̰� ���߿� ���� �ȴ�.
	CStringA _ApiURL{ "/api" }; // api call �Ҷ� �������� �տ� �ٴ� dir �̰� �ָ� ���� �Լ�

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
	///�����庰�� ���� ���ؼ��� �����
	virtual string CheckDbThread(PWS sDSN, SHP<KDatabase>& db, BOOL bSetDbLog = TRUE);

	void InitDbLogProc(KDatabase* db);
	/// <summary>
	/// ���ο��� ���� �޽��� �� ������, outputâ ������ �������� �� ���ٸ� �θ���.
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

	// odiso���� MakeRecsetToJson2 ������ ����
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
					if(lmda(r, c, robj) == 0) // cell������ �־� ��� �Ѵ�. 0�̸� �ϰ͵� ����
					{
						CellToJson(rs, r, c, robj);// type�� ���� quato ����
					}
				}
				ar.push_back(new JSONValue(*robj.get()));
			}
			hsr(keyRecset) = ar;
		}CATCH_GE;
	}


	/// <summary>
	/// ��û ������ ����� �Բ� DB�� �״´�. ���۹��� JSON�����̴�.
	/// </summary>
	/// <param name="sWstr">CStringW ��û</param>
	/// <param name="rpost">CStringW ����</param>
	void LogRequest(CStringW method, CStringW ip, CStringW uuid, CStringW uparams, CStringW func, string requUtf8, string respUtf8, 
		ULONGLONG elapsed, int retnv, string UA_CPU, string UserAgent);
	//CCriticalSection _csLogReq;//'Packets out of order' ���� ���� ���ؽ��� ��ü�� ������ �ȳ���.
	CMutex _mxDbLog;
	/// <summary>
	/// ���� ������ DB�� ����Ѵ�.
	/// </summary>
	/// <param name="e"></param>
	void LogException(CException* e, PWS uuid = nullptr, PWS func = nullptr, int line = 0);
	//void LogException(CException* e, PWS func = nullptr, int line = 0)	{
	//	LogException(e, nullptr, func, line);
	//}
	//CCriticalSection _csLogExcep;
	//CMutex _mxLogExcep;



	/// db log�� �������� threadId : array<query,elapsed> �̷��� 3���� dic�� �ʿ�
	CKCriticalSection _csDbQuery;//���� �����尡 �̿� �ϹǷ�
	CKRbPtr<DWORD, KPtrArray<KQueryLog>> _dbQuery;
	void SvrMessageBeep(int ncount = 1);
	void DbQueryLog(DWORD sessionThreadID, PWS func, PWS uuid);

	ULONGLONG _maxFileSize;


	//���ϵ� ���� ���� �Ϸ� �ߴµ�, CppServer���� GET���� ���� ��û �ϸ� ����ó���� �ٶ��� �̰� �ʿ� ���� �Ǿ���.
	template<typename FNCRESP>
	void ResponseFile(PAS pfull, FNCRESP fnc)
	{
		try {
			CStringW sfile(pfull);
			CFile fl(sfile, CFile::modeRead);
			KAtEnd _fl([&]()->void {//���� ���ٶ� fl�� �״�� ����
				fl.Close();
			});
			auto flen = (int)fl.GetLength();
			if(flen <= _maxFileSize) //7�ް�
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
	/// �����Լ��� vs �����Լ� ��
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
	/// �����Լ��� vs �����Լ��� �����Ѵ�.
	/// �Լ����� ���ٴ� ������ ��ũ�� �̿� �ϸ�, �ɼǵ� ����.
	/// </summary>
	virtual bool MapRemoteFunctions();

	//?RemoteAPI 1:����

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
//��: AddLambdaApi(SampleSelect, 0);