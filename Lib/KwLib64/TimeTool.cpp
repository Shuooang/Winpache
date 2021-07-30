#include "pch.h"
#include "TimeTool.h"
#include "inlinefnc.h"
#include "tchtool.h"
#include "Kw_tool.h"
#include "KDebug.h"
#include "StrBuffer.h"

#include <memory>
#include <sys/timeb.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



typedef struct _REG_TZI_FORMAT
{
	LONG Bias;
	LONG StandardBias;
	LONG DaylightBias;
	SYSTEMTIME StandardDate;
	SYSTEMTIME DaylightDate;
} REG_TZI_FORMAT;

#define REG_TIME_ZONES L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\"
// #define REG_TIME_ZONES_LEN (sizeof(REG_TIME_ZONES)-1)
// #define REG_TZ_KEY_MAXLEN (REG_TIME_ZONES_LEN + (sizeof(((TIME_ZONE_INFORMATION*)0)->StandardName)/2) -1)
// L"Atlantic Standard Time" StandardName
// "GMT Standard Time" "China Standard Time", "Korea Standard Time"
int CLocalTime::GetTimeZoneInformationByName(TIME_ZONE_INFORMATION* ptzi, PWS StandardName)
{
	int rc;
	HKEY hkey_tz;
	DWORD dw;
	REG_TZI_FORMAT regtzi;
	CString tzsubkey(REG_TIME_ZONES);
	tzsubkey += StandardName;

	if(ERROR_SUCCESS != (dw = RegOpenKey(HKEY_LOCAL_MACHINE, tzsubkey, &hkey_tz)))
	{
		KTrace(L"failed to open: HKEY_LOCAL_MACHINE\\%s\n", tzsubkey);
		//pWin32Error(dw, L"RegOpenKey() failed");
		return -1;
	}
	rc = 0;
#define X(param, type, var) \
	do if ((dw = sizeof(var)), (ERROR_SUCCESS != (dw = RegGetValueW(hkey_tz, NULL, param, type, NULL, &var, &dw)))) { \
	KTrace(L"failed to read: HKEY_LOCAL_MACHINE\\%s\\%S\n", tzsubkey, param); \
	rc = -1; \
	goto ennd; \
	} while(0)
	X(L"TZI", RRF_RT_REG_BINARY, regtzi);
	X(L"Std", RRF_RT_REG_SZ, ptzi->StandardName);
	X(L"Dlt", RRF_RT_REG_SZ, ptzi->DaylightName);
#undef X
	ptzi->Bias = regtzi.Bias;
	ptzi->DaylightBias = regtzi.DaylightBias;
	ptzi->DaylightDate = regtzi.DaylightDate;
	ptzi->StandardBias = regtzi.StandardBias;
	ptzi->StandardDate = regtzi.StandardDate;
ennd:
	RegCloseKey(hkey_tz);
	return rc;
}
#ifdef _result_data
- _TIME_ZONE_INFORMATION "GMT Standard Time"
Bias	0	long
+ StandardName	0x002bf9f4 "GMT 표준시"	wchar_t[32]
+ StandardDate{ wYear = 0 wMonth = 10 wDayOfWeek = 0 ... }	_SYSTEMTIME
StandardBias	0	long
+ DaylightName	0x002bfa48 "GMT 일광 절약 시간"	wchar_t[32]
+ DaylightDate{ wYear = 0 wMonth = 3 wDayOfWeek = 0 ... }	_SYSTEMTIME
DaylightBias - 60	long

- _TIME_ZONE_INFORMATION "Atlantic Standard Time"
Bias	240	long
+ StandardName	0x01f8e6fc "대서양 표준시"
+ StandardDate{ wYear = 0 wMonth = 11 wDayOfWeek = 0 ... }	_SYSTEMTIME
StandardBias	0	long
+ DaylightName	0x01f8e750 "대서양 일광 절약 시간"
+ DaylightDate{ wYear = 0 wMonth = 3 wDayOfWeek = 0 ... }	_SYSTEMTIME
DaylightBias - 60	long

- _TIME_ZONE_INFORMATION "Korea Standard Time"
Bias - 540	long
+ StandardName	0x01f8e64c "대한민국 표준시"
+ StandardDate{ wYear = 0 wMonth = 0 wDayOfWeek = 0 ... }	_SYSTEMTIME
StandardBias	0	long
+ DaylightName	0x01f8e6a0 "대한민국 일광 절약 시간"
+ DaylightDate{ wYear = 0 wMonth = 0 wDayOfWeek = 0 ... }	_SYSTEMTIME
DaylightBias - 60	long

- _TIME_ZONE_INFORMATION "China Standard Time"
Bias - 480	long
+ StandardName	0x002bf86c "중국 표준시"	wchar_t[32]
+ StandardDate{ wYear = 0 wMonth = 0 wDayOfWeek = 0 ... }	_SYSTEMTIME
StandardBias	0	long
+ DaylightName	0x002bf8c0 "중국 일광 절약 시간"	wchar_t[32]
+ DaylightDate{ wYear = 0 wMonth = 0 wDayOfWeek = 0 ... }	_SYSTEMTIME
DaylightBias - 60	long

#endif // _result_data

TIME_ZONE_INFORMATION * CLocalTime::s_pTZ = NULL;//?ubjObj

