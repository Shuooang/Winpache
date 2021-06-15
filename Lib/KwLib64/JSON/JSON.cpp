/*
 * File JSON.cpp part of the SimpleJSON Library - http://mjpa.in/json
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
//#include "../stdafx.h"
//1 > c:\dropbox\proj\cmnj\kwlib32\json_s\json.cpp(573) : fatal error C1010 : 미리 컴파일된 헤더를 찾는 동안 예기치 않은 파일의 끝이 나타났습니다. '#include "stdafx.h"'을(를) 소스에 추가하시겠습니까 ?

#include "pch.h"
// 이렇게 하면 컴파일 되지만 편집기에서 못잦고

#include "../KBinary.h"


#include "JSON.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**
 * Blocks off the public constructor
 *
 * @access private
 *
 */
JSON::JSON()
{
}

/**
 * Parses a complete JSON encoded string
 * This is just a wrapper around the UNICODE Parse().
 *
 * @access public
 *
 * @param char* data The JSON text
 *
 * @return JSONValue* Returns a JSON Value representing the root, or NULL on error
 */
JSONValue *JSON::Parse(const char *data)
{
	size_t length = strlen(data) + 1;
	wchar_t *w_data = (wchar_t*)malloc(length * sizeof(wchar_t));
	
	#if defined(WIN32) && !defined(__GNUC__)
		size_t ret_value = 0;
		if (mbstowcs_s(&ret_value, w_data, length, data, length) != 0)
		{
			free(w_data);
			return NULL;
		}
	#elif defined(ANDROID)
		// mbstowcs seems to misbehave on android
		for(size_t i = 0; i<length; i++)
			w_data[i] = (wchar_t)data[i];
	#else
		if (mbstowcs(w_data, data, length) == (size_t)-1)
		{
			free(w_data);
			return NULL;
		}
	#endif
	
	JSONValue *value = JSON::Parse(w_data);
	free(w_data);
	return value;
}

/**
 * Parses a complete JSON encoded string (UNICODE input version)
 *
 * @access public
 *
 * @param wchar_t* data The JSON text
 *
 * @return JSONValue* Returns a JSON Value representing the root, or NULL on error
 */
JSONValue *JSON::Parse(const wchar_t *data)
{
	// Skip any preceding whitespace, end of data = no JSON = fail
	if (!SkipWhitespace(&data))
		return NULL;

	// We need the start of a value here now...
	JSONValue *value = JSONValue::Parse(&data);
	if (value == NULL)
		return NULL;
	
	// Can be white space now and should be at the end of the string then...
	if (SkipWhitespace(&data))
	{
		delete value;
		return NULL;
	}
	
	// We're now at the end of the string
	return value;
}

/**
 * Turns the passed in JSONValue into a JSON encode string
 *
 * @access public
 *
 * @param JSONValue* value The root value
 *
 * @return std::wstring Returns a JSON encoded string representation of the given value
 */
JStr JSON::Stringify(const JSONValue *value)
{
	if (value != NULL)
		return value->Stringify();
	else
		return L"";
}

/**
 * Skips over any whitespace characters (space, tab, \r or \n) defined by the JSON spec
 *
 * @access protected
 *
 * @param wchar_t** data Pointer to a wchar_t* that contains the JSON text
 *
 * @return bool Returns true if there is more data, or false if the end of the text was reached
 */
bool JSON::SkipWhitespace(const wchar_t **data)
{
	while (**data != 0 && (**data == L' ' || **data == L'\t' || **data == L'\r' || **data == L'\n'))
		(*data)++;
	
	return **data != 0;
}

/**
 * Extracts a JSON String as defined by the spec - "<some chars>"
 * Any escaped characters are swapped out for their unescaped values
 *
 * @access protected
 *
 * @param wchar_t** data Pointer to a wchar_t* that contains the JSON text
 * @param std::wstring& str Reference to a std::wstring to receive the extracted string
 *
 * @return bool Returns true on success, false on failure
 */
