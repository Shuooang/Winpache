#pragma once

#include "ktypedef.h"

/// //////////////////////////////////////
/// level 2
/// 여기는 어떤 함수도 안쓰는 경우 넣는다.

#define for_each0(n)  for(int _i=0;_i<(n);_i++)

/// 특정 비트 검사
inline bool KwAttr(int val, int attr)
{
	return (val & attr) == attr;
}
/// 특정 비트 검사 Not
//inline bool KwNotAttr(int val, int attr)
//{
//	return (val & attr) != attr;
//}
/// 특정 비트 검사후 제거
inline bool KwAttrDel(int& val, int attr)
{
	if(KwAttr(val, attr))
	{
		val = (val & ~attr);
		return true;
	}
	return false;
}
/// 특정 비트 검사후 추가
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
		val = (val & ~attr); //있으면 제거
		return true;
	}
	else
		val |= attr;//없으면 추가
	return false;
}
/// 그냥 삭제한 값 리턴
inline int KwAttrRemove(int val, int attr)
{
	return (val & ~attr);
}


// attr 이 미이 조합된 경우 그 중 하나라도 일치 하면 true
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
		delete p; //TOBJ 는 반드시 정의된 clas 형태이어야 한다.
		p = NULL;
	}
}



#define _break {int ____i = 0;}
#define __break {int ____i = 0;} break;


#define throwLINE throw (int)__LINE__


//순전히 C++ syntax 만 있는 매크로
#define CASE_STR(ev)	case ev: return _T(#ev);
#define CASE_STR0(ev)	case ev:  psErr = _T(#ev); break;
#define CASE_ERR(ev)	case ev: sErr = _T(#ev);
