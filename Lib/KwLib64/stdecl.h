#pragma once
#include <string>
//#include <iosfwd> : std::stringstream 불완전한 형식은 사용할 수 없습니다. 대신 아래를 써야 한다.
#include <sstream>
#include <memory>
#include <map>
#include <functional>
#include <vector>
#include <xstring>
//#include <afxmt.h>

using std::string;
using std::wstring;
using std::shared_ptr;
using std::make_shared;
using std::function;
using std::vector;
using std::wstringstream;
using std::stringstream;
using std::string_view;
// std::map은 너무 단순 하여 full name으로 쓴다.
typedef std::wstringstream Tss;
typedef std::stringstream Tas;

// Tss 를 문자열 포인터로 리턴 할려면
#define Psr(ss) (ss.str().c_str())
// 한번 사용한 Tss 를 재사용전에 초기화 하려면
#define SsEmpty(ss)  ss.str(s_NS)
#define SsLength(ss)  ss.str().length()

/// string::find 의 리턴 값이 unsigned __int64 라 
/// if(ir == string::npos) 로 비교 하는걸 잊어 먹어서 
#define _1 (UINT)-1
#define __1 (unsigned __int64)-1L


/// shared_ptr 만들떄 이걸 두번째 인자로 만들어 주면 자동 삭제시 사용자가 정의할 수 있다.
/// ex: object_value = shared_ptr<SomeObj>(&obj1, TNotFree());
struct TNotFree
{
	template<typename T>
	void operator()(T* p)
	{
		//TRACE("Do not free.\n");
		/// JVal::ShareObj(JObj& obj1) 에서 	shared_ptr 인데도 자동 삭제 하지 않으려 할때 쓴다.
		//delete p; // 배열은 delete [] p;
	}
};
//?fail 이건 써보니 안된다.
// ex: object_value = shared_ptr<SomeObj>(&obj1, &TSharePtrDelete);
//template<typename T>
//void TSharePtrDelete(T* p)
//{
//	TRACE("Do not free func.\n");//삭제 하지 않는다.	//delete p; 
//}

// KwLib64/KTemple.h KwCreateFuncValue 가 동일하게 있지만 외부 코드라 KTemple.h를 include하지 않기 위해 넣는다.
///?주의: function type 과 lambda의 함수 타입은 같아야 한다.
/// https->AddCallbackOnGET([&](HTTPSCacheSession* ss, HTTPRequest& req, HTTPResponse& res) -> int {
// [&](HTTPSCacheSession* ss, HTTPRequest& req, HTTPResponse& res) -> int 과 function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)> 
/// 매개변수, 리턴타입이 일치 한다. 
template<typename TFNC, typename TRAMBD>
void KwCreateFuncValue(std::shared_ptr<TFNC>& rtval, TRAMBD lambda)
{
	/* ex: TRegisterCallback 이름으로 CppServer\CppServer-master\examples\HttpsSvr.h 중복
	// 다음과 같은 변수가 선언 되었다고 전제
	shared_ptr<function<TRAMBD>> _fncGET;
	// 변수 선언 : TFNC == function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)> 을 줄이기 위해
	shared_ptr<function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)>> _fncGET;
	// 할당
	template<typename TFNC> void AddCallbackOnGET(TFNC fnc)
	{	TCreateFuncValue(_fncGET, fnc);
		//_fncGET = shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>>(new function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>(fnc));
	}*/
	rtval = std::make_shared<TFNC>(lambda);
	//rtval = shared_ptr<TFNC>(new TFNC(lambda)); 위와 동일
}



