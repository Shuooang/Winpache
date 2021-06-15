#include "pch.h"
#include "ApiSite1.h"

#include "KwLib64/KRect.h"
#include "KwLib64/TimeTool.h"
#include "KwLib64/inlinefnc.h"
#include "KwLib64/KDebug.h"
#include "KwLib64/tchtool.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef _APIEX


int ApiSite1::MigrationImage(CString sql, CString SrcImagePath, CString SrcTable)
{
	try
	{
		auto rs = this->CheckDB();
		ASSERT(rs == NULL);
		_db.TransBegin();

		KRecordset rs(&_db);
		// 3. SQL query실행
		BOOL bOpen = rs.OpenSelectFetch(sql);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.

		// 4. SQL Query결과를 JSON으로 리턴
		//MakeRecsetToJson(rs, jrs);
	// sql 실행 시켜 idx, path를 읽어 온다.
	// Src 이미지 파일을 읽어서, new 이미치 폴더에 GUID로 복사 한다.
	// t_images에 Src의 idx, table, old path, new GUID와 함께 저장 한다.
	// 마그한 v2 table에 imageID는 v1 table의 idx+oldTable의 guid를 넣어주면 된다.

		//JSONArray ar;
		for(int r = 0; r < rs.RowSize(); r++)
		{
			auto idx = rs.CellI(r, 0);
			auto pathn = rs.CellSA(r, 1);// /category/style/7_20200729103233.jpeg
			if(pathn.IsEmpty())
				continue;
			CStringA pathnr = KwReplaceStr(pathn, "/", "\\");
			CStringA fullSrc = CStringA(SrcImagePath) + pathnr;
			auto ie = pathnr.ReverseFind('.');
			CStringA ext;
			if(ie < 0)
				ext = "jpeg";
			else
				ext = pathnr.Mid(ie + 1);
			try
			{

				CFile f(CStringW(fullSrc), CFile::modeRead);
				auto len = f.GetLength();
				char* buf = new char[len];
				KAtEnd d_buf([&]() -> void
					{
						DeleteMeSafe(buf);
					});

				CStringA contType, contLen;
				contType.Format("image/%s", ext);
				contLen.Format("%I64u", len);
				f.Read(buf, (UINT)len);

				JObj jsr;
				CStringA SrcTableA(SrcTable);
				int rv = ImageUpload(buf, len, (PAS)pathn, contType, contLen, jsr, SrcTableA, idx, "jpeg");
			}
			catch(CException*)
			{
				TRACE("File Open error. %s ", fullSrc);
			}
		}



		_db.TransCommit();
	}
	catch(KException* e)//rethrow 한것을 처리 한다.
	{
		_db.TransRollback();
		this->LogException(e, L"MigrationImage");// L"ImageDownLoad");//오류는 파라미터 챙길게 많아서 동기로 처리 한다.
		//JError(jres, e->m_strError, e->_error);
		//rv = e->m_nRetCode;// -100;
		return -1;
	}
	catch(CException* e)
	{//모두 KException 로 바꿔서 rethrow하므로 여기로 오면 호출 하다 의외 심각한 오류일수있다.
		_db.TransRollback();
		CString sError;
		e->GetErrorMessage(sError.GetBuffer(1024), 1024); sError.ReleaseBuffer();
		DWORD err = GetLastError();
		//rv = err;
		if(sError.IsEmpty())
			sError = L"Unknown ImageUpload Error.";
		//JError(jres, sError, err);
		this->LogExcepFN(e);//CException에는 FILE, LINE이 없어 오류는 파라미터 챙길게 많다.
		return -2;
	}
	return 0;
}


/// client: CheckUser
int ApiSite1::UserManage(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		// 1. DB사용 초기화
		Rec(recSelUser);

		if(!jpa.Has("Option"))
			throw_response(eHttp_Bad_Request, "Option is not found.");
		JObj jop(jpa.O("Option"));

		Quat qs;//필수, 쿼터, NULL

		CString fUsrID;
		if(!jpa.Has("fUsrID"))
			fUsrID = DevGuid(L"user");
		else
			fUsrID = jpa.S("fUsrID");

		qs.Field("fUsrID", fUsrID);
		//qs.Field(jpa, "fUsrID");//필수아님 . 없으면 널.
		qs.Field(jpa, "fTel");
		qs.Field(jpa, "fEmail");
		qs.Field(jpa, "fAppVer");
		qs.Field(jpa, "fModel");
		qs.Field(jpa, "fPush");
		qs.Field(jpa, "fProj");// , TRUE);//필수 일단 없앤다.
		qs.Field(jpa, "fLoginID");
		qs.Field(jpa, "fPcPwd");
		qs.Field(jpa, "fCertCode");


		CString action = jop.S("action");

		jrs("Return") = L"OK";//default

		_db.TransBegin();

		if(action == L"select" || action == L"insert" || action == L"update")
		{
			if(action == L"insert" || action == L"update")
			{	// server저장후에 local 저장 안된 경우 uuid가 같은 값으로 오므로 중복 된다.
				qs.SetSQL(L"\
insert into tuser (fUsrID,  fEmail,  fTel, fRecover, fStat,  fAppVer,  fModel,  fPush,  fProj, fLoginID, fPcPwd, fCertCode) values \r\n\
					(@fUsrID, @fEmail, @fTel,     NULL,  'ON', @fAppVer, @fModel, @fPush, @fProj, @fLoginID, @fPcPwd, @fCertCode) \r\n\
	on duplicate key update \n\
		femail = values(fEmail), fTel = values(fTel), fRecover = values(fRecover), fAppVer = values(fAppVer), \n\
		fModel = values(fModel), fPush = values(fPush), fStat = values(fStat) -- \n\
, fLoginID = values(fLoginID), fPcPwd = values(fPcPwd), fCertCode = values(fCertCode)");

				// on duplicate key일떄는 UpdateBusiness 처럼 JsonToUpdateSetField 쓰지 않고, sql만으로 가능 하다.
				_db.ExecuteSQL(qs);
			}

			qs.SetSQL(L"\
select u.fUsrID, u.fTel, u.fTel2, u.fEmail, u.fNickName, u.fAppVer, u.fModel, u.fPush, u.fProj from tuser u where u.fUsrID = @fUsrID;");
			recSelUser.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
			if(recSelUser.NoData())
				jrs("Return") = L"NoData";
			//throw_response(eHttp_Not_Found, L"사용자 정보 오류.");
			SelectEnum(jrs, L"tuser");
			MakeRecsetToJson(recSelUser, jrs);
		}
		else if(action == L"delete") // 이거 client에 요청 모듈 없음.
		{
			//sql.Format(L"update tuser set fStat = 'off' where fUsrID=%s ", jpa.QS("fUsrID"));
			qs.SetSQL(L"update tuser set fStat = 'off' where fUsrID=@fUsrID;");
			_db.ExecuteSQL(qs);
		}
		else
		{
			throw_response(eHttp_Bad_Request, "Parameter Error");// CResponse1::ResponseForPost 에서 catch(KException* e)
		}
		_db.TransCommit();
	} CATCH_DBTR;
	return 0;
}

int ApiSite1::RequestLogin(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		// 1. DB사용 초기화
		Rec(recSelUser);

		Quat qs;//필수, 쿼터, NULL

		CString fUsrID;
		qs.Field(jpa, "fLoginID", TRUE);
		qs.Field(jpa, "fPcPwd");
		qs.Field(jpa, "fCertCode");
		qs.Field(jpa, "fUsrID");
		qs.Field("FIELDS", L"u.fUsrID, u.fTel, u.fTel2, u.fEmail, u.fNickName, u.fAppVer, u.fModel, u.fPush, u.fProj");
		if(jpa.Len("fUsrID"))//앱에서는 fUsrID도 함께 보낸다.
		{
			if(jpa.Len("fPcPwd"))
			{
				qs.SetSQL(L"\
select @FIELDS from tuser u WHERE u.fUsrID = @fUsrID AND u.fLoginID = @fLoginID AND u.fPcPwd = @fPcPwd;");
			}
			else if(jpa.Len("fCertCode"))
			{
				qs.SetSQL(L"\
select @FIELDS from tuser u WHERE u.fUsrID = @fUsrID AND u.fLoginID = @fLoginID AND u.fCertCode = @fCertCode;");
			}
		}
		else
		{
			if(jpa.Len("fPcPwd"))
			{
				qs.SetSQL(L"\
select @FIELDS from tuser u WHERE u.fLoginID = @fLoginID AND u.fPcPwd = @fPcPwd;");
			}
			else if(jpa.Len("fCertCode"))
			{
				qs.SetSQL(L"\
select @FIELDS from tuser u WHERE u.fLoginID = @fLoginID AND u.fCertCode = @fCertCode;");
			}
		}

		recSelUser.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		if(recSelUser.NoData())
			jrs("Return") = L"NoData";
		else
		{
			MakeRecsetToJson(recSelUser, jrs);
			jrs("Return") = L"OK";//default
		}
	} CATCH_DB;
	return 0;
}


