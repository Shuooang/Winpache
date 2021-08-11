#pragma once
#include "stdecl.h"
#include "ktypedef.h"
#include "tchtool.h"
#include "KBinary.h"
#include "StrBuffer.h"
#include "KVal.h"


namespace Kw
{

	typedef std::wstring JSonKey;
	typedef std::wstring JStr;
	typedef std::vector<std::wstring> JStrArray;

	enum JsonType
	{
		JsonType_Null,
		JsonType_String,
		JsonType_Bool,
		JsonType_Double, //JsonType_Number,
		JsonType_Array,
		JsonType_Object,

		JsonType_Int,
		JsonType_Int64,
	};

	class JVal;
	class JObj;
	class JArr;

	typedef shared_ptr<JVal> ShJVal;
	typedef shared_ptr<JObj> ShJObj;
	typedef shared_ptr<JArr> ShJArr;



	// Simple function to check a string 's' has at least 'n' characters
	inline bool simplejson_wcsnlen(PWS s, size_t n) 
	{
		if(s == 0)
			return false;
		PWS save = s;
		while(n-- > 0)
		{
			if(*(save++) == 0) 
				return false;
		}
		return true;
	}


	class JUnit
	{
	public:
		JObj* m_pCJobj;
		CString m_sKey;
		PWS m_k;
		JUnit(JObj* th, PWS k)
			: m_pCJobj(th)//, m_k(k)
		{
			m_sKey = k;
			m_k = (PWS)m_sKey;
		}
		void operator=(const wchar_t* v);
		void operator=(const CString& v);
		void operator=(const CStringA& v);
		void operator=(const char* v);
		void operator=(__int64 v);
		void operator=(unsigned __int64 v); // size_t
		void operator=(int v);
		void operator=(unsigned int v);
		void operator=(CTime v);
		void operator=(double v);//error C2593: 'operator ='��(��) ��ȣ�մϴ�. 0 �϶��� �׸���
		//	void operator=(CJSonObj& v);
		void operator=(ShJObj v);
		void operator=(ShJArr v);
		void operator=(ShJVal v);
		void operator=(JVal& v);
		void operator=(JObj& v);//clone�ؾ� �Ѵ�.
		void operator=(JArr& v);
		void operator=(ONULL& v);
	};

	class JObj : public KStdMap<wstring, ShJVal>
	{
	public:
		JObj(){	}
		~JObj();

		JObj(const JObj& jobj, bool bClone = true)
		{
			if(&jobj == nullptr)
				throw_str("JObj::JObj(jobj jobj,) jobj is empty.");
			Clone(jobj, bClone);
		}

		JObj(const ShJObj sjo, bool bClone = true)
		{
			if(!sjo)
				throw_str("JObj::JObj(ShJObj sjo,) sjo is empty.");
			Clone(*sjo, bClone);
		}
		void operator=(const JObj& jbj)
		{
			Clone(jbj, true);
		}
		void operator=(const ShJObj sjo)
		{
			Clone(*sjo, true);
		}

		void ErrTest();
		//void DeleteAll();clear
#ifdef _DEBUG
		PWS _aaa;
		CStringW _txt;
#endif // _DEBUG
		CStrBufferT<CStringW, LPCWSTR> _buf;
		CStrBufferT<CStringA, LPCSTR> _bufa;
		void toString();
		//void Set(PAS name, ShJVal val);
		template <typename TKEY>
		void Set(TKEY k, ShJVal val)
		{
			SetAt(Pws(k), val);
			toString();
		}

		template <typename TKEY>
		void CopyAt(TKEY k, ShJVal val)
		{
			ShJVal nv = make_shared<JVal>(val);
			SetAt(Pws(k), nv);
		}
// 		void CopyAt(PAS k, ShJVal val)
// 		{
// 			CStringW kw(k);
// 			CopyAt(kw, val);
// 		}

