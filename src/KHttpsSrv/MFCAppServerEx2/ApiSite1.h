#pragma once
#include "KwLib64/HttpError.h"
#include "ApiBase.h"

class CBiz;


class ApiSite1 :
    public CApiBase
{
public:
	/// ODBC를 통해 DB에 연결 한다. 여기서는 MariaDB를 제어판에서 미리 셋팅 해 둬야 한다.
	virtual PAS CheckDB(PWS sDSN = nullptr);

	/// 리모트함수와 로컬함수를 맵핑하여 request했을때 함수명으로 바로 검색 가능하게 매핑 해둔다.
	/// 기본적으로 로컬함수명과 원격함수명은 같다.
	bool MapRemoteFunctions() override;

	/// client App에서 오류가 났을때 네트웤 오류를 제외한 모든걸 보고 한다.
	int ClientException(JObj& jpa, JObj& jrs, int iOp);

	int SampleSelectUUID(JObj& jpa, JObj& jrs, int iOp);


	/// API추가 하는 법
	// 1. AddLambdaApi(NewAPI, 0);
	// 2. 함수선언 : h에
	// 3. 함수정의 : cpp에 몸체

	PWS getExLibName() override
	{
		return L"MFCLibrary1.dll";
	}
};


///
/// 함수 포인터를 담아 두기 위해
//	
// 이전에 함수포인터 방식일때 예: RemoteFunc(SampleSelectUUID, 0);
#define RemoteFunc(fnc, iOp) m_mapRFncs.SetAt(#fnc, CFuncItem(&ApiSite1::fnc, iOp))
// 나중에 람다 등록 방식은 ApiBase에

//#define Rec(rs) KRecordset rs(&_db) moved to Recordset.h

