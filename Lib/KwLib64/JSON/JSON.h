/*
 * File JSON.h part of the SimpleJSON Library - http://mjpa.in/json
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

// #ifndef _JSON_H_
// #define _JSON_H_

// Win32 incompatibilities
#if defined(WIN32) && !defined(__GNUC__)
	#define wcsncasecmp _wcsnicmp
	static inline bool isnan(double x) { return x != x; }
	static inline bool isinf(double x) { return !isnan(x) && isnan(x - x); }
#endif



#include <vector>
#include <string>
#include <map>


// Linux compile fix - from quaker66
#ifdef __GNUC__
	#include <cstring>
	#include <cstdlib>
#endif

// Mac compile fixes - from quaker66, Lion fix by dabrahams
#if defined(__APPLE__) && __DARWIN_C_LEVEL < 200809L || (defined(WIN32) && defined(__GNUC__)) || defined(ANDROID)
	#include <wctype.h>
	#include <wchar.h>
	
	static inline int wcsncasecmp(PS s1, PS s2, size_t n)
	{
		int lc1  = 0;
		int lc2  = 0;
		while (n--)
		{
			lc1 = towlower (*s1);
			lc2 = towlower (*s2);

			if (lc1 != lc2)
				return (lc1 - lc2);
			if (!lc1)
				return 0;
			++s1;
			++s2;
		}
		return 0;
	}
#endif




#ifndef PS
	typedef const wchar_t* PS;
	typedef const char* PAS;
#endif






// Simple function to check a string 's' has at least 'n' characters
static inline bool simplejson_wcsnlen(PS s, size_t n) {
	if (s == 0)
		return false;

	PS save = s;
	while (n-- > 0)
	{
		if (*(save++) == 0) return false;
	}

	return true;
}



#ifndef LPCWSTR
typedef const wchar_t* LPCWSTR;
#endif

#ifndef PWS
typedef	LPCWSTR PWS;
#endif
// Custom types
class JSONValue;

typedef std::wstring JSonKey;
typedef std::wstring JStr;
typedef std::vector<std::wstring> JStrArray;

//typedef std::vector<JSONValue*> JSONArray;
class JSONArray : public std::vector<JSONValue*>
{
public:
	bool _bValueOwner{ true };
	~JSONArray();
	void DeleteAll();//_bValueOwner
	void DeleteValues();//_bValueOwner
	void AddMove(JSONValue*&& jv);//test전
	void Add(JSONValue* jv);
};
//typedef std::map<std::wstring, JSONValue*> JSONObject;
class JSONObject : public std::map<std::wstring, JSONValue*>
{
public:
	JSONObject()
	{
	}
	~JSONObject();




	void DeleteAll();//_bValueOwner
	void DeleteValues();//_bValueOwner

	//new JSONValue(object)에서 전달된 객체의 value인 JSONValue*는 그대로 복사 되는게 아니라 참조 되므로, 
	// 기존객체가 그 value의 오너를 포기 해야 한다.
	bool _bValueOwner{ true };//?deprecated free 하면 오류
	//읽은 순서 보존을 위해. nfmJson 에만 적용. map도 원  소스에서 순서 보전을 위해
	JStrArray _keys;
	// SetAt을 쓴경우만 순서시스템에 작동. 삭제도 반드시 DeleteKey를 사용해야.
	void SetAt(PWS name, JSONValue* val);
	void DeleteKey(PWS name);
	int FindOnArray(PWS name);


	template<typename T>
	void for_loop(T lambda)
	{
		//for(auto& [k, v] : *this) //c++17
		//	lambda(k, v);

		JSONObject::iterator iter; 
		for (iter = this->begin(); iter != this->end(); iter++)
		{
			JSonKey name = (*iter).first;
			auto val = (*iter).second;
			lambda((*iter).first, *(*iter).second);
			//+name	L"levelRtrace"	std::wstring
			//+ val	0x000000000333b930 {_T = L"" _text = L"" _uiData = 0 ...}	JSONValue*
		} 
	}
};
//typedef std::map<std::string, JSONValue*> JSONObjectA;













#include "JSONValue.h"




class JSON
{
	friend class JSONValue;

public:
	static JSONValue* Parse(PAS data);
	static JSONValue* Parse(PS data);
	static JStr Stringify(const JSONValue *value);
	static bool SkipWhitespace(PS *data);
	static bool ExtractString(PS *data, JStr &str);
	static double ParseInt(PS *data);
	static double ParseDecimal(PS *data);
private:
	JSON();
};








//#endif
