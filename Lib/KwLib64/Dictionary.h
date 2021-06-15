#pragma once

#include <atlcoll.h>
#include <functional>
#include <memory>

#include "ktypedef.h"
//#include "RbBase.h"
#include "kinterface.h"
#include "inlinefnc.h"


using std::shared_ptr;
using std::function;

#define VThrow()


#define for_count(n)   for(int _i=0;_i<((n).GetCount());_i++)
#define for_countP(n)  for(int _i=0;_i<((n)->GetCount());_i++)


#define for_eachRevs(n)  for(int _i=((n)-1);_i>=0;_i--)

#define for_list0(lst) for(POSITION _pos = (lst).GetHeadPosition();_pos;)
#define next_list0(lst) (lst).GetNext(_pos)
//      _pos�� next�� �Ѿ� ���� ��� �ϸ� �ȵȴ�.
#define for_rlist0(lst) for(POSITION _pos = (lst).GetTailPosition();_pos;)
#define prev_list0(lst) (lst).GetPrev(_pos)
//      _pos�� next�� �Ѿ� ���� ��� �ϸ� �ȵȴ�.

#define for_map0(lst) for(POSITION _pos = (lst).GetStartPosition();_pos;)
#define next_map0(lst, key,val) (lst).GetNextAssoc(_pos, key, val)

// for loop �� ���� �����ϰ� ���� ����
#define for_eachX(n)  for(int _x=0;_x<(n);_x++)
#define for_eachY(n)  for(int _y=0;_y<(n);_y++)
#define for_each0(n)  for(int _i=0;_i<(n);_i++)
#define for_eachUsr(_i,n)  for(int _i=0;_i<(n);_i++)
#define for_each_1(n)  for(int _i1=0;_i1<(n);_i1++)
#define for_each_2(n)  for(int _i2=0;_i2<(n);_i2++)
#define for_each_3(n)  for(int _i3=0;_i3<(n);_i3++)
#define for_each_2D(n,m)  for(int _r=0;_r<(n);_r++)\
	for(int _c=0;_c<(m);_c++)

#define for_array0(ar) for_each0(_countof(ar))
#define for_array1(ar1) for_each_1(_countof(ar1))

#define FindInArray(ar, key, iFound) 	for_array0(ar)\
{	if(key == ar[_i])\
{	iFound = _i; break;\
}	}

// cmp �� ������ true�� ���� �ϴ� �Լ� == 0 �ƴ�.. ����
#define FindInArrayT(ar, key, iFound, cmp) 	for_array0(ar)\
{	if(cmp(key, ar[_i]))\
{	iFound = _i; break;\
}	}


/// <summary>
/// CString ���ڿ� ��
/// </summary>
/// <typeparam name="TStr">CStringA, CStringW</typeparam>
/// <param name="s1"></param>
/// <param name="s2"></param>
/// <param name="iOp"></param>
/// <returns></returns>
template<typename TStr>
int CompareInsensitive(TStr& s1, TStr& s2)//, ECmpOp iOp = eCmprCase)
{

	TStr si1 = s1.MakeUpper();
	TStr si2 = s2.MakeUpper();
	if(si1 > si2)
		return 1;
	else if(si1 < si2)
		return -1;
	else
		return 0;
}


/*
template<typename TKey>
class CCompareTool
{
public:
	typedef const TKey TCKey;// function ���� ���� ���̰�
	shared_ptr<function<int(TCKey*, TCKey*)>> _fncCompare;
	template<typename TFNC> void AddCompare(TFNC fnc)
	{
		_fncCompare = shared_ptr<function<int(TCKey*, TCKey*)>>(
#ifdef _DEBUG
		DEBUG_NEW
#else
		new
#endif // _DEBUG
function<int(TCKey*, TCKey*)>(fnc));
		// somedic.AddCompare([&](TCKey*, TCKey*)->int { return *p1 - *p2; } );
	}
	void SetInsensitive()
	{
		auto fncIC = 
#ifdef _DEBUG
			DEBUG_NEW
#else
			new
#endif // _DEBUG
			function<int(TCKey*, TCKey*)>([&](TCKey* ps1, TCKey* ps2)->int {
			return CompareInsensitive((*ps1), (*ps2));
			});
		_fncCompare = shared_ptr<function<int(TCKey*, TCKey*)>>(fncIC);
		// somedic.AddCompare([&](TCKey*, TCKey*)->int { return *p1 - *p2; } );
	}
};
*/


