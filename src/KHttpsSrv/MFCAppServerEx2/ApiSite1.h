#pragma once
#include "KwLib64/HttpError.h"
#include "ApiBase.h"

class CBiz;


class ApiSite1 :
    public CApiBase
{
public:
	/// ODBC�� ���� DB�� ���� �Ѵ�. ���⼭�� MariaDB�� �����ǿ��� �̸� ���� �� �־� �Ѵ�.
	virtual PAS CheckDB(PWS sDSN = nullptr);

	/// ����Ʈ�Լ��� �����Լ��� �����Ͽ� request������ �Լ������� �ٷ� �˻� �����ϰ� ���� �صд�.
	/// �⺻������ �����Լ���� �����Լ����� ����.
	bool MapRemoteFunctions() override;

	/// client App���� ������ ������ ��Ʈ�p ������ ������ ���� ���� �Ѵ�.
	int ClientException(JObj& jpa, JObj& jrs, int iOp);

	int SampleSelectUUID(JObj& jpa, JObj& jrs, int iOp);


	/// API�߰� �ϴ� ��
	// 1. AddLambdaApi(NewAPI, 0);
	// 2. �Լ����� : h��
	// 3. �Լ����� : cpp�� ��ü

	PWS getExLibName() override
	{
		return L"MFCLibrary1.dll";
	}
};


///
/// �Լ� �����͸� ��� �α� ����
//	
// ������ �Լ������� ����϶� ��: RemoteFunc(SampleSelectUUID, 0);
#define RemoteFunc(fnc, iOp) m_mapRFncs.SetAt(#fnc, CFuncItem(&ApiSite1::fnc, iOp))
// ���߿� ���� ��� ����� ApiBase��

//#define Rec(rs) KRecordset rs(&_db) moved to Recordset.h