		template <typename TKEY>
		void SetObj(TKEY k, ShJObj sjo, BOOL bClone = FALSE)
		{
			ShJVal sjv = make_shared<JVal>(sjo, bClone);
			Set(Pws(k), sjv);
		}
		template <typename TKEY>
		void SetArray(TKEY k, ShJArr sja, BOOL bClone = FALSE)
		{
			ShJVal sjv = make_shared<JVal>(sja, bClone);
			Set(Pws(k), sjv);
		}
// 		void SetArray(PAS name, ShJArr sja, BOOL bClone = FALSE)
// 		{
// 			CStringW kw(name);
// 			SetArray(kw, sja, bClone);
// 		}
		template <typename TKEY>
		bool DeleteKey(TKEY k)
		{
			auto kw = Pws(k);
			if(Has(kw))
			{
				this->erase(kw);
				return true;
			}
			return false;
		}
// 		bool DeleteKey(PAS name)
// 		{
// 			CStringW kw(name);
// 			return DeleteKey(kw);
// 		}


		//void Import(const JObj& src);

		/// bClone:������ value�� shared_ptr�̹Ƿ� �˾Ƽ� share�ϴµ�
		/// �ȿ� ���� ���� �� ��츸 true�� �ϸ� �ȴ�.
		void Clone(const JObj& src, bool bClone);
		void Clone(const ShJObj& src, bool bClone)
		{
			if(!src)
				throw_str("JObj::Clone(src,) src is empty.");
			Clone(*src, bClone);
		}

		static void CloneObject(const JObj& source, JObj& tar, bool bClone = true);

		template<typename FNC>
		static void CloneObjCond(const JObj& source, JObj& tar, FNC lmbdaCondition = [](auto k, auto sjv) {return true; }, bool bClone = true)
		{
			for(auto& [k, sjv] : source)
			{
				if(lmbdaCondition(k, sjv))
					tar.SetAt(k, make_shared<JVal>(*sjv, bClone));
			}
		}

/*
		PWS GetWide(PAS k)
		{
			static KStdMap<string, wstring> smap;
			auto it = smap.find(k);
			if(it == smap.end())
			{
				CStringW sw(k);
				smap[k] = (PWS)sw;
				it = smap.find(k);
			}
			return (PWS)it->second.c_str();
		}

		/// <summary>
		/// change PWS or PAS to PWS
		/// </summary>
		/// <typeparam name="TKEY">PWS or PAS</typeparam>
		/// <param name="k">key string point</param>
		/// <returns>PWS</returns>
		template <typename TKEY>
		PWS Pws(TKEY k)
		{
			PWS pw = nullptr;
			if(sizeof(k[0]) == sizeof(char))
				pw = GetWide((PAS)k);
			else
			{
				ASSERT(sizeof(k[0]) == sizeof(wchar_t));
				pw = (PWS)k;
			}
			return pw;
		}
*/

		template <typename TKEY>
		ShJVal Get(TKEY k)
		{
			ShJVal sjv;
			Lookup(Pws(k), sjv);//can be FALSE
			return sjv;
		}

// 		ShJVal Get(PAS k) {
// 			CStringW sw(k);
// 			return Get(sw);
// 		}

		PWS Ptr(PWS k);
		//wstring String(PWS k);
		//PAS SA(PAS k);
		//PWS S(PWS k);
		//PWS SN(PAS k); // nullable
		/// �Ǽ� �ΰ�� �Ҽ��� �Ʒ� ��
		template <typename TKEY>
		PWS Str(TKEY k, int point = 2)
		{
			CString& sbuf = _buf.GetBuf();
			ShJVal sjv;
			PWS kw = Pws(k);
			if(Lookup(kw, sjv))
				return sjv->Str(point);
			return L"";
		}

		/// �׸��� �ְ� IsString �̸� ����. �ƴϸ� ��
		//PWS S(PAS k) { CStringW sw(k);		return S((PWS)sw); }
		//PWS S(PAS k, CStringW& sv);

		template <typename TKEY>
		PWS S(TKEY k, PWS def = L"")
		{
			ShJVal sjv;
			if(Lookup(Pws(k), sjv))
				return sjv->S(def);
			return def;
		}
		template <typename TKEY>
		PAS SA(TKEY k, PAS def = "")
		{
			ShJVal sjv;
			if(Lookup(Pws(k), sjv))
				return sjv->SA(def);
			return def;
		}

