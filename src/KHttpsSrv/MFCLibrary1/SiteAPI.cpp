#include "pch.h"
#include "framework.h"
#include "MFCLibrary1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern "C" {



int DEXPORT ExSelectUser(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	KRecordset rs(&_db);
		
	if(!jpa.Has("limit"))
		throw_BadRequest(-1, "Field 'limit' is missing.");
	int limit = jpa.I("limit");	//			limit = 10;
		
	CString swh;
	if(jpa.Has("where"))
		swh.Format(L"where %s ", jpa.S("where"));

	CString sql;
	sql.Format(L"select * from tuser %s limit %d", swh, limit);

	BOOL bOpen = rs.OpenSelectFetch(sql);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.

	rs.MakeRecsetToJson(jrs, L"tuser");
	jrs("Return") = L"OK";
	return 0;
}

int DEXPORT ExSelectUserQS(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	KRecordset rs(&_db);

	// 수동으로 필두값을 검사 한다.
	if(!jpa.Has("limit"))// http 400번 오류와 -1 리턴값과 문자열을 동시에 보낸다.
		throw_BadRequest(-1, "Field 'limit' is missing.");

	/// Quat 객체를 사용 하여 필드체크와 문자열인 경우 '...' 자동으로 따옴표도 붙는다.
	Quat qs;
	// 자동으로 필드 검사 한다. 
	qs.FieldNum(jpa, "limit", TRUE, FALSE);//TRUE:필수, FALSE:''없이
	// 필수를 TRUE로 했는데, 값이 없으면 자동으로 throw 되어 response 된다.

	CString swh;
	if(jpa.Has("where"))
		swh.Format(L"where %s ", jpa.S("where"));
	qs.InsideSQL("where", swh);// 빈값이라도 일단 넣어줘야 한다.
	//HeidiSQL과 호환을 위해 \n\ 앞에 '--'를 넣는다.
	qs.SetSQL(L"SELECT -- \n\
* from tuser @where limit @limit");

	BOOL bOpen = rs.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.

	rs.MakeRecsetToJson(jrs, L"tuser");
	jrs("Return") = L"OK";
	return 0;
}














}//extern "C" {
