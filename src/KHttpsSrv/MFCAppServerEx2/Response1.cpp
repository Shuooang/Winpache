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
	_api->_resp = this;//��ȣ����
}

CResponse1::~CResponse1()
{
	//if(_bOwner && _api)
	//	DeleteMeSafe(_api);
}

void CResponse1::InitReponser(BOOL bDB) {
	//_api->MapRemoteFunctions();// shared_ptr<ApiBase> �ߴ���, virtural override call�� �ȵǳ�.
	auto api = _api.get();
	api->MapRemoteFunctions();//�̷��� �ص�. override call�� �ȵǳ�.
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
	jres("return") = rv;//KException::m_nRetCode //return -1; �� �۾ּ� �ǹ��ִ� ���� - �̸� ����. + �̸� ��Ȳ
	jres("status") = status;//KException::_status HTTP Request �϶� ���
}

int CResponse1::ResponseFileForGet(string url, KBinary& fbuf)
{
	int rv = 0;//http://v2.petme.kr:19479/files/images/org/8D23D27A-4286-4308-B8F1-EF17916B19EF.jpeg
	size_t is0 = url.find((PAS)_api->_rootURL);//, 0Ui64); _rootURL �� �׻� '/'
//	size_t is1 = url.find('/', 1);//, 0Ui64);
	try {
		if(is0 == 0)//URL root�� ��ġ
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
	{//��� KException �� �ٲ㼭 rethrow�ϹǷ� ����� ���� ȣ�� �ϴ� �ǿ� �ɰ��� �����ϼ��ִ�.
		//CString sError;
		//e->GetErrorMessage(sError.GetBuffer(1024), 1024); sError.ReleaseBuffer();
		//DWORD err = GetLastError();
		//rv = err;
		//if (sError.IsEmpty())
		//	sError = L"Unknown GET Error.";
		//JError(jres, sError, err);
		_api->LogExcepFN(e);//CException���� FILE, LINE�� ���� ������ �Ķ���� ì��� ����.
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
	//JObj jreq;// GET �̹Ƿ� ��� �Ķ���ʹ� jreq�� 1�������� �迭 �ȴ�.
	string url = sinfo._url;
	JObj jpa;// = jbj.O("params");
	CStringW sUrl(url.c_str());
	try {
		//CStringW sessionId = KwGetFormattedGuid(true); // ON DUPLICATE KEY �� ���� ���� ������, ���� ���� �ʵ���
		size_t iq = url.find('?');//, 0Ui64);
		string url3 = url.substr(iq+1);
		int np = KwUrlParamToJsonT(url3, jpa);
		CString uparams(url3.c_str());

		CStringW func = jpa.S("func");//jreq�� �ƴϰ� jpa(�Ķ����)�� �� �ִ�.
		uuidW = jpa.S("uuid");

		DWORD thID = ::GetCurrentThreadId();//�̰Ű� ���� threadID
		ULONGLONG tick = GetTickCount64();
		string UA_CPU;
		string UserAgent;
		sinfo._headers.Lookup("UA-CPU", UA_CPU);
		sinfo._headers.Lookup("User-Agent", UserAgent);
		auto ip = sinfo._ip;

		KAtEnd d_log([&, uparams]() {//���� ������ ���� & �� ������ �ȴ�.
			//CStringW resp(rpost.str().c_str());//�񵿱� ������ ���� ������ ������Ƿ� ���ٷ� ������ ���� �����Ѵ�.
			string respUtf8 = rpost.str();//�񵿱� ������ ���� ������ ������Ƿ� ���ٷ� ������ ���� �����Ѵ�.
			auto run = [&, thID, uuidW, func, uparams, respUtf8, tick, rv, UA_CPU, UserAgent, ip]() -> void
			{
				BACKGROUND(2);
				ULONGLONG elapsed = GetTickCount64() - tick;
				//?warning �̰� �񵿱�� �ϸ� 'Packets out of order'DB���� ��� ���� ����. CS�� �ƴ� Mutex�� �ϴ� ��Ż ����.
				_api->LogRequest(L"GET", CStringW(ip.c_str()), uuidW, uparams, func, "", respUtf8, elapsed, rv, UA_CPU, UserAgent);

				/// db Log�� ���� ����:�����Լ�:������ ä�� �ΰ� ���� ������ ���� �Ѵ�.
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

			//if((iOp & CFuncItem::eFncNoDB) == 0)//�̰� ���ǹ� : logException�Ϸ��� DB�׻� ��ƾ�.
			_api->CheckDB();

			if(iOp & eFncPolicy)//��å�� �Լ� �θ��� ����
			{
				JObj jpolicy;
				_api->GetPolicy(jpa, jpolicy);
				jres("policy") = jpolicy;
			}

			// ���� ���� �Լ��� �θ���.
			//rv = (_api->*(funcItm._func))(jpa, jsr, iOp);//ok
			rv = (*funcItm->_func.get())(jpa, jsr, iOp);//ok

			if(rv == 0)
			{
				if(jsr.size() > 0)
					jres("response") = jsr;// {Return:OK/Exists/No Data, Desc}
				jres("return") = rv;//S_OK �� S_FALSE
			}
			else if(rv > 0)// jsr�ȿ� Return:why ��� �ְڰ�, ���Ҿ� error, return �� �ִ´�.
			{
				jres("error") = "API Error.";
				jres("return") = rv;//S_OK �� S_FALSE
			}// rv < 0 �̸� call�� �ʿ��� Unkown Error�� �ִ´�.

			rpost << jres.ToJsonStringUtf8();
			return rv;
		}
	}
	catch(KException* e)
	{
		_api->LogException(e, uuidW);//������ �Ķ���� ì��� ���Ƽ� ����� ó�� �Ѵ�.
		sinfo._status = e->_status == 200 ? eHttp_Bad_Request : e->_status;
		JError(jres, e->m_strError, e->m_nRetCode, sinfo._status);
		rv = e->_error;
	}
	catch(CException* e)
	{//��� KException �� �ٲ㼭 rethrow�ϹǷ� ����� ���� ȣ�� �ϴ� �ǿ� �ɰ��� �����ϼ��ִ�.
		CString sError;
		e->GetErrorMessage(sError.GetBuffer(1024), 1024); sError.ReleaseBuffer();
		DWORD err = GetLastError();
		rv = err;
		if (sError.IsEmpty())
			sError = L"Expectation_Failed.";
		sinfo._status = eHttp_Expectation_Failed;
		JError(jres, sError, err, sinfo._status);// 417); eHttp_Internal_Server_Error = 500,
		_api->LogExcepFN(e);//CException���� FILE, LINE�� ���� ������ �Ķ���� ì��� ����.
		e->Delete();
	}
	rpost << jres.ToJsonStringUtf8();//error�� JSON���� �����Ѵ�.
	return rv;
}

int CResponse1::ResponseForPost(KSessionInfo& sinfo, shared_ptr<KBinData> body, stringstream& rpost)
{
	BACKGROUND(1);
	int rv = 0;//�����Լ� ���ϰ�
	int status = eHttp_OK;// ���Լ��� ������ status 200�� �⺻
	CString requ;
	KwUTF8ToWchar(body.get()->m_p, requ);
	string url = sinfo._url;
	JObj jres;
	JSONValue* jdoc = NULL;
	CString uuidW;
	DWORD thID = ::GetCurrentThreadId();//�̰Ű� ���� threadID
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
	/// d_log �� ������ �� ����: �׷��� ���� ������ catch�� ��ġ�鼭 error �޽����� ì��� �ֱ� �����̴�. ������ ������ ���� ������ ����� ���� ����.
	KAtEnd d_log([&, requ]()
		{//���� ������ ���� & �� ������ �ȴ�.
//CStringW resp(rpost.str().c_str());//�񵿱� ������ ���� ������ ������Ƿ� ���ٷ� ������ ���� �����Ѵ�.
			rpost << jres.ToJsonStringUtf8();
			string requUtf8 = body->m_p ? body->m_p : "(no post data)";
			string respUtf8 = rpost.str();//�񵿱� ������ ���� ������ ������Ƿ� ���ٷ� ������ ���� �����Ѵ�.
			auto run = [&, thID, uuidW, func, uparams, requUtf8, respUtf8, tick, rv, UA_CPU, UserAgent, ip]() -> void
			{
				BACKGROUND(2);
				ULONGLONG elapsed = GetTickCount64() - tick;
				//?warning �̰� �񵿱�� �ϸ� 'Packets out of order'DB���� ��� ���� ����. CS�� �ƴ� Mutex�� �ϴ� ��Ż ����.
				_api->LogRequest(L"POST", CStringW(ip.c_str()), uuidW, uparams, func, requUtf8, respUtf8, elapsed, rv, UA_CPU, UserAgent);

				/// db Log�� ���� ����:�����Լ�:������ ä�� �ΰ� ���� ������ ���� �Ѵ�.
				_api->DbQueryLog(thID, func, uuidW);
			};
			QueueFUNCN(run, "LogRequest");
		});

	try {
		//CStringW sessionId = KwGetFormattedGuid(true); // ON DUPLICATE KEY �� ���� ���� ������, ���� ���� �ʵ���

		auto jdoc = ShJVal(Json::Parse((PWS)requ));
		if(jdoc.get() == nullptr)
			throw new KException("JSON Parse Error.", GetLastError(), 0, L"JSON syntax is wrong.", NULL, __FUNCTION__, __LINE__);
		auto& jbj = *jdoc->AsObject().get();

		//jdoc = JSON::Parse((PWS)requ);//bin.GetPA());//constructor�� �Ҹ���.
		//if(jdoc == NULL)// ���� �����.
		//	throw new KException("JSON Parse Error.", GetLastError(), 0, L"JSON syntax is wrong.", NULL, __FUNCTION__, __LINE__); 
		//KAtEnd d_jdoc([&]() {	DeleteMeSafe(jdoc);	});
		//JObj jbj(jdoc->AsObject());
		size_t iq = url.find('?');//, 0Ui64);
		string url3 = url.substr(iq + 1);
		uparams = CStringW(url3.c_str());

		func = jbj.S("func");
		
		uuidW = jbj.S("uuid");//POST �� ���� JSON ������ �ȿ� �ִ�. GET�� ImageUpload�� URL�� �־ ss->_sinfo._urlparam["uuid"]�� ��´�.

		tick = GetTickCount64();
		CStringA fncA(func);
		JObj jpa = jbj.Oref("params");
		JObj jsr;

		_api->CheckDB();
	
		CFuncItem* funcItm = nullptr;
		if(_api->m_mapRFncs.Lookup(fncA, funcItm))//jproc);
		{
			int iOp = funcItm->_iOp;


			if(iOp & eFncPolicy)//��å�� �Լ� �θ��� ����
			{
				JObj jpolicy;
				_api->GetPolicy(jpa, jpolicy);
				jres("policy") = jpolicy;
			}

			// ���� ���� �Լ��� �θ���. Member function pointer�� ��ü _api�� ��� �޼ҵ�� �θ���.
			rv = (*funcItm->_func)(jpa, jsr, iOp);//ok

			if(rv == 0)
			{
				//AddPolicy(jsr, sFuncA);// jsr�� policy�� ���δ�. ������ �� ������ ������
				if(jsr.size() > 0)
					jres("response") = jsr;// {Return:OK/Exists/No Data, Desc}
				jres("return") = rv;//S_OK �� S_FALSE
			}
			else if (rv > 0)// jsr�ȿ� Return:why ��� �ְڰ�, ���Ҿ� error, return �� �ִ´�.
			{
				//https://developer.mozilla.org/ko/docs/Web/HTTP/Status
				// 400 Bad Request
				// 401 Unauthorized
				jres("error") = "API Error.";
				jres("return") = rv;//S_OK �� S_FALSE
			}// rv < 0 �̸� call�� �ʿ��� Unkown Error�� �ִ´�.

			///rpost << jres.ToJsonStringUtf8(); KAtEnd ���� �ϹǷ� ���⼭ ����
			return status;
		}
		else//�ܺ� DLL�Լ��� ������
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
					jres("return") = rv;//S_OK �� S_FALSE
				}
				else if(rv > 0)// jsr�ȿ� Return:why ��� �ְڰ�, ���Ҿ� error, return �� �ִ´�.
				{///���� ������ ����. throw_response �ϱ� ����
					jres("error") = "API Error.";
					jres("return") = rv;//S_OK �� S_FALSE
				}// rv < 0 �̸� call�� �ʿ��� Unkown Error�� �ִ´�.
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
	{//API�߿� ������ ��� ����� �´�.
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
		_api->LogException(e, uuidW);//������ �Ķ���� ì��� ���Ƽ� ����� ó�� �Ѵ�.
		
		status = sinfo._status;// e->_error;// 400; status
		e->Delete();//DeleteMeSafe(e);
	}
	catch(CException* e)
	{//��� KException �� �ٲ㼭 rethrow�ϹǷ� ����� ���� ȣ�� �ϴ� �ǿ� �ɰ��� �����ϼ��ִ�.
		CString sError;
		e->GetErrorMessage(sError.GetBuffer(1024), 1024); sError.ReleaseBuffer();
		DWORD err = GetLastError();
		if (sError.IsEmpty())
			sError = L"Unknown POST Error.";
		
		sinfo._status = eHttp_Expectation_Failed;
		status = sinfo._status;// e->_error;// 400; status
		JError(jres, sError, err, sinfo._status);// 417); eHttp_Internal_Server_Error = 500,
		_api->LogExcepFN(e);//CException���� FILE, LINE�� ���� ������ �Ķ���� ì��� ����.
		e->Delete();//DeleteMeSafe(e);
	}
	//rpost << jres.ToJsonStringUtf8();//error�� JSON���� �����Ѵ�.
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
	//CString uuid;//������
	CStringW func = L"ImageUpload";
	JObj jres;
	CStringW uuidW(uuid.c_str());
	try {
		_api->CheckDB();
		size_t iq = url.find('?');//, 0Ui64);
		string url3 = url.substr(iq + 1);
		CStringW uparams(url3.c_str());

		//JObj jsr; �̰Ŵ� JSON ��û���� ����.
		string UA_CPU;
		string UserAgent;
		sinfo._headers.Lookup("UA-CPU", UA_CPU);
		sinfo._headers.Lookup("User-Agent", UserAgent);

		DWORD thID = ::GetCurrentThreadId();//�̰Ű� ���� threadID
		ULONGLONG tick = GetTickCount64();
		auto ip = sinfo._ip;
		KAtEnd d_log([&]() {//���� ������ ���� & �� ������ �ȴ�.
			///string jresU8 = jres.ToJsonStringUtf8();//���⼭ �̹��� �۾��� ������ ���� �ϹǷ� utf8�� �ٲ۴�.
			// ���� ����  	MFCAppServerEx2.exe!JSONObject::~JSONObject() �� 321	C++
			//MFCAppServerEx2.exe!JSONValue::~JSONValue() �� 559	C++
			//	MFCAppServerEx2.exe!JObj::ToJsonStringW() �� 1270	C++
			// ���� �״´�.
			/// Tas�� ���� ���״´�. �׷� �Ʒ� QueueFUNCN �� ��� ����.
			/// ����: jres�� ������ KAtEnd���� ���� �Լ������� jres�۾����� �״´�.
			CStringW resp(rpost.str().c_str());//�񵿱� ������ ���� ������ ������Ƿ� ���ٷ� ������ ���� �����Ѵ�.
			string requUtf8 = sReqUtf8;// body.get()->m_p;
			string respUtf8 = rpost.str();//�񵿱� ������ ���� ������ ������Ƿ� ���ٷ� ������ ���� �����Ѵ�.
			//auto run = [&, thID, uuidW, func, uparams, requUtf8, respUtf8, tick, rv]() -> void
			auto run = [&, thID, uuidW, func, uparams, requUtf8, respUtf8, tick, rv, UA_CPU, UserAgent]() -> void
			{
				BACKGROUND(2);
				ULONGLONG elapsed = GetTickCount64() - tick;
				//?warning �̰� �񵿱�� �ϸ� 'Packets out of order'DB���� ��� ���� ����. CS�� �ƴ� Mutex�� �ϴ� ��Ż ����.
				_api->LogRequest(L"POST", CStringW(ip.c_str()), uuidW, uparams, func, requUtf8, respUtf8, elapsed, rv, UA_CPU, UserAgent);

				/// db Log�� ���� ����:�����Լ�:������ ä�� �ΰ� ���� ������ ���� �Ѵ�.
				_api->DbQueryLog(thID, func, uuidW);
			};
			QueueFUNCN(run, "LogRequest");
		});

		string ext = contType.substr(6);//jpeg, png, PNG
		JObj jsr;
		if(tchisame(ext.c_str(), "jpeg") || tchisame(ext.c_str(), "jpg") || tchisame(ext.c_str(), "png"))
		{//image/gif, image/png, image/jpeg, image/bmp, image/webp
			//string fname = ss->_urlparam["filename"];

			//1 �������忡�� �ٿ�ε�
			//2. DB�� Ű�� ��η� ����. ��Ʈ�� ����
			rv = _api->ImageUpload(data, len, fname, contType.c_str(), contLeng.c_str(), jsr);
			if (rv == 0)
			{
				if (jsr.size() > 0)
					jres("response") = jsr;// {Return:OK/Exists/No Data, Desc}
				jres("return") = rv;//S_OK �� S_FALSE
			}
		}
		else
		{
			rv = 2;//����� ��� �����޽����� ����ȴ�.
			CString ser; ser.Format(L"Not Supported image type.(%s)", CStringW(ext.c_str()));
			sinfo._status = eHttp_Unsupported_Media_Type;
			JError(jres, ser, rv, sinfo._status);//415
		}
		rpost << jres.ToJsonStringUtf8();
		return rv;
	}
	catch(KException* e)//rethrow �Ѱ��� ó�� �Ѵ�.
	{
		_api->LogException(e, uuidW);// L"ImageDownLoad");//������ �Ķ���� ì��� ���Ƽ� ����� ó�� �Ѵ�.
		sinfo._status = e->_status == 200 ? eHttp_Bad_Request : e->_status;
		JError(jres, e->m_strError, e->_error, sinfo._status);//417
		rv = e->m_nRetCode;// -100;
	}
	catch(CException* e)
	{//��� KException �� �ٲ㼭 rethrow�ϹǷ� ����� ���� ȣ�� �ϴ� �ǿ� �ɰ��� �����ϼ��ִ�.
		CString sError;
		e->GetErrorMessage(sError.GetBuffer(1024), 1024); sError.ReleaseBuffer();
		DWORD err = GetLastError();
		rv = err;
		if (sError.IsEmpty())
			sError = L"Unknown ImageUpload Error.";
		sinfo._status = eHttp_Expectation_Failed;
		JError(jres, sError, err, sinfo._status);
		_api->LogExcepFN(e);//CException���� FILE, LINE�� ���� ������ �Ķ���� ì��� ����.
	}
	rpost << jres.ToJsonStringUtf8();//error�� JSON���� �����Ѵ�.
	return rv;
}