		template <typename TKEY>
		PWS SN(TKEY k, PWS def = L"")
		{
			ShJVal sjv;
			if(Lookup(Pws(k), sjv))
				return sjv->SN(def);
			return def;
		}

		PWS QS(PWS k, BOOL bNullIfEmpty = TRUE, BOOL bQuat = TRUE, BOOL bNecessary = FALSE);
		PWS QS(PAS k, BOOL bNullIfEmpty = TRUE, BOOL bQuat = TRUE)
		{
			CStringW sw(k);
			return QS(sw, bNullIfEmpty, bQuat);
		}

		/// ���̰� 1�̻� �̸� ����. �ƴϸ� ��
		size_t Length(PAS k);
		BOOL Len(PAS k);
		BOOL IsEmpty(PAS k) { return !Len(k); }
		PWS LenS(PAS k, CStringW& sv);
		BOOL SameS(PAS k, PWS strk);
		BOOL SameSA(PAS k, PAS strk) { CStringW sw(strk); return SameS(k, sw); }
		BOOL OrStr(PAS k, PWS str, char tok = '|');
		CTime T(PAS k);


		template <typename TKEY>
		BOOL Find(TKEY k, PWS str)
		{
			PWS sn = SN(k);
			return tchstr(sn, str) != NULL;
		}


		template <typename TKEY>
		BOOL BeginS(TKEY k, PWS str)
		{
			PWS sn = SN(k);
			return tchbegin(sn, str);
		}

		template <typename TKEY>
		BOOL Append(TKEY k, PWS str)
		{
			auto kw = Pws(k);
			ShJVal sjv;
			if(Lookup(Pws(k), sjv))
			{
				wstring& ws = (wstring&)sjv->AsString();
				ws += str;
				return TRUE;
			}
			(*this)(kw) = str;
			return FALSE;
		}

		/// src���� tar�� �ʵ� �ϳ� ����
		//void Copy(JObj& src, PAS tarF, PAS srcF = nullptr);
		template <typename TKEY>
		void Copy(JObj& src, TKEY tarF, TKEY srcF = nullptr)
		{
			if(srcF == nullptr)
				srcF = tarF;
			PWS srcw = Pws(srcF);
			BOOL bHas = CopyIf(src, Pws(tarF), srcw);
			if(!bHas)
			{
				CStringA s; s.Format("JObj::Copy src(%s) field key Not found.", srcw);
				throw_str(s);
			}
// 			Copy(src, Pws(tarF), Pws(srcF));
		}
		template <typename TKEY>
		BOOL CopyIf(JObj& src, TKEY tarF, TKEY srcF)// = nullptr
		{
			if(!srcF)
				srcF = tarF;
			PWS srf = Pws(srcF);
			PWS taf = Pws(tarF);
			if(src.Has(srf))//this->find(k) != this->end())
			{
				this->DeleteKey(taf);//Ÿ�ٿ� ������ ����
				auto sjvS = src[srf];
				auto sjvT = ShJVal(new JVal(sjvS, true));
				Set((PWS)taf, sjvT);
				return TRUE;
			}
			return FALSE;
		}

		/// src�� �ʵ尡 ������ ����
		//BOOL CopyIf(JObj& src, PAS tarF, PAS srcF = nullptr);// = nullptr
			/// ���̰� �ְ� ������ ���� ��츸 true �̴�.
		int IsUpdated(JObj& src, PAS tarF, PAS srcF = nullptr);
		static int IsUpdated(JObj& src, JObj& tar, PAS tarF, PAS srcF = nullptr);
		static int IsUpdated(ShJObj& src, ShJObj& tar, PAS tarF, PAS srcF = nullptr);
		static int IsUpdated(ShJObj& src, JObj& tar, PAS tarF, PAS srcF = nullptr);
		static int IsUpdated(JObj& src, ShJObj& tar, PAS tarF, PAS srcF = nullptr);

// 		CStringW SLeft(PAS k, int len)
// 		{
// 			CStringW sw(k);
// 			return SLeft((PWS)sw, len);
// 		}
// 		CStringW SLeft(PWS k, int len);
// 		CStringW SRight(PAS k, int len)
// 		{
// 			CStringW sw(k);
// 			return SRight((PWS)sw, len);
// 		}
// 		CStringW SRight(PWS k, int len);
		template <typename TKEY>
		CStringW SLeft(TKEY k, int len)
		{
			ShJVal sjv;
			if(Lookup(Pws(k), sjv))
				return sjv->SLeft(len);
			return L"";
		}

