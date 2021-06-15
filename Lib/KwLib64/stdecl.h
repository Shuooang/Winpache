#pragma once
#include <string>
//#include <iosfwd> : std::stringstream �ҿ����� ������ ����� �� �����ϴ�. ��� �Ʒ��� ��� �Ѵ�.
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
// std::map�� �ʹ� �ܼ� �Ͽ� full name���� ����.
typedef std::wstringstream Tss;
typedef std::stringstream Tas;

// Tss �� ���ڿ� �����ͷ� ���� �ҷ���
#define Psr(ss) (ss.str().c_str())
// �ѹ� ����� Tss �� �������� �ʱ�ȭ �Ϸ���
#define SsEmpty(ss)  ss.str(s_NS)
#define SsLength(ss)  ss.str().length()

/// string::find �� ���� ���� unsigned __int64 �� 
/// if(ir == string::npos) �� �� �ϴ°� �ؾ� �Ծ 
#define _1 (UINT)-1
#define __1 (unsigned __int64)-1L


/// shared_ptr ���鋚 �̰� �ι�° ���ڷ� ����� �ָ� �ڵ� ������ ����ڰ� ������ �� �ִ�.
/// ex: object_value = shared_ptr<SomeObj>(&obj1, TNotFree());
struct TNotFree
{
	template<typename T>
	void operator()(T* p)
	{
		//TRACE("Do not free.\n");
		/// JVal::ShareObj(JObj& obj1) ���� 	shared_ptr �ε��� �ڵ� ���� ���� ������ �Ҷ� ����.
		//delete p; // �迭�� delete [] p;
	}
};
//?fail �̰� �Ẹ�� �ȵȴ�.
// ex: object_value = shared_ptr<SomeObj>(&obj1, &TSharePtrDelete);
//template<typename T>
//void TSharePtrDelete(T* p)
//{
//	TRACE("Do not free func.\n");//���� ���� �ʴ´�.	//delete p; 
//}

// KwLib64/KTemple.h KwCreateFuncValue �� �����ϰ� ������ �ܺ� �ڵ�� KTemple.h�� include���� �ʱ� ���� �ִ´�.
///?����: function type �� lambda�� �Լ� Ÿ���� ���ƾ� �Ѵ�.
/// https->AddCallbackOnGET([&](HTTPSCacheSession* ss, HTTPRequest& req, HTTPResponse& res) -> int {
// [&](HTTPSCacheSession* ss, HTTPRequest& req, HTTPResponse& res) -> int �� function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)> 
/// �Ű�����, ����Ÿ���� ��ġ �Ѵ�. 
template<typename TFNC, typename TRAMBD>
void KwCreateFuncValue(std::shared_ptr<TFNC>& rtval, TRAMBD lambda)
{
	/* ex: TRegisterCallback �̸����� CppServer\CppServer-master\examples\HttpsSvr.h �ߺ�
	// ������ ���� ������ ���� �Ǿ��ٰ� ����
	shared_ptr<function<TRAMBD>> _fncGET;
	// ���� ���� : TFNC == function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)> �� ���̱� ����
	shared_ptr<function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)>> _fncGET;
	// �Ҵ�
	template<typename TFNC> void AddCallbackOnGET(TFNC fnc)
	{	TCreateFuncValue(_fncGET, fnc);
		//_fncGET = shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>>(new function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>(fnc));
	}*/
	rtval = std::make_shared<TFNC>(lambda);
	//rtval = shared_ptr<TFNC>(new TFNC(lambda)); ���� ����
}



#include <xtree>
/// KwStr ws1 = L"xx";
//error C2440: '�ʱ�ȭ ��': 'const wchar_t [3]'���� 'KwStr'(��)�� ��ȯ�� �� �����ϴ�.
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
		//error C2440: 'static_cast': 'KTStr<wchar_t> *'���� 'std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t>>'(��)�� ��ȯ�� �� �����ϴ�.
		//error C2680 : 'std::basic_string<wchar_t,std::char_traits<wchar_t>,std::allocator<wchar_t>>' : dynamic_cast�� ��� ������ �߸��Ǿ����ϴ�.
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
��ó: https://blockdmask.tistory.com/88 [������ ������]
map ���� �ٸ���
operator [] �� ����ؼ� ����(pair<key, value>)�� �߰� �Ǵ� ������ �Ұ��� �մϴ�.
- key ���� �ش�Ǵ� �� ������ ������ "����" �� ����Ű�� �ݺ��ڸ� ��ȯ�մϴ�.
- �󱸰� " ) " ���� ���˴ϴ�.

mm.lower_bound(key);
- key ���� �ش��ϴ� �� ù��° ���Ҹ� ����Ű�� �ݺ��ڸ� ��ȯ�մϴ�.
- ������ " [ " ���� ���˴ϴ�.

mm.equal_range(key);
- key ���� �ش��ϴ� ������ "����" �� pair ��ü�� ��ȯ�մϴ�.

- pair ��ü�� first �� key ���� �ش��ϴ� ������ ù��° �ݺ��ڸ� ��ȯ. (lower_bound)
- pair ��ü�� second �� key ���� �ش��ϴ� ������ ������ ������ "����" �ݺ��ڸ� ��ȯ. (upper_bound)
	
	
	mm.insert(pair<int, int>(60, 6));


��ó: https://blockdmask.tistory.com/88 [������ ������]
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
	/// �׳� for(auto& [k,v] : *this) ���� �Ǵµ� ���� ���ٸ� reverse������
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
///		toString(); �ȿ� ����� �۾� �ϰ� ������ �Ѵ�.
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
	/// �׳� for(auto& [k,v] : *this) ���� �Ǵµ� ���� ���ٸ� reverse������
	virtual void toString()
	{
	}
	TVal Get(TKey key)
	{
		TVal v;
		Lookup(key, v);
		return v;
	}

	
	/// Ex ���� ���� ���ϰ��� ���� break ���ΰ� ��������.
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

// TKey�� string, wstring ���� �ϴ�.
// �̰� ���� ���� ��. KStdMap�ϰ� ���� �޶�
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

// ���� �����͸� ����� �߿� ���� �־ ������ std ���� ��� �ڴ�.
/// TKey�� std::string �Ǵ� std::wstring �� ����.
/// TObj�� �����ͷ� *�� ���� �Ѵ�.
/// m_bAutoFree�� �ǵ帮�� �ʴ� ��, ��ü free�ȴ�.
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
			auto v = new T;// �⺻ �����ڰ� �ִ� ��ü�� ���ؼ� �̸� �Ҵ� �� ���� �� �ִ�.
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

/// wstring <-> string ��ȣ��ȯ
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