#include <xtree>
/// KwStr ws1 = L"xx";
//error C2440: '초기화 중': 'const wchar_t [3]'에서 'KwStr'(으)로 변환할 수 없습니다.
/*
template<typename Tstr, typename Tchar>
class KTStr : public Tstr //std::basic_string<Tchar, std::char_traits<Tchar>, std::allocator<Tchar>>
{
public:
	typedef const Tchar* TPCH;
	KTStr(TPCH s = nullptr)
	{
		*this = s;
	}
	//C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.28.29333\atlmfc\include\atlsimpstr.h
	// 375 line
	operator TPCH() const
	{
		return this->c_str();
	}
	KTStr<Tstr, Tchar>& operator=(TPCH s)
	{
		//error C2440: 'static_cast': 'KTStr<wchar_t> *'에서 'std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t>>'(으)로 변환할 수 없습니다.
		//error C2680 : 'std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t>>' : dynamic_cast의 대상 형식이 잘못되었습니다.
		//auto str = static_cast<std::basic_string<Tchar, std::char_traits<Tchar>, std::allocator<Tchar>>>(this);
		auto str = static_cast<Tstr*>(this);
		this = s;
		return *this;
	}

};

class KwStr : public KTStr<wstring, wchar_t>
{
public:
	KwStr(LPCWSTR s = nullptr)
		: KTStr<wstring, wchar_t>(s)
	{
	}
};
class KaStr : public KTStr<string, char>
{
public:
};
*/
/*
출처: https://blockdmask.tistory.com/88 [개발자 지망생]
map 과는 다르게
operator [] 를 사용해서 원소(pair<key, value>)를 추가 또는 수정이 불가능 합니다.
- key 값에 해당되는 맨 마지막 원소의 "다음" 을 가리키는 반복자를 반환합니다.
- 폐구간 " ) " 으로 사용됩니다.

mm.lower_bound(key);
- key 값에 해당하는 맨 첫번째 원소를 가리키는 반복자를 반환합니다.
- 개구간 " [ " 으로 사용됩니다.

mm.equal_range(key);
- key 값에 해당하는 원소의 "범위" 를 pair 객체로 반환합니다.

- pair 객체의 first 는 key 값에 해당하는 원소의 첫번째 반복자를 반환. (lower_bound)
- pair 객체의 second 는 key 값에 해당하는 원소의 마지막 원소의 "다음" 반복자를 반환. (upper_bound)
	
	
	mm.insert(pair<int, int>(60, 6));


출처: https://blockdmask.tistory.com/88 [개발자 지망생]
*/
template<typename TKey, typename TVal>
class KMulMap
	: public std::multimap<TKey, TVal>
{
public:
	BOOL Lookup(TKey k, TVal& v)
	{
		auto it = this->find(k);
		if(it != this->end())
		{
			v = it->second;
			return TRUE;
		}
		return FALSE;
	}
	bool Has(TKey k)
	{
		auto it = this->find(k);
		return (it != this->end());
	}
	void SetAt(TKey k, TVal v)
	{
		(*this)[k] = v;
	}
	void Insert(TKey k, TVal v)
	{
		insert(std::pair<TKey, TVal>(k, v));
	}
	/// 그냥 for(auto& [k,v] : *this) 쓰면 되는데 굳이 쓴다면 reverse용으로
	template<typename FNC>
	void for_loopEx(FNC lambda)
	{
		int i = 0;
		for(auto& [k, v] : *this)
		{
			if(!lambda(k, v, i))
				break;
			i++;
		}
	}
	template<typename FNC>
	void for_RLoopEx(FNC lambda)
	{
		int i = 0;
		for(auto it = this->rbegin(); it != this->rend(); it++, i++)
		{
			if(!lambda(it->first, it->second, i))
				break;
		}
	}

	template<typename FNC>
	void for_loop(FNC lambda)
	{
		int i = 0;
		for(auto& [k, v] : *this)
		{
			lambda(k, v, i);
			i++;
		}
	}
	template<typename FNC>
	void for_RLoop(FNC lambda)
	{
		int i = 0;
		for(auto it = this->rbegin(); it != this->rend(); it++, i++)
			lambda(it->first, it->second, i);
	}

};

template<typename TKey, typename TVal>
class KStdMap
	: public std::map<TKey, TVal>
{
public:
	//CCriticalSection _cs;
	//friend std::_Tree;
	BOOL Lookup(TKey k, TVal& v)
	{
		if(this == nullptr)
			throw "KStdMap::Lookup() this is null.";
		auto it = this->find(k);
		if(it != this->end())
		{
			v = it->second;
			return TRUE;
		}
		return FALSE;
	}
	bool Has(TKey k)
	{
		if(this == nullptr)
			throw "KStdMap::Has() this is null.";
		auto it = this->find(k);
		return (it != this->end());
	}
	void SetAt(TKey k, TVal v)
	{
		if(this == nullptr)
			throw "KStdMap::SetAt() this is null.";
		(*this)[k] = v;
///		toString(); 안에 디버그 작업 하고 싶으면 한다.
	}
	//KStdMap<TKey, TVal>* operator[](TKey k) { return (*m_pJobj)[k]; }
	//void operator[](TKey k) {  }

	typename std::map<TKey, TVal>::iterator Find(TKey k)
	{
		if(this == nullptr)
			throw "KStdMap::Find() this is null.";
		auto it = this->find(k);
		return it;
	}
	/// 그냥 for(auto& [k,v] : *this) 쓰면 되는데 굳이 쓴다면 reverse용으로
	virtual void toString()
	{
	}
	TVal Get(TKey key)
	{
		TVal v;
		Lookup(key, v);
		return v;
	}

	
	/// Ex 붙은 것은 리턴값에 따라 break 여부가 정해진다.
	template<typename FNC>
	void for_loopEx(FNC lambda)
	{
		int i = 0;
		for(auto& [k, v] : *this)
			if(!lambda(k, v, i))
				break;
	}
	template<typename FNC>
	void for_RLoopEx(FNC lambda)
	{
		int i = 0;
		for(auto it = this->rbegin(); it != this->rend(); it++, i++)
		{
			if(!lambda(it->first, it->second, i))
				break;
		}
	}

	template<typename FNC>
	void for_loop(FNC lambda)
	{
		int i = 0;
		for(auto& [k, v] : *this)
			lambda(k, v, i);
	}
	template<typename FNC>
	void for_RLoop(FNC lambda)
	{
		int i = 0;
		for(auto it = this->rbegin(); it != this->rend(); it++, i++)
		{
			lambda(it->first, it->second, i);
		}
	}
};

