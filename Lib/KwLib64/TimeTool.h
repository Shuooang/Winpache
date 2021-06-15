#pragma once

#include "KTemple.h"




#define NULLYEAR 1980
#define NULLMONTH 1
#define NULLDAY   1









#ifdef DEBUG



template<typename TTIMESP>
class CTimeDumpSP
{
public:
	CString m_s;
	virtual void _dump() {}
	void SetDebugSP()
	{
		TTIMESP* psp = dynamic_cast<TTIMESP*>(this);
		LONGLONG days = (LONG)psp->GetDays();
		m_s.Format(_T("(%I64d) %02d:%02d:%02d"), days, psp->GetHours(), psp->GetMinutes(), psp->GetSeconds());
	}
};


template<typename TTIME>
class CTimeDump
{
public:
	CString m_s;
	virtual void _dump() {}
	void SetDebug(TTIME* psp)
	{
		//TTIME* psp = dynamic_cast<TTIME*>(this);
		CStringW sd;
		//PWS KwDate(int iDayOfWeek, CString & sDate, bool bEn = false);
		KwDate(psp->GetDayOfWeek(), sd, true);
		m_s.Format(L"%04d-%02d-%02d %02d:%02d:%02d(%s)", psp->GetYear(), psp->GetMonth(), psp->GetDay(), psp->GetHour(), psp->GetMinute(), psp->GetSecond(), (PWS)sd);
	}
};

class CKTimeSpan
	: public CTimeSpan
	, public CTimeDumpSP<CTimeSpan>
{
public:

	CKTimeSpan() throw()
		: CTimeSpan()
	{
		SetDebugSP();
	}
	CKTimeSpan(__time64_t time) throw()
		: CTimeSpan(time)
	{
		SetDebugSP();
	}
	CKTimeSpan(CTimeSpan time) throw()
		: CTimeSpan(time)
	{
		SetDebugSP();
	}
	CKTimeSpan(LONG lDays, int nHours, int nMins, int nSecs) throw()
		: CTimeSpan(lDays, nHours, nMins, nSecs)
	{
		SetDebugSP();
		/// summer time제 때문에 날짜 추가 계산을 이것으로 쓰면 안된다.
		// 반드시 KwNextDay 를 사용 해야 한다.
		//ASSERT(!(lDays > 0 && (nHours + nMins + nSecs) == 0));
	}
};

class CKTime
	: public CTime             // 2. 바꾼후로 안죽네
	, public CTimeDump<CTime> // 1. <CTime>이었는데. CTimeDump.SetDebug dynamic_cast에서 죽더라
{
public:

	CKTime() throw()
	{
		SetDebug(this);
	}
	CKTime(__time64_t time) throw()
		: CTime(time)
	{
		SetDebug(this);
	}
	CKTime(const CTime& time) throw()
		: CTime(time)
	{
		SetDebug(this);
	}
	CKTime(const CKTime& time) throw()
		: CTime(time)
	{
		SetDebug(this);
	}
	//CKTime( CKTime& time ) throw()
	//	: CTime(time)
	//{	SETTIMEDEBUG();
	//}
	CKTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec, int nDST = -1)
		: CTime(nYear, nMonth, nDay, nHour, nMin, nSec, nDST)
	{
		SetDebug(this);
	}
	CKTime(WORD wDosDate, WORD wDosTime, int nDST = -1)
		: CTime(wDosDate, wDosTime, nDST)
	{
		SetDebug(this);
	}
	CKTime(const SYSTEMTIME& st, int nDST = -1)
		: CTime(st, nDST)
	{
		SetDebug(this);
	}
	CKTime(const FILETIME& ft, int nDST = -1)
		: CTime(ft, nDST)
	{
		SetDebug(this);
	}

	__time64_t operator=(__time64_t time) throw()
	{
		CTime t = __super::operator=(time);
		SetDebug(this);
		return t.GetTime();
	}
	__time64_t operator=(const CTime& time) throw()
	{
		return operator=(time.GetTime());
	}
	__time64_t operator=(const CKTime& time) throw()
	{
		return operator=(time.GetTime());
	}

	void operator+=(CTimeSpan span) throw()
	{
		__super::operator+=(span);
		SetDebug(this);
	}
	void operator-=(CTimeSpan span) throw()
	{
		__super::operator-=(span);
		SetDebug(this);
	}
};

