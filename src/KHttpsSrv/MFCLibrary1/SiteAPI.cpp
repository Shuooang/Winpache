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
	BOOL bOpen = rs.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
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

/// request score: GamesRun �κи� ������.
{
	"GUID" : "6547C62090904D9396A35DD279DDEC1C", // ���̸� guid
	"GGUID" : "F0BE2C5A95904BC484F59287855C9B3F", // ���� gguid
	"note" : "�ѱ� �׽�Ʈ test 1234", // debug
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


/// request score: GamesRun �κи� ������.
#ifdef _request_data
{
	"GUID" : "6547C62090904D9396A35DD279DDEC1C", // ���̸� guid
		"GGUID" : "F0BE2C5A95904BC484F59287855C9B3F", // ���� gguid
		"note" : "�ѱ� �׽�Ʈ test 1234", // debug
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
			/// sql ���� �������� �޸� �ϴ� ��
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
							"note" : "�ѱ� �׽�Ʈ test 1234",
							"stat" : "finished",
							"time" : 44888877,
							"unit" : 3
				}
			}
#endif // _DEBUG_req
			qs.Field(*sjgm, "GUID", "fuuid", TRUE);//�ʼ�
			qs.Field(*sjgm, "GGUID", "fgguid", TRUE);//�ʼ�
			qs.Field(*sjgm, "cube", "fcube", TRUE);
			qs.Field(*sjgm, "note", "fnote");
			qs.Field(*sjgm, "time", "fSec7th", TRUE);
			qs.Field(*sjgm, "unit", "fUnit");// ���� ������ ���� , TRUE);
			qs.Field(*sjgm, "mode", "fMode");// ���� ������ ����, TRUE);
			qs.Field(*sjgm, "initGGUID", "fInitGguid");// ���� ������ ����, TRUE);
			
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
				/// sql ���� ������� ��ġ �ϴ� ��
				qs1.Field("GGUID", gguid, TRUE);//�ʼ�
				qs1.AppendSQL(L"\
INSERT IGNORE INTO taction (fgguid, findex, fdirect, felapsed, fline, fspace) VALUES -- \r\n");
				int i = 0;//������ �׺��ΰ� �˾Ƴ��� ����
				for(auto& jvl : *jacts)
				{
					if(!jvl->IsObject())
						throw_BadRequest(-1, "Wrong a game action data format.");
					auto& jia = *jvl->AsObject();
					//(*sjact)("index") = index;//ó������ ���⼭ ������µ�, client���� �������
					if(!jia.Has("index"))
						qs1.Field("index", i);
					else
						qs1.Field(jia, "index", TRUE);//�ʼ�
					qs1.Field(jia, "direction", TRUE);//�ʼ�
					qs1.Field(jia, "elapsed", TRUE);//�ʼ� mysql double type range 1.7976931348623157e+308 ~ -2.225077738585072014e-308
					qs1.Field(jia, "line", TRUE);//�ʼ�
					qs1.Field(jia, "space", TRUE);//�ʼ�
					qs1.AppendSQL(L"\t(@GGUID, @index, @direction, @elapsed, @line, @space)",
						i < (nar - 1) ? L", -- \r\n" : L" -- \r\n"); // ���� L";" ���µ�, on duplicate key �����鼭
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
	{/// ���⼭ catch�� ������ ���� Rollback �ϱ� �����̴�.
		KTrace(L"1 %s\n", sql);
		_db.TransRollback();//� ������ 
		throw e;/// rethrow �ؾ� message�� response�ȴ�.
	}/// �ٸ� Exceptions �� caller�� catch �Ѵ�.
	catch(...)
	{
		KTrace(L"2 %s\n", sql);
		_db.TransRollback();//� ������ 
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
			///���⼭ �������� �Ʒ� ����� ���� �ؾ� �ؼ�
			jrs("ReturnPendingGames") = L"Error";// "OK"
			le = e;
		}
	}

	Quat qs;
	/// sql ���� �������� �޸� �ϴ� ��
	qs.Field(jpa, "GUID", "fuuid", TRUE);//�ʼ�
	qs.Field(jpa, "email", "femail", 0,1,0);
	qs.Field(jpa, "SNS", "fSNS", 0, 1, 0);
	qs.Field(jpa, "note", "fnote", 0, 1, 0);
	qs.Field(jpa, "Nickname", "fNickname", 0, 1, 0);
	
	/// �Ʒ� ������ 3��° ���� bNullIfEmpty�ʹ� ��� ���� �����Ͱ� ������ NULL�� ����.
	/// Column 'femail' cannot be null
	/// ���� DB table column �Ӽ��� nullable�� �ٲ�� �Ѵ�.
	/// �̻��ϰ� MariaDB���� not null�� default '' ���鹮�ڸ� ��µ��� NULL�� �ȵ�� ����.
	/// insert field�� �ƿ� ������ �𸦱�, ���� null��  not null �׸� ���� �ִ� ���� default value�� ���谡 ����.
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
	if(le)/// ������ ������� ���� �ϴٰ� ���� ������ �α׵� �׾ƾ� �ϰ�, ���ϵ� �ؾ� �ǰ�.
		throw le;//noexcept(false)
	return 0;
}


