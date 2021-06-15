#pragma once

#include <stdlib.h>
#include <memory.h>

#define _Use_NodeIndi

#include "enumval.h"

//#ifndef __AFX_H__
//#ifndef __POSITION
//struct __POSITION {};
//#endif // POSITION
//#endif // __AFX_H__
//
//#ifndef POSITION
//typedef __POSITION* POSITION;
//#endif // POSITION
#ifndef _AFX
struct __POSITION
{
};
#endif
typedef __POSITION* POSITION;
#ifndef HRESULT
typedef long HRESULT;
#endif // HRESULT

#ifdef RC_INVOKED
#define _HRESULT_TYPEDEF_(_sc) _sc
#else // RC_INVOKED
#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)
#endif // RC_INVOKED

#ifndef E_INVALIDARG

#if defined(_WIN32) && !defined(_MAC)
#define E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x8007000EL)
#define E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80070057L)
#else
#define E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x80000002L)
#define E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80000003L)
#endif

#define S_OK                                   ((HRESULT)0L)
#define S_FALSE                                ((HRESULT)1L)
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#endif // E_INVALIDARG


#ifndef INT_PTR
#if defined(_WIN64)
typedef __int64 INT_PTR, * PINT_PTR;
typedef unsigned __int64 UINT_PTR, * PUINT_PTR;

typedef __int64 LONG_PTR, * PLONG_PTR;
typedef unsigned __int64 ULONG_PTR, * PULONG_PTR;

#define __int3264   __int64

#else
typedef _W64 int INT_PTR, * PINT_PTR;
typedef _W64 unsigned int UINT_PTR, * PUINT_PTR;

typedef _W64 long LONG_PTR, * PLONG_PTR;
typedef _W64 unsigned long ULONG_PTR, * PULONG_PTR;

#define __int3264   __int32

#endif
#endif // INT_PTR


#ifndef ECMPOP // from KTempl.h
template<typename T>
inline int CompareIntValT(T ps1, T ps2)
{
	return (ps1 == ps2)?0: (ps1 < ps2)?-1:1;
}
//
//inline int CompareIntPtr(INT_PTR ps1, INT_PTR ps2)
//{
//	return CompareIntValT(ps1, ps2);
//}
//inline int CompareInt(int ps1, int ps2)
//{
//	return CompareIntValT(ps1, ps2);
//}
//inline int CompareDouble(double ps1, double ps2)
//{
//	double df = (double)(ps1 - ps2);
//	return df < 0.f ? -1 : df > 0.f ? 1 : 0;
//}
//inline int CompareFloat(float ps1, float ps2)
//{
//	float df = (float)(ps1 - ps2);
//	return df < 0.f ? -1 : df > 0.f ? 1 : 0;
//}
#endif // ECMPOP


// limits.h 
#ifndef INT_MIN
#define SHRT_MIN    (-32768)        /* minimum (signed) short value */
#define SHRT_MAX      32767         /* maximum (signed) short value */
#define USHRT_MAX     0xffff        /* maximum unsigned short value */
#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */
#define INT_MAX       2147483647    /* maximum (signed) int value */
#define UINT_MAX      0xffffffff    /* maximum unsigned int value */
#define LONG_MIN    (-2147483647L - 1) /* minimum (signed) long value */
#define LONG_MAX      2147483647L   /* maximum (signed) long value */
#define ULONG_MAX     0xffffffffUL  /* maximum unsigned long value */
#define LLONG_MAX     9223372036854775807i64       /* maximum signed long long int value */
#define LLONG_MIN   (-9223372036854775807i64 - 1)  /* minimum signed long long int value */
#define ULLONG_MAX    0xffffffffffffffffui64       /* maximum unsigned long long int value */
#endif // _DEBUG






#ifndef kassert
#define kassert(b) (b)
#endif // _DEBUG

#define KWENSURE0(b) kassert((b))
#define KWASSERT0(b) kassert((b))
#define KWASSUME0(b) kassert((b))

//#define KwThrow0(b) throw((b))
#define VThrow()
//#pragma push_macro("new")
//#undef new


//
// The red-black tree code is based on the the descriptions in
// "Introduction to Algorithms", by Cormen, Leiserson, and Rivest
//
/*class IRBTree
{
public:
	virtual size_t GetCount() const VThrow() = NULL;
};*/

#pragma warning( disable : 4290 )

// C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.28.29333\atlmfc\include\
//		atlcoll.h
template< typename K, typename V>
class CRBTree0// : public IRBTree
{
public:
	typedef POSITION iterator;
	//typedef typename K K;
	//typedef typename K& KOUTARGTYPE;
	//typedef typename V V;
	//typedef typename V& VOUTARGTYPE;

	// 이전 CTMap 과 코드 변환 호환을 위해
	//typedef typename K		THsKey;//TKEY;   // 저장할때
	//typedef typename K		THsKeyArg;//TKEYARG;// 넣을때
	//typedef typename V		THsVal;//TVAL;   // 저장/넣을때 같다.
	//typedef typename V		THsValArg;//TVALARG;   // 저장/넣을때 같다.

public:
	virtual int _Compare(const K* ps1, const K* ps2, size_t len = -1, ECmpOp iOp = eCmprCase) const VThrow()
	{
		//  		ASSERT(0); 굳이 할필요 있나? operator > < == 이 안되면 오류 나것지.
		return (*ps1 > *ps2) ? 1 : ((*ps1 < *ps2) ? -1 : 0);
	}
	virtual bool _CompareB(const K* ps1, const K* ps2, size_t len = -1, ECmpOp iOp = eCmprCase) const VThrow()
	{
		return _Compare(ps1, ps2, len, iOp) == 0;
	}
	// 	static bool CompareElements( const T& element1, const T& element2 )
	// 	{
	// 		return( (element1 == element2) != 0 );  // != 0 to handle overloads of operator== that return BOOL instead of bool
	// 	}