		template <typename TKEY>
		CStringW SRight(TKEY k, int len)
		{
			ShJVal sjv;
			if(Lookup(Pws(k), sjv))
				return sjv->SRight(len);
			return L"";
		}

// 		double N(PWS k);
// 		double N(PAS k)		{			CStringW sw(k);			return N((PWS)sw);		}
// 		double D(PWS k)		{			return N(k);		}
// 		double D(PAS k)		{			return N(k);		}
		template <typename TKEY>
		double N(TKEY k, double dfv = 0.)
		{
			if(this == NULL)
				throw (L"JObj.this == NULL");
			//?����: if((*this)[k]) �̰� ���� ���� ������� ������.
			ShJVal sjv;
			if(Lookup(Pws(k), sjv))
				return sjv->N(dfv);
			return dfv;
		}
		template <typename TKEY> double D(TKEY k, double dfv = 0.)
		{	return N(k, dfv);	}

		// _buf.GetBuf(); �� ���� String buffer�� ���� �Ѵ�.
		// SQL �ȿ� �� ���� �̹Ƿ� ���� �׷��� �ؾ� �Ѵ�.
		PWS QN(PWS k, int underDot = 0);
		PWS QN(PAS k, int underDot = 0)	{CStringW sw(k);return QN(sw, underDot);}
		
		//double�� �⺻������ �������� Ȯ���Ҷ�
		//int I(PWS k, int def = 0);
		//int I(PAS k, int def = 0) { CStringW sw(k); return (int)I((PWS)sw, def); }
		template <typename TKEY>
		int I(TKEY k, int def = 0)
		{
			ShJVal sjv;
			if(Lookup(Pws(k), sjv))
				return sjv->I(def);
			return def;
		}
		// 		__int64 I64(PWS k);
// 		__int64 I64(PAS k) { CStringW sw(k); return (__int64)I64((PWS)sw); }
		template <typename TKEY>
		__int64 I64(TKEY k, __int64 dfv = 0)
		{
			ShJVal sjv;
			if(Lookup(Pws(k), sjv))
				return sjv->AsInt64();
			return dfv;
		}
		
		template <typename TKEY>
		ShJArr A(TKEY k, bool bCreat = false)
		{
			ShJVal sjv;
			PWS kw = Pws(k);
			if(Lookup(kw, sjv))
			{
				if(sjv->IsArray())
					return sjv->AsArray();
				else
					throw_str("IsArray() false.");
			}
			if(bCreat)
			{
				ShJArr sjo = make_shared<JArr>();
				SetArray(kw, sjo, false);
				return sjo;
			}
			return ShJArr();
		}
		template <typename TKEY> ShJArr Array(TKEY k, bool bCreat = false)
		{
			return A(k, bCreat);
		}

// 		ShJArr Array(PWS k);
// 		ShJArr Array(PAS k) { CStringW sw(k); return Array((PWS)sw); }
// 		ShJArr AMake(PWS k);
// 		ShJArr AMake(PAS k) { CStringW sw(k); return AMake((PWS)sw); }

		/// ���� �Ǵ°� �ƴϰ�, ���� ��ü�� �״�� ����Ų���� �����ͷ� ���ϵȴ�.
		template <typename TKEY>
		ShJObj O(TKEY k, bool bCreat = false)
		{
			/// �Ʒ� ->find���� unhandled�� ������ ���´�. �� unhandled
			//?����: if((*this)[k]) �̰� ���� ���� ������� ������.
			if(this == nullptr)
				throw_str("this == nullptr.");
			ShJVal sjv;
			if(Lookup(Pws(k), sjv))
			{
				if(sjv->IsObject())
					return sjv->AsObject();// shared_ptr ���ΰ� �״�� ���� �ȴ�.
				else
					throw_str("IsObject() false.");
			}
			if(bCreat)
			{
				ShJObj sjo = make_shared<JObj>();
				SetObj(k, sjo, false);
				return sjo;
			}
			return ShJObj();
		}
		template <typename TKEY> ShJObj Obj(TKEY k, bool bCreat = false)
		{	return O(k, bCreat);	}

