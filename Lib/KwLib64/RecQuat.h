#pragma once

#include "KJson.h"
#include "Kw_tool.h"
//              CJsonPbj          JSONValue
//               JObj              JVal
template<typename TJSON, typename TJVAL>
class QuatT
{
public:
	//?usage
/*	Tss ss1;
	ss1 << L"SELECT 1 fOrder, 'holiday' fMode, NULL fBizID, NULL fUsrIdStf,  \n\
			TIMESTAMP(DATE(" << qs.s(sDay) << L"), '00:00:00') fOffBegin, \n\
			TIMESTAMP(DATE(" << qs.s(sNextDay) << L"), '00:00:00') fOffEnd, (case when fOff = 'off' then 0 ELSE 1 END) fOff, \n\
			'공휴일/국경일' fNote, null fLat, null fLon \n\
			FROM tcalendar WHERE fDate = " << qs.s(sDay);
			*/
	~QuatT()
	{
		Reset();
	}
	/// <summary>
	/// SQL문 만들때 field에 값을 줄때 'xxx' quatation string을 알아서 type에 따라 붙여 준다.
	/// </summary>
	/// <param name="afterPoint">double float 일때 소수점 아래 자리수.default는 6이다.</param>
	/// <param name="SetMaxBufCount">문자열 버퍼 사용시 최대 갯수. 기본 5000개인데 더 키울때만 사용 한다.</param>
	QuatT(int afterPoint = 6, int SetMaxBufCount = 5000)
		:_buf(SetMaxBufCount),
		_afterPoint(afterPoint)// 이게 있으면 아래 { 9 }는 아예 안한다.
	{
		ASSERT(afterPoint >= 0);
	}
	CStrBufferT<CString, LPCTSTR> _buf;
	int _afterPoint{ 9 };//위에 class초기화가 있으므로 아예 안한다.

	PWS Qs(PWS val, BOOL bQuat = TRUE, BOOL bNullIfEmpty = TRUE)
	{
		CString& sbuf = _buf.GetBuf();
		if(val != nullptr)
		{
			if(bNullIfEmpty && tchlen(val) == 0)
				return L"NULL";
			else
			{
				if(bQuat)
					sbuf.Format(L"'%s'", val);
				else
					sbuf = val;
				return sbuf;
			}
		}

		if(bNullIfEmpty)
			return L"NULL";
		else
			return nullptr;
	}
	PWS Qs(int k, BOOL bNullIfMinusOne = TRUE)
	{
		CString& sbuf = _buf.GetBuf();
		CString fmt;
		//double d = k;// v->AsNumber();
		if(bNullIfMinusOne && k == -1)// tchlen(val) == 0)
			return L"NULL";

		fmt = L"%d";
		sbuf.Format(L"%d", k);
		return sbuf;
	}
	PWS Qs(double k, BOOL bNullIfMinusOne = TRUE, int afterPoint = -1)
	{
		CString& sbuf = _buf.GetBuf();
		CString fmt;
		//double d = k;// v->AsNumber();
		if(bNullIfMinusOne && k == -1.)// tchlen(val) == 0)
			return L"NULL";

		fmt.Format(L"%%.%df", afterPoint);
		sbuf.Format(fmt, k);
		return sbuf;
	}

	CString _sql; // @val 처럼 heidiSQL에서 바로 복사해온 문자열. 이제 @val 을 진짜 값으로 교체 한다
	CString _sqlv; // @val 처럼 heidiSQL에서 바로 복사해온 문자열. 이제 @val 을 진짜 값으로 교체 한다

	bool _bApplied{ false };
	void SetSQL(PWS sql)
	{
		_sql = sql;
		_sql.Trim();
		_sql.TrimRight(';');
#ifdef _DEBUG
		Apply(); // 원래는 GetSQL 해야 Apply 하는데
		_bApplied = true; // debug중에 최종 SQL이 보고 싶어서
#else
		_bApplied = false;
#endif
	}
	PWS GetSQL()
	{
		if(!_bApplied)
			Apply(); // 결과가 _sqlv 로.
		return (PWS)_sqlv;
	}

	//CKRbPtr<CStringA, CUbj> _fields;
	//std::map<string, CUbj*> _fields;
	std::map<wstring, wstring> _fields;
	//필수, 쿼터, NULL
	bool Field(TJSON& json, LPCSTR ka, BOOL bNecessary = FALSE, BOOL bQuat = TRUE, BOOL bNullIfEmpty = TRUE)
	{
		CString k(ka);
		PWS pval = json.QS(k, bNullIfEmpty, bQuat, bNecessary);//quat ''는 나중에 붙이게 FALSE
		if(bNecessary && pval == nullptr)
			throw_field(ka);
		//return false;
		_fields[(PWS)k] = pval;
		return true;
	}
	/// number type field that has not quatation
	bool FieldNum(TJSON& json, LPCSTR ka, BOOL bNecessary = FALSE, BOOL bNullIfEmpty = TRUE)
	{
		return Field(json, ka, bNecessary, FALSE, bNullIfEmpty);
	}

	bool Field(TJSON& json, LPCWSTR kw, BOOL bNecessary = FALSE, BOOL bQuat = TRUE, BOOL bNullIfEmpty = TRUE)
	{
		CStringA ka(kw);
		return Field(json, ka, bNecessary, bQuat, bNullIfEmpty);
	}

