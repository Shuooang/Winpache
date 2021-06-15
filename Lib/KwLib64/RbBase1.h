#pragma once

#include <atldef.h>
#include <atlbase.h>
#include <new.h>

#pragma warning(push)
#pragma warning(disable: 4702)  // Unreachable code.  This file will have lots of it, especially without EH enabled.
#pragma warning(disable: 4512)  // assignment operator could not be generated
#pragma warning(disable: 4290)  // C++ Exception Specification ignored
#pragma warning(disable: 4127)  // conditional expression constant
#pragma warning(disable: 4571)  //catch(...) blocks compiled with /EHs do NOT catch or re-throw Structured Exceptions

// abstract iteration position
#ifndef _AFX
struct __POSITION
{
};
#endif
typedef __POSITION* POSITION;
#pragma push_macro("new")
#undef new

//
// The red-black tree code is based on the descriptions in
// "Introduction to Algorithms", by Cormen, Leiserson, and Rivest
//
template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class CRBTree1
{
public:
	typedef typename KTraits::INARGTYPE KINARGTYPE;
	typedef typename KTraits::OUTARGTYPE KOUTARGTYPE;
	typedef typename VTraits::INARGTYPE VINARGTYPE;
	typedef typename VTraits::OUTARGTYPE VOUTARGTYPE;

public:
	class CPair :
		public __POSITION
	{
	protected:

		CPair(
			/* _In_ */ KINARGTYPE key,
			/* _In_ */ VINARGTYPE value) :
			m_key(key),
			m_value(value)
		{
		}
		~CPair() throw()
		{
		}

	public:
		const K m_key;
		V m_value;
	};

private:

	class CNode :
		public CPair
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

		CNode(
			/* _In_ */ KINARGTYPE key,
			/* _In_ */ VINARGTYPE value) :
			CPair(key, value),
			m_pParent(NULL),
			m_eColor(RB_BLACK)
		{
		}
		~CNode() throw()
		{
		}
	};

private:
	CNode* m_pRoot;
	size_t m_nCount;
	CNode* m_pFree;
	CAtlPlex* m_pBlocks;
	size_t m_nBlockSize;

	// sentinel node
	CNode* m_pNil;

	// methods
	bool IsNil(_In_ CNode* p) const throw();
	void SetNil(_Outptr_ CNode** p) throw();

	CNode* NewNode(
		/* _In_ */ KINARGTYPE key,
		/* _In_ */ VINARGTYPE value);
	void FreeNode(_Inout_ CNode* pNode) throw();
	void RemovePostOrder(_In_ CNode* pNode) throw();
	CNode* LeftRotate(_In_ CNode* pNode) throw();
	CNode* RightRotate(_In_ CNode* pNode) throw();
	void SwapNode(
		_Out_ CNode* pDest,
		_Inout_ CNode* pSrc) throw();
	CNode* InsertImpl(
		/* _In_ */ KINARGTYPE key,
		/* _In_ */ VINARGTYPE value);
	void RBDeleteFixup(_In_ CNode* pNode) throw();
	bool RBDelete(_In_opt_ CNode* pZ) throw();

#ifdef _DEBUG

	// internal debugging code to verify red-black properties of tree:
	// 1) Every node is either red or black
	// 2) Every leaf (NIL) is black
	// 3) If a node is red, both its children are black
	// 4) Every simple path from a node to a descendant leaf node contains
	//    the same number of black nodes
private:
	void VerifyIntegrity(
		_In_ const CNode* pNode,
		_In_ int nCurrBlackDepth,
		_Out_ int& nBlackDepth) const throw();

public:
	void VerifyIntegrity() const throw();

#endif // _DEBUG

protected:
	CNode* Minimum(_In_opt_ CNode* pNode) const throw();
	CNode* Maximum(_In_opt_ CNode* pNode) const throw();
	CNode* Predecessor(_In_opt_ CNode* pNode) const throw();
	CNode* Successor(_In_opt_ CNode* pNode) const throw();
	CNode* RBInsert(
		/* _In_ */ KINARGTYPE key,
		/* _In_ */ VINARGTYPE value);
	CNode* Find(/* _In_ */ KINARGTYPE key) const throw();
	CNode* FindPrefix(/* _In_ */ KINARGTYPE key) const throw();

protected:
	explicit CRBTree1(_In_ size_t nBlockSize = 10) throw();  // protected to prevent instantiation