#else

typedef CTimeSpan CKTimeSpan;
typedef CTime CKTime;
#endif

// 날짜 없이 시간만 가지는 객체
// CHashData_deprecated::GetTm
// 시간만 Set 하고자 할때
class KTime
{
public:
	KTime()
		: m_h(0)
		, m_m(0)
		, m_s(0)
	{
	}
	//d:\proj\kwlib32\KObject.h(35) : error C2652: 'KTime' : 복사 생성자가 잘못되었습니다. 첫째 매개 변수가 'KTime'이(가) 아니어야 합니다.
	//d:\proj\kwlib32\KObject.h(26) : 'KTime' 선언을 참조하십시오.
		//KTime(KTime t)
		//explicit  
	// 	.\KObject.cpp(1505) : error C2558: class 'KTime' : 복사 생성자를 사용할 수 없거나 복사 생성자가 'explicit'으로 선언되었습니다.
	// 		explicit KTime(const KTime& t)

	KTime(const KTime& t)
		: m_h(t.m_h)
		, m_m(t.m_m)
		, m_s(t.m_s)
	{
	}
	KTime(SYSTEMTIME& t)
		: m_h(t.wHour)
		, m_m(t.wMinute)
		, m_s(t.wSecond)
	{
	}
	KTime(CTime t)
		: m_h(t.GetHour())
		, m_m(t.GetMinute())
		, m_s(t.GetSecond())
	{
	}
	int m_h, m_m, m_s;

	// 날짜는 tt 로 하고 CTime 을 조합 하여 리턴
	CTime GetCTime(CTime tt)
	{
		return GetCTime((__time64_t)tt.GetTime());
	}
	CTime GetCTime(__time64_t tt = 0)
	{
		if(tt > 0)
		{
			CTime t1(tt);
			return CTime(t1.GetYear(), t1.GetMonth(), t1.GetDay(), m_h, m_m, m_s);
		}
		return CTime(NULLYEAR, NULLMONTH, NULLDAY, m_h, m_m, m_s);
	}
};
typedef __time64_t TimeT;//64bit time으로 통일

class CTm
	: public tm
{
public:
	CTm()
	{
		memset(this, 0, sizeof(tm));
	}
	CTm(const wchar_t* strT)
	{
		StrToTm(strT);
	}
	void StrToTm(const wchar_t* strT);
	int Year() { return tm_year + 1900; }
	int Month() { return tm_mon + 1; }
	int Day() { return tm_mday; }
	int Hour() { return tm_hour; }
	int Min() { return tm_min; }
	int Sec() { return tm_sec; }
	CString ToString();
};


class CKwTime
{
public:
	TimeT m_ltime;
#ifdef _DEBUG
	wchar_t m_st[30];
#endif // _DEBUG
	void Dump()
	{
#ifdef _DEBUG
		TimeStr(m_ltime);
#endif // _DEBUG
	}
public:
	CKwTime(TimeT ltime = 0)
		: m_ltime(ltime)
	{
		if(ltime == 0)// default로 현재 시각
		{
			time(&m_ltime);
			Dump();
		}
	}
public:
	CKwTime(struct tm* timeptr)
	{
		m_ltime = mktime(timeptr);
		Dump();
	}

	TimeT GetTime()
	{
		return m_ltime;
	}
	TimeT GetLTime();

	void SetTime(TimeT ltime)
	{
		m_ltime = ltime;
		Dump();
	}

	TimeT GetMidnight(TimeT ltime = 0);

	CTm ToUTC(TimeT ltime = 0);

	CTm ToLocal(TimeT ltime = 0);

