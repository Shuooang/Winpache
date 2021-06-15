/*
 * File JSONValue.h part of the SimpleJSON Library - http://mjpa.in/json
 *
 * Copyright (C) 2010 Mike Anchor
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once
// #ifndef _JSONVALUE_H_
// #define _JSONVALUE_H_

#include <vector>
#include <string>

#include "../tchtool.h"
#include "../KBinary.h"
#include "../StrBuffer.h"
#include "../KVal.h"

#include "JSON.h"

class JSON;

enum JSONType 
{ 
	JSONType_Null, 
	JSONType_String, 
	JSONType_Bool, 
	JSONType_Double, //JSONType_Number,
	JSONType_Array, 
	JSONType_Object,
	
	JSONType_Int,
	JSONType_Int64,
};

//class CAtBuf
//{
//public:
//	wchar_t* rp;
//	CAtBuf(wchar_t* p = NULL)
//		: rp(p)
//	{}
//	~CAtBuf()
//	{
//		if(rp)
//			delete rp;
//	}
//};

class CJSonObj;

class CJsonUnit
{
public:
	CJSonObj* m_th;
	PS m_k;
	CJsonUnit(CJSonObj* th, PS k) : m_th(th), m_k(k)
	{
	}
// 	CJsonUnit(CJsonPbj* th, PS k) : m_th(th->get()), m_k(k)
// 	{
// 	}
	void operator=(const wchar_t * v);
	//	void operator=(int v);
	void operator=(double v);
	//	void operator=(CJSonObj& v);
	void operator=(JSONObject& v);
};




class CJSonObj : public JSONObject
{
public:
	CJsonUnit operator()(PS k)
	{
		return CJsonUnit(this, k);
	}
	~CJSonObj()
	{
	}
	// 	void Set(PS k, PS v) { (*this)[k] = new JSONValue(v); }
	// 	void Set(PS k, int v) { (*this)[k] = new JSONValue(v); } 
	// 	void Set(PS k, double v) { (*this)[k] = new JSONValue(v); }
	bool Has(PS k)
	{
		return this->find(k) != this->end();
	}
};

class CJsonPbj;
class CJsonArray;

class CJsonPbjUnit
{
public:
	CJsonPbj* m_pCJobj;
	CString m_sKey;
	PS m_k;
	CJsonPbjUnit(CJsonPbj* th, PS k) 
		: m_pCJobj(th)//, m_k(k)
	{
		m_sKey = k;
		m_k = (PS)m_sKey;
	}
	void operator=(const wchar_t* v);
	void operator=(const CString& v);
	void operator=(const char * v);
	void operator=(__int64 v);
	void operator=(int v);
	void operator=(double v);//error C2593: 'operator ='이(가) 모호합니다. 0 일때만 그르네
	//	void operator=(CJSonObj& v);
	void operator=(JSONObject& v);
	void operator=(CJsonPbj& v);
	void operator=(CJsonArray& v);
	void operator=(JSONArray& v);
	void operator=(ONULL& v);
};

class CJsonArray
{
public:
	JSONArray* m_pJarr{ nullptr };
	CJsonArray(const JSONArray* jobj = NULL);
	CJsonArray(const JSONArray& jobj)
		: _bOwner(FALSE)
	{
		m_pJarr = (JSONArray*)&jobj;
	}
	~CJsonArray() {};
	BOOL _bOwner{ TRUE };
	JSONArray* get() { return m_pJarr; }
	void Add(CJsonPbj& obj);
	void Add(CJsonArray& arr);
	void Add(JSONArray& arr);
	void Add(JSONObject& obj);

	JSONValue* operator[](int i) { return (*m_pJarr)[i]; }
	//ex: 	auto jxv = jar[r];
	//		CJsonPbj jx(&jxv->AsObject());

};
class CJsonPbj
{
public:
	JSONObject* m_pJobj{ nullptr };
	CJsonPbj(const JSONObject* jobj = NULL);
	CJsonPbj(const JSONObject& jobj)
		: _bOwner(FALSE)
	{
		m_pJobj = (JSONObject*)&jobj;
	}
	~CJsonPbj();
	BOOL _bOwner{ TRUE };
	JSONObject* get()		{	return m_pJobj;	}
	JSONValue* getAt(PWS k) { return (*m_pJobj)[k]; }
	JSONValue* operator[](PWS k) { return (*m_pJobj)[k]; }
	JSONValue* operator[](PAS k) { CStringW kw(k); return (*m_pJobj)[(PWS)kw]; }

	void Import(const JSONObject& src);
	void Clone(const CJsonPbj& src);
	CStrBufferT<CString, LPCTSTR> _buf;
	
	void SetMove(PWS k, CJsonPbj& src);
	
	///새로만들 JObj와 호환성을 위해
	size_t size(){	return m_pJobj->size();	}

	PWS Ptr( PWS k );
	JStr String(PS k);
	PWS S(PWS k);
	PWS QS(PWS k, BOOL bNullIfEmpty = TRUE, BOOL bQuat = TRUE, BOOL bNecessary = FALSE);
	PWS QS(PAS k, BOOL bNullIfEmpty = TRUE, BOOL bQuat = TRUE)
	{
		CStringW sw(k);
		return QS(sw, bNullIfEmpty, bQuat);
	}
	PWS S(PAS k)
	{
		CStringW sw(k);
		return S((PWS)sw);
	}
	CTime T(PAS k);
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
	PWS QN(PAS k, int underDot = 0)
	{
		CStringW sw(k);
		return QN(sw, underDot);
	}
	int I(PAS k)
	{
		CStringW sw(k);
		return (int)N((PWS)sw);//double이 기본이지만 정수인지 확신할때
	}
	const JSONArray* Array(PWS k);
	const JSONArray* Array(PAS k)
	{
		CStringW sw(k);
		return Array((PWS)sw);
	}

	JSONObject* Obj(PWS k);
	JSONObject* O(PAS k)
	{
		CStringW sw(k);
		return Obj((PWS)sw);
	}
	JSONObject* operator->() const throw()
	{
		ATLASSERT(m_pJobj != NULL);
		return m_pJobj;
	}
	CJsonPbjUnit operator()(PS k)
	{
		return CJsonPbjUnit(this, k);
	}
	CJsonPbjUnit operator()(PAS k)
	{
		CStringW kw(k);
		return CJsonPbjUnit(this, kw);
	}
	void operator=(CJsonPbj& v)
	{
		*m_pJobj = *v.m_pJobj;//clone해야 하는데,..
	}

	bool Has(PS k)
	{
		ATLASSERT(m_pJobj != NULL);
		return m_pJobj->find(k) != m_pJobj->end();
	}
	bool Has(PAS k)
	{
		CStringW kw(k);
		return Has(kw);
	}
	bool IsArray(PWS k);
	bool IsArray(PAS k)
	{
		CStringW kw(k);
		return IsArray(kw);
	}
	bool IsObject(PWS k);
	bool IsObject(PAS k)
	{
		CStringW kw(k);
		return IsObject(kw);
	}

	bool IsString(PWS k);
	bool IsString(PAS k)
	{
		CStringW kw(k);
		return IsString(kw);
	}
	bool IsNumber(PWS k);
	bool IsDouble(PAS k);
	bool IsInt(PAS k);
	bool IsInt64(PAS k);
	bool IsNumber(PAS k)
	{
		CStringW kw(k);
		return IsNumber(kw);
	}


	CStringW ToJsonStringW();
	CStringA ToJsonStringUtf8();
	KBinary ToJsonData();

	bool CopyFielsIf(CJsonPbj& src, PWS key);

	// T=CHttpSmo::AddPolicy::<lambda_e6123556f550d27e351a1e5e7f52f328>
	template<typename T>
	void for_loop(T lambda)
	{
		m_pJobj->for_loop(lambda);
	}

	int CopyFieldsAll(CJsonPbj& src);
};

class IStrConvert
{
public:
	virtual const wchar_t* CharToString(const wchar_t* key, wchar_t ch) = NULL;
	//virtual bool CheckIfCharConv(const wchar_t* key) = NULL;
};

class JSONValue 
{
	friend class JSON;

	public:
		//bool _bOwner{ false }; 
		/// JSONArray, JSONObject 가 자체적으로 내부 값 delete 하는 _bValueOwner를 가지고 delete 한다.
		/// 그래서 필요 없다.
		JSONValue(/*NULL*/);
		JSONValue(const wchar_t *char_value1);
		JSONValue(const JSonKey &string_value1);
		JSONValue(bool bool_value1);
		JSONValue(double number_value1);
		JSONValue(int int_value1);
		JSONValue(__int64 int64_value1);
		JSONValue(const JSONArray &array1);
		JSONValue(const JSONObject& obj1);// , JStrArray& array_key1);
		//JSONValue(const JSONObject* object_value1);// , JStrArray& array_key1);
		JSONValue(const JSONValue &source1);
		~JSONValue();
		void operator=(JSONValue& jv);

		void WrappObj(JSONObject& obj1);
		void MoveObj(JSONObject& obj1);

		void MoveArray(JSONArray& arr1);
		// void WrappArray(JSONArray& array1); 이건 나중에 필요 할때, 


		bool IsNull() const;
		bool IsString() const;
		bool IsBool() const;
		bool IsNumber() const;
		bool IsDouble() const;
		bool IsInt64() const;
		bool IsInt() const;
		bool IsArray() const;
		bool IsObject() const;

		const JSonKey &AsString() const;
		bool AsBool() const;
		double AsDouble() const;
		//double AsNumber() const;
		int AsInt() const;
		__int64 AsInt64() const;
		JSONArray &AsArray(); //앞에 const 없앰. array편집 하려고
		JSONObject &AsObject();

		void Clone(const JSONValue& src);
		static void CloneArray(const JSONArray& src, JSONArray& tar); //JSONArray는 std::vector 이므로 자체 Clone이 없다.
		static void CloneObject(const JSONObject& source, JSONObject& tar);

		std::size_t CountChildren() const;
		bool HasChild(std::size_t index) const;
		JSONValue *Child(std::size_t index);
		bool HasChild(const wchar_t* name) const;
		JSONValue *Child(const wchar_t* name);
		JStrArray ObjectKeys() const;
		// bUnicode: \u????
		JSonKey Stringify(const bool bUnicode = true, bool const prettyprint = false, const wchar_t* key = NULL, IStrConvert* pinf = NULL) const;
	protected:
		static JSONValue *Parse(const wchar_t **data);

	private:
		static JSonKey StringifyString(const bool bUnicode, const JSonKey &str, const wchar_t* key, IStrConvert* pinf = NULL);
		JSonKey StringifyImpl(const bool bUnicode, size_t const indentDepth, const wchar_t* key = NULL, IStrConvert* pinf = NULL) const;
		static JSonKey Indent(size_t depth);

