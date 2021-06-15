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


// ���⿡ API�� ���� �Լ��� ���� �Ѵ�.
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
	// _ODBCDSN �� ���� ������ ����ȴ�.
	if(tchlen(sDSN) > 0)
		return CApiBase::CheckDB(sDSN);
//Server Information: MariaDB 10.04.000012
//Connection String:
//DRIVER={MariaDB ODBC 3.1 Driver};TCPIP=1;SERVER=localhost;UID=odisou;PWD=********;DATABASE=odiso;PORT=3306
	//[v] Enable automatic reconnect
	//[v] Allow multille statements
		//"DSN=myDsn;DRIVER={[MariaDB ODBC 3.1 Driver]};UID=xxxx;PWD=********;DATABASE=kkkk;PORT=3306;SERVER=cccc.xxxxx.co.kr;TCPIP=1;";
		//�̰Ŵ� ������/��������/ODBC�������� ���� �̸��� DB����(MariaDB)�� ��� �� ���
	return NULL;
}



// DB ��� ����
int ApiSite1::SampleSelectUUID(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		// 1. DB��� �ʱ�ȭ
		KRecordset rs(&_db);

		// 2. request parmameter �м�
		CString sql;
		CString limit = jpa.S("limit");
		if(limit.IsEmpty())
			throw_BadRequest(-1, "Field 'limit' is missing.");
//			limit = L"10";
		sql.Format(L"select * from t_uuid limit %s", (PWS)limit);

		// 3. SQL query����
		BOOL bOpen = rs.OpenSelectFetch(sql);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.

		// 4. SQL Query����� JSON���� ����
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
		//qs.Field(jpa, "fBizID", TRUE);//�ʼ�, ��ǰ
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