/// ////////////////////////////////////////////////////
/// CCompareTool �� ���� ��ü������ �Ѱ��̰�. ���� atlcoll.h �� KTraits�� ����
/// �Ʒ� 3��ü�� �ٲٰ� CRBMap �ڽ� ��ü ���鶧 K, V ���� ���� ���� KTraits �� �־� ��� �Ѵ�.


template< typename T >
class CDefaultCompareTraitsLambda
{
public:
	//static shared_ptr<function<int(const T*, const T*)>> s_fncCompare;//?kdw

	template<typename TFNC> void AddCompare(TFNC fnc)
	{
		//void TSetLambdaToSharedFunction(shared_ptr<TFNC>&rtval, TRAMBD lambda)
		//TSetLambdaToSharedFunction(s_fncCompare, fnc);
		// ������ �Ẹ��. _fncCompare = shared_ptr<function<int(TCKey*, TCKey*)>>(new function<int(TCKey*, TCKey*)>(fnc));
		//?ex: somedic.AddCompare([&](TCKey*, TCKey*)->int { return *p1 - *p2; } );
	}
	// insensitive ��� �Ϸ��� CDefaultCompareTraits<T>::SetInsensitive(); �ؾ� �Ѵ�.
	static void SetInsensitive()
	{
		AddCompare([&](const T* ps1, const T* ps2)->int
			{
				return CompareInsensitive((*ps1), (*ps2));
			});
		//auto fncIC = new function<int(TCKey*, TCKey*)>([&](T* ps1, T* ps2)->int
		//	{
		//		return CompareInsensitive((*ps1), (*ps2));
		//	});
		//_fncCompare = shared_ptr<function<int(T*, T*)>>(fncIC);
		//?ex: somedic.AddCompare([&](TCKey*, TCKey*)->int { return *p1 - *p2; } );
	}

	/// linearserch �� ���� �񱳸� ���� ����.
	static bool CompareElements(_In_ const T& element1, _In_ const T& element2)
	{
		//return((element1 == element2) != 0);  // != 0 to handle overloads of operator== that return BOOL instead of bool
		return CompareElementsOrdered(element1, element2) == 0;
	}

	/// quickserch �� ū�� ������ �˱� ���� ����.
	static int CompareElementsOrdered(_In_ const T& element1, _In_ const T& element2)
	{
		/*if(s_fncCompare.get() != nullptr)
		{
			return (*s_fncCompare)(&element1, &element2);//shared_ptr�� operator*() == *get() �ǹǷ� ����.
			//return (*_fncCompare.get())(&element1, &element2);//_Ty2& operator*() const noexcept { return *get();}
		}*/
		// ���ٰ� ���� ��쿡 ����.
		if(element1 < element2)
			return(-1);
		else if(element1 == element2)
			return(0);
		else
			return(1);
	}
};

template< typename T >
class CDefaultElementTraitsLambda :
	public CElementTraitsBase< T >,
	public CDefaultHashTraits< T >,
	public CDefaultCompareTraitsLambda< T >
{
};

template< typename T >
class CElementTraitsLambda :
	public CDefaultElementTraitsLambda< T >
{
};

