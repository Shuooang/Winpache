// MFCLibrary1.cpp : Defines the initialization routines for the DLL.
//
// version 1.0.1 
// ProductCode: {2A5561D0-DC9C-4D3C-BEDF-57EA6D80FE6B}
// UpgradeCode: {12F6DA17-2387-42A9-90C4-63FC13241552}
// ---------------------------
/// It is recommended that the ProductCode be changed if you change the version.
// version 1.0.2
// ProductCode: {82AB38C0-F4DC-4EF8-A60A-6FB91C29DE4B} 버전 올릴때 마다 이게 올라 간다.
// UpgradeCode: {12F6DA17-2387-42A9-90C4-63FC13241552}

// version 1.0.3
// ProductCode: {AE39B507-031C-441A-AE85-CD7828BB93B4}
//              {82AB38C0-F4DC-4EF8-A60A-6FB91C29DE4B} 위에 있는 이전 코드를 그대로 써봤다.
// ProductCode가 변경되지 않으면 version을 올렸더라도...
// 다른 버전의 제품이 이미 설치되어 있습니다.이 버전의 설치를 계속할 수 없습니다.이 제품의 현재 버전을 구성하거나 제거하려면 "제어판"에서 "프로그램 추가/제거"를 사용하십시오.
//		이런 오류가 난다.
// 대신 새로운 버젼은 제어판 목록에는 계속 쌓인다. 1.0.0, 1.0.1, 1.0.2 까지. 
//		하지만 ProductCode를 바꾸지 않으면 위의 오류가 나서, 같은 코드의 것을 제거하지 않으면 안깔린다.
// 결론, 따라서 버전만 높이면 제거하게 만들고, 코드까지 바꾸면 이전것 외에 것을 깔린다.

#include "pch.h"
#include "framework.h"
#include "MFCLibrary1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CMFCLibrary1App

BEGIN_MESSAGE_MAP(CMFCLibrary1App, CWinApp)
END_MESSAGE_MAP()


// CMFCLibrary1App construction

CMFCLibrary1App::CMFCLibrary1App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMFCLibrary1App object

CMFCLibrary1App theApp;


// CMFCLibrary1App initialization

BOOL CMFCLibrary1App::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}



//extern "C" BOOL PASCAL EXPORT ExportedFunction2(HANDLE v1, HANDLE v2, HANDLE v3)
extern "C" {



int DEXPORT ExportedFunction2(HANDLE v1, HANDLE v2, HANDLE v3)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// normal function body here
	//TRACE("%d, %d, %d\n", (int)v1, (int)v2, (int)v3);
	return 0;
}


//#define Rec(rs) KRecordset rs(&_db)

int DEXPORT ExThrowTest(KDatabase& _db, JObj & jpa, JObj & jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// normal function body here
	throw_str("throw from DLL function ExThrowTest.");
	return 0;
}

int DEXPORT WhatIsThis(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	jrs("SayHello") = L"Hello World!. Call the function ExGetApiDesc for test.";
	return 0;
}
int DEXPORT ExGetValuesTest(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// normal function body here
	JObj subObj;
	subObj("StringValue") = L"Call the function ExGetApiDesc";
	subObj("IntegerValue") = 1000;
	subObj("FloatValue") = -1.234567;

	jrs("valueCount") = 3;
	jrs("valueGrp1") = subObj;
	return 0;
}

int DEXPORT ExGetArrayOfObject(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	JArr ar;

	JObj subObj1;
	subObj1("StringValue") = L"String Value1";
	subObj1("IntegerValue") = 1000;
	subObj1("FloatValue") = -1.234567;
	ar.Add(subObj1);

	JObj subObj2;
	subObj1("StringValue") = L"Call the function ExGetApiDesc.";
	subObj1("IntegerValue") = 1002;
	subObj1("FloatValue") = -2.234567;
	ar.Add(subObj2);

	jrs("arraySample") = ar;
	jrs("itemCount") = ar.size();

	return 0;
}

