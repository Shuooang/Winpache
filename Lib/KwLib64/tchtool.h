#pragma once

#include "ktypedef.h"
#include "enumval.h"
#include "inlinefnc.h"
#include "KTemple.h"




class TStrSet
{
public:
	TStrSet(int sz)
		: _sz(sz)
	{
	}
	int _sz{ 1 };
	shared_ptr<char[]> sa;
	shared_ptr<wchar_t[]> sw;
	PAS Pas()
	{
		ASSERT(_sz == sizeof(char));
		return sa.get();
	}
	PWS Pws()
	{
		ASSERT(_sz == sizeof(wchar_t));
		return sw.get();
	}
};



template<typename TCH>
TCH  tchlower(const TCH c) { return ((c)-'A'+'a'); }
template<typename TCH>
BOOL  tchiscap(const TCH c) { return ('A' <= c && c <= 'Z'); }
template<typename TCH>
TCH  tchicase(const TCH c) { return tchiscap(c) ? tchlower(c):c; }

template<typename TCH>
TCH tchtolower(TCH uc)
{
	const int gab = 'a' - 'A';
	if ((TCH)'A' <= uc && uc <= (TCH)'Z')
		return (TCH)(uc + gab);
	else
		return uc;
}
template<typename TCH>
TCH tchtoupper(TCH uc)
{
	const int gab = 'a' - 'A';
	if ((TCH)'a' <= uc && uc <= (TCH)'z')
		return (TCH)(uc - gab);
	else
		return uc;
}

template<typename TCH>
int tchcmpchi(const TCH c1, const TCH c2)
{
	return tchicase(c1) - tchicase(c2);
}


template<typename TCH>
TCH* tchchr(const TCH* wcs, int ch)
{
	ASSERT(ch);
	while(*wcs && *wcs != (TCH)ch)
		wcs++;
	if(*wcs == (TCH)ch)
		return (TCH*)wcs;
	return NULL;
}
template<typename TCH>
TCH* tchrchr(const TCH* wcs, int ch)
{
	ASSERT(ch);
	TCH* fnd = NULL;
	for(;*wcs;wcs++)
	{
		if(*wcs == (TCH)ch)
			fnd = (TCH*)wcs;//계속 저장 하면, 맨 마지막것이
	}
	return fnd;
}
template<typename TCH>
INT_PTR tchchrx(const TCH* wcs0, int ch, int start = 0)
{
	TCH* wcs = (TCH*)wcs0 + start;
	while(*wcs && *wcs != (TCH)ch)
		wcs++;
	if(*wcs == (TCH)ch)
		return (TCH*)wcs - wcs0;
	return -1;
}
template<typename TCH> //, CStringT< TCH, StrTraitATL< TCH > > >
int tchrchrx(const TCH* wcs1, TCH c, int start = -1)
{
	if(start == -1)
		start = tchlen(wcs1) -1;
	if(start >= 0)
	{
		for(int i=start;i>=0;i--)
		{
			if(wcs1[i] == c)
				return i;
		}
	}
	return -1;
}

template<typename TString> //, CStringT< TCH, StrTraitATL< TCH > > >
int tchstrrchr(TString& s, WORD c, int start = -1)
{
	if(start == -1)
		start = s.GetLength()-1;

	for(int i=start;i>=0;i--)
	{
		if(s.GetAt(i) == c)
			return i;
	}
	return -1;
}
template<typename TCH>
TCH* tchstr(const TCH* wcs1, const TCH* wcs2)
{
	TCH* cp = (TCH*)wcs1;
	TCH* s1, * s2;
	if(!*wcs2)
		return (TCH*)wcs1;
	if(cp == NULL)
		return NULL;
	while(*cp)
	{
		s1 = cp;
		s2 = (TCH*)wcs2;
		while(*s1 && *s2 && !(*s1 - *s2))
			s1++, s2++;
		if(!*s2)
			return cp;

		cp++;
	}
	return NULL;
}
// _tolower(*src)
template<typename TCH>
TCH* tchstri(const TCH* wcs1, const TCH* wcs2)
{
	TCH* cp = (TCH*)wcs1;
	TCH* s1, * s2;
	if(!*wcs2)
		return (TCH*)wcs1;
	if(cp == NULL)
		return NULL;
	while(*cp)
	{
		s1 = cp;
		s2 = (TCH*)wcs2;
		while(*s1 && *s2 && tchcmpchi(*s1, *s2) == 0)//!(_tolower(*s1) - _tolower(*s2)))
			s1++, s2++;
		if(!*s2)
			return cp;

		cp++;
	}
	return NULL;
}

