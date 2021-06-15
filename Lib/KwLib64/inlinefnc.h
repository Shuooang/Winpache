#pragma once

#include "ktypedef.h"

/// //////////////////////////////////////
/// level 2
/// ����� � �Լ��� �Ⱦ��� ��� �ִ´�.

#define for_each0(n)  for(int _i=0;_i<(n);_i++)

/// Ư�� ��Ʈ �˻�
inline bool KwAttr(int val, int attr)
{
	return (val & attr) == attr;
}
/// Ư�� ��Ʈ �˻� Not
//inline bool KwNotAttr(int val, int attr)
//{
//	return (val & attr) != attr;
//}
/// Ư�� ��Ʈ �˻��� ����
inline bool KwAttrDel(int& val, int attr)
{
	if(KwAttr(val, attr))
	{
		val = (val & ~attr);
		return true;
	}
	return false;
}
/// Ư�� ��Ʈ �˻��� �߰�
inline bool KwAttrAdd(int& val, int attr)
{
	if(KwAttr(val, attr))
		return true;
	else
		val |= attr;
	return false;
}
inline bool KwAttrXor(int& val, int attr)
{
	if(KwAttr(val, attr))
	{
		val = (val & ~attr); //������ ����
		return true;
	}
	else
		val |= attr;//������ �߰�
	return false;
}
/// �׳� ������ �� ����
inline int KwAttrRemove(int val, int attr)
{
	return (val & ~attr);
}


// attr �� ���� ���յ� ��� �� �� �ϳ��� ��ġ �ϸ� true
inline bool KwAttrOr(int val, int attr)
{
	return (val & attr) != 0;
}
#define KwOp(attr) KwAttr(iOp, attr)


inline bool KwIsAlpha(WCHAR c)
{
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}
inline bool KwIsDigit(WCHAR c)
{
	return ('0' <= c && c <= '9');
}
inline bool KwIsAscii(WCHAR c)
{
	return (' ' <= c && c <= '~');// 0x20 ~ 0x7e
}
inline bool KwIsAlNum(WCHAR c)
{
	return KwIsAlpha(c)  || KwIsDigit(c);
}




template< class T >
void KwSwap(T& a, T& b)
{
	T c;
	c = a; a = b; b = c;
}
template< class T >
void KwArrage(T& a, T& b)
{
	if(a > b)
		KwSwap(a, b);
}

template< class T >
T AbsT(T d)
{
	if(d < (T)0)
		d *= (T)(-1);
	return d;
}


template<typename TOBJ>
inline void DeleteMeSafe(TOBJ*& p)
{
	if(p)
	{
		//TOBJ::operator delete(p);
		delete p; //TOBJ �� �ݵ�� ���ǵ� clas �����̾�� �Ѵ�.
		p = NULL;
	}
}



#define _break {int ____i = 0;}
#define __break {int ____i = 0;} break;


#define throwLINE throw (int)__LINE__


//������ C++ syntax �� �ִ� ��ũ��
#define CASE_STR(ev)	case ev: return _T(#ev);
#define CASE_STR0(ev)	case ev:  psErr = _T(#ev); break;
#define CASE_ERR(ev)	case ev: sErr = _T(#ev);