LPCWSTR arFnc[][2] = {
	{L"ExThrowTest",L"Test throw error."},
	{L"ExGetValuesTest",L"Test API returning string."},
	{L"ExGetArrayOfObject",L"Test API returning array of object type."},
	{L"ExGetApiList",L"Get the list of API functions."},
	{L"ExGetApiDesc",L"Get the description of API functions."},
	{L"ExNewBusiness",L"Sample for inserting record."},
	{L"ExUpdateBusiness",L"Sample for updating record."},
	{L"ExRemoveBizClass",L"Sample for deleting record."},
	{L"ExSelectUser",L"Test for accesing DB table 'tuser'."},
//	{L"ExSelectUser",L"Sample that parameter is used by legacy way."},
	{L"ExSelectUserQS",L"Sample that parameter is used by Quat class."},
//	{L"ExCreateServerLogTable",L"Create Request, Error Log Table."},
	{L"ExCreateServerSampleTable",L"Create sample Table."},
//	{L"ExCreateServerDatabase",L"Create first databse for Winpache."},
	
};

int DEXPORT ExGetApiList(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	JArr jar;

	for(int i = 0; i < _countof(arFnc); i++)
		jar.Add(arFnc[i][0]);

	jrs("ApiList") = jar;
	jrs("itemCount") = jar.size();

	return 0;
}
int DEXPORT ExGetApiDesc(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	JObj jo1;

	for(int i = 0; i < _countof(arFnc); i++)
	{
		jo1(arFnc[i][0]) = arFnc[i][1];
	}

	jrs("ApiDesc") = jo1;
	jrs("itemCount") = jo1.size();

	return 0;
}


int DEXPORT ExSelectBizToJson(KDatabase& _db, JObj& jpa, JObj& jrs)
{
	Rec(rec);
	//KRecordset Rec(&_db);
	Quat qs;
	qs.Field(jpa, "fBizID", TRUE);//필수
	qs.SetSQL(L"SELECT -- \
fBizReq fBizID, fBizPath, fBizReq, fUsrIdCeo, fUsrIdAdm, fForm, fTel, fTel2, fTel3, fState, fStJoin, fAnimal, fShowPrice, fBegin, fEnd, fTitle, fSubTitle, fDesc, fAddr, fLat, fLon -- \n\
	FROM tbiz WHERE fBizReq = @fBizID;");
	rec.OpenSelectFetch(qs.GetSQL());//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
	if(rec.NoData())
		jrs("Return") = L"No Data";
	else
		rec.MakeRecsetToJson(jrs, L"tbiz");	// "torder2pet":[{obj},{obj},{},] }

	return 0;
}


int DEXPORT ExNewBusiness(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	// 입력 안했지만, 비지니스로직으로 채워 주는 필드 4개
	jpa("fBizID") = KwGetFormattedGuid(true, L"biz", 16);
	jpa("fState") = L"hide";
	jpa("fStJoin") = L"register";
	jpa("fMemo") = L"최초 사업가입자 신청";

	Quat qs;
	// 필드입력,JSON에서, 필드키, 필수여부, quatation여부, 값이 없으면 null 이 입력됨
	qs.Field(jpa, "fUsrIdReg", TRUE, TRUE, TRUE);
	//qs.Field(jpa, "fBizID", TRUE);//필수, 
	qs.Field(jpa, "fBizPath");//
	qs.Field(jpa, "fForm", TRUE);//필수 shop
	qs.Field(jpa, "fTel", TRUE);//필수
	qs.Field(jpa, "fTel2");
	qs.Field(jpa, "fTel3");
	qs.Field(jpa, "fState", TRUE);//필수 open
	qs.Field(jpa, "fStJoin");//필수 data
	qs.Field(jpa, "fAnimal");//필수 dog
	qs.Field(jpa, "fShowPrice");//필수 show
	qs.Field(jpa, "fBegin");//필수 10:00:00
	qs.Field(jpa, "fEnd");//필수 18:00:00
	qs.Field(jpa, "fTitle", TRUE);//필수
	qs.Field(jpa, "fDesc");
	qs.Field(jpa, "fMemo");
	qs.Field(jpa, "fAddr");

	qs.SetSQL(L"\
INSERT INTO tbiz( -- \n\
fBizID, fBizPath, fBizReq, fUsrIdReg, fUsrIdCeo, fUsrIdAdm, fForm, fTel, fTel2, fTel3, fState, fStJoin, fAnimal, fShowPrice, fBegin, fEnd, fTitle, fSubTitle, fDesc, fMemo, fAddr, fLat, fLon, fUsrIdUpdate)-- \n\
VALUES(NULL, @fBizPath, @fBizID, @fUsrIdReg, NULL, @fUsrIdReg, @fForm, @fTel, @fTel2, @fTel3, @fState, @fStJoin, @fAnimal, @fShowPrice, @fBegin, @fEnd, @fTitle, @fSubTitle, @fDesc, @fMemo, @fAddr, @fLat, @fLon, @fUsrIdReg);\
");
	_db.ExecuteSQL(qs);
	ExSelectBizToJson(_db, jpa, jrs);

	jrs("Return") = L"OK";
	return 0;
}