TimeT CLocalTime::UtcToLocal(TimeT t, TIME_ZONE_INFORMATION* ptz)
{
	if(t == 0)
		return 0;
	SYSTEMTIME syt = { 0, };
	KwCTimeToSystime(CTime(t), &syt);
	return UtcToLocal(syt, ptz);
}
TimeT CLocalTime::LocalToUtc(TimeT t, TIME_ZONE_INFORMATION* ptz)
{
	if(t == 0)
		return 0;
	SYSTEMTIME syt = { 0, };
	KwCTimeToSystime(CTime(t), &syt);
	return LocalToUtc(syt, ptz);
}
TimeT CLocalTime::LocalToUtc(SYSTEMTIME& stm, TIME_ZONE_INFORMATION* ptz)
{
	SYSTEMTIME sytu = { 0, };
	TIME_ZONE_INFORMATION tz;
	LONG BiasInMinutes = 0;
	if(ptz == NULL)
	{
		int bd1 = GetTimeZoneInformation(&tz);
		ptz = &tz;
		switch(bd1)
		{
		case TIME_ZONE_ID_UNKNOWN: case TIME_ZONE_ID_STANDARD:
			BiasInMinutes = tz.StandardBias;
			break;
		case TIME_ZONE_ID_DAYLIGHT:
			BiasInMinutes = tz.DaylightBias;
			break;
		}
	}
	else
		BiasInMinutes = ptz->DaylightBias;

	BOOL b = TzSpecificLocalTimeToSystemTime(ptz, &stm, &sytu);
	CTime t(sytu.wYear, sytu.wMonth, sytu.wDay, sytu.wHour, sytu.wMinute, sytu.wSecond);
	t += CTimeSpan(0, 0, -BiasInMinutes, 0);
	//sytu.wMinute -= BiasInMinutes;
	return t.GetTime();//KwSystimeToCtime(sytu);
}
TimeT CLocalTime::UtcToLocal(SYSTEMTIME& stm, TIME_ZONE_INFORMATION* ptz)
{
	SYSTEMTIME sytl = { 0, };
	TIME_ZONE_INFORMATION tz = { 0, };

	LONG BiasInMinutes = 0;
	if(ptz == NULL)
	{
		int bd1 = GetTimeZoneInformation(&tz);
		ptz = &tz;
		switch(bd1)
		{
		case TIME_ZONE_ID_UNKNOWN:
			// There is no differentiation between standard and
			// daylight savings time.  Proceed as for Standard Time
		case TIME_ZONE_ID_STANDARD:
			BiasInMinutes = ptz->StandardBias;
			break;
		case TIME_ZONE_ID_DAYLIGHT:
			BiasInMinutes = ptz->DaylightBias;
			break;
		default:
			// Something is wrong with the time zone information.  Fail the logon request.
			//NtStatus = STATUS_INVALID_LOGON_HOURS;
			break;
		}
	}
	else
		BiasInMinutes = ptz->DaylightBias;
	BOOL b = SystemTimeToTzSpecificLocalTime(ptz, &stm, &sytl);
	//sytl.wMinute += BiasInMinutes;

#ifdef _DEBUGx
	DYNAMIC_TIME_ZONE_INFORMATION dtz;
	BOOL bd2 = GetTimeZoneInformationForYear(2014, &dtz, &tz);
	DWORD err = !bd2 ? GetLastError() : 0; // err:(2) ERROR_FILE_NOT_FOUND
	SYSTEMTIME syst, systl, systl2, systl3;
	GetSystemTime(&syst);
	GetLocalTime(&systl);
	BOOL b1 = SystemTimeToTzSpecificLocalTime(NULL, &syst, &systl2);
	BOOL b2 = SystemTimeToTzSpecificLocalTime(&tz, &syst, &systl3); //위랑 같다.
	DWORD err2 = !bd2 ? GetLastError() : 0; // err:(2) ERROR_FILE_NOT_FOUND
	//BOOL bd = SystemTimeToTzSpecificLocalTimeEx(&dtz,&syst, &systl);
#endif // _DEBUG

	CTime t(KwSystimeToCtime(sytl));
	t += CTimeSpan(0, 0, BiasInMinutes, 0);
	return t.GetTime();
}


CTime CLocalTime::GetNow_(ETmArea eOp /*= eCtUTC*/)
{
	//앱이 통용 되는 현지 시간으로 맞춘다. 뉴욕이면 -13시간
	CTime tNow = KwGetCurrentTime(eOp) + CTimeSpan(0, m_hrLocalDiff, 0, 0);//eCtUTC);//f0464TimeCoord 에 사용
	return tNow;
}

CString CLocalTime::GetNowStr(ETmArea eOp /*= eCtUTC*/)
{
	CString& sbuf = CStrBuffer::GlobalBuf();
	CTime cTime = GetNow_(eOp);
	KwCTimeToString(cTime, sbuf);
	return sbuf;
}

CString CLocalTime::GetNowDStr(ETmArea eOp /*= eCtUTC*/)
{
	CString& sbuf = CStrBuffer::GlobalBuf();
	CTime cTime = GetNow_(eOp);
	sbuf.Format(_T("%04d-%02d-%02d"), cTime.GetYear(), cTime.GetMonth(), cTime.GetDay());
	return sbuf;
}

TimeT CKwTime::GetMidnight(TimeT ltime /*= 0*/)
{
	if(ltime == 0)
		ltime = m_ltime;
	TimeT clock = ltime % (3600 * 24);
	TimeT midnite = ltime - clock;
#ifdef _DEBUG
	errno_t err;
	CTm today0;//= { 0, 0, 0, 1, 1, 100 };//2010
	err = _localtime64_s(&today0, &midnite);
	// 		assert(today0.tm_sec == 0);
	// 		assert(today0.tm_min == 0);
	// 		assert(today0.tm_hour == 0);
#endif // _DEBUG
	return midnite;
}

CTm CKwTime::ToUTC(TimeT ltime /*= 0*/)
{
	if(ltime == 0)
		ltime = m_ltime;
	errno_t err = 0;
	CTm gmt0;//= { 0, 0, 0, 1, 1, 100 };//2010
	err = _gmtime64_s(&gmt0, &ltime);
	// 		char timebuf[30];
	// 		err = asctime_s(timebuf, 30, &gmt);	//Fri Apr 25 20:51:23 2003
	// 		st = TStrObj(tmpbuf);
	gmt0.tm_year += 1900;
	gmt0.tm_mon += 1;
	return gmt0;
}