template<typename TCH>
__int64 tchstrx(const TCH* wcs1, const TCH* wcs2, int start = 0)
{
	TCH* cp = (TCH*)(wcs1 + start);
	TCH* s1, * s2;
	if(!*wcs2) // '\0'
		return -1;//(TCH*)wcs1;

	while(*cp)
	{
		s1 = cp;
		s2 = (TCH*)wcs2;
		while(*s1 && *s2 && !(*s1 - *s2))
			s1++, s2++;
		if(!*s2)
			return cp -wcs1;

		cp++;
	}
	return -1; // size_t는 unsigned 이므로 -1로 하면 혼동 온다.
}

// c로 구분된 문자열 몇번째 마디(컬럼)에서 발견 되었나(0,1,2,) 없으면 -1
template<typename TCH>
size_t tchstridx(const TCH* wcs1, const TCH* wcs2, const TCH c = (const TCH)'\t')
{
	TCH* cp = (TCH*)wcs1;
	TCH* s1, * s2;
	if(!*wcs2)
		return -1;
	size_t idx = 0;
	while(*cp)
	{
		s1 = cp;
		s2 = (TCH*)wcs2;
		if(!(*cp - c))
			idx++;
		while(*s1 && *s2 && !(*s1 - *s2))
			s1++, s2++;
		if(!*s2)
			return idx;

		cp++;
	}
	return -1;
}

// size_t  가 아니고 int 를 쓰는 이유는, 어차피 2기가 최대 메모리 할당 이므로
// int 0x7fffffff 2giga 이므로 CString 의 인덱스가 대부분 int 이므로 매번 cast하기 번거로워서 int로 한다.
template<typename TCH>
int tchlen(const TCH* wcs)
{
	if(wcs == NULL)
		return 0;
	TCH* eos = (TCH*)wcs;
	while(*eos++);
	return((int)(eos - wcs - 1));
}

// 쓰는데 없지만, 나중에 size_t 형 리턴을 바라는 경우 사용
template<typename TCH>
size_t tchlenl(const TCH* wcs)
{
	if(wcs == NULL)
		return 0;
	TCH* eos = (TCH*)wcs;
	while(*eos++);
	return ((size_t)(eos - wcs - 1));
}

template<typename TCH>
BOOL tchdigit(const TCH* wcs)
{
	if(wcs == NULL)
		return FALSE;
	while(*wcs)
	{
		if(!KwIsDigit(*wcs++))
			return FALSE;
	}
	return TRUE;
}



template<typename TCH>
const TCH* tchncpy(TCH* dest0, const TCH* source, size_t count, TCH chFillEnd = 0)
{
	TCH* dest = (TCH*)dest0;

	while(count && (*dest++ = *source++))    /* copy string */
		count--;

	if(count)                              /* pad out with zeroes */
		while(--count)
			*dest++ = chFillEnd;//L'\0';
	//?주의 count가 '\0'을 포함한 갯수 인데 source가 '\0'을 포함 하지 않았다면 부른 후 반드시 뒤에 '\0'을 붙여 줘야 핟다.	
	return(dest0);
}



template<typename TCH>
const TCH* tchcat(const TCH* dst, const TCH* src)
{
	TCH* cp = (TCH*)dst;
	while(*cp)
		cp++;                   /* find end of dst  */
	while(*cp++ = *src++);       /* Copy src to end of dst */
	return(dst);                  /* return dst */
}

