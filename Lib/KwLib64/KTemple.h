#pragma once

#include <map>
#include <functional>
#include "stdecl.h"

/// //////////////////////////////////////
/// level 3
/// 여기는 C++ 기본 함수만 쓰는 경우 넣는다.
/// 그외는 다 level 4 이다.
/// stdio.h 나 <functional> <memory> 등


//////////////////////////////////////////////////////////////////////////
//  ----------- ZERO MEMORY ----------------------
// 실제 포인터 와 갯수만 알고 있을때..
// CRect m_arRcDay [4]; 일때
// KwZeroStructPtr(&m_arRcDay, 4); 
//?warning 배열명을 바로 넘기면 안된다.
// 이렇게 하면 64*4=256 이 된다.
//  len	256	int
//+	pbuf	0x017b8854	CRect [4]*
//
// 사용 방법은 ....
// 다음 두가지 방법중 하나로 다 된다. 같은 결과 나온다.
//	KwZeroMemory(m_arRcDay);
//	KwZeroArray(m_arRcDay, 4);

// TSTRUCT 가 CRect(*)[3] 일수도 있다.
//?warning 원래 배열이면 배열갯수을 따로 주면 안된다. 지가 알아서 갯수까지 알아내니..
template<typename TYPE>
inline int KwZeroMemory(TYPE& sbuf)
{
	int len = sizeof(TYPE);//return 할려고
	memset((void*)(&sbuf), 0, len);
	return len;
}
template<typename TYPE>
inline int KwZeroArray(TYPE* pbuf, int nCount = 1)
{
	int len = sizeof(TYPE) * nCount;
	memset((void*)(pbuf), 0, len);
	return len;
}

// pTar가 struct  이면 그냥 m_lf = lf; 로  assign 하면 되는데.. 이건 뫠 만들었지?
template<typename TYPE, typename TYPE2>
inline int KwMemCpyP(TYPE* pTar, TYPE2* pSrc)
{
	int len = sizeof(TYPE);
	memcpy((void*)(pTar), (void*)(pSrc), len);
	return len;
}
template<typename TYPE, typename TYPE2>
inline int KwMemCpy(TYPE& pTar, TYPE2& pSrc)
{
	int len = sizeof(TYPE);
	memcpy((void*)(&pTar), (void*)(&pSrc), len);
	return len;
}



// 이 객체는 C++에서 스택에서 벗어 날때, 객체가 소멸 할때, 미리 할일을 예약 한다.
class KAtEnd
{
public:
	shared_ptr<std::function<void()>> m_pLambda;
	CStringA m_fnc;
	int m_line;
	bool _goOn;
	template<typename Func>
	KAtEnd(Func lambda, LPCSTR fnc = NULL, int line = 0)
		: m_pLambda(NULL)
		, m_fnc(fnc)
		, m_line(line)
		, _goOn(true)
	{
		m_pLambda = make_shared<std::function<void()>>(lambda);
// #ifdef _DEBUG
// 			DEBUG_NEW
// #else
// 			new
// #endif // _DEBUG
// 			std::function<void()>(lambda);
	}
	~KAtEnd()
	{
		if(_goOn && m_pLambda)
		{
			(*m_pLambda)();
			//delete m_pLambda;
		}
	}

	//에약된 람다 함수를 실행 하지 않는다.
	void Abort()
	{
		_goOn = false; 
	}
	/*
		IStream* stream = (IStream*)responseVariant.punkVal;
		KAtEnd d_stream([&]() {
				if(stream)
					stream->Release();
		});
		CHAR *szBuff = new CHAR[csz];
		//CAutoFreePtr<CHAR> au(szBuff);
		KAtEnd d_szBuff([szBuff]() {
				delete szBuff;
		});

	*/
};



// CKwAutoPtr: pointer를 내부에 보유 하고 있어서 pointer 객체처럼 쓰려면 CKwAutoPtr 참조
// this: 단지 pointer를 넘겨 주면 자동 free된다.
// auto free하지 않으려면 Detatch 하여 소유권을 넘긴다.
template< typename T >
class CAutoFreePtr
{
public:
	T*& m_p;
	bool m_bOwner;

