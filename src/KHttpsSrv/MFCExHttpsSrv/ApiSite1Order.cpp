#include "pch.h"
#include "ApiSite1.h"

#include "KwLib64/KRect.h"
#include "KwLib64/TimeTool.h"
#include "KwLib64/inlinefnc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif





#ifdef _APIEX

/// <summary>
/// 2021-01-25 11:22:28
/// 예약을 누르는 순간 예약 하나가 탄생 한다. 그리고 그 뒤부터는 하위 객체 생성및 편집 삭제가 이루어 진다.
/// </summary>
int ApiSite1::AddOrder1stNew(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		_db.TransBegin();// insert가 2개 이상이니 트랜잭션 있어야.
		Quat qs;
		//qs_FieldSub(qs1, inlst);
		//SET @fOrderID = w_UCast('order1-1'); --GUID
		//SET @fBizID = w_UCast('00000010');
		//SET @fUsrIdOrd = w_UCast('customer-0001');
		//SET @fUsrIdUpt = @fUsrIdOrd;
		//SET @fMemo = '2nd new order test';
		CString fOrderID = DevGuid(L"order1");

		qs.Field("fOrderID", fOrderID);
		qs.Field(jpa, "fBizID", TRUE);//필수, 샵
		qs.Field(jpa, "fUsrIdOrd", TRUE);//필수
		qs.Field(jpa, "fUsrIdUpt", TRUE);//필수
		qs.Field(jpa, "fMemo"); 
		qs.SetSQL(L"-- torder1등록\n\
INSERT INTO torder1 (fOrderID, fBizID, fUsrIdOrd, fPrevID, fState, fUsrIdUpt, fMemo)-- \n\
	VALUES(@fOrderID, @fBizID, @fUsrIdOrd, NULL, 'saved', @fUsrIdUpt, @fMemo); -- \
");
		_db.ExecuteSQL(qs);

		//-- (1-pet) 오더 신청하자 마라 내 기본 개 한마리 등록
		JObj jpa2;
		jpa2("fOrderID") = fOrderID;
		if(jpa.Has("fPetID"))
			jpa2.Copy(jpa, "fPetID");
		else
			jpa2.Copy(jpa, "fUsrIdOrd"); // 내 펫중 하나 하기 위해
		Fnc_AddOrder2ndPet(jpa2, jrs, iOp);
		/*
		{
			Rec(rec1);
			qs.SetSQL(L"-- order1 읽고\n\
SELECT fOrderID, fBizID, fUsrIdOrd, fPrevID, fState, fUsrIdReq, fUsrIdUpt, fMemo -- \n\
	FROM torder1 WHERE fOrderID = @fOrderID -- \
");
			rec1.OpenSelectFetch(qs.GetSQL());//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
			///AddPolicy(jrs, CFuncItem::eFncContract);
			if(!rec1.NoData()) // 한줄만 object로서 넣는다. array가 아니다.
				MakeRecsetOneRowToJson(rec1, jrs, L"torder1");
			else // 이건 없는데 select 할리가 없다.
				throw_response(eHttp_Bad_Request, "Error in new user order1.");//jrs("Return") = L"No Data";
		}

		{
			//jo1.get()->_bValueOwner = false; 해주지 않아도 윗줄에서 객체를 wrraping했기 때문에 false이다.
			Rec(rec2);
			qs.SetSQL(L"-- order2pet 읽고\n\
SELECT fOrdPetID, fOrderID, fPetID FROM torder2pet o2 WHERE o2.fOrderID = @fOrderID; -- \
");
			rec2.OpenSelectFetch(qs.GetSQL());//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
			if(!rec2.NoData())
			{
				JObj jo1(jrs.O("torder1"));//위에서 넣던거 object 다시 빼서 wrraping만 한뒤, order2 array를 붙인다.
				MakeRecsetToJson(rec2, jo1, L"torder2pet");
				// { "torder1" : {fOrderID:"1234",,,,"torder2pet":[{obj},{obj},{},] }
			}
			//else 내 펫이 등록 된게 없나 보다.
		}*/
		
		JObj jpa3;
		jpa3("fOrderID") = fOrderID;
		Fnc_SelectFullOrder(jpa3, jrs, iOp);
		
		jrs("Return") = L"OK";

		_db.TransCommit();
	} CATCH_DBTR;
	return 0;
}
/// <summary>
/// 2021-01-25 11:22:28
/// 강아지 하나 선택 추가 할때 들어 간다.
/// </summary>
int ApiSite1::Fnc_AddOrder2ndPet(JObj& jpa, JObj& jrs, int iOp)
{
	Quat qs;
	qs.Field(jpa, "fOrderID", TRUE);

	CString fOrdPetID = DevGuid(L"oder2");
	qs.Field("fOrdPetID", fOrdPetID);

	if(jpa.Len("fPetID"))
	{
		qs.Field(jpa, "fPetID");
		qs.SetSQL(L"INSERT INTO torder2pet (fOrdPetID, fOrderID, fPetID) VALUES -- \n\
	(@fOrdPetID, @fOrderID, @fPetID);");
	}
	else // 자동 하나 생성
	{
		qs.Field(jpa, "fUsrIdOrd", TRUE);//필수
		qs.SetSQL(L"INSERT INTO torder2pet (fOrdPetID, fOrderID, fPetID) -- \n\
	SELECT @fOrdPetID, @fOrderID, m.fPetID FROM tmypets m WHERE m.fUsrIdCare = @fUsrIdOrd ORDER BY m.fOrder LIMIT 1;");
	}
	_db.ExecuteSQL(qs);
	return 0;
}
int ApiSite1::AddOrder2ndPet(JObj& jpa, JObj& jrs, int iOp)
{
	int rv = 0;
	try
	{
		rv = Fnc_AddOrder2ndPet(jpa, jrs, iOp);

		if(rv == 0)
			rv = Fnc_SelectFullOrder(jpa, jrs, iOp);
	} CATCH_DB;
	return rv;
}