int ApiSite1::RequestCertCode(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		//AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수

		CStringW fCertCode(KwRandomEnNumStr(6, 1));
		jpa("fCertCode") = fCertCode;

		Quat qs;
		qs.Field(jpa, "fUsrID", TRUE);//필수,
		qs.Field(jpa, "fCertCode", TRUE);//필수,
		qs.JsonToUpdateSetField(jpa, L"fUsrID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		qs.SetSQL(L"UPDATE tuser r SET @SetField WHERE r.fUsrID = @fUsrID;");
		_db.ExecuteSQL(qs);
		jrs("fCertCode") = fCertCode;
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


int ApiSite1::StayKeepalive(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		// 1. 지금 내가 가져 갈것이 있나?
		//	- 예약 전체라든가
		//	- 내 fUsrId로 동시 작업 중인게 있나?
		//	- std::map<wstring, wstring> _fUsrID_fWorkWithID
		//	- fWorkWithID 는 torderwith 테이블에 ,fWorkWithID, fUsrID, fOrderID fState

		/*
HTTP/1.1 200 OK
Connection: Keep-Alive
Content-Encoding: gzip
Content-Type: text/html; charset=utf-8
Date: Thu, 11 Aug 2016 15:23:13 GMT
Keep-Alive: timeout=5, max=1000
Last-Modified: Mon, 25 Jul 2016 04:32:39 GMT
Server: Apache
		*/

		// 1. DB사용 초기화
		KRecordset rs(&_db);

		// 2. request parmameter 분석
		CString sql;
		CString limit = jpa.S("limit");
		if(limit.IsEmpty())
			limit = L"10";
		sql.Format(L"select * from t_uuid limit %s", (PWS)limit);

		// 3. SQL query실행
		BOOL bOpen = rs.OpenSelectFetch(sql);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.

		// 4. SQL Query결과를 JSON으로 리턴
		MakeRecsetToJson(rs, jrs);




		Sleep(20000);



		if(_stackKeep.find(L"admin-0001") != _stackKeep.end())
			jrs("Return") = L"EVENT";
		else
			jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}




void ApiSite1::DeleteLogWhoDid(PWS table, PWS fKey, PWS fUsrIdDel, PWS fKey2)
{
	JObj jde;
	jde("fTable") = table;//param 1
	jde("fKey") = fKey;//param 2

	CString fKey2part;
	if(fKey2)
	{
		jde("fKey2") = fKey2;//param 2.1
		fKey2part.Format(L"fKey2 = %s", fKey2);
	}
	jde("fUsrIdDel") = fUsrIdDel;//param 3

	Quat qsd;
	qsd.Field(jde, "fTable", TRUE);
	qsd.Field(jde, "fKey", TRUE);
	//if(fKey2)// 없으면 안되지
	qsd.Field("fKey2part", fKey2part, 0, 0, 0);//param 2.1
	qsd.Field(jde, "fUsrIdDel", TRUE);
	qsd.SetSQL(L"UPDATE t_dellog SET fUsrIdDel = @fUsrIdDel WHERE fKey = @fKey and fTable = @fTable @fKey2part;");
	_db.ExecuteSQL(qsd);
}

void ApiSite1::CreateTENUM()
{
	Rec(recSelUser);
	//JObj jop(jpa.O("Option"));
	Rec(rec);
	Quat qs;
	//	qs.Field(jpa, "fBizID", TRUE);//필수
	qs.SetSQL(L"SELECT c.TABLE_NAME, c.COLUMN_NAME, \
CONCAT(left(MID(c.COLUMN_TYPE, 6), CHAR_LENGTH(c.COLUMN_TYPE) -6)) fElist, \
c.COLUMN_COMMENT FROM information_schema.COLUMNS c \
WHERE c.TABLE_SCHEMA = 'petme21' AND c.DATA_TYPE = 'enum' AND c.TABLE_NAME NOT LIKE '~%';");
	rec.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
	int nrow = rec.GetRecordCount();
	for(int r = 0; r < nrow; r++)
	{
		CString fused = rec.Cell(r, "TABLE_NAME");
		CString ffield = rec.Cell(r, "COLUMN_NAME");
		CString fNote = rec.Cell(r, "COLUMN_COMMENT");
		CString fElist = rec.Cell(r, "fElist");
		CStringArray ar;
		KwCutByToken(fElist, L",", ar);
		Quat qsi;
		qsi.Field("fused", fused);
		qsi.Field("ffield", ffield);
		fNote = KwQuat(fNote);
		qsi.Field("fNote", fNote);

		for(int i = 0; i < ar.GetCount(); i++)
		{
			CString fCode = ar[i];
			fCode.Trim('\'');
			qsi.Field("fused", fused);
			if(i == 0)
				qsi.Field("fNote", fNote);
			else
				qsi.Field("fNote", L"");
			qsi.Field("fCode", fCode);
			/// DEBUG tenum1 => tenum
			qsi.SetSQL(L"INSERT INTO tenum (fused, ffield, fCode, fKR, fNote, fAutoMsg) VALUES \
	(@fused, @ffield, @fCode, NULL, @fNote, 'inserted') \
	on duplicate key update fAutoMsg = 'exists';");
			try
			{
				_db.ExecuteSQL(qsi);
			}
			catch(CDBException* e)
			{
				CString s; s.Format(L"CDBException:%s - %s\n", e->m_strError, e->m_strStateNativeOrigin);
				KwMessageBox(s);
			}
			catch(KException* e)
			{
				CString s; s.Format(L"KException:%s - %s\n", e->m_strError, e->m_strStateNativeOrigin);
				KwMessageBox(s);
			}
			catch(CException* e)
			{
				auto buf = new TCHAR(1024);
				KAtEnd d_buf([&]() { delete buf; });
				e->GetErrorMessage(buf, 1000);
				TRACE(L"CException:%s - %s %d\n", buf, __FUNCTION__, __LINE__);
				KwMessageBox(buf);
			}

		}

	}
}


void ApiSite1::SelectEnum(JObj& jrs, PWS table, PWS union_add)
{
	CString sTable = table;
	if(sTable[0] != '\'')
		sTable.Format(L"'%s'", table);

	Rec(rece);
	Quat qs;
	qs.Field("fTable", sTable, 1, 0);
	if(union_add == nullptr)
		union_add = L"";//안 넣어주면 없다고 오류 난다.
	qs.Field("union_add", union_add, 0, 0, 0);
	qs.SetSQL(L"SELECT e.fTable, e.fField, e.fCode, e.fKR FROM tenum e WHERE fTable in (@fTable) @union_add");//ORDER BY fField;
	rece.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
	if(!rece.NoData())
		MakeRecsetToJson(rece, jrs, L"tenum");
	//MakeRecsetOneRowToJson(rece, jrs, L"tenum");	// torderschd
}

/// 고객 신청
int ApiSite1::InsertFamily(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		qs.Field(jpa, "fBizID", TRUE);//필수, 상품
		qs.Field(jpa, "fUsrID", TRUE);//필수, 
		qs.Field(jpa, "fClass", TRUE);//필수, 
		qs.Field(jpa, "fState", TRUE);//필수, 
		qs.SetSQL(L"INSERT INTO tbizfamily (fBizID, fUsrID, fClass, fState) VALUES -- \n\
(@fBizID, @fUsrID, @fClass, @fState);");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}
int ApiSite1::UpdateFamily(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		qs.Field(jpa, "fBizID", TRUE);//필수, 상품
		qs.Field(jpa, "fUsrID", TRUE);//필수, 
		qs.Field(jpa, "fClass");//필수, 
		qs.Field(jpa, "fState");//필수, 
		qs.JsonToUpdateSetField(jpa, L"fBizID", L"fUsrID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		//qs.SetSQL(L"UPDATE tmypets r SET @SetField WHERE r.fPetID = @fPetID;");
		qs.SetSQL(L"UPDATE tbizfamily SET @SetField WHERE fBizID = @fBizID AND fUsrID = @fUsrID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


/// 내 반려동물 등록
int ApiSite1::AddMyPet(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		CString fPetID = DevGuid(L"mypet");
		qs.Field("fPetID", fPetID);
		qs.Field(jpa, "fBizID");// 상품
		qs.Field(jpa, "fUsrIdCare", TRUE);//필수, 
		qs.Field(jpa, "fBreedID");// 
		qs.Field(jpa, "fAgeMonth");// 
		qs.Field(jpa, "fName", TRUE);// 
		qs.Field(jpa, "fWeight");// 
		qs.Field(jpa, "fGender");// 
		qs.Field(jpa, "fMemo");// 
		qs.SetSQL(L"INSERT INTO tmypets (\
fPetID, fUsrIdCare, fBreedID, fAgeMonth, fName, fWeight, fGender, fMemo) VALUES  -- \n\
(@fPetID, @fUsrIdCare, @fBreedID, @fAgeMonth, @fName, @fWeight, @fGender, @fMemo);");
		_db.ExecuteSQL(qs);
		jrs("fPetID") = fPetID;

		jpa("fPetID") = fPetID;
		//jpa.Copy(jpa, "fBizID");
		jpa.Copy(jpa, "fUsrID", "fUsrIdCare");
		Fnc_SelectPetList(jpa, jrs, iOp);

		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// 내 반려동물 등록
int ApiSite1::UpdateMyPet(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		qs.Field(jpa, "fPetID", TRUE);//필수,
		qs.Field(jpa, "fBizID");
		qs.Field(jpa, "fUsrIdCare");//필수, 
		qs.Field(jpa, "fBreedID");//
		qs.Field(jpa, "fAgeMonth");// 
		qs.Field(jpa, "fName");// 
		qs.Field(jpa, "fWeight");// 
		qs.Field(jpa, "fGender");// 
		qs.Field(jpa, "fMemo");// 
		qs.JsonToUpdateSetField(jpa, L"fPetID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		qs.SetSQL(L"UPDATE tmypets r SET @SetField WHERE r.fPetID = @fPetID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

int ApiSite1::DeleteMyPet(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		qs.Field(jpa, "fPetID", TRUE);//필수,
		qs.SetSQL(L"DELETE FROM tmypets WHERE r.fPetID = @fPetID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}
#endif // _APIEX



#ifdef _APIEX




/// [App에서 샵개설] -> 업종선택 후 -> 제휴요청 이다.
int ApiSite1::NewBusiness(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		// 서버가 챙겨 줘야 하는 필드
		jpa("fBizID") = DevGuid(L"biz");
		jpa("fState") = L"hide";
		jpa("fStJoin") = L"register";
		jpa("fMemo") = L"최초 사업가입자 신청";

		Quat qs;
		qs.Field(jpa, "fUsrIdReg", TRUE);//필수
		qs.Field(jpa, "fBizID", TRUE);//필수
		qs.Field(jpa, "fBizPath", TRUE);//필수
		qs.Field(jpa, "fForm", TRUE);//필수
		qs.Field(jpa, "fTel", TRUE);//필수
		qs.Field(jpa, "fTel2");
		qs.Field(jpa, "fTel3");
		qs.Field(jpa, "fState", TRUE);//필수
		qs.Field(jpa, "fStJoin", TRUE);//필수
		qs.Field(jpa, "fAnimal", TRUE);//필수
		qs.Field(jpa, "fShowPrice", TRUE);//필수
		qs.Field(jpa, "fBegin", TRUE);//필수
		qs.Field(jpa, "fEnd", TRUE);//필수
		qs.Field(jpa, "fTitle", TRUE);//필수
		qs.Field(jpa, "fDesc");
		qs.Field(jpa, "fMemo");
		qs.Field(jpa, "fAddr");
		qs.SetSQL(L"\
INSERT INTO tbiz(-- \n\
	fBizID, fBizPath, fBizReq, fUsrIdReg, fUsrIdCeo, fUsrIdAdm, fForm, fTel, fTel2, fTel3, fState, fStJoin, fAnimal, fShowPrice, fBegin, fEnd, fTitle, fSubTitle, fDesc, fMemo, fAddr, fLat, fLon, fUsrIdUpdate)-- \n\
	VALUES(NULL, @fBizPath, @fBizID, @fUsrIdReg, NULL, @fUsrIdReg, @fForm, @fTel, @fTel2, @fTel3, @fState, @fStJoin, @fAnimal, @fShowPrice, @fBegin, @fEnd, @fTitle, @fSubTitle, @fDesc, @fMemo, @fAddr, @fLat, @fLon, @fUsrIdReg);\
");
		_db.ExecuteSQL(qs);

		SelectBizToJson(jpa, jrs);
		
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

int ApiSite1::SelectBizToJson(JObj& jpa, JObj& jrs)
{
	Rec(rec);
	Quat qs;
	qs.Field(jpa, "fBizID", TRUE);//필수
	qs.SetSQL(L"SELECT \
fBizReq fBizID, fBizPath, fBizReq, fUsrIdCeo, fUsrIdAdm, fForm, fTel, fTel2, fTel3, fState, fStJoin, fAnimal, fShowPrice, fBegin, fEnd, fTitle, fSubTitle, fDesc, fAddr, fLat, fLon -- \n\
	FROM tbiz WHERE fBizReq = @fBizID;");
	rec.OpenSelectFetch(qs.GetSQL());//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
	if(rec.NoData())
		jrs("Return") = L"No Data";
	else
		MakeRecsetOneRowToJson(rec, jrs, L"tbiz");	// "torder2pet":[{obj},{obj},{},] }
	return 0;
}


/// App에서 샵개설 -> [업종선택] 후 -> 제휴요청 이다.
int ApiSite1::AddBizClass(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Quat qs;
		if(jpa.IsArray("fBizClsCD"))
		{
			auto sarBzClass = jpa.Array("fBizClsCD"); //vector<JSONValue*>
			if(sarBzClass.get() == NULL)
				throw_response(eHttp_Bad_Request, "Need array of biz class code.");
			auto& arBzClass = *sarBzClass.get();
			CString fBizID;
			for(int i = 0; i < arBzClass.size(); i++)
			{
				ShJVal shbc = arBzClass[i];
				JVal& bc = *shbc.get();
				CString sbc = bc.AsString().c_str();
				if(i > 0)
					fBizID += L",";
				fBizID += qs.Qs(sbc, 1, 0);
			}
			qs_FieldSub(qs, fBizID);
		}
		else if(jpa.IsString("fBizClsCD"))
		{
			qs.Field(jpa, "fBizID", TRUE);//필수, 샵
		}

		qs.SetSQL(L"\
INSERT INTO tbizclassin(fBizID, fBIzClsCD)-- \n\
	SELECT @fBizID, c.fBizClsCD FROM tbizclass c WHERE fBizClsCD IN (@inBzCls); -- to c++");
		_db.ExecuteSQL(qs);

		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// [App에서 샵개설] -> 업종선택 후 -> 제휴요청 이다.
int ApiSite1::UpdateBusiness(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Quat qs;
		qs.JsonToUpdateSetField(jpa, L"fBizID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		qs.SetSQL(L"UPDATE tbiz SET @SetField where fBizID = @fBizID;");
		_db.ExecuteSQL(qs);

		SelectBizToJson(jpa, jrs);

		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


/// App에서 샵개설 -> [업종선택] 후 -> 제휴요청 이다.
// 업종선택 하기 위해 리스트 요청한다.
int ApiSite1::ListBizClass(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		KRecordset rs(&_db);
		rs.OpenSelectFetch(L"SELECT * FROM tbizclass bc");
		MakeRecsetToJson(rs, jrs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


/// 하나씩 뺀다.
int ApiSite1::RemoveBizClass(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Quat qs;
		qs.Field(jpa, "fOrdOptID", TRUE);//필수, 샵
		qs.Field(jpa, "fBIzClsCD", TRUE);//필수, 샵
		qs.SetSQL(L"DELETE FROM tbizclass WHERE fBizID = @fBizID and fBIzClsCD = @fBIzClsCD;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// App에서 샵개설 -> 업종선택 후 -> 제휴요청 이다.
int ApiSite1::RequestToJoin(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		// biz기초데잍 생성-TEST.sql 참조.
		Rec(rec1);
		Quat qs;
		qs.Field(jpa, "fBizID", TRUE);//필수, 샵

		qs.SetSQL(L"SELECT fBizClsCD FROM tbizclassin i WHERE i.fBizID = @fBizID;");
		rec1.OpenSelectFetch(qs.GetSQL());//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		if(rec1.NoData()) // 업종 선택도 안하고 제휴를 요청 하다니~
			throw_response(eHttp_Bad_Request, L"No Business Sectors");

		jpa("fStJoin") = L"request";//제휴요청

		qs.Field(jpa, "fUsrIdUpdate", TRUE);//필수, 샵
		qs.Field(jpa, "fStJoin");//
		qs.JsonToUpdateSetField(jpa, L"fBizID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		qs.SetSQL(L"\
UPDATE tbiz SET @SetField where fBizID = @fBizID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// tbiz에 하나 등록 하면서 모든 기초데이터를 샵데이터로 복사한다. 함수 w_GenerateBizData
int ApiSite1::RegisterBiz(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Quat qs;
		CString fBizID = DevGuid(L"biz");
		qs_Field(qs, fBizID);
		qs.Field(jpa, "fUsrIdUpdate", TRUE);//필수, 샵
		//qs.Field(jpa, "fBizID", TRUE);//필수, 샵

		qs.SetSQL(L"CALL w_GenerateBizData(@fUsrIdUpdate, @fBizID);");
		_db.ExecuteSQL(qs);

		Quat qs1;
		jpa("fStJoin") = L"joined";//제휴요청
		qs1.Field(jpa, "fUsrIdUpdate", TRUE);//필수, 샵
		qs1.Field(jpa, "fBizID", TRUE);//필수, 샵
		qs1.Field(jpa, "fStJoin");
		qs1.JsonToUpdateSetField(jpa, L"fBizID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		qs1.SetSQL(L"\
UPDATE tbiz SET @SetField where fBizID = @fBizID;");
		_db.ExecuteSQL(qs1.GetSQL());

		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// PC passwd로 로그인
/// PC 인증 요청
/// PC 인증키로 로그인
int ApiSite1::LoginPC(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		if(!jpa.Has("Option"))
			throw_response(eHttp_Bad_Request, L"Option is not found.");
		JObj jop(jpa.O("Option"));

		Rec(rec1);
		CString action = jop.S("action");
		Quat qs;
		qs.Field(jpa, "fLoginID", TRUE);//필수, 샵

		if(action == L"LoginByPasswd")//jpa.Has("fPcPwd"))
		{
			qs.Field(jpa, "fPcPwd", TRUE);//필수, 샵
			qs.SetSQL(L"SELECT \
u.fUsrID,u.fTel,u.fEmail,u.fNickName,u.fLoginID,u.fnote,u.fOkEmail,u.fOkPush,u.fState -- \n\
 FROM tuser u WHERE fLoginID = @fLoginID AND fPcPwd = @fPcPwd;");
			rec1.OpenSelectFetch(qs.GetSQL());//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
			if(rec1.NoData()) // 업종 선택도 안하고 제휴를 요청 하다니~
				jrs("Return") = L"NoData";
			else
			{
				MakeRecsetToJson(rec1, jrs);
				jrs("Return") = L"OK";
			}
		}
		else if(action == L"LoginByEncode")//jpa.Has("fCertKey"))//앱 인증 코드요청. 받으면 랜덤 코드 넣은 다음
		{
			// 앱에서 인증키 받은후 그 키로 다시 로그인. 앱에서 비번 바꾸는 기능 있음.
			qs.Field(jpa, "fUsrID");//필수, 샵
			qs.Field(jpa, "fCertKey");//필수, 샵
			qs.SetSQL(L"SELECT \
u.fUsrID,u.fTel,u.fEmail,u.fNickName,u.fLoginID,u.fnote,u.fOkEmail,u.fOkPush,u.fState -- \n\
FROM tcertification c JOIN tuser u ON u.fUsrID = c.fUsrID WHERE fUsrID = @fUsrID AND fCertKey = @fCertKey;");
			rec1.OpenSelectFetch(qs.GetSQL());//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
			if(rec1.NoData()) // 업종 선택도 안하고 제휴를 요청 하다니~
				jrs("Return") = L"NoData";
			else
			{
				MakeRecsetToJson(rec1, jrs);
				jrs("Return") = L"OK";
			}
		}
		else if(action == L"RequestCertKey") // 인증 요청하면 앱에서 RequestCertificatedKey
		{
			CString fCertKey(KwRandomEnNumStr(6, 1));
			qs_Field(qs, fCertKey);
			qs.Field(jpa, "fUsrID");//필수, 샵
			qs.SetSQL(L"INSERT INTO tcertification (fUsrID,fCertKey,fLoginID) -- \n\
	SELECT @fUsrID, @fCertKey, @fLoginID;");
			_db.ExecuteSQL(qs);//일단 메뉴에서 인증키 요청 하면 최근꺼
			jrs("Return") = L"OK";
		}
		else
			throw_response(eHttp_Bad_Request, L"Unknown action requestd.");
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// <summary>
/// 앱에서 인증키 요청. 나중에 푸시로 자동 실행. PC에서 인증 요청 하고 
/// 앱에서 인증키 요청 하면 최근에 요청한 것이 뜬다.
/// 이때 30초 안에 PC에서 로그인 한다.
/// </summary>
int ApiSite1::GetCertificatedKey(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Rec(rec1);
		Quat qs;
		qs.Field(jpa, "fLoginID", TRUE);//필수, 샵
		qs.Field(jpa, "fUsrID", TRUE);//필수, 샵
		qs.Field(jpa, "fCertKey", TRUE);//필수, 샵
		// fConsumed 가 널인 가장 최근거 1개를 가져 간다.
		qs.SetSQL(L"SELECT * FROM tcertification WHERE fPcLogID = @fLoginID AND fConsumed IS NULL ORDER BY fCreated DESC LIMIT 1;");
		rec1.OpenSelectFetch(qs.GetSQL());//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		if(rec1.NoData()) // 업종 선택도 안하고 제휴를 요청 하다니~
			throw_response(eHttp_Not_Found, L"인증 요청한 적이 없습니다.");
		MakeRecsetToJson(rec1, jrs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


int ApiSite1::ChangePasswordForPC(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Rec(rec1);
		Quat qs;
		qs.Field(jpa, "fUsrID", TRUE);//필수, 샵
		qs.Field(jpa, "fPcPwd", TRUE);//필수, 샵
		// fConsumed 가 널인 가장 최근거 1개를 가져 간다.
			// 앱에서 인증키 받은후 그 키로 다시 로그인. 앱에서 비번 바꾸는 기능 있음.
		qs.Field(jpa, "fUsrID");//필수, 샵
		qs.Field(jpa, "fPcPwd");//필수, 샵
		qs.SetSQL(L"UPDATE tuser SET fPcPwd = @fPcPwd WHERE fUsrID = @fUsrID;");
		_db.ExecuteSQL(qs);//일단 메뉴에서 인증키 요청 하면 최근꺼

		/// 바꾸고 나서 다시 읽어가?
//		qs.SetSQL(L"SELECT \
//u.fUsrID,u.fTel,u.fEmail,u.fNickName,u.fPcLogID,u.fnote,u.fOkEmail,u.fOkPush,u.fState -- \n\
// FROM tuser u WHERE fUsrID = @fUsrID;");
//		rec1.OpenSelectFetch(qs.GetSQL());//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
//		if(rec1.NoData()) // 업종 선택도 안하고 제휴를 요청 하다니~
//			jrs("Return") = L"NoData";
//		else
//		{
//			MakeRecsetToJson(rec1, jrs);
//			jrs("Return") = L"OK";
//		}
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


/// <summary>
/// 사용권한을 체크 한다. 어느 사용자가 특정 샵 데이터에 권한이 있는가.
/// 없으면 throw하여 클라이언트 까지 전달
/// </summary>
/// <param name="fUsrID">권한 체크할 사용자</param>
/// <param name="fBizID">권한 대상 샵</param>
/// <throw>403 Forbidden</param>
void ApiSite1::AccessRight(JObj& jpa, int iOp)
{
	try
	{
		if(iOp & eFncAccess)
		{
			if(!jpa.IsObject("Access"))
				throw_response(eHttp_Bad_Request, L"Data for Acess not found! Acess:{fUsrID:, fBizID:}");//403 Forbidden 401과 다른 점은 서버가 클라이언트가 누구인지 알고 있습니다. 401 Unauthorized 
			JObj& jacs = *jpa.Obj(L"Access").get();

			Rec(rec);
			Quat qs;
			qs.Field(jacs, "fUsrID", TRUE);//필수, user
			qs.Field(jacs, "fBizID", TRUE);//필수, 샵
			qs.Field("fCharge", "manager");

			qs.SetSQL(L"\
SELECT COUNT(*) FROM -- \n\
	(	SELECT b.fBizID FROM tbiz b  -- \n\
			JOIN tbizstaff f ON  f.fCharge = @fCharge AND f.fUsrIdStf = @fUsrID -- \n\
			JOIN tbiz bu ON bu.fBizID = f.fBizID -- \n\
			WHERE INSTR(b.fBizPath, CONCAT(bu.fBizPath, bu.fBizID, '/')) OR b.fBizID = bu.fBizID -- \n\
	) ds WHERE fBizID = @fBizID;");
			rec.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
			int count = rec.CellI(0, 0);
			if(count <= 0)
				throw_response(eHttp_Unauthorized, L"Forbidden (권한이 없습니다.)");//403 Forbidden 401과 다른 점은 서버가 클라이언트가 누구인지 알고 있습니다. 401 Unauthorized 
		}
	} CATCH_DB;
}

int ApiSite1::ListBizPrice(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess
	
		Rec(rec1);
		Quat qs;
		qs.Field(jpa, "fUsrID", TRUE);//필수, 샵
		qs.Field(jpa, "fBizID", TRUE);//필수, 샵

		qs.SetSQL(L"SELECT c.fPrcID,c.fProdID,c.fBrdSzCD,c.fMinWeit,c.fMaxWeit,c.fPrice,c.fCurrency,c.fShow -- \n\
	FROM tpricebiz c JOIN tproductbiz p ON p.fProdID = c.fProdID AND p.fBizID = @fBizID AND c.fShow = 'show';");
		rec1.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		if(rec1.NoData()) // 업종 선택도 안하고 제휴를 요청 하다니~
			jrs("Return") = L"No Data";
		else //throw_response(eHttp_Bad_Request, L"Error in shedule.");
			MakeRecsetToJson(rec1, jrs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

int ApiSite1::UpdateBizPrice(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		qs.JsonToUpdateSetField(jpa, L"fPrcID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		qs.SetSQL(L"UPDATE tpricebiz r SET @SetField WHERE r.fPrcID = @fPrcID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}
int ApiSite1::DeleteBizPrice(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		qs.Field(jpa, "fPrcID", TRUE);//필수, 샵
		qs.SetSQL(L"DELETE FROM tpricebiz WHERE r.fPrcID = @fPrcID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

int ApiSite1::AddBizPrice(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		CString fPrcID = DevGuid(L"price");
		qs_Field(qs, fPrcID);

		qs.Field(jpa, "fUsrID", TRUE);//필수, 수정한사람
		qs.Field(jpa, "fProdID", TRUE);//필수, 상품
		qs.Field(jpa, "fBrdSzCD", TRUE);//필수, small, large...
		qs.Field(jpa, "fMinWeit", TRUE);//필수, 최저 체중, 
		qs.Field(jpa, "fMaxWeit", TRUE);//필수, 최고 체중
		qs.Field(jpa, "fPrice", TRUE);//필수, 샵
		qs.Field(jpa, "fCurrency");//NULL
		qs.Field(jpa, "fShow");//NULL
		qs.SetSQL(L"INSERT INTO tpricebiz (\
fPrcID,fProdID,fBrdSzCD,fMinWeit,fMaxWeit,fPrice,fCurrency,fShow,fUsrID -- \n\
) VALUES (@fPrice WHERE r.fPrcID = @fPrcID;");
		_db.ExecuteSQL(qs);
		jrs("fPrcID") = fPrcID;
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

void ApiSite1::AddStaff(JObj& jpa, JObj& jrs)
{
	Quat qs;
	qs.Field(jpa, "fUsrIdStf");//필수 아님. 가상담당-유기명
	qs.Field(jpa, "fBizID", TRUE);//필수
	qs.Field(jpa, "fState");//
	qs.Field(jpa, "fCharge");//
	qs.Field(jpa, "fPosition");//
	qs.Field(jpa, "fDesc");//
	qs.Field(jpa, "fNote");//관리자용
	qs.SetSQL(L"INSERT INTO tbizstaff(fUsrIdStf, fBizID, fState, fCharge, fPosition, fDesc, fNote) VALUES -- \n\
(@fUsrIdStf, @fBizID, @fState, @fCharge, @fPosition, @fDesc, @fNote);");
	//('staff-a021', 'biz-0002', 'ON', 'manager', '스탭1', '테스트 생성자', 'AccessRight테스트'),
	_db.ExecuteSQL(qs);
	jrs("fUsrIdStf") = jpa.S("fUsrIdStf");
	jrs("fBizID") = jpa.S("fBizID");
}


int ApiSite1::AddStaff(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		_db.TransBegin();// insert가 2개 이상이니 트랜잭션 있어야.

		AddStaff(jpa, jrs);
		AddVirtualCharge(jpa, jrs);
		jrs("Return") = L"OK";

		_db.TransCommit();
	} CATCH_DBTR;
	return 0;
}
void ApiSite1::DelStaff(JObj& jpa, JObj& jrs)
{
	Quat qs;
	qs.Field(jpa, "fUsrIdStf", TRUE);//필수
	qs.Field(jpa, "fBizID", TRUE);//필수
	qs.SetSQL(L"DELETE FROM tbizstaff WHERE fUsrIdStf = @fUsrIdStf AND fBizID = @fBizID;");
	_db.ExecuteSQL(qs);
}
int ApiSite1::DelStaff(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		_db.TransBegin();// insert가 2개 이상이니 트랜잭션 있어야.

		DelStaff(jpa, jrs);
		//스탭은 삭제 되어도 가상담당을 삭제 하면 이전 기록에 영향을 미치기 때문에 삭제 하면 안된다.
		DelVirtualCharge(jpa, jrs);

		_db.TransCommit();
	} CATCH_DBTR;
	return 0;
}
int ApiSite1::UpdateStaff(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		qs.Field(jpa, "fUsrIdStf", TRUE);//필수, 상품
		qs.Field(jpa, "fBizID", TRUE);//필수- 유기명 담당자
		qs.JsonToUpdateSetField(jpa, L"fUsrIdStf", L"fBizID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		qs.SetSQL(L"UPDATE tbizstaff r SET @SetField WHERE r.fUsrIdStf = @fUsrIdStf AND fBizID = @fBizID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// <summary>
/// 타임테이블에 컬럼. 가상담당.
/// </summary>
void ApiSite1::AddVirtualCharge(JObj& jpa, JObj& jrs)
{
	Quat qs;
	CString fVChrgID = DevGuid(L"vcharge");
	qs_Field(qs, fVChrgID);

	qs.Field(jpa, "fUsrID");//필수 아님. 가상담당-유기명
	qs.Field(jpa, "fBizID", TRUE);//필수, 상품
	qs.SetSQL(L"INSERT INTO tbizvirtual(fVChrgID, fUsrID, fBizID) VALUES -- \n\
(@fVChrgID, @fUsrID, @fBizID);");
	_db.ExecuteSQL(qs);
	jrs("fVChrgID") = fVChrgID;

}
int ApiSite1::AddVirtualCharge(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		AddVirtualCharge(jpa, jrs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}
void ApiSite1::DelVirtualCharge(JObj& jpa, JObj& jrs)
{
	Quat qs;
	Rec(rec4);
	qs.Field(jpa, "fVChrgID", TRUE);//필수, 가상담당자 ID
	qs.SetSQL(L"SELECT s.fBeginTo FROM torderschd s WHERE s.fVChrgIdWrk = @fVChrgID -- \
 AND s.fTimeUse = 'occupy';"); //AND s.fBeginTo >= NOW() 과거꺼가 있어도 삭제 안한다.
	rec4.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
	if(!rec4.NoData())
	{
		jrs("Message") = L"담당한 일정이 있어 삭제 되지 않습니다.";
		return;
	}
	// 담당 예약이 없는 경우만 삭제
	qs.SetSQL(L"DELETE FROM tbizvirtual WHERE fVChrgID = @fVChrgID;");
	_db.ExecuteSQL(qs);
}

int ApiSite1::DelVirtualCharge(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		DelVirtualCharge( jpa, jrs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}
int ApiSite1::UpdateVirtualCharge(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		qs.Field(jpa, "fVChrgID", TRUE);//필수, 상품
		qs.Field(jpa, "fUsrID", TRUE);//필수- 유기명 담당자
		qs.JsonToUpdateSetField(jpa, L"fVChrgID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		qs.SetSQL(L"UPDATE tbizvirtual r SET @SetField WHERE r.fVChrgID = @fVChrgID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


/// <summary>
/// 으늘 하루 타임 테이블. 오더 할때도 필요.
/// 스탭별로 주는 것은 아래가 통한다.
/// deprecated
/// </summary>
int ApiSite1::GetTodaySchduleTotal(JObj& jpa, JObj& jrs, int iOp)
{
	ASSERT(0);// deprecated
	try
	{
		Quat qs;
		if(jpa.S("fAllStaff") == L"every")// 모두 합쳐서 준다. 즉, 우리샵은 디자이너 별로 안해요.
		{
			return GetTodaySchdule(jpa, jrs, iOp);
		}
		Rec(rec1);

		jpa("fTaskID") = DevGuid(L"task");
		qs.Field(jpa, "fTaskID", TRUE);//필수, 샵
		qs.Field(jpa, "fBizID", TRUE);//필수, 샵
		qs.Field(jpa, "fUsrID", TRUE);//필수, 샵
		qs.Field(jpa, "fDay", TRUE);//필수, 샵
		qs.SetSQL(L"CALL w_select_today(@fTaskID, @fBizID, @fUsrID, @fDay, @fAllStaff);");
		rec1.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		if(rec1.NoData()) // 업종 선택도 안하고 제휴를 요청 하다니~
			throw_response(eHttp_Bad_Request, L"Error in shedule.");
		MakeRecsetToJson(rec1, jrs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

int ApiSite1::GetTodaySchdule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Rec(rec1);
		Rec(recU);
		//CMFCServerView::OnBnClickedTestapi()
		jpa("fAllStaff") = tchsame(jpa.S("fAllStaff"), L"every") ? 1 : 0;
		
		Quat qs;
		jpa("fTaskID") = DevGuid(L"task");
		qs.Field(jpa, "fTaskID", TRUE);//필수, 샵

		qs.Field(jpa, "fAllStaff", TRUE);//필수, 샵
		qs.Field(jpa, "fBizID", TRUE);//필수, 샵
		qs.Field(jpa, "fUsrID", TRUE);//필수, 샵
		qs.Field(jpa, "fDay", TRUE);//필수, 샵
		//qs.Field(jpa, "fOffOn");// NULL이 들어 가야 하는데. sql문에 NULL 상수 처리
		int fAllStaff = jpa.I("fAllStaff");

		qs.SetSQL(L"\
SELECT v.fVChrgID, v.fUsrID, f.fOrderSched, f.fPosition, u.fTel, u.fNickName, f.fAvailable, f.fCharge, v.fBizID -- \n\
	FROM tbizvirtual v  -- \n\
	LEFT JOIN tbizstaff f ON v.fUsrID = f.fUsrIdStf AND f.fBizID = v.fBizID -- \n\
	LEFT JOIN tuser u ON u.fUsrID = v.fUsrID -- \n\
	WHERE v.fBizID = @fBizID;");
		recU.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.

		qs.SetSQL(L"\
call w_AllScheduleToTemp(@fTaskID, NULL, @fBizID, @fUsrID, @fDay, 1, 0);");
		_db.ExecuteSQL(qs);//,fLat,fLon,fUpdated,

		qs.SetSQL(L"\
SELECT s.fIDX,s.fOrder,s.fSchedEvent,s.fSrcDesc,s.fBizID,s.fVChrgID,s.fOffBegin,s.fOffEnd,s.fOff, s.fSchedOrgID, s.fMode,s.fRepeat,s.fNote,s.fUsrID -- \n\
	FROM t_m_allsched s  -- \n\
	WHERE fTaskID = @fTaskID ORDER BY fBizID, fOrder;");

		rec1.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		if(rec1.NoData()) // 업종 선택도 안하고 제휴를 요청 하다니~
			throw_response(eHttp_Bad_Request, L"Error in shedule.");
		/// 반드시 fOff가 널(공통사항)이 먼저 오도록 한다. fVChrgID 는 어차피 모아서 나중에 합치므로
		/// 그래야 공통값으로 다 셋팅 하고 나서 각자 일정을 셋팅 하도록 유도 한다.

		int minu = MinuteForCut30;// 30; // work unit minute : 30분 간격을 일단위로 전제.
		int m48 = NumberOfTimeCut48;// 24 * (60 / minu); // 배열 갯수 48개

		CBiz cbiz;//biz는 하나다. VChrg가 여러개 이지.
		auto bz = &cbiz;//SearchScheduleByLocation_off_1랑 맞출려고
		int fOff_10 = 0;
		CString fVChrgTotal = L"total";

		for(int r = 0; r < recU.RowSize(); r++)
		{
			CString vch = recU.Cell(r, "fVChrgID");
			if(!vch.IsEmpty())// null이면 공통일정이므로 개인전체를 의미 한다.
				bz->AddVirtualCharge(vch, r);
		}

		for(int r = 0; r < rec1.RowSize(); r++)
		{
			ScheduleToTimeTable(rec1, bz, r);
		}
		//지금까지 같은 회사의 디자이너들 일정을 합친 후 값을 이전 값에 and 한다.
		CBiz bzt0;//합친거는 따로 만든다. 
		auto bzt = &bzt0; // 검색 쪽 소스와 비슷 하게 하기위해
		bzt->AddVirtualCharge(fVChrgTotal);

		if(fAllStaff == 0)
		{
			SumOfEachVCharged(bz, bzt);
		}

		auto pbz = fAllStaff == 0 ? bzt : bz;

		JObj jtbl;
		CTime t = jpa.T("fDay");
		CTimeSpan sp(0, 0, minu, 0);

		JObj jocc;
		JObj jchr;
		for(auto& [kVChrgID, vcObj] : pbz->_mapVcObj) // total이면 vc는 하나 이겠지.
		{
			JObj jvinf;
			jvinf("fVChrgID"   ) = vcObj->fVChrgID;
			if(vcObj->rowU >= 0) //fAllStaff == 1)
			{
				recU.SetJsonD(jvinf, vcObj->rowU, "fOrderSched");//정렬키
				recU.SetJsonS(jvinf, vcObj->rowU, "fUsrID");//사용자
				recU.SetJsonS(jvinf, vcObj->rowU, "fBizID");//사용자
				recU.SetJsonS(jvinf, vcObj->rowU, "fPosition");//직위
				recU.SetJsonS(jvinf, vcObj->rowU, "fTel");//전화
				recU.SetJsonS(jvinf, vcObj->rowU, "fNickName");//닉네임
			}

			jchr(kVChrgID.c_str()) = jvinf;//fVChrgID
			///jchr.SetMove(kVChrgID.c_str(), jvinf);// jvinf에는 IsObject가 없고 다 IsString 이므로 굳이 Move할 필요 없다.
			
			JArr jarr;//JSONArray
			CTime t00(t.GetYear(), t.GetMonth(), t.GetDay(), 0, 0, 0);
			for(int i = 0; i < vcObj->_arCell.GetCount(); i++)
			{
				auto sl = vcObj->_arCell[i];

				JObj robj;//t_m_timeday 만든다.
				CString fTime;
				KwCTimeToString(t00, fTime);
				robj("fTime") = fTime;//30분단위 하루시간
				robj("fOff") = sl->fOff;// sl->fOff; 1이면 점유, 0이면 예약가능
				if(sl->fPause.GetLength() > 0)
					robj("fPause") = sl->fPause;// sl->fOff; 1이면 점유, 0이면 예약가능
				robj("fNote") = sl->fNote;//가공된 노트
				//robj("fVChrgID") = vcObj->fVChrgID;
				if(sl->rowS >= 0)//일정이 있는 경우만 0,1,2,... 이다.
				{
					rec1.SetJsonS(robj, sl->rowS, "fIDX");//unique key
					rec1.SetJsonS(robj, sl->rowS, "fSchedOrgID");//일정 원천 키 NULL , 
					rec1.SetJsonS(robj, sl->rowS, "fSchedEvent");//일정 관련키 예: torderschd.fOrderID
					rec1.SetJsonS(robj, sl->rowS, "fMode");//closed, schedule, order...
					rec1.SetJsonS(robj, sl->rowS, "fRepeat");//once, dayly, monthly..
					rec1.SetJsonS(robj, sl->rowS, "fSchedOrgID");//일정원천키
					rec1.SetJsonS(robj, sl->rowS, "fOffBegin");//시작
					rec1.SetJsonS(robj, sl->rowS, "fOffEnd");//끝
					rec1.SetJsonS(robj, sl->rowS, "fVChrgID");//담당. 컬럼이 아니다.진짜가상담당. NULL이면 공통 일정.
					rec1.SetJsonS(robj, sl->rowS, "fUsrID");//담당. 진짜 사람.
				}
				KTrace(L"%s\t%d\t%s\n", fTime, sl->fOff, sl->fNote);
				jarr.Add(robj);// new JSONValue(*robj.get()));

				t00 += sp;
			}
			jocc(kVChrgID.c_str()) = jarr;//fVChrgID
		}//);

		//jrs("occupy") = jocc;
		//jrs("charge") = jchr;
		jrs(L"occupy") = jocc;//안에 object나 array가 있으면 SetMove가 효율적이다.
		jrs(L"charge") = jchr;

		jrs("fMinuteForCut30") = MinuteForCut30;
		jrs("fNumberOfTimeCut48") = NumberOfTimeCut48;
		jrs("Return") = L"OK";
	} CATCH_DB;

	return 0;
}

/// 타임테이블 화면에서 마우스 조작시 불려짐
int ApiSite1::UpdateTodaySchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		qs.Field(jpa, "fSchedOrgID", TRUE);//필수, torderschd.fOrdShedID tbizschedule.fSchdID
		qs.Field(jpa, "fMode", TRUE);//필수, order | schedule
		qs.Field(jpa, "fVChrgID");//필수- 가상 담당자
		qs.Field(jpa, "fOffBegin", TRUE);//필수- begin
		qs.Field(jpa, "fOffEnd", TRUE);//필수- end
		qs.Field(jpa, "fUsrID");//유기명 담당자 : NULL 일수 있다.
		qs.Field(jpa, "fUsrIdUpt", TRUE);// 변경한 사람

		CTime tb = jpa.T("fOffBegin");
		CTime te = jpa.T("fOffEnd");
		CTime nt = CTime(te.GetYear(), te.GetMonth(), te.GetDay(), 0, 0, 0);
		if(tb < nt && nt < te) /// 자정 넘어 갔다.
			throw_response(eHttp_Bad_Request, "종료 시간이 자정 넘어 갔슴.");
		
		CString fMode = jpa.S("fMode");

		if(fMode == "order") // 예약 torderschd
		{
			JObj jch;
			jch.Copy(jpa, "fOrdShedID", "fSchedOrgID");
			//jch.Copy(jpa, "fUsrIdStf", "fUsrID"); 예약 받은 사람 이므로 Update에 안쓴다.
			jch.Copy(jpa, "fVChrgIdWrk", "fVChrgID");
			jch.Copy(jpa, "fBeginTo", "fOffBegin");
			jch.Copy(jpa, "fEndTo", "fOffEnd");
			jch.Copy(jpa, "fUsrIdUpt");

			Quat qso;
			qso.Field(jch, "fOrdShedID", TRUE);
			//qso.Field(jch, "fUsrIdStf", TRUE);
			qso.Field(jch, "fVChrgIdWrk", TRUE);
			qso.Field(jch, "fBeginTo", TRUE);
			qso.Field(jch, "fEndTo", TRUE);
			qso.Field(jch, "fUsrIdUpt", TRUE);
	

			qso.JsonToUpdateSetField(jch, L"fOrdShedID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
			qso.SetSQL(L"UPDATE torderschd r SET @SetField WHERE r.fOrdShedID = @fOrdShedID;");
			_db.ExecuteSQL(qso);
		}
		else if(fMode == "schedule")// tbizschedule
		{
			JObj jch;
			jch.Copy(jpa, "fSchdID", "fSchedOrgID");
			jch.Copy(jpa, "fUsrID"); //개인 일정이 바뀐다.
			jch.Copy(jpa, "fBeginTo", "fOffBegin");
			jch.Copy(jpa, "fEndTo", "fOffEnd");
			jch.Copy(jpa, "fUsrIdOwn", "fUsrIdUpt");

			Quat qso;
			qso.Field(jch, "fSchdID", TRUE);
			qso.Field(jch, "fUsrID", TRUE);
			qso.Field(jch, "fBeginTo", TRUE);
			qso.Field(jch, "fEndTo", TRUE);
			qso.Field(jch, "fUsrIdOwn", TRUE);
			qso.JsonToUpdateSetField(jch, L"fSchdID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
			qso.SetSQL(L"UPDATE tbizschedule r SET @SetField WHERE r.fSchdID = @fSchdID;");
			_db.ExecuteSQL(qso);
		}
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// 테스트 완료. 화면에서 팝업 메뉴로 삭제
int ApiSite1::DeleteTodaySchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		qs.Field(jpa, "fSchedOrgID", TRUE);//필수, torderschd.fOrdShedID tbizschedule.fSchdID
		qs.Field(jpa, "fMode", TRUE);//필수, order | schedule

		CString fMode = jpa.S("fMode");

		_db.TransBegin();// insert가 2개 이상이니 트랜잭션 있어야.

		if(fMode == "order") // 예약 torderschd
		{
			JObj jch;
			jch.Copy(jpa, "fOrdShedID", "fSchedOrgID");
			jch.Copy(jpa, "fUsrIdUpt");

			Quat qso;
			qso.Field(jch, "fOrdShedID", TRUE);
			qso.JsonToUpdateSetField(jch, L"fOrdShedID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
			qso.SetSQL(L"DELETE FROM torderschd WHERE fOrdShedID = @fOrdShedID;");
			_db.ExecuteSQL(qso);
			
			DeleteLogWhoDid(L"torderschd", jch.S("fOrdShedID"), jch.S("fUsrIdUpt"));
			_db.TransCommit();
		}
		else if(fMode == "schedule")// tbizschedule
		{
			JObj jch;
			jch.Copy(jpa, "fSchdID", "fSchedOrgID");
			jch.Copy(jpa, "fUsrIdOwn", "fUsrIdUpt");

			Quat qso;
			qso.Field(jch, "fSchdID", TRUE);
			qso.SetSQL(L"DELETE FROM tbizschedule WHERE fSchdID = @fSchdID;");
			_db.ExecuteSQL(qso);

			DeleteLogWhoDid(L"tbizschedule", jch.S("fSchdID"), jch.S("fUsrIdOwn"));
	
			_db.TransCommit();
		}
		jrs("Return") = L"OK";
	} CATCH_DBTR;
	return 0;
}


/// 테스트 미완료. 화면에서 팝업 메뉴로 추가
int ApiSite1::AddTodayOrder(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		JObj jch;
		jch.Copy(jpa, "fOrdShedID", "fSchedOrgID");
		jch.Copy(jpa, "fUsrIdStf", "fUsrIdUpt");//예약 받은 사람
		jch.Copy(jpa, "fVChrgIdWrk", "fVChrgID");
		jch.Copy(jpa, "fBeginTo", "fOffBegin");
		jch.Copy(jpa, "fEndTo", "fOffEnd");
		jch.Copy(jpa, "fUsrIdUpt");
		
		Quat qso;
		qso.Field(jch, "fOrdShedID", TRUE);
		qso.Field(jch, "fUsrIdStf", TRUE);
		qso.Field(jch, "fVChrgIdWrk", TRUE);
		qso.Field(jch, "fBeginTo", TRUE);
		qso.Field(jch, "fEndTo", TRUE);
		qso.Field(jch, "fUsrIdUpt", TRUE);

		CString fMode = jpa.S("fMode");

		//qso.JsonToUpdateSetField(jch, L"fOrdShedID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		qso.SetSQL(L"INSERT INTO torderschd (fOrdShedID,fUsrIdStf,fVChrgIdWrk,fBeginTo,fEndTo,fUsrIdUpt) VALUES -- \n\
(@fOrdShedID,@fUsrIdStf,@fVChrgIdWrk,@fBeginTo,@fEndTo,@fUsrIdUpt);");
		_db.ExecuteSQL(qso);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// 테스트 미완료. 화면에서 팝업 메뉴로 추가
int ApiSite1::AddTodaySchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		// t_m_allsched
		qs.Field(jpa, "fSchedOrgID", TRUE);//필수, key
		//qs.Field(jpa, "fVChrgID");//필수- 가상 담당자
		qs.Field(jpa, "fOffBegin", TRUE);//필수- 시작
		qs.Field(jpa, "fOffEnd", TRUE);//필수- 끝
		qs.Field(jpa, "fSrcDesc", TRUE);//필수-

		// tbizschedule
		qs.Field(jpa, "fBizID", TRUE);//필수- 스탭
		qs.Field(jpa, "fUsrID", TRUE);//필수- 스탭
		qs.Field(jpa, "fAttr", TRUE);//필수- 스탭
		qs.Field(jpa, "fRepeat", TRUE);//필수- 스탭
		qs.Field(jpa, "fOff", TRUE);//필수-
		qs.Field(jpa, "fMode", TRUE);//필수- schedule
		qs.Field(jpa, "fOrder", TRUE);//필수-
		//qs.Field(jpa, "fVChrgIdWrk");//

		CString fMode = jpa.S("fMode");

		JObj jch;
		jch.Copy(jpa, "fSchdID", "fSchedOrgID");
		jch.Copy(jpa, "fBizID");
		jch.Copy(jpa, "fUsrID");
		jch.Copy(jpa, "fAttr");
		jch.Copy(jpa, "fRepeat");
		jch.Copy(jpa, "fOff");
		jch.Copy(jpa, "fBeginTo", "fOffBegin");
		jch.Copy(jpa, "fEndTo", "fOffEnd");
		jch.Copy(jpa, "fNote", "fSrcDesc");
		jch.Copy(jpa, "fUsrIdOwn", "fUsrIdUpt");

		Quat qso;
		qso.JsonToUpdateSetField(jch, L"fSchdID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		qso.SetSQL(L"INSERT INTO tbizschedule (fSchdID, fBizID, fUsrID, fAttr, fRepeat, fOff, fBeginTo, fEndTo, fNote, fUsrIdOwn) VALUES -- \n\
(@fSchdID, @fBizID, @fUsrID, @fAttr, @fRepeat, @fOff, @fBeginTo, @fEndTo, @fNote, @fUsrIdOwn);");
//('shedule-0005', 'biz-0002', 'staff-a024', 'private', 'off', '2021-02-10 17:00:00', '2021-02-11 18:00:00', '장기 휴가', 'staff-a024', '2021-03-03 22:57:01');
		_db.ExecuteSQL(qso);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}



/// 아직 테스트 안함. 상세창에서 편집시
int ApiSite1::UpdateOrderSchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		qs.Field(jpa, "fSchedOrgID", TRUE);//필수, 상품
		qs.Field(jpa, "fVChrgID");//필수- 유기명 담당자
		qs.Field(jpa, "fOffBegin", TRUE);//필수- 유기명 담당자
		qs.Field(jpa, "fOffEnd", TRUE);//필수- 유기명 담당자
		qs.Field(jpa, "fUsrID", TRUE);//필수- 변경한 사람

		CString fMode = jpa.S("fMode");

		qs.JsonToUpdateSetField(jpa, L"fOrdShedID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		qs.SetSQL(L"UPDATE tbizvirtual r SET @SetField WHERE r.fVChrgID = @fVChrgID;");
		_db.ExecuteSQL(qs);

		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// 아직 테스트 안함. 상세창에서 편집시
int ApiSite1::UpdateBizSchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Quat qs;
		qs.Field(jpa, "fSchedOrgID", TRUE);//필수, 상품
		qs.Field(jpa, "fVChrgID");//필수- 유기명 담당자
		qs.Field(jpa, "fOffBegin", TRUE);//필수- 유기명 담당자
		qs.Field(jpa, "fOffEnd", TRUE);//필수- 유기명 담당자
		qs.Field(jpa, "fUsrID", TRUE);//필수- 유기명 담당자
		qs.Field(jpa, "fVChrgIdWrk");//
		qs.Field(jpa, "fUsrIdOwn", TRUE);//필수- 유기명 담당자

		CString fMode = jpa.S("fMode");

		JObj jch;
		jch("fOrdShedID") = jpa.S("fSchedOrgID");
		jch("fUsrIdUpt") = jpa.S("fUsrID");
		jch("fVChrgIdWrk") = jpa.S("fVChrgIdWrk");
		jch("fUsrIdUpt") = jpa.S("fUsrID");
		jch("fUsrIdOwn") = jpa.S("fUsrIdUpt");
		Quat qso;
		//qso.Field(

		qs.JsonToUpdateSetField(jpa, L"fSchedOrgID"); // 키를 제외한 필드가 @SetField로 할당 된다. 키는 where에 쓰이고
		qs.SetSQL(L"UPDATE tbizvirtual r SET @SetField WHERE r.fVChrgID = @fVChrgID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


int ApiSite1::SelectOrderSchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		//AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Rec(rec);
		Quat qs;
		qs.Field(jpa, "fOrdShedID", TRUE);//필수
		qs.SetSQL(L"SELECT os.fOrdShedID, os.fUsrIdStf, o1.fOrderID, o1.fUsrIdOrd,  t.fPetID, t.fOrder, -- \n\
	o2.fOrdPetID, o2.fPetID, o3.fOrdPrdID, o3.fProdID, o4.fOrdOptID, o4.fPrdOptID, op.fPrdOptID, -- \n\
	os.fSchdType, os.fWorkStat, os.fTimeUse, o1.fState, os.fBeginTo, os.fEndTo, -- \n\
	us.fNickName fNameStf, uo.fNickName fNameOrd, t.fName fNamePet, p.fName fNamePrd, op.fName fNameOpt -- \n\
	,((o1.fOrderID IS NOT NULL) + (o2.fOrdPetID IS NOT NULL) + (o3.fOrdPrdID IS NOT NULL) + (o4.fOrdOptID IS NOT NULL)) fScore -- \n\
	FROM torderschd os  -- \n\
	LEFT JOIN tuser us ON us.fUsrID = os.fUsrIdStf -- \n\
	LEFT JOIN torder1 o1 ON o1.fOrderID = os.fOrderID -- \n\
	LEFT JOIN tuser uo ON uo.fUsrID = o1.fUsrIdOrd -- \n\
	LEFT JOIN torder2pet o2 ON o2.fOrderID = o1.fOrderID -- \n\
	LEFT JOIN tmypets t ON t.fPetID = o2.fPetID -- \n\
	LEFT JOIN torder3prd o3 ON o3.fOrdPetID = o2.fOrdPetID -- \n\
	LEFT JOIN tproductbiz p ON p.fProdID = o3.fProdID -- \n\
	LEFT JOIN torder4opt o4 ON o4.fOrdPrdID = o3.fOrdPrdID -- \n\
	LEFT JOIN tprodoptbiz op ON op.fPrdOptID = o4.fPrdOptID -- \n\
	WHERE fOrdShedID = @fOrdShedID ORDER BY fScore DESC, t.fOrder LIMIT 1;");
		rec.OpenSelectFetch(qs);
		if(!rec.NoData())
		{
			MakeRecsetToJson(rec, jrs, L"torderschd");
			SelectEnum(jrs, L"'torderschd', 'torder1'");// table 여러개 있때는 in ()에 넣을수 있게 quat를 두른다.
			jrs("Return") = L"OK";
		}
		else
			jrs("Return") = L"No Data";
		//throw_response(eHttp_Not_Found, L"일정 정보가 없습니다.");
	} CATCH_DB;
	return 0;
}

int ApiSite1::SelectBizSchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Rec(rec);
		Quat qs;
		qs.Field(jpa, "fSchdID", TRUE);//필수
		qs.SetSQL(L"\
SELECT s.fSchdID, s.fBizID, s.fUsrID, us.fNickName, s.fAttr, s.fRepeat, s.fOff, s.fBeginTo, s.fEndTo, s.fNote -- \n\
	FROM tbizschedule s LEFT JOIN tuser us ON us.fUsrID = s.fUsrID -- \n\
	WHERE fSchdID = @fSchdID;");
		rec.OpenSelectFetch(qs);
		if(!rec.NoData())
		{
			MakeRecsetToJson(rec, jrs, L"tbizschedule");
			SelectEnum(jrs, L"tbizschedule");
			jrs("Return") = L"OK";
		}
		else
			jrs("Return") = L"No Data";
		//throw_response(eHttp_Not_Found, L"일정 정보가 없습니다.");
	} CATCH_DB;
	return 0;
}

int ApiSite1::SelectCustomList(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		//AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Rec(rec);
		Quat qs;
		qs.Field(jpa, "fBizID", TRUE);//필수
		qs.SetSQL(L"SELECT f.fBizID, f.fUsrID, f.fClass, f.fState, u.fTel, u.fEmail, u.fNickName, u.fnote, t.fPetID, t.fBreedID, r.fName fNameBreed , t.fName -- \n\
	FROM tbizfamily f JOIN tuser u ON u.fUsrID = f.fUsrID and fBizID = @fBizID -- \n\
	LEFT JOIN (SELECT t.fPetID, t.fBreedID, t.fName, t.fUsrIdCare, t.fOrder FROM tmypets t GROUP BY fUsrIdCare HAVING t.fOrder = MIN(t.fOrder)) t ON t.fUsrIdCare = f.fUsrID -- \n\
	LEFT JOIN tbreeds r ON r.fBreedID = t.fBreedID;");
		rec.OpenSelectFetch(qs.GetSQL());//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		if(!rec.NoData())
		{
			MakeRecsetToJson(rec, jrs, L"table");	// tbiz
			SelectEnum(jrs, L"'tbizfamily', 'tuser'");
			jrs("Return") = L"OK";
		}
		else
			jrs("Return") = L"No Data";
		//throw_response(eHttp_Not_Found, L"등록된 고객 정보가 없습니다.");
	} CATCH_DB;
	return 0;
}

int ApiSite1::SelectCustomDetail(JObj& jpa, JObj& jrs, int iOp)
{
	JObj jop;
	jop("action") = L"select";
	jpa("Option") = jop;
	//jpa("fProj") = L"petme2";
	return UserManage(jpa, jrs, iOp);
}
int ApiSite1::AddWorkLog(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		static KStdMap<wstring, wstring> smap;
		if(smap.size() == 0)
		{
			smap[L"목록"] = L"list";
			smap[L"상세보기"] = L"detail";
			smap[L"변경"] = L"update";
			smap[L"추가"] = L"insert";
			smap[L"새로생성"] = L"insert";
			smap[L"삭제"] = L"delete";
		};
		CString fMode = jpa.S("fMode");
		wstring fMode2;
		if(!smap.Lookup((PWS)fMode, fMode2))
			fMode2 = fMode;
		CString fMemo = jpa.S("fMemo");
		if(fMemo.GetLength() > 100)
			fMemo = fMemo.Left(100);
		Quat qs; 
		CString fWorkID = DevGuid(L"work");
		qs.Field("fWorkID", fWorkID);
		qs.Field(jpa, "fUsrID", TRUE);//필수, 샵
		qs.Field(jpa, "fBizID", TRUE);//필수, 샵
		qs.Field(jpa, "fObject", TRUE);//필수, 샵
		qs.Field(jpa, "fTitle", TRUE);//필수, 샵
		qs.Field(jpa, "fObjID", TRUE);//필수
		qs.Field(jpa, "fObjID2");//필수
		qs.Field(jpa, "fObjName", TRUE);//필수
		qs.Field(jpa, "fObjName2");//필수
		qs.Field("fMode", fMode2.c_str(), TRUE);//필수
		qs.Field(jpa, "fDevice", TRUE);//필수
		qs.Field(jpa, "fTmWork", TRUE);//필수
		qs.Field("fMemo", fMemo);//필수
		qs.SetSQL(L"\
INSERT INTO tworklog \
( fWorkID,  fUsrID,  fBizID,  fObject,  fTitle,  fObjID,  fObjID2,  fObjName,  fObjName2,  fMode,  fDevice,  fTmWork,  fMemo) VALUES -- \n\
(@fWorkID, @fUsrID, @fBizID, @fObject, @fTitle, @fObjID, @fObjID2, @fObjName, @fObjName2, @fMode, @fDevice, @fTmWork, @fMemo);");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


int ApiSite1::SelectWorkLogList(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		//AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		Rec(rec);
		Quat qs;
		qs.Field(jpa, "fUsrID", TRUE);//필수
		qs.SetSQL(L"SELECT f.* FROM tworklog f WHERE fUsrID = @fUsrID ORDER BY fTmWork DESC limit 50;");
		rec.OpenSelectFetch(qs.GetSQL());
		if(!rec.NoData())
		{
			MakeRecsetToJson(rec, jrs, L"table");
			SelectEnum(jrs, L"tworklog");
			jrs("Return") = L"OK";
		}
		else
			jrs("Return") = L"No Data";
	} CATCH_DB;
	return 0;
}




// 스케줄 읽어서 타임테이블에 배치
void ApiSite1::ScheduleToTimeTable(KRecordset& rec1, CBiz* bz, int r)
{
	int minu = MinuteForCut30;// 30; // work unit minute : 30분 간격을 일단위로 전제.
	int m48 = NumberOfTimeCut48;// 24 * (60 / minu); // 배열 갯수 48개

	auto fNote = rec1.Cell(r, "fNote");
	auto fMode = rec1.Cell(r, "fMode");
#ifdef DEBUG
	auto stb = rec1.Cell(r, "fOffBegin");
	auto ste = rec1.Cell(r, "fOffEnd");
#endif // DEBUG

	auto tb = rec1.CellT(r, "fOffBegin");
	auto te = rec1.CellT(r, "fOffEnd");
	// 시간 구간으로 시간매트릭스 index 0 ~ 47
	int ib = tb.GetHour() * (60 / minu) + (tb.GetMinute() / minu);//=(HOUR(B46) * (60/A46)) + MINUTE(B46)/A46

	if(fMode == L"closed")
		_break;
	// 날짜 지나 가면 뒤에 00:00 을 24:00 으로 고친다.
	//if(KwCompareDate(tb, te) > 0)// && KwCompareTimeOnly(tb, te) >= 0)
	int cmpDate = KwCompareDate(tb, te);
	int hourEnd = cmpDate < 0 && te.GetHour() == 0 ? 24 : te.GetHour();/// 끝 시각이 다음날이고 0시이면 24로 한다.
	int ie = hourEnd * (60 / minu) + (te.GetMinute() / minu);
	if(48 < ie)
		throw_response(eHttp_Bad_Request, "fOffEnd time is invalid.");

	CStringW sfOff = rec1.Cell(r, "fOff");
	int fOff = sfOff == L"off" ? 1 : sfOff == L"ON" ? 0 : -1;//점유 되었으면 off이므로 1로 한다.
	int fOff_10 = 1;// and 이므로 1로 초기화 해야 한다. or 이면 0

	int fOrder = rec1.CellI(r, "fOrder");
	CStringW fVChrgID = rec1.Cell(r, "fVChrgID");
	bool bEmptyVChrg = fVChrgID.IsEmpty();

#ifdef DEBUG
	KTrace(L"%d\t%s\t%s\t%s\t%s\t%s\n", fOrder, stb, ste, fMode, fVChrgID, fNote);
#endif // DEBUG

	if(fMode == L"holiday")///	--  1. 공휴일 : or 1
	{
		for(int i = 0; i < m48; i++)// 샘플에서는 32,33 만 11
		{
			if(fOff == 1)
				bz->SetPauseTime(fVChrgID, i, L"holiday", r); // ri[i] = fOff;
			//bz->note[i] = fNote;//덮어쓰면 ON(0) 일때도 and 와 같다.
		}
		/// 휴일 off 인것만 읽어 온다. 아니면 데이터 없다.
		// 11111111111111 11111111111111 11111111111111 11111111111111 휴일
		// 00000000000000 00000000000000 00000000000000 00000000000000 평일
	}
	else if(fMode == L"closed") ///	-- 2. 영업외시간 : or 1
	{
		for(int i = ib; i < ie; i++)
		{
			//auto occ = bz->GetOccupy(fVChrgID, i);
			if(fOff == 1)
				bz->SetPauseTime(fVChrgID, i, L"closed", r);//occ | 
		}
	}
	else if(fMode == L"schedule" || fMode == L"order")
	{
		if(bEmptyVChrg) // 샵 전체 일정
		{
			for(int i = ib; i < ie; i++)
			{
				if(fOff == 0) ///	-- 3. 공통일정 중 특별오픈 : ON or 0 : 특별출근자가 있어야 0
					bz->SetPauseTime(fVChrgID, i, L"open", r);//occ | 
					//vff |= fOff;
				else          ///	-- 4. 공통일정 중 off : or 1
				{
					auto occ = bz->GetOccupy(fVChrgID, i);
					auto vff = occ;
					vff |= fOff;
					bz->SetOccupy(fVChrgID, i, vff, fNote, r);
				}
			}// 아직 스탭들 or 조합 한거 전체에 안 합침.
		}
		else // 스탭 개인 일정
		{
			for(int i = ib; i < ie; i++)
			{
				if(fOff == 0)   ///	-- 5. 개인일정 특별출근 : ON set 0
					bz->SetPauseTime(fVChrgID, i, L"work", r);//occ | 
				else
				{
					auto occ = bz->GetOccupy(fVChrgID, i);
					auto vff = occ;
					vff |= fOff;///	-- 6. 개인일정 중 off : or 1
					bz->SetOccupy(fVChrgID, i, vff, fNote, r); // 회사일정 | 자기일정
				}
			}
			bz->nUsr++;
		}
	}
}

// 각 개인 값들을 조합
void ApiSite1::SumOfEachVCharged(CBiz* bz, CBiz* bzt)
{
	int minu = MinuteForCut30;// 30; // work unit minute : 30분 간격을 일단위로 전제.
	int m48 = NumberOfTimeCut48;// 24 * (60 / minu); // 배열 갯수 48개
	CString fVChrgTotal = L"total";

	for(int i = 0; i < m48; i++)
	{
		auto occ = 0;// bzt->GetOccupy(fVChrgTotal, i);// 토탈 이전 값에 and 하려고
		int iv = 0;
		for(auto& [vc, vcObj] : bz->_mapVcObj)//각 데이터 읽어서 : 가로 방향 루프
		{
			auto cell = vcObj->_arCell.GetAt(i);
			if(iv == 0) // 첫 항목은 set
				occ = cell->fOff;
			else
				occ &= cell->fOff;// 두번째 부터 and
			iv++;
		}
		bzt->SetOccupy(fVChrgTotal, i, occ, occ ? L"" : L"예약 가능", -1);// 따로 토탈에 넣는다.
	}
}




void CBiz::AddVirtualCharge(PWS vchrg, int row)
{
	ASSERT(tchlen(vchrg) > 0);
	KVCharg* vcObj = nullptr;
	if(!_mapVcObj.Lookup(vchrg, vcObj))
	{
		vcObj = new KVCharg();/// 여기서 48개 KSlice 가 할당 된다.
		vcObj->fVChrgID = vchrg;
		vcObj->rowU = row;
		_vci[vchrg] = (int)_mapVcObj.size();// 넣기 전에 size가 인덱스 번호 _mx에 쓰기 위해
		_mapVcObj.SetAt(vchrg, vcObj);
	}
}

KVCharg* CBiz::getMatrix(PWS kch)
{
	// 가상담당 합산한것은 키를 total로 한다.
	KVCharg* pri = nullptr;
	_mapVcObj.Lookup(kch, pri);
	return pri;
}

void CBiz::SetPauseTime(PWS kvch, int iTime, CString occupy, int row)
{
	if(tchlen(kvch) == 0)///이면 모두에게 줘야 한다.
	{
		for(auto& [vch, vcObj] : _mapVcObj)
		{
			auto sl = vcObj->_arCell[iTime];
			sl->fPause = occupy;
		}//);
	}
	else // 특정 fVChrgID 에게만 점유 여부를 셋팅 한다.
	{
		auto vcObj = getMatrix(kvch);
		if(vcObj)
		{
			auto sl = vcObj->_arCell[iTime];
			sl->fPause = occupy;
		}
	}
}

void CBiz::SetOccupy(PWS kvch, int iTime, int occupy, CString note, int row)
{
	int lhd = tchlen("00:00  ");
	// fVChrgID 가 널이면 공통 일정이니 모든 컬럼에 값이 들어 간다.
	if(tchlen(kvch) == 0)///이면 모두에게 줘야 한다.
	{
#ifdef _DEBUG
		for(int i = 0; i < _mapVcObj.size(); i++)
		{
			_mx[iTime][i + lhd] = occupy ? '1' : '0';
			tchcpy(_mx[iTime] + lhd + 10, (PAS)CStringA(note));
		}
#endif
		for(auto& [vch, vcObj] : _mapVcObj)
			//_mapVcObj.for_loop([&](TVcstr k, KVCharg* arsl) -> void
		{
			auto sl = vcObj->_arCell[iTime];
			sl->fOff = occupy;
			sl->fNote = note;
			sl->rowS = row;
		}//);
	}
	else // 특정 fVChrgID 에게만 점유 여부를 셋팅 한다.
	{
#ifdef _DEBUG
		int iv = _vci[kvch] + lhd;//
		_mx[iTime][iv] = occupy ? '1' : '0';
		tchcpy(_mx[iTime] + lhd + 10, (PAS)CStringA(note));
#endif
		auto vcObj = getMatrix(kvch);
		if(vcObj)
		{
			auto sl = vcObj->_arCell[iTime];
			sl->fOff = occupy;
			//auto sl = arsl->GetAt(iTime);
			//sl->fOff = occupy;
			sl->fNote = note;
			sl->rowS = row;
		}
	}
}

int CBiz::GetOccupy(PWS kvch, int iTime)
{
	// fVChrgID 가 널이면 공통 일정이니 모든 컬럼에 값이 들어 간다.
	int occ = 0;
	if(tchlen(kvch) == 0)//이면 모두에게 줘야 한다.
	{
		for(auto& [vch, vcObj] : _mapVcObj)
			//_mapVcObj.for_loop([&](TVcstr kv, TArrSl* arsl) -> void
		{
			auto sl = vcObj->_arCell[iTime];
			occ |= sl->fOff;
		}//);// 굳이 다 읽어서 or 조합 해서 리턴 한다.
	}
	else // 특정 fVChrgID 에게만 점유 여부를 셋팅 한다.
	{
		auto vcObj = getMatrix(kvch);
		if(vcObj)
		{
			auto sl = vcObj->_arCell[iTime];
			//auto sl = arsl->GetAt(iTime);
			occ = sl->fOff;// vcObj->_arCell[iTime];// sl->fOff;
		}
	}
	return occ;
}





int ApiSite1::Fnc_SelectPetList(JObj& jpa, JObj& jrs, int iOp)
{/*
 SelectPetList 할때 fBizID(샵ID)를 주는 이유를 설명 하겠습니다.
1. 일단 안주는 경우는 고객 개인이 요청 했을떄 뿐입니다.
2. 샵에서 요청할 경우는 붙여줘야 그래야 가격을 매길떄 사용 되는 (소형견, 중형견, 대형견, 특수견) 구분을 할수 있습니다.
3. 사이즈 판단이 샵 주관적이기 때문에 샵별로 있습니다.
4. 품종(tbreeds)는 독립적이지만, 품종의 크기 기준(tbreedsizebase) 에 따라, 샵별 품종 크기(tbreedsizebiz) 를 참조 하기 때문에 fBizID가 주어집니다.
5. 이것은 예약시 가격 산정시 크기에 따라 가격이 달라 집니다.
 */
	Rec(rec);
	Quat qs;
	qs.Field(jpa, "fBizID");//필수
	qs.Field(jpa, "fUsrID", TRUE);//필수

	if(jpa.Has("fBizID"))
	{
		qs.SetSQL(L"SELECT t.fPetID, t.fUsrIdCare, t.fBreedID, t.fAgeMonth, t.fName, t.fRegNo, t.fWeight, t.fGender, t.fNeuSurg, t.fOrder, t.fMemo, -- \n\
	r.fName fNameBreed, r.fSpecies, s.fBrdSzCD -- \n\
	FROM tmypets t -- 여기서 주의 fBrdSzCD는 tbreeds 것을 쓰면 안된다.\n\
	JOIN tbreeds r ON t.fBreedID = r.fBreedID -- \n\
	left JOIN tbreedsizebiz s ON s.fBreedID = r.fBreedID AND s.fBizID = @fBizID -- \n\
	WHERE fUsrIdCare = @fUsrID AND fPetID IS NOT NULL ORDER BY t.fOrder;");

	}
	else
	{
		qs.SetSQL(L"SELECT t.fPetID, t.fUsrIdCare, t.fBreedID, t.fAgeMonth, t.fName, t.fRegNo, t.fWeight, t.fGender, t.fNeuSurg, t.fOrder, t.fMemo, -- \n\
	r.fName fNameBreed, r.fSpecies, sb.fBrdSzCD -- \n\
	FROM tmypets t -- \n\
	JOIN tbreeds r ON t.fBreedID = r.fBreedID -- \n\
	JOIN tbreedsizebase sb ON sb.fBrdSzCD = r.fBrdSzCD -- \n\
	WHERE fUsrIdCare = @fUsrID AND fPetID IS NOT NULL ORDER BY t.fOrder; ");

	}
	rec.OpenSelectFetch(qs);
	if(rec.NoData())
	{
		//throw_response(eHttp_Not_Found, L"등록된 펫 정보가 없습니다.");
		jrs("Return") = L"NoData";
	}
	else
	{
		MakeRecsetToJson(rec, jrs, L"table");	// tbiz
		SelectEnum(jrs, L"'tmypets', 'tbreeds'", L"UNION -- \n\
SELECT 'tmypets' fTable, 'fBrdSzCD' fField, s.fBrdSzCD fCode, s.fName fKR FROM tbreedsizebase s");
		jrs("Return") = L"OK";
	}
	return 0;
}
int ApiSite1::SelectPetList(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Fnc_SelectPetList(jpa, jrs, iOp);
		//AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수 SelectPetList
	} CATCH_DB;
	return 0;
}

int ApiSite1::SelectProductList(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		//AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수 SelectPetList
		Rec(rec);
		Quat qs;
		qs.Field(jpa, "fBizID", TRUE);//필수
		//qs.Field(jpa, "fUsrID", TRUE);//필수
		qs.SetSQL(L"SELECT p.fProdID, p.fProdTpCD, p.fName, p.fShow, p.fKind, p.fElapsed, p.fOrder, p.fDesc\
			FROM tproductbiz p WHERE p.fBizID = @fBizID;");
		rec.OpenSelectFetch(qs.GetSQL());//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		if(rec.NoData())
		{
			jrs("Return") = L"NoData";
		}
		else
		{
			MakeRecsetToJson(rec, jrs, L"table");	// tbiz
	//else
	//	throw_response(eHttp_Not_Found, L"등록된 펫 정보가 없습니다.");
			SelectEnum(jrs, L"tproductbiz");
			jrs("Return") = L"OK";
		}
	} CATCH_DB;
	return 0;
}

#endif // _APIEX