/*
�⺻������ �񱳿����� �ٸ��� Ŭ������ �޶�� �Ѵ�. 
�׶��׶� �񱳸� �ٸ��� �Ϸ���
�� 3�� ��ü�� ������ �ϰ�, �װ��� KTraits = [���⿡ class] �� �ξ�� �Ѵ�.
���������� ���� Find�� FindPrefix�� override�� �ȵǾ� ��¿�� ����.

template< typename K, typename V, class KTraits = CElementTraitsLambda< K >, class VTraits = CElementTraitsLambda< V > >
class CRBMapMy :
	public CRBTree< K, V, KTraits, VTraits >
 
*/




















/// <summary>
/// �̰� CKRbMap0 �� case insensitive��ɸ� �ʿ� �ؼ� stringŰ�� ���� ����Ŵ�.
/// </summary>
/// <typeparam name="TStr"></typeparam>
/// <typeparam name="TVal0"></typeparam>
template<typename TKey, typename TVal, class KTraits = CElementTraitsLambda< TKey >, class VTraits = CElementTraitsLambda< TKey >>
class CKRbVal
	: public CRBMap<TKey, TVal, KTraits, VTraits>
	, public IClonable
	//, public CCompareTool<TKey>
{
public:
	typedef CKRbVal<TKey, TVal> ThisType;

	CKRbVal()
	{
	}
	/*virtual int _Compare(const TKey* ps1, const TKey* ps2, size_t len = -1, ECmpOp iOp = eCmprCase) const VThrow()
	{
		if(this->_fncCompare.get())
		{// value�� IClonable�� �ƴ� ��� clone�Լ��� ���ٷ� �����Ѵ�.
			return (*this->_fncCompare)(ps1, ps2);//pb->_CreateMe(iOp, __FUNCTION__);
		}
		else {//CString�� < > == �����ڰ� �ǹǷ� CRBTree0�� �ִ°� �׳� �ҷ��� �ȴ�.
			return __super::_Compare(ps1, ps2, len, iOp);
		}
	}*/
	typedef typename CRBTree<TKey, TVal, KTraits, VTraits>::CPair CPair;
	//typedef typename CRBTree<TKey, TVal, KTraits, VTraits>::CNode CNode;

	virtual void _Clone(IClonable* pSrc0, int iOp = eCloneDefault)
	{
		
		//auto ThisD = dynamic_cast<ThisType*>(pSrc0);// �̰� �³� ����.
		auto ThisS = static_cast<ThisType*>(pSrc0);// �̰� �³� ����.
		if((iOp & eCloneAppend) == 0)
			this->RemoveAll();
		TKey sKey;
		TVal sVal;
		for(POSITION pos = ThisS->GetHeadPosition();pos;)
		{
			ThisS->GetNextAssoc(pos, sKey, sVal);
			ThisS->SetAt(sKey, sVal);// pointer copy
		}
	}

	//               CKRbVal< TKey, TVal, KTraits, VTraits >::
	virtual CPair* Find(/* _In_ */ TKey key) const throw()
	{
		return nullptr;
	}

	/*
	template<typename FNC>
	void for_loop(FNC lambda)
	{
		/// C++�ɼǴٺ��⿡�� Conformance mode : No �ؾ� �� Yes(/ permissive - ) �� �Ǿ� ������ ������
		for(POSITION pos = GetHeadPosition(); pos; GetNext(pos))
		{
			TKey k;
			TVal v;
			GetAt(pos, k, v);
			lambda(k, v);
		}
	}
	*/
};



template<typename TObj>
class CPtrClonable
{
public:
	// value�� TPtr�� base type�̰� ����ü�� �� derived�ΰ�� �׳� new TPtr �ϸ� �ȵǰ� �� ������ ���� �ٸ� type�ϼ� �ִ�.
	// ���ٷ� Value creator�� �־� �ش�.
	shared_ptr<function<IClonable* (IClonable*)>> _fncValCreator;
	template<typename TFNC> void AddValCreator(TFNC fnc)
	{
		_fncValCreator = shared_ptr<function<IClonable* (IClonable*)>>(
#ifdef _DEBUG
		DEBUG_NEW
#else
		new
#endif // _DEBUG
function<IClonable* (IClonable*)>(fnc));
		// somedic.AddValCreator([&](IClonable* pSrc) { ... some code here; } );
	}
	shared_ptr<function<TObj* (TObj*)>> _fncClone;
	template<typename TFNC> void AddClonor(TFNC fnc)
	{
		_fncClone = shared_ptr<function<TObj* (TObj*)>>(
#ifdef _DEBUG
		DEBUG_NEW
#else
		new
#endif // _DEBUG
function<TObj* (TObj*)>(fnc));
		// somedic.AddClonor([&](TObj* pSrc) { ... some code here; } );
	}

