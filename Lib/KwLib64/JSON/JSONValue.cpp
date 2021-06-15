/*
 * File JSONValue.cpp part of the SimpleJSON Library - http://mjpa.in/json
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
#include "pch.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <math.h>

#include "../KBinary.h"
#include "../Kw_tool.h"
#include "../inlinefnc.h"
#include "../TimeTool.h"

#include "JSON.h"
#include "JSONValue.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Macros to free an array/object

#define FREE_ARRAY(x) { JSONArray::iterator iter; for (iter = x.begin(); iter != x.end(); iter++) { delete *iter; } }
#define FREE_OBJECT(x) \
{\
	JSONObject::iterator iter; \
	for (iter = x.begin(); iter != x.end(); iter++) \
	{\
		delete (*iter).second; \
	} \
}


void CJsonUnit::operator=(double v)
{
	(*m_th)[m_k] = new JSONValue(v);
}
void CJsonUnit::operator=(JSONObject& v)
{
	(*m_th)[m_k] = new JSONValue(v);
}
void CJsonUnit::operator=(const wchar_t* v)
{
	if(v)
		(*m_th)[m_k] = new JSONValue(v);
	else
		(*m_th)[m_k] = new JSONValue();
}





void CJsonPbjUnit::operator=(const char* v)
{
	JSONObject* pjo = m_pCJobj->get();
	CStringW vw(v);
	if(v)
		(*pjo)[m_k] = new JSONValue(vw);
	else
		(*pjo)[m_k] = new JSONValue();
}

void CJsonPbjUnit::operator=(const CStringW& v)
{
	operator=((PS)v);
	// 	JSONObject* pjo = m_pCJobj->get();
	// 	(*pjo)[m_k] = new JSONValue((PS)v);
}
void CJsonPbjUnit::operator=(const wchar_t* v)
{
	JSONObject* pjo = m_pCJobj->get();
	if (v)
	{
		//JSONValue jv(v);
		(*pjo)[m_k] = new JSONValue(v); //&jv; new로 안주고 스택변수 주면 내부에서 에러 난다.
	}
	else
		(*pjo)[m_k] = new JSONValue();
}
//error C2593: 'operator ='이(가) 모호합니다.
void CJsonPbjUnit::operator=(__int64 v)
{
	(*m_pCJobj->get())[m_k] = new JSONValue((__int64)v);
}
void CJsonPbjUnit::operator=(int v)
{
	(*m_pCJobj->get())[m_k] = new JSONValue((int)v);
}

void CJsonPbjUnit::operator=(double v)
{
	(*m_pCJobj->get())[m_k] = new JSONValue(v);
}
void CJsonPbjUnit::operator=(JSONObject& v)
{
	(*m_pCJobj->get())[m_k] = new JSONValue(v);
}

void CJsonPbjUnit::operator=(CJsonPbj& v)
{
	(*m_pCJobj->get())[m_k] = new JSONValue(*v.get());
}
void CJsonPbjUnit::operator=(CJsonArray& v)
{
	(*m_pCJobj->get())[m_k] = new JSONValue(*v.get());
}
void CJsonPbjUnit::operator=(JSONArray& v)
{
	(*m_pCJobj->get())[m_k] = new JSONValue(v);
}
void CJsonPbjUnit::operator=(ONULL& v)
{
	auto jv = new JSONValue();
	(*m_pCJobj->get())[m_k] = jv;
}





/**
 * Parses a JSON encoded value to a JSONValue object
 *
 * @access protected
 *
 * @param wchar_t** data Pointer to a wchar_t* that contains the data
 *
 * @return JSONValue* Returns a pointer to a JSONValue object on success, NULL on error
 */