		// O() ����
// 		ShJObj OMake(PWS k);// make_shared<JObj>() �� ����� �־ ��ü�� �ִ´�.
// 		ShJObj OMake(PAS k) { CStringW sw(k); return OMake((PWS)sw); }
// 		ShJObj O(PAS k)					{CStringW sw(k);return Obj((PWS)sw);}
// 		ShJObj O(string k)				{CStringW sw(k.c_str());return Obj((PWS)sw);}
		ShJObj OO(PAS k1, PAS k2);
		ShJObj OO(PAS k1, string k2)	{return OO(k1, k2.c_str());}
		ShJObj OO(string k1, string k2)	{return OO(k1.c_str(), k2.c_str());}
		ShJObj OO(string k1, PAS k2)	{return OO(k1.c_str(), k2);}
		//ShJObj AI(PAS k1, int i = 0);
		ShJArr OA(PAS k1, PAS k2);
		PWS OOS(PAS k1, PAS k2, PAS k3);

		/// GetArrayItem ���� : �ű� ���� Array ���� �ϴ°� ���� ������� ����.

		// ���� CJsonPbj�� ȣȥ�� ����. �Ÿ� *jobj.O("field") �ϱ� ������. jobj.Oref("field"); ����
		//JObj _jobjNull;// { nullptr };
// 		template <typename TKEY>
// 		JObj& Oref(TKEY k)
// 		{
// 			//CStringW sw(k);
// 			auto sjo = O(k);
// 			if(!sjo)//.get())
// 				throw_str("JObj::Oref() ShJObj is empty.");
// 			return *sjo;
// 		}
// 		//ShJObj operator->() const throw()
		//{
		//	ASSERT(m_pJobj != NULL);
		//	return m_pJobj;
		//}
// 		JUnit operator()(PWS k)			{return JUnit(this, k);}
// 		JUnit operator()(wstring k)		{return JUnit(this, k.c_str());}
// 		JUnit operator()(string k)		{CStringW kw(k.c_str());return JUnit(this, kw);}
// 		JUnit operator()(PAS k)			{CStringW kw(k);return JUnit(this, kw);}
// 		JUnit Unit(PWS k)				{return JUnit(this, k);}
// 		JUnit Unit(PAS k)				{CStringW kw(k);return JUnit(this, kw);}
		template <typename TKEY>
		JUnit operator()(TKEY k) 
		{ return JUnit(this, Pws(k)); 
		}

		


		template <typename TKEY>
		JUnit Unit(TKEY k) 
		{
			return JUnit(this, Pws(k));
		}