template<typename TCH>
const TCH* tchcpy(const TCH* dst, const TCH* src)
{
	TCH* cp = (TCH*)dst;

	while(*cp++ = *src++)
		;               /* Copy src over dst */
	return(dst);
}

enum { eCpnInt, eCpnFloat, eCpnCutZero, eCpnNormal };
template<typename TCH>
const TCH* tchcpynum(const TCH* dst, const TCH* src, int iOp = 3)
{
	// if iOp==1 이면 double 로 소숫점까지 복사, 0이면 '.'에서끝낸다.
	// 2: 뒤에 0 잘라 내기
	TCH* cp = (TCH*)dst;
	int j=0;
	BOOL bDot = 0;
	BOOL bOver = 0;
	for(int i=0;i<1024;i++)//숫자가 가장 긴것을 1024길이로 본거지.
	{
		TCH c = src[i];
		if(((TCH)'0' <= c && c <= (TCH)'9') || c == (TCH)'-' || c == (TCH)'\0')
			cp[j++] = c;
		else if(c == (TCH)'.')
		{
			bDot = 1;
			if(iOp == eCpnInt || bDot)// . 이 나왔는데 정수? 이미 나왔어? 끝내자.
			{
				cp[j++] = (TCH)'\0';
				break;
			}
			else
				cp[j++] = c;
		}
		//if(iOp == eCpnInt && c == (TCH)'.')
		//	break;
		if(c == (TCH)'\0')
			break;
		if(i >= 126)
		{
			cp[j++] = (TCH)'\0';
			bOver = 1;
			break;
		}
	}
	if((iOp & eCpnNormal) == eCpnNormal && bDot && j > 1)
	{
		for(int r=j-2;r>0;r--)
		{
			TCH c = cp[r];
			if(c == (TCH)'.')
			{
				cp[r] = (TCH)'\0';
				break;
			}
			else if(c == (TCH)'0')// 뒤에 0 계속 제거
				cp[r] = (TCH)'\0';
			else
				break;
		}
	}
	return bOver ? NULL : dst;
}



template< class Tint >
inline LPCSTR KwItoaA(Tint iv, CHAR* pbuf, size_t szBuf = 10)
{
	_itoa_s((int)iv, (LPTSTR)pbuf, szBuf, 10);
	return (LPCSTR)pbuf;
}

template< class Tchar >
inline __int64 KwAtoi64(LPCWSTR sdv)
{
	Tchar p[127] = { '\0', };
	tchcpynum(p, sdv);
	if(sizeof(Tchar) == 1)
		return _atoi64((char*)p);
	else
		return _wtoi64((wchar_t*)p);
	//	return _ttoi64(sdv);
}

template< class Tchar >
inline int KwAtoi(const Tchar* sdv)
{
	Tchar p[127] = { '\0', };
	tchcpynum(p, sdv);
	if(sizeof(Tchar) == 1)
		return atoi((char*)p);
	else
		return _wtoi((wchar_t*)p);
}
template< class Tchar >
inline double KwAtof(const Tchar* sdv)
{
	Tchar p[127] = { '\0', };
	tchcpynum(p, sdv);
	if(sizeof(Tchar) == 1)
		return atof((char*)p);
	else
		return _wtof((wchar_t*)p);
}


template<typename TCH>
int tchcmp(const TCH* src, const TCH* dst)
{
	int ret = 0;
	if(src == dst)
		return 0;
	if(!src && dst)
		return -1;
	if(src && !dst)
		return 1;

	if(src && dst)
	{
		while(!(ret = (int)(*src - *dst)) && *dst)
			++src, ++dst;

		if(ret < 0)
			ret = -1;
		else if(ret > 0)
			ret = 1;
	}
	else
	{
		if(src)
			ret = 1;
		else
			ret = -1;
	}

	return(ret);
}

template<typename TCH>
bool tchsame(const TCH* src, const TCH* dst)
{
	return tchcmp(src, dst) == 0;
}