JSONValue *JSONValue::Parse(const wchar_t **data)
{
	try
	{
		// Is it a string?
		if(**data == '"')
		{
			JSonKey str;
			if(!JSON::ExtractString(&(++(*data)), str))
			{
#ifdef _DEBUG
				auto v1 = data;
				auto v2 = *data;
				auto v3 = (*data) + 1;
				//auto v4 = &((*data) + 1);
				JSON::ExtractString(&((*data)), str);
#endif
				//++(*data);
				throw (int)__LINE__;// return NULL;
			}
			else
			{
				//++(*data);
				return new JSONValue(str);
			}
		}
		else if((simplejson_wcsnlen(*data, 4) && wcsncasecmp(*data, L"true", 4) == 0) || (simplejson_wcsnlen(*data, 5) && wcsncasecmp(*data, L"false", 5) == 0))
		{// Is it a boolean?
			bool value = wcsncasecmp(*data, L"true", 4) == 0;
			(*data) += value ? 4 : 5;
			return new JSONValue(value);
		}
		else if(simplejson_wcsnlen(*data, 4) && wcsncasecmp(*data, L"null", 4) == 0)
		{// Is it a null?
			(*data) += 4;
			return new JSONValue();
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
				number = JSON::ParseInt(data);
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
				double decimal = JSON::ParseDecimal(data);

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
				double expo = JSON::ParseInt(data);
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
					return new JSONValue((int)number);
				}
				else
				{
					return new JSONValue((__int64)number);
				}
			}
			return new JSONValue(number);
		}
		else if(**data == L'{')// An object?
		{
#define CHECK_FREE(stErr) if(stErr) {FREE_OBJECT(object);throw (int)__LINE__;}// return NULL;}

			CJSonObj object;//std::map<std::wstring, JSONValue*>
			//JStrArray array_key;
			(*data)++;
			while(**data != 0)
			{
				// Whitespace at the start?
				CHECK_FREE(!JSON::SkipWhitespace(data));

				// Special case - empty object
				if(object.size() == 0 && **data == L'}')
				{
					(*data)++;
					auto jv = new JSONValue();
					jv->MoveObj(object);//냉무라도 통상 Move한다.
					return jv;
					//return new JSONValue(object);
				}

				// We want a string now...
				JSonKey name;
				CHECK_FREE(!JSON::ExtractString(&(++(*data)), name));
#ifdef _DEBUG
				if(wcscmp((const wchar_t*)name.c_str(), L"Script") == 0)
				{
					auto key = name.c_str();
				}
#endif // _DEBUG
				// More whitespace?
				CHECK_FREE(!JSON::SkipWhitespace(data));

				// Need a : now
				CHECK_FREE(*((*data)++) != L':');

				// More whitespace?
				CHECK_FREE(!JSON::SkipWhitespace(data));

				// The value is here
				JSONValue* value = Parse(data);
				if(value == NULL)
				{
					value = Parse(data);
					FREE_OBJECT(object);
					throw (int)__LINE__;// return NULL; 
				}//	CHECK_FREE (value == NULL);

				// Add the name:value
				if(object.find(name) != object.end())
					object.DeleteKey(name.c_str());
				//delete object[name];

				//value->parent = this;
				value->SetKey(name);
				object.SetAt(name.c_str(), value);////////////////////////////// insert ////////////////////////////////////////
				//array_key.push_back(name);
				auto key1 = value->GetKey();

				// More whitespace?
				CHECK_FREE(!JSON::SkipWhitespace(data));

				// End of object?
				if(**data == L'}')
				{
					(*data)++;
					//object._bValueOwner = false;
					auto jv = new JSONValue();
					jv->MoveObj(object);
					 return jv;
				}

				// Want a , now
				CHECK_FREE(**data != L',');

				(*data)++;
			}

			// Only here if we ran out of data
			CHECK_FREE(1);
		}
		else if(**data == L'[')// An array?
		{
			JSONArray array;

			(*data)++;

#define CHECK_ARRAY(bVal) if(bVal) {FREE_ARRAY(array);throw (int)__LINE__;}// return NULL;}
#define CHECK_ARRAY2(bVal) if(bVal) {array.DeleteAll();throw (int)__LINE__;}// return NULL;}
			while(**data != 0)
			{
				// Whitespace at the start?
				CHECK_ARRAY(!JSON::SkipWhitespace(data));

				// Special case - empty array
				if(array.size() == 0 && **data == L']')
				{
					(*data)++;
					auto jv = new JSONValue();
					jv->MoveArray(array);
					return jv;
					//return new JSONValue(array);
				}

				// Get the value
				JSONValue* value = Parse(data);
				CHECK_ARRAY(value == NULL);

				// Add the value
				array.push_back(value);

				// More whitespace?
				CHECK_ARRAY(!JSON::SkipWhitespace(data));

				// End of array?
				if(**data == L']')
				{
					(*data)++;
					auto jv = new JSONValue();
					jv->MoveArray(array);
					return jv;
					//return new JSONValue(array);
				}

				// Want a , now
				CHECK_ARRAY(**data != L',');

				(*data)++;
			}

			// Only here if we ran out of data
			CHECK_ARRAY(1);
		}
		else// Ran out of possibilites, it's bad!
		{
			throw (int)__LINE__;// return NULL;
		}
	}
	catch(int )//line)
	{
		return NULL;//(int)__LINE__ cast를 해야 한다. 
	}/*
	catch(UINT line)
	{
		return NULL;
	}
	catch(INT_PTR line)
	{
		return NULL;
	}
	catch(UINT_PTR line)
	{
		return NULL;
	}
	catch(...)
	{
		return NULL;
	}*/
}

JSONValue::JSONValue()
{
	type = JSONType_Null;
}

JSONValue::JSONValue(const wchar_t *char_value1)
{
	type = JSONType_String;
	string_value = JSonKey(char_value1);
}

JSONValue::JSONValue(const JSonKey &string_value1)
{
	type = JSONType_String;
	string_value = string_value1;
}

JSONValue::JSONValue(bool mbool_value)
{
	type = JSONType_Bool;
	bool_value = mbool_value;
}