	class CPair : public __POSITION
	{
	protected:
		CPair(K key, V value): m_key(key), m_value(value){}
		~CPair() VThrow(){}
	public:
		//const //3>s:\projsmo\smo\src\kwlib32\rbbase.h(503) : error C2758: 'CRBTree0<K,V>::CPair::m_key' : 생성자 기본/멤버 이니셜라이저 목록에 초기화해야 합니다.
		K m_key;
		V m_value;
	};

public:
	class CNode : public CPair
	{
	public:
		enum RB_COLOR
		{
			RB_RED,
			RB_BLACK
		};

	public:
		RB_COLOR m_eColor;
		CNode* m_pLeft;
		CNode* m_pRight;
		CNode* m_pParent;

		CNode(K key, V value)
			: CPair(key, value)
			, m_pParent(NULL)
			, m_eColor(RB_BLACK)
			, m_pLeft(NULL)
			, m_pRight(NULL)
		{}
		~CNode() VThrow() {}
	};

private:
	CNode* m_pRoot;
	size_t m_nCount;
	// sentinel node
	CNode* m_pNil;

	// methods
	bool IsNil(CNode* p) const VThrow();
	void SetNil(CNode** p) VThrow();

	CNode* NewNode(K key, V value) VThrow();//( ... );
	void FreeNode(CNode* pNode) VThrow();
	void RemovePostOrder(CNode* pNode) VThrow();
	CNode* LeftRotate(CNode* pNode) VThrow();
	CNode* RightRotate(CNode* pNode) VThrow();
	void SwapNode(CNode* pDest, CNode* pSrc) VThrow();
	CNode* InsertImpl(K key, V value) VThrow();//( ... );
	void RBDeleteFixup(CNode* pNode) VThrow();
	bool RBDelete(CNode* pZ) VThrow();

#ifdef _DEBUG

	// internal debugging code to verify red-black properties of tree:
	// 1) Every node is either red or black
	// 2) Every leaf (NIL) is black
	// 3) If a node is red, both its children are black
	// 4) Every simple path from a node to a descendant leaf node contains 
	//    the same number of black nodes
private:
	void VerifyIntegrity(const CNode* pNode, int nCurrBlackDepth, int& nBlackDepth) const VThrow();

public:
	void VerifyIntegrity() const VThrow();

#endif // _DEBUG

protected:
	CNode* Minimum(CNode* pNode) const VThrow();
	CNode* Maximum(CNode* pNode) const VThrow();
	CNode* Predecessor(CNode* pNode) const VThrow();
	CNode* Successor(CNode* pNode) const VThrow();
	CNode* RBInsert(K key, V value) VThrow();
	CNode* Find(K key) const VThrow();
	CNode* FindPrefix(K key) const VThrow();

protected:
	explicit CRBTree0(size_t nBlockSize = 10) VThrow();  // protected to prevent instantiation

public:
	~CRBTree0() VThrow();

	void RemoveAll() VThrow();
	void RemoveAt(POSITION pos) VThrow();

	size_t size() const VThrow() { return GetCount(); }
	size_t GetCount() const VThrow();
	bool IsEmpty() const VThrow();

	POSITION FindFirstKeyAfter(K key) const VThrow();

	POSITION GetHeadPosition() const VThrow();
	POSITION GetStartPosition() const VThrow() //추가됨
	{
		return GetHeadPosition();
	}
	POSITION begin() const VThrow() { return GetHeadPosition(); }

	POSITION GetTailPosition() const VThrow();
	POSITION end() const VThrow() { return (POSITION)NULL; }
	void GetNextAssoc(POSITION& pos, K& key, V& value) const;
	const CPair* GetNext(POSITION& pos) const VThrow();
	CPair* GetNext(POSITION& pos) VThrow();
	const CPair* GetPrev(POSITION& pos) const VThrow();
	CPair* GetPrev(POSITION& pos) VThrow();
	const K& GetNextKey(POSITION& pos) const VThrow();
	const V& GetNextValue(POSITION& pos) const VThrow();
	V& GetNextValue(POSITION& pos) VThrow();

	CPair* GetAt(POSITION pos) VThrow();
	CPair* GetHeadAt() VThrow();
	CPair* GetTailAt() VThrow();
	const CPair* GetAt(POSITION pos) const VThrow();
	void GetAt(POSITION pos, K& key, V& value) const;
	const K& GetKeyAt(POSITION pos) const;
	const V& GetValueAt(POSITION pos) const;
	V& GetValueAt(POSITION pos);
	void SetValueAt(POSITION pos, V value);

private:
	// Private to prevent use
	CRBTree0(const CRBTree0&) VThrow();
	CRBTree0& operator=(const CRBTree0&) VThrow();



public:
#ifdef _DEBUG
	bool m_bDump;
	CString m_sDump0;
	// 덤프 횟수를 줄이기 위해서.. 가끔 반복 되는 작업에서 건너 띄도록 한다.
	int m_nDumpDeferred;
	virtual LPCTSTR DumpFmt()
	{
		return _T("%x:%x");
	}
#endif // _DEBUG
};