bool JSON::ExtractString(const wchar_t **data, JStr &str)
{
	str = L"";
	
	while (**data != 0)
	{
		// Save the char so we can change it if need be
		wchar_t next_char = **data;
		
		// Escaping something?
		if (next_char == L'\\')
		{
			// Move over the escape char
			(*data)++;
			
			// Deal with the escaped char
			switch (**data)
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
					if (!simplejson_wcsnlen(*data, 5))
						return false;
					
					// Deal with the chars
					next_char = 0;
					for (int i = 0; i < 4; i++)
					{
						// Do it first to move off the 'u' and leave us on the
						// final hex digit as we move on by one later on
						(*data)++;
						
						next_char <<= 4;
						
						// Parse the hex digit
						if (**data >= '0' && **data <= '9')
							next_char |= (**data - '0');
						else if (**data >= 'A' && **data <= 'F')
							next_char |= (10 + (**data - 'A'));
						else if (**data >= 'a' && **data <= 'f')
							next_char |= (10 + (**data - 'a'));
						else
						{
							// Invalid hex digit = invalid JSON
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
		else if (next_char == L'"')
		{
			(*data)++;
			str.reserve(); // Remove unused capacity
			return true;
		}
		
		// Disallowed char?
		else if (next_char < L' ')// && next_char != L'\t')
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

/**
 * Parses some text as though it is an integer
 *
 * @access protected
 *
 * @param wchar_t** data Pointer to a wchar_t* that contains the JSON text
 *
 * @return double Returns the double value of the number found
 */
double JSON::ParseInt(const wchar_t **data)
{
	double integer = 0;
	while (**data != 0 && **data >= '0' && **data <= '9')
		integer = integer * 10 + (*(*data)++ - '0');
	
	return integer;
}

/**
 * Parses some text as though it is a decimal
 *
 * @access protected
 *
 * @param wchar_t** data Pointer to a wchar_t* that contains the JSON text
 *
 * @return double Returns the double value of the decimal found
 */
double JSON::ParseDecimal(const wchar_t **data)
{
	double decimal = 0.0;
  double factor = 0.1;
	while (**data != 0 && **data >= '0' && **data <= '9')
  {
    int digit = (*(*data)++ - '0');
		decimal = decimal + digit * factor;
    factor *= 0.1;
  }
	return decimal;
}


void JSONObject::SetAt(PWS name, JSONValue* val)
{
	if (this->find(name) != this->end())
	{
		int i0 = FindOnArray(name);
		if (i0 >= 0)
			_keys.erase(_keys.begin() + i0);//덮어 쓰므로 이전 위치것은 지워
	}
	_keys.push_back(name);
	(*this)[name] = val;
}

void JSONObject::DeleteKey(PWS name)
{
	JSONObject::const_iterator it = this->find(name);
	if(it == this->end())
	{
		int i0 = FindOnArray(name);
		if(i0 >= 0)
			_keys.erase(_keys.begin() + i0);

		delete (*this)[name];
		//delete it->second;
	}
}

int JSONObject::FindOnArray(PWS name)
{
	for(size_t i = 0;i < _keys.size();i++)
	{
		if(wcscmp(name, _keys[i].c_str()) == 0)
			return (int)i;
	}
	return -1;
}







#ifdef _samples_

using namespace std;

// Just some sample JSON text, feel free to change but could break demo
const wchar_t* EXAMPLE = L"\
						  { \
						  \"string_name\" : \"string\tvalue and a \\\"따옴표\\\" and a unicode char \\u00BE and a c:\\\\path\\\\ or a \\/unix\\/path\\/ :D\", \
						  \"bool_name\" : true, \
						  \"bool_second\" : FaLsE, \
						  \"null_name\" : nULl, \
						  \"negative\" : -34.276, \
						  \"sub_object\" : { \
						  \"foo\" : \"abc\", \
						  \"bar\" : 1.35e2, \
						  \"blah\" : { \"a\" : \"A\", \"b\" : \"B\", \"c\" : \"C\" } \
						  }, \
						  \"array_letters\" : [ \"a\", \"b\", \"c\", [ 1, 2, 3  ]  ] \
						  }    ";

#ifdef _DEBUGx
{ 	
	"string_name" : "string	value and a \"quote\" and a unicode char \u00BE and a c:\\path\\ or a \/unix\/path\/ :D", 	
		"bool_name" : true, 	
		"bool_second" : FaLsE, 	
		"null_name" : nULl, 	
		"negative" : -34.276, 	
		"sub_object" : 
	{ 						
		"foo" : "abc", 						 
			"bar" : 1.35e2, 						 
			"blah" : 
		{ 
			"a" : "A", 
				"b" : "B", 
				"c" : "C" 
		} 					
	}, 	
		"array_letters" : 
	[
		"a", "b", "c", 
		[ 1, 2, 3  
		]  
	] 
}    
#endif // _DEBUGx
// Example 1
void example1()
{
	// Parse example data
	JSONValue *value = JSON::Parse(EXAMPLE);

	// Did it go wrong?
	if (value == NULL)
	{
		print_out(L"Example code failed to parse, did you change it?\r\n");
	}
	else
	{
		// Retrieve the main object
		JSONObject root;
		if (value->IsObject() == false)
		{
			print_out(L"The root element is not an object, did you change the example?\r\n");
		}
		else
		{
			root = value->AsObject();// operator= clone 된다.

			// Retrieving a string
			if (root.find(L"string_name") != root.end() && root[L"string_name"]->IsString())
			{
				print_out(L"string_name:\r\n");
				print_out(L"------------\r\n");
				print_out(root[L"string_name"]->AsString().c_str());
				print_out(L"\r\n\r\n");
			}

			// Retrieving a boolean
			if (root.find(L"bool_second") != root.end() && root[L"bool_second"]->IsBool())
			{
				print_out(L"bool_second:\r\n");
				print_out(L"------------\r\n");
				print_out(root[L"bool_second"]->AsBool() ? L"it's true!" : L"it's false!");
				print_out(L"\r\n\r\n");
			}

			// Retrieving an array
			if (root.find(L"array_letters") != root.end() && root[L"array_letters"]->IsArray())
			{
				JSONArray array = root[L"array_letters"]->AsArray();
				print_out(L"array_letters:\r\n");
				print_out(L"--------------\r\n");
				for (unsigned int i = 0; i < array.size(); i++)
				{
					wstringstream output;
					output << L"[" << i << L"] => " << array[i]->Stringify() << L"\r\n";
					print_out(output.str().c_str());
				}
				print_out(L"\r\n");
			}

			// Retrieving nested object
			if (root.find(L"sub_object") != root.end() && root[L"sub_object"]->IsObject())
			{
				print_out(L"sub_object:\r\n");
				print_out(L"-----------\r\n");
				print_out(root[L"sub_object"]->Stringify().c_str());
				print_out(L"\r\n\r\n");
			}
		}

		delete value;
	}
}

// Example 2
void example2()
{
	JSONObject root;

	// Adding a string
	root[L"test_string"] = new JSONValue(L"hello world");

	// Create a random integer array
	JSONArray array;
	srand((unsigned)time(0));
	for (int i = 0; i < 10; i++)
		array.push_back(new JSONValue((double)(rand() % 100)));
	root[L"sample_array"] = new JSONValue(array);

	// Create a value
	JSONValue *value = new JSONValue(root);

	// Print it
	print_out(value->Stringify().c_str());

	// Clean up
	delete value;
}

// Example 3 : compact vs. prettyprint
void example3()
{
	const wchar_t* EXAMPLE3 =
		L"{\
		 \"SelectedTab\":\"Math\",\
		 \"Widgets\":[\
		 {\"WidgetPosition\":[0,369,800,582],\"WidgetIndex\":1,\"WidgetType\":\"WidgetCheckbox.1\"},\
		 {\"WidgetPosition\":[235,453,283,489],\"IsWidgetVisible\":-1,\"Caption\":\"On\",\"EnableCaption\":-1,\"Name\":\"F2.View\",\"CaptionPosition\":2,\"ControlWidth\":25,\"ControlHeight\":36,\"Font\":0,\"Value\":\"Off\",\"WidgetIndex\":2,\"WidgetType\":\"WidgetCheckbox.1\"},\
		 {\"WidgetPosition\":[235,494,283,530],\"IsWidgetVisible\":-1,\"Caption\":\"On\",\"EnableCaption\":-1,\"Name\":\"F3.View\",\"CaptionPosition\":2,\"ControlWidth\":25,\"ControlHeight\":36,\"Font\":0,\"Value\":\"Off\",\"WidgetIndex\":3,\"WidgetType\":\"WidgetCheckbox.1\"},\
		 {\"WidgetPosition\":[235,536,283,572],\"IsWidgetVisible\":-1,\"Caption\":\"On\",\"EnableCaption\":-1,\"Name\":\"F4.View\",\"CaptionPosition\":2,\"ControlWidth\":25,\"ControlHeight\":36,\"Font\":0,\"Value\":\"Off\",\"WidgetIndex\":4,\"WidgetType\":\"WidgetCheckbox.1\"},\
		 {\"WidgetPosition\":[287,417,400,439],\"IsWidgetVisible\":-1,\"Caption\":\"\",\"EnableCaption\":0,\"Name\":\"F1.Equation\",\"CaptionPosition\":1,\"ControlWidth\":113,\"ControlHeight\":22,\"Font\":0,\"AlignText\":0,\"EnableBorder\":0,\"CaptionOnly\":0,\"Value\":\"FFT(C1)\",\"WidgetIndex\":9,\"WidgetType\":\"WidgetStaticText.1\"},\
		 {\"WidgetPosition\":[191,409,230,445],\"IsWidgetVisible\":0,\"Caption\":\"F1\",\"EnableCaption\":0,\"Name\":\"F1.MeasureOpGui\",\"CaptionPosition\":1,\"ControlWidth\":39,\"ControlHeight\":36,\"Font\":0,\"ButtonOnly\":-1,\"PickerTitle\":\"Select Measurement To Graph\",\"Value\":\"Amplitude\",\"WidgetIndex\":17,\"WidgetType\":\"WidgetProcessorCombobox.1\"},\
		 {\"WidgetPosition\":[191,409,230,445],\"IsWidgetVisible\":-1,\"Caption\":\"F1\",\"EnableCaption\":0,\"Name\":\"F1.Operator1gui\",\"CaptionPosition\":1,\"ControlWidth\":39,\"ControlHeight\":36,\"Font\":0,\"ButtonOnly\":-1,\"PickerTitle\":\"Select Math Operator\",\"Value\":\"FFT\",\"WidgetIndex\":25,\"WidgetType\":\"WidgetProcessorCombobox.1\"},\
		 {\"WidgetPosition\":[191,452,230,487],\"IsWidgetVisible\":-1,\"Caption\":\"F2\",\"EnableCaption\":0,\"Name\":\"F2.Operator1gui\",\"CaptionPosition\":1,\"ControlWidth\":39,\"ControlHeight\":36,\"Font\":0,\"ButtonOnly\":-1,\"PickerTitle\":\"Select Math Operator\",\"Value\":\"Zoom\",\"WidgetIndex\":26,\"WidgetType\":\"WidgetProcessorCombobox.1\"}\
		 ]\
		 }";

	// Parse example data
	JSONValue *value = JSON::Parse(EXAMPLE3);
	if (value)
	{
		print_out(L"-----------\r\n");
		print_out(value->Stringify().c_str());
		print_out(L"\r\n");
		print_out(L"-----------\r\n");
		print_out(value->Stringify(true).c_str());
		print_out(L"\r\n");
		print_out(L"-----------\r\n");
	}

	// Clean up
	delete value;
}

// Example 4 : List keys in an object.
void example4()
{
	// Parse the example.
	JSONValue *main_object = JSON::Parse(EXAMPLE);
	if (main_object == NULL)
	{
		print_out(L"Example code failed to parse, did you change it?\r\n");
	}
	else if (!main_object->IsObject())
	{
		print_out(L"Example code is not an object, did you change it?\r\n");
		delete main_object;
	}
	else
	{
		// Print the main object.
		print_out(L"Main object:\r\n");
		print_out(main_object->Stringify(true).c_str());
		print_out(L"-----------\r\n");

		// Fetch the keys and print them out.
		std::vector<std::wstring> keys = main_object->ObjectKeys();
		std::vector<std::wstring>::iterator iter = keys.begin();
		while (iter != keys.end())
		{
			print_out(L"Key: ");
			print_out((*iter).c_str());
			print_out(L"\r\n");

			// Get the key's value.
			JSONValue *key_value = main_object->Child((*iter).c_str());
			if (key_value)
			{
				print_out(L"Value: ");
				print_out(key_value->Stringify().c_str());
				print_out(L"\r\n");
				print_out(L"-----------\r\n");
			}

			// Next key.
			iter++;
		}

		delete main_object;
	}
}











#endif // _samples_

JSONArray::~JSONArray()
{
	DeleteValues();
}
void JSONArray::DeleteAll()//_bValueOwner
{
	DeleteValues();
	this->clear();
}
void JSONArray::DeleteValues()//_bValueOwner
{
	if(_bValueOwner)
		for(auto& v : *this)
		{
			delete v;
			v = nullptr;
		}
}

//test전
void JSONArray::AddMove(JSONValue*&& jv)
{
	//ASSERT(0);// 아직 안만들었어.
	// jv가 object, array이면 _bOwner를 풀고 안으로 들어 가는 거다.
	if(jv->IsObject())
	{
		auto obj = jv->AsObject();
		obj._bValueOwner = false;
	}
	else if(jv->IsArray())
	{
		auto arr = jv->AsArray();
		arr._bValueOwner = false;
	}
	push_back(jv);
}

void JSONArray::Add(JSONValue* jv)
{
	push_back(jv);
}


JSONObject::~JSONObject()
{
	if(_bValueOwner)//MoveObject 에서 false 하는 경우도 있다.
	{
		// 옛날 방식
		//JSONObject::iterator iter;
		//for(iter = this->begin(); iter != this->end(); iter++)
		//{
		//	auto key = (*iter).first;
		//	auto val = (*iter).second;
		//}

		for(auto& [k, v] : *this) //c++ 17
		{
			delete v;
			v = nullptr;
		}
	}
	//this->clear(); 하지 않아도 지가 한다.
}

void JSONObject::DeleteValues()//_bValueOwner
{
	if(_bValueOwner)
		for(auto& [k, v] : *this)
		{
			delete v;
			v = nullptr;
		}
}
void JSONObject::DeleteAll()//_bValueOwner
{
	DeleteValues();
	this->clear();
}