CTm CKwTime::ToLocal(TimeT ltime /*= 0*/)
{
	if(ltime == 0)
		ltime = m_ltime;
	errno_t err = 0;
	CTm today0;// = { 0, 0, 0, 1, 1, 100 };//2010
	err = _localtime64_s(&today0, &ltime);
	// 	today0.tm_year += 1900;
	// 	today0.tm_mon += 1;
	return today0;
}

void CKwTime::To12Hour()
{
	char ampm[] = "AM";
	errno_t err = 0;
	tm today = ToLocal();
	if(today.tm_hour >= 12)
	{
		strcpy_s(ampm, sizeof(ampm), "PM");
		today.tm_hour -= 12;
	}
	if(today.tm_hour == 0)  // Adjust if midnight hour.
		today.tm_hour = 12;

	// Convert today into an ASCII string 
	//err = asctime_s(timebuf, 26, &today);
	if(err == 0)
	{
		// Note how pointer addition is used to skip the first 11 
		// characters and printf is used to trim off terminating 
		// characters.
		//
		//printf( "12-hour time:\t\t\t\t%.8s %s\n", timebuf + 11, ampm );
	}
}

int CKwTime::TimeDifference()
{
	struct _timeb tstruct = { 0 };
	_ftime_s(&tstruct);
	return (int)(tstruct.timezone / 60);
}

bool CKwTime::IsDaylightSaving()
{
	struct _timeb tstruct = { 0 };
	_ftime_s(&tstruct);
	return tstruct.dstflag ? true : false;
}

char* CKwTime::GetTimeZone()
{
	//error C4996: '__tzname': This function or variable may be unsafe. 
	//	Consider using _get_tzname instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
	//return _tzname[0];
	size_t rv;
	//char buf[512];
	CStringA& sbuf = CStrBufferA::GlobalBuf();
	char* buf = sbuf.GetBuffer(512);
	errno_t er = _get_tzname(&rv, buf, 500, 0);
	sbuf.ReleaseBuffer();
	return (char*)(LPCSTR)sbuf;
}

CStringA CKwTime::TimeStrA(TimeT ltime /*= 0*/)
{
	CStringA sbuf;
	char* tmpbuf = sbuf.GetBuffer(100);
	CTm tl = ToLocal(ltime);
	strftime(tmpbuf, 20, "%Y-%m-%d %H:%M:%S", &tl);
	return tmpbuf;
}

CStringW CKwTime::TimeStr(TimeT ltime /*= 0*/)
{
	CStringW sbuf;
	wchar_t* tmpbuf = sbuf.GetBuffer(100);
	CTm tl = ToLocal(ltime);
#ifdef _IPHONE

#else
	wcsftime(tmpbuf, 20, L"%Y-%m-%d %H:%M:%S", &tl);
#endif // _IPHONE
	return tmpbuf;
}

TimeT CKwTime::GetLTime()
{
	return CLocalTime::UtcToLocal(m_ltime, CLocalTime::s_pTZ);
}


// 사용자가 입력 하여 오류 있을지 모르는 문자열 분석
// 15 분, 0.5 시간, 2주, ..
bool KwParseTimeDurationStr1(LPCWSTR psst, CTimeSpan& sp)
{
	CString st, su;//(psEngTime);
	int len = lstrlen(psst);

	int i=0;
	for(;i<len;i++)
	{
		TCHAR ch = psst[i];
		if(KwIsDigit(ch) || ch == '.')
			st += (TCHAR)ch;
		else
			break;
	}
	for(;i<len;i++)
	{
		TCHAR ch = psst[i];
		if(KwIsSpace(ch))		continue;
		else			break;
	}
	for(;i<len;i++)
	{
		TCHAR ch = psst[i];
		if(KwIsSpace(ch))		break;
		else
			su += (TCHAR)ch;
	}

	double sec = _tstof(st);
	//_ttoi(st);
	su.MakeLower();

	if(su == _T("분") || su.Find(_T("minute")) == 0) sec *= 60;
	else if(su == _T("시간") || su.Find(_T("hour")) == 0) sec *= 60*60;
	else if(su == _T("일") || su.Find(_T("day")) == 0) sec *= 60*60*24;
	else if(su == _T("주") || su.Find(_T("week")) == 0) sec *= 60*60*24*7;
	else if(su == _T("초") || su.Find(_T("sec")) == 0) sec *= 1;
	else
		return false;

	sp = CTimeSpan(0, 0, 0, (int)sec);
	return true;
}


