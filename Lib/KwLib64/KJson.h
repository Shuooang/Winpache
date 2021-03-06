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
		void operator=(double v);//error C2593: 'operator ='이(가) 모호합니다. 0 일때만 그르네
		//	void operator=(CJSonObj& v);
		void operator=(ShJObj v);
		void operator=(ShJArr v);
		void operator=(ShJVal v);
		void operator=(JVal& v);
		void operator=(JObj& v);//clone해야 한다.
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

		/// bClone:어차피 value가 shared_ptr이므로 알아서 share하는데
		/// 안에 값을 따로 갈 경우만 true로 하면 된다.
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
		/// 실수 인경우 소수점 아래 수
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

		/// 항목이 있고 IsString 이면 리턴. 아니면 널
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

		/// 길이가 1이상 이면 리턴. 아니면 널
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

		/// src에서 tar로 필드 하나 복사
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
				this->DeleteKey(taf);//타겟에 있으면 삭제
				auto sjvS = src[srf];
				auto sjvT = ShJVal(new JVal(sjvS, true));
				Set((PWS)taf, sjvT);
				return TRUE;
			}
			return FALSE;
		}

		/// src에 필드가 있으면 복사
		//BOOL CopyIf(JObj& src, PAS tarF, PAS srcF = nullptr);// = nullptr
			/// 길이가 있고 내용이 같은 경우만 true 이다.
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
			//?주의: if((*this)[k]) 이걸 쓰는 쑨간 만들어져 버린다.
			ShJVal sjv;
			if(Lookup(Pws(k), sjv))
				return sjv->N(dfv);
			return dfv;
		}
		template <typename TKEY> double D(TKEY k, double dfv = 0.)
		{	return N(k, dfv);	}

		// _buf.GetBuf(); 로 잡은 String buffer로 리턴 한다.
		// SQL 안에 쓸 문자 이므로 굳이 그렇게 해야 한다.
		PWS QN(PWS k, int underDot = 0);
		PWS QN(PAS k, int underDot = 0)	{CStringW sw(k);return QN(sw, underDot);}
		
		//double이 기본이지만 정수인지 확신할때
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

		/// 복사 되는게 아니고, 내부 객체가 그대로 가르킨것이 포인터로 리턴된다.
		template <typename TKEY>
		ShJObj O(TKEY k, bool bCreat = false)
		{
			/// 아래 ->find에서 unhandled로 못빠져 나온다. 왜 unhandled
			//?주의: if((*this)[k]) 이걸 쓰는 쑨간 만들어져 버린다.
			if(this == nullptr)
				throw_str("this == nullptr.");
			ShJVal sjv;
			if(Lookup(Pws(k), sjv))
			{
				if(sjv->IsObject())
					return sjv->AsObject();// shared_ptr 내부가 그대로 노출 된다.
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

		// O() 참조
// 		ShJObj OMake(PWS k);// make_shared<JObj>() 로 만들어 넣어서 빈객체라도 넣는다.
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

		/// GetArrayItem 참조 : 거기 내부 Array 접근 하는거 많이 만들어져 있음.

		// 이전 CJsonPbj와 호혼을 위해. 매면 *jobj.O("field") 하기 귀찮아. jobj.Oref("field"); 끝내
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


		/// 1차키의 값이 배열, 그 배열의 2차 인덱스가 JObj 크리고 2차키로 값을 담아 온다.
		/// 즉 table을 가져 와서 idx번째 row 중 k 키값을 가져 온다.
		bool GetArrayItem(PAS karr, int idx, PAS k, CStringW& rval);
		bool GetArrayItem(PAS karr, int idx, PAS k, int& rval);
		ShJVal GetArrayItem(PAS karr, int idx, PAS k);
		ShJObj GetArrayItem(PAS karr, int idx);
		//void operator=(ShJObj v)
		//{
		//	*m_pJobj = *v.m_pJobj;//clone해야 하는데,..
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
		/// 데이터가 없는 경우만 디폴트 값으로 넣는다. 
		/// </summary>
		/// <returns>Has의 리턴값과 같다. </returns>
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
		static void CloneArray(const JArr& src, JArr& tar, bool bClone = true); //JArr는 std::vector 이므로 자체 Clone이 없다.
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
		///void Add(JArr& v, bool bClone = true); array안에 array넣는 것은 아직 없네?
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
		/// JArr, JObj 가 자체적으로 내부 값 delete 하는 _bValueOwner를 가지고 delete 한다.
		/// 그래서 필요 없다.
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
		/// const는 shared_ptr 값을 못바꾸고, 안에 pointer가 가르키는 객체는 바꿀수 있다.
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
		__int64 int64_{-1};// 32bit 에서는 int와 같이 4byte 이지만, 64bit 에서는 8byte이다.
		double  double_{-1.};
		ShJArr array_;//실제로 데이터가 [,,,] array 인 경우
// 		int     int_value{-1};
// 		bool    bool_value{false};
// 		unsigned int     uint_value{ -1 };
// 		unsigned __int64 uint64_value{ -1 };// 32bit 에서는 int와 같이 4byte 이지만, 64bit 에서는 8byte이다.
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

		/// 이걸 assign할때는 삭제 안되는 shared_ptr로 싸서 넣는데 그때 const이면 안들어 간다.
		JVal(JObj& jv, bool bClone = true);//clone해야 한다. jv가 스택 변수 이고  false이면 큰일 단다.
		JVal(JArr& jv, bool bClone = true);//clone해야 한다.
		JVal(JVal& jv, bool bClone = true);//clone해야 한다.
		~JVal();

		// 생성 되면서 = 를 쓸경우는 JVal(JVal& jv)가 불린다.
		void operator=(JVal& jv);

		/// clone 방지
		void ShareObj(JObj& obj1);
		//void WrappObj(JObj& obj1);
		//void MoveObj(JObj& obj1);
		//void MoveArray(JArr& arr1);
		// void WrappArray(JArr& array1); 이건 나중에 필요 할때, 


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
		ShJArr AsArray()			{ ASSERT(IsArray());	return array_;		} //앞에 const 없앰. array편집 하려고
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
		//wstring _key;// 나는 상위 map 에 어떤 키로 setat 되었다.


					  //std::vector<std::wstring> _keyOrder;// isObject인 경우 key는 처음 읽을때 어떤 순서로 들어 갔다.
		//DWORD_PTR _uiData{ 0 }; // HTREEITEM


		void InitArray();
		void InitObject();
		//JStrArray array_key;//map 인 경우 원래 소스의 순서를 기억 하기 위해. 삭제 하거나 추가 할경우 고려 해야. 또한 순서를 바꿀수도 있다.
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