	explicit CAutoFreePtr(T*& p)
		throw()
		: m_p(p)
		, m_bOwner(true)
	{
		if(m_p == NULL)
			m_bOwner = false;
	}
	virtual ~CAutoFreePtr() throw()
	{
		Free();
	}
	virtual void Free() throw()
	{
		if(m_bOwner)
			DeleteMeSafe(m_p);
	}
	T* Detatch()
	{
		ASSERT(m_bOwner);//Detatch 가 중복 실행 되면 안되.
		ASSERT(m_p);
		T* p = m_p;
		m_p = NULL;
		m_bOwner = false;
		return p;
	}
	operator T* () const throw()
	{
		return m_p;
	}
	T& operator*() const
	{
		ATLENSURE(m_p!=NULL);
		return *m_p;
	}
	T** operator&() throw()
	{
		ATLASSERT(m_p==NULL);
		return &m_p;
	}
	T* operator->() const throw()
	{
		ATLASSERT(m_p!=NULL);
		return m_p;
	}
	bool operator!() const throw()
	{
		return (m_p == NULL);
	}
	bool operator<(_In_opt_ T* pT) const throw()
	{
		return m_p < pT;
	}
	bool operator!=(_In_opt_ T* pT) const
	{
		return !operator==(pT);
	}
	bool operator==(_In_opt_ T* pT) const throw()
	{
		return m_p == pT;
	}
	T* operator =(T* ptr)
	{
		ASSERT(m_p == NULL); //기존 포인터를 Free하던가 Detatch하던가 하고 새 값을 넣어야지.아직 까진 덮어 쓰는 기능은 좀... 위험
		if(m_p != ptr)
		{
			Free();
			Attach(ptr, true);
		}
		return m_p;
	}
	void Attach(T* p, bool bOwner = true)
	{
		ASSERT(m_p == NULL); // 이전것에 대한 확실한 처리를 위해 Free()를 부른 후 Attach 하도록 규정 한다.
		m_bOwner = bOwner;
		m_p = p;
	}

};

/* Dictionary.h 에 std::vector base로 새로 만듦. 안에를 들여다 볼수 있는 std:: 를 쓰려고
template<typename TObj>
class KPtrArray
	: public CArray<TObj*, TObj*>
	//, public IIncludeIndexArray<TObj*>
	//, public ICollectionBase
{	// _CArrayAutoPtr_ old name

	bool m_bAutoDelete;//DeleteAll()  에만 적용 된다.
public:
	typedef TObj TPtr;
	KPtrArray(bool bAutoDelete = true)
		: m_bAutoDelete(bAutoDelete)//때에 따라서 자동 free되지 않도록 할 수 있다.SetAutoFree(false)
	{
	}
	virtual ~KPtrArray()
	{
		DeleteAll();
	}
	void SetAutoFree(bool bAutoDelete=true)
	{
		m_bAutoDelete = bAutoDelete;
	}
	void SetOwner(bool bAutoDelete=true)
	{
		m_bAutoDelete = bAutoDelete;
	}
	bool GetOwner()
	{
		return m_bAutoDelete;
	}
	virtual void DeleteAll()
	{
		FreeAt(0, CArray<TObj*, TObj*>::GetCount());
		RemoveAll();
	}
	virtual void FreeAt(int index, INT_PTR nCount = 1)
	{
		if(m_bAutoDelete)
		{
			for(int i=index;i<index+nCount;i++)
			{
				TObj*& pt = this->GetAt(i);
				DeleteMeSafe(pt);
			}
		}
	}

	// 배열안의 item 하나를 free도 하고 RemoveAt 한다.
	virtual void DeleteAt(int index, int nCount = 1)
	{
		FreeAt(index, nCount);
		RemoveAt(index, nCount);
	}

	void SetAt(INT_PTR idx, TObj* p)
	{
		if(this->GetCount() > idx)
		{
			TObj*& pt = this->GetAt(idx);
			ASSERT(pt == NULL);//이미 있다면 NULL이어야지.
		}
		__super::SetAt(idx, p);
	}

	// overwrite without free
	void PutAt(INT_PTR idx, TObj* p)
	{
		__super::SetAt(idx, p);
	}
	// Add 도 커버 한다.
	void SetAtGrow(INT_PTR idx, TObj* p)
	{
		int n = this->GetCount();
		if(n > idx)
		{
			TObj*& pt = this->GetAt(idx);
			ASSERT(pt == NULL);//이미 있다면 NULL이어야지.
		}
		//	FreeAt(idx);//원래 있던값은 free해야지
		__super::SetAtGrow(idx, p);
		for(int i = n; i < idx; i++) // 부족한 크기 만큼 NULL로 채운다.
			SetAt(i, NULL); //  반드시 NULL을 넣어 두어야 한다. 그래야 강제로 free 안당하지
	}

	virtual void InsertAt(INT_PTR nIndex, TObj* newElement, INT_PTR nCount = 1)
	{
		__super::InsertAt(nIndex, newElement, nCount);
	}

	// 배열안의 item 하나를 RemoveAt 만 하고 FreeAt은 안한다.
	virtual void RemoveAt(int nIndex, int nCount = 1)
	{
		__super::RemoveAt(nIndex, nCount);
	}

	virtual void RemoveAll()
	{
		__super::RemoveAll();
	}

};
*/
template<typename ... Args> 
std::string KwFormat(const std::string& format, Args ... args)
{
	size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;// Extra space for '\0' 
	if (size <= 0) { throw std::runtime_error("Error during formatting."); } 
	std::unique_ptr<char[]> buf(new char[size]); 
	snprintf(buf.get(), size, format.c_str(), args ...); 
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside 
}
//출처: https://jangjy.tistory.com/359 [살다보니..]




