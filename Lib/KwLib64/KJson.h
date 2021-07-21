#pragma once
#include "stdecl.h"
#include "ktypedef.h"
#include "tchtool.h"
#include "KBinary.h"
#include "StrBuffer.h"
#include "KVal.h"


namespace Kw
{

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
		void Set(PAS name, ShJVal val);
		void Set(PWS name, ShJVal val)
		{
			SetAt(name, val);
		}
		void SetObj(PWS name, ShJObj sjo, BOOL bClone = FALSE);
		void SetObj(PAS name, ShJObj sjo, BOOL bClone = FALSE)
		{
			CStringW kw(name);
			SetObj(kw, sjo, bClone);
		}
		void SetArray(PWS name, ShJArr sja, BOOL bClone = FALSE);
		void SetArray(PAS name, ShJArr sja, BOOL bClone = FALSE)
		{
			CStringW kw(name);
			SetArray(kw, sja, bClone);
		}
		bool DeleteKey(PWS name);
		bool DeleteKey(PAS name)
		{
			CStringW kw(name);
			return DeleteKey(kw);
		}


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


		PWS Ptr(PWS k);
		wstring String(PWS k);
		PAS SA(PAS k);
		PWS S(PWS k);
		PWS SN(PAS k); // nullable
		PWS QS(PWS k, BOOL bNullIfEmpty = TRUE, BOOL bQuat = TRUE, BOOL bNecessary = FALSE);
		PWS QS(PAS k, BOOL bNullIfEmpty = TRUE, BOOL bQuat = TRUE)
		{
			CStringW sw(k);
			return QS(sw, bNullIfEmpty, bQuat);
		}

		/// 실수 인경우 소수점 아래 수
		PWS Str(PAS k, int point = 2);
		/// 항목이 있고 IsString 이면 리턴. 아니면 널
		PWS S(PAS k) { CStringW sw(k);		return S((PWS)sw); }
		/// 길이가 1이상 이면 리턴. 아니면 널
		BOOL Len(PAS k);
		PWS S(PAS k, CStringW& sv);
		PWS LenS(PAS k, CStringW& sv);
		BOOL SameS(PAS k, PWS strk);
		BOOL BeginS(PAS k, PWS str);
		BOOL Find(PAS k, PWS str);
		BOOL Append(PAS k, PWS str);
		BOOL OrStr(PAS k, PWS str, char tok = '|');
		CTime T(PAS k);

		void Copy(JObj& src, PAS tarF, PAS srcF = nullptr);
		void Copy(JObj& src, PWS tarF, PWS srcF = nullptr)
		{
			CStringA tw(tarF);
			if(srcF == nullptr)
				srcF = tarF;
			CStringA sw(srcF);
			Copy(src, tw, sw);
		}
		BOOL CopyIf(JObj& src, PAS tarF, PAS srcF = nullptr);// = nullptr
			/// 길이가 있고 내용이 같은 경우만 true 이다.
		int IsUpdated(JObj& src, PAS tarF, PAS srcF = nullptr);
		static int IsUpdated(JObj& src, JObj& tar, PAS tarF, PAS srcF = nullptr);
		static int IsUpdated(ShJObj& src, ShJObj& tar, PAS tarF, PAS srcF = nullptr);
		static int IsUpdated(ShJObj& src, JObj& tar, PAS tarF, PAS srcF = nullptr);
		static int IsUpdated(JObj& src, ShJObj& tar, PAS tarF, PAS srcF = nullptr);

		CStringW SLeft(PAS k, int len)
		{
			CStringW sw(k);
			return SLeft((PWS)sw, len);
		}
		CStringW SLeft(PWS k, int len);
		CStringW SRight(PAS k, int len)
		{
			CStringW sw(k);
			return SRight((PWS)sw, len);
		}
		CStringW SRight(PWS k, int len);

		double N(PWS k);
		double N(PAS k)
		{
			CStringW sw(k);
			return N((PWS)sw);
		}
		double D(PWS k)
		{
			return N(k);
		}
		double D(PAS k)
		{
			return N(k);
		}
		int I(PWS k);

