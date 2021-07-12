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
