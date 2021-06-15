#include "pch.h"
#include "Response1.h"
#include "KwLib64/Recordset.h"
#include "KwLib64/DlgTool.h"
#include "KwLib64/tchtool.h"
#include "KwLib64/ThreadPool.h"
#include "KwLib64/Lock.h"
#include "KwLib64/HttpError.h"

#include "ApiBase.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNAMIC(KException, CException)




CResponse1::CResponse1(shared_ptr<CApiBase> api)//, BOOL bOwner)
	: _api(api)//, _bOwner(bOwner)
{
	_api->_resp = this;//상호참조
}

CResponse1::~CResponse1()
{
	//if(_bOwner && _api)
	//	DeleteMeSafe(_api);
}

void CResponse1::InitReponser(BOOL bDB) {
	//_api->MapRemoteFunctions();// shared_ptr<ApiBase> 했더니, virtural override call이 안되네.
	auto api = _api.get();
	api->MapRemoteFunctions();//이렇게 해도. override call이 안되네.
	if(bDB)
		_api->CheckDB();
}

string JError(PAS msg, int rv, int status)
{
	// S_FALSE = 1  FAILED = -1  S_OK = 0
	Tas ss;
	ss << "{\"error\":\"" << msg << "\", \"return\":" << rv << "\", \"status\":" << status << "}";
	return ss.str();
}

void JError(JObj& jres, PWS msg, int rv, int status)
{
	jres("error") = msg;//KException::m_strError
	jres("return") = rv;//KException::m_nRetCode //return -1; 이 앱애서 의미있는 구분 - 이면 오류. + 이면 상황
	jres("status") = status;//KException::_status HTTP Request 일때 사용
}