	TObj* ClonePObj(TObj* pOld)
	{
		TObj* pNew = NULL;
		if(pOld)
		{

			if(_fncClone.get())
			{// value�� IClonable�� �ƴ� ��� clone�Լ��� ���ٷ� �����Ѵ�.
				pNew = (*_fncClone)(pOld);//pb->_CreateMe(iOp, __FUNCTION__);
				// mydic.AddClonor([&](TObj* pSrc) { 
				//    TObj* pn = new TObjDerived();
				//    pn->_sss = pSrc->_sss;
				//    ... some code here; 
				//	  return pn; } );
			}
			else {
				IClonable* pOldI = dynamic_cast<IClonable*>(pOld);
				if(pOldI)
				{
					if(_fncValCreator.get())
						pNew = (*_fncValCreator)(pOld);//pb->_CreateMe(iOp, __FUNCTION__);
					else
						pNew = 
#ifdef _DEBUG
						DEBUG_NEW
#else
						new
#endif // _DEBUG
						TObj();
					IClonable* pNewI = dynamic_cast<IClonable*>(pNew);
					int iOp = 0;
					pNewI->_Clone(pOldI, iOp);
				}
				else {//IClonable�� �ƴϸ� �׳� ����: ��� val�� �ƹ� class�� �ȴٴ� �Ŵ�.
					pNew = 
#ifdef _DEBUG
						DEBUG_NEW
#else
						new
#endif // _DEBUG
						TObj(pOld); // COlb(const COlb& src) {} �� �ִ� ���� �ϴ� ���� �Ѵ�.
				}
			}
		}// else src�� NULL�̸� �׳� �װ� ����.
		return pNew;
	}
};


