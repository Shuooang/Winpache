#include "pch.h"
#include "KJson.h"
#include "TimeTool.h"
#include "Kw_tool.h"
#include "KVal.h"

//#include "Json/Json.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace Kw
{

	/// <summary>
	/// 
	/// </summary>
	/// <param name="data">null terminated string</param>
	/// <returns></returns>
	ShJObj Json::ParseUtf8(const char* data)
	{
		CString requ;
		KwUTF8ToWchar(data, requ);
		ShJVal jdoc = Json::Parse((PWS)requ);
		if(jdoc->IsObject())
			return jdoc->AsObject();
		else
			return ShJObj();
	}
	ShJVal Json::Parse(const char* data)
	{
		auto shv = ShJVal();
		size_t length = strlen(data) + 1;
		wchar_t* w_data = (wchar_t*)malloc(length * sizeof(wchar_t));

#if defined(WIN32) && !defined(__GNUC__)
		size_t ret_value = 0;
		if(mbstowcs_s(&ret_value, w_data, length, data, length) != 0)
		{
			free(w_data);
			return shv;
		}
#elif defined(ANDROID)
		// mbstowcs seems to misbehave on android
		for(size_t i = 0; i < length; i++)
			w_data[i] = (wchar_t)data[i];
#else
		if(mbstowcs(w_data, data, length) == (size_t)-1)
		{
			free(w_data);
			return shv;
		}
#endif

		shv = Json::Parse(w_data);
		free(w_data);
		return shv;
	}


	ShJVal Json::Parse(const wchar_t* data)
	{
		auto shv = ShJVal();
		// Skip any preceding whitespace, end of data = no Json = fail
		if(!SkipWhitespace(&data))
			return shv;

		// We need the start of a value here now...
		shv = JVal::Parse(&data);
		
		if(!shv)
			return shv;//fail

		// Can be white space now and should be at the end of the string then...
		if(SkipWhitespace(&data))
			return shv;//fail

		shv->toString();
		// We're now at the end of the string
		return shv;
	}

	wstring Json::Stringify(const ShJVal value)
	{
		if(value != NULL)
			return value->Stringify();
		else
			return L"";
	}


	bool Json::SkipWhitespace(const wchar_t** data)
	{
		while(**data != 0 && (**data == L' ' || **data == L'\t' || **data == L'\r' || **data == L'\n'))
			(*data)++;

		return **data != 0;
	}


	bool Json::ExtractString(const wchar_t** data, JStr& str)
	{
		str = L"";

		while(**data != 0)
		{
			// Save the char so we can change it if need be
			wchar_t next_char = **data;

			// Escaping something?
			if(next_char == L'\\')
			{
				// Move over the escape char
				(*data)++;

				// Deal with the escaped char
				switch(**data)
				{
				case L'"': next_char = L'"'; break;
				case L'\\': next_char = L'\\'; break;
				case L'/': next_char = L'/'; break;
				case L'b': next_char = L'\b'; break;
				case L'f': next_char = L'\f'; break;
				case L'n': next_char = L'\n'; break;
				case L'r': next_char = L'\r'; break;
				case L't': next_char = L'\t'; break;
				case L'u':
				{
					// We need 5 chars (4 hex + the 'u') or its not valid
					if(!simplejson_wcsnlen(*data, 5))
						return false;

					// Deal with the chars
					next_char = 0;
					for(int i = 0; i < 4; i++)
					{
						// Do it first to move off the 'u' and leave us on the
						// final hex digit as we move on by one later on
						(*data)++;

						next_char <<= 4;

						// Parse the hex digit
						if(**data >= '0' && **data <= '9')
							next_char |= (**data - '0');
						else if(**data >= 'A' && **data <= 'F')
							next_char |= (10 + (**data - 'A'));
						else if(**data >= 'a' && **data <= 'f')
							next_char |= (10 + (**data - 'a'));
						else
						{
							// Invalid hex digit = invalid Json
							return false;
						}
					}
					break;
				}

				// By the spec, only the above cases are allowed
				default:
					return false;
				}
			}

			// End of the string?
			else if(next_char == L'"')
			{
				(*data)++;
				str.reserve(); // Remove unused capacity
				return true;
			}

			// Disallowed char?
			else if(next_char < L' ')// && next_char != L'\t')
			{
				// SPEC Violation: Allow tabs due to real world cases
				const static wchar_t* arOK = L"\t\r\n\x1e\x1f";
				//                              {RS) (US) 30, 31
				if(wcschr(arOK, next_char))
				{
					wchar_t wc = next_char;
				}
				else
					return false;
			}
			else
			{
			}

			// Add the next char
			str += next_char;

			// Move on
			(*data)++;
		}

		// If we're here, the string ended incorrectly
		return false;
	}

	double Json::ParseInt(const wchar_t** data)
	{
		double integer = 0;
		while(**data != 0 && **data >= '0' && **data <= '9')
			integer = integer * 10 + (*(*data)++ - '0');

		return integer;
	}


	double Json::ParseDecimal(const wchar_t** data)
	{
		double decimal = 0.0;
		double factor = 0.1;
		while(**data != 0 && **data >= '0' && **data <= '9')
		{
			int digit = (*(*data)++ - '0');
			decimal = decimal + digit * factor;
			factor *= 0.1;
		}
		return decimal;
	}


	JArr::~JArr()
	{
		//DeleteValues();
	}

	JObj::~JObj()
	{
	
	}

	void JObj::toString()
	{
#if _DEBUG
		_txt.Empty();
		CString s;
		for(auto& [k, v] : *this)
		{
			s.Format(L"%s: %s, ", k.c_str(), v->Str());
			_txt += s;
		}
		_aaa = (PWS)_txt;
#endif
	}

	void JVal::toString()
	{
#if _DEBUG
		_txt.Empty();
		std::wstring sts;
		JSonTextVal(sts, 50, true);
		_txt = sts.c_str();
		//_aaa = (PWS)_txt;
/*
		CString s;
		if(IsObject())
		{
			auto jobj = AsObject();
			_txt = L"(object)";// jobj->_txt;
			_aaa = jobj->_aaa;
		}
		else if(IsString())
		{
			auto& ws = AsString();
			_txt = ws.c_str();
			_aaa = (PWS)_txt;
		}
		else if(IsInt())
		{
			auto i = AsInt();
			wstring ws = std::to_wstring(i);
			_txt = ws.c_str();
		}
		else if(IsInt64())
		{
			auto i = AsInt64();
			wstring ws = std::to_wstring(i);
			_txt = ws.c_str();
		}
		else if(IsDouble())
		{
			auto i = AsDouble();
			wstring ws = std::to_wstring(i);
			_txt = ws.c_str();
		}
*/

#endif
	}

	void JObj::Set(PAS name, ShJVal val)
	{
		CStringW k(name);
		//wstring wk = (PWS)k;
		SetAt((PWS)k, val);// []�� ���� �ʰ� Set�� ��� toString�� ���� �ȴ�.
// 		(*this)[name] = val;
// 		toString();
		toString();
	}
	void JObj::SetObj(PWS name, ShJObj sjo, BOOL bClone)
	{
		ShJVal sjv = make_shared<JVal>(sjo, bClone);
		Set(name, sjv);
		//(*this)[name] = sjv;
	}
	void JObj::SetArray(PWS name, ShJArr sja, BOOL bClone)
	{
		ShJVal sjv = make_shared<JVal>(sja, bClone);
		Set(name, sjv);
	}

	bool JObj::DeleteKey(PWS name)
	{
		//JObj::const_iterator it = this->find(name);
		if(Has(name))//it == this->end())
		{
			this->erase(name);
			return true;
		}
		return false;
	}





	//JObj::JObj(const ShJObj jobj)
	//{
	//	if(jobj == NULL)
	//	{
	//		this = new JObj();
	//		_bOwner = TRUE;
	//	}
	//	else
	//	{
	//		this = (ShJObj)jobj;
	//		_bOwner = FALSE;
	//	}
	//}

	//void JObj::Import(const ShJObj src)
	//{
	//	JObj::CloneObject(src, *this);
	//}

	void JObj::Clone(const JObj& src, bool bClone)
	{
		//if(*src == nullptr)
		//	throw_str("JObj::Clone(JObj& jobj,) jobj is nullptr.");

		JObj::CloneObject(src, *this, bClone);
		//tar._bOwner = TRUE;
	}
	void JObj::CloneObject(const JObj& source, JObj& tar, bool bClone)
	{
			for(auto& [k, sjv] : source)
			{
				/// clone �ؾ� �ϹǷ� sjv �� ���� ���� �ʴ´�.
				///?����: ���⼭ JVal(JVal& jv) �� �ҷ����� ���� ������ JVal�� ���� ������.
				/// auto jv = *sjv;/// �Ѵ� ������ �ƴҶ��� �ش�
				//auto& jv = *sjv;/// �̷��� �ϸ� reference �� ���Ѵ�. �̶��� ���� �ϸ� �ȴ�.
				//auto jv2 = new JVal(jv, bClone);/// �̷��� �ϸ� pointer ���Ѵ�.
				//auto pval = ShJVal(new JVal(*sjv, bClone));//v�� cast���ϸ� �����Ѱ��̺ҷ�����.
				//tar[k] = ShJVal(new JVal(*sjv, bClone));;// v�� object �̰ų� array�� �Kī�� �ȴ�.
				tar.SetAt(k, make_shared<JVal>(*sjv, bClone));

				//tar[k] = make_shared<JVal>(*sjv, bClone); //ShJVal(new JVal(*sjv, bClone));;// v�� object �̰ų� array�� �Kī�� �ȴ�.
			}
			if(bClone)
			{
			}
			else
			{
				/// ���� bClone�̸� deep copy�� �ȴ�.
				/// std::map ��ü ��ɿ� �׸��� ������ ���� ������ �Kī�Ǵ� �ϴϴ�. 
				// (std::map<wstring, ShJVal>&)tar = source;
			}
	}

	ShJObj JArr::FindByValue(PAS field, PWS value)
	{
		//auto sja = _sjo->Array("table");
		//if(sja)
		{
			for(auto& sjv : *this)// ���Ͼ� ��ġ
			{
				if(sjv->IsObject())
				{
					auto sjoRow = sjv->AsObject();
					//ShJVal sjv;
					//if(sjoRow->Lookup(field, sjv))//�߰�
					//{
					//}
					if(sjoRow->SameS(field, value))//�߰�
						return sjoRow;

				}
			}
		}
		return nullptr;
	}

	void JArr::CloneArray(const JArr& src, JArr& tar, bool bClone)
	{
		if(bClone)
		{
			tar.clear();
			for(auto& sjv1 : src)
				tar.push_back(make_shared<JVal>(*sjv1, bClone));
		}
		else
			(std::vector<ShJVal>&)tar = src;
	}
	void JObj::ErrTest()
	{
		auto jnull = this->Obj(L"notexist");
		auto an1y = jnull->O("any");
	}
	wstring JObj::String(PWS k)
	{
		//?����: if((*this)[k]) �̰� ���� ���� ������� ������.
		if(Has(k)) //this->find(k) != this->end())
		{
			auto sjv = (*this)[k];
			if(sjv->IsString())
			{
				wstring val = sjv->AsString().c_str();
				return val;//tchsame(val, L"null") ? wstring.Empty() : val;
			}
		}
		return wstring();
	}

	/// JVal�� ���� �Ѵ�.
	void JObj::Copy(JObj& src, PAS tarF, PAS srcF)// = nullptr
	{
		BOOL bHas = CopyIf(src, tarF, srcF);
// 		if(!srcF)
// 			srcF = tarF;
// 		CStringW srf(srcF);
// 		CStringW taf(tarF);
// 
// 		if(src.Has(srf))//this->find(k) != this->end())
// 		{
// 			this->DeleteKey(taf);//Ÿ�ٿ� ������ ����
// 
// 			auto sjvS = src[(PWS)srf];
// 			auto sjvT = ShJVal(new JVal(sjvS, true));
// 			//this->Clone(sjv, true);
// 			(*this)[(PWS)taf] = sjvT;
// 		}
// 		else
		if(!bHas)
		{
			CStringA s; s.Format("JObj::Copy src(%s) field key Not found.", srcF);
			throw_str(s);
		}
	}

	/// src�� �ʵ尡 ������ ����
	BOOL JObj::CopyIf(JObj& src, PAS tarF, PAS srcF)// = nullptr
	{
		if(!srcF)
			srcF = tarF;
		CStringW srf(srcF);
		CStringW taf(tarF);

		if(src.Has(srf))//this->find(k) != this->end())
		{
			this->DeleteKey(taf);//Ÿ�ٿ� ������ ����
			auto sjvS = src[(PWS)srf];
			auto sjvT = ShJVal(new JVal(sjvS, true));
			//this->Clone(sjv, true);
			//(*this)[(PWS)taf] = sjvT;
			Set((PWS)taf, sjvT);
			return TRUE;
		}
		return FALSE;
	}
	ShJVal JObj::Get(PWS k)
	{
		ShJVal sjv;
		Lookup(k, sjv);//can be FALSE
		return sjv;
	}
	/// ���̰� �ְ� ������ ���� ��츸 true �̴�.
	int JObj::IsUpdated(JObj& src, JObj& tar, PAS tarF, PAS srcF)
	{
		if(!srcF)
			srcF = tarF;
		CStringW srf(srcF);
		CStringW taf(tarF);

		if(src.Has(srf))//this->find(k) != this->end())
		{
			auto sjvS = src[(PWS)srf];
			auto s = sjvS->AsString();
			if(tar.Has(srf))
			{
				auto sjvT = tar[(PWS)taf];
				auto t = sjvT->AsString();
				if(s.size() > 0)
				{
					if(t.size() > 0)
						return s == t ? 0 : 1;// ���� ����. : ���� �Ǿ���.
					else
						return -1; // ��������.
				}
				else
				{
					if(t.size() > 0)
						return 1;//���� �Ǿ���.
					else
						return -2;// zero length�� ����. -1�� ó�� no request, clear output
				}
			}
			else // no tar
			{
				return s.size() > 0 ? -1 : -2;// �ִٰ� ������ : ������ ��� ����.
			}
		}
		else // ���ٰ�
		{
			if(tar.Has(srf))
			{
				auto sjvT = tar[(PWS)taf];
				auto t = sjvT->AsString();
				return t.size() > 0 ? 1 : -2;//���ٰ� ���� : ������ ��� ����.
			}
			else
				return -2;
		}
	}

	int JObj::IsUpdated(ShJObj& src, ShJObj& tar, PAS tarF, PAS srcF)
	{
		if(src.get() && tar.get())
			return IsUpdated(*src, *tar, tarF, srcF);
		else
		{
			if(src.get())
			{
				JObj tarj;
				return IsUpdated(*src, tarj, tarF, srcF);
			}
			else
			{
				if(tar.get())
				{
					JObj srcj;
					return IsUpdated(srcj, *tar, tarF, srcF);
				}
				else
					return -2;
			}
		}
	}
	int JObj::IsUpdated(ShJObj& src, JObj& tar, PAS tarF, PAS srcF)
	{
		if(src.get())
			return IsUpdated(*src, tar, tarF, srcF);
		else
		{
			JObj srcj;
			return IsUpdated(srcj, tar, tarF, srcF);
		}
	}
	int JObj::IsUpdated(JObj& src, ShJObj& tar, PAS tarF, PAS srcF)
	{
		if(tar.get())
			return IsUpdated(src, *tar, tarF, srcF);
		else
		{
			JObj tarj;
			return IsUpdated(src, tarj, tarF, srcF);
		}
	}
	int JObj::IsUpdated(JObj& src, PAS tarF, PAS srcF)
	{
		return IsUpdated(src, *this, tarF, srcF);
	}




	PWS JObj::Ptr(PWS k)
	{
		///?����: if((*this)[k]) �̰� ���� ���� ������� ������.
		if(Has(k))//this->find(k) != this->end())
		{
			auto sjv = (*this)[k];
			return sjv->Ptr();
			//if(sjv->IsString())
			//{
			//	PWS val = sjv->AsString().c_str();
			//	/// IsString �ε� null�ϸ� ���ݾ�.
			//	return val;// tchsame(val, L"null") ? L"" : val;
			//}
			//else if(sjv->IsNull())
			//{
			//	return nullptr;
			//}
			//else
			//{
			//	ASSERT(0);//���ڿ� �϶��� ��û�ؾ� �Ѵ�. �ƴϸ� GetText �� ������.
			//}
		}
		return NULL;
	}


	PWS JVal::SN()
	{
		if(IsString())
			return string_.c_str();
		else if(IsNull())
			return NULL;
		throw_str("Not a string type.");
	}
	PWS JObj::SN(PAS k)
	{
		ShJVal sjv;
		CStringW kw(k);
		if(Lookup((PWS)kw, sjv))
			return sjv->SN();
		return NULL;
	}

	PWS JObj::S(PAS k, CStringW& sv)
	{
		CStringW kw(k);
		ShJVal sjv;
		if(Lookup((PWS)kw, sjv))
		{
			if(sjv->IsString())
			{
				sv = sjv->AsString().c_str();
				return (PWS)sv;
			}
		}
		return NULL;
	}
	
	PWS JObj::LenS(PAS k, CStringW& sv)
	{
		if(this != nullptr)
		{
			CStringW kw(k);
			ShJVal sjv;
			if(Lookup((PWS)kw, sjv))
			{
				if(sjv->IsString())
				{
					auto& ws = sjv->AsString();
					if(ws.length() > 0)
						sv = ws.c_str();
					return (PWS)sv;
				}
			}
		}
		return NULL;
	}
	size_t JObj::Length(PAS k)
	{
		ShJVal sjv;
		CStringW kw(k);
		if(Lookup((PWS)kw, sjv))
		{
			if(sjv->IsString())
			{
				wstring ws = sjv->AsString();
				return ws.size();
			}
			else
				throw_str("Not a string type for Len().");
		}
		return 0;
	}
	BOOL JObj::Len(PAS k)
	{
		return Length(k) > 0;
// 		ShJVal sjv;
// 		CStringW kw(k);
// 		if(Lookup((PWS)kw, sjv))
// 		{
// 			if(sjv->IsString())
// 			{
// 				wstring ws = sjv->AsString();
// 				return ws.size() > 0;
// 			}
// 			else
// 				throw_str("Not a string type for Len().");
// 		}
// 		return FALSE;
	}
	BOOL JObj::SameS(PAS k, PWS str)
	{
		if(this == nullptr)
			return str == nullptr;
		PWS sn = SN(k);
		return tchsame(sn, str);
	}

	BOOL JObj::BeginS(PAS k, PWS str)
	{
		PWS sn = SN(k);
		return tchbegin(sn, str);
	}

	BOOL JObj::Find(PAS k, PWS str)
	{
		PWS sn = SN(k);
		return tchstr(sn, str) != NULL;
	}

	BOOL JObj::Append(PAS k, PWS str)
	{
		CString sn = SN(k);
		BOOL b = !sn.IsEmpty();
		sn += str;
		(*this)(k) = sn;
		return b;
	}
	/// <summary>
	/// 
	/// </summary>
	/// <param name="k"></param>
	/// <param name="str"></param>
	/// <param name="tok">���� ���� '|' �Ǵ� ';' ��</param>
	/// <returns>��� ��� ������ TRUE </returns>
	BOOL JObj::OrStr(PAS k, PWS str, char tok)
	{
		if(!Find(k, str))
		{
			CString sapp;
			if(LenS(k, sapp))
				sapp += tok;
			sapp += str;
			(*this)(k) = sapp;
			return TRUE;// Append(k, sapp);
		}
		return FALSE;
	}


	PAS JVal::SA()/// const : _bufa ������ const�� ������.
	{
		CStringA& sbuf = _bufa.GetBuf();
		PWS sw = S();
		sbuf = CStringA(sw);
		return (PAS)sbuf;
	}
	PAS JObj::SA(PAS k)
	{
		CString kw = CString(k);
		ShJVal sjv;
		if(Lookup((PWS)kw, sjv))
			return sjv->SA();
		//if(Has(kw))//this->find(k) != this->end())
		//{
		//	auto sjv = (*this)[(PWS)kw];
		//	return sjv->SA();
		//}
		return NULL;
	}

	PWS JVal::S() const
	{
		if(IsString())
			return string_.c_str();
		else if(IsNull())
			return L"";
		throw_str("Not a string type.");
	}
	PWS JObj::S(PWS k)
	{
		ShJVal sjv;
		if(Lookup((PWS)k, sjv))
			return sjv->S();
		//if(Has(k))//this->find(k) != this->end())
		//{
		//	auto sjv = (*this)[k];
		//	return sjv->S();
		//}
		return L"";
		//PWS s = Ptr(k);
		//if(s == NULL)
		//	return L"";
		//else
		//	return s;
	}

	PWS JObj::Str(PAS k, int point)
	{
		CString& sbuf = _buf.GetBuf();
		ShJVal sjv;
		CStringW kw(k);
		if(Lookup((PWS)kw, sjv))
			return sjv->Str(point);
		return L"";
		//auto buf = sbuf.GetBuffer(lenBuf);
		//if(Has(k))//this->find(k) != this->end())
		//{
		//	CStringW kw(k);
		//	auto sjv = (*this)[(PWS)kw];
		//	return sjv->Str(point);
		//	//if(sjv->IsString())
		//	//{
		//	//	PWS val = sjv->AsString().c_str();
		//	//	return val;
		//	//}
		//	//else if(sjv->IsDouble())
		//	//{
		//	//	//CString fmt; fmt = L"%%f";
		//	//	double d = sjv->AsDouble();
		//	//	CString fmt; fmt.Format(L"%%.%df", point);
		//	//	sbuf.Format(fmt, (double)d);
		//	//	return sbuf;
		//	//}
		//	//else if(sjv->IsInt())
		//	//{
		//	//	auto d = sjv->AsInt();
		//	//	sbuf.Format(L"%d", (int)d);
		//	//	return sbuf;
		//	//}
		//	//else if(sjv->IsInt64())
		//	//{
		//	//	auto d = sjv->AsInt64();
		//	//	sbuf.Format(L"%I64d", (__int64)d);
		//	//	return sbuf;
		//	//}
		//	//else if(sjv->IsBool())
		//	//	return sjv->AsBool() ? L"1" : L"0";
		//	//else if(sjv->IsNull())
		//	//	return L"NULL";
		//	//else if(sjv->IsObject())
		//	//	return L"[obj]";//�̷��� SQL������ ������ ���� ����.
		//	//else if(sjv->IsArray())
		//	//	return L"[array]";
		//	//else
		//	//{
		//	//	ASSERT(0);
		//	//}
		//}
		//return L"NULL";
	}
	//CStringA& sbuf = _bufa.GetBuf();

	CTime JVal::T()
	{
		CString s = S();
		if(s.GetLength() > 3)
		{
			CTime t = KwStdTimeToTime(s);
			return t;
		}
		return CTime(0);
	}
	CTime JObj::T(PAS k)
	{
		CStringW kw(k);
		ShJVal sjv;
		if(Lookup((PWS)kw, sjv))
			return sjv->T();
		//auto sjv = (*this)[(PWS)k];
		//return sjv->T();
		return CTime();
	}


	int JVal::I(int def)
	{
		if(IsInt())
			return AsInt();
		return def;
	}
	int JObj::I(PWS k, int def)// = 0
	{
		ShJVal sjv;
		if(Lookup(k, sjv))
			return sjv->I();
		return def;
	}
	/// <summary>
	/// Quata string: SQL ���� �����ͷ� ���϶� '%s' ��� %s �� �ᵵ ' '�� �ٿ��ش�. ���� ��� NULL �� sql���� �°� �ش�.
	/// </summary>
	/// <param name="k"></param>
	/// <param name="bNullIfEmpty">���� ""��  </param>
	/// <returns></returns>
	PWS JObj::QS(PWS k, BOOL bNullIfEmpty, BOOL bQuat, BOOL bNecessary)
	{
		CString& sbuf = _buf.GetBuf();
		//auto buf = sbuf.GetBuffer(lenBuf);

		if(Has(k))//this->find(k) != this->end())
		{
			auto sjv = (*this)[k];
			if(sjv->IsString())
			{
				PWS val = sjv->AsString().c_str();
				int len = tchlen(val);
				if(bNecessary && len == 0)
				{
					return nullptr;
				}
				if(bNullIfEmpty && len == 0)
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
			else if(sjv->IsDouble())
			{
				//CString fmt; fmt = L"%%f";
				double d = sjv->AsDouble();
				sbuf.Format(L"%f", (double)d);
				return sbuf;
			}
			else if(sjv->IsInt())
			{
				//CString fmt; fmt = L"%%d";
				auto d = sjv->AsInt();
				sbuf.Format(L"%d", (int)d);
				return sbuf;
			}
			else if(sjv->IsInt64())
			{
				//CString fmt;fmt = L"%%I64d";
				auto d = sjv->AsInt64();
				sbuf.Format(L"%I64d", (__int64)d);
				return sbuf;
			}
			else if(sjv->IsBool())
				return sjv->AsBool() ? L"1" : L"0";
			else if(sjv->IsNull())
				return L"NULL";
			else if(sjv->IsObject())
				return L"[obj]";//�̷��� SQL������ ������ ���� ����.
			else if(sjv->IsArray())
				return L"[array]";
			else
			{
				ASSERT(0);
			}
		}
		else if(bNecessary)
		{
			return nullptr;
		}
		return L"NULL";
	}




	// SQL query���� ���� ���ڿ��� ���Ѵ�.
	PWS JObj::QN(PWS k, int underDot)
	{
		CString& sbuf = _buf.GetBuf();

		if(this == NULL)
			throw_str("JObj.this == NULL");
		//?����: if((*this)[k]) �̰� ���� ���� ������� ������.
		bool bDone = false;
		if(Has(k))
		{
			auto v = (*this)[k];
			if(v->IsDouble())
			{
				bDone = true;
				sbuf = v->GetJSonText().c_str();
			}
			else if(v->IsInt())
			{
				bDone = true;
				sbuf = v->GetJSonText().c_str();
			}
			else if(v->IsInt64())
			{
				bDone = true;
				sbuf = v->GetJSonText().c_str();
				//	fmt = L"%I64d";
			}
			else if(v->IsString())
			{
				bDone = true;
				sbuf = v->GetJSonText().c_str();
			}

			if(bDone)
				return (PWS)sbuf;

			// ���� ���ʹ� ��� ���ڿ��� ���� �ϹǷ� ���� sbuf�� �� �ʿ� ����.
			if(v->IsBool())// mySQL������ SQL���� boolean ���� 0, 1 �� ó�� �ǹǷ�
				return v->AsBool() ? L"1" : L"0";
			else if(v->IsNull())
				return L"NULL";
			else if(v->IsObject())
				return L"[obj]";//�̷��� SQL������ ������ ���� ����.
			else if(v->IsArray())
				return L"[array]";
			else
			{
				ASSERT(0);
			}
		}
		return L"NULL";
	}

	/// ���� �迭�� ���� ������ NULL
	ShJArr JObj::Array(PWS k)
	{
		ShJVal sjv;
		if(Lookup(k, sjv))
		{
			if(sjv->IsArray())
				return sjv->AsArray();
			else
				throw_str("IsArray() false.");
		}
		return NULL;
	}
	/// ���� �迭�� ������ ���� �� ����
	ShJArr JObj::AMake(PWS k)
	{
		ShJVal sjv;
		if(Lookup(k, sjv))
		{
			if(sjv->IsArray())
				return sjv->AsArray();// shared_ptr ���ΰ� �״�� ���� �ȴ�.
			else
				throw_str("IsObject() false.");
		}
		ShJArr sjo = make_shared<JArr>();
		SetArray(k, sjo, false);
		return sjo;
	}






	ShJObj JObj::OO(PAS k1, PAS k2)
	{
		CStringW kw1(k1);
		CStringW kw2(k2);
		ShJObj sjo1 = Obj(kw1);
		if(sjo1)
			return sjo1->Obj(kw2);
		return nullptr;
	}
	ShJArr JObj::OA(PAS k1, PAS k2)
	{
		CStringW kw1(k1);
		CStringW kw2(k2);
		ShJObj sjo1 = Obj(kw1);
		if(!sjo1)
			return sjo1->Array(kw2);
		return nullptr;
	}
	PWS JObj::OOS(PAS k1, PAS k2, PAS k3)
	{
		ShJObj sjo = OO(k1, k2);
		return sjo->S(k3);
	}

	/// ���� �Ǵ°� �ƴϰ�, ���� ��ü�� �״�� ����Ų���� �����ͷ� ���ϵȴ�.
	ShJObj JObj::Obj(PWS k)
	{
		/// �Ʒ� ->find���� unhandled�� ������ ���´�. �� unhandled
		//?����: if((*this)[k]) �̰� ���� ���� ������� ������.
		if(this == nullptr)
			throw_str("this == nullptr.");

		ShJVal sjv;
		if(Lookup(k, sjv))
		{
			if(sjv->IsObject())
				return sjv->AsObject();// shared_ptr ���ΰ� �״�� ���� �ȴ�.
			else
				throw_str("IsObject() false.");
		}
		return ShJObj();
	}
	/// make_shared<JObj>() �� ����� �־ ��ü�� �ִ´�.
	ShJObj JObj::OMake(PWS k)
	{
		ShJVal sjv;
		if(Lookup(k, sjv))
		{
			if(sjv->IsObject())
				return sjv->AsObject();// shared_ptr ���ΰ� �״�� ���� �ȴ�.
			else
				throw_str("IsObject() false.");
		}
		ShJObj sjo = make_shared<JObj>();
		SetObj(k, sjo, false);
		return sjo;
	}

	CStringW JVal::SLeft(int len)
	{
		wstring s = S();
		if(s.length() > len)
			s = s.substr(0, len);
		return CStringW(s.c_str());
	}

	CStringW JVal::SRight(int len)
	{
		wstring s = S();
		if(s.length() > len)
		{
			size_t ist = s.length() - len;
			s = s.substr(ist, len);
		}
		return CStringW(s.c_str());

	}

	CStringW JObj::SLeft(PWS k, int len)
	{
		ShJVal sjv;
		if(Lookup(k, sjv))
			return sjv->SLeft(len);
		return L"";
		//wstring s = String(k);
		//if(s.length() > len)
		//	s = s.substr(0, len);
		//return CStringW(s.c_str());
	}

	CStringW JObj::SRight(PWS k, int len)
	{
		ShJVal sjv;
		if(Lookup(k, sjv))
			return sjv->SRight(len);
		return L"";
		//wstring s = String(k);
		//if(s.length() > len)
		//{
		//	size_t ist = s.length() - len;
		//	s = s.substr(ist, len);
		//}
		//return CStringW(s.c_str());
	}

	double JVal::N()
	{
		if(this == NULL)
			throw (L"JObj.JVal == NULL");
		//?����: if((*this)[k]) �̰� ���� ���� ������� ������.
		auto sjv = this;
		if(sjv->IsDouble())
			return sjv->AsDouble();
		else if(sjv->IsInt())
			return (double)sjv->AsInt();
		else if(sjv->IsDouble())
			return (double)sjv->AsInt64();
		else if(sjv->IsString())
		{

			CStringW s = sjv->AsString().c_str();
			if(s.IsEmpty())
				return 0;

			return (double)KwAtoi((LPCWSTR)s);
		}
		return 0.;
	}

	double JObj::N(PWS k)
	{
		if(this == NULL)
			throw (L"JObj.this == NULL");
		//?����: if((*this)[k]) �̰� ���� ���� ������� ������.
		ShJVal sjv;
		if(Lookup(k, sjv))
		{
			return sjv->N();
			//if(sjv->IsDouble())
			//	return sjv->AsDouble();
			//else if(sjv->IsInt())
			//	return (double)sjv->AsInt();
			//else if(sjv->IsDouble())
			//	return (double)sjv->AsInt64();
			//else if(sjv->IsString())
			//{
			//	CStringW s = sjv->AsString().c_str();
			//	if(s.IsEmpty())
			//		return 0;
			//	return (double)KwAtoi((LPCWSTR)s);
			//}
		}
		return 0.;
	}
	__int64 JObj::I64(PWS k)
	{
		if(this == NULL)
			throw (L"JObj.this == NULL");
		ShJVal sjv;
		if(Lookup(k, sjv))
			return sjv->AsInt64();
		return 0;
	}


	bool JObj::IsArray(PWS k)
	{
		ShJVal sjv;
		return Lookup(k, sjv) ? sjv->IsArray() : false;
	}

	bool JObj::IsObject(PWS k)
	{
		ShJVal sjv;
		return Lookup(k, sjv) ? sjv->IsObject() : false;
		//return (this->find(k) != this->end() && (*this)[k]->IsObject());
	}

	bool JObj::IsString(PWS k)
	{
		ShJVal sjv;
		return Lookup(k, sjv) ? sjv->IsString() : false;
	}

	bool JObj::IsNumber(PWS k)
	{
		ShJVal sjv;
		return Lookup(k, sjv) ? sjv->IsNumber() : false;
	}
	bool JObj::IsDouble(PAS k)
	{
		CStringW wk(k);
		ShJVal sjv;
		return Lookup((PWS)wk, sjv) ? sjv->IsDouble() : false;
		//return (this->find((PWS)wk) != this->end() && (*this)[(PWS)wk]->IsDouble());
	}
	bool JObj::IsInt(PAS k)
	{
		CStringW wk(k);
		ShJVal sjv;
		return Lookup((PWS)wk, sjv) ? sjv->IsInt() : false;
		//return (this->find((PWS)wk) != this->end() && (*this)[(PWS)wk]->IsInt());
	}
	bool JObj::IsInt64(PAS k)
	{
		CStringW wk(k);
		ShJVal sjv;
		return Lookup((PWS)wk, sjv) ? sjv->IsInt64() : false;
		//return (this->find((PWS)wk) != this->end() && (*this)[(PWS)wk]->IsInt64());
	}
	bool JObj::IsNull(PAS k)
	{
		CStringW wk(k);
		ShJVal sjv;
		return Lookup((PWS)wk, sjv) ? sjv->IsNull() : false;
	}

	CStringW JObj::ToJsonStringW()
	{
		JVal jsv;
		/// ���⼭ JVal jsv(*jov) �ϰ� �Ʒ����� �������鼭 �ȵ� object�� ���� ������ ������ ��� ������ �����.
		jsv.ShareObj(*this);
		CStringW sutf8 = jsv.Stringify(false, true).c_str();
		return sutf8;
	}

	CStringA JObj::ToJsonStringUtf8()
	{
		CStringW sw = ToJsonStringW();
		CStringA sautf8;
		KwWcharToUTF8(sw, sautf8);
		return sautf8;
	}
	SHP<KBinary> JObj::ToJsonBinaryUtf8()
	{
		CStringW sw = ToJsonStringW();
		SHP<KBinary> sautf8 = make_shared<KBinary>();
		KwWcharToUTF8(sw, *sautf8);
		return sautf8;
	}

	KBinary JObj::ToJsonData()
	{
		CStringA jutf8 = this->ToJsonStringUtf8();
		KBinary bin;
		bin.SetPtr((PAS)jutf8, jutf8.GetLength());
		return bin;
	}

	bool JObj::CopyFielsIf(JObj& src, PWS key)
	{
		if(src.Has(key))
		{
			Set(key, src[key]);
			//(*this)[key] = src[key];
			return true;
		}
		return false;
	}

	int JObj::CopyFieldsAll(JObj& src)
	{
		int n = 0;
		//src.for_loop([&](JSonKey key, auto val) -> void
		for(auto& [key, val] : src)
		{
			Set(key.c_str(), ShJVal(new JVal(*val)));
			//(*this)[key.c_str()] = ShJVal(new JVal(*val));
			n++;
		}
		return n;
	}
	//void JObj::SetMove(PWS k, JObj& src)
	//{
	//	//src.get()->_bValueOwner = false; MoveObj���� �Ѵ�.
	//	auto jv = new JVal();
	//	jv->MoveObj(*src);
	//	(*this)[k] = jv;// new JVal(v);
	//}

	JArr::JArr(const JArr& jobj, bool bClone)
	{
		CloneArray(jobj, *this, bClone);
	}
	JArr::JArr(const ShJArr sja, bool bClone)
	{
		CloneArray(*sja, *this, bClone);
		//*this = *sja;
	}








	///?error �̰� bool�� ��� �����ϸ�, �׳� ����� �ϴ� �ȴ�. ���� ���̸� bool type���� ��� �� ���� �ִ�.
	void JUnit::operator=(const char* v)
	{
		ShJVal sjv = v ? make_shared<JVal>(v) : sjv = make_shared<JVal>();
		m_pCJobj->Set(m_k, sjv);
	}

	void JUnit::operator=(const CStringW& v)
	{
		operator=((PWS)v);
	}
	void JUnit::operator=(const CStringA& v)
	{
		CStringW vw(v);
		operator=((PWS)vw);
	}
	void JUnit::operator=(const wchar_t* v)
	{
		ShJVal sjv = v ? make_shared<JVal>(v) : sjv = make_shared<JVal>();
		m_pCJobj->Set(m_k, sjv);
	}
	//error C2593: 'operator ='��(��) ��ȣ�մϴ�.
	void JUnit::operator=(__int64 v)
	{
		ShJVal sjv = make_shared<JVal>(v);
		m_pCJobj->Set(m_k, sjv);//ShJVal(new JVal((__int64)v)));
	}
	void JUnit::operator=(int v)
	{
		ShJVal sjv = make_shared<JVal>(v);
		m_pCJobj->Set(m_k, sjv);//ShJVal(new JVal((int)v)));
		//(*m_pCJobj)[m_k] = ShJVal(new JVal((int)v));
	}
	void JUnit::operator=(unsigned __int64 v)
	{
		ShJVal sjv = make_shared<JVal>(v);
		m_pCJobj->Set(m_k, sjv);//ShJVal(new JVal((unsigned __int64)v)));
	}
	void JUnit::operator=(unsigned int v)
	{
		ShJVal sjv = make_shared<JVal>(v);
		m_pCJobj->Set(m_k, sjv);//ShJVal(new JVal((unsigned int)v)));
	}

	void JUnit::operator=(CTime v)
	{
		CStringW s;
		KwCTimeToString(v, s);
		operator=((PWS)s);
	}

	void JUnit::operator=(double v)
	{
		ShJVal sjv = make_shared<JVal>(v);
		m_pCJobj->Set(m_k, sjv);//ShJVal(new JVal(v)));
		//(*m_pCJobj)[m_k] = ShJVal(new JVal(v));
	}
	
	/// Sh??? �� �ٶ��� �׳� share������
	void JUnit::operator=(ShJObj sv)
	{
		ShJVal sjv = make_shared<JVal>(sv);
		m_pCJobj->Set(m_k, sjv);//ShJVal(new JVal(sv)));
		//(*m_pCJobj)[m_k] = ShJVal(new JVal(sv));
	}
	void JUnit::operator=(ShJArr sv)
	{
		ShJVal sjv = make_shared<JVal>(sv);
		m_pCJobj->Set(m_k, sjv);// ShJVal(new JVal(sv)));
		//(*m_pCJobj)[m_k] = ShJVal(new JVal(sv));
	}
	void JUnit::operator=(ShJVal sv)
	{
		ShJVal sjv = make_shared<JVal>(sv);
		m_pCJobj->Set(m_k, sjv);//ShJVal(new JVal(sv)));
		//(*m_pCJobj)[m_k] = sv;// ShJVal(new JVal(sv));
	}
	void JUnit::operator=(JVal& jv)
	{
		ShJVal sjv = make_shared<JVal>(jv);
		m_pCJobj->Set(m_k, sjv);//ShJVal(new JVal(jv)));
		//(*m_pCJobj)[m_k] = ShJVal(new JVal(jv));
	}

	/// �Ʒ� ó�� ��ü�� ���� �ִ� ���� clone�Ѵ�.
	void JUnit::operator=(JObj& v)
	{
		ShJVal sjv = make_shared<JVal>(v);
		m_pCJobj->Set(m_k, sjv);//ShJVal(new JVal(v, true)));
		//(*m_pCJobj)[m_k] = ShJVal(new JVal(v, true)); // v�� ���� ���� �̸� clone���� ������ ���ÿ��� ���� ����.
	}
	void JUnit::operator=(JArr& v)
	{
		ShJVal sjv = make_shared<JVal>(v);
		m_pCJobj->Set(m_k, sjv);// ShJVal(new JVal(v, true)));
		//(*m_pCJobj)[m_k] = ShJVal(new JVal(v, true));
	}


	void JUnit::operator=(ONULL& v)
	{
		//auto jv = ShJVal(new JVal();
		m_pCJobj->Set(m_k, ShJVal(new JVal()));
		//(*m_pCJobj)[m_k] = ShJVal(new JVal());
	}















	/**
	 * Parses a Json encoded value to a JVal object
	 *
	 * @access protected
	 *
	 * @param wchar_t** data Pointer to a wchar_t* that contains the data
	 *
	 * @return JVal* Returns a pointer to a JVal object on success, NULL on error
	 */
	ShJVal JVal::Parse(const wchar_t** data)
	{
		try
		{
			// Is it a string?
			if(**data == '"')
			{
				JSonKey str;
				if(!Json::ExtractString(&(++(*data)), str))
				{
#ifdef _DEBUG
					auto v1 = data;
					auto v2 = *data;
					auto v3 = (*data) + 1;
					//auto v4 = &((*data) + 1);
					Json::ExtractString(&((*data)), str);
#endif
					//++(*data);
					throw (int)__LINE__;// return NULL;
				}
				else
				{
					//++(*data);
					return ShJVal(new JVal(str));
				}
			}
			else if((simplejson_wcsnlen(*data, 4) && _wcsnicmp(*data, L"true", 4) == 0) || 
					(simplejson_wcsnlen(*data, 5) && _wcsnicmp(*data, L"false", 5) == 0))
			{// Is it a boolean?
				bool value = _wcsnicmp(*data, L"true", 4) == 0;
				(*data) += value ? 4 : 5;
				return ShJVal(new JVal(value));
			}
			else if(simplejson_wcsnlen(*data, 4) && _wcsnicmp(*data, L"null", 4) == 0)
			{// Is it a null?
				(*data) += 4;
				return ShJVal(new JVal());
			}
			else if(**data == L'-' || (**data >= L'0' && **data <= L'9'))
			{// Is it a number?
				// Negative?
				bool neg = **data == L'-';
				if(neg)
					(*data)++;
				int len = tchlen(*data);

				bool bDot = false;
				bool bFloat = false;
				//bool bInt = false;
				//bool bLong = false;
				double number = 0.0;
				// Parse the whole part of the number - only if it wasn't 0
				if(**data == L'0')
					(*data)++;
				else if(**data >= L'1' && **data <= L'9')
					number = Json::ParseInt(data);
				else
					throw (int)__LINE__;// return NULL;

				// Could be a decimal now...
				if(**data == '.')
				{
					bFloat = true;
					bDot = true;
					(*data)++;

					// Not get any digits?
					if(!(**data >= L'0' && **data <= L'9'))
						throw (int)__LINE__;// return NULL;

					// Find the decimal and sort the decimal place out
					// Use ParseDecimal as ParseInt won't work with decimals less than 0.1
					// thanks to Javier Abadia for the report & fix
					double decimal = Json::ParseDecimal(data);

					// Save the number
					number += decimal;
				}

				// Could be an exponent now...
				if(**data == L'E' || **data == L'e')
				{
					bFloat = true;
					(*data)++;

					// Check signage of expo
					bool neg_expo = false;
					if(**data == L'-' || **data == L'+')
					{
						neg_expo = **data == L'-';
						(*data)++;
					}

					// Not get any digits?
					if(!(**data >= L'0' && **data <= L'9'))
						throw (int)__LINE__;// return NULL;

					// Sort the expo out
					double expo = Json::ParseInt(data);
					for(double i = 0.0; i < expo; i++)
						number = neg_expo ? (number / 10.0) : (number * 10.0);
				}

				// Was it neg?
				if(neg)
					number *= -1;
				if(!bFloat)
				{
					if(number <= 2147483648)//len <= 9)// 2,147,483,648
					{
						return ShJVal(new JVal((int)number));
					}
					else
					{
						return ShJVal(new JVal((__int64)number));
					}
				}
				return ShJVal(new JVal(number));
			}
			else if(**data == L'{')// An object?
			{
//#define CHECK_FREE(stErr) if(stErr) {FREE_OBJECT(object);throw (int)__LINE__;}// return NULL;}
#define CHECK_FREE(stErr) if(stErr) { sjo.reset(); throw (int)__LINE__;}// return NULL;}

				auto sjo = ShJObj(new JObj());//std::map<std::wstring, JVal*>
				//JStrArray array_key;
				(*data)++;
				while(**data != 0)
				{
					// Whitespace at the start?
					CHECK_FREE(!Json::SkipWhitespace(data));

					// Special case - empty object
					if(sjo->size() == 0 && **data == L'}')
					{
						(*data)++;
						auto jv = new JVal(sjo);
						//jv->MoveObj(object);//�ù��� ��� Move�Ѵ�.
						return ShJVal(jv);
						//return new JVal(object);
					}

					// We want a string now...
					JSonKey name;
					CHECK_FREE(!Json::ExtractString(&(++(*data)), name));
#ifdef _DEBUGx
					if(wcscmp((const wchar_t*)name.c_str(), L"Script") == 0)
					{
						auto key = name.c_str();
					}
#endif // _DEBUG
					// More whitespace?
					CHECK_FREE(!Json::SkipWhitespace(data));

					// Need a : now
					CHECK_FREE(*((*data)++) != L':');

					// More whitespace?
					CHECK_FREE(!Json::SkipWhitespace(data));

					// The value is here
					ShJVal value = Parse(data);
					CHECK_FREE (!value);

					// Add the name:value
					if(sjo->Has(name.c_str()))
						sjo->DeleteKey(name.c_str());

					sjo->SetAt(name.c_str(), value);////////////////////////////// insert ////////////////////////////////////////
					//auto key1 = value->GetKey();

					// More whitespace?
					CHECK_FREE(!Json::SkipWhitespace(data));

					// End of object?
					if(**data == L'}')
					{
						(*data)++;
						return ShJVal(new JVal(sjo));
					}

					// Want a , now
					CHECK_FREE(**data != L',');

					(*data)++;
				}
			}
			else if(**data == L'[')// An array?
			{
				auto array = ShJArr(new JArr());

				(*data)++;

//#define CHECK_ARRAY(bVal) if(bVal) {FREE_ARRAY(array);throw (int)__LINE__;}// return NULL;}
#define CHECK_ARRAY(bVal) if(bVal) {array->clear();throw (int)__LINE__;}// return NULL;}
				while(**data != 0)
				{
					// Whitespace at the start?
					CHECK_ARRAY(!Json::SkipWhitespace(data));

					// Special case - empty array
					if(array->size() == 0 && **data == L']')
					{
						(*data)++;
						auto jv = new JVal(array);
						//jv->MoveArray(array);
						return ShJVal(jv);
						//return new JVal(array);
					}

					// Get the value
					ShJVal sjv = Parse(data);
					CHECK_ARRAY(sjv == NULL);

					// Add the value
					array->push_back(sjv);

					// More whitespace?
					CHECK_ARRAY(!Json::SkipWhitespace(data));

					// End of array?
					if(**data == L']')
					{
						(*data)++;
						auto jv = new JVal(array);
						//jv->MoveArray(array);
						return ShJVal(jv);
						//return new JVal(array);
					}

					// Want a , now
					CHECK_ARRAY(**data != L',');

					(*data)++;
				}

			}
			else// Ran out of possibilites, it's bad!
			{
				throw (int)__LINE__;// return NULL;
			}
		}
		catch(int eline)//line)
		{
			TRACE("%d line JSON parsing error.\n", eline);
		}
		return ShJVal();//(int)__LINE__ cast�� �ؾ� �Ѵ�. 
	}

	JVal::JVal()
	{
		type = JsonType_Null;
		toString();
	}

	JVal::JVal(const wchar_t* char_value1)
	{
		type = JsonType_String;
		string_ = JSonKey(char_value1);
		toString();
	}
	JVal::JVal(const char* char_value1)
	{
		type = JsonType_String;
		CStringW wch(char_value1);
		string_ = JSonKey((PWS)wch);
		toString();
	}

	JVal::JVal(const JSonKey& string_value1)
	{
		type = JsonType_String;
		string_ = string_value1;
		toString();
	}

	JVal::JVal(bool mbool_value)
	{
		type = JsonType_Bool;
		int64_ = mbool_value ? 1 : 0;
		toString();
	}

	JVal::JVal(double value1)
	{
		type = JsonType_Double;
		double_ = value1;
		toString();
	}
	JVal::JVal(int value1)
	{
		type = JsonType_Int;
		int64_ = value1;
		toString();
	}
	JVal::JVal(__int64 value1)
	{
		type = JsonType_Int64;
		int64_ = value1;
		toString();
	}
	JVal::JVal(unsigned int value1)
	{
		ASSERT(value1 <= 0x7fffffff);
		type = JsonType_Int;
		int64_ = (__int64)value1;
		toString();
	}
	JVal::JVal(unsigned __int64 value1)
	{
		ASSERT(value1 <= 0x7fffffffffffffff);
		type = JsonType_Int64;
		int64_ = (__int64)value1;
		toString();
	}


	JVal::JVal(const ShJArr sja, bool bClone)
	{
		type = JsonType_Array;
		if(bClone)
		{
			InitArray();
			JArr::CloneArray(*sja, *array_, bClone);
		}
		else
			array_ = sja;
		toString();
	}

	/// object value�� clone�ȴ�.
	JVal::JVal(const ShJObj sjo, bool bClone)//, JStrArray& array_key1)
	{
		type = JsonType_Object;
		if(bClone)
		{
			InitObject();
			JObj::CloneObject(*sjo, *object_, bClone);
		}
		else
		{
			object_ = sjo;
		}
		toString();
	}

	JVal::JVal(JArr& ja, bool bClone)
	{
		type = JsonType_Array;
		if(bClone)
		{
			InitArray();
			JArr::CloneArray(ja, *array_, bClone);
		}
		else
			array_ = ShJArr(&ja, TNotFree());
		toString();
	}

	/// object value�� clone�ȴ�. fail
	/// constructor�� ���� object_value�� NULL����ü ��� ������ ���� �ʱ�ȭ�� �ȵǾ� �ִ�.
	JVal::JVal(JObj& jo, bool bClone)//, JStrArray& array_key1)
	{
		type = JsonType_Object;
		if(bClone)
		{
			InitObject();
			JObj::CloneObject(jo, *object_, bClone);
		}
		else
			object_ = ShJObj(&jo, TNotFree());// jo�� ���� �»��� ���ú��� �̸� �̷��� ���� �ȵ���.
		toString();
	}

	/// <summary>
	/// object�� array�϶��� bClone�� ������ �޴´�.
	/// </summary>
	/// <param name="msource"></param>
	/// <param name="bClone"></param>
	JVal::JVal(JVal& msource, bool bClone)
	{
		this->Clone(msource, bClone);
	}
	JVal::JVal(const ShJVal msource, bool bClone)
	{
		this->Clone(*msource, bClone);
	}













	/// this�� object value�� ���� ��ü�� ������ �����Ͽ� free���� �ʴ´�.
	void JVal::ShareObj(JObj& obj1)//, JStrArray& array_key1)
	{
		type = JsonType_Object;
		object_ = ShJObj(&obj1, TNotFree());
		/// ShJObj�� object_value�� ����� this�� ������� TNotFree�� �ҷ� ���� 
		/// reference�� ���̰ų� ���� ���� �ʴ´�. �ӽ÷� �ΰ� �ִٰ� ������ �������.
	}
	/*
	void JVal::WrappObj(JObj& obj1)//, JStrArray& array_key1)
	{
		type = JsonType_Object;

		object_value = ShJObj(&obj1, TNotFree());
	}

	/// �ٸ� object value�� �Ű� �ͼ� �����ǵ� ���� ����, �� ���� ��ü�� ������ �����ϸ� free���� �ʴ´�.
	void JVal::MoveObj(JObj& obj1)//, JStrArray& array_key1)
	{
		type = JsonType_Object;
		object_value = obj1;
		object_value._bValueOwner = true;// ������ �������� �Űܰ���.
		obj1._bValueOwner = false;// ������ �������� ����.JVal�� �����⸸ ����.
	}

	void JVal::MoveArray(ShJArr& arr1)//, JStrArray& array_key1)
	{
		type = JsonType_Array;
		array_value = arr1;
		array_value._bValueOwner = true;// ������ �������� �Űܰ���.
		arr1._bValueOwner = false;// ������ �������� ����.JVal�� �����⸸ ����.
	}
	*/


	JVal::~JVal()
	{
	}

	PWS JVal::Ptr() const
	{
		auto sjv = this;
		if(sjv->IsString())
		{
			PWS val = sjv->AsString().c_str();
			/// IsString �ε� null�ϸ� ���ݾ�.
			return val;// tchsame(val, L"null") ? L"" : val;
		}
		else if(sjv->IsNull())
		{
			return nullptr;
		}
		else
		{
			//ASSERT(0);//���ڿ� �϶��� ��û�ؾ� �Ѵ�. �ƴϸ� GetText �� ������.
			throw_str("Not a string type. Ptr()");
		}
//		return nullptr;
	}
	PWS JVal::Str(int point)
	{
		CString& sbuf = _buf.GetBuf();

		auto sjv = this;
		if(sjv->IsString())
		{
			PWS val = sjv->AsString().c_str();
			return val;
		}
		else if(sjv->IsDouble())
		{
			//CString fmt; fmt = L"%%f";
			double d = sjv->AsDouble();
			CString fmt; fmt.Format(L"%%.%df", point);
			sbuf.Format(fmt, (double)d);
			return sbuf;
		}
		else if(sjv->IsInt())
		{
			auto d = sjv->AsInt();
			sbuf.Format(L"%d", (int)d);
			return sbuf;
		}
		else if(sjv->IsInt64())
		{
			auto d = sjv->AsInt64();
			sbuf.Format(L"%I64d", (__int64)d);
			return sbuf;
		}
		else if(sjv->IsBool())
			return sjv->AsBool() ? L"1" : L"0";
		else if(sjv->IsNull())
			return L"NULL";
		else if(sjv->IsObject())
			return L"[obj]";//�̷��� SQL������ ������ ���� ����.
		else if(sjv->IsArray())
			return L"[array]";
		else
		{
			throw_str("Not a string type. JVal::Str()");
		}
	}

	/*bool JVal::IsNull() const
	{
		return type == JsonType_Null;
	}
	bool JVal::IsString() const
	{
		return type == JsonType_String;
	}
	bool JVal::IsBool() const
	{
		return type == JsonType_Bool;
	}
	bool JVal::IsNumber() const
	{
		return IsDouble() || IsInt() || IsInt64();// type == JsonType_Number;
	}
	bool JVal::IsDouble() const
	{
		return type == JsonType_Double;
	}
	bool JVal::IsInt64() const
	{
		return type == JsonType_Int64;
	}
	bool JVal::IsInt() const
	{
		return type == JsonType_Int;
	}
	bool JVal::IsArray() const
	{
		return type == JsonType_Array;
	}
	bool JVal::IsObject() const
	{
		return type == JsonType_Object;
	}
	const JSonKey& JVal::AsString() const
	{
		return string_value;
	}
	bool JVal::AsBool() const
	{
		return bool_value;
	}
	double JVal::AsDouble() const
	{
		return double_value;
	}
	int JVal::AsInt() const
	{
		return int_value;
	}
	__int64 JVal::AsInt64() const
	{
		return int64_value;
	}
	ShJArr JVal::AsArray()
	{
		return array_value;
	}
	ShJObj JVal::AsObject()
	{
		return object_value;
	}
	*/
	void JVal::Clone(const JVal& msource, bool bClone)
	{
		//parent = msource.parent;
		type = msource.type;

		switch(type)
		{
		case JsonType_String:
			string_ = msource.string_;
			break;
		case JsonType_Double:
			double_ = msource.double_;
			break;
		case JsonType_Bool:
		case JsonType_Int:
		case JsonType_Int64:
			int64_ = msource.int64_;
			break;
		case JsonType_Array:
		{
			if(bClone)
			{
				InitArray();
				JArr::CloneArray(*msource.array_, *array_, bClone);
			}
			else
				array_ = msource.array_;// ShJArr(&ja, TNotFree());
			break;
		}
		case JsonType_Object:
		{
			if(bClone)
			{
				InitObject();
				JObj::CloneObject(*msource.object_, *object_, bClone);
			}
			else
				object_ = msource.object_;
			break;
		}
		case JsonType_Null:		// Nothing to do.
			break;
		}
		toString();
	}

	std::size_t JVal::CountChildren() const
	{
		switch(type)
		{
		case JsonType_Array:
			return array_->size();
		case JsonType_Object:
			return object_->size();
		default:
			return 0;
		}
	}

	bool JVal::HasChild(std::size_t index) const
	{
		if(type == JsonType_Array)
		{
			return index < array_->size();
		}
		else
		{
			return false;
		}
	}

	ShJVal JVal::Child(std::size_t index)
	{
		if(index < array_->size())
		{
			return array_->GetAt((int)index);
			//return (*array_value)[index];
		}
		else
		{
			return NULL;
		}
	}
	bool JVal::HasChild(const wchar_t* name) const
	{
		return object_->Has(name);
	}
	ShJVal JVal::Child(const wchar_t* name)
	{
		ShJVal sjv;
		object_->Lookup(name, sjv);
		return sjv;
	}


	std::wstring JVal::Stringify(const bool bUnicode, bool const prettyprint, const wchar_t* key, IStrConvert2* pinf) const
	{
		size_t const indentDepth = prettyprint ? 1 : 0;
		return StringifyImpl(bUnicode, indentDepth, key, pinf);
	}

	std::wstring JVal::StringifyImpl(const bool bUnicode, size_t const indentDepth, const wchar_t* key, IStrConvert2* pinf) const
	{
		std::wstring ret_string;
		size_t const indentDepth1 = indentDepth ? indentDepth + 1 : 0;
		std::wstring const indentStr = Indent(indentDepth);
		std::wstring const indentStr1 = Indent(indentDepth1);

		switch(type)
		{
		case JsonType_Null:
			ret_string = L"null";
			break;

		case JsonType_String:
			ret_string = StringifyString(bUnicode, string_, key, pinf);
			break;

		case JsonType_Bool:
			ret_string = int64_ == 1 ? L"true" : L"false";
			break;

		case JsonType_Double:
		{
			if(isinf(double_) || isnan(double_))
				ret_string = L"null";
			else
			{
				std::wstringstream ss;
				ss.precision(15);
				ss << double_;
				ret_string = ss.str();
			}
			break;
		}

		case JsonType_Int64:
		{
			if(isinf((double)int64_) || isnan((double)int64_))
				ret_string = L"null";
			else
			{
				std::wstringstream ss;
				ss.precision(15);
				ss << int64_;
				ret_string = ss.str();
			}
			break;
		}

		case JsonType_Int:
		{
			if(isinf((double)int64_) || isnan((double)int64_))
				ret_string = L"null";
			else
			{
				std::wstringstream ss;
				ss.precision(15);
				ss << (int)int64_;
				ret_string = ss.str();
			}
			break;
		}

		case JsonType_Array:
		{
			ret_string = indentDepth ? L"[\n" + indentStr1 : L"[";
			//ShJArr::const_iterator iter = array_value.begin();
			WCHAR wbuf[20] = { 0, };
			//for(int i = 0; iter != array_value.end(); i++)
			int szArray = (int)array_->size();
			int i = 0;
			for(auto& sjv : *array_)
			{
				//_itow_s((int)i, wbuf, 19, 10); //KwItoaW(i, wbuf, 19)
				///?todo wbuf�� Ư�����ڿ� ���ؼ� ���Ƿ� ���� �Ϸ� �Ҷ� IStrConvert::CharToString �� override�� interface�� ���� �޾� �Ѵ�.
				/// �̰Ŵ� �Ŀ� ���� ������� �ٲ�� �ڴ�.
				ret_string += sjv->StringifyImpl(bUnicode, indentDepth1, wbuf, pinf);

				// Not at the end - add a separator		//if(++iter != array_value.end())
				if(i != (szArray - 1))//���������� �ƴϸ�
				{
					Tss ss1;
					if(!indentDepth)
						ss1 << L", ";
					else
						ss1 << L",\n" << indentStr1;
					ret_string += ss1.str();
				}//ret_string += indentDepth ? L", " : L",\n" + indentStr1;
				i++;
			}
			ret_string += indentDepth ? L"\n" + indentStr + L"]" : L"]";
			break;
		}

		case JsonType_Object:
		{
			ret_string = indentDepth ? L"{\n" + indentStr1 : L"{";

			int szObj = (int)object_->size();
			int i = 0;
			for(auto& [key, sjv] : *object_)
			{
				ret_string += StringifyString(bUnicode, key, NULL);
				ret_string += L":";
				ret_string += sjv->StringifyImpl(bUnicode, indentDepth1, key.c_str(), pinf);// bUseConvt ? pinf : NULL);

				if(i != (szObj - 1))//���������� �ƴϸ�
				{
					Tss ss1;
					if(!indentDepth)
						ss1 << L", ";
					else
						ss1 << L",\n" << indentStr1;
					ret_string += ss1.str();
					//ret_string += indentDepth ? L", " : L",\n\t";
				}
				i++;
			}

			ret_string += indentDepth ? L"\n" + indentStr + L"}" : L"}";
			break;
		}
		}
		return ret_string;
	}

	std::wstring JVal::StringifyString(const bool bUnicode, const std::wstring& str, const wchar_t* key, IStrConvert2* pinf)
	{
		std::wstring str_out = L"\"";

		std::wstring::const_iterator iter = str.begin();
		while(iter != str.end())
		{
			wchar_t chr = *iter;
			const wchar_t* prv = NULL;

			if(pinf)
			{
				//if (pinf->CheckIfCharConv(key))
				prv = pinf->CharToString(key, chr);
			}
			if(prv)
			{
				str_out += prv;
			}
			else
			{
				if(chr == L'"' ||  // " => \" ��
					chr == L'\\' || // \ => \\ ��
					chr == L'/')    // / => \/ �� Jsonǥ��(Ư�̻���)
				{
					str_out += L'\\';
					str_out += chr;
				}
				else if(chr == L'\b')
					str_out += L"\\b";
				else if(chr == L'\f')
					str_out += L"\\f";
				else if(chr == L'\n')
					str_out += L"\\n";
				else if(chr == L'\r')
					str_out += L"\\r";
				else if(chr == L'\t')
					str_out += L"\\t";
				//else if (chr < L' ')
				//	str_out += L" ";
				else if(chr < L' ' || chr > 126)
				{// space ���� ������ ����?
					if(bUnicode)
					{
						str_out += L"\\u";
						for(int i = 0; i < 4; i++)
						{
							int value = (chr >> 12) & 0xf;
							if(value >= 0 && value <= 9)
								str_out += (wchar_t)('0' + value);
							else if(value >= 10 && value <= 15)
								str_out += (wchar_t)('A' + (value - 10));
							chr <<= 4;
						}
					}
					else
						str_out += chr;
				}
				else
					str_out += chr;
			}
			iter++;
		}

		str_out += L"\"";
		return str_out;
	}

	/**
	 * Creates the indentation string for the depth given
	 *
	 * @access private
	 *
	 * @param size_t indent The prettyprint indentation depth (0 : no indentation)
	 *
	 * @return std::wstring Returns the string
	 */
	std::wstring JVal::Indent(size_t depth)
	{
		const size_t indent_step = 2;
		depth ? --depth : 0;
		std::wstring indentStr(depth * indent_step, ' ');
		return indentStr;
	}

	//void JVal::SetKey(std::wstring key)
	//{
	//	//_key = key;
	//}

	int JVal::setValue(ShJVal snd1)
	{
		int rv = 0;
		if(this->type == snd1->type)
		{
			if(snd1->IsString())
				this->setString(snd1->AsString().c_str());
			else if(snd1->IsDouble())
				this->setDouble(snd1->AsDouble());
			else if(snd1->IsBool())
				this->setBool(snd1->AsBool());
			else
				rv = -1;
		}
		else
			rv =  -2;
		toString();
		return rv;
	}

	double JVal::CompareValue(ShJVal snd1)
	{
		double rv = -1;// object �ΰ�� -1 ����
		if(this->type == snd1->type)
		{
			if(snd1->IsString())
				rv = wcscmp(snd1->AsString().c_str(), this->AsString().c_str());
			else if(snd1->IsDouble())
			{
				double drv = snd1->AsDouble() - this->AsDouble();
				rv = (drv == 0.) ? 0 : (drv > 0.) ? 1 : -1;
			}
			else if(snd1->IsBool())
				rv = (snd1->AsBool() ? 1 : 0) - (this->AsBool() ? 1 : 0);
		}
		return rv;
	}

	int JVal::IsSameValue(ShJVal snd1)
	{
		double drv = CompareValue(snd1);
		int rv = drv == 0. ? 1 : 0;//drv < 0 ? -1 : 1;// object �ΰ�� -1 ����
		//if (this->type == snd1->type)
		//{
		//	if (snd1->IsString())
		//		rv = this->AsString() == snd1->AsString() ? 1 : 0;
		//	else if (snd1->IsNumber())
		//		rv = this->AsNumber() == snd1->AsNumber() ? 1 : 0;
		//	else if (snd1->IsBool())
		//		rv = this->AsBool() == snd1->AsBool() ? 1 : 0;
		//}
		return rv;
	}

	std::wstring JVal::GetJSonText(int maxlen)
	{
		std::wstring sts;
		JSonTextVal(sts, 100);

		//wchar_t* buf = new wchar_t[1024];//[1024];
		//KAtEnd d_buf([&]() { delete buf; });
		//if(this->IsString())
		//{

		//	auto wstr = this->AsString();
		//	if(wstr.length() == 0)
		//		return sts;

		//	wchar_t* sbuf = new wchar_t[wstr.length() + 8];//[1024];
		//	KAtEnd d_sbuf([&]() { delete sbuf; });
		//	if(wstr.length() > maxlen)
		//	{
		//		wstr = wstr.substr(0, 30);
		//		swprintf_s(sbuf, 1024, L"\"%s...\"", wstr.c_str());
		//	}
		//	else
		//		swprintf_s(sbuf, 1024, L"\"%s\"", wstr.c_str());
		//	sts = sbuf;
		//	return sts;
		//}


		//if(this->IsDouble())
		//{
		//	auto dn = this->AsDouble();
		//	swprintf_s(buf, 1024, L"%f", dn);
		//}
		//else if(this->IsInt())
		//{
		//	auto dn = this->AsInt();
		//	swprintf_s(buf, 1024, L"%d", (int)dn);
		//}
		//else if(this->IsInt64())
		//{
		//	auto dn = this->AsInt64();
		//	swprintf_s(buf, 1024, L"%I64d", (__int64)dn);
		//}
		//else if(this->IsBool())
		//	swprintf_s(buf, 1024, L"%s", this->AsBool() ? L"true" : L"false");
		//else
		//	swprintf_s(buf, 1024, L"(unknown)");
		//sts = buf;
		return sts;
	}

	void JVal::JSonTextVal(std::wstring& sts, int maxlen, bool bNoQuat)
	{
		sts.clear();
		//std::wstring sts;
		if(this->IsString())
		{

			auto wstr = this->AsString();
			if(wstr.length() == 0)
			{
				//sts.clear(); �̹� �ߴ�.
				return;
			}

			//wchar_t* sbuf = new wchar_t[wstr.length() + 8];//[1024];
			//KAtEnd d_sbuf([&]() { DeleteMeSafe(sbuf); });
			wchar_t sbuf[1024] = { 0 };
			if(wstr.length() > maxlen)
			{
				wstr = wstr.substr(0, 30);
				if(bNoQuat)
					swprintf_s(sbuf, 1020, L"%s...", wstr.c_str());
				else
					swprintf_s(sbuf, 1020, L"\"%s...\"", wstr.c_str());
			}
			else
			{
				if(bNoQuat)
					swprintf_s(sbuf, 1020, L"%s", wstr.c_str());
				else
					swprintf_s(sbuf, 1020, L"\"%s\"", wstr.c_str());

			}
			sts = sbuf;
			return;
		}

		//auto sbuf = make_shared<wchar_t>(1024);
		CStringW sbuf;
		wchar_t* buf = sbuf.GetBuffer(1024);// new wchar_t[1024];//[1024];
		//wchar_t* buf = sbuf.get();// new wchar_t[1024];//[1024];
		//KAtEnd d_buf([&, buf]() { DeleteMeSafe(buf); });

		if(this->IsDouble())
		{
			auto dn = this->AsDouble();
			swprintf_s(buf, 1024, L"%f", dn);
		}
		else if(this->IsInt())
		{
			auto dn = this->AsInt();
			swprintf_s(buf, 1024, L"%d", (int)dn);
		}
		else if(this->IsInt64())
		{
			auto dn = this->AsInt64();
			swprintf_s(buf, 1024, L"%I64d", (__int64)dn);
		}
		else if(this->IsBool())
			swprintf_s(buf, 1024, L"%s", this->AsBool() ? L"true" : L"false");
		else if(this->IsNull())
		{
			swprintf_s(buf, 1024, L"(null)");
		}
		else if(this->IsArray())
		{
			swprintf_s(buf, 1024, L"(array)");
		}
		else if(this->IsObject())
		{
			swprintf_s(buf, 1024, L"(object)");
		}
		else
			swprintf_s(buf, 1024, L"(unknown)");
		sts = buf;
		sbuf.ReleaseBuffer();
		return;
	}

	void JVal::InitArray()
	{
		if(!array_)
			array_ = ShJArr(new JArr());//instance ������
		else
			array_->clear();
	}

	void JVal::InitObject()
	{
		if(!object_)
			object_ = ShJObj(new JObj());//instance ������
		else
			object_->clear();
	}

	void JVal::operator=(JVal& jv)
	{
		//_key = jv._key;
#ifdef _DEBUG
		_txt = jv._txt;
		//_aaa = (PWS)_txt;
#endif // _DEBUG
		//_uiData = jv._uiData;
		//parent = jv.parent;
		type = jv.type;
		string_ = jv.string_;
		int64_ = jv.int64_;
		double_ = jv.double_;
	}



	/// ���� �迭�� ������ �����Ͱ� �������� ��� 
	ShJObj JObj::GetArrayItem(PAS karr, int idx)
	{
		if(Has(karr))
		{
			if(IsArray(karr))
			{
				auto sjarr = Array(karr);
				if(sjarr->size() > idx)
				{
					auto sjv = sjarr->at(idx);
					if(sjv->IsObject())
					{
						return sjv->AsObject();
					}
				}
			}
		}
		return nullptr;
	}

	// k�� value�� � type ���� �𸣴ϱ� JVal�� �޴´�.
	ShJVal JObj::GetArrayItem(PAS karr, int idx, PAS k)
	{
		ShJVal sjv;
		ShJObj sjo = GetArrayItem(karr, idx);
		if(!sjo)
			return sjv;
		CString wk(k);
		if(sjo->Lookup((PWS)wk, sjv))//sjos->Has(k))
		{
			if(!sjv->IsNull())
				return sjv;
		}
		return sjv;
	}

	bool JObj::GetArrayItem(PAS karr, int idx, PAS k, CStringW& rval)
	{
		ShJVal sjv = GetArrayItem(karr, idx, k);
		if(!sjv)
			return false;
		rval = sjv->S();
		if(rval.GetLength() > 0)
			return true;
		return false;
	}

	bool JObj::GetArrayItem(PAS karr, int idx, PAS k, int& rval)
	{
		ShJVal sjv = GetArrayItem(karr, idx, k);
		if(!sjv)
			return false;
		if(sjv->IsInt())
		{
			rval = sjv->AsInt();
			return true;
		}
		else
			throw_str("Wrong return type. GetArrayItem");
		return false;
	}









	void JArr::Add(const wchar_t* v)
	{
		KArray<ShJVal>::Add(ShJVal(new JVal(v)));
	}
	void JArr::Add(double v)
	{
		KArray<ShJVal>::Add(ShJVal(new JVal(v)));
	}
	void JArr::Add(int v)
	{
		KArray<ShJVal>::Add(ShJVal(new JVal(v)));
	}
	//inline void Add(long v)
	//{
	//	KArray<ShJVal>::Add(ShJVal(new JVal(v)));
	//}
	void JArr::Add(bool v)
	{
		KArray<ShJVal>::Add(ShJVal(new JVal(v)));
	}

	/// �޸� ��ȿ����. �ڵ� ����
	void JArr::Add(JObj& v, bool bClone)
	{
		//auto jv = new JVal(v, bClone);//v�� ���� clone �ȴ�.
		ShJVal sjv = make_shared<JVal>(v, bClone);
		KArray<ShJVal>::Add(sjv);// ShJVal(jv));
	}

	/// �޸� ȿ����. �ڵ� ����
	void JArr::Add(ShJObj sv, bool bClone)
	{
		//		auto jv = new JVal(sv);// sv�ȿ� �ִ� JObj �� ��� share�ȴ�.
		ShJVal sjv = make_shared<JVal>(sv, bClone);
		__super::Add(sjv);
		//KArray<ShJVal>::Add
	}
	void JArr::Add(ShJVal sv, bool bClone)
	{
		ShJVal sjv = make_shared<JVal>(sv, bClone);
		__super::Add(sjv);
	}







};