		// _buf.GetBuf(); 로 잡은 String buffer로 리턴 한다.
		// SQL 안에 쓸 문자 이므로 굳이 그렇게 해야 한다.
		PWS QN(PWS k, int underDot = 0);
		PWS QN(PAS k, int underDot = 0)	{CStringW sw(k);return QN(sw, underDot);}
		//double이 기본이지만 정수인지 확신할때
		int I(PAS k)					{CStringW sw(k);return (int)N((PWS)sw);}
		const ShJArr Array(PWS k);
		const ShJArr Array(PAS k)		{CStringW sw(k);return Array((PWS)sw);}

		/// 복사 되는게 아니고, 내부 객체가 그대로 가르킨것이 포인터로 리턴된다.
		ShJObj Obj(PWS k);
		ShJObj O(PAS k)					{CStringW sw(k);return Obj((PWS)sw);}
		ShJObj O(string k)				{CStringW sw(k.c_str());return Obj((PWS)sw);}
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
		JObj& Oref(PAS k)
		{
			CStringW sw(k);
			auto sjo = Obj((PWS)sw);
			if(!sjo)//.get())
				throw_str("JObj::Oref() ShJObj is empty.");
			return *sjo;
		}
		//ShJObj operator->() const throw()
		//{
		//	ASSERT(m_pJobj != NULL);
		//	return m_pJobj;
		//}
		JUnit operator()(PWS k)			{return JUnit(this, k);}
		JUnit operator()(wstring k)		{return JUnit(this, k.c_str());}
		JUnit operator()(string k)		{CStringW kw(k.c_str());return JUnit(this, kw);}
		JUnit operator()(PAS k)			{CStringW kw(k);return JUnit(this, kw);}
		JUnit Unit(PWS k)				{return JUnit(this, k);}
		JUnit Unit(PAS k)				{CStringW kw(k);return JUnit(this, kw);}


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

		bool Has(wstring kw)
		{
			if(kw.empty())
				throw_str("Has(kw) kw is empty");
			return this->find(kw) != this->end();
		}
		bool Has(string k)
		{
			if(k.empty())
				throw_str("Has(k) k is empty");
			CStringW kw(k.c_str());
			return this->Has((PWS)kw);
		}

		bool Has(PWS k)
		{
			if(this == nullptr)
				throw_str("Has(k) k is null");
			wstring wk(k);
			return this->Has(wk);
		}
		bool Has(PAS k)
		{
			CStringW kw(k);
			return Has((PWS)kw);
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
		void Add(ShJObj sv);
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
		string __{"                                                                                                    "};
		//wstring _text;// ui에 보여지는 text로 디버깅용으로만 사용

#endif // _DEBUG
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

		const wstring& AsString() const	{	return string_value;		}
		bool AsBool() const			{		return bool_value;		}
		double AsDouble() const		{		return double_value;		}	//double AsNumber() const;
		int AsInt() const			{		return int_value;		}
		__int64 AsInt64() const		{		return int64_value;		}
		ShJArr AsArray()			{		return array_value;		} //앞에 const 없앰. array편집 하려고
		ShJObj AsObject()			{		return object_value;		}

		PWS S() const;
		PWS Str(int point = 2);
		PWS Ptr() const;
		PAS SA();
		PWS SN(); // nullable
		CTime T();
		CStringW SLeft(int len);
		CStringW SRight(int len);



		double N();
		double D()
		{
			return N();
		}
		int I();







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

		ShJVal parent{ nullptr };
		JsonType type{ JsonType_Null };

		wstring string_value;
		bool    bool_value{ false };
		double  double_value{ -1. };
		int     int_value{ -1 };
		__int64 int64_value{ -1 };// 32bit 에서는 int와 같이 4byte 이지만, 64bit 에서는 8byte이다.
// 		unsigned int     uint_value{ -1 };
// 		unsigned __int64 uint64_value{ -1 };// 32bit 에서는 int와 같이 4byte 이지만, 64bit 에서는 8byte이다.

		ShJArr array_value;//실제로 데이터가 [,,,] array 인 경우
		ShJObj object_value;
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
			string_value = v;
		}
		void setDouble(double v)
		{
			ASSERT(IsNumber());
			double_value = v;
		}
		void setInt(int v)
		{
			ASSERT(IsInt());
			int_value = v;
		}
		void setLong64(long v)
		{
			ASSERT(IsInt64());
			int64_value = v;
		}
		void setBool(bool v)
		{
			ASSERT(IsBool());
			bool_value = v;
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