JSONValue::JSONValue(double value1)
{
	type = JSONType_Double;
	double_value = value1;
}
JSONValue::JSONValue(int value1)
{
	type = JSONType_Int;
	int_value = value1;
}
JSONValue::JSONValue(__int64 value1)
{
	type = JSONType_Int64;
	int64_value = value1;
}

JSONValue::JSONValue(const JSONArray &marray_value)
{
	type = JSONType_Array;
	//array_value = marray_value;
	JSONValue::CloneArray(marray_value, array_value);
}

/// object value가 clone된다.
JSONValue::JSONValue(const JSONObject& obj1)//, JStrArray& array_key1)
{
	type = JSONType_Object;
	JSONValue::CloneObject(obj1, object_value);
}

/// this가 object value의 남의 객체에 껍질만 보유하여 free되지 않는다.
void JSONValue::WrappObj(JSONObject& obj1)//, JStrArray& array_key1)
{
	type = JSONType_Object;
	object_value = obj1;
	object_value._bValueOwner = false;// 포인터 소유권이 없다.JSONValue가 껍데기만 쓴다.
}

/// 다른 object value가 옮겨 와서 소유권도 가져 오며, 그 남의 객체는 껍질만 보유하며 free되지 않는다.
void JSONValue::MoveObj(JSONObject& obj1)//, JStrArray& array_key1)
{
	type = JSONType_Object;
	object_value = obj1;
	object_value._bValueOwner = true;// 포인터 소유권이 옮겨간다.
	obj1._bValueOwner = false;// 포인터 소유권이 없다.JSONValue가 껍데기만 쓴다.
}

void JSONValue::MoveArray(JSONArray& arr1)//, JStrArray& array_key1)
{
	type = JSONType_Array;
	array_value = arr1;
	array_value._bValueOwner = true;// 포인터 소유권이 옮겨간다.
	arr1._bValueOwner = false;// 포인터 소유권이 없다.JSONValue가 껍데기만 쓴다.
}

/// 기본적으로 Clone되니 효율을 위한 다면 Move???? 나 Wrapp???을 써야 한다.
JSONValue::JSONValue(const JSONValue &msource)
{
	this->Clone(msource);
}

JSONValue::~JSONValue()
{
}

bool JSONValue::IsNull() const
{
	return type == JSONType_Null;
}
bool JSONValue::IsString() const
{
	return type == JSONType_String;
}
bool JSONValue::IsBool() const
{
	return type == JSONType_Bool;
}
bool JSONValue::IsNumber() const
{
	return IsDouble() || IsInt() || IsInt64();// type == JSONType_Number;
}
bool JSONValue::IsDouble() const
{
	return type == JSONType_Double;
}
bool JSONValue::IsInt64() const
{
	return type == JSONType_Int64;
}
bool JSONValue::IsInt() const
{
	return type == JSONType_Int;
}
bool JSONValue::IsArray() const
{
	return type == JSONType_Array;
}
bool JSONValue::IsObject() const
{
	return type == JSONType_Object;
}
const JSonKey &JSONValue::AsString() const
{
	return string_value;
}
bool JSONValue::AsBool() const
{
	return bool_value;
}
double JSONValue::AsDouble() const
{
	return double_value;
}
int JSONValue::AsInt() const
{
	return int_value;
}
__int64 JSONValue::AsInt64() const
{
	return int64_value;
}
JSONArray &JSONValue::AsArray()
{
	return array_value;
}
JSONObject &JSONValue::AsObject()
{
	return object_value;
}

void JSONValue::Clone(const JSONValue& msource)
{
	parent = msource.parent;
	type = msource.type;

	switch(type)
	{
	case JSONType_String:
		string_value = msource.string_value;
		break;
	case JSONType_Bool:
		bool_value = msource.bool_value;
		break;
	case JSONType_Double:
		double_value = msource.double_value;
		break;
	case JSONType_Int64:
		int64_value = msource.int64_value;
		break;
	case JSONType_Int:
		int_value = msource.int_value;
		break;
	case JSONType_Array:
	{
		CloneArray(msource.array_value, array_value);
	}
	case JSONType_Object:
	{
		CloneObject(msource.object_value, object_value);
	}
	case JSONType_Null:		// Nothing to do.
		break;
	}
}
void JSONValue::CloneObject(const JSONObject& source, JSONObject& tar)
{
	for(auto& [k, v] : source)
	{
		auto pval = new JSONValue((JSONValue&)*v);//v를 cast안하면 엉뚱한것이불려진다.
		tar[k] = pval;// v가 object 이거나 array면 딮카피 된다.
	}
}

/// JSONArray는 std::vector 이므로 자체 Clone이 없다.
void JSONValue::CloneArray(const JSONArray& source_array, JSONArray& tar)
{
	for(auto& jval : source_array)
	{
		auto pval = new JSONValue(*jval);
		/// 이게 pval->Clone(item); 이므로 내부적으로 deep copy가 이루어 진다.
		pval->parent = jval->parent;
		tar.push_back(pval);
	}
}
std::size_t JSONValue::CountChildren() const
{
	switch (type)
	{
		case JSONType_Array:
			return array_value.size();
		case JSONType_Object:
			return object_value.size();
		default:
			return 0;
	}
}

