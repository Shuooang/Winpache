#include "pch.h"
#include "ApiSite1.h"

#include "KwLib64/KRect.h"
#include "KwLib64/TimeTool.h"
#include "KwLib64/inlinefnc.h"
#include "KwLib64/KDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _APIEX

/// SearchBizByLocation
int ApiSite1::SearchBizByLocation(JObj& jpa, JObj& jrs, int iOp)
{
	JObj& hs = jpa;
	JObj& hsr = jrs;
	try
	{
		if(!jpa.Has("Option"))
			throw_response(eHttp_Bad_Request, L"Option is not found.");
		JObj jop(jpa.O("Option"));

		// 1. DB사용 초기화
		Rec(rec);
		int fAllStaff = 0;//합친다.
		CString action = jop.S("action");

		CString uuid = jpa.S("fUsrID");
		CString key = hs.S("key");

		jrs("Return") = L"OK";//default

		CString sql;


		CRectD rc;
		if(hs.Has("fl_lat"))
		{
			rc.Set(hs.N("fl_lon"), hs.N("fl_lat"), hs.N("fr_lon"), hs.N("nl_lat"));
		}
		else if(hs.Has("org_x")) // iOS MapKit
		{
			rc.SetRc(hs.N("org_x"), hs.N("org_y"), hs.N("sz_w"), -hs.N("sz_h"));
		}
		else
		{
			double lat = hs.N("lat");
			double lon = hs.N("lon");
			double w2 = 1; //절반
			double h2 = 1; //절반
			if(hs.Has("width"))
			{
				w2 = hs.N("width") / 2;
				h2 = hs.N("height") / 2;
			}

			rc.Set(lon - w2, lat + h2, lon + w2, lat - h2);
		}
		if(rc.l >= rc.r)
		{
			rc.l -= 0.001;
			rc.r += 0.00l;
		}
		if(rc.b >= rc.t)
		{
			rc.b -= 0.001;
			rc.t += 0.00l;
		}
		CString rcl, rcr, rcb, rct;
		rcl.Format(L"%.6f", rc.l);
		rcr.Format(L"%.6f", rc.r);
		rcb.Format(L"%.6f", rc.b);
		rct.Format(L"%.6f", rc.t);
		Quat qs;

		// 3. SQL query실행
		CString fproj = L"perme2";
		Tss ss1;
		Tss sKwd;
		if(key.GetLength() > 0)
			sKwd << "(b.fTitle like '%" << (PS)key << L"%' or b.fDesc like '%" << (PS)key << L"%') and\n";

		ss1 << L"select b.fBizID, b.fLat, b.fLon, b.fTitle, b.fDesc, b.fForm, b.fTel, b.fStJoin, b.fState from tbiz b  \n\
		left join tuser i on i.fUsrID = '" << (PS)uuid << L"' and i.fProj='" << (PS)fproj << L"'\n\
	where b.fLat >= " << (PS)rcb << L" and b.fLat < " << (PS)rct << L" and b.fLon >= " << (PS)rcl << L" and b.fLon < " << (PS)rcr << L" and \n\
		b.fState in ('open','pause') and \n\
		" << Psr(sKwd) << L"\
		(	((b.fbegin <  b.fend) and (b.fbegin < TIME(now()) and TIME(now()) <= b.fend)) or \n\
			((b.fbegin >= b.fend) and (TIME(now()) <= b.fbegin or b.fend < TIME(now()))) ) \n\
		order by p.fUpdated desc limit 200";

		sql = Psr(ss1);

		BOOL bOpen = rec.OpenSelectFetch(sql);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		///AddPolicy(jrs, CFuncItem::eFncContract);
		if(!rec.NoData())
			MakeRecsetToJson(rec, jrs);
		else // 이건 없는데 select 할리가 없다.
			jrs("Return") = L"No Data";
	} CATCH_DB;
	return 0;
}