public:
	~CRBTree1() throw();

	void RemoveAll() throw();
	void RemoveAt(_In_ POSITION pos) throw();

	size_t GetCount() const throw();
	bool IsEmpty() const throw();

	POSITION FindFirstKeyAfter(/* _In_ */ KINARGTYPE key) const throw();

	POSITION GetHeadPosition() const throw();
	POSITION GetTailPosition() const throw();
	void GetNextAssoc(
		_Inout_ POSITION& pos,
		_Out_ KOUTARGTYPE key,
		_Out_ VOUTARGTYPE value) const;
	const CPair* GetNext(_Inout_ POSITION& pos) const throw();
	CPair* GetNext(_Inout_ POSITION& pos) throw();
	const CPair* GetPrev(_Inout_ POSITION& pos) const throw();
	CPair* GetPrev(_Inout_ POSITION& pos) throw();
	const K& GetNextKey(_Inout_ POSITION& pos) const throw();
	const V& GetNextValue(_Inout_ POSITION& pos) const throw();
	V& GetNextValue(_Inout_ POSITION& pos) throw();

	CPair* GetAt(_In_ POSITION pos) throw();
	const CPair* GetAt(_In_ POSITION pos) const throw();
	void GetAt(
		_In_ POSITION pos,
		_Out_ KOUTARGTYPE key,
		_Out_ VOUTARGTYPE value) const;
	const K& GetKeyAt(_In_ POSITION pos) const;
	const V& GetValueAt(_In_ POSITION pos) const;
	V& GetValueAt(_In_ POSITION pos);
	void SetValueAt(
		_In_ POSITION pos,
		/* _In_ */ VINARGTYPE value);

private:
	// Private to prevent use
	CRBTree1(_In_ const CRBTree1&) throw();
	CRBTree1& operator=(_In_ const CRBTree1&) throw();
};

template< typename K, typename V, class KTraits, class VTraits >
inline bool CRBTree1< K, V, KTraits, VTraits >::IsNil(_In_ CNode* p) const throw()
{
	return (p == m_pNil);
}

template< typename K, typename V, class KTraits, class VTraits >
inline void CRBTree1< K, V, KTraits, VTraits >::SetNil(_Outptr_ CNode** p) throw()
{
	ATLENSURE(p != NULL);
	*p = m_pNil;
}

template< typename K, typename V, class KTraits, class VTraits >
CRBTree1< K, V, KTraits, VTraits >::CRBTree1(_In_ size_t nBlockSize) throw() :
	m_pRoot(NULL),
	m_nCount(0),
	m_nBlockSize(nBlockSize),
	m_pFree(NULL),
	m_pBlocks(NULL),
	m_pNil(NULL)
{
	ATLASSERT(nBlockSize > 0);
}

template< typename K, typename V, class KTraits, class VTraits >
CRBTree1< K, V, KTraits, VTraits >::~CRBTree1() throw()
{
	RemoveAll();
	if(m_pNil != NULL)
	{
		free(m_pNil);
	}
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree1< K, V, KTraits, VTraits >::RemoveAll() throw()
{
	if(!IsNil(m_pRoot))
		RemovePostOrder(m_pRoot);
	m_nCount = 0;
	m_pBlocks->FreeDataChain();
	m_pBlocks = NULL;
	m_pFree = NULL;
	m_pRoot = m_pNil;
}

template< typename K, typename V, class KTraits, class VTraits >
size_t CRBTree1< K, V, KTraits, VTraits >::GetCount() const throw()
{
	return m_nCount;
}

template< typename K, typename V, class KTraits, class VTraits >
bool CRBTree1< K, V, KTraits, VTraits >::IsEmpty() const throw()
{
	return(m_nCount == 0);
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CRBTree1< K, V, KTraits, VTraits >::FindFirstKeyAfter(/* _In_ */ KINARGTYPE key) const throw()
{
	return(FindPrefix(key));
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree1< K, V, KTraits, VTraits >::RemoveAt(_In_ POSITION pos) throw()
{
	ATLASSERT(pos != NULL);
	RBDelete(static_cast<CNode*>(pos));
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CRBTree1< K, V, KTraits, VTraits >::GetHeadPosition() const throw()
{
	return(Minimum(m_pRoot));
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CRBTree1< K, V, KTraits, VTraits >::GetTailPosition() const throw()
{
	return(Maximum(m_pRoot));
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree1< K, V, KTraits, VTraits >::GetNextAssoc(
	_Inout_ POSITION& pos,
	_Out_ KOUTARGTYPE key,
	_Out_ VOUTARGTYPE value) const
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);

	key = pNode->m_key;
	value = pNode->m_value;

	pos = Successor(pNode);
}

template< typename K, typename V, class KTraits, class VTraits >
const typename CRBTree1< K, V, KTraits, VTraits >::CPair* CRBTree1< K, V, KTraits, VTraits >::GetNext(
	_Inout_ POSITION& pos) const throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);
	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CPair* CRBTree1< K, V, KTraits, VTraits >::GetNext(
	_Inout_ POSITION& pos) throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);
	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
const typename CRBTree1< K, V, KTraits, VTraits >::CPair* CRBTree1< K, V, KTraits, VTraits >::GetPrev(
	_Inout_ POSITION& pos) const throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Predecessor(pNode);

	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CPair* CRBTree1< K, V, KTraits, VTraits >::GetPrev(
	_Inout_ POSITION& pos) throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Predecessor(pNode);

	return pNode;
}

template< typename K, typename V, class KTraits, class VTraits >
const K& CRBTree1< K, V, KTraits, VTraits >::GetNextKey(
	_Inout_ POSITION& pos) const throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);

	return pNode->m_key;
}