int CResponse1::ResponseFileForGet(string url, KBinary& fbuf)
{
	int rv = 0;//http://v2.petme.kr:19479/files/images/org/8D23D27A-4286-4308-B8F1-EF17916B19EF.jpeg
	size_t is0 = url.find((PAS)_api->_rootURL);//, 0Ui64); _rootURL 는 항상 '/'
//	size_t is1 = url.find('/', 1);//, 0Ui64);
	try {
		if(is0 == 0)//URL root와 일치
			rv = _api->FileOpen(url, fbuf);
		else return 2;

		//if (is1 != string::npos) {
		//	string root = url.substr(0, is1);
		//	if (root == (PAS)_api->_rootURL)
		//	{
		//		rv = _api->FileOpen(url, fbuf);
		//	}
		//}
		//else return 2;
	}
	catch (CException* e)
	{//모두 KException 로 바꿔서 rethrow하므로 여기로 오면 호출 하다 의외 심각한 오류일수있다.
		//CString sError;
		//e->GetErrorMessage(sError.GetBuffer(1024), 1024); sError.ReleaseBuffer();
		//DWORD err = GetLastError();
		//rv = err;
		//if (sError.IsEmpty())
		//	sError = L"Unknown GET Error.";
		//JError(jres, sError, err);
		_api->LogExcepFN(e);//CException에는 FILE, LINE이 없어 오류는 파라미터 챙길게 많다.
		e->Delete();
		return 1;
	}
	return rv;
}
int CResponse1::ResponseForGet(KSessionInfo& sinfo, stringstream& rpost)
{
	//static int si2 = 0;
	//rpost << "{"func":"Test", "mode":"GET", "msg":"This is reponse of POST.", "serial": " << (si2++) << "}";
	//return 0;
	BACKGROUND(1);
	int rv = 0;
	CStringW uuidW;
	JObj jres;
	//JObj jreq;// GET 이므로 모든 파라미터는 jreq에 1차원으로 배열 된다.
	string url = sinfo._url;
	JObj jpa;// = jbj.O("params");
	CStringW sUrl(url.c_str());
	try {
		//CStringW sessionId = KwGetFormattedGuid(true); // ON DUPLICATE KEY 를 쓰지 않은 이유는, 덮어 쓰지 않도록
		size_t iq = url.find('?');//, 0Ui64);
		string url3 = url.substr(iq+1);
		int np = KwUrlParamToJsonT(url3, jpa);
		CString uparams(url3.c_str());

		CStringW func = jpa.S("func");//jreq이 아니고 jpa(파라미터)에 다 있다.
		uuidW = jpa.S("uuid");

		DWORD thID = ::GetCurrentThreadId();//이거가 세션 threadID
		ULONGLONG tick = GetTickCount64();
		string UA_CPU;
		string UserAgent;
		sinfo._headers.Lookup("UA-CPU", UA_CPU);
		sinfo._headers.Lookup("User-Agent", UserAgent);
		auto ip = sinfo._ip;

		KAtEnd d_log([&, uparams]() {//동기 람다인 경우는 & 만 있으면 된다.
			//CStringW resp(rpost.str().c_str());//비동기 람다인 경우는 스택이 사라지므로 람다로 전달할 값을 복사한다.
			string respUtf8 = rpost.str();//비동기 람다인 경우는 스택이 사라지므로 람다로 전달할 값을 복사한다.
			auto run = [&, thID, uuidW, func, uparams, respUtf8, tick, rv, UA_CPU, UserAgent, ip]() -> void
			{
				BACKGROUND(2);
				ULONGLONG elapsed = GetTickCount64() - tick;
				//?warning 이거 비동기로 하면 'Packets out of order'DB에서 계속 에러 난다. CS가 아닌 Mutex로 하니 별탈 없음.
				_api->LogRequest(L"GET", CStringW(ip.c_str()), uuidW, uparams, func, "", respUtf8, elapsed, rv, UA_CPU, UserAgent);

				/// db Log를 위해 세션:원격함수:쿼리를 채워 두고 세션 끝날때 저장 한다.
				_api->DbQueryLog(thID, func, uuidW);
			};
			QueueFUNCN(run, "LogRequest");
		});

		CStringA fncA(func);

		CFuncItem* funcItm = nullptr;
		//shared_ptr<function<int(JObj&, JObj&, int)>>* funcItm = nullptr;
		if(_api->m_mapRFncs.Lookup(fncA, funcItm))//jproc);
		{
			JObj jsr;
			int iOp = funcItm->_iOp;

			//if((iOp & CFuncItem::eFncNoDB) == 0)//이거 무의미 : logException하려면 DB항상 살아야.
			_api->CheckDB();

			if(iOp & eFncPolicy)//정책은 함수 부르기 전에
			{
				JObj jpolicy;
				_api->GetPolicy(jpa, jpolicy);
				jres("policy") = jpolicy;
			}

			// 드디어 원격 함수를 부른다.
			//rv = (_api->*(funcItm._func))(jpa, jsr, iOp);//ok
			rv = (*funcItm->_func.get())(jpa, jsr, iOp);//ok

			if(rv == 0)
			{
				if(jsr.size() > 0)
					jres("response") = jsr;// {Return:OK/Exists/No Data, Desc}
				jres("return") = rv;//S_OK 나 S_FALSE
			}
			else if(rv > 0)// jsr안에 Return:why 들어 있겠고, 더불어 error, return 도 넣는다.
			{
				jres("error") = "API Error.";
				jres("return") = rv;//S_OK 나 S_FALSE
			}// rv < 0 이면 call한 쪽에서 Unkown Error를 넣는다.

			rpost << jres.ToJsonStringUtf8();
			return rv;
		}
	}
	catch(KException* e)
	{
		_api->LogException(e, uuidW);//오류는 파라미터 챙길게 많아서 동기로 처리 한다.
		sinfo._status = e->_status == 200 ? eHttp_Bad_Request : e->_status;
		JError(jres, e->m_strError, e->m_nRetCode, sinfo._status);
		rv = e->_error;
	}
	catch(CException* e)
	{//모두 KException 로 바꿔서 rethrow하므로 여기로 오면 호출 하다 의외 심각한 오류일수있다.
		CString sError;
		e->GetErrorMessage(sError.GetBuffer(1024), 1024); sError.ReleaseBuffer();
		DWORD err = GetLastError();
		rv = err;
		if (sError.IsEmpty())
			sError = L"Expectation_Failed.";
		sinfo._status = eHttp_Expectation_Failed;
		JError(jres, sError, err, sinfo._status);// 417); eHttp_Internal_Server_Error = 500,
		_api->LogExcepFN(e);//CException에는 FILE, LINE이 없어 오류는 파라미터 챙길게 많다.
		e->Delete();
	}
	rpost << jres.ToJsonStringUtf8();//error도 JSON으로 리턴한다.
	return rv;
}