		/// 1��Ű�� ���� �迭, �� �迭�� 2�� �ε����� JObj ũ���� 2��Ű�� ���� ��� �´�.
		/// �� table�� ���� �ͼ� idx��° row �� k Ű���� ���� �´�.
		bool GetArrayItem(PAS karr, int idx, PAS k, CStringW& rval);
		bool GetArrayItem(PAS karr, int idx, PAS k, int& rval);
		ShJVal GetArrayItem(PAS karr, int idx, PAS k);
		ShJObj GetArrayItem(PAS karr, int idx);
		//void operator=(ShJObj v)
		//{
		//	*m_pJobj = *v.m_pJobj;//clone�ؾ� �ϴµ�,..
		//}
// 		bool Has(wstring kw)
// 		{
// 			return __super::Find(kw) != this->end();
// 		}
// 		bool Has(string k)
// 		{
// 			if(k.empty())
// 				TRACE("Has(k) k is empty.(Can empty string be key?)\n");
// 			CStringW kw(k.c_str());
// 			wstring wsk((PWS)kw);
// 			return this->Has(wsk);
// 		}
		// 		bool Has(PWS k)
// 		{
// 			wstring wk(k);
// 			return this->Has(wk);
// 		}
// 		bool Has(PAS k)
// 		{
// 			CStringW kw(k);
// 			return Has((PWS)kw);
// 		}
		template <typename TKEY>
		bool Has(TKEY k)
		{
// 			PWS pw = nullptr;
// 			if(sizeof(k[0]) == sizeof(char))
// 			{
// 				CStringW kw(k);
// 				pw = (PWS)kw;
// 			}
// 			else
// 			{
// 				ASSERT(sizeof(k[0]) == sizeof(wchar_t));
// 				pw = (PWS)k;
// 			}
			return __super::Find(Pws(k)) != this->end();
		}
// 		bool Has(CStringW sk)
// 		{
// 			return Has((PWS)sk);
// 		}
		/// <summary>
		/// �����Ͱ� ���� ��츸 ����Ʈ ������ �ִ´�. 
		/// </summary>
		/// <returns>Has�� ���ϰ��� ����. </returns>
		template<typename TKEY, typename TVAL>
		bool HasElse(TKEY k, TVAL v)
		{
			PWS pwk = Pws(k);
			if(!Has(pwk))
			{
				(*this)(pwk) = v;
				return false;
			}
			return true;
		}

		bool IsArray(PAS k) {	CStringW kw(k);	return IsArray(kw); }
		bool IsObject(PAS k){	CStringW kw(k);	return IsObject(kw); }
		bool IsString(PAS k){	CStringW kw(k);	return IsString(kw); }
		bool IsNumber(PAS k) { CStringW kw(k);	return IsNumber(kw); }
		bool IsDouble(PWS k) { CStringA kw(k);	return IsDouble(kw); }
		bool IsInt64(PWS k){	CStringA kw(k);	return IsInt64(kw); }

		bool IsString(PWS k);
		bool IsDouble(PAS k);
		bool IsNumber(PWS k);//IsDouble() || IsInt() || IsInt64()
		bool IsInt(PAS k);
		bool IsInt64(PAS k);
		bool IsArray(PWS k);
		bool IsObject(PWS k);
		bool IsNull(PAS k);

		CStringW ToJsonStringW();
		CStringA ToJsonStringUtf8();
		SHP<KBinary> ToJsonBinaryUtf8();
		KBinary ToJsonData();
		bool CopyFielsIf(JObj& src, PWS key);
		int CopyFieldsAll(JObj& src);

		template<typename TVAL>
		bool SetIfNull(PAS key, TVAL val)
		{
			if(!this->Has(key))
			{
				(*this)(key) = val;
				return true;
			}
			return false;
		}

	};


	class JArr : public KArray<ShJVal>
	{
	public:
		JArr()
		{
		}
		JArr(const JArr& jobj, bool bClone = true);
		JArr(const ShJArr jobj, bool bClone = true);

		bool _bValueOwner{ true };
		~JArr();
		static void CloneArray(const JArr& src, JArr& tar, bool bClone = true); //JArr�� std::vector �̹Ƿ� ��ü Clone�� ����.
		void Clone(const ShJArr& src, bool bClone = true)
		{
			if(!src)
				throw_str("JArr::Clone(src,) src is empty.");
			JArr::CloneArray(*src, *this, bClone);
		}
		void Add(const wchar_t* v);
		void Add(double v);
		void Add(int v);
		//void Add(long v);
		void Add(bool v);
		void Add(JObj& v, bool bClone = true);
		///void Add(JArr& v, bool bClone = true); array�ȿ� array�ִ� ���� ���� ����?
		void Add(ShJObj sv, bool bClone = true);
		void Add(ShJVal sv, bool bClone = true);
		ShJObj FindByValue(PAS field, PWS value);
	};

	
	class IStrConvert2
	{
	public:
		virtual const wchar_t* CharToString(const wchar_t* key, wchar_t ch) = NULL;
		//virtual bool CheckIfCharConv(const wchar_t* key) = NULL;
	};

	class JVal
	{