	//	+tmpbuf	0x0012fee0       "Today is Sunday, day 17 of October in the year 2010."	char [128]
	// strftime( tmpbuf, 128,"Today is %A    , day %d of %B      in the year %Y  .\n", &today );
	//
	//	err = ctime_s(timebuf, 26, &ltime); Time in seconds since UTC 1/1/70:   1051303883

	// 	char tmpbuf[128], timebuf[26], ampm[] = "AM";
	// 	time_t ltime;
	// 	struct _timeb tstruct;
	// 	struct tm today, gmt, xmas = { 0, 0, 12, 25, 11, 93 };

	void To12Hour();
	int TimeDifference();
	bool IsDaylightSaving();
	char* GetTimeZone();

	CStringA TimeStrA(TimeT ltime = 0);
	CStringW TimeStr(TimeT ltime = 0);
};


enum ETmArea
{
	eCtLocal, // app 이 실행 되는 곳
	eCtUTC,   // GMT
	eCtSvr,   // server machine의 로컬 시각
};
class CLocalTime
{
public:
	CLocalTime()
		: m_hrLocalDiff(0), m_eUTC(eCtUTC)
	{
	}
	ETmArea m_eUTC;
	int m_hrLocalDiff;// 서버 로컬시간과 사용되는 지역의 로컬 시간
	CTime GetNow_(ETmArea eOp = eCtUTC);
	TimeT GetNowT_(ETmArea eOp = eCtUTC) { return GetNow_(eOp).GetTime(); }
	CString GetNowStr(ETmArea eOp = eCtUTC);
	CString GetNowDStr(ETmArea eOp = eCtUTC);

public:
	static TIME_ZONE_INFORMATION* s_pTZ;
	static CLocalTime s_ltime;

	static CTime GetNow__(ETmArea eOp) { return s_ltime.GetNow_(eOp); }
	static CTime GetLocalTime__() { return GetNow__(eCtLocal); }
	static TimeT GetNowT__(ETmArea eOp = eCtUTC) { return s_ltime.GetNowT_(eOp); }
	static TimeT UtcToLocal(TimeT stm, TIME_ZONE_INFORMATION* ptz = NULL);
	//	static TimeT UtcToLocal(CTime& t);
	static TimeT UtcToLocal(SYSTEMTIME& stm, TIME_ZONE_INFORMATION* ptz = NULL);
	static TimeT LocalToUtc(TimeT stm, TIME_ZONE_INFORMATION* ptz = NULL);
	static TimeT LocalToUtc(SYSTEMTIME& stm, TIME_ZONE_INFORMATION* ptz = NULL);
	static int GetTimeZoneInformationByName(TIME_ZONE_INFORMATION* ptzi, LPCWSTR StandardName);
};

/// <summary>
/// time paring
/// </summary>
void        KwParseTimeStr(SYSTEMTIME& syt, LPCWSTR psEngTime);
__time64_t  KwParseTimeStr2(SYSTEMTIME& syt, LPCWSTR psEngTime);
__time64_t  KwParseTimeStr4(LPCWSTR psEngTime, SYSTEMTIME* pSyt = NULL, bool bDateOnly = false);
DATE        KwParseTimeStr6(LPCWSTR psEngTime, SYSTEMTIME* pSyt = NULL, bool bDateOnly = false);
SYSTEMTIME* KwParseTimeStr5(LPCWSTR psEngTime, SYSTEMTIME* pSyt, bool bDateOnly = false);
bool        KwParseTimeDurationStr1(LPCWSTR psst, CTimeSpan& sp);
SYSTEMTIME* KwParseTimeStr10(LPCWSTR psEngTime, SYSTEMTIME* pSyt, bool bDateOnly = false);
CStringW    KwParseTimeStrToStr(LPCWSTR pst);


/// <summary>
/// time type conversion
/// </summary>
__time64_t   KwStdTimeToTime(LPCWSTR psEngTime);
LPSYSTEMTIME KwCStringToTime10(LPCWSTR strData, LPSYSTEMTIME pSt, bool bDateOnly = false);