int CResponse1::ResponseForPost(KSessionInfo& sinfo, shared_ptr<KBinData> body, stringstream& rpost)
{
	BACKGROUND(1);
	int rv = 0;//원격함수 리턴값
	int status = eHttp_OK;// 이함수가 리턴할 status 200이 기본
	CString requ;
	KwUTF8ToWchar(body.get()->m_p, requ);
	string url = sinfo._url;
	JObj jres;
	JSONValue* jdoc = NULL;
	CString uuidW;
	DWORD thID = ::GetCurrentThreadId();//이거가 세션 threadID
	CStringW func;
	CString uparams;
	ULONGLONG tick = 0;// GetTickCount64();
	
	string UA_CPU;
	string UserAgent;
// 	+["UA-CPU"]	"AMD64"	std::pair<std::string const, std::string>
// 	+["User-Agent"]	"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.2; Win64; x64; Trident/7.0; .NET4.0C; .NET4.0E; .NET CLR 2.0.50727; .NET CLR 3.0.30729; .NET CLR 3.5.30729)"	std::pair<std::string const, std::string>
	sinfo._headers.Lookup("UA-CPU", UA_CPU);
	sinfo._headers.Lookup("User-Agent", UserAgent);
	auto ip = sinfo._ip;
	/// d_log 를 밖으로 뺀 이유: 그래야 오류 났을때 catch를 거치면서 error 메시지를 챙길수 있기 때문이다. 안으로 넣으면 오류 났을때 빈것이 날라 간다.
	KAtEnd d_log([&, requ]()
		{//동기 람다인 경우는 & 만 있으면 된다.
//CStringW resp(rpost.str().c_str());//비동기 람다인 경우는 스택이 사라지므로 람다로 전달할 값을 복사한다.
			rpost << jres.ToJsonStringUtf8();
			string requUtf8 = body->m_p ? body->m_p : "(no post data)";
			string respUtf8 = rpost.str();//비동기 람다인 경우는 스택이 사라지므로 람다로 전달할 값을 복사한다.
			auto run = [&, thID, uuidW, func, uparams, requUtf8, respUtf8, tick, rv, UA_CPU, UserAgent, ip]() -> void
			{
				BACKGROUND(2);
				ULONGLONG elapsed = GetTickCount64() - tick;
				//?warning 이거 비동기로 하면 'Packets out of order'DB에서 계속 에러 난다. CS가 아닌 Mutex로 하니 별탈 없음.
				_api->LogRequest(L"POST", CStringW(ip.c_str()), uuidW, uparams, func, requUtf8, respUtf8, elapsed, rv, UA_CPU, UserAgent);

				/// db Log를 위해 세션:원격함수:쿼리를 채워 두고 세션 끝날때 저장 한다.
				_api->DbQueryLog(thID, func, uuidW);
			};
			QueueFUNCN(run, "LogRequest");
		});

	try {
		//CStringW sessionId = KwGetFormattedGuid(true); // ON DUPLICATE KEY 를 쓰지 않은 이유는, 덮어 쓰지 않도록

		auto jdoc = ShJVal(Json::Parse((PWS)requ));
		if(jdoc.get() == nullptr)
			throw new KException("JSON Parse Error.", GetLastError(), 0, L"JSON syntax is wrong.", NULL, __FUNCTION__, __LINE__);
		auto& jbj = *jdoc->AsObject().get();

		//jdoc = JSON::Parse((PWS)requ);//bin.GetPA());//constructor가 불린다.
		//if(jdoc == NULL)// 가끔 생기네.
		//	throw new KException("JSON Parse Error.", GetLastError(), 0, L"JSON syntax is wrong.", NULL, __FUNCTION__, __LINE__); 
		//KAtEnd d_jdoc([&]() {	DeleteMeSafe(jdoc);	});
		//JObj jbj(jdoc->AsObject());
		size_t iq = url.find('?');//, 0Ui64);
		string url3 = url.substr(iq + 1);
		uparams = CStringW(url3.c_str());

		func = jbj.S("func");
		
		uuidW = jbj.S("uuid");//POST 인 경우는 JSON 데이터 안에 있다. GET과 ImageUpload는 URL에 있어서 ss->_sinfo._urlparam["uuid"]로 얻는다.

		tick = GetTickCount64();
		CStringA fncA(func);
		JObj jpa = jbj.Oref("params");
		JObj jsr;

		_api->CheckDB();
	
		CFuncItem* funcItm = nullptr;
		if(_api->m_mapRFncs.Lookup(fncA, funcItm))//jproc);
		{
			int iOp = funcItm->_iOp;


			if(iOp & eFncPolicy)//정책은 함수 부르기 전에
			{
				JObj jpolicy;
				_api->GetPolicy(jpa, jpolicy);
				jres("policy") = jpolicy;
			}

			// 드디어 원격 함수를 부른다. Member function pointer를 객체 _api의 멤버 메소드로 부른다.
			rv = (*funcItm->_func)(jpa, jsr, iOp);//ok

			if(rv == 0)
			{
				//AddPolicy(jsr, sFuncA);// jsr에 policy를 붙인다. 전에는 더 위층에 했으나
				if(jsr.size() > 0)
					jres("response") = jsr;// {Return:OK/Exists/No Data, Desc}
				jres("return") = rv;//S_OK 나 S_FALSE
			}
			else if (rv > 0)// jsr안에 Return:why 들어 있겠고, 더불어 error, return 도 넣는다.
			{
				//https://developer.mozilla.org/ko/docs/Web/HTTP/Status
				// 400 Bad Request
				// 401 Unauthorized
				jres("error") = "API Error.";
				jres("return") = rv;//S_OK 나 S_FALSE
			}// rv < 0 이면 call한 쪽에서 Unkown Error를 넣는다.

			///rpost << jres.ToJsonStringUtf8(); KAtEnd 에서 하므로 여기서 제거
			return status;
		}
		else//외부 DLL함수를 디진다
		{
			// int(*)(KDatabase&, Kw::JObj&, Kw::JObj&, int)
			if(CApiBase::s_hDllExtra == NULL)
			{
				CApiBase::s_hDllExtra = ::LoadLibraryW(_api->getExLibName());// L"MFCLibrary1.dll");
				if(CApiBase::s_hDllExtra == NULL)
				{
					CStringA s; s.Format("Library is not found including (%s).", fncA);
					throw_response(eHttp_Not_Found, s);
				}
			}
			API_SITE ExDllApiFunc = (API_SITE)::GetProcAddress(CApiBase::s_hDllExtra, (PAS)fncA);
			if(ExDllApiFunc)
			{
				rv = ExDllApiFunc(_api->_db, jpa, jsr, 0);
				if(rv == 0)
				{
					if(jsr.size() > 0)
						jres("response") = jsr;// {Return:OK/Exists/No Data, Desc}
					jres("return") = rv;//S_OK 나 S_FALSE
				}
				else if(rv > 0)// jsr안에 Return:why 들어 있겠고, 더불어 error, return 도 넣는다.
				{///여기 올일이 없다. throw_response 하기 때문
					jres("error") = "API Error.";
					jres("return") = rv;//S_OK 나 S_FALSE
				}// rv < 0 이면 call한 쪽에서 Unkown Error를 넣는다.
				return status;
			}
			else
			{
				CStringA s; s.Format("Function is not found.(%s)", fncA);
				throw_response(eHttp_Not_Found, s);
			}
		}
	}
	catch(KException* e)
	{//API중에 오류는 모두 여기로 온다.
		if(e->_sExcept == L"CDBException")
		{
			sinfo._status = e->_status == 200 ? eHttp_Expectation_Failed : e->_status;
			JError(jres, L"Data Base Error.", e->m_nRetCode, sinfo._status);
		}
		else
		{
			sinfo._status = e->_status == 200 ? eHttp_Bad_Request : e->_status;
			JError(jres, e->m_strError, e->m_nRetCode, sinfo._status);
		}
		_api->LogException(e, uuidW);//오류는 파라미터 챙길게 많아서 동기로 처리 한다.
		
		status = sinfo._status;// e->_error;// 400; status
		e->Delete();//DeleteMeSafe(e);
	}
	catch(CException* e)
	{//모두 KException 로 바꿔서 rethrow하므로 여기로 오면 호출 하다 의외 심각한 오류일수있다.
		CString sError;
		e->GetErrorMessage(sError.GetBuffer(1024), 1024); sError.ReleaseBuffer();
		DWORD err = GetLastError();
		if (sError.IsEmpty())
			sError = L"Unknown POST Error.";
		
		sinfo._status = eHttp_Expectation_Failed;
		status = sinfo._status;// e->_error;// 400; status
		JError(jres, sError, err, sinfo._status);// 417); eHttp_Internal_Server_Error = 500,
		_api->LogExcepFN(e);//CException에는 FILE, LINE이 없어 오류는 파라미터 챙길게 많다.
		e->Delete();//DeleteMeSafe(e);
	}
	//rpost << jres.ToJsonStringUtf8();//error도 JSON으로 리턴한다.
	return status;
}