/// <summary>
/// 2021-01-25 11:22:28
/// 예약을 누르는 순간 예약 하나가 탄생 한다. 그리고 그 뒤부터는 하위 객체 생성및 편집 삭제가 이루어 진다.
/// </summary>
int ApiSite1::AddOrder3rdProduct(JObj& jpa, JObj& jrs, int iOp)
{
	int rv = 0;
	try
	{
		Quat qs;
		CString fOrdPrdID = DevGuid(L"oder3");

		qs_Field(qs, fOrdPrdID);
		qs.Field(jpa, "fOrdPetID", TRUE);//필수, 샵
		qs.Field(jpa, "fProdID", TRUE);//필수
		qs.SetSQL(L"\
INSERT INTO torder3prd-- \n\
			(fOrdPetID, fOrdPrdID, fProdID, fPrevID) VALUES-- \n\
			(@fOrdPetID, @fOrdPrdID, @fProdID, NULL); -- \
");
		_db.ExecuteSQL(qs);
		rv = Fnc_SelectFullOrder(jpa, jrs, iOp);
	} CATCH_DB;
	return rv;
}

/// <summary>
/// 2021-01-25 11:22:28
/// 예약을 누르는 순간 예약 하나가 탄생 한다. 그리고 그 뒤부터는 하위 객체 생성및 편집 삭제가 이루어 진다.
/// </summary>
int ApiSite1::AddOrder4thOption(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Quat qs;
		CString fOrdOptID = DevGuid(L"oder4");
		qs_Field(qs, fOrdOptID);
		qs.Field(jpa, "fOrdPrdID", TRUE);//필수, 샵
		qs.Field(jpa, "fPodOptID", TRUE);//필수
		qs.SetSQL(L"\
INSERT INTO torder4opt(fOrdOptID, fOrdPrdID, fPrdOptID) VALUES-- \n\
			(@fOrdOpt1111, @fOrdPrdID111, @fPodOpt11); -- \
");
		_db.ExecuteSQL(qs);

		{
			Rec(rec4);
			qs.SetSQL(L"-- orde4opt 읽고\n\
SELECT fOrdOptID, fOrdPrdID, fPrdOptID FROM torder4opt o4 WHERE fOrdOptID = @fOrdOptID; -- \
");
			rec4.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
			if(!rec4.NoData())
				MakeRecsetOneRowToJson(rec4, jrs, L"torder4opt");	// "torder2pet":[{obj},{obj},{},] }
		}
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}





