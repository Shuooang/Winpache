#include "pch.h"
#include "ApiSite1.h"

#include "KwLib64/KRect.h"
#include "KwLib64/TimeTool.h"
#include "KwLib64/inlinefnc.h"
#include "KwLib64/KDebug.h"
#include "KwLib64/tchtool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// 여기에 API에 쓰일 함수를 맵핑 한다.
bool ApiSite1::MapRemoteFunctions()
{
	if(__super::MapRemoteFunctions())
		return true;
	int iOp1 = 0;
	//AddApi("SampleSelectUUID", [&](JObj& jpa, JObj& jrs, int iOp) -> int {	return SampleSelectUUID(jpa, jrs, iOp);	}, iOp1);
	AddLambdaApi(SampleSelectUUID, 0);
	AddLambdaApi(SampleSelect, 0);
	AddLambdaApi(SampleInsert, 0);
	AddLambdaApi(SampleUpdate, 0);
	AddLambdaApi(SampleDelete, 0);

#if _DEBUG
//	_stackKeep[L"admin-0001"] = 1;
#endif
	return false;
}


PAS ApiSite1::CheckDB(PWS sDSN)
{
	// _ODBCDSN 에 값이 있으면 연결된다.
	if(tchlen(sDSN) > 0)
		return CApiBase::CheckDB(sDSN);
//Server Information: MariaDB 10.04.000012
//Connection String:
//DRIVER={MariaDB ODBC 3.1 Driver};TCPIP=1;SERVER=localhost;UID=odisou;PWD=********;DATABASE=odiso;PORT=3306
	//[v] Enable automatic reconnect
	//[v] Allow multille statements
		//"DSN=myDsn;DRIVER={[MariaDB ODBC 3.1 Driver]};UID=xxxx;PWD=********;DATABASE=kkkk;PORT=3306;SERVER=cccc.xxxxx.co.kr;TCPIP=1;";
		//이거는 제어판/관리도구/ODBC설정에서 단지 이름과 DB종류(MariaDB)만 등록 한 경우
	return NULL;
}



// DB 사용 예제
int ApiSite1::SampleSelectUUID(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		// 1. DB사용 초기화
		KRecordset rs(&_db);

		// 2. request parmameter 분석
		CString sql;
		CString limit = jpa.S("limit");
		if(limit.IsEmpty())
			throw_BadRequest(-1, "Field 'limit' is missing.");
//			limit = L"10";
		sql.Format(L"select * from t_uuid limit %s", (PWS)limit);

		// 3. SQL query실행
		BOOL bOpen = rs.OpenSelectFetch(sql);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.

		// 4. SQL Query결과를 JSON으로 리턴
		rs.MakeRecsetToJson(jrs, L"t_uuid");

		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}






int ApiSite1::ClientException(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		//AccessRight(jpa, iOp);
		Quat qs;
		//qs.Field(jpa, "fBizID", TRUE);//필수, 상품
		qs.Field(jpa, "fUsrID");
		qs.Field(jpa, "f02class");
		qs.Field(jpa, "f10Err");
		qs.Field(jpa, "fReturn");
		qs.Field(jpa, "f12ToString");
		qs.Field(jpa, "fSysInfo");
		qs.Field(jpa, "fStack");
		qs.Field(jpa, "fLine");

		qs.Field(jpa, "fObject");
		qs.Field(jpa, "fDebug");
		qs.Field(jpa, "fPakageApp");
		qs.Field(jpa, "fOsMachine");
		qs.Field(jpa, "fModel");
		qs.Field(jpa, "fComName");
		qs.Field(jpa, "fTimeOcurd");
		qs.Field(jpa, "fAppVer");
		qs.Field(jpa, "fMachine");
		qs.Field(jpa, "fNetStat");

		qs.SetSQL(L"INSERT INTO t_excepclient (\
fUsrID,   f02class,  f10Err,  fReturn,  f12ToString,  fSysInfo,  fStack,  fLine,  fObject,  fDebug,  fPakageApp,  fOsMachine,  fModel,  fComName,  fTimeOcurd,  fAppVer,  fMachine,  fNetStat) VALUES ( -- \n\
@fUsrID, @f02class, @f10Err, @fReturn, @f12ToString, @fSysInfo, @fStack, @fLine, @fObject, @fDebug, @fPakageApp, @fOsMachine, @fModel, @fComName, @fTimeOcurd, @fAppVer, @fMachine, @fNetStat);");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