template<typename TTIME>
void KwCTimeToSystime(TTIME t, LPSYSTEMTIME pSt)
{
	KwZeroArray(pSt);
	//ZeroMemory(pSt, sizeof(SYSTEMTIME));
	pSt->wYear = t.GetYear();
	pSt->wMonth = t.GetMonth();
	pSt->wDay = t.GetDay();
	pSt->wHour = t.GetHour();
	pSt->wMinute = t.GetMinute();
	pSt->wSecond = t.GetSecond();
}

__time64_t KwSystimeToCtime(SYSTEMTIME& st);

DATE KwCTimeToVarDate(CTime t);
CTime KwVarDateToCTime(DATE dt);

CTime KwNextHour(const CTime cTime);

CTime KwNextDay(const CTime cTime, int nday);

COleDateTime KwNextDay(const COleDateTime cTime, int nday);

void KwSystimeToTm(SYSTEMTIME& st, tm* ptm);


/// <summary>
/// time tool
/// </summary>
bool KwIsSameSysTime(LPSYSTEMTIME st0, LPSYSTEMTIME st1);

bool KwIsSameSysDate(LPSYSTEMTIME st0, LPSYSTEMTIME st1);

bool KwInitSysTime(SYSTEMTIME& st);

bool KwCheckTime(SYSTEMTIME& st);

bool KwCheckTime(LPCTSTR strt);

CTime KwCStringToCTime(LPCTSTR strData);

__time64_t KwCStringToCTime(LPCTSTR strData, CTime& cTime);

LPSYSTEMTIME KwCStringToSysTime(LPCTSTR str, LPSYSTEMTIME pSt);

inline LPSYSTEMTIME KwCStringToTime(LPCTSTR strData, LPSYSTEMTIME pSt, bool bDateOnly = false)
{
	return KwCStringToTime10(strData, pSt, bDateOnly);
}

/// <summary>
/// time to string
/// </summary>
LPCWSTR      KwTimeToString(CStringW& sTime, bool bSpace, TCHAR cSpDay, TCHAR cSpTime, int y, int m, int d, int hr, int mn, int sc);

int          KwMonthFromStr10(LPCWSTR sMonth);

LPCTSTR      KwCTimeToString(CTime cTime, CString& sTime, bool bSpace = true, TCHAR cSpDay = '-', TCHAR cSpTime = ':');
LPCTSTR      KwCTimeToDateString(CTime cTime, CString& sTime, bool bSpace = true, TCHAR cSpDay = '-', TCHAR cSpTime = ':');

inline LPCTSTR KwSystimeToString(SYSTEMTIME* psyt, CString& sTime, bool bSpace = true, TCHAR cSpDay = '-', TCHAR cSpTime = ':')
{
	return KwTimeToString(sTime, bSpace, cSpDay, cSpTime, psyt->wYear, psyt->wMonth, psyt->wDay,
		psyt->wHour, psyt->wMinute, psyt->wSecond);
}

CTime KwCStringToCTime(LPCTSTR strData);


struct _EnMon
{
	LPCWSTR smon;
	int mon;
};




/// <summary>
/// current time function
/// </summary>
/// <param name="bUTC"></param>
CTime KwGetCurrentTime(int bUTC = eCtLocal);

// 오늘 날짜를 "1996-07-03"의 형태로 문자열을 리턴한다.
CString KwGetCurrentDateString(int bUTC = eCtLocal);
CString KwGetToday(int bUTC = eCtLocal);

//  현재 시간을 "17:15" 의 문자열로 리턴한다.
CString KwGetCurrentTimeString(int bUTC = eCtLocal);
CString KwGetCurrentTimeFullString(int bUTC = eCtLocal);
COleDateTime KwGetCurrentTimeV(int bUTC = eCtLocal);


LPCWSTR KwDate(int iDayOfWeek, CStringW& sDate, bool bEn = false);




CKTime KwGetTodayTime(int bUTC = eCtLocal);

int KwCompareDate(CKTime t1, CKTime t2);
int KwCompareTimeOnly(CKTime t1, CKTime t2);