bool JSONValue::HasChild(std::size_t index) const
{
	if (type == JSONType_Array)
	{
		return index < array_value.size();
	}
	else
	{
		return false;
	}
}

JSONValue *JSONValue::Child(std::size_t index)
{
	if (index < array_value.size())
	{
		return array_value[index];
	}
	else
	{
		return NULL;
	}
}
bool JSONValue::HasChild(const wchar_t* name) const
{
	if (type == JSONType_Object)
	{
		return object_value.find(name) != object_value.end();
	}
	else
	{
		return false;
	}
}
JSONValue* JSONValue::Child(const wchar_t* name)
{
	JSONObject::const_iterator it = object_value.find(name);
	if (it != object_value.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}

/**
 * Retrieves the keys of the JSON Object or an empty vector
 * if this value is not an object.
 *
 * @access public
 *
 * @return std::vector<std::wstring> A vector containing the keys.
 */
JStrArray JSONValue::ObjectKeys() const
{
	JStrArray keys;

	if (type == JSONType_Object)
	{
		JSONObject::const_iterator iter = object_value.begin();
		while (iter != object_value.end())
		{
			keys.push_back(iter->first);
			iter++;
		}
	}

	return keys;
}

/**
 * Creates a JSON encoded string for the value with all necessary characters escaped
 *
 * @access public
 *
 * @param bool prettyprint Enable prettyprint
 *
 * @return std::wstring Returns the JSON string
 */
std::wstring JSONValue::Stringify(const bool bUnicode, bool const prettyprint, const wchar_t* key, IStrConvert* pinf) const
{
	size_t const indentDepth = prettyprint ? 1 : 0;
	return StringifyImpl(bUnicode, indentDepth, key, pinf);
}


/**
 * Creates a JSON encoded string for the value with all necessary characters escaped
 *
 * @access private
 *
 * @param size_t indentDepth The prettyprint indentation depth (0 : no prettyprint)
 *
 * @return std::wstring Returns the JSON string
 */
std::wstring JSONValue::StringifyImpl(const bool bUnicode, size_t const indentDepth, const wchar_t* key, IStrConvert* pinf) const
{
	std::wstring ret_string;
	size_t const indentDepth1 = indentDepth ? indentDepth + 1 : 0;
	std::wstring const indentStr = Indent(indentDepth);
	std::wstring const indentStr1 = Indent(indentDepth1);

	switch (type)
	{
		case JSONType_Null:
			ret_string = L"null";
			break;

		case JSONType_String:
			ret_string = StringifyString(bUnicode, string_value, key, pinf);
			break;

		case JSONType_Bool:
			ret_string = bool_value ? L"true" : L"false";
			break;

		case JSONType_Double:
		{
			if(isinf(double_value) || isnan(double_value))
				ret_string = L"null";
			else
			{
				std::wstringstream ss;
				ss.precision(15);
				ss << double_value;
				ret_string = ss.str();
			}
			break;
		}

		case JSONType_Int64:
		{
			if(isinf((double)int64_value) || isnan((double)int64_value))
				ret_string = L"null";
			else
			{
				std::wstringstream ss;
				ss.precision(15);
				ss << int64_value;
				ret_string = ss.str();
			}
			break;
		}

		case JSONType_Int:
		{
			if(isinf((double)int_value) || isnan((double)int_value))
				ret_string = L"null";
			else
			{
				std::wstringstream ss;
				ss.precision(15);
				ss << int_value;
				ret_string = ss.str();
			}
			break;
		}

		case JSONType_Array:
		{
			ret_string = indentDepth ? L"[\n" + indentStr1 : L"[";
			JSONArray::const_iterator iter = array_value.begin();
			WCHAR wbuf[20] = {0,};
			for (int i=0;iter != array_value.end();i++)
			{
				_itow_s((int)i, wbuf, 19, 10); //KwItoaW(i, wbuf, 19)
				ret_string += (*iter)->StringifyImpl(bUnicode, indentDepth1, wbuf, pinf);

				// Not at the end - add a separator
				if (++iter != array_value.end())
					ret_string += L",";
			}
			ret_string += indentDepth ? L"\n" + indentStr + L"]" : L"]";
			break;
		}

		case JSONType_Object:
		{
			ret_string = indentDepth ? L"{\n" + indentStr1 : L"{";
			if(object_value.size() == object_value._keys.size())
			{
				JSONObject::const_iterator iter = object_value.begin();
				for(int i=0;i< object_value._keys.size();i++) //원래 순서를 유지하기 위해 배열에 키넣어 두었다가 지금 쓴다.
				{
					auto key = object_value._keys[i];
					bool bUseConvt = false;
					JSONValue* val = object_value.find(key)->second;
					ret_string += StringifyString(bUnicode, key, NULL);
					ret_string += L":";
					ret_string += val->StringifyImpl(bUnicode, indentDepth1, key.c_str(), pinf);// bUseConvt ? pinf : NULL);
					// Not at the end - add a separator
					if (++iter != object_value.end())
						ret_string += L",";
				}
			}
			else
			{
				JSONObject::const_iterator iter = object_value.begin();
				while (iter != object_value.end())
				{
					bool bUseConvt = false;
					if (pinf)
					{
						//bUseConvt = pinf->CheckIfCharConv((*iter).first.c_str());
					}
	#ifdef _DEBUGx
					if ((*iter).first == L"Script")
					{
						const wchar_t* pkey = (*iter).first.c_str();
						ASSERT(1);
					}
	#endif // _DEBUG
					auto second = (*iter).second;
					if (second)
					{
						ret_string += StringifyString(bUnicode, (*iter).first, NULL);
						ret_string += L":";
						ret_string += second->StringifyImpl(bUnicode, indentDepth1, (*iter).first.c_str(), pinf);// bUseConvt ? pinf : NULL);

					// Not at the end - add a separator
						if (++iter != object_value.end())
							ret_string += L",";
					}
					else
					{
						ret_string += L"";//중단점 줄려고 괜히
					}
				}
			}
			ret_string += indentDepth ? L"\n" + indentStr + L"}" : L"}";
			break;
		}
	}

	return ret_string;
}

/**
 * Creates a JSON encoded string with all required fields escaped
 * Works from http://www.ecma-internationl.org/publications/files/ECMA-ST/ECMA-262.pdf
 * Section 15.12.3.
 *
 * @access private
 *
 * @param std::wstring str The string that needs to have the characters escaped
 *
 * @return std::wstring Returns the JSON string
 */
std::wstring JSONValue::StringifyString(const bool bUnicode, const std::wstring &str, const wchar_t* key, IStrConvert* pinf)
{
	std::wstring str_out = L"\"";

	std::wstring::const_iterator iter = str.begin();
	while (iter != str.end())
	{
		wchar_t chr = *iter;
		const wchar_t* prv = NULL;

		if (pinf)
		{
			//if (pinf->CheckIfCharConv(key))
			prv = pinf->CharToString(key, chr);
		}
		if (prv)
		{
			str_out += prv;
		}
		else
		{
			if (chr == L'"' ||  // " => \" 로
				chr == L'\\' || // \ => \\ 로
				chr == L'/')    // / => \/ 로 JSON표준(특이사항)
			{
				str_out += L'\\';
				str_out += chr;
			}
			else if (chr == L'\b')
				str_out += L"\\b";
			else if (chr == L'\f')
				str_out += L"\\f";
			else if (chr == L'\n')
				str_out += L"\\n";
			else if (chr == L'\r')
				str_out += L"\\r";
			else if (chr == L'\t')
				str_out += L"\\t";
			//else if (chr < L' ')
			//	str_out += L" ";
			else if (chr < L' ' || chr > 126)
			{// space 보다 적은게 뭐지?
				if (bUnicode)
				{
					str_out += L"\\u";
					for (int i = 0; i < 4; i++)
					{
						int value = (chr >> 12) & 0xf;
						if (value >= 0 && value <= 9)
							str_out += (wchar_t)('0' + value);
						else if (value >= 10 && value <= 15)
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
std::wstring JSONValue::Indent(size_t depth)
{
	const size_t indent_step = 2;
	depth ? --depth : 0;
	std::wstring indentStr(depth * indent_step, ' ');
	return indentStr;
}

void JSONValue::SetKey(std::wstring key)
{
	_key = key;
}

int JSONValue::setValue(JSONValue* snd1)
{
	if(this->type == snd1->type)
	{
		if(snd1->IsString())
			this->setString(snd1->AsString().c_str());
		else if(snd1->IsDouble())
			this->setDouble(snd1->AsDouble());
		else if(snd1->IsBool())
			this->setBool(snd1->AsBool());
		else
			return -1;
	}
	else
		return -2;
	return 0;
}

double JSONValue::CompareValue(JSONValue* snd1)
{
	double rv = -1;// object 인경우 -1 리턴
	if (this->type == snd1->type)
	{
		if (snd1->IsString())
			rv = wcscmp(snd1->AsString().c_str(), this->AsString().c_str());
		else if(snd1->IsDouble())
		{
			double drv = snd1->AsDouble() - this->AsDouble();
			rv = (drv == 0.) ? 0 : (drv > 0.) ? 1 : -1;
		}
		else if (snd1->IsBool())
			rv = (snd1->AsBool() ? 1:0) - (this->AsBool() ? 1:0);
	}
	return rv;
}

int JSONValue::IsSameValue(JSONValue* snd1)
{
	double drv = CompareValue(snd1);
	int rv = drv == 0. ? 1 : 0;//drv < 0 ? -1 : 1;// object 인경우 -1 리턴
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

std::wstring JSONValue::GetText(int maxlen)
{
	wchar_t* buf = new wchar_t[1024];//[1024];
	KAtEnd d_buf([&](){delete buf;});
	std::wstring sts;
	if(this->IsString())
	{

		auto wstr = this->AsString();
		if(wstr.length() == 0)
			return sts;

		wchar_t* sbuf = new wchar_t[wstr.length() + 8];//[1024];
		KAtEnd d_sbuf([&]() { delete sbuf; });
		if(wstr.length() > maxlen)
		{
			wstr = wstr.substr(0, 30);
			swprintf_s(sbuf, 1024, L"\"%s...\"", wstr.c_str());
		}
		else
			swprintf_s(sbuf, 1024, L"\"%s\"", wstr.c_str());
		sts = sbuf;
		return sts;
	}


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
	else
		swprintf_s(buf, 1024, L"(unknown)");
	sts = buf;
	return sts;
}

void JSONValue::operator=(JSONValue& jv)
{
	_key = jv._key;
	_text = jv._text;
	_uiData = jv._uiData;
	parent = jv.parent;
	type = jv.type;
	string_value = jv.string_value;
	bool_value = jv.bool_value;
	double_value = jv.double_value;
}

PS CJsonPbj::Ptr(PS k)
{
	if(m_pJobj == NULL)
		throw (L"CJsonPbj.m_pJobj == NULL");

	//?주의: if((*m_pJobj)[k]) 이걸 쓰는 쑨간 만들어져 버린다.
	if(Has(k))//m_pJobj->find(k) != m_pJobj->end())
	{
		if((*m_pJobj)[k]->IsString())
		{
			PS val = (*m_pJobj)[k]->AsString().c_str();
			return tchsame(val, L"null") ? L"" : val;
		}
		else if((*m_pJobj)[k]->IsNull())
		{
			return nullptr;
		}
		else
		{
			ASSERT(0);//문자열 일때만 요청해야 한다. 아니면 GetText 를 쓰던가.
		}
	}
	return NULL;
}

CJsonPbj::CJsonPbj(const JSONObject* jobj)
{
	if(jobj == NULL)
	{ 
		m_pJobj = new JSONObject();
		_bOwner = TRUE;
	}
	else {
		m_pJobj = (JSONObject*)jobj;
		_bOwner = FALSE;
	}
}

CJsonPbj::~CJsonPbj()
{
	if (_bOwner)
	{
		DeleteMeSafe(m_pJobj);
	}
}

void CJsonPbj::Import(const JSONObject& src)
{
	JSONValue::CloneObject(src, *this->m_pJobj);
}

void CJsonPbj::Clone(const CJsonPbj& src)
{
	JSONValue::CloneObject(*src.m_pJobj, *this->m_pJobj);
	//tar._bOwner = TRUE;
}

JStr CJsonPbj::String(PS k)
{
	if(m_pJobj == NULL)
		throw (L"CJsonPbj.m_pJobj == NULL");

	//?주의: if((*m_pJobj)[k]) 이걸 쓰는 쑨간 만들어져 버린다.
	if (Has(k)) //m_pJobj->find(k) != m_pJobj->end())
	{
		if((*m_pJobj)[k]->IsString())
		{
			JStr val = (*m_pJobj)[k]->AsString().c_str();
			return val;//tchsame(val, L"null") ? JStr.Empty() : val;
		}
	}
	return NULL;
}
PS CJsonPbj::S(PS k)
{
	PS s = Ptr(k);
	if (s == NULL)
		return L"";
	else
		return s;
}
CTime CJsonPbj::T(PAS k)
{
	CString s = S(k);
	if(s.GetLength() > 3)
	{
		CTime t = KwStdTimeToTime(s);
		return t;
	}
	return CTime(0);
}

/// <summary>
/// SQL 문에 데이터로 쓰일때 '%s' 대신 %s 만 써도 ' '를 붙여준다. 없는 경우 NULL 을 sql문에 맞게 준다.
/// </summary>
/// <param name="k"></param>
/// <param name="bNullIfEmpty">공백 ""을  </param>
/// <returns></returns>
PS CJsonPbj::QS(PS k, BOOL bNullIfEmpty, BOOL bQuat, BOOL bNecessary)
{
	CString& sbuf = _buf.GetBuf();
	//auto buf = sbuf.GetBuffer(lenBuf);

	if (Has(k))//m_pJobj->find(k) != m_pJobj->end())
	{
		auto v = (*m_pJobj)[k];
		if (v->IsString())
		{
			PS val = v->AsString().c_str();
			int len = tchlen(val);
			if(bNecessary && len == 0)
			{
				return nullptr;
			}
			if (bNullIfEmpty && len == 0)
				return L"NULL";
			else	{
				if(bQuat)
					sbuf.Format(L"'%s'", val);
				else
					sbuf = val;
				return sbuf;
			}
		}
		else if(v->IsDouble())
		{
			//CString fmt; fmt = L"%%f";
			double d = v->AsDouble();
			sbuf.Format(L"%f", (double)d);
			return sbuf;
		}
		else if(v->IsInt())
		{
			//CString fmt; fmt = L"%%d";
			auto d = v->AsInt();
			sbuf.Format(L"%d", (int)d);
			return sbuf;
		}
		else if(v->IsInt64())
		{
			//CString fmt;fmt = L"%%I64d";
			auto d = v->AsInt64();
			sbuf.Format(L"%I64d", (__int64)d);
			return sbuf;
		}
		else if (v->IsBool())
			return v->AsBool() ? L"1" : L"0";
		else if (v->IsNull())
			return L"NULL";
		else if (v->IsObject())
			return L"[obj]";//이래야 SQL에러가 나도록 유도 하지.
		else if (v->IsArray())
			return L"[array]";
		else {
			ASSERT(0);
		}
	}
	else if(bNecessary)
	{
		return nullptr;
	}
	return L"NULL";
}


// SQL query문에 쓰일 문자열을 구한다.
PWS CJsonPbj::QN(PWS k, int underDot)
{
	CString& sbuf = _buf.GetBuf();

	if (m_pJobj == NULL)
		throw (L"CJsonPbj.m_pJobj == NULL");
	//?주의: if((*m_pJobj)[k]) 이걸 쓰는 쑨간 만들어져 버린다.
	bool bDone = false;
	if (Has(k))
	{
		auto v = (*m_pJobj)[k];
		if(v->IsDouble())
		{
			bDone = true;
			sbuf = v->GetText().c_str();
			//CString fmt;
			//double d = v->AsDouble();
			//if(underDot > 0)
			//{
			//	fmt.Format(L"%%.%df", underDot);
			//	sbuf.Format(fmt, d);
			//}
			//else
			//{
			//	fmt = L"%d";
			//	sbuf.Format(L"%d", (int)d);
			//}
			//return sbuf;
		}
		else if(v->IsInt())
		{
			bDone = true;
			sbuf = v->GetText().c_str();
			//CString fmt;
			//auto d = v->AsInt();
			//fmt = L"%d";
			//sbuf.Format(L"%d", (int)d);
			//return sbuf;
		}
		else if(v->IsInt64())
		{
			bDone = true;
			sbuf = v->GetText().c_str();
			//CString fmt;
			//auto d = v->AsInt64();
			//	fmt = L"%I64d";
			//	sbuf.Format(L"%I64d", (int)d);
			//return sbuf;
		}
		else if (v->IsString())
		{
			bDone = true;
			sbuf = v->GetText().c_str();
			//CStringW s = v->AsString().c_str();
			//if (s.IsEmpty())
			//	return 0;
			//return s;
		}
	
		if(bDone)
			return (PWS)sbuf;

		// 여기 부터는 상수 문자열을 리턴 하므로 굳이 sbuf를 쓸 필요 없다.
		if (v->IsBool())// mySQL에서는 SQL에서 boolean 값은 0, 1 로 처리 되므로
			return v->AsBool() ? L"1" : L"0";
		else if (v->IsNull())
			return L"NULL";
		else if (v->IsObject())
			return L"[obj]";//이래야 SQL에러가 나도록 유도 하지.
		else if (v->IsArray())
			return L"[array]";
		else{
			ASSERT(0);
		}
	}
	return L"NULL";
}

const JSONArray* CJsonPbj::Array(PWS k)
{
	JSONObject* pbj = NULL;
	if(m_pJobj == NULL)
		throw (L"CJsonPbj.m_pJobj == NULL");
	//?주의: if((*m_pJobj)[k]) 이걸 쓰는 쑨간 만들어져 버린다.
	if(Has(k))
	{
		if(m_pJobj->find(k) != m_pJobj->end() && (*m_pJobj)[k]->IsArray())
			return &(*m_pJobj)[k]->AsArray();
	}
	return NULL;
}

JSONObject* CJsonPbj::Obj( PS k )
{
	JSONObject* pbj = NULL;
	if(m_pJobj == NULL)
		throw (L"CJsonPbj.m_pJobj == NULL");
	//?주의: if((*m_pJobj)[k]) 이걸 쓰는 쑨간 만들어져 버린다.
	if(Has(k))
	{
		if (m_pJobj->find(k) != m_pJobj->end() && (*m_pJobj)[k]->IsObject())
			return &(*m_pJobj)[k]->AsObject();
	}
	return NULL;
}

CStringW CJsonPbj::SLeft(PWS k, int len)
{
	JStr s = String(k);
	if(s.length() > len)
		s = s.substr(0, len);
	return CStringW(s.c_str());
}

CStringW CJsonPbj::SRight(PWS k, int len)
{
	JStr s = String(k);
	if(s.length() > len)
	{
		size_t ist = s.length() - len;
		s = s.substr(ist, len);
	}
	return CStringW(s.c_str());

}

double CJsonPbj::N( PWS k )
{
	if(m_pJobj == NULL)
		throw (L"CJsonPbj.m_pJobj == NULL");
	//?주의: if((*m_pJobj)[k]) 이걸 쓰는 쑨간 만들어져 버린다.
	if(Has(k))
	{
		if((*m_pJobj)[k]->IsDouble())
			return (*m_pJobj)[k]->AsDouble();
		else if((*m_pJobj)[k]->IsInt())
			return (double)(*m_pJobj)[k]->AsInt();
		else if((*m_pJobj)[k]->IsDouble())
			return (double)(*m_pJobj)[k]->AsInt64();
		else if((*m_pJobj)[k]->IsString())
		{
			
			CStringW s = (*m_pJobj)[k]->AsString().c_str();
			if(s.IsEmpty())
				return 0;

			return (double)KwAtoi((LPCWSTR)s);
		}
	}
	return 0.;
}


bool CJsonPbj::IsArray(PWS k)
{
	return (m_pJobj->find(k) != m_pJobj->end() && (*m_pJobj)[k]->IsArray());
}

bool CJsonPbj::IsObject(PWS k)
{
	return (m_pJobj->find(k) != m_pJobj->end() && (*m_pJobj)[k]->IsObject());
}

bool CJsonPbj::IsString(PWS k)
{
	return (m_pJobj->find(k) != m_pJobj->end() && (*m_pJobj)[k]->IsString());
}

bool CJsonPbj::IsNumber(PWS k)
{
	return (m_pJobj->find(k) != m_pJobj->end() && (*m_pJobj)[k]->IsNumber());
}
bool CJsonPbj::IsDouble(PAS k)
{
	CStringW wk(k);
	return (m_pJobj->find((PWS)wk) != m_pJobj->end() && (*m_pJobj)[(PWS)wk]->IsDouble());
}
bool CJsonPbj::IsInt(PAS k)
{
	CStringW wk(k);
	return (m_pJobj->find((PWS)wk) != m_pJobj->end() && (*m_pJobj)[(PWS)wk]->IsInt());
}
bool CJsonPbj::IsInt64(PAS k)
{
	CStringW wk(k);
	return (m_pJobj->find((PWS)wk) != m_pJobj->end() && (*m_pJobj)[(PWS)wk]->IsInt64());
}

CStringW CJsonPbj::ToJsonStringW()
{
	JSONObject* jov = this->get();
	JSONValue jsv; 
	/// 여기서 JSONValue jsv(*jov) 하고 아래에서 없어지면서 안데 object도 날려 버리기 때문에 모든 문제가 생겼다.
	jsv.WrappObj(*jov);
	CStringW sutf8 = jsv.Stringify(false, true).c_str();
	return sutf8;
}

CStringA CJsonPbj::ToJsonStringUtf8()
{
	CStringW sw = ToJsonStringW();
	CStringA sautf8;
	KwWcharToUTF8(sw, sautf8);
	return sautf8;
}

KBinary CJsonPbj::ToJsonData()
{
	CStringA jutf8 = this->ToJsonStringUtf8();
	KBinary bin;
	bin.SetPtr((PAS)jutf8, jutf8.GetLength());
	return bin;
}

bool CJsonPbj::CopyFielsIf(CJsonPbj& src, PWS key)
{
	if (src.Has(key))
	{
		(*this)(key) = src(key);
		return true;
	}
	return false;
}

int CJsonPbj::CopyFieldsAll(CJsonPbj& src)
{
	int n = 0;
	src.for_loop([&](JSonKey key, auto val) -> void
		{
			(*m_pJobj)[key.c_str()] = &val; //OK
			//jsf1[key] = val; //fail
			//jsf1[key.c_str()] = val; //fail
			n++;
		});
	return n;
}
void CJsonPbj::SetMove(PWS k, CJsonPbj& src)
{
	//src.get()->_bValueOwner = false; MoveObj에서 한다.
	auto jv = new JSONValue();
	jv->MoveObj(*src.get());
	(*m_pJobj)[k] = jv;// new JSONValue(v);

}


CJsonArray::CJsonArray(const JSONArray* jobj)
{
	if(jobj == NULL)
	{
		m_pJarr = new JSONArray();
		_bOwner = TRUE;
	}
	else
	{
		m_pJarr = (JSONArray*)jobj;
		_bOwner = FALSE;
	}
}

void CJsonArray::Add(CJsonPbj& obj)
{
	m_pJarr->Add(new JSONValue(*obj.get()));
}
void CJsonArray::Add(CJsonArray& arr)
{
	m_pJarr->Add(new JSONValue(*arr.get()));
}
void CJsonArray::Add(JSONArray& arr)
{
	m_pJarr->Add(new JSONValue(&arr));
}
void CJsonArray::Add(JSONObject& obj)
{
	m_pJarr->Add(new JSONValue(&obj));
}