template<typename TCH>
bool tchnsame(const TCH* src, const TCH* dst, size_t count)
{
	return tchncmp(src, dst, count) == 0;
}

template<typename TCH>
bool tchbegin(const TCH* src, const TCH* dst)
{
	return tchncmp(src, dst, tchlen(dst)) == 0;
}
template<typename TCH>
bool tchend(const TCH* src, const TCH* dst)
{
	auto esrc = tchlen(src);
	auto edst = tchlen(dst);
	if(esrc >= edst)
	{
		return tchncmp(src + (esrc - edst), dst, edst) == 0;
	}
	return false;
}
template<typename TCH>
int tchncmp(const TCH* src, const TCH* dst, size_t count)
{
	ASSERT(src && dst);
	if(!count)
		return(0);
	while(--count && *src && *src == *dst)
	{
		src++;
		dst++;
	}
	int ret = (*(TCH*)src - *(TCH*)dst);
	if(ret > 1) ret = 1;
	else if(ret < -1) ret = -1;
	return ret;
	/*
		if(count)
		{
			do
			{
				dst++;
				src++;
			}
			while ( (--count) && *dst && (*dst == *src) );
			return (int)(*dst - *src);
		}q
		else
			return 0;
	*/
}

template<typename TCH>
int tchicmp(const TCH* src, const TCH* dst)
{
	ASSERT(src && dst);
	int ret = 0;

	//while(!(ret = (int)(_tolower(*src) - _tolower(*dst))) && *dst)
	while(!(ret = (int)tchcmpchi(*src, *dst)) && *dst)
		++src, ++dst;

	if(ret < 0)
		ret = -1;
	else if(ret > 0)
		ret = 1;

	return(ret);
}

template<typename TCH>
bool tchisame(const TCH* src, const TCH* dst)
{
	return tchicmp(src, dst) == 0;
}

template<typename TCH>
bool tchnisame(const TCH* src, const TCH* dst)
{
	return tchnicmp(src, dst) == 0;
}

template<typename TCH>
bool tchibegin(const TCH* src, const TCH* dst)
{
	return tchnicmp(src, dst, tchlen(dst)) == 0;
}

template<typename TCH>
int tchnicmp(const TCH* dst, const TCH* src, size_t count)
{
	ASSERT(src && dst);
	TCH f, l;

	if(count)
	{
		do
		{
			f = tchtolower(*dst);
			l = tchtolower(*src);
			dst++;
			src++;
		} while((--count) && (f) && (f == l));
	}
	return (int)(f - l);
}



/*
template<typename TCH>
TCH* tchinc(TCH* uc)
{
	return ++uc;
}*/

template<typename TCH>
int tchHashcode(TCH* value)
{
	int h = 0;//hash;
	TCH* val = value;
	for(; *val; val++)
	{
		h = 31*h + *val;
	}
	return h;
}






template<typename TCH>
bool tchSpace(TCH c)
{
	static const TCH ps[5] ={' ', '\t', '\r', '\n', '\0'};
	TCH* p = (TCH*)ps;
	for(;*p;p++)
	{
		if(c == *p)
			return true;
	}
	return false;
}

// UNICODE전용
inline bool KwIsSpace(WCHAR c)
{
	return tchchr(L" \t\r\n", c) != NULL;
}





template<typename TCH>
TCH* tchgetstr(const TCH* wcs1, const int idx, int& len, const TCH c = (const TCH)'\t')
{
	TCH* cp = (TCH*)wcs1;
	TCH* s1 = NULL;
	int i = 0;
	TCH* fp = cp;
	len = 0;
	while(*cp)
	{
		if(i == idx && s1 == NULL)
			s1 = cp;

		if(*cp == c || *cp == '\0') // tab 이면
		{
			if(s1)
				return s1;
			i++;
		}
		if(s1)
			len++;
		cp++;
	}
	return NULL;
}


template<typename TCH>
bool tchisdigit(TCH c)
{
	return ('0' <= c && c <= '9');
}