// key 가 int 인 경우만 미리 만들어 놓았다.
template<>
inline int CRBTree0< int, int >::_Compare(const int* ps1, const int* ps2, size_t len, ECmpOp iOp) const VThrow()
{
	return CompareIntValT(*ps1, *ps2);
}
template<>
inline int CRBTree0< int, unsigned int >::_Compare(const int* ps1, const int* ps2, size_t len, ECmpOp iOp) const VThrow()
{
	return CompareIntValT(*ps1, *ps2);
}
template<>
inline int CRBTree0< int, float>::_Compare(const int* ps1, const int* ps2, size_t len, ECmpOp iOp) const VThrow()
{
	return CompareIntValT(*ps1, *ps2);
}
template<>
inline int CRBTree0< int, double>::_Compare(const int* ps1, const int* ps2, size_t len, ECmpOp iOp) const VThrow()
{
	return CompareIntValT(*ps1, *ps2);
}

template<> // OK
inline int CRBTree0< unsigned int, unsigned int>::_Compare(const unsigned int* ps1, const unsigned int* ps2, size_t len, ECmpOp iOp) const VThrow()
{
	return CompareIntValT(*ps1, *ps2);
}


//error C2244: 'CRBTree0<K,V>::_Compare' : 함수 정의를 기존 선언과 일치시킬 수 없습니다.
// template<typename unsigned int, typename V>
// inline int CRBTree0< unsigned int, V>::_Compare(const unsigned int* ps1, const unsigned int* ps2, int len, ECmpOp iOp) const VThrow()
// {	return CompareIntValT(*ps1, *ps2);	}





template< typename K, typename V>
inline bool CRBTree0< K, V >::IsNil(CNode* p) const VThrow()
{
	return (p == m_pNil);
}

template< typename K, typename V >
inline void CRBTree0< K, V >::SetNil(CNode** p)
{
	KWENSURE0(p != NULL);
	*p = m_pNil;
}

template< typename K, typename V >
CRBTree0< K, V >::CRBTree0(size_t nBlockSize) VThrow()
	: m_pRoot(NULL)
	, m_nCount(0)
	, m_pNil(NULL)
#ifdef _DEBUG
	, m_bDump(true)
	, m_nDumpDeferred(0)
#endif // _DEBUG
{
	KWASSERT0(nBlockSize > 0);
}

template< typename K, typename V >
CRBTree0< K, V >::~CRBTree0() VThrow()
{
	RemoveAll();
	if(m_pNil)
		delete m_pNil;
}

template< typename K, typename V >
void CRBTree0< K, V >::RemoveAll() VThrow()
{
	if(!IsNil(m_pRoot))
		RemovePostOrder(m_pRoot);
	m_nCount = 0;

	m_pRoot = m_pNil;
}

template< typename K, typename V >
size_t CRBTree0< K, V >::GetCount() const VThrow()
{
	return m_nCount;
}

template< typename K, typename V >
bool CRBTree0< K, V >::IsEmpty() const VThrow()
{
	return(m_nCount == 0);
}

template< typename K, typename V >
POSITION CRBTree0< K, V >::FindFirstKeyAfter(K key) const VThrow()
{
	return(FindPrefix(key));
}

template< typename K, typename V >
void CRBTree0< K, V >::RemoveAt(POSITION pos) VThrow()
{
	KWASSERT0(pos != NULL);
	RBDelete(static_cast<CNode*>(pos));
}

template< typename K, typename V >
POSITION CRBTree0< K, V >::GetHeadPosition() const VThrow()
{
	return(Minimum(m_pRoot));
}

template< typename K, typename V >
POSITION CRBTree0< K, V >::GetTailPosition() const VThrow()
{
	return(Maximum(m_pRoot));
}

template< typename K, typename V >
void CRBTree0< K, V >::GetNextAssoc(POSITION& pos, K& key, V& value) const
{
	KWASSERT0(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);

	key = pNode->m_key;
	value = pNode->m_value;

	pos = Successor(pNode);
}

template< typename K, typename V >
const typename CRBTree0< K, V >::CPair* CRBTree0< K, V >::GetNext(POSITION& pos) const VThrow()
{
	KWASSERT0(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);
	return pNode;
}

template< typename K, typename V >
typename CRBTree0< K, V >::CPair* CRBTree0< K, V >::GetNext(POSITION& pos) VThrow()
{
	KWASSERT0(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);
	return pNode;
}