// test OnBnClickedTestapi()
int ApiSite1::SearchScheduleByLocation(JObj& jpa, JObj& jrs, int iOp)
{
	JObj& hs = jpa;
	JObj& hsr = jrs;
	try
	{
		// 1. DB사용 초기화
		Rec(rec1);
		Rec(recU);
		Rec(rec2);

		int fAllStaff = 0;//합친다.

		int minu = 30; // work unit minute : 30분 간격을 일단위로 전제.
		int m48 = 24 * (60 / minu); // 배열 갯수 48개
		// 1	국경일 휴일 다빼고 영업시간만 1로 셋팅 나머지 0으로

		//CString uuid = jpa.S("fUsrID");

		CString fBegin = jpa.S("fBegin");// L"2020-12-27 14:00:00"
		CString fEnd = jpa.S("fEnd");// L"2020-12-27 15:00:00"
		CTime t1 = jpa.T("fBegin");
		CTime t2 = jpa.T("fEnd");
		CTimeSpan sp = t2 - t1;

		if(t1.GetYear() != t2.GetYear() ||
			t1.GetMonth() != t2.GetMonth() ||
			t1.GetDay() != t2.GetDay() ||
			t1.GetSecond() != 0 ||
			!(t1.GetMinute() == 0 || t1.GetMinute() == 30) ||
			(t2.GetSecond() != 0) ||
			!(t2.GetMinute() == 0 || t2.GetMinute() == 30) ||
			sp.GetTotalMinutes() <= 0)
			throw_response(eHttp_Bad_Request, "검색 시간은 같은날 0분 또는 30분 이어야 합니다.");

		CString fDay = fBegin.Left(10) + L" 00:00:00";

		CTime tDay = KwCStringToCTime(fDay);
		CTime tNextDay = KwNextDay(tDay, 1);//DATE_ADD(DATE(@theDay), INTERVAL 1 DAY);
		//CString theNextDay;
		//KwCTimeToDateString(tNextDay, theNextDay);

		// 검색 하려는 시간 구간으로 시간매트릭스 index 0 ~ 47
		int iTm1 = t1.GetHour() * (60 / minu) + (t1.GetMinute() / minu);//=(HOUR(B46) * (60/A46)) + MINUTE(B46)/A46
		int iTm2 = t2.GetHour() * (60 / minu) + (t2.GetMinute() / minu);

		jpa("fTaskID") = DevGuid(L"task");

		Quat qs;
		qs_Field(qs, fDay);
		qs.Field(jpa, "fTaskID", TRUE);//필수, 작업키
		qs.Field(jpa, "fUsrID", TRUE);//필수, 요청한 사용자
		qs.Field(jpa, "fLat", TRUE);//필수, 위도 36.000
		qs.Field(jpa, "fLon", TRUE);//필수, 경도 127.000

		qs.SetSQL(L"call w_AllScheduleToTemp(@fTaskID, NULL, NULL, @fUsrID, @fDay, 1, 0);");
		_db.ExecuteSQL(qs);//,fLat,fLon,fUpdated,
		
		qs.SetSQL(L"CALL w_BizDistanc_t_m_distance(@fTaskID, @fLat, @fLon);");
		_db.ExecuteSQL(qs);//,fLat,fLon,fUpdated,


		/// 거리상 가까운 샵 가상담당 읽어 오기
		qs.SetSQL(L"\
SELECT * FROM ( -- \n\
	SELECT v.fBizID, v.fVChrgID, v.fUsrID, u.fTel, u.fNickName, b2.fDist -- \n\
		FROM tbizvirtual v -- \n\
		JOIN t_m_distance b2 ON b2.fTaskID = @fTaskID AND b2.fBizID = v.fBizID -- \n\
		left JOIN tuser u ON u.fUsrID = v.fUsrID -- \n\
			ORDER BY b2.fDist LIMIT 100 -- 이 정렬은 순전히 limit 100을 위한거 \n\
) nf2 ORDER BY fBizID, fVChrgID desc;");
		recU.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		if(recU.NoData())
		{
			jrs("Return") = L"No Data";
			return 0;
		}
		//throw_response(eHttp_Conflict, "주변에 펫 샵 담당자가 검색 되지 않습니다.");

		qs.SetSQL(L"\
SELECT * FROM ( -- \n\
	SELECT nf.fOrder, nf.fMode, nf.fBizID, nf.fVChrgID, nf.fOffBegin, nf.fOffEnd, nf.fOff, nf.fNote, round(b2.fLat,6), round(b2.fLon,6), round(b2.fDist,3) -- \n\
		FROM t_m_allsched nf -- \n\
		JOIN t_m_distance b2 ON b2.fTaskID = @fTaskID AND b2.fBizID = nf.fBizID -- \n\
				ORDER BY b2.fDist LIMIT 100 -- 이 정렬은 순전히 limit 100을 위한거 \n\
) nf2 ORDER BY fBizID, fOrder, fVChrgID desc, fOff;");
		BOOL bOpen4 = rec1.OpenSelectFetch(qs);
		if(rec1.NoData())
		{
			jrs("Return") = L"No Data";
			return 0;
		}
		//throw_response(eHttp_Conflict, "주변에 펫 샵 일정이 검색 되지 않습니다.");

		int fOff_10 = 0;
		//KwStr ws1 = L"xx";//fail
		wstring ws2 = L"xx";
		//KaStr as1 = "xx";
		wstring fVChrgTotal = L"total";

		/// 업체별 CBiz객체 : fBizID vs CBiz 
		KStdMapPtr<wstring, CBiz> mtr;
		KStdMapPtr<wstring, CBiz> mtrTt;
		KAtEnd d_mf([&]()
			{
				for(auto& [k, v] : mtr)
					DeleteMeSafe(v);
				for(auto& [k, v] : mtrTt)
					DeleteMeSafe(v);
			});

		/// 없체 하나당 두개 씩 - 그안에 가상담당별 하나씩 하루 리스트 만든다. 두번째는 total 값 가상담당 만둘어 넣는다.
		for(int r = 0; r < recU.RowSize(); r++)
		{
			/// 업체별 CBiz객체 생성
			CBiz* bz0 = nullptr;
			CString fBizID = recU.Cell(r, "fBizID");
			if(!mtr.Has((PWS)fBizID))
			{
				bz0 = new CBiz();
				bz0->fBizID = (PWS)fBizID;
				mtr[(PWS)fBizID] = bz0;
			}
			else
				bz0 = mtr[(PWS)fBizID];
			ASSERT(bz0);
			/// 가상 담당별 컬럼 생성
			CString vch = recU.Cell(r, "fVChrgID");
			if(!vch.IsEmpty())// null이면 공통일정이므로 개인전체를 의미 한다.
				bz0->AddVirtualCharge(vch, r);

			/// 업체별 합산값 넣어둘 CBiz객체 생성
			CBiz* bzt0 = nullptr;
			if(!mtrTt.Has((PWS)fBizID))
			{
				bzt0 = new CBiz();
				bzt0->fBizID = (PWS)fBizID;
				mtrTt[(PWS)fBizID] = bzt0;
			}
			else
				bzt0 = mtrTt[(PWS)fBizID];
			ASSERT(bzt0);
			/// 가상 합산값("totoal") 넣어둘 컬럼 생성
			bzt0->AddVirtualCharge(fVChrgTotal.c_str(), r);//KVCharg 컬럼을 추가 한다.
		}

		// 2	먼저 특별히 근무 하면 1로 OR 해주고
		for(int r = 0; r < rec1.RowSize(); r++)
		{
			CStringW fBizID = rec1.Cell(r, 2);
			ASSERT(!fBizID.IsEmpty());
			auto bz = mtr.get((PWS)fBizID);
			if(bz)//tbizvirtual 없는데, 일정이 있는 경우 는 사실 없다. 
				ScheduleToTimeTable(rec1, bz, r);
			else
				KTrace(L"일정은 근무시간만 있고, 담당자 없는 샵 건너 뜀. %s", fBizID);
		}

		// mtr => mtrTt ,  bz => bzt
		for(auto& [fBizID, bz] : mtr)
		{
			auto bzt = mtrTt.get(fBizID.c_str());
			if(bzt)
				SumOfEachVCharged(bz, bzt);
		}

		/// //////////////////////////////////////////////////////////////
		/// 여기 까지가 GetTodaySchduleTotal 와 fBizID 별 있는거 빼고 같다.
		/// //////////////////////////////////////////////////////////////


		//1. 거리순 인덱스 만들기.
		/// 원하는 시간 범위 슬라이스가 1인지 체크 해야지.
		//2. CJson 으로 만들어서 <- wrong
		//2. in (,,,) 에 넣을 문자열 fBizID 을 만든다.
		//3. tbiz를 위 in 조건으로 order by 거리순으로 다시 쿼리 하여 리턴
		CString inlst;
		int nResult = 0;
		/// total 리스트로 그 시간이 점유 되었나 검사 한다.
		for(auto& [bzID, bzt] : mtrTt)//회사별 토탈값 루프
		{
			// 원하는 시간 범위 슬라이스가 모두 1인지 체크 해야지.
			bool bAllOn = true;
			for(int i = iTm1; i < iTm2; i++)// 검색 키 시간 을 스캔 하여
			{
				auto vcTotal = bzt->_mapVcObj[fVChrgTotal.c_str()];
				auto sl = vcTotal->_arCell[i];// cell체크
				if(sl->fOff == 1)
				{
					bAllOn = false; /// 이미 점유 되었다.
					break;
				}
			}

			if(bAllOn)//요청시 시간 범위내 모두 0(On)으로 검색 조건에 맞는 샵이다.
			{
				CString inbz;
				inbz.Format(L"'%s',", bzt->fBizID.c_str());
				inlst += (PS)inbz; /// in (, , ,) : SQL 문자열을 넣어 만든다.
				nResult++;
			}
		}

		if(nResult > 0)
			inlst.TrimRight(',');
		else
		{
			jrs("Return") = L"No Data";
			return 0;
		}

		/// 여기서 DELETE FROM t_m_??? 하는게 좋겠지.
		// 이제 찾은 샵을 리턴 한다.
		
		Rec(rec);
		//b.fSubTitle,b.fDesc,b.fAddr,
		qs_FieldSub(qs, inlst);
		qs.SetSQL(L"\
SELECT b.fBizID, b.fForm, b.fTel, b.fState, b.fAnimal, b.fAddr, b.fBegin, b.fEnd, b.fTitle, b.fSubTitle, b.fDesc, b.fLat, b.fLon, d.fDist-- \n\
	FROM tbiz b	JOIN t_m_distance d ON d.fBizID = b.fBizID-- \n\
		WHERE b.fBizID IN(@inlst) ORDER BY d.fDist;");

		rec.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		if(rec.NoData())
		{
			jrs("Return") = L"No Data";
			return 0;
		}
		//if(rec.NoData()) 위에서 fBizID in (,,,) 갯수 nResult에서 체크
		MakeRecsetToJson(rec, jrs);
		//2021-01-23 04:21 에 완료. 이제 Post테스트 하려면 진짜 데이터가 들어 가야 한다.
		jrs("Return") = L"OK";//default
	} CATCH_DB;
	return 0;
}

int ApiSite1::SearchScheduleByLocation_off_1(JObj& jpa, JObj& jrs, int iOp)
{
	return 0;
}

#endif // _APIEX
