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

		/// �Ǽ� �ΰ�� �Ҽ��� �Ʒ� ��
		PWS Str(PAS k, int point = 2);
		/// �׸��� �ְ� IsString �̸� ����. �ƴϸ� ��
		PWS S(PAS k) { CStringW sw(k);		return S((PWS)sw); }
		/// ���̰� 1�̻� �̸� ����. �ƴϸ� ��
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
			/// ���̰� �ְ� ������ ���� ��츸 true �̴�.
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

		// _buf.GetBuf(); �� ���� String buffer�� ���� �Ѵ�.
		// SQL �ȿ� �� ���� �̹Ƿ� ���� �׷��� �ؾ� �Ѵ�.
		PWS QN(PWS k, int underDot = 0);
		PWS QN(PAS k, int underDot = 0)	{CStringW sw(k);return QN(sw, underDot);}
		//double�� �⺻������ �������� Ȯ���Ҷ�
		int I(PAS k)					{CStringW sw(k);return (int)N((PWS)sw);}
		const ShJArr Array(PWS k);
		const ShJArr Array(PAS k)		{CStringW sw(k);return Array((PWS)sw);}

		/// ���� �Ǵ°� �ƴϰ�, ���� ��ü�� �״�� ����Ų���� �����ͷ� ���ϵȴ�.
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

		/// GetArrayItem ���� : �ű� ���� Array ���� �ϴ°� ���� ������� ����.

		// ���� CJsonPbj�� ȣȥ�� ����. �Ÿ� *jobj.O("field") �ϱ� ������. jobj.Oref("field"); ����
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
		/// JArr, JObj �� ��ü������ ���� �� delete �ϴ� _bValueOwner�� ������ delete �Ѵ�.
		/// �׷��� �ʿ� ����.
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
		/// const�� shared_ptr ���� ���ٲٰ�, �ȿ� pointer�� ����Ű�� ��ü�� �ٲܼ� �ִ�.
		JVal(const ShJArr sja, bool bClone = true);
		JVal(const ShJObj sjo, bool bClone = true);// , JStrArray& array_key1);
		JVal(const ShJVal sjv, bool bClone = true);
		//JVal(const JObj* object_value1);// , JStrArray& array_key1);
		//JVal(const ShJVal jv);
#ifdef _DEBUG
		CStringW _txt;
		string __{"                                                                                                    "};
		//wstring _text;// ui�� �������� text�� ���������θ� ���

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

		const wstring& AsString() const	{	return string_value;		}
		bool AsBool() const			{		return bool_value;		}
		double AsDouble() const		{		return double_value;		}	//double AsNumber() const;
		int AsInt() const			{		return int_value;		}
		__int64 AsInt64() const		{		return int64_value;		}
		ShJArr AsArray()			{		return array_value;		} //�տ� const ����. array���� �Ϸ���
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
		//wstring _key;// ���� ���� map �� � Ű�� setat �Ǿ���.


					  //std::vector<std::wstring> _keyOrder;// isObject�� ��� key�� ó�� ������ � ������ ��� ����.
		//DWORD_PTR _uiData{ 0 }; // HTREEITEM

		ShJVal parent{ nullptr };
		JsonType type{ JsonType_Null };

		wstring string_value;
		bool    bool_value{ false };
		double  double_value{ -1. };
		int     int_value{ -1 };
		__int64 int64_value{ -1 };// 32bit ������ int�� ���� 4byte ������, 64bit ������ 8byte�̴�.
// 		unsigned int     uint_value{ -1 };
// 		unsigned __int64 uint64_value{ -1 };// 32bit ������ int�� ���� 4byte ������, 64bit ������ 8byte�̴�.

		ShJArr array_value;//������ �����Ͱ� [,,,] array �� ���
		ShJObj object_value;
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