//구: KwReplaceStrBelongedFast복사함.
template<typename TSTR, typename TCH>
TSTR KwReplaceStr(TSTR src, const TCH* sfind, const TCH* newStr)
{
	BOOL b = FALSE;
	TSTR ss1;
	typedef const TCH* TPS;
	int len = tchlen((TPS)src);
	if (len > 0)
	{
		int lfind = tchlen((TPS)sfind);
		TSTR bfs;
		TPS bf1 = (TPS)bfs.GetBuffer(len + 1);//new WCHAR[len +1];
		tchcpy(bf1, (TPS)src);
		ss1.Preallocate(len * 3);// += 속도 향상을 위해
		TCH* p = (TCH*)bf1;
		while (*p)
		{
			TCH* p1 = tchstr(p, sfind);//L"\\n");
			if (p1)
			{
				TCH wc = sfind[0];
				*p1 = '\0';//임시로 문자열 끝
				ss1 += p;
				ss1 += newStr;//(WCHAR)'\n';
				*p1 = wc;//'\\';
				p = p1 + lfind;
			}
			else
			{
				ss1 += p;
				break;
			}
		}
		//DeleteMeSafe(bf1);
	}
	return ss1;
}


template<typename tk, typename tv>
void KwClearMapPtrVal(std::map<tk, tv> m)
{

	for(auto& [key, val] : m)
	{
		DeleteMeSafe(val);
		//std::cout << key << " => " << val << '\n';
	}
	m.clear();
	//std::map<k, v>::iterator iter;
/*	for(auto iter = this->begin(); iter != this->end(); iter++)
	{
		auto name = (*iter).first;
		auto val = (*iter).second;
		DeleteMeSafe(val);
	}*/
}

template<typename TFNC, typename TRAMBD>
void TSetLambdaToSharedFunction(shared_ptr<TFNC>& rtval, TRAMBD lambda)
{
	/* ex
	// 다음과 같은 변수가 선언 되었다고 전제
	shared_ptr<function<TRAMBD>> _fncGET;

	// 변수 선언 : TFNC == function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)> 을 줄이기 위해
	shared_ptr<function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)>> _fncGET;

	// 할당
	template<typename TFNC> void AddCallbackOnGET(TFNC fnc)
	{
		TCreateFuncValue(_fncGET, fnc);
		//_fncGET = shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>>(new function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>(fnc));
	}*/
	rtval = shared_ptr<TFNC>(new TFNC(lambda)); //아래와 동일
	//rtval = std::make_shared<TFNC>(lambda);//error C2664: 'std::function<int (SOCKADDR_IN &,std::shared_ptr<char>,int,std::shared_ptr<char> &,int &)>::function(std::nullptr_t) noexcept': 인수 1을(를) 'TRAMBD'에서 'std::nullptr_t'(으)로 변환할 수 없습니다.
	
}

/// <summary>
/// min <= v <= maxv
/// </summary>
/// <typeparam name="TVal"></typeparam>
/// <param name="v"></param>
/// <param name="minv">inclusive</param>
/// <param name="maxv">inclusive</param>
template<typename TVal>
void KwSetMinMax(TVal& v, TVal minv, TVal maxv)
{
	if(v > maxv)
		v = maxv;
	if(v < minv)
		v = minv;
}
template<typename TVal>
bool KwIsInside(TVal& v, TVal minv, TVal maxv)
{
	return (minv <= v && v <= maxv);
}