__time64_t KwSystimeToCtime(SYSTEMTIME& st)
{
	if(!(
		(st.wYear >= 1900) &&
		(st.wMonth >= 1 && st.wMonth <= 12) &&
		(st.wDay >= 1 && st.wDay <= 31) &&
		(st.wHour >= 0 && st.wHour <= 23) &&
		(st.wMinute >= 0 && st.wMinute <= 59) &&
		(st.wSecond >= 0 && st.wSecond <= 59)))
		return 0;
	CTime t(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return t.GetTime();
}

// 오류 없는 웬만한 모양의 시간 문자열 분석
//"Mon Dec  6 16:24:51 2004" //Ddd Mmm Date hh:mm:ss yyyy,
// 표준시각 //"Sat, 30 Dec 2006 17:48:49 +0900"
// "Tuesday, 6 Nov 2007"	from MSDN
__time64_t KwParseTimeStr4(LPCWSTR psEngTime, SYSTEMTIME* pSyt, bool bDateOnly)
{
	SYSTEMTIME syt ={0,};
	if(pSyt == NULL)
		pSyt = &syt;
	KwParseTimeStr5(psEngTime, pSyt, bDateOnly);
	return KwSystimeToCtime(*pSyt);
}

DATE KwParseTimeStr6(LPCWSTR psEngTime, SYSTEMTIME* pSyt, bool bDateOnly)
{
	SYSTEMTIME syt ={0,};
	if(pSyt == NULL)
		pSyt = &syt;
	KwParseTimeStr5(psEngTime, pSyt, bDateOnly);
	COleDateTime odt(*pSyt);
	return (DATE)odt;
}

SYSTEMTIME* KwParseTimeStr5(LPCWSTR psEngTime, SYSTEMTIME* pSyt, bool bDateOnly)
{
	return KwParseTimeStr10(psEngTime, pSyt, bDateOnly);
}

LPCWSTR KwCTimeToString(CTime t, CString& sTime, bool bSpace, TCHAR cSpDay, TCHAR cSpTime)
{
	return KwTimeToString(sTime, bSpace, cSpDay, cSpTime, t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());
}
LPCWSTR KwCTimeToDateString(CTime t, CString& sTime, bool bSpace, TCHAR cSpDay, TCHAR cSpTime)
{
	return KwTimeToString(sTime, bSpace, cSpDay, cSpTime, t.GetYear(), t.GetMonth(), t.GetDay(), -1, -1, -1);// t.GetHour(), t.GetMinute(), t.GetSecond());
}
LPCWSTR KwTimeToString(CString& sTime, bool bSpace, TCHAR cSpDay, TCHAR cSpTime, int y, int m, int d, int hr, int mn, int sc)
{
	if (hr >= 0 && mn >= 0 && sc >= 0)
	{
		if (bSpace)
			sTime.Format(_T("%04d%c%02d%c%02d %02d%c%02d%c%02d"), y, cSpDay, m, cSpDay, d, hr, cSpTime, mn, cSpTime, sc);
		else
			sTime.Format(_T("%04d%02d%02d%02d%02d%02d"), y, m, d, hr, mn, sc);
	}
	else
	{
		if (bSpace)
			sTime.Format(_T("%04d%c%02d%c%02d"), y, cSpDay, m, cSpDay, d);
		else
			sTime.Format(_T("%04d%02d%02d"), y, m, d);
	}
	return sTime;
}
/*
CStringW KwParseTimeStrToStr(LPCWSTR pst)
{
	CStringW sbuf;
	CTime t = KwParseTimeStr4(pst);
	KwCTimeToString(t, sbuf);
	return sbuf;
#ifdef _DEBUGx
	CString start_time = FMTS(L"'%s'", KwParseTimeStrToStr(L"20161102153547068"));
	CString start_tim2 = FMTS(L"'%s'", KwParseTimeStrToStr(L"20161102153547"));
#endif // _DEBUG
}
*/

// 표준시각 //"Sat, 30 Dec 2006 17:48:49 +0900"
// 시차 까지 계산 하여 __time64_t 로 리턴
__time64_t KwStdTimeToTime(LPCWSTR psEngTime)// for CTime
{
	SYSTEMTIME syt;
	__time64_t tdff = KwParseTimeStr4(psEngTime, &syt);
	CTime tin = KwSystimeToCtime(syt);
	return tin.GetTime();// + tdff; 시차를 빼지 않는다. GMT 시간 그대로 쓴다.
}




SYSTEMTIME* KwParseTimeStr10(LPCWSTR psEngTime, SYSTEMTIME* pSyt, bool bDateOnly)
{
	ASSERT(psEngTime != NULL);
	// caller가 알아서 좋은 변수 보내도록 조치.

	SYSTEMTIME syt ={0,0,0,0,0,0,0,0};
	//if(!KwIsAlpha(psEngTime[0]) && IsDateHan(psEngTime[0]) < 0)

	int pm = 0; // "오후" 또는 "PM" 이면 12가 된다.
	bool bAmPm = false;
	if(KwIsDigit(psEngTime[0]))
	{
		// 		if(psEngTime[10] == 'T')
		// 			KwParseTimeStr5(psEngTime, &syt);
		// //		<dc:date>2007-07-04T11:54:22+09:00</dc:date> 
		// 		else
		KwCStringToTime10(psEngTime, &syt, bDateOnly); // 숫자로만 된것은 맨 처음이 숫자 이다. 순서는 책임 못진다.
	}
	else
	{
		CStringArray ar;
		KwCutByToken(psEngTime, L", ", ar);

		__time64_t tsp = 0; // 시차
		syt.wMilliseconds = 0;
		for_each0(ar.GetCount())
		{
			CString& ps = ar[_i];
			if(IsDateHan(ps[0]) >= 0 || IsDateEng(ps) >= 0)
			{	// 한글은 요일로 간주
			}
			else if(KwIsPmHour(ps, pm)) //pm 은 오후 12:?? 인 경우 를 제외 하고 12를 더하면 된다.
			{
				//pm이 이미 바뀜
				bAmPm = true; // 오전/오후,am,pm 등의 글씨가 있었다.
			}
			else if(KwIsAlpha(ps[0]))
			{
				int iMon = KwMonthFromStr10(ps);
				if(iMon >= 1)// year
				{
					syt.wMonth = iMon;
				}
				// else : it's date
			}
			else if(ps == _T("GMT") || ps[0] == '+' || ps[0] == '-') //시차
			{
				//CString& sdf = ar[_i];
				//tsp = KwParseTimeDiff(ps);
			}
			else if(ps.GetLength() > 2 && ps[2] == ':' || ps[1] == ':') // hh:mm:ss
			{
				CStringArray art;
				KwCutByToken(ps, _T(":"), art);

				syt.wHour = _ttoi(art[0]);

				if(bAmPm)
				{
					if(pm == 0) // am
					{
						if(syt.wHour == 12) //오전 12: 는 0: 이다.
							syt.wHour = 0;
					}
					else // pm
					{
						ASSERT(1 <= syt.wHour && syt.wHour <= 12); //오전 12: 는 0: 이다.
						if(syt.wHour != 12) //오후는 12:00 제외한 나머지는 +12를 해야 한다.
							syt.wHour += pm;
					}
				}
				syt.wMinute = _ttoi(art[1]);

				if(art.GetCount() > 2)
					syt.wSecond = _ttoi(art[2]);
			}
			else
			{
				int n = _ttoi(ps);
				if(1900 <= n && n <= 3000)	// year
					syt.wYear = n;
				else
				{
					if(syt.wDay == 0) //날짜 가 먼저 오는것으로 간주
					{
						ASSERT(1 <= n && n <= 31);
						syt.wDay = n;
					}
					else // month
					{
						ASSERT(1 <= n && n <= 12);	// month
						ASSERT(syt.wMonth == 0);
						syt.wMonth = n;
					}
				}
			}
		}
	}
	// 	// 오후 인경우 12 더한다.
	// 	if(bAmPm)
	// 	{
	// 		if(syt.wHour != 12)// 오후 12:00 가 24:00 로 되는걸 방지 
	// 			syt.wHour += pm;
	// 	}
	// 	else 
	// 	{
	// // 		if(syt.wHour == 12)// 오후 12:00 가 24:00 로 되는걸 방지 
	// // 			syt.wHour = 0;
	// 	}

	if(bDateOnly)
	{
		syt.wHour = syt.wMinute = syt.wSecond = 0;
	}

	if(pSyt)
	{
		if(syt.wYear == 0 && syt.wMonth == 0 && syt.wDay == 0)
		{
			// 시간만 있는 경우, 시간만 복사 한다. 리턴값은 1980/1/1 로 리턴 한다.
			// 가져온 pSyt 의 날짜 부분을 손상 하지 않는다.
			pSyt->wHour   = syt.wHour;
			pSyt->wMinute = syt.wMinute;
			pSyt->wSecond = syt.wSecond;

			if(pSyt->wYear == 0 && pSyt->wMonth == 0 && pSyt->wDay == 0)
			{
				// 가져온것, 만든것 둘다 날짜가 없으면 default 1/1/1980 로 한다.
				syt.wYear = NULLYEAR;
				syt.wMonth = syt.wDay = 1;
				return pSyt;
				//return KwSystimeToCtime(syt);//.GetTime();
			}
			else
				return pSyt;
			//return KwSystimeToCtime(*pSyt);//.GetTime();
		}
		else
			*pSyt = syt;
	}
	//The upper date limit is 12/31/3000. The lower limit is 1/1/1970 12:00:00 AM GMT.
	if(syt.wYear == 0 && syt.wMonth == 0 && syt.wDay == 0)
	{
		syt.wYear = NULLYEAR;
		syt.wMonth = syt.wDay = 1;
	}
	// 	DATE dt;
	// 	VERIFY(SystemTimeToVariantTime(&st, &dt));
	// 	return dt;
	//	return KwSystimeToCtime(syt);//.GetTime();
	return pSyt;
}



// 결국 문자열 -> CTime 은 모두 여기를 통과 하게 된다.
// CTime 을 쓰지 않기 때문에 1900- 이 초기값이다.
LPSYSTEMTIME KwCStringToTime10(LPCWSTR strData, LPSYSTEMTIME pSt, bool bDateOnly)
{
	static LPCWSTR _seps = _T("- :/.");
	int lsep = 5;
	WCHAR seps[10] ={'\0',};
	//KwZeroMemory(seps);
	memset(seps, 0, 10);
	wcscpy_s(seps, _seps);
	WCHAR* tok=NULL;
	LPCWSTR par[10] ={0,};
	int warr[8]={NULLYEAR,NULLMONTH,NULLDAY,0,0,0,0,0}; // static 이면 이전 값이 그래로 있잖아.

	int nAm = 0;// 'PM' 인경우 12 로 바뀐다.

	int len = (int)wcslen(strData);
	ASSERT(len > 3);
	bool bAllDigit = true;
	for_each0(len)
	{
		WCHAR c = strData[_i];
		if(!KwIsDigit(c))
		{
			bAllDigit = false;
			break;
		}
	}

	if(bAllDigit)
	{
		if(len == 4)
		{
			//_stscanf
			swscanf_s(strData, _T("%04d"), warr);
		}
		else if(len == 6)
		{
			swscanf_s(strData, _T("%02d%02d%02d"), warr, warr+1, warr+2);
		}
		else if(len == 8)
			swscanf_s(strData, _T("%04d%02d%02d"), warr, warr+1, warr+2);
		else
		{
			if(len == 12)
				swscanf_s(strData, _T("%04d%02d%02d%02d%02d"), warr, warr+1, warr+2, warr+3, warr+4);
			else if(len == 14)
				swscanf_s(strData, _T("%04d%02d%02d%02d%02d%02d"), warr, warr+1, warr+2, warr+3, warr+4, warr+5);
			else if(len == 17)
				swscanf_s(strData, _T("%04d%02d%02d%02d%02d%02d%03d"), warr, warr+1, warr+2, warr+3, warr+4, warr+5, warr+6);
		}
	}
	else
	{
		//"8/9/2006 8:00 AM" 의 경우는?
		if(len > 0)//strData)
		{
			//			CString sSeps = seps;
			//		<dc:date>2007-07-04T11:54:22+09:00</dc:date> 
			if(strData[10] == 'T')
			{
				seps[lsep] = 'T';	lsep++;
				seps[lsep] = '\0';
				//				sSeps += 'T';
				// 				_stscanf(strData, _T("%04d-%02d-%02dT%02d%02d%02d"), 
				// 					warr, warr+1, warr+2, warr+3, warr+4, warr+5);		
			}

			//CAutoBufWrapperW ptr(tchlen(strData)+1);
			auto htr = std::shared_ptr<WCHAR>(new WCHAR[tchlen(strData)+1]);
			auto ptr = htr.get();
			tchcpy(ptr, (LPCWSTR)strData);
			WCHAR* next_token1 = NULL;

			tok = wcstok_s(ptr, seps, &next_token1);
			int nitem = 0;
			for(int i=0; tok != NULL && i<8;i++)
			{
				par[i] = tok;
				tok = wcstok_s(NULL, seps, &next_token1);
				nitem++;
			}

			if(tchlen(par[2]) == 4)
			{// ("8/19/2006 8:00 AM"); // 년도가 3번째 오는 경우 월/일/년 => 년/월/일 
				LPCWSTR y = par[2];
				LPCWSTR m = par[0];
				LPCWSTR d = par[1];
				par[0] = y;	// 2006, 8, 19
				par[1] = m;
				par[2] = d;

			}

			if((par[5] && tchicmp(_T("PM"), par[5])) || (par[6] && tchicmp(_T("PM"), par[6])))
			{
				nAm = 12;
			}
			for_each0(nitem)
			{
				if(KwIsDigit(par[_i][0]))
					warr[_i] = _wtoi(par[_i]);
#ifdef _DEBUG
				else
					ASSERT(nAm == 12);
#endif // _DEBUG
			}
		}
	}


	// 	if(warr[2] > 1000)
	// 	{
	// 		int y = warr[2];
	// 		int m = warr[0];
	// 		int d = warr[1];
	// 		warr[0] = y;
	// 		warr[1] = m;
	// 		warr[2] = d;
	// 	}

	if(0 <= warr[0] && warr[0] < 30)
		warr[0] += 2000;
	else if(80 <= warr[0] && warr[0] < 100)
		warr[0] += 1900;

	ASSERT(warr[0] >= 0);
	pSt->wYear		= warr[0];
	pSt->wMonth		= warr[1];
	pSt->wDay		= warr[2];
	pSt->wHour		= bDateOnly ? 0 : warr[3];
	pSt->wMinute	= bDateOnly ? 0 : warr[4];
	pSt->wSecond	= bDateOnly ? 0 : warr[5];
	pSt->wMilliseconds = bDateOnly ? 0 : warr[6];
	return pSt;
}

int _KMonthCmp0(const void* p1, const void* p2, BOOL bCaseSensitive)
{
	_EnMon* pe1 = (_EnMon*)p1;
	_EnMon* pe2 = (_EnMon*)p2;
	return KwTextCmp(pe1->smon, pe2->smon, eCmprNone, 3);
}

int KwMonthFromStr10(LPCWSTR sMonth)
{
	static _EnMon s_sm[] =
	{
		{ L"Apr",4,},
		{ L"Aug",8,},
		{ L"Dec",12,},
		{ L"Feb",2,},
		{ L"Jan",1,},
		{ L"Jul",7,},
		{ L"Jun",6,},
		{ L"Mar",3,},
		{ L"May",5,},
		{ L"Nov",11,},
		{ L"Oct",10,},
		{ L"Sep",9,},
	};

	_EnMon mon ={sMonth, -1};

	_EnMon* pp = (_EnMon*)KwBsearch((void*)&mon, s_sm, 12, sizeof(_EnMon), (int(*)(const void*, const void*, BOOL))_KMonthCmp0, false);

	if(pp)
		return pp->mon;
	else
	{
		return -1;
	}
}


CString KwGetCurrentDateString(int bUTC)
{
	CString sbuf;
	CTime cTime = KwGetCurrentTime(bUTC);
	sbuf.Format(_T("%04d-%02d-%02d"), cTime.GetYear(), cTime.GetMonth(), cTime.GetDay());
	return sbuf;
}

CString KwGetToday(int bUTC)
{
	CString sbuf;
	CTime today = KwGetCurrentTime(bUTC);
	sbuf.Format(_T("%04d-%02d-%02d 00:00:00"), today.GetYear(), today.GetMonth(), today.GetDay());
	return sbuf;
}

CKTime KwGetTodayTime(int bUTC)
{
	CString sbuf;
	CKTime now = KwGetCurrentTime(bUTC);
	CKTime today(now.GetYear(), now.GetMonth(), now.GetDay(), 0, 0, 0);
	return today;
}


CString KwGetCurrentTimeString(int bUTC)
{
	CString sbuf;
	CTime cTime = KwGetCurrentTime(bUTC);
	sbuf.Format(_T("%02d:%02d"), cTime.GetHour(), cTime.GetMinute());
	return sbuf;
}

CString KwGetCurrentTimeFullString(int bUTC)
{
	CString sbuf;
	CTime cTime = KwGetCurrentTime(bUTC);
	KwCTimeToString(cTime, sbuf);

	// 	sbuf.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"),
	// 		cTime.GetYear(),cTime.GetMonth(),cTime.GetDay(),
	// 		cTime.GetHour(),cTime.GetMinute(),cTime.GetSecond());
	return sbuf;//CString(buf);
}


CTime KwVarDateToCTime(DATE dt)
{
	SYSTEMTIME st ={0,};
	VariantTimeToSystemTime(dt, &st);
	CTime t(st);
	return t;
}

//see also SetTimeZoneInformation
CTime KwGetCurrentTime(int bUTC)
{
	COleDateTime dt = KwGetCurrentTimeV(bUTC);
	return KwVarDateToCTime(dt);
}
COleDateTime KwGetCurrentTimeV(int bUTC)
{
#ifdef _DEBUGx
	COTime to = COleDateTime::GetCurrentTime();
	CString sto = to.Format(_T("%Y-%m-%d(%a), %H:%M:%S"));
	CTime tn = CTime::GetCurrentTime();
	SYSTEMTIME sysTime;
	GetSystemTime(&sysTime);
#endif // _DEBUG
	SYSTEMTIME st2;
	if(bUTC)
		GetSystemTime(&st2);
	else
		GetLocalTime(&st2);//오직 이것만이 CE 에서도 현시각 리턴 한다.
	COleDateTime t(st2);//.wYear, st2.wMonth, st2.wDay, st2.wHour, st2.wMinute, st2.wSecond);
	return t;
	// -		st2	{wYear=2008 wMonth=5 wDayOfWeek=2 ...}	_SYSTEMTIME
	// 		wYear	2008	unsigned short
	// 		wMonth	5	unsigned short
	// 		wDayOfWeek	2	unsigned short
	// 		wDay	13	unsigned short
	// 		wHour	17	unsigned short
	// 		wMinute	39	unsigned short
	// 		wSecond	20	unsigned short
	// 		wMilliseconds	0	unsigned short
	// -		sysTime	{wYear=2008 wMonth=5 wDayOfWeek=2 ...}	_SYSTEMTIME
	// 		wYear	2008	unsigned short
	// 		wMonth	5	unsigned short
	// 		wDayOfWeek	2	unsigned short
	// 		wDay	13	unsigned short
	// 		wHour	8	unsigned short
	// 		wMinute	39	unsigned short
	// 		wSecond	19	unsigned short
	// 		wMilliseconds	0	unsigned short
	// -		tn	{m_s={...}}	CTime
	// +		[ATL::CTime]	{m_time=1210667958}	ATL::CTime
	// -		m_s	{0x003a7e80}	ATL::CStringT<wchar_t,StrTraitMFC<wchar_t,ATL::ChTraitsOS<wchar_t> > >
	// -		[ATL::CSimpleStringT<wchar_t,0>]	{0x003a7e80}	ATL::CSimpleStringT<wchar_t,0>
	// +		m_pszData	0x003a7e80 "2008-05-13 01:39(화)"	wchar_t*
	//현재 2008-05-13 오후 5:39

}

bool KwInitSysTime(SYSTEMTIME& st)
{
	if (st.wYear < NULLYEAR || st.wYear > 3000)
	{
		st.wYear = NULLYEAR;
		st.wMonth = NULLMONTH;
		st.wDay = NULLDAY;
		st.wHour = 0;
		st.wMinute = 0;
		st.wSecond = 0;
		st.wMilliseconds = 0;
		st.wDayOfWeek = 0;
		return false;
	}
	return true;
}

bool KwCheckTime(LPCTSTR strt)
{
	SYSTEMTIME st = { 0, };
	KwInitSysTime(st);
	KwCStringToTime(strt, &st); //단순히 문자열을 쪼개서 만든후
	return KwCheckTime(st);
}
// COleDateTime 에 넣었다 빼보면..
bool KwCheckTime(SYSTEMTIME& st)
{
	COleDateTime dt = st;
	SYSTEMTIME st1 = { 0, };
	KwInitSysTime(st1);
	try
	{
		dt.GetAsSystemTime(st1);
		if (st.wYear != st1.wYear ||
			st.wDay != st1.wDay ||
			st.wHour != st1.wHour ||
			st.wMinute != st1.wMinute ||
			st.wSecond != st1.wSecond ||
			st.wMilliseconds != st1.wMilliseconds)
			//|| st.wDayOfWeek != st1.wDayOfWeek)//단순히 문자열을 쪼개서는 안나오는 정보
			throw false;
	}
	catch (...)
	{
		return false;
	}

	return true;
}

// CTime 때문에 NULL => 1980-1-1 0:0:0 이 된다.
CTime KwCStringToCTime(LPCTSTR strData)
{
	CTime t;
	KwCStringToCTime(strData, t);
	return t;
}
__time64_t KwCStringToCTime(LPCTSTR strData, CTime& cTime)
{
	SYSTEMTIME st = { 0, };
	KwInitSysTime(st);//KwCStringToSysTime 에서 한값을 덮어 오므로 할필요 없는것 같다.

	KwCStringToSysTime(strData, &st);

	//	KwInitSysTime(st);
	CTime cTime0(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	cTime = cTime0;//담아서 보내야지
	ASSERT(cTime.GetYear() >= NULLYEAR);
	ASSERT(cTime.GetTime() >= 0);
	return cTime.GetTime();
}

LPSYSTEMTIME KwCStringToSysTime(LPCTSTR strData, LPSYSTEMTIME pSt)
{
	SYSTEMTIME st1 = { 0, };
	KwInitSysTime(st1);

	KwCStringToTime(strData, &st1);

	WORD wMilliseconds = st1.wMilliseconds;
	COleDateTime dt(st1.wYear, st1.wMonth, st1.wDay, st1.wHour, st1.wMinute, st1.wSecond);
	if (dt.m_status == COleDateTime::invalid)
	{
		CTime dt0(st1.wYear, st1.wMonth, st1.wDay, st1.wHour, st1.wMinute, st1.wSecond);
		SYSTEMTIME st0 = { (WORD)dt0.GetYear(), (WORD)dt0.GetMonth(), (WORD)dt0.GetDay(), (WORD)dt0.GetHour(), (WORD)dt0.GetMinute(), (WORD)dt0.GetSecond(), };
		*pSt = st0;
	}
	else
	{
#ifdef _DEBUGx
		CTime dt0(st1.wYear, st1.wMonth, st1.wDay, st1.wHour, st1.wMinute, st1.wSecond);
		CString s1; s1.Format(_T("%04d%02d%02d%02d%02d%02d"), dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(), dt.GetSecond());
		CString s0; s0.Format(_T("%04d%02d%02d%02d%02d%02d"), dt0.GetYear(), dt0.GetMonth(), dt0.GetDay(), dt0.GetHour(), dt0.GetMinute(), dt0.GetSecond());
#endif // _DEBUG
		try
		{
			dt.GetAsSystemTime(*pSt);// 여기서 시각이 잘못되었다면.. pSt의 값은 바뀐다.
		}
		catch (...)
		{
			pSt->wYear = 0;
			KwInitSysTime(*pSt);
			return pSt;
		}
	}
	pSt->wMilliseconds = wMilliseconds;//warr[6];
	return pSt;
}


CTime KwNextHour(const CTime cTime)
{
	CTimeSpan oneHour(0, 1, 0, 0);
	CTime tmp(cTime);
	tmp += oneHour;
	return tmp;
}
// 시간은 그대로 날짜만 이동
/*CString KwNextDayStr(const CTime cTime, int nday)
{
	CTime t = KwNextDay(cTime, nday);
	CString ts;
	KwCTimeToString(t, ts);
	return ts;
}*/
CTime KwNextDay(const CTime cTime, int nday)
{
	COleDateTime cTimeO(cTime.GetTime());
	DATE dt = KwNextDay(cTimeO, nday);
	return KwVarDateToCTime(dt);
}
COleDateTime KwNextDay(const COleDateTime cTime, int nday)
{
	COleDateTimeSpan oneDay(nday, 0, 0, 0);
	COleDateTime tmp(cTime);
	tmp += oneDay;

	if (cTime.GetHour() != tmp.GetHour())
	{
		ASSERT(nday != 0);
		int h1 = cTime.GetHour();
		int h2 = tmp.GetHour();
		if ((h1 - h2) == 1 || (h1 == 0 && h2 == 23)) //summer time 시작 23시간이 하루
			tmp += COleDateTimeSpan(0, 1, 0, 0);
		else if ((h1 - h2) == -1 || (h1 == 23 && h2 == 0))//summer time 끝 25시간이 하루
			tmp += COleDateTimeSpan(0, -1, 0, 0);// 10 -> 11 or 23 -> 0
	}
	ASSERT(cTime.GetHour() == tmp.GetHour());
	return tmp;
}

CString CTm::ToString()
{
	CString sTime;
	PWS KwTimeToString(CString & sTime, bool bSpace, TCHAR cSpDay, TCHAR cSpTime, int y, int m, int d, int hr, int mn, int sc);
	KwTimeToString(sTime, true, '-', ':', Year(), Month(), Day(), Hour(),
		Min(), Sec());
	return sTime;
}

void CTm::StrToTm(const wchar_t* strT)
{
	memset(this, 0, sizeof(tm));

	SYSTEMTIME st;
	KwParseTimeStr10(strT, &st);
	//	KwCStringToTime10(strT, &st);
	KwSystimeToTm(st, this);

	//	swscanf(strT, L"%04d-%02d%-02d %02d:%02d:%02d", &tm_year, &tm_mon, &tm_mday, &tm_hour, &tm_min, &tm_sec);
		// 잘안된다. 뒤에 시간 부분은 모두 0으로 나온다.
	//	-		tm	{tm_sec=0 tm_min=0 tm_hour=0 ...}	tm
	// 		tm_sec	0	int
	// 		tm_min	0	int
	// 		tm_hour	0	int
	// 		tm_mday	0	int
	// 		tm_mon	9	int
	// 		tm_year	110	int
	// 		tm_wday	0	int
	// 		tm_yday	0	int
	// 		tm_isdst	0	int
	// 	tm_year -= 1900; todo
	// 	tm_mon -= 1;
}

void KwSystimeToTm(SYSTEMTIME& st, struct tm* ptm)
{
	ptm->tm_sec   = st.wSecond;
	ptm->tm_min   = st.wMinute;
	ptm->tm_hour  = st.wHour;
	ptm->tm_mday  = st.wDay;
	ptm->tm_mon   = st.wMonth - 1;
	ptm->tm_year  = st.wYear - 1900;
	ptm->tm_wday  = -1;
	ptm->tm_yday  = -1;
	ptm->tm_isdst = -1;
}


static LPCTSTR s_smd[] = { _T("Sun"), _T("Mon"), _T("Tue"), _T("Wed"), _T("Thu"), _T("Fri"), _T("Sat"), };
PWS KwDate(int iDayOfWeek, CStringW& sDate, bool bEn)
{
	// CKTime::GetDayOfWeek() 1(Sun), 2(Mon),,,,7(Sat) 이다.
	static UINT DayOfWeek[] =
	{
		LOCALE_SABBREVDAYNAME7,   // Sunday
		LOCALE_SABBREVDAYNAME1,
		LOCALE_SABBREVDAYNAME2,
		LOCALE_SABBREVDAYNAME3,
		LOCALE_SABBREVDAYNAME4,
		LOCALE_SABBREVDAYNAME5,
		LOCALE_SABBREVDAYNAME6,  // Saturday
		//			"일", // Sunday
		//			"월",
		//			"화",
		//			"수",
		//			"목", 
		//			"금", 
		//			"토", // Saturday
	};
	//static TCHAR strWeekday[256];
	//CKTime time(KwGetCurrentTime());   // Initialize CKTime with current time
	if(bEn)
	{
		sDate = s_smd[iDayOfWeek - 1];//?support 2015-12-09 17.56.43.png
	}
	else
	{
		LPTSTR pDate = (LPTSTR)sDate.GetBuffer(256);
		::GetLocaleInfo(LOCALE_USER_DEFAULT,   // Get string for day of the week from system
			DayOfWeek[iDayOfWeek - 1],   // Get day of week from CKTime
			pDate, sizeof(256));
		sDate.ReleaseBuffer();
	}
	//	ATLTRACE("%s\n", strWeekday);               // Print out day of the week
	return sDate;
}

int KwCompareDate(CKTime t1, CKTime t2)
{
	CKTime d1(t1.GetYear(), t1.GetMonth(), t1.GetDay(), 0, 0, 0);
	CKTime d2(t2.GetYear(), t2.GetMonth(), t2.GetDay(), 0, 0, 0);
	CKTimeSpan sp = d1 - d2;
	auto ll = sp.GetTotalSeconds();
	return ll == 0 ? 0 : ll > 0 ? 1 : -1;
}

/// 날짜 상관 없이 시간만 따질때
int KwCompareTimeOnly(CKTime t1, CKTime t2)
{
	CKTime d1(t1.GetYear(), t1.GetMonth(), t1.GetDay(), t1.GetHour(), t1.GetMinute(), t1.GetSecond());
	CKTime d2(t1.GetYear(), t1.GetMonth(), t1.GetDay(), t2.GetHour(), t2.GetMinute(), t2.GetSecond());
	CKTimeSpan sp = d1 - d2;
	auto ll = sp.GetTotalSeconds();
	return ll == 0 ? 0 : ll > 0 ? 1 : -1;
}