/// <summary>
/// TObj�� IClonable �� �ƴϸ� AddClonor ����� deep clone�� ����� �ȴ�.
/// SetInsensitive�Ҽ� �ִ�.
/// </summary>
/// <typeparam name="TStr"></typeparam>
/// <typeparam name="TObj">IClonable�ƴϾ �ȴ�.</typeparam>
template<typename TKey, typename TObj, class KTraits = CElementTraitsLambda< TKey >>
class CKRbPtr
	: public CRBMap<TKey, TObj*, KTraits>
	, public IClonable
	, public CPtrClonable<TObj>
	//, public CCompareTool<TKey>
{
public:
	bool m_bAutoFree{ true };

	typedef const TKey TCKey;// function ���� ���� ���̰�
	typedef CKRbPtr<TKey, TObj> ThisType;

	/*virtual int _Compare(const TKey* ps1, const TKey* ps2, size_t len = -1, ECmpOp iOp = eCmprCase) const VThrow()
	{
		auto cmp = static_cast<CCompareTool<TKey>>(this);
		if(cmp->_fncCompare.get())
		{// value�� IClonable�� �ƴ� ��� clone�Լ��� ���ٷ� �����Ѵ�.
			return (*cmp->_fncCompare)(ps1, ps2);//pb->_CreateMe(iOp, __FUNCTION__);
		}
		else {//CString�� < > == �����ڰ� �ǹǷ� CRBTree0�� �ִ°� �׳� �ҷ��� �ȴ�.
			return __super::_Compare(ps1, ps2, len, iOp);
		}
	}*/

	CKRbPtr()
		//: CRBMap0<TKey, TObj*>(iOp)
	{
	}
	~CKRbPtr()
	{
		DeleteAll();
	}
	virtual BOOL DeleteKey(TKey k)
	{
		POS it = FindNode(k);
		if(it)
		{
			DeleteAtPbj(it, true);//_RemoveIndex ����
			return TRUE;
		}
		return FALSE;
	}

	virtual void DeleteAtPbj(POS it, bool bFree = true)
	{
		//SynchThis();
		TKey key = this->GetKeyAt(it);
		TObj* pbj = this->GetValueAt(it);
		if(bFree)
			DeleteVal(it);

		this->RemoveAt(it); // super �Լ��� this-> �� �ҷ��� �Ѵ�.
		//Dump0();
	}

	void DeleteVal(POS pos)
	{
		if(pos)
		{
			//CRBTree0<TKey, TObj*>::CPair* par = this->GetAt(pos);//, key, value
			auto par = this->GetAt(pos);//, key, value
			DeleteMeSafe(par->m_value);
		}
	}
	virtual void DeleteAll()
	{
		//auto rb = static_cast<CRBMap<TKey, TObj*>>(this);
		if(m_bAutoFree)
		{
			for(POS it = this->GetHeadPosition();it;)
			{
				DeleteVal(it);
				this->GetNext(it);
			}
		}
		this->RemoveAll();
	}
	BOOL DetachKey(TKey k, TObj** pptr = NULL, TObj* pValue = NULL)
	{
		POS it = FindNode(k);
		if(it)
		{
			if(pptr)
				*pptr = DetachAt(it);//_RemoveIndex ����
			else //���� free����. �̰� �θ����� ptr �޾� �ξ�����. �׸��� �˾Ƽ� free�ϰ���.
				DetachAt(it);//_RemoveIndex ����
			return TRUE;
		}
		return FALSE;
	}
	virtual POSITION SetAt(TKey key, TObj* value) VThrow()
	{
		//auto rb = static_cast<CRBMap<TKey, TObj*>>(this);
		//CPair* pNode = Find(key); CPair�� ��ã�´�.
		auto pNode = this->Find(key);
		if(pNode == NULL)
		{
			pNode = this->RBInsert(key, value);
		}
		else
		{
			if(m_bAutoFree && pNode->m_value)
			{
				//?warning ���� ���� �ι� ��� ���� ���� ���� delete�Ǹ� ū��
				ASSERT(pNode->m_value != value);
				// key�� �״�� �ε� �� _RemoveIndex?
				//_RemoveIndex(key, 0, &pNode->m_value);//?warning ��ġ �ؼ� �Ʒ� "= NULL" ���� ó�� �ؾ�
				DeleteMeSafe(pNode->m_value);
			}
			pNode->m_value = value;
		}
		return(pNode);
	}
	virtual const POSITION FindNode(TKey k)
	{
		//SynchThis();
		return (const POSITION)this->Lookup(k);
	}
/*
	virtual void _Clone(IClonable* pSrc0, int iOp = eCloneDefault)
	{
		auto pSrc = dynamic_cast<ThisType*>(pSrc0);// �̰� �³� ����.
		ASSERT(pSrc);
		if((iOp & eCloneAppend) == 0)
			DeleteAll();

		TKey sKey;
		TObj* pOld = NULL;
		for(POSITION pos = pSrc->GetHeadPosition();pos;)
		{
			pSrc->GetNextAssoc(pos, sKey, pOld);
			TObj* pNew = ClonePObj(pOld);
			SetAt(sKey, pNew);
		}
	}
	template<typename FNC>
	void for_loop(FNC lambda)
	{
		/// C++�ɼǴٺ��⿡�� Conformance mode : No �ؾ� �� Yes(/ permissive - ) �� �Ǿ� ������ ������
		for(POSITION pos = GetHeadPosition(); pos; GetNext(pos))
		{
			TKey k;
			TObj* v;
			GetAt(pos, k, v);
			lambda(k, v);
		}
	}*/
	// for_loopReverse �� GetTailPosition GetPrev
};







