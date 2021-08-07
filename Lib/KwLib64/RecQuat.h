#pragma once

#include "KJson.h"
#include "Kw_tool.h"
//              CJsonPbj          JSONValue
//               JObj              JVal
// class JObj;
// class JVal;

using namespace Kw;

/// template<typename JObj, typename JVal>
/// template class�� typedef�� ��ü �ߴ�.
/// header�� �Լ��� ������, ���� �۾��ϱ� ���ŷӴ�.
class QuatT
{
public:
// 	typedef class JObj JOBJ;
// 	typedef class JVal JVaL;
	//?usage
/*	Tss ss1;
	ss1 << L"SELECT 1 fOrder, 'holiday' fMode, NULL fBizID, NULL fUsrIdStf,  \n\
			TIMESTAMP(DATE(" << qs.s(sDay) << L"), '00:00:00') fOffBegin, \n\
			TIMESTAMP(DATE(" << qs.s(sNextDay) << L"), '00:00:00') fOffEnd, (case when fOff = 'off' then 0 ELSE 1 END) fOff, \n\
			'������/������' fNote, null fLat, null fLon \n\
			FROM tcalendar WHERE fDate = " << qs.s(sDay);
			*/
	~QuatT()
	{
		Reset();
	}
	/// <summary>
	/// SQL�� ���鶧 field�� ���� �ٶ� 'xxx' quatation string�� �˾Ƽ� type�� ���� �ٿ� �ش�.
	/// </summary>
	/// <param name="afterPoint">double float �϶� �Ҽ��� �Ʒ� �ڸ���.default�� 6�̴�.</param>
	/// <param name="SetMaxBufCount">���ڿ� ���� ���� �ִ� ����. �⺻ 5000���ε� �� Ű�ﶧ�� ��� �Ѵ�.</param>
	QuatT(int afterPoint = 6, int SetMaxBufCount = 5000)
		:_buf(SetMaxBufCount),
		_afterPoint(afterPoint)// �̰� ������ �Ʒ� { 9 }�� �ƿ� ���Ѵ�.
	{
		ASSERT(afterPoint >= 0);
	}
	CStrBufferT<CString, LPCTSTR> _buf;
	int _afterPoint{ 9 };//���� class�ʱ�ȭ�� �����Ƿ� �ƿ� ���Ѵ�.

	PWS Qs(PWS val, BOOL bQuat = TRUE, BOOL bNullIfEmpty = TRUE);
	PWS Qs(int k, BOOL bNullIfMinusOne = TRUE);
	PWS Qs(double k, BOOL bNullIfMinusOne = TRUE, int afterPoint = -1);

	CString _sql; // @val ó�� heidiSQL���� �ٷ� �����ؿ� ���ڿ�. ���� @val �� ��¥ ������ ��ü �Ѵ�
	CString _sqlv; // @val ó�� heidiSQL���� �ٷ� �����ؿ� ���ڿ�. ���� @val �� ��¥ ������ ��ü �Ѵ�

	bool _bApplied{ false };

	void SetSQL(PWS sql)
	{
		_sql = sql;
// 		_sql.Trim();
// 		_sql.TrimRight(';');
#ifdef _DEBUG
		Apply(); // ������ GetSQL �ؾ� Apply �ϴµ�
		_bApplied = true; // debug�߿� ���� SQL�� ���� �;
#else
		_bApplied = false;
#endif
	}


	PWS GetSQL()
	{
		if(!_bApplied)
			Apply(); // ����� _sqlv ��.
		return (PWS)_sqlv;
	}

	KStdMap<wstring, wstring> _fields;

	/// �ʼ�, ����, NULL
	bool Field(JObj& json, LPCSTR ka, BOOL bNecessary = FALSE, BOOL bQuat = TRUE, BOOL bNullIfEmpty = TRUE);

	bool Field(JObj& json, LPCSTR kget, LPCSTR kput, BOOL bNecessary = FALSE, BOOL bQuat = TRUE, BOOL bNullIfEmpty = TRUE);

	/// number type field that has not quatation
	bool FieldNum(JObj& json, LPCSTR kget, LPCSTR kput, BOOL bNecessary = FALSE, BOOL bNullIfEmpty = TRUE)
	{
		return Field(json, kget, kput, bNecessary, FALSE, bNullIfEmpty);
	}
	/// number type field that has not quatation
	bool FieldNum(JObj& json, LPCSTR ka, BOOL bNecessary = FALSE, BOOL bNullIfEmpty = TRUE)
	{
		return Field(json, ka, bNecessary, FALSE, bNullIfEmpty);
	}

	bool Field(JObj& json, LPCWSTR kw, BOOL bNecessary = FALSE, BOOL bQuat = TRUE, BOOL bNullIfEmpty = TRUE)
	{
		CStringA ka(kw);
		return Field(json, ka, bNecessary, bQuat, bNullIfEmpty);
	}

	size_t CopyField(QuatT& qt)//<JObj,JVal>
	{
		this->_fields = qt._fields;
		return _fields.size();
	}
	JObj* _json{ nullptr };