// TKey는 string, wstring 가능 하다.
// 이거 괜히 만든 듯. KStdMap하고 뭐가 달라
//template<typename TKey, typename TVal>
class KStrMap
	: public KStdMap<string, string>// std::map<TKey, TVal>
{
public:
//	string Get(string key)
// 	{
// 		string v;
// 		Lookup(key, v);
// 		return v;
// 	}
};

class KWStrMap
	: public KStdMap<wstring, wstring>// std::map<TKey, TVal>
{
public:
// 	wstring Get(wstring key)
// 	{
// 		wstring v;
// 		Lookup(key, v);
// 		return v;
// 	}
};
class KAWStrMap
	: public KStdMap<string, wstring>// std::map<TKey, TVal>
{
public:
};
class KWAStrMap
	: public KStdMap<string, wstring>// std::map<TKey, TVal>
{
public:
};

// 내부 데이터를 디버깅 중에 볼수 있어서 앞으로 std 쪽을 써야 겠다.
/// TKey는 std::string 또는 std::wstring 을 쓴다.
/// TObj는 포인터로 *는 생략 한다.
/// m_bAutoFree를 건드리지 않는 한, 자체 free된다.
template<typename TKey, typename TObj>
class KStdMapPtr
	: public KStdMap<TKey, TObj*>
{
public:

	KStdMapPtr()
	{
		if(_token == 313409)
			_token = 313409;
	}
	~KStdMapPtr()
	{
		DeleteAll();
	}
	int _token{0};
	BOOL DeleteKey(TKey k)
	{
		auto it = this->find(k);
		if(it != this->end())
		{
			TObj* v = it->second;
			DeleteMeSafe(v);
			erase(it);
			return TRUE;
		}
		return FALSE;
	}

	BOOL m_bAutoFree{ true };
	/*	BOOL Lookup(TKey k, TObj*& v)
		{
			auto it = this->find(k);
			if(it != this->end())
			{
				v = it->second;
				return TRUE;
			}
			return FALSE;
		}
		bool Has(TKey k)
		{
			auto it = this->find(k);
			return (it != this->end());
		}
		void SetAt(TKey k, TObj* v)
		{
			(*this)[k] = v;
		}
	*/
	virtual void DeleteAll()
	{
		//auto rb = static_cast<CRBMap<TKey, TObj*>>(this);
		if(m_bAutoFree)
		{
			if(_token == 313409)
				_token = 313409;
			for(auto [k, v] : *this)
				DeleteMeSafe(v);
		}
		this->clear();
	}
	TObj* get(TKey k)
	{
		auto it = this->find(k);
		if(it != this->end())
			return it->second;
		return nullptr;
	}

};

template<typename T>
class KArray : public std::vector<T>
{
public:
	//CCriticalSection _cs;
	KArray()
	{
	}
	KArray(int n, T v)
	{
		for(int i = 0; i < n; i++)
		{
			Add(v);
		}
	}
	void Add(T v)
	{
		this->push_back(v);
	}
	T& GetAt(int i)
	{
		return (*this)[i];
	}
	int GetCount()
	{
		return (int)this->size();
	}
	int Last()
	{
		return GetCount() - 1;
	}
};

template<typename T>
class KPtrArray : public KArray<T*>
{
public:
	BOOL m_bAutoFree{ true };
	//template<typename FNC)
	KPtrArray(int n = 0) //, FNC lbd)
	{
		for(int i = 0; i < n; i++)
		{
			auto v = new T;// 기본 생성자가 있는 객체에 한해서 미리 할당 해 놓을 수 있다.
			Add(v);
		}
	}
	~KPtrArray()
	{
		DeleteAll();
	}
	void DeleteAll()
	{
		if(m_bAutoFree)
			for(auto& v : *this)
				DeleteMeSafe(v);
	}
};

template<typename T>
class KList : public std::list<T>
{
public:
	//CCriticalSection _cs;
};

template<typename TObj>
class KPtrList : public KList<TObj*>
{
public:
	BOOL m_bAutoFree{ true };
	~KPtrList()
	{
		if(m_bAutoFree)
			for(auto& v : *this)
				DeleteMeSafe(v);
	}
};

/// wstring <-> string 상호변환
template<typename TS1, typename TS2>
void Ucode(TS1 str1, TS2& str2)
{//str1 -> str1
	str2.assign(str1.begin(), str1.end());
}

inline wstring ToWStr(string str)
{
	wstring ws;
	Ucode(str, ws);
	return ws;
}
inline string ToAStr(wstring ws)
{
	string as;
	Ucode(ws, as);
	return as;
}