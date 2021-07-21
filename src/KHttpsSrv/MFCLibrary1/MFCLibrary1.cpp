// MFCLibrary1.cpp : Defines the initialization routines for the DLL.
//
// version 1.0.1 
// ProductCode: {2A5561D0-DC9C-4D3C-BEDF-57EA6D80FE6B}
// UpgradeCode: {12F6DA17-2387-42A9-90C4-63FC13241552}
// ---------------------------
/// It is recommended that the ProductCode be changed if you change the version.
// version 1.0.2
// ProductCode: {82AB38C0-F4DC-4EF8-A60A-6FB91C29DE4B} ���� �ø��� ���� �̰� �ö� ����.
// UpgradeCode: {12F6DA17-2387-42A9-90C4-63FC13241552}

// version 1.0.3
// ProductCode: {AE39B507-031C-441A-AE85-CD7828BB93B4}
//              {82AB38C0-F4DC-4EF8-A60A-6FB91C29DE4B} ���� �ִ� ���� �ڵ带 �״�� ��ô�.
// ProductCode�� ������� ������ version�� �÷ȴ���...
// �ٸ� ������ ��ǰ�� �̹� ��ġ�Ǿ� �ֽ��ϴ�.�� ������ ��ġ�� ����� �� �����ϴ�.�� ��ǰ�� ���� ������ �����ϰų� �����Ϸ��� "������"���� "���α׷� �߰�/����"�� ����Ͻʽÿ�.
//		�̷� ������ ����.
// ��� ���ο� ������ ������ ��Ͽ��� ��� ���δ�. 1.0.0, 1.0.1, 1.0.2 ����. 
//		������ ProductCode�� �ٲ��� ������ ���� ������ ����, ���� �ڵ��� ���� �������� ������ �ȱ򸰴�.
// ���, ���� ������ ���̸� �����ϰ� �����, �ڵ���� �ٲٸ� ������ �ܿ� ���� �򸰴�.

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
	qs.Field(jpa, "fBizID", TRUE);//�ʼ�
	qs.SetSQL(L"SELECT -- \
fBizReq fBizID, fBizPath, fBizReq, fUsrIdCeo, fUsrIdAdm, fForm, fTel, fTel2, fTel3, fState, fStJoin, fAnimal, fShowPrice, fBegin, fEnd, fTitle, fSubTitle, fDesc, fAddr, fLat, fLon -- \n\
	FROM tbiz WHERE fBizReq = @fBizID;");
	rec.OpenSelectFetch(qs.GetSQL());//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
	if(rec.NoData())
		jrs("Return") = L"No Data";
	else
		rec.MakeRecsetToJson(jrs, L"tbiz");	// "torder2pet":[{obj},{obj},{},] }

	return 0;
}


int DEXPORT ExNewBusiness(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	// �Է� ��������, �����Ͻ��������� ä�� �ִ� �ʵ� 4��
	jpa("fBizID") = KwGetFormattedGuid(true, L"biz", 16);
	jpa("fState") = L"hide";
	jpa("fStJoin") = L"register";
	jpa("fMemo") = L"���� ��������� ��û";

	Quat qs;
	// �ʵ��Է�,JSON����, �ʵ�Ű, �ʼ�����, quatation����, ���� ������ null �� �Էµ�
	qs.Field(jpa, "fUsrIdReg", TRUE, TRUE, TRUE);
	//qs.Field(jpa, "fBizID", TRUE);//�ʼ�, 
	qs.Field(jpa, "fBizPath");//
	qs.Field(jpa, "fForm", TRUE);//�ʼ� shop
	qs.Field(jpa, "fTel", TRUE);//�ʼ�
	qs.Field(jpa, "fTel2");
	qs.Field(jpa, "fTel3");
	qs.Field(jpa, "fState", TRUE);//�ʼ� open
	qs.Field(jpa, "fStJoin");//�ʼ� data
	qs.Field(jpa, "fAnimal");//�ʼ� dog
	qs.Field(jpa, "fShowPrice");//�ʼ� show
	qs.Field(jpa, "fBegin");//�ʼ� 10:00:00
	qs.Field(jpa, "fEnd");//�ʼ� 18:00:00
	qs.Field(jpa, "fTitle", TRUE);//�ʼ�
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
	qs.JsonToUpdateSetField(jpa, L"fBizID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
	qs.SetSQL(L"UPDATE tbiz SET @SetField where fBizID = @fBizID;");
	_db.ExecuteSQL(qs);

	ExSelectBizToJson(_db, jpa, jrs);

	jrs("Return") = L"OK";
	return 0;
}


int DEXPORT ExRemoveBizClass(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	Quat qs;
	qs.Field(jpa, "fBizID", TRUE);//�ʼ�
	qs.Field(jpa, "fBIzClsCD", TRUE);//�ʼ�

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

/* ���Ȼ� ����
int DEXPORT ExCreateServerDatabase(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	// set default value
	jpa.SetIfNull("database", L"HttpSvr");
	jpa.SetIfNull("collate", L"utf16_unicode_ci");

	Quat qs;
	qs.Field(jpa, "database", TRUE, FALSE);//�ʼ�l, no quat
	qs.Field(jpa, "collate", TRUE);//�ʼ�
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

	BOOL bOpen = rs.OpenSelectFetch(sql);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.

	rs.MakeRecsetToJson(jrs, L"tuser");
	jrs("Return") = L"OK";
	return 0;
}

int DEXPORT ExSelectUserQS(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	KRecordset rs(&_db);

	// �������� �ʵΰ��� �˻� �Ѵ�.
	if(!jpa.Has("limit"))// http 400�� ������ -1 ���ϰ��� ���ڿ��� ���ÿ� ������.
		throw_BadRequest(-1, "Field 'limit' is missing.");

	/// Quat ��ü�� ��� �Ͽ� �ʵ�üũ�� ���ڿ��� ��� '...' �ڵ����� ����ǥ�� �ٴ´�.
	Quat qs;
	// �ڵ����� �ʵ� �˻� �Ѵ�. 
	qs.FieldNum(jpa, "limit", TRUE, FALSE);//TRUE:�ʼ�, FALSE:''����
	// �ʼ��� TRUE�� �ߴµ�, ���� ������ �ڵ����� throw �Ǿ� response �ȴ�.

	CString swh;
	if(jpa.Has("where"))
		swh.Format(L"where %s ", jpa.S("where"));
	qs.InsideSQL("where", swh);// ���̶� �ϴ� �־���� �Ѵ�.
	//HeidiSQL�� ȣȯ�� ���� \n\ �տ� '--'�� �ִ´�.
	qs.SetSQL(L"SELECT -- \n\
* from tuser @where limit @limit");

	BOOL bOpen = rs.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.

	rs.MakeRecsetToJson(jrs, L"tuser");
	jrs("Return") = L"OK";
	return 0;
}











}//extern "C" {