	template<typename TVal>//�ʼ�, ����, NULL
	bool Field(LPCWSTR kw, TVal v, BOOL bNecessary = FALSE, BOOL bQuat = TRUE, BOOL bNullIfEmpty = TRUE)
	{
		CStringA ka(kw);
		return Field((LPCSTR)ka, v, bNecessary, bQuat, bNullIfEmpty);
	}
	
	template<typename TVal>//�ʼ�, ����, NULL
	bool Field(LPCSTR ka, TVal v, BOOL bNecessary = FALSE, BOOL bQuat = TRUE, BOOL bNullIfEmpty = TRUE)
	{
		//auto uv = new CUbj();
		//if(bNecessary && v == nullptr)
		//	throw_field(ka);

		CUbj uv;
		//uv->Set(v);
		uv.Set(v); // v�� ���̸� �� �״�� ����.
		PWS psk = uv.GetTxtW();//StrBuffer�� �� ���� CString& �ƴϰ� & ��Ʈ�ȴ�.
		wstring sk = psk;
		if(bNecessary && sk.length() == 0)
			throw_field(ka);
		//return false;

		//_fields.SetAt(k, uv);
		//_fields[k] = uv;
		CString k(ka);
		_fields[(PWS)k] = Qs(sk.c_str(), bQuat, bNullIfEmpty);
		return true;
	}
	/// number type field that has not quatation
	template<typename TVal>//�ʼ�, ����, NULL
	bool FieldNum(LPCSTR ka, TVal v, BOOL bNecessary = FALSE, BOOL bNullIfEmpty = TRUE)
	{
		return Field(ka, v, bNecessary, FALSE, bNullIfEmpty);
	}

	/// sub sql in main sql query string.
	bool InsideSQL(LPCSTR ka, PWS v)
	{
		return Field(ka, v, FALSE, FALSE, FALSE);
	}

	CStringW _setval;

	/// @SetField is created as inside string theat update field. ex: field1='xxx', field2='yyy'
	void JsonToUpdateSetField(JObj& jpa, PWS keyField, PWS keyField2 = nullptr);


	/// update sql������ set �ڿ� f1 = v1, f2 = v2, f3 = v3 ���� �� �����.
	/// �ǵڿ��� , �� ����.
	/// _fields[L"SetField"]
	bool AddSetField(PWS kw, JVal& jsv, BOOL bLast = FALSE)
	{
		CString value;
		auto it = _fields.find((PWS)kw);
		if(it != _fields.end())
		{
			value = it->second.c_str();// ���⼭ 'xxx' quat �� ��������.
			CStringW sv;
			sv.Format(L"%s = %s", kw, (PWS)value);
			if(!bLast)
				sv += L", ";
			_setval += sv;
			if(bLast)
				_fields[L"SetField"] = _setval;
			// UPDATE table set @SetField where fBizID = @fBizID
			return true;
		}
		return false;
	}
	template<typename TVal>//�ʼ�, ����, NULL
	bool AddSetField(PWS kw, TVal v, BOOL bLast = FALSE, BOOL bQuat = TRUE, BOOL bNullIfEmpty = TRUE)
	{
		CStringW sv;
		sv.Format(L"%s = %s", kw, Qs(v, bQuat, bNullIfEmpty));
		if(!bLast)
			sv += L", ";

		_setval += sv;

		if(bLast)
		{
			_fields[L"SetField"] = _setval;
		}
	}


	void Reset()
	{
		//_fields.DeleteAll();.
		//KwClearMapPtrVal(_fields);
		_fields.clear();
		//_sql.Empty();
		_sqlv.Empty();
	}

	void AppendSQL(PWS psql, PWS finish = L"");


	/// <summary>
	/// �Ѳ����� ������ ������ϰ�, multi insert �Ҷ� �� record ����, Quat._fields �ȿ� ������ �Ź� �ٲ�Ƿ�
	/// �ٷ� �� ���ڵ常 ���� �Ͽ� _sqlv �� �ٿ� �ش�.
	/// </summary>
	/// <param name="psql">���� ��� �ִ� ���� sql. ��: INSERT �� ���� record (@fcol1, @fcol2,,) </param>
	/// <param name="finish">�ǵڿ� ���̴� ���. ',' �Ǵ� ';'</param>
	void AppendApply(PWS psql, PWS finish);
	void Apply();

	void Apply(CString& sql, CString& sqlv, KStdMap<wstring, wstring>& fields);
};

// ��ȣ�� �ɼ� ����: �ʼ�, ����, NULL�� ǥ��
#define qs_Field(qs, k) qs.Field(#k, k)
#define qs_Field1(qs, k, bNecessary) qs.Field(#k, k, bNecessary)
#define qs_Field2(qs, k, bNecessary,  bQuat) qs.Field(#k, k, bNecessary,  bQuat)
#define qs_Field3(qs, k, bNecessary,  bQuat, bNullIfEmpty) qs.Field(#k, k, bNecessary,  bQuat, bNullIfEmpty)

#define qs_FieldSub(qs, k) qs.Field(#k, k, 1,  0)

#define QEnd(qs) qs.Apply()
#define QSF(k) qs_Field(qs, k)




//typedef class QuatT<CJsonPbj, JSONValue> QuatJ;
typedef class QuatT Quat; //<Kw::JObj, Kw::JVal>
