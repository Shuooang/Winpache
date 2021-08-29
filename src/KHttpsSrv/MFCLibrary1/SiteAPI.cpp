#include "pch.h"
#include "framework.h"
#include "KwLib64/KDebug.h"
#include "MFCLibrary1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


extern "C" {



#ifdef _DEBUG
/// <summary>
/// select test table.
/// </summary>
/// <param name="_db">database object</param>
/// <param name="jpa">request json object</param>
/// <param name="jrs">response json object</param>
/// <param name="iOp">option</param>
/// <returns>0 if sucess. + value logical error. - value is critical eror.</returns>
int DEXPORT ExSiteSample1(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	KRecordset rs(&_db);
	Quat qs;

	qs.SetSQL(L"SELECT * from testtable limit 100");
	BOOL bOpen = rs.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
	rs.MakeRecsetToJson(jrs, L"testtable");

	jrs("Return") = L"OK";
	return 0;
}
/*

{"func":"ExSiteSample1", "params": {}}

{
  "response":{
	"Return":"OK",
	"testtable":[
	  {
		"fName":"Jason",
		"fNumber":1,
		"fTel":"01112345678"
	  },
	  {
		"fName":"Tom",
		"fNumber":2,
		"fTel":"01145457878"
	  }
	]
  },
  "return":0
}
*/
#endif // _DEBUG









/* gagage
	static int s_i = 0;
	s_i++;
	SHP<KRecordset> srs = make_shared<KRecordset>(&_db);
	while(!srs->m_hstmt)
		srs = make_shared<KRecordset>(&_db);
	KRecordset& rs = *srs;
	if(rs.m_hstmt == SQL_NULL_HSTMT)
		TRACE("%2d. KRecordset.m_hstmt is %s\n", s_i, rs.m_hstmt == NULL ? "NULL" : "OK");
*/


#ifdef _Sample_for_2D_Cube

/// request score: GamesRun 부분만 보낸다.
{
	"GUID" : "6547C62090904D9396A35DD279DDEC1C", // 게이머 guid
	"GGUID" : "F0BE2C5A95904BC484F59287855C9B3F", // 게임 gguid
	"note" : "한글 테스트 test 1234", // debug
	"actions" : [
		{
			"direction":2,
				"elapsed" : 0,
				"line" : 8,
				"space" : 3
		},
		{
		  "direction":0,
		  "elapsed" : 0,
		  "line" : 2,
		  "space" : 3
		},
		{
		  "direction":1,
		  "elapsed" : 71620880,
		  "line" : 2,
		  "space" : 3
		},
		{
		  "direction":3,
		  "elapsed" : 89246389,
		  "line" : 8,
		  "space" : 3
		}
	],
	"startIndex":2,
	"stat" : "finished",
	"time" : 89246383
}

#endif // _Sample_for_2D_Cube


/// request score: GamesRun 부분만 보낸다.
#ifdef _request_data
{
	"GUID" : "6547C62090904D9396A35DD279DDEC1C", // 게이머 guid
		"GGUID" : "F0BE2C5A95904BC484F59287855C9B3F", // 게임 gguid
		"note" : "한글 테스트 test 1234", // debug
		"actions" :
		[
	{
		"direction":2,
			"elapsed" : 0,
			"line" : 8,
			"space" : 3
	},
		{
		  "direction":0,
		  "elapsed" : 0,
		  "line" : 2,
		  "space" : 3
		},
		{
		  "direction":1,
		  "elapsed" : 71620880,
		  "line" : 2,
		  "space" : 3
		},
		{
		  "direction":3,
		  "elapsed" : 89246389,
		  "line" : 8,
		  "space" : 3
		}
		],
		"startIndex":2,
			"stat" : "finished",
			"time" : 89246383
}
#endif // _request_data
/// save game score
int SaveCubeScore(KDatabase& _db, JArr& pngm, JObj& jrs, int iOp) noexcept(false)//throw(...)
{
	CString sql;
	try
	{
		_db.TransBegin();
		for(auto& sjvgm : pngm)
		{
			ShJObj sjgm = sjvgm->AsObject();
			Quat qs;
			/// sql 문에 변수명을 달리 하는 예
#ifdef _DEBUG_req
			{
				"func":"CubeSaveCubeScore",
					"params" : {
					"GGUID":"5C75C044FF03444FBE48E7E76DFF49B5",
						"GUID" : "3D813446B6084A6AA48D0AACADD61F34",
						"actions" : [
					{
						"direction":1,
							"elapsed" : 24960507,
							"index" : 0,
							"line" : 2,
							"space" : 3
					},
	  {
		"direction":2,
		"elapsed" : 44888880,
		"index" : 1,
		"line" : 3,
		"space" : 3
	  }
						],
						"cube":"0,0,0,4,1,1,2,2,2\n0,0,0,4,1,1,2,2,2\n4,1,1,2,2,2,0,0,0\n3,3,3,1,4,4,5,5,5\n3,3,3,1,4,4,5,5,5\n3,3,3,1,4,4,5,5,5\n",
							"mode" : "axis",
							"note" : "한글 테스트 test 1234",
							"stat" : "finished",
							"time" : 44888877,
							"unit" : 3
				}
			}
#endif // _DEBUG_req
			qs.Field(*sjgm, "GUID", "fuuid", TRUE);//필수
			qs.Field(*sjgm, "GGUID", "fgguid", TRUE);//필수
			qs.Field(*sjgm, "cube", "fcube", TRUE);
			qs.Field(*sjgm, "note", "fnote");
			qs.Field(*sjgm, "time", "fSec7th", TRUE);
			qs.Field(*sjgm, "unit", "fUnit");// 기존 데이터 땜에 , TRUE);
			qs.Field(*sjgm, "mode", "fMode");// 기존 데이터 땜에, TRUE);
			qs.Field(*sjgm, "initGGUID", "fInitGguid");// 기존 데이터 땜에, TRUE);
			
			ShJArr jacts = sjgm->Array("actions");
			auto nar = jacts->size();
			qs.FieldNum("fCount", (int)nar);
			Quat qs1;
			if(nar > 0)
			{
				qs.SetSQL(L"\
INSERT IGNORE INTO tgame  ( fuuid,  fgguid, fcube,   fnote,   fSec7th, fUnit,  fMode,  fCount,  fInitGguid) -- \n\
                   VALUES (@fuuid, @fgguid, @fcube, @fnote, @fSec7th, @fUnit, @fMode, @fCount, @fInitGguid);");
				sql = qs._sqlv;
				_db.ExecuteSQL(qs);

				CString gguid = sjgm->S("GGUID");
				/// sql 문에 변수명과 일치 하는 예
				qs1.Field("GGUID", gguid, TRUE);//필수
				qs1.AppendSQL(L"\
INSERT IGNORE INTO taction (fgguid, findex, fdirect, felapsed, fline, fspace) VALUES -- \r\n");
				int i = 0;//마지막 항복인걸 알아내기 위해
				for(auto& jvl : *jacts)
				{
					if(!jvl->IsObject())
						throw_BadRequest(-1, "Wrong a game action data format.");
					auto& jia = *jvl->AsObject();
					//(*sjact)("index") = index;//처음에는 여기서 만들었는데, client에서 보내기로
					if(!jia.Has("index"))
						qs1.Field("index", i);
					else
						qs1.Field(jia, "index", TRUE);//필수
					qs1.Field(jia, "direction", TRUE);//필수
					qs1.Field(jia, "elapsed", TRUE);//필수 mysql double type range 1.7976931348623157e+308 ~ -2.225077738585072014e-308
					qs1.Field(jia, "line", TRUE);//필수
					qs1.Field(jia, "space", TRUE);//필수
					qs1.AppendSQL(L"\t(@GGUID, @index, @direction, @elapsed, @line, @space)",
						i < (nar - 1) ? L", -- \r\n" : L" -- \r\n"); // 원래 L";" 였는데, on duplicate key 붙으면서
					i++;
				}
				qs1.AppendSQL(L"\
		ON DUPLICATE KEY -- \r\n\
		UPDATE frewrite = frewrite + 1;");
				sql = qs1._sqlv;
				_db.ExecuteSQL(qs1);
			}
#ifdef _DEBUG_sample
			INSERT INTO taction(fgguid, findex, fdirect, felapsed, fline, fspace) VALUES--
				('0FDD1FA6E4DC4E3EA31D0E3ECDF0C9F4', '0', 3, 12641622, 6, 3),
				('0FDD1FA6E4DC4E3EA31D0E3ECDF0C9F4', '1', 3, 24892756, 5, 3);
#endif // _DEBUG_sample
		}
		_db.TransCommit();
	}
	catch(CException* e)
	{/// 여기서 catch한 이유는 단지 Rollback 하기 위함이다.
		KTrace(L"1 %s\n", sql);
		_db.TransRollback();//어떤 오류도 
		throw e;/// rethrow 해야 message가 response된다.
	}/// 다른 Exceptions 은 caller가 catch 한다.
	catch(...)
	{
		KTrace(L"2 %s\n", sql);
		_db.TransRollback();//어떤 오류도 
		throw_BadRequest(-3, L"Unknown error in SaveCubeScore.");
	}
	return 0;
}

int DEXPORT CubeRegisterUser(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp) noexcept(false) //throw le;
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());


	ShJArr pngm = jpa.Array("PendingGames");
	CException* le = nullptr;
	if(pngm)
	{
		try
		{
			int rv = SaveCubeScore(_db, *pngm, jrs, iOp);
			jrs("ReturnPendingGames") = L"OK";
		}
		catch (CException* e)
		{
			///여기서 에러나도 아래 등록은 시행 해야 해서
			jrs("ReturnPendingGames") = L"Error";// "OK"
			le = e;
		}
	}

	Quat qs;
	/// sql 문에 변수명을 달리 하는 예
	qs.Field(jpa, "GUID", "fuuid", TRUE);//필수
	qs.Field(jpa, "email", "femail", 0,1,0);
	qs.Field(jpa, "SNS", "fSNS", 0, 1, 0);
	qs.Field(jpa, "note", "fnote", 0, 1, 0);
	qs.Field(jpa, "Nickname", "fNickname", 0, 1, 0);
	
	/// 아래 에러는 3번째 변수 bNullIfEmpty와는 상관 없이 데이터가 없으면 NULL로 들어간다.
	/// Column 'femail' cannot be null
	/// 따라서 DB table column 속성을 nullable로 바꿔야 한다.
	/// 이상하게 MariaDB에서 not null에 default '' 공백문자를 줬는데도 NULL이 안들어 간다.
	/// insert field에 아예 없으면 모를까, 직접 null을  not null 항목에 집어 넣는 경우는 default value와 관계가 없다.
