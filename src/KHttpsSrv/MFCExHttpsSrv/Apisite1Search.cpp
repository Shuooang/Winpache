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

		// 1. DB��� �ʱ�ȭ
		Rec(rec);
		int fAllStaff = 0;//��ģ��.
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
			double w2 = 1; //����
			double h2 = 1; //����
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

		// 3. SQL query����
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

		BOOL bOpen = rec.OpenSelectFetch(sql);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
		///AddPolicy(jrs, CFuncItem::eFncContract);
		if(!rec.NoData())
			MakeRecsetToJson(rec, jrs);
		else // �̰� ���µ� select �Ҹ��� ����.
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
		// 1. DB��� �ʱ�ȭ
		Rec(rec1);
		Rec(recU);
		Rec(rec2);

		int fAllStaff = 0;//��ģ��.

		int minu = 30; // work unit minute : 30�� ������ �ϴ����� ����.
		int m48 = 24 * (60 / minu); // �迭 ���� 48��
		// 1	������ ���� �ٻ��� �����ð��� 1�� ���� ������ 0����

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
			throw_response(eHttp_Bad_Request, "�˻� �ð��� ������ 0�� �Ǵ� 30�� �̾�� �մϴ�.");

		CString fDay = fBegin.Left(10) + L" 00:00:00";

		CTime tDay = KwCStringToCTime(fDay);
		CTime tNextDay = KwNextDay(tDay, 1);//DATE_ADD(DATE(@theDay), INTERVAL 1 DAY);
		//CString theNextDay;
		//KwCTimeToDateString(tNextDay, theNextDay);

		// �˻� �Ϸ��� �ð� �������� �ð���Ʈ���� index 0 ~ 47
		int iTm1 = t1.GetHour() * (60 / minu) + (t1.GetMinute() / minu);//=(HOUR(B46) * (60/A46)) + MINUTE(B46)/A46
		int iTm2 = t2.GetHour() * (60 / minu) + (t2.GetMinute() / minu);

		jpa("fTaskID") = DevGuid(L"task");

		Quat qs;
		qs_Field(qs, fDay);
		qs.Field(jpa, "fTaskID", TRUE);//�ʼ�, �۾�Ű
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�, ��û�� �����
		qs.Field(jpa, "fLat", TRUE);//�ʼ�, ���� 36.000
		qs.Field(jpa, "fLon", TRUE);//�ʼ�, �浵 127.000

		qs.SetSQL(L"call w_AllScheduleToTemp(@fTaskID, NULL, NULL, @fUsrID, @fDay, 1, 0);");
		_db.ExecuteSQL(qs);//,fLat,fLon,fUpdated,
		
		qs.SetSQL(L"CALL w_BizDistanc_t_m_distance(@fTaskID, @fLat, @fLon);");
		_db.ExecuteSQL(qs);//,fLat,fLon,fUpdated,


		/// �Ÿ��� ����� �� ������ �о� ����
		qs.SetSQL(L"\
SELECT * FROM ( -- \n\
	SELECT v.fBizID, v.fVChrgID, v.fUsrID, u.fTel, u.fNickName, b2.fDist -- \n\
		FROM tbizvirtual v -- \n\
		JOIN t_m_distance b2 ON b2.fTaskID = @fTaskID AND b2.fBizID = v.fBizID -- \n\
		left JOIN tuser u ON u.fUsrID = v.fUsrID -- \n\
			ORDER BY b2.fDist LIMIT 100 -- �� ������ ������ limit 100�� ���Ѱ� \n\
) nf2 ORDER BY fBizID, fVChrgID desc;");
		recU.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
		if(recU.NoData())
		{
			jrs("Return") = L"No Data";
			return 0;
		}
		//throw_response(eHttp_Conflict, "�ֺ��� �� �� ����ڰ� �˻� ���� �ʽ��ϴ�.");

		qs.SetSQL(L"\
SELECT * FROM ( -- \n\
	SELECT nf.fOrder, nf.fMode, nf.fBizID, nf.fVChrgID, nf.fOffBegin, nf.fOffEnd, nf.fOff, nf.fNote, round(b2.fLat,6), round(b2.fLon,6), round(b2.fDist,3) -- \n\
		FROM t_m_allsched nf -- \n\
		JOIN t_m_distance b2 ON b2.fTaskID = @fTaskID AND b2.fBizID = nf.fBizID -- \n\
				ORDER BY b2.fDist LIMIT 100 -- �� ������ ������ limit 100�� ���Ѱ� \n\
) nf2 ORDER BY fBizID, fOrder, fVChrgID desc, fOff;");
		BOOL bOpen4 = rec1.OpenSelectFetch(qs);
		if(rec1.NoData())
		{
			jrs("Return") = L"No Data";
			return 0;
		}
		//throw_response(eHttp_Conflict, "�ֺ��� �� �� ������ �˻� ���� �ʽ��ϴ�.");

		int fOff_10 = 0;
		//KwStr ws1 = L"xx";//fail
		wstring ws2 = L"xx";
		//KaStr as1 = "xx";
		wstring fVChrgTotal = L"total";

		/// ��ü�� CBiz��ü : fBizID vs CBiz 
		KStdMapPtr<wstring, CBiz> mtr;
		KStdMapPtr<wstring, CBiz> mtrTt;
		KAtEnd d_mf([&]()
			{
				for(auto& [k, v] : mtr)
					DeleteMeSafe(v);
				for(auto& [k, v] : mtrTt)
					DeleteMeSafe(v);
			});

		/// ��ü �ϳ��� �ΰ� �� - �׾ȿ� �����纰 �ϳ��� �Ϸ� ����Ʈ �����. �ι�°�� total �� ������ ���Ѿ� �ִ´�.
		for(int r = 0; r < recU.RowSize(); r++)
		{
			/// ��ü�� CBiz��ü ����
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
			/// ���� ��纰 �÷� ����
			CString vch = recU.Cell(r, "fVChrgID");
			if(!vch.IsEmpty())// null�̸� ���������̹Ƿ� ������ü�� �ǹ� �Ѵ�.
				bz0->AddVirtualCharge(vch, r);

			/// ��ü�� �ջ갪 �־�� CBiz��ü ����
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
			/// ���� �ջ갪("totoal") �־�� �÷� ����
			bzt0->AddVirtualCharge(fVChrgTotal.c_str(), r);//KVCharg �÷��� �߰� �Ѵ�.
		}

		// 2	���� Ư���� �ٹ� �ϸ� 1�� OR ���ְ�
		for(int r = 0; r < rec1.RowSize(); r++)
		{
			CStringW fBizID = rec1.Cell(r, 2);
			ASSERT(!fBizID.IsEmpty());
			auto bz = mtr.get((PWS)fBizID);
			if(bz)//tbizvirtual ���µ�, ������ �ִ� ��� �� ��� ����. 
				ScheduleToTimeTable(rec1, bz, r);
			else
				KTrace(L"������ �ٹ��ð��� �ְ�, ����� ���� �� �ǳ� ��. %s", fBizID);
		}

		// mtr => mtrTt ,  bz => bzt
		for(auto& [fBizID, bz] : mtr)
		{
			auto bzt = mtrTt.get(fBizID.c_str());
			if(bzt)
				SumOfEachVCharged(bz, bzt);
		}

		/// //////////////////////////////////////////////////////////////
		/// ���� ������ GetTodaySchduleTotal �� fBizID �� �ִ°� ���� ����.
		/// //////////////////////////////////////////////////////////////


		//1. �Ÿ��� �ε��� �����.
		/// ���ϴ� �ð� ���� �����̽��� 1���� üũ �ؾ���.
		//2. CJson ���� ���� <- wrong
		//2. in (,,,) �� ���� ���ڿ� fBizID �� �����.
		//3. tbiz�� �� in �������� order by �Ÿ������� �ٽ� ���� �Ͽ� ����
		CString inlst;
		int nResult = 0;
		/// total ����Ʈ�� �� �ð��� ���� �Ǿ��� �˻� �Ѵ�.
		for(auto& [bzID, bzt] : mtrTt)//ȸ�纰 ��Ż�� ����
		{
			// ���ϴ� �ð� ���� �����̽��� ��� 1���� üũ �ؾ���.
			bool bAllOn = true;
			for(int i = iTm1; i < iTm2; i++)// �˻� Ű �ð� �� ��ĵ �Ͽ�
			{
				auto vcTotal = bzt->_mapVcObj[fVChrgTotal.c_str()];
				auto sl = vcTotal->_arCell[i];// cellüũ
				if(sl->fOff == 1)
				{
					bAllOn = false; /// �̹� ���� �Ǿ���.
					break;
				}
			}

			if(bAllOn)//��û�� �ð� ������ ��� 0(On)���� �˻� ���ǿ� �´� ���̴�.
			{
				CString inbz;
				inbz.Format(L"'%s',", bzt->fBizID.c_str());
				inlst += (PS)inbz; /// in (, , ,) : SQL ���ڿ��� �־� �����.
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

		/// ���⼭ DELETE FROM t_m_??? �ϴ°� ������.
		// ���� ã�� ���� ���� �Ѵ�.
		
		Rec(rec);
		//b.fSubTitle,b.fDesc,b.fAddr,
		qs_FieldSub(qs, inlst);
		qs.SetSQL(L"\
SELECT b.fBizID, b.fForm, b.fTel, b.fState, b.fAnimal, b.fAddr, b.fBegin, b.fEnd, b.fTitle, b.fSubTitle, b.fDesc, b.fLat, b.fLon, d.fDist-- \n\
	FROM tbiz b	JOIN t_m_distance d ON d.fBizID = b.fBizID-- \n\
		WHERE b.fBizID IN(@inlst) ORDER BY d.fDist;");

		rec.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
		if(rec.NoData())
		{
			jrs("Return") = L"No Data";
			return 0;
		}
		//if(rec.NoData()) ������ fBizID in (,,,) ���� nResult���� üũ
		MakeRecsetToJson(rec, jrs);
		//2021-01-23 04:21 �� �Ϸ�. ���� Post�׽�Ʈ �Ϸ��� ��¥ �����Ͱ� ��� ���� �Ѵ�.
		jrs("Return") = L"OK";//default
	} CATCH_DB;
	return 0;
}

int ApiSite1::SearchScheduleByLocation_off_1(JObj& jpa, JObj& jrs, int iOp)
{
	return 0;
}

#endif // _APIEX