// ASCII -> Number
template<typename TCH>
int tchatonum(const TCH* wcs1)
{
	TCH* cp = (TCH*)wcs1;
	int l = tchlen(wcs1);
	TCH* p = 
#ifdef _DEBUG
		DEBUG_NEW
#else
		new
#endif // _DEBUG
		TCH[l+1];
	int i=0;
	for(;i<l;i++)
	{
		TCH c = wcs1[i];
		if(tchisdigit(c) || (c == '-' && i==0))
			p[i] = c;
	}
	p[i] = '\0';

	return KwAtoi(p);
}




// tok 안에 ch 가 존재 하는지
template<typename TCH>
bool tchisin(TCH ch, const TCH* tok)
{
	ASSERT(tok);
	for(TCH* p = tok; *p; p++)
	{
		if(*p == ch)
			return true;
	}
	return false;
}


template<typename TCH>
int KwTextCmpT(const TCH* ps1, const TCH* ps2, int iOp, int len)
{
	int rv = 0;
	if(len < 0)
	{
		if(KwAttr(iOp, eCmprCase))
			rv = tchcmp(ps1, ps2);
		else
			rv = tchicmp(ps1, ps2);
	}
	else
	{
		ASSERT(len > 0);
		if(KwAttr(iOp, eCmprCase))
			rv = tchncmp(ps1, ps2, len);
		else
			rv = tchnicmp(ps1, ps2, len);
	}
	return rv;
}

template<typename TPCH>
void KwTrimSpace(TPCH& pcb, TPCH& pnt)
{
	while(pcb < pnt && *pcb == ' ')
		pcb++;

	while(pcb < pnt && *(pnt-1) == ' ')
		pnt--;
}

template<typename TV>
void KwSort2(TV& v1, TV& v2)
{
	if(v2 < v1)
	{
		TV t = v1;
		v1 = v2;
		v2 = v1;
	}
}

// 각도구한다. 작은쪽 방향 구한다.
template<typename T>
T KwDgreeDev(T h1, T h2)
{
	T h =  0;
	if(h1 > h2)
		KwSwap(h1, h2);

	h = h2 -h1; // 큰 - 작 = +차
	if(h > 180) // 반대 방향의 각도차 구해야
	{
		ASSERT(h2 > 180);
		h1 += 360;	// 작은것을 한 회 돌린후 거기서 뺀다.
		h = h1 -h2;
		ASSERT(h <= 180);
	}
	return h;
}

template<typename T>
T KwDgreeAvg(T h1, T h2)
{
	T h =  0;
	if(h1 > h2)
		KwSwap(h1, h2);

	h = h2 -h1; // 큰 - 작 = +차
	if(h > 180) // 반대 방향의 각도차 구해야
	{
		ASSERT(h2 > 180);
		h1 += 360;	// 작은것을 한 회 돌린후 거기서 뺀다.
		h = h1 -h2; // 다른쪽 차이 구해
		T av = h2 + (h/2); // 작은것 + 반차
		if(av >= 360) // 한바퀴 넘의면 제거
			av -= 360;
		return av;
	}
	else
		return h1 + (h/2);
}

template<typename T>
T KwDgreeAvgN(T h1, T h2, int n1, int n2 = 1)
{
	T h =  0;
	if(h1 > h2)
	{
		KwSwap(h1, h2);
		KwSwap(n1, n2);
	}

	h = h2 -h1; // 큰 - 작 = +차
	if(h > 180) // 반대 방향의 각도차 구해야
	{
		ASSERT(h2 > 180);
		h1 += 360;	// 작은것을 한 회 돌린후 거기서 뺀다.
	}

	T av = ((h1*n1) + (h2*n2)) / (n1+n2);
	if(av >= 360) // 한바퀴 넘의면 제거
		av -= 360;
	return av;
}

#define ISWORD(c) (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || c=='_'  || KwIsHangul(c))