	public:
		//bool _bOwner{ false }; 
		/// JArr, JObj �� ��ü������ ���� �� delete �ϴ� _bValueOwner�� ������ delete �Ѵ�.
		/// �׷��� �ʿ� ����.
		JVal();
		JVal(const wchar_t* char_value1);
		JVal(const char* char_value1);
		JVal(const wstring& string_value1);
		JVal(bool bool_value1);
		JVal(double number_value1);
		JVal(int int_value1);
		JVal(__int64 int64_value1);
		JVal(unsigned int int_value1);
		JVal(unsigned __int64 int64_value1);

		/// share obj
		/// const�� shared_ptr ���� ���ٲٰ�, �ȿ� pointer�� ����Ű�� ��ü�� �ٲܼ� �ִ�.
		JVal(const ShJArr sja, bool bClone = true);
		JVal(const ShJObj sjo, bool bClone = true);// , JStrArray& array_key1);
		JVal(const ShJVal sjv, bool bClone = true);
		//JVal(const JObj* object_value1);// , JStrArray& array_key1);
		//JVal(const ShJVal jv);
#ifdef _DEBUG
		CStringW _txt;
		string _________________________________________________;
#endif // _DEBUG
		JsonType type{JsonType_Null};
		ShJObj object_;
		//ShJVal parent{nullptr};

		wstring string_;
		__int64 int64_{-1};// 32bit ������ int�� ���� 4byte ������, 64bit ������ 8byte�̴�.
		double  double_{-1.};
		ShJArr array_;//������ �����Ͱ� [,,,] array �� ���
// 		int     int_value{-1};
// 		bool    bool_value{false};
// 		unsigned int     uint_value{ -1 };
// 		unsigned __int64 uint64_value{ -1 };// 32bit ������ int�� ���� 4byte ������, 64bit ������ 8byte�̴�.
		void toString();

// 		void DebugValue()
// 		{
// #ifdef DEBUG
// 			toString();
// 			//JSonTextVal(_text, 50);
// #endif
// 		}
		void JSonTextVal(std::wstring& sts, int maxlen = 50, bool bNoQuat = false);

		CStrBufferT<CStringW, LPCWSTR> _buf;
		CStrBufferT<CStringA, LPCSTR> _bufa;

		/// �̰� assign�Ҷ��� ���� �ȵǴ� shared_ptr�� �μ� �ִµ� �׶� const�̸� �ȵ�� ����.
		JVal(JObj& jv, bool bClone = true);//clone�ؾ� �Ѵ�. jv�� ���� ���� �̰�  false�̸� ū�� �ܴ�.
		JVal(JArr& jv, bool bClone = true);//clone�ؾ� �Ѵ�.
		JVal(JVal& jv, bool bClone = true);//clone�ؾ� �Ѵ�.
		~JVal();

		// ���� �Ǹ鼭 = �� ������ JVal(JVal& jv)�� �Ҹ���.
		void operator=(JVal& jv);

		/// clone ����
		void ShareObj(JObj& obj1);
		//void WrappObj(JObj& obj1);
		//void MoveObj(JObj& obj1);
		//void MoveArray(JArr& arr1);
		// void WrappArray(JArr& array1); �̰� ���߿� �ʿ� �Ҷ�, 


		bool IsNull() const			{		return type == JsonType_Null;		}
		bool IsString() const		{		return type == JsonType_String;		}
		bool IsBool() const			{		return type == JsonType_Bool;		}
		bool IsDouble() const		{		return type == JsonType_Double;		}
		bool IsInt64() const		{		return type == JsonType_Int64;		}
		bool IsInt() const			{		return type == JsonType_Int;		}
		
		bool IsNumber() const { return IsDouble() || IsInt() || IsInt64(); }// type == JsonType_Number;
		bool IsArray() const		{		return type == JsonType_Array;		}
		bool IsObject() const		{		return type == JsonType_Object;		}