//int CResponse1::ResponseImageUpload(HTTPSCacheSession* ss, string ext, stringstream& rpost)

int CResponse1::ResponseImageUpload(KSessionInfo& sinfo, string uuid, PAS data, size_t len, string fname,
								string contType, string contLeng, Tas& rpost)
{
	ASSERT(tchstrx(contType.c_str(), "image/") == 0);
	string url = sinfo._url;
	int rv = 0;
	CStringA requA, sReqUtf8;
	requA.Format("%s, %s, %s", contType.c_str(), contLeng.c_str(), fname.c_str());
	//CStringW requ(requA);
	KwCharToUTF8(requA, sReqUtf8);
	//CString uuid;//접속자
	CStringW func = L"ImageUpload";
	JObj jres;
	CStringW uuidW(uuid.c_str());
	try {
		_api->CheckDB();
		size_t iq = url.find('?');//, 0Ui64);
		string url3 = url.substr(iq + 1);
		CStringW uparams(url3.c_str());

		//JObj jsr; 이거는 JSON 요청때만 쓴다.
		string UA_CPU;
		string UserAgent;
		sinfo._headers.Lookup("UA-CPU", UA_CPU);
		sinfo._headers.Lookup("User-Agent", UserAgent);

		DWORD thID = ::GetCurrentThreadId();//이거가 세션 threadID
		ULONGLONG tick = GetTickCount64();
		auto ip = sinfo._ip;
		KAtEnd d_log([&]() {//동기 람다인 경우는 & 만 있으면 된다.
			///string jresU8 = jres.ToJsonStringUtf8();//여기서 이미지 작업은 끝나서 리턴 하므로 utf8로 바꾼다.
			// 웃줄 들어가면  	MFCAppServerEx2.exe!JSONObject::~JSONObject() 줄 321	C++
			//MFCAppServerEx2.exe!JSONValue::~JSONValue() 줄 559	C++
			//	MFCAppServerEx2.exe!JObj::ToJsonStringW() 줄 1270	C++
			// 에서 죽는다.
			/// Tas를 쓰니 안죽는다. 그럼 아래 QueueFUNCN 도 살려 보자.
			/// 원인: jres가 동기콜 KAtEnd에서 쓰면 함수리턴후 jres작업에서 죽는다.
			CStringW resp(rpost.str().c_str());//비동기 람다인 경우는 스택이 사라지므로 람다로 전달할 값을 복사한다.
			string requUtf8 = sReqUtf8;// body.get()->m_p;
			string respUtf8 = rpost.str();//비동기 람다인 경우는 스택이 사라지므로 람다로 전달할 값을 복사한다.
			//auto run = [&, thID, uuidW, func, uparams, requUtf8, respUtf8, tick, rv]() -> void
			auto run = [&, thID, uuidW, func, uparams, requUtf8, respUtf8, tick, rv, UA_CPU, UserAgent]() -> void
			{
				BACKGROUND(2);
				ULONGLONG elapsed = GetTickCount64() - tick;
				//?warning 이거 비동기로 하면 'Packets out of order'DB에서 계속 에러 난다. CS가 아닌 Mutex로 하니 별탈 없음.
				_api->LogRequest(L"POST", CStringW(ip.c_str()), uuidW, uparams, func, requUtf8, respUtf8, elapsed, rv, UA_CPU, UserAgent);

				/// db Log를 위해 세션:원격함수:쿼리를 채워 두고 세션 끝날때 저장 한다.
				_api->DbQueryLog(thID, func, uuidW);
			};
			QueueFUNCN(run, "LogRequest");
		});

		string ext = contType.substr(6);//jpeg, png, PNG
		JObj jsr;
		if(tchisame(ext.c_str(), "jpeg") || tchisame(ext.c_str(), "jpg") || tchisame(ext.c_str(), "png"))
		{//image/gif, image/png, image/jpeg, image/bmp, image/webp
			//string fname = ss->_urlparam["filename"];

			//1 서버입장에서 다운로드
			//2. DB에 키와 경로로 저장. 루트는 빼고
			rv = _api->ImageUpload(data, len, fname, contType.c_str(), contLeng.c_str(), jsr);
			if (rv == 0)
			{
				if (jsr.size() > 0)
					jres("response") = jsr;// {Return:OK/Exists/No Data, Desc}
				jres("return") = rv;//S_OK 나 S_FALSE
			}
		}
		else
		{
			rv = 2;//양수를 줘야 에러메시지가 응답된다.
			CString ser; ser.Format(L"Not Supported image type.(%s)", CStringW(ext.c_str()));
			sinfo._status = eHttp_Unsupported_Media_Type;
			JError(jres, ser, rv, sinfo._status);//415
		}
		rpost << jres.ToJsonStringUtf8();
		return rv;
	}
	catch(KException* e)//rethrow 한것을 처리 한다.
	{
		_api->LogException(e, uuidW);// L"ImageDownLoad");//오류는 파라미터 챙길게 많아서 동기로 처리 한다.
		sinfo._status = e->_status == 200 ? eHttp_Bad_Request : e->_status;
		JError(jres, e->m_strError, e->_error, sinfo._status);//417
		rv = e->m_nRetCode;// -100;
	}
	catch(CException* e)
	{//모두 KException 로 바꿔서 rethrow하므로 여기로 오면 호출 하다 의외 심각한 오류일수있다.
		CString sError;
		e->GetErrorMessage(sError.GetBuffer(1024), 1024); sError.ReleaseBuffer();
		DWORD err = GetLastError();
		rv = err;
		if (sError.IsEmpty())
			sError = L"Unknown ImageUpload Error.";
		sinfo._status = eHttp_Expectation_Failed;
		JError(jres, sError, err, sinfo._status);
		_api->LogExcepFN(e);//CException에는 FILE, LINE이 없어 오류는 파라미터 챙길게 많다.
	}
	rpost << jres.ToJsonStringUtf8();//error도 JSON으로 리턴한다.
	return rv;
}