template<typename TStr, typename TCH>
int tstristrIndex(TStr& str, int start, const TCH* pKey, BOOL bWord)
{
	if(!*pKey)
		return NULL;

	TStr key = pKey;
	int len = key.GetLength();//lstrlen(key);
	int lenMatch = 0;
	const int gab = 'a' - 'A';

	int iFind = -1;
	//LPTSTR lpFind = NULL;

//	for(LPTSTR lp = (LPTSTR)str;;lp++)
	for(int i=start;i<str.GetLength();i++)
	{
		TCH c = str.GetAt(i);//*lp;
		TCH k = key[lenMatch];
		if(c == k)
			lenMatch++;
		else if(ISWORD(c))
		{
			if(c >= 'a' && c <= 'z')	// toupper
				c += gab;
			if(c == k)
				lenMatch++;
			else
				lenMatch = 0;
		}
		else
			lenMatch = 0;

		if(lenMatch == 1)
		{
			//			if(bWord && !(lp == str || !ISWORD(*(lp-1))))	// top or prvious char is not alpha
			if(bWord && !(i == start || !ISWORD(str.GetAt(i-1))))	// top or prvious char is not alpha
				lenMatch = 0;
			else
				iFind = i;//lpFind = lp;
		}
		if(lenMatch == len)
		{
			//			if(bWord && !(i == start || !ISWORD(str.GetAt(i+1))))	// top or prvious char is not alpha
			if(bWord && !(i == len-1 || !ISWORD(str.GetAt(i+1))))	// top or prvious char is not alpha
				lenMatch = 0;
			else
				return iFind;
		}
	}
	return -1;
}



//1>C:\Dropbox\Proj\CmnJ\KwLib64\inlinefnc.h(24, 19) : error C4430: 형식 지정자가 없습니다.int로 가정합니다.참고: C++에서는 기본 int를 지원하지 않습니다.
//template< class Tint >
//inline PWS KwItoaW(int iv, WCHAR* pbuf, size_t szBuf = 10)
//{
//	_itow_s((int)iv, pbuf, szBuf, 10);
//	return (PWS)pbuf;
//}

template<typename TCH>
TStrSet tchprecede(const TCH* wcs, int ch)
{
	ASSERT(ch);
	auto i = tchchrx(wcs, ch);
	TStrSet sst(sizeof(TCH));
	if(i >= 0)
	{
		if(sst._sz == 1) //sizeof(TCH) == 1)(char*)operator new( buffer_size_here )
		{
			sst.sa = shared_ptr<char[]>((char*)new char[i + 1]);
			char* p = sst.sa.get();
			p[i] = (TCH)0;
			tchncpy((char*)p, (const char*)wcs, i);
		}
		else
		{
			sst.sw = shared_ptr<wchar_t[]>((wchar_t*)new wchar_t[i + 1]);
			wchar_t* p = sst.sw.get();
			p[i] = (TCH)0;
			tchncpy((wchar_t*)p, (const wchar_t*)wcs, i);
		}
	}
	return sst;
}

template<typename TCH>
TStrSet tchbehind(const TCH* wcs, int ch)
{
	ASSERT(ch);
	auto i = tchchrx(wcs, ch);
	auto len = tchlen(wcs);
	TStrSet sst(sizeof(TCH));
	if(i >= 0)
	{
		auto blen = len - i - 1;// 12 - 7 - 1
		if(sst._sz == 1) //sizeof(TCH) == 1)(char*)operator new( buffer_size_here )
		{
			sst.sa = shared_ptr<char[]>((char*)new char[blen + 1]);
			char* p = sst.sa.get();
			p[0] = (TCH)0;
			p[blen] = (TCH)0;
			tchncpy((char*)p, (const char*)(wcs + i + 1), blen); // ch 다음 부터 복사 한다.
		}
		else
		{
			sst.sw = shared_ptr<wchar_t[]>((wchar_t*)new wchar_t[blen + 1]);
			wchar_t* p = sst.sw.get();
			p[0] = (TCH)0;
			p[blen] = (TCH)0;
			tchncpy((wchar_t*)p, (const wchar_t*)(wcs + i + 1), blen);
		}
	}
	return sst;
}