template< typename K, typename V >
const typename CRBTree0< K, V >::CPair* CRBTree0< K, V >::GetPrev(POSITION& pos) const VThrow()
{
	KWASSERT0(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Predecessor(pNode);

	return pNode;
}

template< typename K, typename V >
typename CRBTree0< K, V >::CPair* CRBTree0< K, V >::GetPrev(POSITION& pos) VThrow()
{
	KWASSERT0(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Predecessor(pNode);

	return pNode;
}

template< typename K, typename V >
const K& CRBTree0< K, V >::GetNextKey(POSITION& pos) const VThrow()
{
	KWASSERT0(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);

	return pNode->m_key;
}

template< typename K, typename V >
const V& CRBTree0< K, V >::GetNextValue(POSITION& pos) const VThrow()
{
	KWASSERT0(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);

	return pNode->m_value;
}

template< typename K, typename V >
V& CRBTree0< K, V >::GetNextValue(POSITION& pos) VThrow()
{
	KWASSERT0(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);

	return pNode->m_value;
}

template< typename K, typename V >
typename CRBTree0< K, V >::CPair* CRBTree0< K, V >::GetAt(POSITION pos) VThrow()
{
	KWASSERT0(pos != NULL);

	return(static_cast<CPair*>(pos));
}
template< typename K, typename V >
typename CRBTree0< K, V >::CPair* CRBTree0< K, V >::GetHeadAt() VThrow()
{
	POSITION pos = GetHeadPosition();
	return pos ? GetAt(pos) : NULL;
}
template< typename K, typename V >
typename CRBTree0< K, V >::CPair* CRBTree0< K, V >::GetTailAt() VThrow()
{
	POSITION pos = GetTailPosition();
	return pos ? GetAt(pos) : NULL;
}

template< typename K, typename V >
const typename CRBTree0< K, V >::CPair* CRBTree0< K, V >::GetAt(POSITION pos) const VThrow()
{
	KWASSERT0(pos != NULL);

	return(static_cast<const CPair*>(pos));
}

template< typename K, typename V >
void CRBTree0< K, V >::GetAt(POSITION pos, K& key, V& value) const
{
	KWENSURE0(pos != NULL);
	key = static_cast<CNode*>(pos)->m_key;
	value = static_cast<CNode*>(pos)->m_value;
}

template< typename K, typename V >
const K& CRBTree0< K, V >::GetKeyAt(POSITION pos) const
{
	KWENSURE0(pos != NULL);
	return static_cast<CNode*>(pos)->m_key;
}

template< typename K, typename V >
const V& CRBTree0< K, V >::GetValueAt(POSITION pos) const
{
	KWENSURE0(pos != NULL);
	return static_cast<CNode*>(pos)->m_value;
}

template< typename K, typename V >
V& CRBTree0< K, V >::GetValueAt(POSITION pos)
{
	KWENSURE0(pos != NULL);
	return static_cast<CNode*>(pos)->m_value;
}

template< typename K, typename V >
void CRBTree0< K, V >::SetValueAt(POSITION pos, V value)
{
	KWENSURE0(pos != NULL);
	static_cast<CNode*>(pos)->m_value = value;
}

template< typename K, typename V >
typename CRBTree0< K, V >::CNode* CRBTree0< K, V >::NewNode(K key, V value) VThrow()
{
	if(m_pNil == NULL)
	{
		#ifdef _DEBUG
				m_pNil = DEBUG_NEW CNode( key, value );
		#else
		m_pNil = new CNode(key, value);
		#endif // _DEBUG
		ASSERT(m_pNil);// == NULL)
//			KwThrow0( E_OUTOFMEMORY );
		m_pNil->m_eColor = CNode::RB_BLACK;
		m_pNil->m_pParent = m_pNil->m_pLeft = m_pNil->m_pRight = m_pNil;
		m_pRoot = m_pNil;
	}

	CNode* pNewNode =
		#ifdef _DEBUG
			DEBUG_NEW 
		#else
		new
		#endif // _DEBUG
		CNode(key, value);

	pNewNode->m_eColor = CNode::RB_RED;
	SetNil(&pNewNode->m_pLeft);
	SetNil(&pNewNode->m_pRight);
	SetNil(&pNewNode->m_pParent);

	m_nCount++;
	KWASSUME0(m_nCount > 0);

	return(pNewNode);
}

template< typename K, typename V >
void CRBTree0< K, V >::FreeNode(CNode* pNode)
{
	KWENSURE0(pNode != NULL);
	delete pNode;
	KWASSUME0(m_nCount > 0);
	m_nCount--;
}

template< typename K, typename V >
void CRBTree0< K, V >::RemovePostOrder(CNode* pNode) VThrow()
{
	if(IsNil(pNode))
		return;
	RemovePostOrder(pNode->m_pLeft);
	RemovePostOrder(pNode->m_pRight);
	FreeNode(pNode);
}

template< typename K, typename V >
typename CRBTree0< K, V >::CNode* CRBTree0< K, V >::LeftRotate(CNode* pNode) VThrow()
{
	KWASSERT0(pNode != NULL);
	if(pNode == NULL)
		return NULL;

	CNode* pRight = pNode->m_pRight;
	pNode->m_pRight = pRight->m_pLeft;
	if(!IsNil(pRight->m_pLeft))
		pRight->m_pLeft->m_pParent = pNode;

	pRight->m_pParent = pNode->m_pParent;
	if(IsNil(pNode->m_pParent))
		m_pRoot = pRight;
	else if(pNode == pNode->m_pParent->m_pLeft)
		pNode->m_pParent->m_pLeft = pRight;
	else
		pNode->m_pParent->m_pRight = pRight;

	pRight->m_pLeft = pNode;
	pNode->m_pParent = pRight;
	return pNode;

}

template< typename K, typename V >
typename CRBTree0< K, V >::CNode* CRBTree0< K, V >::RightRotate(CNode* pNode) VThrow()
{
	KWASSERT0(pNode != NULL);
	if(pNode == NULL)
		return NULL;

	CNode* pLeft = pNode->m_pLeft;
	pNode->m_pLeft = pLeft->m_pRight;
	if(!IsNil(pLeft->m_pRight))
		pLeft->m_pRight->m_pParent = pNode;

	pLeft->m_pParent = pNode->m_pParent;
	if(IsNil(pNode->m_pParent))
		m_pRoot = pLeft;
	else if(pNode == pNode->m_pParent->m_pRight)
		pNode->m_pParent->m_pRight = pLeft;
	else
		pNode->m_pParent->m_pLeft = pLeft;

	pLeft->m_pRight = pNode;
	pNode->m_pParent = pLeft;
	return pNode;
}

template< typename K, typename V >
typename CRBTree0< K, V >::CNode* CRBTree0< K, V >::Find(K key) const VThrow()
{
	CNode* pKey = NULL;
	CNode* pNode = m_pRoot;
	while(!IsNil(pNode) && (pKey == NULL))
	{
		int nCompare = _Compare(&key, &pNode->m_key);//KTraits::CompareElementsOrdered( key, pNode->m_key );
		if(nCompare == 0)
		{
			pKey = pNode;
		}
		else
		{
			if(nCompare < 0)
			{
				pNode = pNode->m_pLeft;
			}
			else
			{
				pNode = pNode->m_pRight;
			}
		}
	}

	if(pKey == NULL)
	{
		return(NULL);
	}

#pragma warning(push)
#pragma warning(disable:4127)

	while(true) // 중복 키 인 경우 같은 것 중에서 맨 앞에 것을 리턴 하기 위한 짓
	{
		CNode* pPrev = Predecessor(pKey);
		if((pPrev != NULL) && _CompareB(&key, &pPrev->m_key))//KTraits::CompareElements
		{
			pKey = pPrev;
		}
		else
		{
			return(pKey);
		}
	}

#pragma warning(pop)
}

template< typename K, typename V >
typename CRBTree0< K, V >::CNode* CRBTree0< K, V >::FindPrefix(K key) const VThrow()
{
	// First, attempt to find a node that matches the key exactly
	CNode* pParent = NULL;
	CNode* pKey = NULL;
	CNode* pNode = m_pRoot;
	while(!IsNil(pNode) && (pKey == NULL))
	{
		pParent = pNode;
		int nCompare = _Compare(&key, &pNode->m_key);//KTraits::CompareElementsOrdered( key, pNode->m_key );
		if(nCompare == 0)
		{
			pKey = pNode;
		}
		else if(nCompare < 0)
		{
			pNode = pNode->m_pLeft;
		}
		else
		{
			pNode = pNode->m_pRight;
		}
	}

	if(pKey != NULL)
	{
		// We found a node with the exact key, so find the first node after 
		// this one with a different key 
		while(true)
		{
			CNode* pNext = Successor(pKey);
			if((pNext != NULL) && _CompareB(&key, &pNext->m_key))//KTraits::CompareElements
			{
				pKey = pNext;
			}
			else
			{
				return pNext;
			}
		}
	}
	else if(pParent != NULL)
	{
		// No node matched the key exactly, so pick the first node with 
		// a key greater than the given key
		int nCompare = _Compare(&key, &pParent->m_key);//KTraits::CompareElementsOrdered( key, pParent->m_key );
		if(nCompare < 0)
		{
			pKey = pParent;
		}
		else
		{
			KWASSERT0(nCompare > 0);
			pKey = Successor(pParent);
		}
	}

	return(pKey);
}

template< typename K, typename V >
void CRBTree0< K, V >::SwapNode(CNode* pDest, CNode* pSrc)
{
	KWENSURE0(pDest != NULL);
	KWENSURE0(pSrc != NULL);

	pDest->m_pParent = pSrc->m_pParent;
	if(pSrc->m_pParent->m_pLeft == pSrc)
	{
		pSrc->m_pParent->m_pLeft = pDest;
	}
	else
	{
		pSrc->m_pParent->m_pRight = pDest;
	}

	pDest->m_pRight = pSrc->m_pRight;
	pDest->m_pLeft = pSrc->m_pLeft;
	pDest->m_eColor = pSrc->m_eColor;
	pDest->m_pRight->m_pParent = pDest;
	pDest->m_pLeft->m_pParent = pDest;

	if(m_pRoot == pSrc)
	{
		m_pRoot = pDest;
	}
}

template< typename K, typename V >
typename CRBTree0< K, V >::CNode* CRBTree0< K, V >::InsertImpl(K key, V value) VThrow()
{
	CNode* pNew = NewNode(key, value);

	CNode* pY = NULL;
	CNode* pX = m_pRoot;

	while(!IsNil(pX))
	{
		pY = pX;
		if(_Compare(&key, &pX->m_key) <= 0)//KTraits::CompareElementsOrdered( key, pX->m_key ) <= 0 )
			pX = pX->m_pLeft;
		else
			pX = pX->m_pRight;
	}

	pNew->m_pParent = pY;
	if(pY == NULL)
	{
		m_pRoot = pNew;
	}
	else if(_Compare(&key, &pY->m_key) <= 0)//if( KTraits::CompareElementsOrdered( key, pY->m_key ) <= 0 )
		pY->m_pLeft = pNew;
	else
		pY->m_pRight = pNew;

	return pNew;
}

template< typename K, typename V >
void CRBTree0< K, V >::RBDeleteFixup(CNode* pNode)
{
	KWENSURE0(pNode != NULL);

	CNode* pX = pNode;
	CNode* pW = NULL;

	while((pX != m_pRoot) && (pX->m_eColor == CNode::RB_BLACK))
	{
		if(pX == pX->m_pParent->m_pLeft)
		{
			pW = pX->m_pParent->m_pRight;
			if(pW->m_eColor == CNode::RB_RED)
			{
				pW->m_eColor = CNode::RB_BLACK;
				pW->m_pParent->m_eColor = CNode::RB_RED;
				LeftRotate(pX->m_pParent);
				pW = pX->m_pParent->m_pRight;
			}
			if(pW->m_pLeft->m_eColor == CNode::RB_BLACK && pW->m_pRight->m_eColor == CNode::RB_BLACK)
			{
				pW->m_eColor = CNode::RB_RED;
				pX = pX->m_pParent;
			}
			else
			{
				if(pW->m_pRight->m_eColor == CNode::RB_BLACK)
				{
					pW->m_pLeft->m_eColor = CNode::RB_BLACK;
					pW->m_eColor = CNode::RB_RED;
					RightRotate(pW);
					pW = pX->m_pParent->m_pRight;
				}
				pW->m_eColor = pX->m_pParent->m_eColor;
				pX->m_pParent->m_eColor = CNode::RB_BLACK;
				pW->m_pRight->m_eColor = CNode::RB_BLACK;
				LeftRotate(pX->m_pParent);
				pX = m_pRoot;
			}
		}
		else
		{
			pW = pX->m_pParent->m_pLeft;
			if(pW->m_eColor == CNode::RB_RED)
			{
				pW->m_eColor = CNode::RB_BLACK;
				pW->m_pParent->m_eColor = CNode::RB_RED;
				RightRotate(pX->m_pParent);
				pW = pX->m_pParent->m_pLeft;
			}
			if(pW->m_pRight->m_eColor == CNode::RB_BLACK && pW->m_pLeft->m_eColor == CNode::RB_BLACK)
			{
				pW->m_eColor = CNode::RB_RED;
				pX = pX->m_pParent;
			}
			else
			{
				if(pW->m_pLeft->m_eColor == CNode::RB_BLACK)
				{
					pW->m_pRight->m_eColor = CNode::RB_BLACK;
					pW->m_eColor = CNode::RB_RED;
					LeftRotate(pW);
					pW = pX->m_pParent->m_pLeft;
				}
				pW->m_eColor = pX->m_pParent->m_eColor;
				pX->m_pParent->m_eColor = CNode::RB_BLACK;
				pW->m_pLeft->m_eColor = CNode::RB_BLACK;
				RightRotate(pX->m_pParent);
				pX = m_pRoot;
			}
		}
	}

	pX->m_eColor = CNode::RB_BLACK;
}


template< typename K, typename V >
bool CRBTree0< K, V >::RBDelete(CNode* pZ) VThrow()
{
	if(pZ == NULL)
		return false;

	CNode* pY = NULL;
	CNode* pX = NULL;
	if(IsNil(pZ->m_pLeft) || IsNil(pZ->m_pRight))
		pY = pZ;
	else
		pY = Successor(pZ);

	if(!IsNil(pY->m_pLeft))
		pX = pY->m_pLeft;
	else
		pX = pY->m_pRight;

	pX->m_pParent = pY->m_pParent;

	if(IsNil(pY->m_pParent))
		m_pRoot = pX;
	else if(pY == pY->m_pParent->m_pLeft)
		pY->m_pParent->m_pLeft = pX;
	else
		pY->m_pParent->m_pRight = pX;

	if(pY->m_eColor == CNode::RB_BLACK)
		RBDeleteFixup(pX);

	if(pY != pZ)
		SwapNode(pY, pZ);

	if(m_pRoot != NULL)
		SetNil(&m_pRoot->m_pParent);

	FreeNode(pZ);

	return true;
}

template< typename K, typename V >
typename CRBTree0< K, V >::CNode* CRBTree0< K, V >::Minimum(CNode* pNode) const VThrow()
{
	if(pNode == NULL || IsNil(pNode))
	{
		return NULL;
	}

	CNode* pMin = pNode;
	while(!IsNil(pMin->m_pLeft))
	{
		pMin = pMin->m_pLeft;
	}

	return pMin;
}

template< typename K, typename V >
typename CRBTree0< K, V >::CNode* CRBTree0< K, V >::Maximum(CNode* pNode) const VThrow()
{
	if(pNode == NULL || IsNil(pNode))
	{
		return NULL;
	}

	CNode* pMax = pNode;
	while(!IsNil(pMax->m_pRight))
	{
		pMax = pMax->m_pRight;
	}

	return pMax;
}

template< typename K, typename V >
typename CRBTree0< K, V >::CNode* CRBTree0< K, V >::Predecessor(CNode* pNode) const VThrow()
{
	if(pNode == NULL)
	{
		return(NULL);
	}
	if(!IsNil(pNode->m_pLeft))
	{
		return(Maximum(pNode->m_pLeft));
	}

	CNode* pParent = pNode->m_pParent;
	CNode* pLeft = pNode;
	while(!IsNil(pParent) && (pLeft == pParent->m_pLeft))
	{
		pLeft = pParent;
		pParent = pParent->m_pParent;
	}

	if(IsNil(pParent))
	{
		pParent = NULL;
	}
	return(pParent);
}

template< typename K, typename V >
typename CRBTree0< K, V >::CNode* CRBTree0< K, V >::Successor(CNode* pNode) const VThrow()
{
	if(pNode == NULL)
	{
		return NULL;
	}
	if(!IsNil(pNode->m_pRight))
	{
		return Minimum(pNode->m_pRight);
	}

	CNode* pParent = pNode->m_pParent;
	CNode* pRight = pNode;
	while(!IsNil(pParent) && (pRight == pParent->m_pRight))
	{
		pRight = pParent;
		pParent = pParent->m_pParent;
	}

	if(IsNil(pParent))
	{
		pParent = NULL;
	}
	return pParent;
}

template< typename K, typename V >
typename CRBTree0< K, V >::CNode* CRBTree0< K, V >::RBInsert(K key, V value) VThrow()
{
	CNode* pNewNode = InsertImpl(key, value);

	CNode* pX = pNewNode;
	pX->m_eColor = CNode::RB_RED;
	CNode* pY = NULL;
	while(pX != m_pRoot && pX->m_pParent->m_eColor == CNode::RB_RED)
	{
		if(pX->m_pParent == pX->m_pParent->m_pParent->m_pLeft)
		{
			pY = pX->m_pParent->m_pParent->m_pRight;
			if(pY != NULL && pY->m_eColor == CNode::RB_RED)
			{
				pX->m_pParent->m_eColor = CNode::RB_BLACK;
				pY->m_eColor = CNode::RB_BLACK;
				pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
				pX = pX->m_pParent->m_pParent;
			}
			else
			{
				if(pX == pX->m_pParent->m_pRight)
				{
					pX = pX->m_pParent;
					LeftRotate(pX);
				}
				pX->m_pParent->m_eColor = CNode::RB_BLACK;
				pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
				RightRotate(pX->m_pParent->m_pParent);
			}
		}
		else
		{
			pY = pX->m_pParent->m_pParent->m_pLeft;
			if(pY != NULL && pY->m_eColor == CNode::RB_RED)
			{
				pX->m_pParent->m_eColor = CNode::RB_BLACK;
				pY->m_eColor = CNode::RB_BLACK;
				pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
				pX = pX->m_pParent->m_pParent;
			}
			else
			{
				if(pX == pX->m_pParent->m_pLeft)
				{
					pX = pX->m_pParent;
					RightRotate(pX);
				}
				pX->m_pParent->m_eColor = CNode::RB_BLACK;
				pX->m_pParent->m_pParent->m_eColor = CNode::RB_RED;
				LeftRotate(pX->m_pParent->m_pParent);
			}
		}
	}

	m_pRoot->m_eColor = CNode::RB_BLACK;
	SetNil(&m_pRoot->m_pParent);

	return(pNewNode);
}

#ifdef _DEBUG

template< typename K, typename V >
void CRBTree0< K, V >::VerifyIntegrity(const CNode* pNode, int nCurrBlackDepth, int& nBlackDepth) const VThrow()
{
	bool bCheckForBlack = false;
	bool bLeaf = true;

	if(pNode->m_eColor == CNode::RB_RED)
		bCheckForBlack = true;
	else
		nCurrBlackDepth++;

	KWASSERT0(pNode->m_pLeft != NULL);
	if(!IsNil(pNode->m_pLeft))
	{
		bLeaf = false;
		if(bCheckForBlack)
		{
			KWASSERT0(pNode->m_pLeft->m_eColor == CNode::RB_BLACK);
		}

		VerifyIntegrity(pNode->m_pLeft, nCurrBlackDepth, nBlackDepth);
	}

	KWASSERT0(pNode->m_pRight != NULL);
	if(!IsNil(pNode->m_pRight))
	{
		bLeaf = false;
		if(bCheckForBlack)
		{
			KWASSERT0(pNode->m_pRight->m_eColor == CNode::RB_BLACK);
		}

		VerifyIntegrity(pNode->m_pRight, nCurrBlackDepth, nBlackDepth);
	}

	KWASSERT0(pNode->m_pParent != NULL);
	KWASSERT0((IsNil(pNode->m_pParent)) ||
		(pNode->m_pParent->m_pLeft == pNode) ||
		(pNode->m_pParent->m_pRight == pNode));

	if(bLeaf)
	{
		if(nBlackDepth == 0)
		{
			nBlackDepth = nCurrBlackDepth;
		}
		else
		{
			KWASSERT0(nBlackDepth == nCurrBlackDepth);
		}
	}
}

template< typename K, typename V >
void CRBTree0< K, V >::VerifyIntegrity() const VThrow()
{
	if((m_pRoot == NULL) || (IsNil(m_pRoot)))
		return;

	KWASSUME0(m_pRoot->m_eColor == CNode::RB_BLACK);
	int nBlackDepth = 0;
	VerifyIntegrity(m_pRoot, 0, nBlackDepth);
}

#endif // _DEBUG

template< typename K, typename V/*, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > */>
class CRBMap0 :
	public CRBTree0< K, V >
{
public:
	//typedef typename K K;//c++17 VS2019 부터 class내 typedef이 장속이 안된다. 그래서 자식들이 다시 해준다.
	//typedef typename V V;

	explicit CRBMap0(size_t nBlockSize = 10) VThrow();
	virtual ~CRBMap0() VThrow();
	friend CRBTree0< K, V >::CNode;
	friend CRBTree0< K, V >::CPair;
	virtual bool Lookup(K key, V& value) VThrow();//const
	//virtual const CRBTree0< K, V >::CNode* Lookup( K key ) VThrow();//const
	// const 놈을 제거 한다.

	virtual CRBTree0< K, V >::CNode* Lookup(K key) VThrow();

	virtual const typename V& GetVaule(K key) VThrow();

	virtual POSITION SetAt(K key, V value) VThrow();
	virtual bool RemoveKey(K key) VThrow();
};

template< typename K, typename V >
CRBMap0< K, V >::CRBMap0(size_t nBlockSize) VThrow() :
	CRBTree0< K, V >(nBlockSize)
{
}

template< typename K, typename V >
CRBMap0< K, V >::~CRBMap0() VThrow()
{
}

//error C2556: 'CRBTree0<K,V>::CNode *CRBMap0<K,V>::Lookup(ATL::CStringT<BaseType,StringTraits>) VThrow()' : 오버로드된 함수가 'const CRBTree0<K,V>::CNode *CRBMap0<K,V>::Lookup(ATL::CStringT<BaseType,StringTraits>) VThrow()'과(와) 반환 형식만 다릅니다.
// const 놈을 제거 한다.
// template< typename K, typename V >
// const typename CRBMap0< K, V >::CNode* CRBMap0< K, V >::Lookup( K key ) VThrow()//const
// {
// 	//?usage
// 	//CKRbStr::CNode* pnd = m_attr.Lookup(key);
// 
// 	CRBMap0< K, V >::CNode* pPair = Find(key);
// 	return pPair;
// }

template< typename K, typename V >
typename CRBTree0< K, V >::CNode* CRBMap0< K, V >::Lookup(K key) VThrow()
{
	//?usage
	//CRBTree0::CNode* pnd = m_attr.Lookup(key);
	//CRBTree0< K, V >::CNode*
	auto pPair = Find(key);
	return pPair;
}

template< typename K, typename V >
const typename V& CRBMap0< K, V >::GetVaule(K key) VThrow()
{
	// Lookup(key) == NULL 인 경우 throw 0 을 처리 한다.
	return Lookup(key)->m_value;
}


template< typename K, typename V >
bool CRBMap0< K, V >::Lookup(K key, V& value) VThrow() //const 
{
	//const CRBTree0< K, V >::CPair* 
	auto pLookup = Find(key);
	if(pLookup == NULL)
		return false;

	value = pLookup->m_value;
	return true;
}

template< typename K, typename V >
POSITION CRBMap0< K, V >::SetAt(K key, V value) VThrow()
{
	//CRBTree0< K, V >::CPair* 
	auto pNode = Find(key);
	if(pNode == NULL)
	{
		return(RBInsert(key, value));
	}
	else
	{
		pNode->m_value = value;

		return(pNode);
	}
}

template< typename K, typename V >
bool CRBMap0< K, V >::RemoveKey(K key) VThrow()
{
	const POSITION pos = (const POSITION)Lookup(key);
	if(pos != NULL)
	{
		RemoveAt(pos);

		return(true);
	}
	else
	{
		return(false);
	}
}

template< typename K, typename V/*, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V >*/ >
class CRBMultiMap0 :
	public CRBTree0< K, V >
{
public:
	typedef typename K K;//c++17 VS2019 부터 class내 typedef이 장속이 안된다. 그래서 자식들이 다시 해준다.
	typedef typename V V;

	explicit CRBMultiMap0(size_t nBlockSize = 10) VThrow();
	~CRBMultiMap0() VThrow();

	virtual POSITION Insert(K key, V value) VThrow();
	size_t RemoveKey(K key) VThrow();
	//friend class CPair;
	POSITION FindFirstWithKey(K key) const VThrow();
	const CRBTree0< K, V >::CPair* GetNextWithKey(POSITION& pos, K key) const VThrow();
	CRBTree0< K, V >::CPair* GetNextWithKey(POSITION& pos, K key) VThrow();
	const V& GetNextValueWithKey(POSITION& pos, K key) const VThrow();
	V& GetNextValueWithKey(POSITION& pos, K key) VThrow();

#ifdef _Usage
	/// ///// 키 와 값으로 검색 하려면 루프를 직접 돌려서 '값'이 같은가는 수동으로 체크 해야 한다.
	// 범위 일떄: 시작값과 일치 하지 않아도 그 사이 값을 모두 찾아
	for_rbMulRangeHeadInclude(b, rb)
	{
		TKey& key = rb.GetKeyAt(_pos);
		TVal& val = rb.GetValueAt(_pos);
	}

	// 특정 값 일때 같아야 한다.
	CString slr = phs02->S(f0200Serial); //f2500Serial
	cxaf* phs25 = NULL;
	for_rbMulRangeHeadInclude(slr, mapMyNodes)
	{
		auto* phs25x = rb.GetValueAt(_pos);
		if(phs25->IsSameS(f25BrKey, brKey))
		{
			phs25 = phs25x;
			break;
		}
	}
	if(!phs25)
	{
		// not found then, do something
	}
#endif // _Usage


};

template< typename K, typename V >
CRBMultiMap0< K, V >::CRBMultiMap0(size_t nBlockSize) VThrow() :
	CRBTree0< K, V >(nBlockSize)
{
}

template< typename K, typename V >
CRBMultiMap0< K, V >::~CRBMultiMap0() VThrow()
{
}

template< typename K, typename V >
POSITION CRBMultiMap0< K, V >::Insert(K key, V value) VThrow()
{
	return(RBInsert(key, value));
}

template< typename K, typename V >
size_t CRBMultiMap0< K, V >::RemoveKey(K key) VThrow()
{
	size_t nElementsDeleted = 0;

	POSITION pos = FindFirstWithKey(key);
	while(pos != NULL)
	{
		POSITION posDelete = pos;
		GetNextWithKey(pos, key);
		RemoveAt(posDelete);
		nElementsDeleted++;
	}

	return(nElementsDeleted);
}

template< typename K, typename V >
POSITION CRBMultiMap0< K, V >::FindFirstWithKey(K key) const VThrow()
{
	return(Find(key));
}

template< typename K, typename V >
const typename CRBTree0< K, V >::CPair* CRBMultiMap0< K, V >::GetNextWithKey(POSITION& pos, K key) const VThrow()
{
	KWASSERT0(pos != NULL);
	//const CPair* 
	auto pNode = this->GetNext(pos);
	//if((pos == NULL) || !_CompareB(&(static_cast<CPair*>(pos)->m_key), &key))//KTraits::CompareElements
	if((pos == NULL) || !_CompareB(&(static_cast<typename CRBTree0< K, V >::CPair*>(pos)->m_key), &key))//KTraits::CompareElements
	{
		pos = NULL;
	}

	return(pNode);
}

template< typename K, typename V >
typename CRBTree0< K, V >::CPair* CRBMultiMap0< K, V >::GetNextWithKey(POSITION& pos, K key) VThrow()
{
	KWASSERT0(pos != NULL);
	//CPair* 
		auto pNode = this->GetNext(pos);
	if((pos == NULL) || !_CompareB(&(static_cast<typename CRBTree0< K, V >::CPair*>(pos)->m_key), &key))//KTraits::CompareElements
	{
		pos = NULL;
	}

	return(pNode);
}

template< typename K, typename V >
const V& CRBMultiMap0< K, V >::GetNextValueWithKey(POSITION& pos, K key) const VThrow()
{
	//const CPair*
	auto pPair = GetNextWithKey(pos, key);

	return(pPair->m_value);
}

template< typename K, typename V >
V& CRBMultiMap0< K, V >::GetNextValueWithKey(POSITION& pos, K key) VThrow()
{
	//CPair* 
	auto pPair = GetNextWithKey(pos, key);

	return(pPair->m_value);
}

//#pragma pop_macro("new")