int ApiSite1::DeleteOrder2ndPet(JObj& jpa, JObj& jrs, int iOp)
{
	int rv = 0;
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess
		Quat qs;
		qs.Field(jpa, "fOrdPetID", TRUE);//필수, 샵
		qs.SetSQL(L"DELETE FROM torder2pet WHERE fOrdPetID = @fOrdPetID;");
		_db.ExecuteSQL(qs);
		rv = Fnc_SelectFullOrder(jpa, jrs, iOp);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return rv;
}


int ApiSite1::DeleteOrder3rdProduct(JObj& jpa, JObj& jrs, int iOp)
{
	int rv = 0;
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess
	
		Quat qs;
		qs.Field(jpa, "fOrdPrdID", TRUE);//필수, 샵
		qs.SetSQL(L"DELETE FROM torder3prd WHERE fOrdPrdID = @fOrdPrdID;");
		_db.ExecuteSQL(qs);
		rv = Fnc_SelectFullOrder(jpa, jrs, iOp);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return rv;
}

int ApiSite1::DeleteOrder4thOption(JObj& jpa, JObj& jrs, int iOp)
{
	int rv = 0;
	try
	{
		Quat qs;
		qs.Field(jpa, "fOrdOptID", TRUE);//필수, 샵
		qs.SetSQL(L"DELETE FROM torder4opt WHERE fOrdOptID = @fOrdOptID;");
		_db.ExecuteSQL(qs);
		rv = Fnc_SelectFullOrder(jpa, jrs, iOp);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return rv;
}



int ApiSite1::LoadOrder(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Rec(rec1);
		Quat qs;
		qs.Field(jpa, "fBizID", TRUE);//필수, 샵
		qs.Field(jpa, "fUsrID", TRUE);//필수, 샵
		qs.Field(jpa, "fDay", TRUE);//필수, 샵
		qs.Field(jpa, "fAllStaff", TRUE);//필수, 샵
		qs.SetSQL(L"CALL w_select_today(@fBizID, @fUsrID, @fDay, @fAllStaff);");
		rec1.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		if(rec1.NoData()) // 업종 선택도 안하고 제휴를 요청 하다니~
			throw_response(eHttp_Bad_Request, "Error in shedule.");
		MakeRecsetToJson(rec1, jrs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}


/// <summary>
/// 오더 관련 스케쥴 하나 추가
/// </summary>
int ApiSite1::AddOrderShedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		AddOrderShedule(jpa, jrs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

void ApiSite1::AddOrderShedule(JObj& jpa, JObj& jrs)
{
	Quat qs;
	CString fOrdShedID = DevGuid(L"ordshed");
	qs_Field(qs, fOrdShedID);
	qs.Field(jpa, "fVChrgIdWrk");//필수 아님. 가상담당-유기명
	qs.Field(jpa, "fOrderID", TRUE);//필수
	qs.Field(jpa, "fBeginTo", TRUE);//필수
	qs.Field(jpa, "fUsrIdStf");//필수 이 오더 책임자.
	qs.Field(jpa, "fUsrIdUpt", TRUE);//필수 이오더 입력자.
	qs.SetSQL(L"INSERT INTO torderschd (fOrdShedID, fUsrIdStf, fVChrgIdWrk, fOrderID, fBeginTo, fEndTo, fUsrIdUpt) VALUES -- \n\
(@fOrdShedID, @fVChrgIdWrk, @fOrderID, @fBeginTo, @fEndTo, @fUsrIdUpt);");
	_db.ExecuteSQL(qs);
	jrs("fOrdShedID") = fOrdShedID;
}


int ApiSite1::MapSheduleToOrderProduct(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수
		MapSheduleToOrderProduct(jpa, jrs);
		jrs("Return") = L"OK";
	} CATCH_DB;
	return 0;
}

void ApiSite1::MapSheduleToOrderProduct(JObj& jpa, JObj& jrs)
{
	Quat qs;
	CString fShedMapID = DevGuid(L"shedmap");
	qs_Field(qs, fShedMapID);
	qs.Field(jpa, "fOrdShedID");//필수 아님. 가상담당-유기명
	qs.Field(jpa, "fOrdPrdID", TRUE);//필수
	qs.SetSQL(L"INSERT INTO tshedmap(fShedMapID, fOrdShedID, fOrdPrdID) VALUES -- \n\
(@fShedMapID, @fOrdShedID, @fOrdPrdID);");//('shedmap-0001', 'ordshed-0002', 'order3-0011');
	_db.ExecuteSQL(qs);
	jrs("fShedMapID") = fShedMapID;
}




/// <summary>
/// 오더상품 스케쥴 하나 추가 - 스케쥴 후 매핑까지
/// </summary>
int ApiSite1::AddOrderProductShedule(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		AccessRight(jpa, iOp);//iOp & eFncAccess fUsrID, fBizID 필수

		_db.TransBegin();// insert가 2개 이상이니 트랜잭션 있어야.

		AddOrderShedule(jpa, jrs);
		MapSheduleToOrderProduct(jpa, jrs);

		jrs("Return") = L"OK";
		_db.TransCommit();
	} CATCH_DBTR;
	return 0;
}



int ApiSite1::SelectFullOrder(JObj& jpa, JObj& jrs, int iOp)
{
	return Fnc_SelectFullOrder(jpa, jrs, iOp);
}
int ApiSite1::Fnc_SelectFullOrder(JObj& jpa, JObj& jrs, int iOp)
{
	try
	{
		Rec(rec);
		Quat qs;
		qs.Field(jpa, "fOrderID", TRUE);//필수
		qs.SetSQL(L"\
SELECT o1.fOrderID, o1.fBizID, o1.fUsrIdOrd, o1.fState, o1.fEditing, o1.fPriceAgree, u.fTel, u.fNickName, '//' cut1 -- \n\
	, o2.fOrdPetID, o3.fOrdPrdID , o4.fOrdOptID, '//ID:펫,상품,옵션' cut2 -- \n\
	, o2.fPetID, o3.fProdID, o4.fPrdOptID, '//주인,품종' cut3 -- \n\
	, t2.fUsrIdCare, t2.fBreedID, t2.fAgeMonth, t2.fName fNamePet, t2.fWeight, t2.fGender, t2.fNeuSurg, br.fBreedID, br.fName fNameBr, rs.fBrdSzCD, '//상품,가격' cut4 -- \n\
	, p3.fProdTpCD, p3.fName fNamePrd, p3.fKind, p3.fElapsed, p3.fDesc, pc.fPrcID, pc.fPrice fPricePrd, '//옵션' cut5 -- pc.fBrdSzCD, pc.fMinWeit, pc.fMaxWeit, \n\
	, op.fName fNameOpt, op.fType, op.fExtraTime, op.fPrice fPriceOpt, op.fPrdOptID -- \n\
	FROM torder1 o1 -- \n\
	JOIN tuser u ON o1.fUsrIdOrd = u.fUsrID -- \n\
	LEFT JOIN torder2pet o2 ON o2.fOrderID = o1.fOrderID  -- \n\
	LEFT JOIN tmypets_ord t2 ON t2.fPetID = o2.fPetID AND o2.fOrdPetID = t2.fOrdPetID -- \n\
	LEFT JOIN tbreeds br ON br.fBreedID = t2.fBreedID -- \n\
	LEFT JOIN tbreedsizebiz rs ON rs.fBreedID = br.fBreedID AND o1.fBizID = rs.fBizID -- \n\
	LEFT JOIN torder3prd o3 ON o3.fOrdPetID = o2.fOrdPetID -- \n\
	LEFT JOIN tproductbiz_ord p3 ON p3.fProdID = o3.fProdID AND o3.fOrdPrdID = p3.fOrdPrdID -- \n\
LEFT JOIN tpricebiz pc ON p3.fProdID = pc.fProdID  AND pc.fBrdSzCD = rs.fBrdSzCD AND (pc.fMinWeit <= t2.fWeight AND t2.fWeight < pc.fMaxWeit) -- 가격, 체충 고려 \n\
	LEFT JOIN torder4opt o4 ON o4.fOrdPrdID = o3.fOrdPrdID -- \n\
	LEFT JOIN tprodoptbiz_ord op ON op.fPrdOptID = o4.fPrdOptID AND o4.fOrdOptID = op.fOrdOptID -- \n\
	WHERE o1.fOrderID = @fOrderID;");
		rec.OpenSelectFetch(qs);//이때 아직 연결 전이면 ODBC선택 창이 발 뜬다.
		if(rec.NoData())
			throw_response(eHttp_Not_Found, L"등록된 예약 정보가 없습니다.");

		double fPriceTotal = 0.;
		auto& rs = rec;
		JArr ar;

		/// 절대GUID키 이므로 먼저 수집을 하고 나중에 상위 객체에 붙인다.
		KStdMap<wstring, ShJObj> gPet;
		KStdMap<wstring, int> nChildPet;

		KStdMap<wstring, ShJObj> gPrd;
		KStdMap<wstring, int> nChildPrd;

		KStdMap<wstring, ShJObj> gOpt;
		KStdMap<wstring, int> nChildOpt;

		JObj gOrder;
		for(int r = 0; r < rs.RowSize(); r++)
		{
			JObj robj;
			if(gOrder.size() == 0) //!jrs.Has("gOrder"))
			{
				rs.SetJsonS(gOrder, r, "fOrderID");
				rs.SetJsonS(gOrder, r, "fBizID");
				rs.SetJsonS(gOrder, r, "fUsrIdOrd");
				rs.SetJsonS(gOrder, r, "fTel");
				rs.SetJsonS(gOrder, r, "fNickName");
				rs.SetJsonS(gOrder, r, "fState");
				rs.SetJsonS(gOrder, r, "fEditing");
				rs.SetJsonF(gOrder, r, "fPriceAgree");//실수
				//robj("fOrderID") = rs.CellS(r, "fOrderID");
			}
			CString fOrdPetID = rs.Cell(r, "fOrdPetID");
			if(fOrdPetID.GetLength() > 0) /// 중복해서 나오니 이미 넣었나 본다.
			{
				ShJObj sjo2;
				if(!gPet.Has((PWS)fOrdPetID))
				{
					sjo2 = make_shared<JObj>();
					rs.SetJsonS(*sjo2, r, "fOrdPetID");
					rs.SetJsonS(*sjo2, r, "fPetID");
					rs.SetJsonS(*sjo2, r, "fUsrIdCare");
					rs.SetJsonS(*sjo2, r, "fBreedID");
					rs.SetJsonI(*sjo2, r, "fAgeMonth");//정수
					rs.SetJsonS(*sjo2, r, "fNamePet");
					rs.SetJsonF(*sjo2, r, "fWeight");//실수
					rs.SetJsonS(*sjo2, r, "fGender");
					rs.SetJsonS(*sjo2, r, "fNeuSurg");
					rs.SetJsonS(*sjo2, r, "fBreedID");
					rs.SetJsonS(*sjo2, r, "fNameBr");
					rs.SetJsonS(*sjo2, r, "fBrdSzCD");
					nChildPrd[(PWS)fOrdPetID] = 0;
					gPet[(PWS)fOrdPetID] = sjo2;
				}
				else
					sjo2 = gPet[(PWS)fOrdPetID];

				CString fOrdPrdID = rs.Cell(r, "fOrdPrdID");
				if(fOrdPrdID.GetLength() > 0) /// 중복해서 나오니 이미 넣었나 본다.
				{
					/// sjo2 에 fOrdPrdID 가 있는지 봐야 한다.
					/// sjo2 에 gPrd1,2,3.. 가져 와서 안에 fOrdPrdID 와 같은게 있나
					/// fOrdPrdID 가 상위 gPet 에 상관 없이 유니크 하므로 gPrd[]에 넣어 두고 검사
					ShJObj sjo3;
					if(!gPrd.Has((PWS)fOrdPrdID))
					{
						sjo3 = make_shared<JObj>();
						rs.SetJsonS(*sjo3, r, "fOrdPrdID");
						rs.SetJsonS(*sjo3, r, "fProdID");
						rs.SetJsonS(*sjo3, r, "fProdTpCD");
						rs.SetJsonS(*sjo3, r, "fNamePrd");
						rs.SetJsonS(*sjo3, r, "fKind");
						rs.SetJsonF(*sjo3, r, "fPricePrd");
						rs.SetJsonI(*sjo3, r, "fElapsed");//정수
						rs.SetJsonS(*sjo3, r, "fDesc");

						fPriceTotal += sjo3->D("fPricePrd");

						nChildOpt[(PWS)fOrdPrdID] = 0;
						gPrd[(PWS)fOrdPrdID] = sjo3;///임시저장 - 그래야 중복 체크 하지
						
						int nPrd = nChildPrd[(PWS)fOrdPetID];// 상위 fOrdPetID 밑에 상품이 몇개냐?
						nPrd++;
						CStringA gPrdN; gPrdN.Format("gPrd%d", nPrd);
						sjo2->SetObj(gPrdN, sjo3);
						nChildPrd[(PWS)fOrdPetID] = nPrd;
					}
					else
						sjo3 = gPrd[(PWS)fOrdPrdID];

					CString fOrdOptID = rs.Cell(r, "fOrdOptID");
					if(fOrdOptID.GetLength() > 0) /// 중복해서 나오니 이미 넣었나 본다.
					{
						ShJObj sjo4;
						if(!gOpt.Has((PWS)fOrdOptID))
						{
							sjo4 = make_shared<JObj>();
							rs.SetJsonS(*sjo4, r, "fOrdOptID");
							rs.SetJsonS(*sjo4, r, "fPrdOptID");
							rs.SetJsonS(*sjo4, r, "fNameOpt");
							rs.SetJsonS(*sjo4, r, "fType");
							rs.SetJsonI(*sjo4, r, "fExtraTime");//정수
							rs.SetJsonF(*sjo4, r, "fPriceOpt");//실수
							rs.SetJsonS(*sjo4, r, "fPrdOptID");

							fPriceTotal += sjo4->D("fPriceOpt");
							gOpt[(PWS)fOrdOptID] = sjo4;
							int nOpt = nChildOpt[(PWS)fOrdPrdID];
							nOpt++;
							CStringA gOptN; gOptN.Format("gOpt%d", nOpt);
							//ShJVal sjv = make_shared<JVal>(sjo, FALSE);
							sjo3->SetObj(gOptN, sjo4);//상위 JObj 에 넣어 주기
							nChildOpt[(PWS)fOrdOptID] = nOpt;
						}
						else
							sjo4 = gOpt[(PWS)fOrdOptID];// 이 밑으로 없으므로 sjo4는 더이상 쓸모 없음.
					}
				}
			}
		}


		gOrder("fPriceTotal") = fPriceTotal;
		ShJObj torder = make_shared<JObj>();
		(*torder)("gOrder") = gOrder;/// 이건 하나 밖에 없으니 바로 넣어 버린다.

		if(gPet.size() > 0)
		{
			int nPet = 0;
			for(auto& [k, sPet] : gPet)
			{
				nPet++;
				CStringA k; k.Format("gPet%d", nPet);
				(*torder)((PAS)k) = sPet;/// 이건 루트에 붙이기로 했으니.
			}
		}

		jrs("torder") = torder;/// 이건 하나 밖에 없으니 바로 넣어 버린다.

		Rec(rece);
		Quat qse;
		qse.SetSQL(L"\
SELECT e.fTable, e.fField, e.fCode, e.fKR, ( -- \n\
	case e.fTable  -- \n\
	when 'torder1' then 'gOrder' -- \n\
	when 'tuser' then 'gOrder' -- \n\
	when 'torder2pet' then 'gPet' -- \n\
	when 'tmypets' then 'gPet' -- \n\
	when 'tbreeds' then 'gPet' -- \n\
	when 'tbreedsizebiz' then 'gPet' -- \n\
	when 'torder3prd' then 'gPrd' -- \n\
	when 'tproductbiz' then 'gPrd' -- \n\
	when 'torder4opt' then 'gOpt' -- \n\
	when 'tprodoptbiz' then 'gOpt' -- \n\
	end) fGrp FROM tenum e WHERE e.fTable -- \n\
	IN ('tuser', 'tmypets', 'tbreeds', 'tproductbiz', 'tprodoptbiz', 'tbreedsizebiz', 'torder1', 'torder2pet', 'torder3prd', 'torder4opt')  -- \n\
UNION  -- \n\
SELECT 'tmypets' fTable, 'fBrdSzCD' fField, s.fBrdSzCD fCode, s.fName fKR, 'gPet' fGrp FROM tbreedsizebase s;");
		rece.OpenSelectFetch(qse);
		if(!rece.NoData())
		{
			MakeRecsetToJson(rece, jrs, L"tenum");
			///?주의:  -- \n\ 를  in 뒤에 붙이면 ODBC에서는 에러 난다. 'Unknown table 'e' in field list'
			jrs("Return") = L"OK";
		}
		else
			jrs("Return") = L"NoData";
	} CATCH_DB;
	return 0;
}

#endif // _APIEX
