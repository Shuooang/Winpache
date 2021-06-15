#pragma once

#include <map>
#include <functional>
#include "stdecl.h"

/// //////////////////////////////////////
/// level 3
/// ����� C++ �⺻ �Լ��� ���� ��� �ִ´�.
/// �׿ܴ� �� level 4 �̴�.
/// stdio.h �� <functional> <memory> ��


//////////////////////////////////////////////////////////////////////////
//  ----------- ZERO MEMORY ----------------------
// ���� ������ �� ������ �˰� ������..
// CRect m_arRcDay [4]; �϶�
// KwZeroStructPtr(&m_arRcDay, 4); 
//?warning �迭���� �ٷ� �ѱ�� �ȵȴ�.
// �̷��� �ϸ� 64*4=256 �� �ȴ�.
//  len	256	int
//+	pbuf	0x017b8854	CRect [4]*
//
// ��� ����� ....
// ���� �ΰ��� ����� �ϳ��� �� �ȴ�. ���� ��� ���´�.
//	KwZeroMemory(m_arRcDay);
//	KwZeroArray(m_arRcDay, 4);

// TSTRUCT �� CRect(*)[3] �ϼ��� �ִ�.
//?warning ���� �迭�̸� �迭������ ���� �ָ� �ȵȴ�. ���� �˾Ƽ� �������� �˾Ƴ���..
template<typename TYPE>
inline int KwZeroMemory(TYPE& sbuf)
{
	int len = sizeof(TYPE);//return �ҷ���
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

// pTar�� struct  �̸� �׳� m_lf = lf; ��  assign �ϸ� �Ǵµ�.. �̰� �� �������?
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



// �� ��ü�� C++���� ���ÿ��� ���� ����, ��ü�� �Ҹ� �Ҷ�, �̸� ������ ���� �Ѵ�.
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

	//����� ���� �Լ��� ���� ���� �ʴ´�.
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



// CKwAutoPtr: pointer�� ���ο� ���� �ϰ� �־ pointer ��üó�� ������ CKwAutoPtr ����
// this: ���� pointer�� �Ѱ� �ָ� �ڵ� free�ȴ�.
// auto free���� �������� Detatch �Ͽ� �������� �ѱ��.
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
		ASSERT(m_bOwner);//Detatch �� �ߺ� ���� �Ǹ� �ȵ�.
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
		ASSERT(m_p == NULL); //���� �����͸� Free�ϴ��� Detatch�ϴ��� �ϰ� �� ���� �־����.���� ���� ���� ���� ����� ��... ����
		if(m_p != ptr)
		{
			Free();
			Attach(ptr, true);
		}
		return m_p;
	}
	void Attach(T* p, bool bOwner = true)
	{
		ASSERT(m_p == NULL); // �����Ϳ� ���� Ȯ���� ó���� ���� Free()�� �θ� �� Attach �ϵ��� ���� �Ѵ�.
		m_bOwner = bOwner;
		m_p = p;
	}

};

/* Dictionary.h �� std::vector base�� ���� ����. �ȿ��� �鿩�� ���� �ִ� std:: �� ������
template<typename TObj>
class KPtrArray
	: public CArray<TObj*, TObj*>
	//, public IIncludeIndexArray<TObj*>
	//, public ICollectionBase
{	// _CArrayAutoPtr_ old name

	bool m_bAutoDelete;//DeleteAll()  ���� ���� �ȴ�.
public:
	typedef TObj TPtr;
	KPtrArray(bool bAutoDelete = true)
		: m_bAutoDelete(bAutoDelete)//���� ���� �ڵ� free���� �ʵ��� �� �� �ִ�.SetAutoFree(false)
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

	// �迭���� item �ϳ��� free�� �ϰ� RemoveAt �Ѵ�.
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
			ASSERT(pt == NULL);//�̹� �ִٸ� NULL�̾����.
		}
		__super::SetAt(idx, p);
	}

	// overwrite without free
	void PutAt(INT_PTR idx, TObj* p)
	{
		__super::SetAt(idx, p);
	}
	// Add �� Ŀ�� �Ѵ�.
	void SetAtGrow(INT_PTR idx, TObj* p)
	{
		int n = this->GetCount();
		if(n > idx)
		{
			TObj*& pt = this->GetAt(idx);
			ASSERT(pt == NULL);//�̹� �ִٸ� NULL�̾����.
		}
		//	FreeAt(idx);//���� �ִ����� free�ؾ���
		__super::SetAtGrow(idx, p);
		for(int i = n; i < idx; i++) // ������ ũ�� ��ŭ NULL�� ä���.
			SetAt(i, NULL); //  �ݵ�� NULL�� �־� �ξ�� �Ѵ�. �׷��� ������ free �ȴ�����
	}

	virtual void InsertAt(INT_PTR nIndex, TObj* newElement, INT_PTR nCount = 1)
	{
		__super::InsertAt(nIndex, newElement, nCount);
	}

	// �迭���� item �ϳ��� RemoveAt �� �ϰ� FreeAt�� ���Ѵ�.
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
//��ó: https://jangjy.tistory.com/359 [��ٺ���..]




//��: KwReplaceStrBelongedFast������.
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
		ss1.Preallocate(len * 3);// += �ӵ� ����� ����
		TCH* p = (TCH*)bf1;
		while (*p)
		{
			TCH* p1 = tchstr(p, sfind);//L"\\n");
			if (p1)
			{
				TCH wc = sfind[0];
				*p1 = '\0';//�ӽ÷� ���ڿ� ��
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
	// ������ ���� ������ ���� �Ǿ��ٰ� ����
	shared_ptr<function<TRAMBD>> _fncGET;

	// ���� ���� : TFNC == function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)> �� ���̱� ����
	shared_ptr<function<int(HTTPCacheSession*, HTTPRequest&, HTTPResponse&)>> _fncGET;

	// �Ҵ�
	template<typename TFNC> void AddCallbackOnGET(TFNC fnc)
	{
		TCreateFuncValue(_fncGET, fnc);
		//_fncGET = shared_ptr<function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>>(new function<int(HTTPSCacheSession*, HTTPRequest&, HTTPResponse&)>(fnc));
	}*/
	rtval = shared_ptr<TFNC>(new TFNC(lambda)); //�Ʒ��� ����
	//rtval = std::make_shared<TFNC>(lambda);//error C2664: 'std::function<int (SOCKADDR_IN &,std::shared_ptr<char>,int,std::shared_ptr<char> &,int &)>::function(std::nullptr_t) noexcept': �μ� 1��(��) 'TRAMBD'���� 'std::nullptr_t'(��)�� ��ȯ�� �� �����ϴ�.
	
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