		const wstring& AsString() const	{	return string_;		}
		bool AsBool() const			{		return int64_ == 1;		}
		double AsDouble() const		{ ASSERT(IsDouble());	return double_;		}	//double AsNumber() const;
		int AsInt() const			{ ASSERT(IsInt());	return (int)int64_;		}
		__int64 AsInt64() const { ASSERT(IsInt64());	return int64_; }
		ShJArr AsArray()			{ ASSERT(IsArray());	return array_;		} //�տ� const ����. array���� �Ϸ���
		ShJObj AsObject()			{ ASSERT(IsObject());	return object_;		}

		PWS S(PWS def = L"") const;
		PWS Str(int point = 2);
		PWS Ptr() const;
		PAS SA(PAS def = "");
		PWS SN(PWS def = L""); // nullable
		CTime T();
		CStringW SLeft(int len);
		CStringW SRight(int len);



		double N(double dfv = 0.);
		double D(double dfv = 0.)
		{
			return N(dfv);
		}
		int I(int def = 0);







		void Clone(const JVal& src, bool bClone = true);


		std::size_t CountChildren() const;
		bool HasChild(std::size_t index) const;
		ShJVal Child(std::size_t index);
		bool HasChild(const wchar_t* name) const;
		ShJVal Child(const wchar_t* name);
		wstring Stringify(const bool bUnicode = true, bool const prettyprint = false, const wchar_t* key = NULL, IStrConvert2* pinf = NULL) const;


		static ShJVal Parse(const wchar_t** data);

	private:
		static wstring StringifyString(const bool bUnicode, const wstring& str, const wchar_t* key, IStrConvert2* pinf = NULL);
		wstring StringifyImpl(const bool bUnicode, size_t const indentDepth, const wchar_t* key = NULL, IStrConvert2* pinf = NULL) const;
		static wstring Indent(size_t depth);

	public:
		/// /////////////////////////////////////////////////////////////////////////
		//wstring _key;// ���� ���� map �� � Ű�� setat �Ǿ���.


					  //std::vector<std::wstring> _keyOrder;// isObject�� ��� key�� ó�� ������ � ������ ��� ����.
		//DWORD_PTR _uiData{ 0 }; // HTREEITEM


		void InitArray();
		void InitObject();
		//JStrArray array_key;//map �� ��� ���� �ҽ��� ������ ��� �ϱ� ����. ���� �ϰų� �߰� �Ұ�� ��� �ؾ�. ���� ������ �ٲܼ��� �ִ�.
		/// /////////////////////////////////////////////////////////////////////////



		//void SetKey(std::wstring key);
		//void SetKey(PWS key) { _key = key; }
		//void SetData(DWORD_PTR data) { _uiData = data; }
		//PWS GetKey() { return _key.c_str(); }
		//void AddKey(PWS key) { _keyOrder.push_back(key);}
		void setString(const wchar_t* v)
		{
			ASSERT(IsString());
			string_ = v;
		}
		void setDouble(double v)
		{
			ASSERT(IsNumber());
			double_ = v;
		}
		void setInt(int v)
		{
			ASSERT(IsInt());
			int64_ = v;
		}
		void setLong64(long v)
		{
			ASSERT(IsInt64());
			int64_ = v;
		}
		void setBool(bool v)
		{
			ASSERT(IsBool());
			int64_ = v ? 1 : 0;
		}
		int setValue(ShJVal snd1);
		double CompareValue(ShJVal snd1);
		int IsSameValue(ShJVal snd1);

		std::wstring GetJSonText(int maxlen = 1024);

	};


	class Json
	{
		friend class JVal;

	public:
		static ShJObj ParseUtf8(const char* data);
		static ShJVal Parse(const char* data);
		static ShJVal Parse(const wchar_t* data);
		static wstring Stringify(const ShJVal value);
		static bool SkipWhitespace(PWS* data);
		static bool ExtractString(PWS* data, wstring& str);
		static double ParseInt(PWS* data);
		static double ParseDecimal(PWS* data);
	private:
		Json();
	};











};

#define KJSPUT(val) js(#val) = val
#define KJSGETS(val) val = js.S(#val)
#define KJSGETSA(val) val = CStringA(js.S(#val))
#define KJSGETN(val) val = js.N(#val)
#define KJSGETI(val) val = js.I(#val)