public:
	/// /////////////////////////////////////////////////////////////////////////
	JSonKey _key;// 나는 상위 map 에 어떤 키로 setat 되었다.
	JSonKey _text;// ui에 보여지는 text로 디버깅용으로만 사용
		//std::vector<std::wstring> _keyOrder;// isObject인 경우 key는 처음 읽을때 어떤 순서로 들어 갔다.
	DWORD_PTR _uiData{ 0 }; // HTREEITEM

	JSONValue* parent{ nullptr };
	JSONType type{ JSONType_Null };

	JSonKey string_value;
	bool bool_value{ false };
	double double_value{ -1. };
	int int_value{ -1 };
	__int64 int64_value{ -1 };// 32bit 에서는 int와 같이 4byte 이지만, 64bit 에서는 8byte이다.
		
	JSONArray array_value;//실제로 데이터가 [,,,] array 인 경우
	JSONObject object_value;
	//JStrArray array_key;//map 인 경우 원래 소스의 순서를 기억 하기 위해. 삭제 하거나 추가 할경우 고려 해야. 또한 순서를 바꿀수도 있다.
	/// /////////////////////////////////////////////////////////////////////////



	void SetKey(std::wstring key);
	void SetKey(PWS key) { _key = key; }
	void SetData(DWORD_PTR data) { _uiData = data; }
	PWS GetKey() { return _key.c_str(); }
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
	int setValue(JSONValue* snd1);
	double CompareValue(JSONValue* snd1);
	int IsSameValue(JSONValue* snd1);
	
	std::wstring GetText(int maxlen = 1024);

};

