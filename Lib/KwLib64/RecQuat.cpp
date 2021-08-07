#include "pch.h"

#include "KJson.h"
#include "RecQuat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


PWS QuatT::Qs(PWS val, BOOL bQuat /*= TRUE*/, BOOL bNullIfEmpty /*= TRUE*/)
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

PWS QuatT::Qs(int k, BOOL bNullIfMinusOne /*= TRUE*/)
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

PWS QuatT::Qs(double k, BOOL bNullIfMinusOne /*= TRUE*/, int afterPoint /*= -1*/)
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

bool QuatT::Field(JObj& json, LPCSTR ka, BOOL bNecessary /*= FALSE*/, BOOL bQuat /*= TRUE*/, BOOL bNullIfEmpty /*= TRUE*/)
{
	CString k(ka);
	PWS pval = json.QS(k, bNullIfEmpty, bQuat, bNecessary);//quat ''�� ���߿� ���̰� FALSE
	if(bNecessary && pval == nullptr)
		throw_field(ka);
	//return false;
	_fields[(PWS)k] = pval;
	return true;
}

bool QuatT::Field(JObj& json, LPCSTR kget, LPCSTR kput, BOOL bNecessary /*= FALSE*/, BOOL bQuat /*= TRUE*/, BOOL bNullIfEmpty /*= TRUE*/)
{
	CString k(kget);
	PWS pval = json.QS(k, bNullIfEmpty, bQuat, bNecessary);//quat ''�� ���߿� ���̰� FALSE
	if(bNecessary && pval == nullptr)
		throw_field(kget);
	CString kp(kput);
	_fields[(PWS)kp] = pval;
	return true;
}

void QuatT::JsonToUpdateSetField(JObj& jpa, PWS keyField, PWS keyField2 /*= nullptr*/)
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
			//���⿡ ���� �׸��� �� ������ ������ ����. �ʵ�� �ٸ� �׸��� Option:{} �̿�
			this->Field(jpa, key.c_str(), bKey);//�ʼ� üũ
			if(!bKey) // where �� ���̴� ���� �ƴϐ�
				this->AddSetField(key.c_str(), jsv, n == (l - 1 - nKey));
		}
		n++;
	}//);
	//_fields[L"SetField"] = _setval;
}

void QuatT::AppendSQL(PWS psql, PWS finish /*= L""*/)
{
	_sql += psql;
	_sql += finish;
	AppendApply(psql, finish); // ����� _sqlv ��.
}

void QuatT::AppendApply(PWS psql, PWS finish)
{
	CString sql(psql);
	CString sqlv;
	Apply(sql, sqlv, _fields);
	_sqlv += sqlv;
	_sqlv += finish;
	_bApplied = true;
}

void QuatT::Apply()
{
	Apply(_sql, _sqlv, _fields);
	_bApplied = true;
}

void QuatT::Apply(CString& sql, CString& sqlv, KStdMap<wstring, wstring>& fields)
{
	sql.TrimRight(L";"); //\t\r\n  �� ���� ���� ���� �ذ��� -- �� �ְ� �Ǿ� ���� ����.
	sqlv.Empty();
	/// ��� �ʵ尪�� ''�� ���δµ� �ʵ尪�� NULL�� �������  @NULL �� ���� set �ϸ� set field = NULL ó�� �ȴ�. ���� �ƴϰ�.
	if(!fields.Has(L"NULL"))
		fields[L"NULL"] = L"NULL";
	bool bAt = false;
	CString var;
	CString value;
	for(int i = 0; i < sql.GetLength() + 1; i++)
	{
		WCHAR ch = i < sql.GetLength() ? sql.GetAt(i) : '\0';
		if(bAt)
		{
			if(KwIsAlNum(ch) || ch == '_')
			{
				var += ch;
			}
			else// ������ '\0'�� ����� �´�.
			{
				CString value;
				//wstring val = _fields.find(var);
				auto it = fields.find((PWS)var);
				if(it != fields.end())
				{
					//value = Qs(it->second.c_str());// ���⼭ 'xxx' quat �� ��������. (�̹� ���� �� �����Ƿ�)
					value = it->second.c_str();// ���⼭ 'xxx' quat �� ������ �����Ƿ� �׷��� �ִ´�. NULL ����
				}
				else
				{
					CString ser; ser.Format(L"SQL variable @%s is not found.(Apply error)", (PWS)var);
					throw_str(ser);
				}

				sqlv += value;
				sqlv += ch;
				bAt = false;
				var.Empty();
			}
		}
		else
		{
			if(ch == '@')
				bAt = true;
			else
				sqlv += ch;
		}

	}
}