	size_t CopyField(QuatT<TJSON,TJVAL>& qt)
	{
		this->_fields = qt._fields;
		return _fields.size();
	}
	TJSON* _json{ nullptr };

	template<typename TVal>//필수, 쿼터, NULL
	bool Field(LPCWSTR kw, TVal v, BOOL bNecessary = FALSE, BOOL bQuat = TRUE, BOOL bNullIfEmpty = TRUE)
	{
		CStringA ka(kw);
		return Field((LPCSTR)ka, v, bNecessary, bQuat, bNullIfEmpty);
	}
	
	template<typename TVal>//필수, 쿼터, NULL
	bool Field(LPCSTR ka, TVal v, BOOL bNecessary = FALSE, BOOL bQuat = TRUE, BOOL bNullIfEmpty = TRUE)
	{
		//auto uv = new CUbj();
		//if(bNecessary && v == nullptr)
		//	throw_field(ka);

		CUbj uv;
		//uv->Set(v);
		uv.Set(v); // v가 널이면 널 그대로 들어간다.
		PWS psk = uv.GetTxtW();//StrBuffer를 쓴 것이 CString& 아니고 & 빠트렸다.
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
	template<typename TVal>//필수, 쿼터, NULL
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
	void JsonToUpdateSetField(TJSON& jpa, PWS keyField, PWS keyField2 = nullptr)
	{
		_setval.Empty();
		//jpa->for_loop([&](wstring key, JSONValue& jsv) -> void
		auto l = jpa.size();
		int nKey = 1;
		if(keyField2)
			nKey++;
		int n = 0;
		for(auto& [key, sjv] : jpa)
		{
			auto& jsv = *sjv;
			bool bKey = key == keyField || (keyField2 != nullptr && key == keyField2);// L"fBizID";
			if(!(jsv.IsArray() || jsv.IsObject()))
			{
				//여기에 서브 항목이 또 있을수 있으니 제외. 필드와 다른 항목은 Option:{} 이용
				this->Field(jpa, key.c_str(), bKey);//필수 체크
				if(!bKey) // where 에 쓰이는 것이 아니먄
					this->AddSetField(key.c_str(), jsv, n == (l - 1 - nKey));
			}
			n++;
		}//);
		//_fields[L"SetField"] = _setval;
	}
	/// update sql문에서 set 뒤에 f1 = v1, f2 = v2, f3 = v3 같이 다 만든다.
	/// 맨뒤에는 , 를 제거.
	/// _fields[L"SetField"]
	bool AddSetField(PWS kw, TJVAL& jsv, BOOL bLast = FALSE)
	{
		CString value;
		auto it = _fields.find((PWS)kw);
		if(it != _fields.end())
		{
			value = it->second.c_str();// 여기서 'xxx' quat 가 씌워진다.
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
	template<typename TVal>//필수, 쿼터, NULL
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
	void Apply()
	{
		_sqlv.Empty();
		_fields[L"NULL"] = L"NULL"; // 다 '' 싸는데 NULL 그야 말로 SET = COL1 = NULL 처럼 필드값이 삭제 된다. 행이 아니고.

		bool bAt = false;
		CString var;
		CString value;
		for(int i = 0; i < _sql.GetLength() + 1; i++)
		{
			WCHAR ch = i < _sql.GetLength() ? _sql.GetAt(i) : '\0';
			if(bAt)
			{
				if(KwIsAlNum(ch) || ch == '_')
				{
					var += ch;
				}
				else// 끝나고 '\0'도 여기로 온다.
				{
					CString value;
					//wstring val = _fields.find(var);
					auto it = _fields.find((PWS)var);
					if(it != _fields.end())
					{
						//value = Qs(it->second.c_str());// 여기서 'xxx' quat 가 씌워진다. (이미 씌워 져 있으므로)
						value = it->second.c_str();// 여기서 'xxx' quat 가 씌워져 있으므로 그래로 넣는다. NULL 에외
					}
					else
					{
						CString ser; ser.Format(L"SQL variable @%s is not found.(Apply error)", var);
						throw_str(ser);
					}

					_sqlv += value;
					_sqlv += ch;
					bAt = false;
					var.Empty();
				}
			}
			else
			{
				if(ch == '@')
					bAt = true;
				else
					_sqlv += ch;
			}

		}
		_bApplied = true;
	}
};

// 번호는 옵션 갯수: 필수, 쿼터, NULL로 표시
#define qs_Field(qs, k) qs.Field(#k, k)
#define qs_Field1(qs, k, bNecessary) qs.Field(#k, k, bNecessary)
#define qs_Field2(qs, k, bNecessary,  bQuat) qs.Field(#k, k, bNecessary,  bQuat)
#define qs_Field3(qs, k, bNecessary,  bQuat, bNullIfEmpty) qs.Field(#k, k, bNecessary,  bQuat, bNullIfEmpty)

#define qs_FieldSub(qs, k) qs.Field(#k, k, 1,  0)

#define QEnd(qs) qs.Apply()
#define QSF(k) qs_Field(qs, k)




//typedef class QuatT<CJsonPbj, JSONValue> QuatJ;
typedef class QuatT<Kw::JObj, Kw::JVal> Quat;
