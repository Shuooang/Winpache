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
		// 3. SQL query����
		BOOL bOpen = rs.OpenSelectFetch(sql);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.

		// 4. SQL Query����� JSON���� ����
		//MakeRecsetToJson(rs, jrs);
	// sql ���� ���� idx, path�� �о� �´�.
	// Src �̹��� ������ �о, new �̹�ġ ������ GUID�� ���� �Ѵ�.
	// t_images�� Src�� idx, table, old path, new GUID�� �Բ� ���� �Ѵ�.
	// ������ v2 table�� imageID�� v1 table�� idx+oldTable�� guid�� �־��ָ� �ȴ�.

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
	catch(KException* e)//rethrow �Ѱ��� ó�� �Ѵ�.
	{
		_db.TransRollback();
		this->LogException(e, L"MigrationImage");// L"ImageDownLoad");//������ �Ķ���� ì��� ���Ƽ� ����� ó�� �Ѵ�.
		//JError(jres, e->m_strError, e->_error);
		//rv = e->m_nRetCode;// -100;
		return -1;
	}
	catch(CException* e)
	{//��� KException �� �ٲ㼭 rethrow�ϹǷ� ����� ���� ȣ�� �ϴ� �ǿ� �ɰ��� �����ϼ��ִ�.
		_db.TransRollback();
		CString sError;
		e->GetErrorMessage(sError.GetBuffer(1024), 1024); sError.ReleaseBuffer();
		DWORD err = GetLastError();
		//rv = err;
		if(sError.IsEmpty())
			sError = L"Unknown ImageUpload Error.";
		//JError(jres, sError, err);
		this->LogExcepFN(e);//CException���� FILE, LINE�� ���� ������ �Ķ���� ì��� ����.
		return -2;
	}
	return 0;
}


/// client: CheckUser
int ApiSite1::UserManage(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		// 1. DB��� �ʱ�ȭ
		Rec(recSelUser);

		if(!jpa.Has("Option"))
			throw_response(eHttp_Bad_Request, "Option is not found.");
		JObj jop(jpa.O("Option"));

		Quat qs;//�ʼ�, ����, NULL

		CString fUsrID;
		if(!jpa.Has("fUsrID"))
			fUsrID = DevGuid(L"user");
		else
			fUsrID = jpa.S("fUsrID");

		qs.Field("fUsrID", fUsrID);
		//qs.Field(jpa, "fUsrID");//�ʼ��ƴ� . ������ ��.
		qs.Field(jpa, "fTel");
		qs.Field(jpa, "fEmail");
		qs.Field(jpa, "fAppVer");
		qs.Field(jpa, "fModel");
		qs.Field(jpa, "fPush");
		qs.Field(jpa, "fProj");// , TRUE);//�ʼ� �ϴ� ���ش�.
		qs.Field(jpa, "fLoginID");
		qs.Field(jpa, "fPcPwd");
		qs.Field(jpa, "fCertCode");


		CString action = jop.S("action");

		jrs("Return") = L"OK";//default

		_db.TransBegin();

		if(action == L"select" || action == L"insert" || action == L"update")
		{
			if(action == L"insert" || action == L"update")
			{	// server�����Ŀ� local ���� �ȵ� ��� uuid�� ���� ������ ���Ƿ� �ߺ� �ȴ�.
				qs.SetSQL(L"\
insert into tuser (fUsrID,  fEmail,  fTel, fRecover, fStat,  fAppVer,  fModel,  fPush,  fProj, fLoginID, fPcPwd, fCertCode) values \r\n\
					(@fUsrID, @fEmail, @fTel,     NULL,  'ON', @fAppVer, @fModel, @fPush, @fProj, @fLoginID, @fPcPwd, @fCertCode) \r\n\
	on duplicate key update \n\
		femail = values(fEmail), fTel = values(fTel), fRecover = values(fRecover), fAppVer = values(fAppVer), \n\
		fModel = values(fModel), fPush = values(fPush), fStat = values(fStat) -- \n\
, fLoginID = values(fLoginID), fPcPwd = values(fPcPwd), fCertCode = values(fCertCode)");

				// on duplicate key�ϋ��� UpdateBusiness ó�� JsonToUpdateSetField ���� �ʰ�, sql������ ���� �ϴ�.
				_db.ExecuteSQL(qs);
			}

			qs.SetSQL(L"\
select u.fUsrID, u.fTel, u.fTel2, u.fEmail, u.fNickName, u.fAppVer, u.fModel, u.fPush, u.fProj from tuser u where u.fUsrID = @fUsrID;");
			recSelUser.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
			if(recSelUser.NoData())
				jrs("Return") = L"NoData";
			//throw_response(eHttp_Not_Found, L"����� ���� ����.");
			SelectEnum(jrs, L"tuser");
			MakeRecsetToJson(recSelUser, jrs);
		}
		else if(action == L"delete") // �̰� client�� ��û ��� ����.
		{
			//sql.Format(L"update tuser set fStat = 'off' where fUsrID=%s ", jpa.QS("fUsrID"));
			qs.SetSQL(L"update tuser set fStat = 'off' where fUsrID=@fUsrID;");
			_db.ExecuteSQL(qs);
		}
		else
		{
			throw_response(eHttp_Bad_Request, "Parameter Error");// CResponse1::ResponseForPost ���� catch(KException* e)
		}
		_db.TransCommit();
	} CATCH_DBTR;
	return 0;
}