int DEXPORT ExUpdateBusiness(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	Quat qs;
	qs.JsonToUpdateSetField(jpa, L"fBizID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
	qs.SetSQL(L"UPDATE tbiz SET @SetField where fBizID = @fBizID;");
	_db.ExecuteSQL(qs);

	ExSelectBizToJson(_db, jpa, jrs);

	jrs("Return") = L"OK";
	return 0;
}


int DEXPORT ExRemoveBizClass(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	Quat qs;
	qs.Field(jpa, "fBizID", TRUE);//필수
	qs.Field(jpa, "fBIzClsCD", TRUE);//필수

	qs.SetSQL(L"DELETE FROM tbizclass WHERE fBizID = @fBizID and fBIzClsCD = @fBIzClsCD;");

	_db.ExecuteSQL(qs);
	jrs("Return") = L"OK";
	return 0;
}

/*L"\
CREATE TABLE `t_ktrace2` ( -- \n\
	`fStr` MEDIUMTEXT NULL DEFAULT NULL COLLATE 'utf16_unicode_ci', -- \n\
	`fPid` INT(10) NULL DEFAULT NULL -- \n\
) -- \n\
COMMENT='KTraceErr' -- \n\
COLLATE='utf16_unicode_ci' -- \n\
ENGINE=MyISAM -- \n\
AUTO_INCREMENT=1024"
*/

/* 보안상 제거
int DEXPORT ExCreateServerDatabase(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	// set default value
	jpa.SetIfNull("database", L"HttpSvr");
	jpa.SetIfNull("collate", L"utf16_unicode_ci");

	Quat qs;
	qs.Field(jpa, "database", TRUE, FALSE);//필수l, no quat
	qs.Field(jpa, "collate", TRUE);//필수
	qs.SetSQL(L"CREATE DATABASE `@database` COLLATE @collate");

	//L"CREATE DATABASE `HttpSvr` COLLATE 'utf16_unicode_ci'");
	_db.ExecuteSQL(qs);

	JArr jtbl;
	jtbl.Add(L"HttpSvr");

	jrs("tablesDatabase") = jtbl;
	jrs("Return") = L"OK";
	return 0;
}


int DEXPORT ExCreateServerLogTable(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	KDatabase::CreateTable(_db, "..\\..\\CREATE_TABLE_t_reqlog.sql");
	KDatabase::CreateTable(_db, "..\\..\\CREATE_TABLE_t_excepsvr.sql");

	JArr jtbl;
	jtbl.Add(L"t_reqlog");
	jtbl.Add(L"t_excepsvr");

	jrs("tablesCreated") = jtbl;
	jrs("Return") = L"OK";
	return 0;
}


int DEXPORT ExCreateServerSampleTable(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	KDatabase::CreateTable(_db, "..\\..\\CREATE_TABLE_tbiz.sql");
	KDatabase::CreateTable(_db, "..\\..\\CREATE_TABLE_tbizclass.sql");

	JArr jtbl;
	jtbl.Add(L"tbiz");
	jtbl.Add(L"tbizclass");

	jrs("tablesCreated") = jtbl;
	jrs("Return") = L"OK";
	return 0;
}
*/




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