#ifdef _DEBUG_error
	{
		"error":"Column 'femail' cannot be null\n",
			"return" : 122,
			"status" : 417
	}
#endif // _DEBUG_error
	qs.SetSQL(L"\
INSERT INTO tuser ( fuuid,  femail,  fSNS,  fnote, fNickname) -- \n\
			VALUES (@fuuid, @femail, @fSNS, @fnote, @fNickname) -- \n\
	on duplicate key -- \n\
update femail = values(femail), fSNS = values(fSNS), fnote = values(fnote), fNickname = values(fNickname), fCntAccess = fCntAccess + 1;");
	_db.ExecuteSQL(qs);
	jrs("Return") = L"OK";
	if(le)/// 위에서 펜딩게임 저장 하다가 오류 나더라도 로그도 쌓아야 하고, 리턴도 해야 되고.
		throw le;//noexcept(false)
	return 0;
}


int DEXPORT CubeSaveCubeScore(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ShJArr pngm;
	if(jpa.Has("PendingGames"))/// 꾸러미채로 올수도
	{
		pngm = jpa.Array("PendingGames");
	}
	else /// 단인 항목으로 올 수도
	{
		pngm = make_shared<JArr>();
		pngm->Add(jpa, false);
	}
	int rv = SaveCubeScore(_db, *pngm, jrs, iOp);

	jrs("Return") = L"OK";
	return 0;
}