int DEXPORT CubeSaveCubeScore(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	ShJArr pngm;
	if(jpa.Has("PendingGames"))/// �ٷ���ä�� �ü���
	{
		pngm = jpa.Array("PendingGames");
	}
	else /// ���� �׸����� �� ����
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
	qs.Field(jpa, "GGUID", "fgguid", TRUE);//�ʼ�

	qs.SetSQL(L"SELECT count(*) from tgame where fgguid = @fgguid");
	BOOL bOpen = rs.OpenSelectFetch(qs);/// ���� ���̺��� 2�� ������ transaction ���� �Ѳ����� ó�� �Ͽ����Ƿ� �ϳ��� üũ
	int ngame = rs.CellI(0, 0);
	jrs("count") = ngame;/// ���� �̹� �Ǿ����� �翬�� 1 �̰���.

	jrs("Return") = ngame ? L"OK" : L"No Data";
	return 0;
}

int DEXPORT CubeGetRankingList(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	KRecordset rs(&_db);
	Quat qs;
// 	if(!jpa.Has("unit"))
// 		jpa("unit") = 3;/// ����� 3ĭ �̵��� ��� ��ŷ ����. ���Ŀ��� 1ĭ, 2ĭ ���׵� ����
// 	if(!jpa.Has("class"))
// 		jpa("class") = "personal";/// ����� 3ĭ �̵��� ��� ��ŷ ����. ���Ŀ��� 1ĭ, 2ĭ ���׵� ����
	jpa.HasElse("unit", 3);/// default �� 3
	jpa.HasElse("class", "personal");
	qs.Field(jpa, "unit", "fUnit");//�ʼ�
	qs.Field(jpa, "class", "fClass");

	qs.SetSQL(L"SELECT g.fgguid, g.fuuid, (g.fSec7th/10000000) fSec, g.fCount, u.fNickname, DATE_ADD(g.fTimeReg, INTERVAL -9 HOUR) fTimeReg, g.fcube -- \r\n\
FROM tgame g JOIN tuser u ON g.fuuid=u.fuuid WHERE -- \r\n\
g.fUnit = @fUnit and g.fClass = @fClass and g.fState = 'complete' ORDER BY g.fSec7th, g.fTimeReg desc LIMIT 20;");
	BOOL bOpen = rs.OpenSelectFetch(qs);/// ���� ���̺��� 2�� ������ transaction ���� �Ѳ����� ó�� �Ͽ����Ƿ� �ϳ��� üũ
	jrs("Return") = rs.MakeRecsetToJson(jrs, L"table") ? "OK" : "No Data";
	return 0;
}


int DEXPORT CubeGetGameActions(KDatabase& _db, JObj& jpa, JObj& jrs, int iOp)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	KRecordset rs(&_db);
	Quat qs;
	qs.Field(jpa, "GGUID", "fgguid", TRUE);//�ʼ�
	qs.SetSQL(L"SELECT a.findex, a.fdirect, a.fline, a.fspace, a.felapsed FROM taction a WHERE a.fgguid = @fgguid ORDER BY a.findex;");
	BOOL bOpen = rs.OpenSelectFetch(qs);/// ���� ���̺��� 2�� ������ transaction ���� �Ѳ����� ó�� �Ͽ����Ƿ� �ϳ��� üũ
	jrs("Return") = rs.MakeRecsetToJson(jrs, L"table") ? "OK" : "No Data";
	return 0;
}

 

















}//extern "C" {