class CJSONValue
{
public:
	JSONValue *m_pJdoc;
	int m_nRef;
	int* m_pNRef;
	CJSONValue(JSONValue *pJdoc = NULL)
		: m_pJdoc(pJdoc)
		, m_nRef(0)
		, m_pNRef(&m_nRef)
	{
// 		CJSONValue jdoc0 = JSON::Parse(resw);//constructor가 불린다.
// 		CJSONValue jdoc(JSON::Parse(resw));
	}
	CJSONValue(CJSONValue& Jdoc)
		: m_pJdoc(Jdoc.m_pJdoc)
		, m_nRef(-1)
	{
//		CJSONValue jdoc2 = jdoc;//constructor가 불린다. 없으면 디폴트 assign이 되어 operator는 안불린다.
		Jdoc.m_nRef++;
		m_pNRef = Jdoc.m_pNRef;
	}
	~CJSONValue()
	{
		if((*m_pNRef) == 0)
			DeleteMeSafe(m_pJdoc);
		else
			(*m_pNRef)--;
	}
	JSONValue *getJson()
	{
		return m_pJdoc;
	}

	JSONValue* operator->() const throw()
	{
		ATLASSERT(m_pJdoc != NULL);
		return m_pJdoc;
	}
	void operator=(CJSONValue& Jdoc)
	{
// 		CJSONValue jdoc3;
//		jdoc3 = jdoc;
		ATLASSERT(m_pJdoc == NULL);
		m_pJdoc = Jdoc.m_pJdoc;
		Jdoc.m_nRef++;
		m_pNRef = Jdoc.m_pNRef;
	}
	void operator=(JSONValue *pJdoc)
	{
		ATLASSERT(m_pJdoc == NULL);
		m_pJdoc = pJdoc;
		m_nRef = 0;
	}
};




#define JSPUT(val) js(#val) = val
#define JSGETS(val) val = js.S(#val)
#define JSGETN(val) val = js.N(#val)
#define JSGETI(val) val = js.I(#val)




//#endif