///?deprecated : 
int DEXPORT CubeCheckCubeScore(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	KRecordset rs(&_db);
	Quat qs;
	qs.Field(jpa, "GGUID", "fgguid", TRUE);//필수

	qs.SetSQL(L"SELECT count(*) from tgame where fgguid = @fgguid");
	BOOL bOpen = rs.OpenSelectFetch(qs);/// 위에 테이블이 2개 이지만 transaction 으로 한꺼번에 처리 하였으므로 하나만 체크
	int ngame = rs.CellI(0, 0);
	jrs("count") = ngame;/// 저장 이미 되었으면 당연히 1 이겠지.

	jrs("Return") = ngame ? L"OK" : L"No Data";
	return 0;
}

int DEXPORT CubeGetRankingList(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	KRecordset rs(&_db);
	Quat qs;
// 	if(!jpa.Has("unit"))
// 		jpa("unit") = 3;/// 현재는 3칸 이동만 경기 랭킹 가능. 차후에는 1칸, 2칸 리그도 가능
// 	if(!jpa.Has("class"))
// 		jpa("class") = "personal";/// 현재는 3칸 이동만 경기 랭킹 가능. 차후에는 1칸, 2칸 리그도 가능
	jpa.HasElse("unit", 3);/// default 값 3
	jpa.HasElse("class", "personal");
	qs.Field(jpa, "unit", "fUnit");//필수
	qs.Field(jpa, "class", "fClass");

	qs.SetSQL(L"SELECT g.fgguid, g.fuuid, (g.fSec7th/10000000) fSec, g.fCount, u.fNickname, DATE_ADD(g.fTimeReg, INTERVAL -9 HOUR) fTimeReg, g.fcube -- \r\n\
FROM tgame g JOIN tuser u ON g.fuuid=u.fuuid WHERE -- \r\n\
g.fUnit = @fUnit and g.fClass = @fClass and g.fState = 'complete' ORDER BY g.fSec7th, g.fTimeReg desc LIMIT 20;");
	BOOL bOpen = rs.OpenSelectFetch(qs);/// 위에 테이블이 2개 이지만 transaction 으로 한꺼번에 처리 하였으므로 하나만 체크
	jrs("Return") = rs.MakeRecsetToJson(jrs, L"table") ? "OK" : "No Data";
	return 0;
}


int DEXPORT CubeGetGameActions(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	KRecordset rs(&_db);
	Quat qs;
	qs.Field(jpa, "GGUID", "fgguid", TRUE);//필수
	qs.SetSQL(L"SELECT a.findex, a.fdirect, a.fline, a.fspace, a.felapsed FROM taction a WHERE a.fgguid = @fgguid ORDER BY a.findex;");
	BOOL bOpen = rs.OpenSelectFetch(qs);/// 위에 테이블이 2개 이지만 transaction 으로 한꺼번에 처리 하였으므로 하나만 체크
	jrs("Return") = rs.MakeRecsetToJson(jrs, L"table") ? "OK" : "No Data";
	return 0;
}

 

















}//extern "C" {
