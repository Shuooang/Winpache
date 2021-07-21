#include "pch.h"
#include "framework.h"
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








}//extern "C" {