template< typename K, typename V, class KTraits, class VTraits >
const V& CRBTree1< K, V, KTraits, VTraits >::GetNextValue(
	_Inout_ POSITION& pos) const throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);

	return pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
V& CRBTree1< K, V, KTraits, VTraits >::GetNextValue(
	_Inout_ POSITION& pos) throw()
{
	ATLASSERT(pos != NULL);
	CNode* pNode = static_cast<CNode*>(pos);
	pos = Successor(pNode);

	return pNode->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CPair* CRBTree1< K, V, KTraits, VTraits >::GetAt(
	_In_ POSITION pos) throw()
{
	ATLASSERT(pos != NULL);

	return(static_cast<CPair*>(pos));
}

template< typename K, typename V, class KTraits, class VTraits >
const typename CRBTree1< K, V, KTraits, VTraits >::CPair* CRBTree1< K, V, KTraits, VTraits >::GetAt(
	_In_ POSITION pos) const throw()
{
	ATLASSERT(pos != NULL);

	return(static_cast<const CPair*>(pos));
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree1< K, V, KTraits, VTraits >::GetAt(
	_In_ POSITION pos,
	_Out_ KOUTARGTYPE key,
	_Out_ VOUTARGTYPE value) const
{
	ATLENSURE(pos != NULL);
	key = static_cast<CNode*>(pos)->m_key;
	value = static_cast<CNode*>(pos)->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
const K& CRBTree1< K, V, KTraits, VTraits >::GetKeyAt(_In_ POSITION pos) const
{
	ATLENSURE(pos != NULL);
	return static_cast<CNode*>(pos)->m_key;
}

template< typename K, typename V, class KTraits, class VTraits >
const V& CRBTree1< K, V, KTraits, VTraits >::GetValueAt(_In_ POSITION pos) const
{
	ATLENSURE(pos != NULL);
	return static_cast<CNode*>(pos)->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
V& CRBTree1< K, V, KTraits, VTraits >::GetValueAt(_In_ POSITION pos)
{
	ATLENSURE(pos != NULL);
	return static_cast<CNode*>(pos)->m_value;
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree1< K, V, KTraits, VTraits >::SetValueAt(
	_In_ POSITION pos,
	/* _In_ */ VINARGTYPE value)
{
	ATLENSURE(pos != NULL);
	static_cast<CNode*>(pos)->m_value = value;
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CNode* CRBTree1< K, V, KTraits, VTraits >::NewNode(
	/* _In_ */ KINARGTYPE key,
	/* _In_ */ VINARGTYPE value)
{
	if(m_pFree == NULL)
	{
		if(m_pNil == NULL)
		{
			m_pNil = reinterpret_cast<CNode*>(malloc(sizeof(CNode)));
			if(m_pNil == NULL)
			{
				AtlThrow(E_OUTOFMEMORY);
			}
			memset(m_pNil, 0x00, sizeof(CNode));
			m_pNil->m_eColor = CNode::RB_BLACK;
			m_pNil->m_pParent = m_pNil->m_pLeft = m_pNil->m_pRight = m_pNil;
			m_pRoot = m_pNil;
		}

		CAtlPlex* pPlex = CAtlPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CNode));
		if(pPlex == NULL)
		{
			AtlThrow(E_OUTOFMEMORY);
		}
		CNode* pNode = static_cast<CNode*>(pPlex->data());
		pNode += m_nBlockSize - 1;
		for(INT_PTR iBlock = m_nBlockSize - 1; iBlock >= 0; iBlock--)
		{
			pNode->m_pLeft = m_pFree;
			m_pFree = pNode;
			pNode--;
		}
	}
	ATLASSUME(m_pFree != NULL);

	CNode* pNewNode = m_pFree;
	::new(pNewNode) CNode(key, value);

	m_pFree = m_pFree->m_pLeft;
	pNewNode->m_eColor = CNode::RB_RED;
	SetNil(&pNewNode->m_pLeft);
	SetNil(&pNewNode->m_pRight);
	SetNil(&pNewNode->m_pParent);

	m_nCount++;
	ATLASSUME(m_nCount > 0);

	return(pNewNode);
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree1< K, V, KTraits, VTraits >::FreeNode(_Inout_ CNode* pNode) throw()
{
	ATLENSURE(pNode != NULL);
	pNode->~CNode();
	pNode->m_pLeft = m_pFree;
	m_pFree = pNode;
	ATLASSUME(m_nCount > 0);
	m_nCount--;
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree1< K, V, KTraits, VTraits >::RemovePostOrder(_In_ CNode* pNode) throw()
{
	if(IsNil(pNode))
		return;
	RemovePostOrder(pNode->m_pLeft);
	RemovePostOrder(pNode->m_pRight);
	FreeNode(pNode);
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CNode* CRBTree1< K, V, KTraits, VTraits >::LeftRotate(
	_In_ CNode* pNode) throw()
{
	ATLASSERT(pNode != NULL);
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

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CNode* CRBTree1< K, V, KTraits, VTraits >::RightRotate(
	_In_ CNode* pNode) throw()
{
	ATLASSERT(pNode != NULL);
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

#pragma warning(push)
#pragma warning(disable:28182 28183)

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CNode* CRBTree1< K, V, KTraits, VTraits >::Find(
	/* _In_ */ KINARGTYPE key) const throw()
{
	CNode* pKey = NULL;
	CNode* pNode = m_pRoot;
	while(!IsNil(pNode) && (pKey == NULL))
	{
		int nCompare = KTraits::CompareElementsOrdered(key, pNode->m_key);
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

	while(true)
	{
		CNode* pPrev = Predecessor(pKey);
		if((pPrev != NULL) && KTraits::CompareElements(key, pPrev->m_key))
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

#pragma warning(pop)

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CNode* CRBTree1< K, V, KTraits, VTraits >::FindPrefix(
	/* _In_ */ KINARGTYPE key) const throw()
{
	// First, attempt to find a node that matches the key exactly
	CNode* pParent = NULL;
	CNode* pKey = NULL;
	CNode* pNode = m_pRoot;
	while(!IsNil(pNode) && (pKey == NULL))
	{
		pParent = pNode;
		int nCompare = KTraits::CompareElementsOrdered(key, pNode->m_key);
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
			if((pNext != NULL) && KTraits::CompareElements(key, pNext->m_key))
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
		int nCompare = KTraits::CompareElementsOrdered(key, pParent->m_key);
		if(nCompare < 0)
		{
			pKey = pParent;
		}
		else
		{
			ATLASSERT(nCompare > 0);
			pKey = Successor(pParent);
		}
	}

	return(pKey);
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree1< K, V, KTraits, VTraits >::SwapNode(_Out_ CNode* pDest, _Inout_ CNode* pSrc) throw()
{
	ATLENSURE(pDest != NULL);
	ATLENSURE(pSrc != NULL);

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

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CNode* CRBTree1< K, V, KTraits, VTraits >::InsertImpl(
	/* _In_ */ KINARGTYPE key,
	/* _In_ */ VINARGTYPE value)
{
	CNode* pNew = NewNode(key, value);

	CNode* pY = NULL;
	CNode* pX = m_pRoot;

	while(!IsNil(pX))
	{
		pY = pX;
		if(KTraits::CompareElementsOrdered(key, pX->m_key) <= 0)
			pX = pX->m_pLeft;
		else
			pX = pX->m_pRight;
	}

	pNew->m_pParent = pY;
	if(pY == NULL)
	{
		m_pRoot = pNew;
	}
	else if(KTraits::CompareElementsOrdered(key, pY->m_key) <= 0)
		pY->m_pLeft = pNew;
	else
		pY->m_pRight = pNew;

	return pNew;
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree1< K, V, KTraits, VTraits >::RBDeleteFixup(_In_ CNode* pNode) throw()
{
	ATLENSURE(pNode != NULL);

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


template< typename K, typename V, class KTraits, class VTraits >
bool CRBTree1< K, V, KTraits, VTraits >::RBDelete(_In_opt_ CNode* pZ) throw()
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

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CNode* CRBTree1< K, V, KTraits, VTraits >::Minimum(
	_In_opt_ CNode* pNode) const throw()
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

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CNode* CRBTree1< K, V, KTraits, VTraits >::Maximum(
	_In_opt_ CNode* pNode) const throw()
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

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CNode* CRBTree1< K, V, KTraits, VTraits >::Predecessor(
	_In_opt_ CNode* pNode) const throw()
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

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CNode* CRBTree1< K, V, KTraits, VTraits >::Successor(
	_In_opt_ CNode* pNode) const throw()
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

template< typename K, typename V, class KTraits, class VTraits >
typename CRBTree1< K, V, KTraits, VTraits >::CNode* CRBTree1< K, V, KTraits, VTraits >::RBInsert(
	/* _In_ */ KINARGTYPE key,
	/* _In_ */ VINARGTYPE value)
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

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree1< K, V, KTraits, VTraits >::VerifyIntegrity(
	_In_ const CNode* pNode,
	_In_ int nCurrBlackDepth,
	_Out_ int& nBlackDepth) const throw()
{
	bool bCheckForBlack = false;
	bool bLeaf = true;

	if(pNode->m_eColor == CNode::RB_RED)
		bCheckForBlack = true;
	else
		nCurrBlackDepth++;

	ATLASSERT(pNode->m_pLeft != NULL);
	if(!IsNil(pNode->m_pLeft))
	{
		bLeaf = false;
		if(bCheckForBlack)
		{
			ATLASSERT(pNode->m_pLeft->m_eColor == CNode::RB_BLACK);
		}

		VerifyIntegrity(pNode->m_pLeft, nCurrBlackDepth, nBlackDepth);
	}

	ATLASSERT(pNode->m_pRight != NULL);
	if(!IsNil(pNode->m_pRight))
	{
		bLeaf = false;
		if(bCheckForBlack)
		{
			ATLASSERT(pNode->m_pRight->m_eColor == CNode::RB_BLACK);
		}

		VerifyIntegrity(pNode->m_pRight, nCurrBlackDepth, nBlackDepth);
	}

	ATLASSERT(pNode->m_pParent != NULL);
	ATLASSERT((IsNil(pNode->m_pParent)) ||
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
			ATLASSERT(nBlackDepth == nCurrBlackDepth);
		}
	}
}

template< typename K, typename V, class KTraits, class VTraits >
void CRBTree1< K, V, KTraits, VTraits >::VerifyIntegrity() const throw()
{
	if((m_pRoot == NULL) || (IsNil(m_pRoot)))
		return;

	ATLASSUME(m_pRoot->m_eColor == CNode::RB_BLACK);
	int nBlackDepth = 0;
	VerifyIntegrity(m_pRoot, 0, nBlackDepth);
}

#endif // _DEBUG

template< typename K, typename V, class KTraits = CElementTraits< K >, class VTraits = CElementTraits< V > >
class CRBMultiMap1 :
	public CRBTree< K, V, KTraits, VTraits >
{
public:
	using typename CRBTree<K, V, KTraits, VTraits>::KINARGTYPE;
	using typename CRBTree<K, V, KTraits, VTraits>::VINARGTYPE;
	typedef typename CRBTree<K, V, KTraits, VTraits>::CPair CPair;

	explicit CRBMultiMap1(_In_ size_t nBlockSize = 10) throw();
	~CRBMultiMap1() throw();

	POSITION Insert(
		/* _In_ */ KINARGTYPE key,
		/* _In_ */ VINARGTYPE value);
	size_t RemoveKey(/* _In_ */ KINARGTYPE key) throw();

	POSITION FindFirstWithKey(/* _In_ */ KINARGTYPE key) const throw();
	const CPair* GetNextWithKey(
		_Inout_ POSITION& pos,
		/* _In_ */ KINARGTYPE key) const throw();
	CPair* GetNextWithKey(
		_Inout_ POSITION& pos,
		/* _In_ */ KINARGTYPE key) throw();
	const V& GetNextValueWithKey(
		_Inout_ POSITION& pos,
		/* _In_ */ KINARGTYPE key) const throw();
	V& GetNextValueWithKey(
		_Inout_ POSITION& pos,
		/* _In_ */ KINARGTYPE key) throw();
};

template< typename K, typename V, class KTraits, class VTraits >
CRBMultiMap1< K, V, KTraits, VTraits >::CRBMultiMap1(_In_ size_t nBlockSize) throw() :
	CRBTree< K, V, KTraits, VTraits >(nBlockSize)
{
}

template< typename K, typename V, class KTraits, class VTraits >
CRBMultiMap1< K, V, KTraits, VTraits >::~CRBMultiMap1() throw()
{
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CRBMultiMap1< K, V, KTraits, VTraits >::Insert(
	/* _In_ */ KINARGTYPE key,
	/* _In_ */ VINARGTYPE value)
{
	return(this->RBInsert(key, value));
}

template< typename K, typename V, class KTraits, class VTraits >
size_t CRBMultiMap1< K, V, KTraits, VTraits >::RemoveKey(
	/* _In_ */ KINARGTYPE key) throw()
{
	size_t nElementsDeleted = 0;

	POSITION pos = FindFirstWithKey(key);
	while(pos != NULL)
	{
		POSITION posDelete = pos;
		GetNextWithKey(pos, key);
		this->RemoveAt(posDelete);
		nElementsDeleted++;
	}

	return(nElementsDeleted);
}

template< typename K, typename V, class KTraits, class VTraits >
POSITION CRBMultiMap1< K, V, KTraits, VTraits >::FindFirstWithKey(
	/* _In_ */ KINARGTYPE key) const throw()
{
	return(this->Find(key));
}

template< typename K, typename V, class KTraits, class VTraits >
const typename CRBMultiMap1< K, V, KTraits, VTraits >::CPair* CRBMultiMap1< K, V, KTraits, VTraits >::GetNextWithKey(
	_Inout_ POSITION& pos,
	/* _In_ */ KINARGTYPE key) const throw()
{
	ATLASSERT(pos != NULL);
	const CPair* pNode = this->GetNext(pos);
	if((pos == NULL) || !KTraits::CompareElements(static_cast<CPair*>(pos)->m_key, key))
	{
		pos = NULL;
	}

	return(pNode);
}

template< typename K, typename V, class KTraits, class VTraits >
typename CRBMultiMap1< K, V, KTraits, VTraits >::CPair* CRBMultiMap1< K, V, KTraits, VTraits >::GetNextWithKey(
	_Inout_ POSITION& pos,
	/* _In_ */ KINARGTYPE key) throw()
{
	ATLASSERT(pos != NULL);
	CPair* pNode = this->GetNext(pos);
	if((pos == NULL) || !KTraits::CompareElements(static_cast<CPair*>(pos)->m_key, key))
	{
		pos = NULL;
	}

	return(pNode);
}

template< typename K, typename V, class KTraits, class VTraits >
const V& CRBMultiMap1< K, V, KTraits, VTraits >::GetNextValueWithKey(
	_Inout_ POSITION& pos,
	/* _In_ */ KINARGTYPE key) const throw()
{
	const CPair* pPair = GetNextWithKey(pos, key);

	return(pPair->m_value);
}

template< typename K, typename V, class KTraits, class VTraits >
V& CRBMultiMap1< K, V, KTraits, VTraits >::GetNextValueWithKey(
	_Inout_ POSITION& pos,
	/* _In_ */ KINARGTYPE key) throw()
{
	CPair* pPair = GetNextWithKey(pos, key);

	return(pPair->m_value);
}

#pragma pop_macro("new")

//}; // namespace ATL
//#pragma pack(pop)

#pragma warning(pop)