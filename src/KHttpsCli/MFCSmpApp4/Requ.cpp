#include "pch.h"
#include "Requ.h"
#include "MFCSmpApp4.h"

#include "KwLib64/HTTPS/HttpClient.h"
#include "KwLib64/tchtool.h"
#include <processenv.h>

KRequ::KRequ(PWS usrID)
{
	if(tchlen(usrID) > 0)
		_UsrID = usrID;
	else
	{
		auto app = (CSmpApp4*)AfxGetApp();
		_UsrID = app->getLoginData("fUsrID");
		_BizID = app->getLoginData("fBizID");
	}
	//if(tchlen(bizID) > 0)
	//	_bizID = bizID;
	//else
	//{
	//	auto app = (CSmpApp4*)AfxGetApp();
	//	_bizID = app->getBizID();
	//}

#ifdef DEBUG
	CString s;
	PWS pw = s.GetBuffer(100);
	GetEnvironmentVariableW(L"USERNAME", (LPWSTR)pw, 100);
	s.ReleaseBuffer();
// 	if(s == L"dwkang")
// 		_dmn = L"localhost";
// 	else
// 		_dmn = L"codi.xxxx.co.kr";//L"192.168.0.170";//petme�繫�� //ȸ�ǽ�
#endif
}

void KRequ::RequestPost(PAS func, JObj& jin, JObj& jout)
{
//	PAS prefix = "CSmpView::RM_";// GetTodaySchdule"	const char*
	int lpre = (int)tchstrx(func, "::RM_");
	if(lpre >= 0)
		lpre += 5;// prefix);
	else
		lpre = 0;
	//ASSERT(lpre > 0);
	CStringA sfunc = (func + lpre);

	CString surl = getUrl(sfunc);
	CHttpClient cl;
	KBinary bin, binr;
	JObj jrq;//CJsonPbj

	/// ���ϱ� ������ ���� �α����� ����� ID �� ��û �ʵ��� fUsrID �� �򰥸��� �־ ����.
	//if(_uuid.IsEmpty() && jin.Has("fUsrID"))
	//	_uuid = jin.S("fUsrID");

	jrq("uuid") = _UsrID; /// ���� �α��� �����ID (�α���ID �ƴ�)
	jrq("func") = CStringW(sfunc);/// ����Ʈ �Լ���
	jrq("params") = jin;         /// �Ķ����


	/// �Ķ���� ��
	/*
	{
		"fAllStaff":1, "fBizID" : "biz-0002", "fDay" : "2021-02-09", "fUserID" : "staff-a021"
	}
	*/
	/// ���俹
/*	{
  "response":{
	"Return":"OK","charge":{
	  "vcharge-0002":{
	  }
*/





	std::map<string, string> hdrs;
	//	hdrs["Cache-Control"] = "no-cache";
	hdrs["Content-Type"] = "application/json";
	//	hdrs["Content-Length"] = slenA;
	string sjsn = jrq.ToJsonStringUtf8();
	bin.SetPtr(sjsn.c_str(), sjsn.length());
	hres hr = cl.RequestPostSSL(surl, &binr, &bin, &hdrs);

	CString sjsnR;
	KwUTF8ToWchar(binr.GetPA(), sjsnR);//binr�� ���ڿ��� c style �� �ڿ� '\0'�� �ٿ��ش�.
	ShJVal jdoc = Json::Parse((PWS)sjsnR);
	if(jdoc.get())
	{
		//KAtEnd d_jdoc([&]() { DeleteMeSafe(jdoc); });
		JObj& jres = *jdoc->AsObject().get();
		auto rv = jres.I("return");
		if(rv == 0)
		{
			if(jres.Has("response"))
			{
				auto response = jres.O("response");
				jout.Clone(response, true);// jdoc->AsObject());
			}
		}
		else
		{
			auto error = jres.S("error");
			if(_errBox)
				KwMessageBox(L"Server Error: %s", error);
			else
				KTrace(L"Server Error: %s", error);
		}
	}
	else
	{
		TRACE("Response is NULL. (JSON::Parse)");
	}
}
/*
{
  "Return":"OK",
  "table":[
	{  "fAnimal":null,"fBegin":"2021-02-19 10:00:00","fBizID":"biz-0002","fDist":0.888042173392903,"fEnd":"2021-02-19 18:00:00","fLat":37,"fLon":127.01,"fState":null,"fTel":null,"fTitle":"�ι�°��"
	},
	{  "fAnimal":"dog","fBegin":"2021-02-19 10:00:00","fBizID":"biz-0001","fDist":8.88042128765225,"fEnd":"2021-02-19 18:00:00","fLat":37,"fLon":127.1,"fState":null,"fTel":null,"fTitle":"ù��°��"
	}
  ]
}
*/