int ApiSite1::RequestLogin(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		// 1. DB��� �ʱ�ȭ
		Rec(recSelUser);

		Quat qs;//�ʼ�, ����, NULL

		CString fUsrID;
		qs.Field(jpa, "fLoginID", TRUE);
		qs.Field(jpa, "fPcPwd");
		qs.Field(jpa, "fCertCode");
		qs.Field(jpa, "fUsrID");
		qs.Field("FIELDS", L"u.fUsrID, u.fTel, u.fTel2, u.fEmail, u.fNickName, u.fAppVer, u.fModel, u.fPush, u.fProj");
		if(jpa.Len("fUsrID"))//�ۿ����� fUsrID�� �Բ� ������.
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

		recSelUser.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
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
		//AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�

		CStringW fCertCode(KwRandomEnNumStr(6, 1));
		jpa("fCertCode") = fCertCode;

		Quat qs;
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�,
		qs.Field(jpa, "fCertCode", TRUE);//�ʼ�,
		qs.JsonToUpdateSetField(jpa, L"fUsrID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
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
		// 1. ���� ���� ���� ������ �ֳ�?
		//	- ���� ��ü��簡
		//	- �� fUsrId�� ���� �۾� ���ΰ� �ֳ�?
		//	- std::map<wstring, wstring> _fUsrID_fWorkWithID
		//	- fWorkWithID �� torderwith ���̺� ,fWorkWithID, fUsrID, fOrderID fState

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

		// 1. DB��� �ʱ�ȭ
		KRecordset rs(&_db);

		// 2. request parmameter �м�
		CString sql;
		CString limit = jpa.S("limit");
		if(limit.IsEmpty())
			limit = L"10";
		sql.Format(L"select * from t_uuid limit %s", (PWS)limit);

		// 3. SQL query����
		BOOL bOpen = rs.OpenSelectFetch(sql);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.

		// 4. SQL Query����� JSON���� ����
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
	//if(fKey2)// ������ �ȵ���
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
	//	qs.Field(jpa, "fBizID", TRUE);//�ʼ�
	qs.SetSQL(L"SELECT c.TABLE_NAME, c.COLUMN_NAME, \
CONCAT(left(MID(c.COLUMN_TYPE, 6), CHAR_LENGTH(c.COLUMN_TYPE) -6)) fElist, \
c.COLUMN_COMMENT FROM information_schema.COLUMNS c \
WHERE c.TABLE_SCHEMA = 'petme21' AND c.DATA_TYPE = 'enum' AND c.TABLE_NAME NOT LIKE '~%';");
	rec.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
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
		union_add = L"";//�� �־��ָ� ���ٰ� ���� ����.
	qs.Field("union_add", union_add, 0, 0, 0);
	qs.SetSQL(L"SELECT e.fTable, e.fField, e.fCode, e.fKR FROM tenum e WHERE fTable in (@fTable) @union_add");//ORDER BY fField;
	rece.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
	if(!rece.NoData())
		MakeRecsetToJson(rece, jrs, L"tenum");
	//MakeRecsetOneRowToJson(rece, jrs, L"tenum");	// torderschd
}

/// �� ��û
int ApiSite1::InsertFamily(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		qs.Field(jpa, "fBizID", TRUE);//�ʼ�, ��ǰ
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�, 
		qs.Field(jpa, "fClass", TRUE);//�ʼ�, 
		qs.Field(jpa, "fState", TRUE);//�ʼ�, 
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
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		qs.Field(jpa, "fBizID", TRUE);//�ʼ�, ��ǰ
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�, 
		qs.Field(jpa, "fClass");//�ʼ�, 
		qs.Field(jpa, "fState");//�ʼ�, 
		qs.JsonToUpdateSetField(jpa, L"fBizID", L"fUsrID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
		//qs.SetSQL(L"UPDATE tmypets r SET @SetField WHERE r.fPetID = @fPetID;");
		qs.SetSQL(L"UPDATE tbizfamily SET @SetField WHERE fBizID = @fBizID AND fUsrID = @fUsrID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


/// �� �ݷ����� ���
int ApiSite1::AddMyPet(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		CString fPetID = DevGuid(L"mypet");
		qs.Field("fPetID", fPetID);
		qs.Field(jpa, "fBizID");// ��ǰ
		qs.Field(jpa, "fUsrIdCare", TRUE);//�ʼ�, 
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

/// �� �ݷ����� ���
int ApiSite1::UpdateMyPet(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		qs.Field(jpa, "fPetID", TRUE);//�ʼ�,
		qs.Field(jpa, "fBizID");
		qs.Field(jpa, "fUsrIdCare");//�ʼ�, 
		qs.Field(jpa, "fBreedID");//
		qs.Field(jpa, "fAgeMonth");// 
		qs.Field(jpa, "fName");// 
		qs.Field(jpa, "fWeight");// 
		qs.Field(jpa, "fGender");// 
		qs.Field(jpa, "fMemo");// 
		qs.JsonToUpdateSetField(jpa, L"fPetID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
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
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		qs.Field(jpa, "fPetID", TRUE);//�ʼ�,
		qs.SetSQL(L"DELETE FROM tmypets WHERE r.fPetID = @fPetID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}
#endif // _APIEX



#ifdef _APIEX




/// [App���� ������] -> �������� �� -> ���޿�û �̴�.
int ApiSite1::NewBusiness(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		// ������ ì�� ��� �ϴ� �ʵ�
		jpa("fBizID") = DevGuid(L"biz");
		jpa("fState") = L"hide";
		jpa("fStJoin") = L"register";
		jpa("fMemo") = L"���� ��������� ��û";

		Quat qs;
		qs.Field(jpa, "fUsrIdReg", TRUE);//�ʼ�
		qs.Field(jpa, "fBizID", TRUE);//�ʼ�
		qs.Field(jpa, "fBizPath", TRUE);//�ʼ�
		qs.Field(jpa, "fForm", TRUE);//�ʼ�
		qs.Field(jpa, "fTel", TRUE);//�ʼ�
		qs.Field(jpa, "fTel2");
		qs.Field(jpa, "fTel3");
		qs.Field(jpa, "fState", TRUE);//�ʼ�
		qs.Field(jpa, "fStJoin", TRUE);//�ʼ�
		qs.Field(jpa, "fAnimal", TRUE);//�ʼ�
		qs.Field(jpa, "fShowPrice", TRUE);//�ʼ�
		qs.Field(jpa, "fBegin", TRUE);//�ʼ�
		qs.Field(jpa, "fEnd", TRUE);//�ʼ�
		qs.Field(jpa, "fTitle", TRUE);//�ʼ�
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
	qs.Field(jpa, "fBizID", TRUE);//�ʼ�
	qs.SetSQL(L"SELECT \
fBizReq fBizID, fBizPath, fBizReq, fUsrIdCeo, fUsrIdAdm, fForm, fTel, fTel2, fTel3, fState, fStJoin, fAnimal, fShowPrice, fBegin, fEnd, fTitle, fSubTitle, fDesc, fAddr, fLat, fLon -- \n\
	FROM tbiz WHERE fBizReq = @fBizID;");
	rec.OpenSelectFetch(qs.GetSQL());//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
	if(rec.NoData())
		jrs("Return") = L"No Data";
	else
		MakeRecsetOneRowToJson(rec, jrs, L"tbiz");	// "torder2pet":[{obj},{obj},{},] }
	return 0;
}


/// App���� ������ -> [��������] �� -> ���޿�û �̴�.
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
			qs.Field(jpa, "fBizID", TRUE);//�ʼ�, ��
		}

		qs.SetSQL(L"\
INSERT INTO tbizclassin(fBizID, fBIzClsCD)-- \n\
	SELECT @fBizID, c.fBizClsCD FROM tbizclass c WHERE fBizClsCD IN (@inBzCls); -- to c++");
		_db.ExecuteSQL(qs);

		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// [App���� ������] -> �������� �� -> ���޿�û �̴�.
int ApiSite1::UpdateBusiness(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Quat qs;
		qs.JsonToUpdateSetField(jpa, L"fBizID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
		qs.SetSQL(L"UPDATE tbiz SET @SetField where fBizID = @fBizID;");
		_db.ExecuteSQL(qs);

		SelectBizToJson(jpa, jrs);

		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


/// App���� ������ -> [��������] �� -> ���޿�û �̴�.
// �������� �ϱ� ���� ����Ʈ ��û�Ѵ�.
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


/// �ϳ��� ����.
int ApiSite1::RemoveBizClass(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Quat qs;
		qs.Field(jpa, "fOrdOptID", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fBIzClsCD", TRUE);//�ʼ�, ��
		qs.SetSQL(L"DELETE FROM tbizclass WHERE fBizID = @fBizID and fBIzClsCD = @fBIzClsCD;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// App���� ������ -> �������� �� -> ���޿�û �̴�.
int ApiSite1::RequestToJoin(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		// biz���ʵ��� ����-TEST.sql ����.
		Rec(rec1);
		Quat qs;
		qs.Field(jpa, "fBizID", TRUE);//�ʼ�, ��

		qs.SetSQL(L"SELECT fBizClsCD FROM tbizclassin i WHERE i.fBizID = @fBizID;");
		rec1.OpenSelectFetch(qs.GetSQL());//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
		if(rec1.NoData()) // ���� ���õ� ���ϰ� ���޸� ��û �ϴٴ�~
			throw_response(eHttp_Bad_Request, L"No Business Sectors");

		jpa("fStJoin") = L"request";//���޿�û

		qs.Field(jpa, "fUsrIdUpdate", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fStJoin");//
		qs.JsonToUpdateSetField(jpa, L"fBizID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
		qs.SetSQL(L"\
UPDATE tbiz SET @SetField where fBizID = @fBizID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// tbiz�� �ϳ� ��� �ϸ鼭 ��� ���ʵ����͸� �������ͷ� �����Ѵ�. �Լ� w_GenerateBizData
int ApiSite1::RegisterBiz(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Quat qs;
		CString fBizID = DevGuid(L"biz");
		qs_Field(qs, fBizID);
		qs.Field(jpa, "fUsrIdUpdate", TRUE);//�ʼ�, ��
		//qs.Field(jpa, "fBizID", TRUE);//�ʼ�, ��

		qs.SetSQL(L"CALL w_GenerateBizData(@fUsrIdUpdate, @fBizID);");
		_db.ExecuteSQL(qs);

		Quat qs1;
		jpa("fStJoin") = L"joined";//���޿�û
		qs1.Field(jpa, "fUsrIdUpdate", TRUE);//�ʼ�, ��
		qs1.Field(jpa, "fBizID", TRUE);//�ʼ�, ��
		qs1.Field(jpa, "fStJoin");
		qs1.JsonToUpdateSetField(jpa, L"fBizID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
		qs1.SetSQL(L"\
UPDATE tbiz SET @SetField where fBizID = @fBizID;");
		_db.ExecuteSQL(qs1.GetSQL());

		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// PC passwd�� �α���
/// PC ���� ��û
/// PC ����Ű�� �α���
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
		qs.Field(jpa, "fLoginID", TRUE);//�ʼ�, ��

		if(action == L"LoginByPasswd")//jpa.Has("fPcPwd"))
		{
			qs.Field(jpa, "fPcPwd", TRUE);//�ʼ�, ��
			qs.SetSQL(L"SELECT \
u.fUsrID,u.fTel,u.fEmail,u.fNickName,u.fLoginID,u.fnote,u.fOkEmail,u.fOkPush,u.fState -- \n\
 FROM tuser u WHERE fLoginID = @fLoginID AND fPcPwd = @fPcPwd;");
			rec1.OpenSelectFetch(qs.GetSQL());//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
			if(rec1.NoData()) // ���� ���õ� ���ϰ� ���޸� ��û �ϴٴ�~
				jrs("Return") = L"NoData";
			else
			{
				MakeRecsetToJson(rec1, jrs);
				jrs("Return") = L"OK";
			}
		}
		else if(action == L"LoginByEncode")//jpa.Has("fCertKey"))//�� ���� �ڵ��û. ������ ���� �ڵ� ���� ����
		{
			// �ۿ��� ����Ű ������ �� Ű�� �ٽ� �α���. �ۿ��� ��� �ٲٴ� ��� ����.
			qs.Field(jpa, "fUsrID");//�ʼ�, ��
			qs.Field(jpa, "fCertKey");//�ʼ�, ��
			qs.SetSQL(L"SELECT \
u.fUsrID,u.fTel,u.fEmail,u.fNickName,u.fLoginID,u.fnote,u.fOkEmail,u.fOkPush,u.fState -- \n\
FROM tcertification c JOIN tuser u ON u.fUsrID = c.fUsrID WHERE fUsrID = @fUsrID AND fCertKey = @fCertKey;");
			rec1.OpenSelectFetch(qs.GetSQL());//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
			if(rec1.NoData()) // ���� ���õ� ���ϰ� ���޸� ��û �ϴٴ�~
				jrs("Return") = L"NoData";
			else
			{
				MakeRecsetToJson(rec1, jrs);
				jrs("Return") = L"OK";
			}
		}
		else if(action == L"RequestCertKey") // ���� ��û�ϸ� �ۿ��� RequestCertificatedKey
		{
			CString fCertKey(KwRandomEnNumStr(6, 1));
			qs_Field(qs, fCertKey);
			qs.Field(jpa, "fUsrID");//�ʼ�, ��
			qs.SetSQL(L"INSERT INTO tcertification (fUsrID,fCertKey,fLoginID) -- \n\
	SELECT @fUsrID, @fCertKey, @fLoginID;");
			_db.ExecuteSQL(qs);//�ϴ� �޴����� ����Ű ��û �ϸ� �ֱٲ�
			jrs("Return") = L"OK";
		}
		else
			throw_response(eHttp_Bad_Request, L"Unknown action requestd.");
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// <summary>
/// �ۿ��� ����Ű ��û. ���߿� Ǫ�÷� �ڵ� ����. PC���� ���� ��û �ϰ� 
/// �ۿ��� ����Ű ��û �ϸ� �ֱٿ� ��û�� ���� ���.
/// �̶� 30�� �ȿ� PC���� �α��� �Ѵ�.
/// </summary>
int ApiSite1::GetCertificatedKey(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Rec(rec1);
		Quat qs;
		qs.Field(jpa, "fLoginID", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fCertKey", TRUE);//�ʼ�, ��
		// fConsumed �� ���� ���� �ֱٰ� 1���� ���� ����.
		qs.SetSQL(L"SELECT * FROM tcertification WHERE fPcLogID = @fLoginID AND fConsumed IS NULL ORDER BY fCreated DESC LIMIT 1;");
		rec1.OpenSelectFetch(qs.GetSQL());//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
		if(rec1.NoData()) // ���� ���õ� ���ϰ� ���޸� ��û �ϴٴ�~
			throw_response(eHttp_Not_Found, L"���� ��û�� ���� �����ϴ�.");
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
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fPcPwd", TRUE);//�ʼ�, ��
		// fConsumed �� ���� ���� �ֱٰ� 1���� ���� ����.
			// �ۿ��� ����Ű ������ �� Ű�� �ٽ� �α���. �ۿ��� ��� �ٲٴ� ��� ����.
		qs.Field(jpa, "fUsrID");//�ʼ�, ��
		qs.Field(jpa, "fPcPwd");//�ʼ�, ��
		qs.SetSQL(L"UPDATE tuser SET fPcPwd = @fPcPwd WHERE fUsrID = @fUsrID;");
		_db.ExecuteSQL(qs);//�ϴ� �޴����� ����Ű ��û �ϸ� �ֱٲ�

		/// �ٲٰ� ���� �ٽ� �о?
//		qs.SetSQL(L"SELECT \
//u.fUsrID,u.fTel,u.fEmail,u.fNickName,u.fPcLogID,u.fnote,u.fOkEmail,u.fOkPush,u.fState -- \n\
// FROM tuser u WHERE fUsrID = @fUsrID;");
//		rec1.OpenSelectFetch(qs.GetSQL());//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
//		if(rec1.NoData()) // ���� ���õ� ���ϰ� ���޸� ��û �ϴٴ�~
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
/// �������� üũ �Ѵ�. ��� ����ڰ� Ư�� �� �����Ϳ� ������ �ִ°�.
/// ������ throw�Ͽ� Ŭ���̾�Ʈ ���� ����
/// </summary>
/// <param name="fUsrID">���� üũ�� �����</param>
/// <param name="fBizID">���� ��� ��</param>
/// <throw>403 Forbidden</param>
void ApiSite1::AccessRight(JObj& jpa, int iOp)
{
	try
	{
		if(iOp & eFncAccess)
		{
			if(!jpa.IsObject("Access"))
				throw_response(eHttp_Bad_Request, L"Data for Acess not found! Acess:{fUsrID:, fBizID:}");//403 Forbidden 401�� �ٸ� ���� ������ Ŭ���̾�Ʈ�� �������� �˰� �ֽ��ϴ�. 401 Unauthorized 
			JObj& jacs = *jpa.Obj(L"Access").get();

			Rec(rec);
			Quat qs;
			qs.Field(jacs, "fUsrID", TRUE);//�ʼ�, user
			qs.Field(jacs, "fBizID", TRUE);//�ʼ�, ��
			qs.Field("fCharge", "manager");

			qs.SetSQL(L"\
SELECT COUNT(*) FROM -- \n\
	(	SELECT b.fBizID FROM tbiz b  -- \n\
			JOIN tbizstaff f ON  f.fCharge = @fCharge AND f.fUsrIdStf = @fUsrID -- \n\
			JOIN tbiz bu ON bu.fBizID = f.fBizID -- \n\
			WHERE INSTR(b.fBizPath, CONCAT(bu.fBizPath, bu.fBizID, '/')) OR b.fBizID = bu.fBizID -- \n\
	) ds WHERE fBizID = @fBizID;");
			rec.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
			int count = rec.CellI(0, 0);
			if(count <= 0)
				throw_response(eHttp_Unauthorized, L"Forbidden (������ �����ϴ�.)");//403 Forbidden 401�� �ٸ� ���� ������ Ŭ���̾�Ʈ�� �������� �˰� �ֽ��ϴ�. 401 Unauthorized 
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
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fBizID", TRUE);//�ʼ�, ��

		qs.SetSQL(L"SELECT c.fPrcID,c.fProdID,c.fBrdSzCD,c.fMinWeit,c.fMaxWeit,c.fPrice,c.fCurrency,c.fShow -- \n\
	FROM tpricebiz c JOIN tproductbiz p ON p.fProdID = c.fProdID AND p.fBizID = @fBizID AND c.fShow = 'show';");
		rec1.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
		if(rec1.NoData()) // ���� ���õ� ���ϰ� ���޸� ��û �ϴٴ�~
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
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		qs.JsonToUpdateSetField(jpa, L"fPrcID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
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
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		qs.Field(jpa, "fPrcID", TRUE);//�ʼ�, ��
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
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		CString fPrcID = DevGuid(L"price");
		qs_Field(qs, fPrcID);

		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�, �����ѻ��
		qs.Field(jpa, "fProdID", TRUE);//�ʼ�, ��ǰ
		qs.Field(jpa, "fBrdSzCD", TRUE);//�ʼ�, small, large...
		qs.Field(jpa, "fMinWeit", TRUE);//�ʼ�, ���� ü��, 
		qs.Field(jpa, "fMaxWeit", TRUE);//�ʼ�, �ְ� ü��
		qs.Field(jpa, "fPrice", TRUE);//�ʼ�, ��
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
	qs.Field(jpa, "fUsrIdStf");//�ʼ� �ƴ�. ������-�����
	qs.Field(jpa, "fBizID", TRUE);//�ʼ�
	qs.Field(jpa, "fState");//
	qs.Field(jpa, "fCharge");//
	qs.Field(jpa, "fPosition");//
	qs.Field(jpa, "fDesc");//
	qs.Field(jpa, "fNote");//�����ڿ�
	qs.SetSQL(L"INSERT INTO tbizstaff(fUsrIdStf, fBizID, fState, fCharge, fPosition, fDesc, fNote) VALUES -- \n\
(@fUsrIdStf, @fBizID, @fState, @fCharge, @fPosition, @fDesc, @fNote);");
	//('staff-a021', 'biz-0002', 'ON', 'manager', '����1', '�׽�Ʈ ������', 'AccessRight�׽�Ʈ'),
	_db.ExecuteSQL(qs);
	jrs("fUsrIdStf") = jpa.S("fUsrIdStf");
	jrs("fBizID") = jpa.S("fBizID");
}


int ApiSite1::AddStaff(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		_db.TransBegin();// insert�� 2�� �̻��̴� Ʈ����� �־��.

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
	qs.Field(jpa, "fUsrIdStf", TRUE);//�ʼ�
	qs.Field(jpa, "fBizID", TRUE);//�ʼ�
	qs.SetSQL(L"DELETE FROM tbizstaff WHERE fUsrIdStf = @fUsrIdStf AND fBizID = @fBizID;");
	_db.ExecuteSQL(qs);
}
int ApiSite1::DelStaff(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		_db.TransBegin();// insert�� 2�� �̻��̴� Ʈ����� �־��.

		DelStaff(jpa, jrs);
		//������ ���� �Ǿ �������� ���� �ϸ� ���� ��Ͽ� ������ ��ġ�� ������ ���� �ϸ� �ȵȴ�.
		DelVirtualCharge(jpa, jrs);

		_db.TransCommit();
	} CATCH_DBTR;
	return 0;
}
int ApiSite1::UpdateStaff(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		qs.Field(jpa, "fUsrIdStf", TRUE);//�ʼ�, ��ǰ
		qs.Field(jpa, "fBizID", TRUE);//�ʼ�- ����� �����
		qs.JsonToUpdateSetField(jpa, L"fUsrIdStf", L"fBizID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
		qs.SetSQL(L"UPDATE tbizstaff r SET @SetField WHERE r.fUsrIdStf = @fUsrIdStf AND fBizID = @fBizID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// <summary>
/// Ÿ�����̺� �÷�. ������.
/// </summary>
void ApiSite1::AddVirtualCharge(JObj& jpa, JObj& jrs)
{
	Quat qs;
	CString fVChrgID = DevGuid(L"vcharge");
	qs_Field(qs, fVChrgID);

	qs.Field(jpa, "fUsrID");//�ʼ� �ƴ�. ������-�����
	qs.Field(jpa, "fBizID", TRUE);//�ʼ�, ��ǰ
	qs.SetSQL(L"INSERT INTO tbizvirtual(fVChrgID, fUsrID, fBizID) VALUES -- \n\
(@fVChrgID, @fUsrID, @fBizID);");
	_db.ExecuteSQL(qs);
	jrs("fVChrgID") = fVChrgID;

}
int ApiSite1::AddVirtualCharge(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		AddVirtualCharge(jpa, jrs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}
void ApiSite1::DelVirtualCharge(JObj& jpa, JObj& jrs)
{
	Quat qs;
	Rec(rec4);
	qs.Field(jpa, "fVChrgID", TRUE);//�ʼ�, �������� ID
	qs.SetSQL(L"SELECT s.fBeginTo FROM torderschd s WHERE s.fVChrgIdWrk = @fVChrgID -- \
 AND s.fTimeUse = 'occupy';"); //AND s.fBeginTo >= NOW() ���Ų��� �־ ���� ���Ѵ�.
	rec4.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
	if(!rec4.NoData())
	{
		jrs("Message") = L"����� ������ �־� ���� ���� �ʽ��ϴ�.";
		return;
	}
	// ��� ������ ���� ��츸 ����
	qs.SetSQL(L"DELETE FROM tbizvirtual WHERE fVChrgID = @fVChrgID;");
	_db.ExecuteSQL(qs);
}

int ApiSite1::DelVirtualCharge(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		DelVirtualCharge( jpa, jrs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}
int ApiSite1::UpdateVirtualCharge(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		qs.Field(jpa, "fVChrgID", TRUE);//�ʼ�, ��ǰ
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�- ����� �����
		qs.JsonToUpdateSetField(jpa, L"fVChrgID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
		qs.SetSQL(L"UPDATE tbizvirtual r SET @SetField WHERE r.fVChrgID = @fVChrgID;");
		_db.ExecuteSQL(qs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


/// <summary>
/// ���� �Ϸ� Ÿ�� ���̺�. ���� �Ҷ��� �ʿ�.
/// ���Ǻ��� �ִ� ���� �Ʒ��� ���Ѵ�.
/// deprecated
/// </summary>
int ApiSite1::GetTodaySchduleTotal(JObj& jpa, JObj& jrs, int iOp)
{
	ASSERT(0);// deprecated
	try
	{
		Quat qs;
		if(jpa.S("fAllStaff") == L"every")// ��� ���ļ� �ش�. ��, �츮���� �����̳� ���� ���ؿ�.
		{
			return GetTodaySchdule(jpa, jrs, iOp);
		}
		Rec(rec1);

		jpa("fTaskID") = DevGuid(L"task");
		qs.Field(jpa, "fTaskID", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fBizID", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fDay", TRUE);//�ʼ�, ��
		qs.SetSQL(L"CALL w_select_today(@fTaskID, @fBizID, @fUsrID, @fDay, @fAllStaff);");
		rec1.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
		if(rec1.NoData()) // ���� ���õ� ���ϰ� ���޸� ��û �ϴٴ�~
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
		qs.Field(jpa, "fTaskID", TRUE);//�ʼ�, ��

		qs.Field(jpa, "fAllStaff", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fBizID", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fDay", TRUE);//�ʼ�, ��
		//qs.Field(jpa, "fOffOn");// NULL�� ��� ���� �ϴµ�. sql���� NULL ��� ó��
		int fAllStaff = jpa.I("fAllStaff");

		qs.SetSQL(L"\
SELECT v.fVChrgID, v.fUsrID, f.fOrderSched, f.fPosition, u.fTel, u.fNickName, f.fAvailable, f.fCharge, v.fBizID -- \n\
	FROM tbizvirtual v  -- \n\
	LEFT JOIN tbizstaff f ON v.fUsrID = f.fUsrIdStf AND f.fBizID = v.fBizID -- \n\
	LEFT JOIN tuser u ON u.fUsrID = v.fUsrID -- \n\
	WHERE v.fBizID = @fBizID;");
		recU.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.

		qs.SetSQL(L"\
call w_AllScheduleToTemp(@fTaskID, NULL, @fBizID, @fUsrID, @fDay, 1, 0);");
		_db.ExecuteSQL(qs);//,fLat,fLon,fUpdated,

		qs.SetSQL(L"\
SELECT s.fIDX,s.fOrder,s.fSchedEvent,s.fSrcDesc,s.fBizID,s.fVChrgID,s.fOffBegin,s.fOffEnd,s.fOff, s.fSchedOrgID, s.fMode,s.fRepeat,s.fNote,s.fUsrID -- \n\
	FROM t_m_allsched s  -- \n\
	WHERE fTaskID = @fTaskID ORDER BY fBizID, fOrder;");

		rec1.OpenSelectFetch(qs);//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
		if(rec1.NoData()) // ���� ���õ� ���ϰ� ���޸� ��û �ϴٴ�~
			throw_response(eHttp_Bad_Request, L"Error in shedule.");
		/// �ݵ�� fOff�� ��(�������)�� ���� ������ �Ѵ�. fVChrgID �� ������ ��Ƽ� ���߿� ��ġ�Ƿ�
		/// �׷��� ���밪���� �� ���� �ϰ� ���� ���� ������ ���� �ϵ��� ���� �Ѵ�.

		int minu = MinuteForCut30;// 30; // work unit minute : 30�� ������ �ϴ����� ����.
		int m48 = NumberOfTimeCut48;// 24 * (60 / minu); // �迭 ���� 48��

		CBiz cbiz;//biz�� �ϳ���. VChrg�� ������ ����.
		auto bz = &cbiz;//SearchScheduleByLocation_off_1�� �������
		int fOff_10 = 0;
		CString fVChrgTotal = L"total";

		for(int r = 0; r < recU.RowSize(); r++)
		{
			CString vch = recU.Cell(r, "fVChrgID");
			if(!vch.IsEmpty())// null�̸� ���������̹Ƿ� ������ü�� �ǹ� �Ѵ�.
				bz->AddVirtualCharge(vch, r);
		}

		for(int r = 0; r < rec1.RowSize(); r++)
		{
			ScheduleToTimeTable(rec1, bz, r);
		}
		//���ݱ��� ���� ȸ���� �����̳ʵ� ������ ��ģ �� ���� ���� ���� and �Ѵ�.
		CBiz bzt0;//��ģ�Ŵ� ���� �����. 
		auto bzt = &bzt0; // �˻� �� �ҽ��� ��� �ϰ� �ϱ�����
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
		for(auto& [kVChrgID, vcObj] : pbz->_mapVcObj) // total�̸� vc�� �ϳ� �̰���.
		{
			JObj jvinf;
			jvinf("fVChrgID"   ) = vcObj->fVChrgID;
			if(vcObj->rowU >= 0) //fAllStaff == 1)
			{
				recU.SetJsonD(jvinf, vcObj->rowU, "fOrderSched");//����Ű
				recU.SetJsonS(jvinf, vcObj->rowU, "fUsrID");//�����
				recU.SetJsonS(jvinf, vcObj->rowU, "fBizID");//�����
				recU.SetJsonS(jvinf, vcObj->rowU, "fPosition");//����
				recU.SetJsonS(jvinf, vcObj->rowU, "fTel");//��ȭ
				recU.SetJsonS(jvinf, vcObj->rowU, "fNickName");//�г���
			}

			jchr(kVChrgID.c_str()) = jvinf;//fVChrgID
			///jchr.SetMove(kVChrgID.c_str(), jvinf);// jvinf���� IsObject�� ���� �� IsString �̹Ƿ� ���� Move�� �ʿ� ����.
			
			JArr jarr;//JSONArray
			CTime t00(t.GetYear(), t.GetMonth(), t.GetDay(), 0, 0, 0);
			for(int i = 0; i < vcObj->_arCell.GetCount(); i++)
			{
				auto sl = vcObj->_arCell[i];

				JObj robj;//t_m_timeday �����.
				CString fTime;
				KwCTimeToString(t00, fTime);
				robj("fTime") = fTime;//30�д��� �Ϸ�ð�
				robj("fOff") = sl->fOff;// sl->fOff; 1�̸� ����, 0�̸� ���డ��
				if(sl->fPause.GetLength() > 0)
					robj("fPause") = sl->fPause;// sl->fOff; 1�̸� ����, 0�̸� ���డ��
				robj("fNote") = sl->fNote;//������ ��Ʈ
				//robj("fVChrgID") = vcObj->fVChrgID;
				if(sl->rowS >= 0)//������ �ִ� ��츸 0,1,2,... �̴�.
				{
					rec1.SetJsonS(robj, sl->rowS, "fIDX");//unique key
					rec1.SetJsonS(robj, sl->rowS, "fSchedOrgID");//���� ��õ Ű NULL , 
					rec1.SetJsonS(robj, sl->rowS, "fSchedEvent");//���� ����Ű ��: torderschd.fOrderID
					rec1.SetJsonS(robj, sl->rowS, "fMode");//closed, schedule, order...
					rec1.SetJsonS(robj, sl->rowS, "fRepeat");//once, dayly, monthly..
					rec1.SetJsonS(robj, sl->rowS, "fSchedOrgID");//������õŰ
					rec1.SetJsonS(robj, sl->rowS, "fOffBegin");//����
					rec1.SetJsonS(robj, sl->rowS, "fOffEnd");//��
					rec1.SetJsonS(robj, sl->rowS, "fVChrgID");//���. �÷��� �ƴϴ�.��¥������. NULL�̸� ���� ����.
					rec1.SetJsonS(robj, sl->rowS, "fUsrID");//���. ��¥ ���.
				}
				KTrace(L"%s\t%d\t%s\n", fTime, sl->fOff, sl->fNote);
				jarr.Add(robj);// new JSONValue(*robj.get()));

				t00 += sp;
			}
			jocc(kVChrgID.c_str()) = jarr;//fVChrgID
		}//);

		//jrs("occupy") = jocc;
		//jrs("charge") = jchr;
		jrs(L"occupy") = jocc;//�ȿ� object�� array�� ������ SetMove�� ȿ�����̴�.
		jrs(L"charge") = jchr;

		jrs("fMinuteForCut30") = MinuteForCut30;
		jrs("fNumberOfTimeCut48") = NumberOfTimeCut48;
		jrs("Return") = L"OK";
	} CATCH_DB;

	return 0;
}

/// Ÿ�����̺� ȭ�鿡�� ���콺 ���۽� �ҷ���
int ApiSite1::UpdateTodaySchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		qs.Field(jpa, "fSchedOrgID", TRUE);//�ʼ�, torderschd.fOrdShedID tbizschedule.fSchdID
		qs.Field(jpa, "fMode", TRUE);//�ʼ�, order | schedule
		qs.Field(jpa, "fVChrgID");//�ʼ�- ���� �����
		qs.Field(jpa, "fOffBegin", TRUE);//�ʼ�- begin
		qs.Field(jpa, "fOffEnd", TRUE);//�ʼ�- end
		qs.Field(jpa, "fUsrID");//����� ����� : NULL �ϼ� �ִ�.
		qs.Field(jpa, "fUsrIdUpt", TRUE);// ������ ���

		CTime tb = jpa.T("fOffBegin");
		CTime te = jpa.T("fOffEnd");
		CTime nt = CTime(te.GetYear(), te.GetMonth(), te.GetDay(), 0, 0, 0);
		if(tb < nt && nt < te) /// ���� �Ѿ� ����.
			throw_response(eHttp_Bad_Request, "���� �ð��� ���� �Ѿ� ����.");
		
		CString fMode = jpa.S("fMode");

		if(fMode == "order") // ���� torderschd
		{
			JObj jch;
			jch.Copy(jpa, "fOrdShedID", "fSchedOrgID");
			//jch.Copy(jpa, "fUsrIdStf", "fUsrID"); ���� ���� ��� �̹Ƿ� Update�� �Ⱦ���.
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
	

			qso.JsonToUpdateSetField(jch, L"fOrdShedID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
			qso.SetSQL(L"UPDATE torderschd r SET @SetField WHERE r.fOrdShedID = @fOrdShedID;");
			_db.ExecuteSQL(qso);
		}
		else if(fMode == "schedule")// tbizschedule
		{
			JObj jch;
			jch.Copy(jpa, "fSchdID", "fSchedOrgID");
			jch.Copy(jpa, "fUsrID"); //���� ������ �ٲ��.
			jch.Copy(jpa, "fBeginTo", "fOffBegin");
			jch.Copy(jpa, "fEndTo", "fOffEnd");
			jch.Copy(jpa, "fUsrIdOwn", "fUsrIdUpt");

			Quat qso;
			qso.Field(jch, "fSchdID", TRUE);
			qso.Field(jch, "fUsrID", TRUE);
			qso.Field(jch, "fBeginTo", TRUE);
			qso.Field(jch, "fEndTo", TRUE);
			qso.Field(jch, "fUsrIdOwn", TRUE);
			qso.JsonToUpdateSetField(jch, L"fSchdID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
			qso.SetSQL(L"UPDATE tbizschedule r SET @SetField WHERE r.fSchdID = @fSchdID;");
			_db.ExecuteSQL(qso);
		}
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// �׽�Ʈ �Ϸ�. ȭ�鿡�� �˾� �޴��� ����
int ApiSite1::DeleteTodaySchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		qs.Field(jpa, "fSchedOrgID", TRUE);//�ʼ�, torderschd.fOrdShedID tbizschedule.fSchdID
		qs.Field(jpa, "fMode", TRUE);//�ʼ�, order | schedule

		CString fMode = jpa.S("fMode");

		_db.TransBegin();// insert�� 2�� �̻��̴� Ʈ����� �־��.

		if(fMode == "order") // ���� torderschd
		{
			JObj jch;
			jch.Copy(jpa, "fOrdShedID", "fSchedOrgID");
			jch.Copy(jpa, "fUsrIdUpt");

			Quat qso;
			qso.Field(jch, "fOrdShedID", TRUE);
			qso.JsonToUpdateSetField(jch, L"fOrdShedID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
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


/// �׽�Ʈ �̿Ϸ�. ȭ�鿡�� �˾� �޴��� �߰�
int ApiSite1::AddTodayOrder(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		JObj jch;
		jch.Copy(jpa, "fOrdShedID", "fSchedOrgID");
		jch.Copy(jpa, "fUsrIdStf", "fUsrIdUpt");//���� ���� ���
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

		//qso.JsonToUpdateSetField(jch, L"fOrdShedID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
		qso.SetSQL(L"INSERT INTO torderschd (fOrdShedID,fUsrIdStf,fVChrgIdWrk,fBeginTo,fEndTo,fUsrIdUpt) VALUES -- \n\
(@fOrdShedID,@fUsrIdStf,@fVChrgIdWrk,@fBeginTo,@fEndTo,@fUsrIdUpt);");
		_db.ExecuteSQL(qso);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// �׽�Ʈ �̿Ϸ�. ȭ�鿡�� �˾� �޴��� �߰�
int ApiSite1::AddTodaySchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		// t_m_allsched
		qs.Field(jpa, "fSchedOrgID", TRUE);//�ʼ�, key
		//qs.Field(jpa, "fVChrgID");//�ʼ�- ���� �����
		qs.Field(jpa, "fOffBegin", TRUE);//�ʼ�- ����
		qs.Field(jpa, "fOffEnd", TRUE);//�ʼ�- ��
		qs.Field(jpa, "fSrcDesc", TRUE);//�ʼ�-

		// tbizschedule
		qs.Field(jpa, "fBizID", TRUE);//�ʼ�- ����
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�- ����
		qs.Field(jpa, "fAttr", TRUE);//�ʼ�- ����
		qs.Field(jpa, "fRepeat", TRUE);//�ʼ�- ����
		qs.Field(jpa, "fOff", TRUE);//�ʼ�-
		qs.Field(jpa, "fMode", TRUE);//�ʼ�- schedule
		qs.Field(jpa, "fOrder", TRUE);//�ʼ�-
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
		qso.JsonToUpdateSetField(jch, L"fSchdID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
		qso.SetSQL(L"INSERT INTO tbizschedule (fSchdID, fBizID, fUsrID, fAttr, fRepeat, fOff, fBeginTo, fEndTo, fNote, fUsrIdOwn) VALUES -- \n\
(@fSchdID, @fBizID, @fUsrID, @fAttr, @fRepeat, @fOff, @fBeginTo, @fEndTo, @fNote, @fUsrIdOwn);");
//('shedule-0005', 'biz-0002', 'staff-a024', 'private', 'off', '2021-02-10 17:00:00', '2021-02-11 18:00:00', '��� �ް�', 'staff-a024', '2021-03-03 22:57:01');
		_db.ExecuteSQL(qso);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}



/// ���� �׽�Ʈ ����. ��â���� ������
int ApiSite1::UpdateOrderSchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		qs.Field(jpa, "fSchedOrgID", TRUE);//�ʼ�, ��ǰ
		qs.Field(jpa, "fVChrgID");//�ʼ�- ����� �����
		qs.Field(jpa, "fOffBegin", TRUE);//�ʼ�- ����� �����
		qs.Field(jpa, "fOffEnd", TRUE);//�ʼ�- ����� �����
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�- ������ ���

		CString fMode = jpa.S("fMode");

		qs.JsonToUpdateSetField(jpa, L"fOrdShedID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
		qs.SetSQL(L"UPDATE tbizvirtual r SET @SetField WHERE r.fVChrgID = @fVChrgID;");
		_db.ExecuteSQL(qs);

		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

/// ���� �׽�Ʈ ����. ��â���� ������
int ApiSite1::UpdateBizSchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Quat qs;
		qs.Field(jpa, "fSchedOrgID", TRUE);//�ʼ�, ��ǰ
		qs.Field(jpa, "fVChrgID");//�ʼ�- ����� �����
		qs.Field(jpa, "fOffBegin", TRUE);//�ʼ�- ����� �����
		qs.Field(jpa, "fOffEnd", TRUE);//�ʼ�- ����� �����
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�- ����� �����
		qs.Field(jpa, "fVChrgIdWrk");//
		qs.Field(jpa, "fUsrIdOwn", TRUE);//�ʼ�- ����� �����

		CString fMode = jpa.S("fMode");

		JObj jch;
		jch("fOrdShedID") = jpa.S("fSchedOrgID");
		jch("fUsrIdUpt") = jpa.S("fUsrID");
		jch("fVChrgIdWrk") = jpa.S("fVChrgIdWrk");
		jch("fUsrIdUpt") = jpa.S("fUsrID");
		jch("fUsrIdOwn") = jpa.S("fUsrIdUpt");
		Quat qso;
		//qso.Field(

		qs.JsonToUpdateSetField(jpa, L"fSchedOrgID"); // Ű�� ������ �ʵ尡 @SetField�� �Ҵ� �ȴ�. Ű�� where�� ���̰�
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
		//AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Rec(rec);
		Quat qs;
		qs.Field(jpa, "fOrdShedID", TRUE);//�ʼ�
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
			SelectEnum(jrs, L"'torderschd', 'torder1'");// table ������ �ֶ��� in ()�� ������ �ְ� quat�� �θ���.
			jrs("Return") = L"OK";
		}
		else
			jrs("Return") = L"No Data";
		//throw_response(eHttp_Not_Found, L"���� ������ �����ϴ�.");
	} CATCH_DB;
	return 0;
}

int ApiSite1::SelectBizSchedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Rec(rec);
		Quat qs;
		qs.Field(jpa, "fSchdID", TRUE);//�ʼ�
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
		//throw_response(eHttp_Not_Found, L"���� ������ �����ϴ�.");
	} CATCH_DB;
	return 0;
}

int ApiSite1::SelectCustomList(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		//AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Rec(rec);
		Quat qs;
		qs.Field(jpa, "fBizID", TRUE);//�ʼ�
		qs.SetSQL(L"SELECT f.fBizID, f.fUsrID, f.fClass, f.fState, u.fTel, u.fEmail, u.fNickName, u.fnote, t.fPetID, t.fBreedID, r.fName fNameBreed , t.fName -- \n\
	FROM tbizfamily f JOIN tuser u ON u.fUsrID = f.fUsrID and fBizID = @fBizID -- \n\
	LEFT JOIN (SELECT t.fPetID, t.fBreedID, t.fName, t.fUsrIdCare, t.fOrder FROM tmypets t GROUP BY fUsrIdCare HAVING t.fOrder = MIN(t.fOrder)) t ON t.fUsrIdCare = f.fUsrID -- \n\
	LEFT JOIN tbreeds r ON r.fBreedID = t.fBreedID;");
		rec.OpenSelectFetch(qs.GetSQL());//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
		if(!rec.NoData())
		{
			MakeRecsetToJson(rec, jrs, L"table");	// tbiz
			SelectEnum(jrs, L"'tbizfamily', 'tuser'");
			jrs("Return") = L"OK";
		}
		else
			jrs("Return") = L"No Data";
		//throw_response(eHttp_Not_Found, L"��ϵ� �� ������ �����ϴ�.");
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
			smap[L"���"] = L"list";
			smap[L"�󼼺���"] = L"detail";
			smap[L"����"] = L"update";
			smap[L"�߰�"] = L"insert";
			smap[L"���λ���"] = L"insert";
			smap[L"����"] = L"delete";
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
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fBizID", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fObject", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fTitle", TRUE);//�ʼ�, ��
		qs.Field(jpa, "fObjID", TRUE);//�ʼ�
		qs.Field(jpa, "fObjID2");//�ʼ�
		qs.Field(jpa, "fObjName", TRUE);//�ʼ�
		qs.Field(jpa, "fObjName2");//�ʼ�
		qs.Field("fMode", fMode2.c_str(), TRUE);//�ʼ�
		qs.Field(jpa, "fDevice", TRUE);//�ʼ�
		qs.Field(jpa, "fTmWork", TRUE);//�ʼ�
		qs.Field("fMemo", fMemo);//�ʼ�
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
		//AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ�
		Rec(rec);
		Quat qs;
		qs.Field(jpa, "fUsrID", TRUE);//�ʼ�
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




// ������ �о Ÿ�����̺� ��ġ
void ApiSite1::ScheduleToTimeTable(KRecordset& rec1, CBiz* bz, int r)
{
	int minu = MinuteForCut30;// 30; // work unit minute : 30�� ������ �ϴ����� ����.
	int m48 = NumberOfTimeCut48;// 24 * (60 / minu); // �迭 ���� 48��

	auto fNote = rec1.Cell(r, "fNote");
	auto fMode = rec1.Cell(r, "fMode");
#ifdef DEBUG
	auto stb = rec1.Cell(r, "fOffBegin");
	auto ste = rec1.Cell(r, "fOffEnd");
#endif // DEBUG

	auto tb = rec1.CellT(r, "fOffBegin");
	auto te = rec1.CellT(r, "fOffEnd");
	// �ð� �������� �ð���Ʈ���� index 0 ~ 47
	int ib = tb.GetHour() * (60 / minu) + (tb.GetMinute() / minu);//=(HOUR(B46) * (60/A46)) + MINUTE(B46)/A46

	if(fMode == L"closed")
		_break;
	// ��¥ ���� ���� �ڿ� 00:00 �� 24:00 ���� ��ģ��.
	//if(KwCompareDate(tb, te) > 0)// && KwCompareTimeOnly(tb, te) >= 0)
	int cmpDate = KwCompareDate(tb, te);
	int hourEnd = cmpDate < 0 && te.GetHour() == 0 ? 24 : te.GetHour();/// �� �ð��� �������̰� 0���̸� 24�� �Ѵ�.
	int ie = hourEnd * (60 / minu) + (te.GetMinute() / minu);
	if(48 < ie)
		throw_response(eHttp_Bad_Request, "fOffEnd time is invalid.");

	CStringW sfOff = rec1.Cell(r, "fOff");
	int fOff = sfOff == L"off" ? 1 : sfOff == L"ON" ? 0 : -1;//���� �Ǿ����� off�̹Ƿ� 1�� �Ѵ�.
	int fOff_10 = 1;// and �̹Ƿ� 1�� �ʱ�ȭ �ؾ� �Ѵ�. or �̸� 0

	int fOrder = rec1.CellI(r, "fOrder");
	CStringW fVChrgID = rec1.Cell(r, "fVChrgID");
	bool bEmptyVChrg = fVChrgID.IsEmpty();

#ifdef DEBUG
	KTrace(L"%d\t%s\t%s\t%s\t%s\t%s\n", fOrder, stb, ste, fMode, fVChrgID, fNote);
#endif // DEBUG

	if(fMode == L"holiday")///	--  1. ������ : or 1
	{
		for(int i = 0; i < m48; i++)// ���ÿ����� 32,33 �� 11
		{
			if(fOff == 1)
				bz->SetPauseTime(fVChrgID, i, L"holiday", r); // ri[i] = fOff;
			//bz->note[i] = fNote;//����� ON(0) �϶��� and �� ����.
		}
		/// ���� off �ΰ͸� �о� �´�. �ƴϸ� ������ ����.
		// 11111111111111 11111111111111 11111111111111 11111111111111 ����
		// 00000000000000 00000000000000 00000000000000 00000000000000 ����
	}
	else if(fMode == L"closed") ///	-- 2. �����ܽð� : or 1
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
		if(bEmptyVChrg) // �� ��ü ����
		{
			for(int i = ib; i < ie; i++)
			{
				if(fOff == 0) ///	-- 3. �������� �� Ư������ : ON or 0 : Ư������ڰ� �־�� 0
					bz->SetPauseTime(fVChrgID, i, L"open", r);//occ | 
					//vff |= fOff;
				else          ///	-- 4. �������� �� off : or 1
				{
					auto occ = bz->GetOccupy(fVChrgID, i);
					auto vff = occ;
					vff |= fOff;
					bz->SetOccupy(fVChrgID, i, vff, fNote, r);
				}
			}// ���� ���ǵ� or ���� �Ѱ� ��ü�� �� ��ħ.
		}
		else // ���� ���� ����
		{
			for(int i = ib; i < ie; i++)
			{
				if(fOff == 0)   ///	-- 5. �������� Ư����� : ON set 0
					bz->SetPauseTime(fVChrgID, i, L"work", r);//occ | 
				else
				{
					auto occ = bz->GetOccupy(fVChrgID, i);
					auto vff = occ;
					vff |= fOff;///	-- 6. �������� �� off : or 1
					bz->SetOccupy(fVChrgID, i, vff, fNote, r); // ȸ������ | �ڱ�����
				}
			}
			bz->nUsr++;
		}
	}
}

// �� ���� ������ ����
void ApiSite1::SumOfEachVCharged(CBiz* bz, CBiz* bzt)
{
	int minu = MinuteForCut30;// 30; // work unit minute : 30�� ������ �ϴ����� ����.
	int m48 = NumberOfTimeCut48;// 24 * (60 / minu); // �迭 ���� 48��
	CString fVChrgTotal = L"total";

	for(int i = 0; i < m48; i++)
	{
		auto occ = 0;// bzt->GetOccupy(fVChrgTotal, i);// ��Ż ���� ���� and �Ϸ���
		int iv = 0;
		for(auto& [vc, vcObj] : bz->_mapVcObj)//�� ������ �о : ���� ���� ����
		{
			auto cell = vcObj->_arCell.GetAt(i);
			if(iv == 0) // ù �׸��� set
				occ = cell->fOff;
			else
				occ &= cell->fOff;// �ι�° ���� and
			iv++;
		}
		bzt->SetOccupy(fVChrgTotal, i, occ, occ ? L"" : L"���� ����", -1);// ���� ��Ż�� �ִ´�.
	}
}




void CBiz::AddVirtualCharge(PWS vchrg, int row)
{
	ASSERT(tchlen(vchrg) > 0);
	KVCharg* vcObj = nullptr;
	if(!_mapVcObj.Lookup(vchrg, vcObj))
	{
		vcObj = new KVCharg();/// ���⼭ 48�� KSlice �� �Ҵ� �ȴ�.
		vcObj->fVChrgID = vchrg;
		vcObj->rowU = row;
		_vci[vchrg] = (int)_mapVcObj.size();// �ֱ� ���� size�� �ε��� ��ȣ _mx�� ���� ����
		_mapVcObj.SetAt(vchrg, vcObj);
	}
}

KVCharg* CBiz::getMatrix(PWS kch)
{
	// ������ �ջ��Ѱ��� Ű�� total�� �Ѵ�.
	KVCharg* pri = nullptr;
	_mapVcObj.Lookup(kch, pri);
	return pri;
}

void CBiz::SetPauseTime(PWS kvch, int iTime, CString occupy, int row)
{
	if(tchlen(kvch) == 0)///�̸� ��ο��� ��� �Ѵ�.
	{
		for(auto& [vch, vcObj] : _mapVcObj)
		{
			auto sl = vcObj->_arCell[iTime];
			sl->fPause = occupy;
		}//);
	}
	else // Ư�� fVChrgID ���Ը� ���� ���θ� ���� �Ѵ�.
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
	// fVChrgID �� ���̸� ���� �����̴� ��� �÷��� ���� ��� ����.
	if(tchlen(kvch) == 0)///�̸� ��ο��� ��� �Ѵ�.
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
	else // Ư�� fVChrgID ���Ը� ���� ���θ� ���� �Ѵ�.
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
	// fVChrgID �� ���̸� ���� �����̴� ��� �÷��� ���� ��� ����.
	int occ = 0;
	if(tchlen(kvch) == 0)//�̸� ��ο��� ��� �Ѵ�.
	{
		for(auto& [vch, vcObj] : _mapVcObj)
			//_mapVcObj.for_loop([&](TVcstr kv, TArrSl* arsl) -> void
		{
			auto sl = vcObj->_arCell[iTime];
			occ |= sl->fOff;
		}//);// ���� �� �о or ���� �ؼ� ���� �Ѵ�.
	}
	else // Ư�� fVChrgID ���Ը� ���� ���θ� ���� �Ѵ�.
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
 SelectPetList �Ҷ� fBizID(��ID)�� �ִ� ������ ���� �ϰڽ��ϴ�.
1. �ϴ� ���ִ� ���� �� ������ ��û ������ ���Դϴ�.
2. ������ ��û�� ���� �ٿ���� �׷��� ������ �ű拚 ��� �Ǵ� (������, ������, ������, Ư����) ������ �Ҽ� �ֽ��ϴ�.
3. ������ �Ǵ��� �� �ְ����̱� ������ ������ �ֽ��ϴ�.
4. ǰ��(tbreeds)�� ������������, ǰ���� ũ�� ����(tbreedsizebase) �� ����, ���� ǰ�� ũ��(tbreedsizebiz) �� ���� �ϱ� ������ fBizID�� �־����ϴ�.
5. �̰��� ����� ���� ������ ũ�⿡ ���� ������ �޶� ���ϴ�.
 */
	Rec(rec);
	Quat qs;
	qs.Field(jpa, "fBizID");//�ʼ�
	qs.Field(jpa, "fUsrID", TRUE);//�ʼ�

	if(jpa.Has("fBizID"))
	{
		qs.SetSQL(L"SELECT t.fPetID, t.fUsrIdCare, t.fBreedID, t.fAgeMonth, t.fName, t.fRegNo, t.fWeight, t.fGender, t.fNeuSurg, t.fOrder, t.fMemo, -- \n\
	r.fName fNameBreed, r.fSpecies, s.fBrdSzCD -- \n\
	FROM tmypets t -- ���⼭ ���� fBrdSzCD�� tbreeds ���� ���� �ȵȴ�.\n\
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
		//throw_response(eHttp_Not_Found, L"��ϵ� �� ������ �����ϴ�.");
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
		//AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ� SelectPetList
	} CATCH_DB;
	return 0;
}

int ApiSite1::SelectProductList(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		//AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID �ʼ� SelectPetList
		Rec(rec);
		Quat qs;
		qs.Field(jpa, "fBizID", TRUE);//�ʼ�
		//qs.Field(jpa, "fUsrID", TRUE);//�ʼ�
		qs.SetSQL(L"SELECT p.fProdID, p.fProdTpCD, p.fName, p.fShow, p.fKind, p.fElapsed, p.fOrder, p.fDesc\
			FROM tproductbiz p WHERE p.fBizID = @fBizID;");
		rec.OpenSelectFetch(qs.GetSQL());//�̶� ���� ���� ���̸� ODBC���� â�� �� ���.
		if(rec.NoData())
		{
			jrs("Return") = L"NoData";
		}
		else
		{
			MakeRecsetToJson(rec, jrs, L"table");	// tbiz
	//else
	//	throw_response(eHttp_Not_Found, L"��ϵ� �� ������ �����ϴ�.");
			SelectEnum(jrs, L"tproductbiz");
			jrs("Return") = L"OK";
		}
	} CATCH_DB;
	return 0;
}

#endif // _APIEX
