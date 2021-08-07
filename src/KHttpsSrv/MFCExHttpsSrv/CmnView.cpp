
// CmnView.cpp: CMFCServerView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "KwLib64/DlgTool.h"
#include "KwLib64/tchtool.h"
#include "KwLib64/TimeTool.h"
#include "KwLib64/ThreadPool.h"
#include "ApiBase.h"
#include "ApiSite1.h"

#include "KwLib64/Recordset.h"
#include "KwLib64/KDebug.h"


#ifndef SHARED_HANDLERS
#include "MFCExHttpsSrv.h"
#endif
//#include "MFCAppServerEx2Doc.h"
#include "CmnView.h"
#include "SrvView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CmnDoc* CmnView::GetDocument() const
{
	CFormInvokable* vu = dynamic_cast<CFormInvokable*>(_vu);
	CDocument* doc = vu->GetDoc();
	CmnDoc* cdoc = dynamic_cast<CmnDoc*>(doc);
	return cdoc;
}

void CmnView::DoDataExchange(CDataExchange* pDX)
{
	CmnDoc* doc = GetDocument();
	if(doc == NULL)
		return;
/*
	KDDX_Check(_bSSL);
	KDDX_Check(_bStaticCache);

	//KDDX_TextA(_Title);
	// 	KDDX_TextA(_cachedPath);
	//	KDDX_TextA(_cachedUrl);
	KDDX_Text(_CacheLife);

	KDDX_TextA(_rootLocal);
	KDDX_TextA(_defFile);
	KDDX_TextA(_uploadLocal);

	KDDX_TextA(_ApiURL);
	KDDX_Text(_port);
	KDDX_TextA(_ODBCDSN);
	KDDX_Text(_note);

	DDX_Control(pDX, IDC_CacheLife, c_CacheLife);
// 	DDX_Control(pDX, IDC_cachedPath, c_cachedPath);
// 	DDX_Control(pDX, IDC_cachedUrl, c_cachedUrl);
	DDX_Control(pDX, IDC_rootLocal, c_rootLocal);
	DDX_Control(pDX, IDC_defFile, c_defFile);
	DDX_Control(pDX, IDC_ApiURL, c_ApiURL);
	DDX_Control(pDX, IDC_port, c_port);
	DDX_Control(pDX, IDC_ODBCDSN, c_ODBCDSN);
//	DDX_Control(pDX, IDC_Title, c_Title);
	DDX_Control(pDX, IDC_note, c_note);

*/
	if(!pDX->m_bSaveAndValidate) //읽어 들이면 UpdateData(); 
		doc->InitApi();
}

// CMFCServerView 메시지 처리기
void CmnView::SampleServer()
{
	CmnDoc* doc = GetDocument();
	if(doc)
		doc->SampleServer();
}
int CmnView::CheckData()
{
	CmnDoc* doc = GetDocument();
	if(!doc)
		return -1;
	ShJObj sjd = doc->GetJData(); //동기화 때문에 복사해 온다.
	if(sjd->I("_port") < 4000 && sjd->I("_port") != 80 && sjd->I("_port") != 443)
	{
		KwMessageBox(L"port is invalid!\nTry again!");
		throw 11;
	}
	if(sjd->IsEmpty("_cachedPath"))
	{
		KwMessageBox(L"Cached path is invalid!\nFile cache won't run.");
		//throw 12;
	}
	if(sjd->IsEmpty("_cachedUrl"))
	{
		KwMessageBox(L"Cached URL path is invalid!\nFile cache won't run");
		//throw 13;
	}
	if(sjd->IsEmpty("_rootLocal"))
	{
		KwMessageBox(L"Root local path is invalid!\nFile request will be denied.");
		//throw 13;
	}
	if(sjd->IsEmpty("_defFile"))
	{
		KwMessageBox(L"Default file is invalid!\n\"index.html\" will be default value.");
		//doc->_jdata("_defFile") = "index.html";
	}
	if(sjd->IsEmpty("_uploadLocal"))
	{
		KwMessageBox(L"Upload local path is invalid!\nTry again!");
		throw 13;
	}

	if(sjd->I("_bSSL"))
	{
		if(sjd->IsEmpty("_certificate"))
		{
			KwMessageBox(L"Certificate is invalid!");
			throw 21;
		}
		if(sjd->IsEmpty("_privatekey"))
		{
			KwMessageBox(L"Private key is invalid!");
			throw 22;
		}
		if(sjd->IsEmpty("_dhparam"))
		{
			KwMessageBox(L"DHparam key is invalid!");
			throw 23;
		}
		if(sjd->IsEmpty("_prvpwd"))
		{
			///KwMessageBox(L"Password is invalid!");
			TRACE(L"Password is empty!\n");
			//throw 24;
		}
	}
	return 0;
}

int CmnView::InitServer()
{
	BACKGROUND(1);
	CmnDoc* doc = GetDocument();
	try
	{
		if(doc == NULL)
			throw 1;
		//UpdateData(); BACKGROUND에서 쓰면 죽어.
		int rv = CheckData();
		if(rv != 0)
			return -1;

		doc->InitServerValue();

		doc->InitApi();
	}
	catch(int ierr)
	{
		return ierr;
	}
	return 0;
}



//HttpCmn것 과 동일
void CmnView::MakeJsonResponse(HTTPResponse& res, JObj& jres)
{
	string jresU8 = jres.ToJsonStringUtf8();//여기서 이미지 작업은 끝나서 리턴 하므로 utf8로 바꾼다.
	HttpCmn::MakeJsonResponse(res, jresU8);
}
//void CMFCServerView::MakeJsonResponse(HTTPResponse& res, string_view js)
//{
//	res.MakeGetResponse(js, "application/json; charset=UTF-8");// "text/plain" default
//}


void CmnView::StartServer()
{
	FOREGROUND();
	_vu->UpdateData();//변경된 값으로 돌려야지. 저장은 안하더라도.
	UpdateControl("starting");

	auto run = [&]() -> void
	{
		BACKGROUND(1);
		CmnDoc* doc = GetDocument();
		ShJObj sjd = doc->GetJData(); //동기화 때문에 복사해 온다.
		CMyHttps* https = nullptr;
		CMyHttp* http = nullptr;
		
		doc->StartServer();
		int rv = 0;
		if(doc->GetSSL())
		{
			HTTPSCacheSession* pss = nullptr;
			https = doc->GetServer();
			rv = StartServerHttpT(https, pss);
		}
		else
		{
			HTTPCacheSession* pss = nullptr;
			http = doc->GetServerNoSSL();
			rv = StartServerHttpT(http, pss);
		}

		if(rv != 0)
		{
			//auto vu = _vu;
			KwBeginInvoke(_vu, ([&]()-> void {
				_vu->UpdateData();
				UpdateControl("error");
				}));
		}
	};
	CMainPool::QueueFunc(run);
}

void CmnView::StopServer()
{
	FOREGROUND();
	UpdateControl("stopping");
	CmnDoc* doc = GetDocument();
	if(!doc->IsStarted())
		return;
	CMainPool::QueueFunc([&, doc]() -> void
		{
			doc->StopServer();
			Sleep(1000);
			doc->FreeDllLibrary();
		});
}

void CmnView::RestartServer()
{
	FOREGROUND();
	UpdateControl("restarting");
	CmnDoc* doc = GetDocument();
	CMainPool::QueueFunc([&, doc]() -> void
		{
			doc->RestartServer();
		});
}

/// called from CChildFrame::, CMainFrame::OnClose()
void CmnView::Shutdown(PAS from)//?Shutdown 
{
	FOREGROUND();
	/// 이 뷰가 닫히는걸 app에 기록에서 제외 시킨다.
	auto ivc = dynamic_cast<KCheckWnd*>(GetMainApp());
	TRACE("1. called from: %s. ivc->ViewRemove(_id:%d); %s\n", from ? from:"", _id,  __FUNCTION__);//서버 시작 안했을때는 1.에서 끝난다.
	ivc->ViewRemove(_id);

	CmnDoc* doc = GetDocument();
	if(!doc->IsStarted())
		return;

	CMainPool::QueueFunc([&, doc]() ///backgroud 작업 큐
		{
			TRACE("3. doc->ShutdownServer() %s\n", __FUNCTION__);
			doc->ShutdownServer();
		});
	TRACE("2. ::WaitForSingleObject(%#X); %s\n", _hEventShutdown, __FUNCTION__);
	::WaitForSingleObject(_hEventShutdown, 60000);//?destroy 2

	TRACE("8. ResetEvent(_hEventShutdown:%#X); View Closed %s\n", _hEventShutdown, __FUNCTION__);
	ResetEvent(_hEventShutdown);//?destroy 6 이벤트 또 Wait할 수 있음.

/// called from CChildFrame::, CMainFrame::OnClose()
// 212) 1. ivc->ViewRemove(_id); CmnView::Shutdown
// 227) 2. ::WaitForSingleObject(0X63C); CmnView::Shutdown
// 223) 3. doc->ShutdownServer() CmnView::Shutdown::<lambda_a41>::operator ()
// Server shutdowning...
// Done!
// HTTPSCacheServer::onStopped 
// 405) 4. https->AddCallbackOnStopped({...}); CmnView::StartServerHttpT::<lambda_9aa>::operator ()
// 410) 5. 0 = ivc->ViewFind(vuid); CmnView::StartServerHttpT::<lambda_9aa>::operator ()
// 438) 7. SetEvent(hev:0X63C); CmnView::StartServerHttpT::<lambda_9aa>::operator ()
// 234) 8. ResetEvent(_hEventShutdown:0X63C); View Closed CmnView::Shutdown
// ~CMyHttp() destroyed.

	/// 그냥 Stop했을때
// Server stopping...
// Done!
// HTTPSCacheServer::onStopped 
// 405) 4. https->AddCallbackOnStopped({...}); CmnView::StartServerHttpT::<lambda_9aa14e97bf68dba8b8efcbb4dc5a5203>::operator ()
// 410) 5. 1 = ivc->ViewFind(vuid); CmnView::StartServerHttpT::<lambda_9aa14e97bf68dba8b8efcbb4dc5a5203>::operator ()
// 413) 6. _vu(0XB5275760)->GetSafeHwnd(); hev(0X62C) CmnView::StartServerHttpT::<lambda_9aa14e97bf68dba8b8efcbb4dc5a5203>::operator ()
// 418) 6.1 KwBeginInvoke(_vu(0XB5275760)); hev(0X62C) CmnView::StartServerHttpT::<lambda_9aa14e97bf68dba8b8efcbb4dc5a5203>::operator ()
// 438) 7. SetEvent(hev:0X62C); CmnView::StartServerHttpT::<lambda_9aa14e97bf68dba8b8efcbb4dc5a5203>::operator ()
// 432) 9. KwEnableWindow(_vu, IDC_); CmnView::StartServerHttpT::<lambda_9aa14e97bf68dba8b8efcbb4dc5a5203>::()::<lambda_387d88d58356c23fc2dc1c44501f2ebe>::operator ()

}



void CmnView::SelectFolder(CStringA& target)
{
	//| OFN_ALLOWMULTISELECT 
	CFolderPickerDialog folderPickerDialog(L"", OFN_FILEMUSTEXIST | OFN_ENABLESIZING, _vu, sizeof(OPENFILENAME));
	CString folderPath;
	if(folderPickerDialog.DoModal() == IDOK)
	{
		POSITION pos = folderPickerDialog.GetStartPosition();
		while(pos)
			folderPath = folderPickerDialog.GetNextPathName(pos);
		target = folderPath;
		if(target.Right(1) != L"\\")
			target += L"\\";
		_vu->UpdateData(0);
	}
}
CString CmnView::SelectFolder(PWS folderinit)
{
	CString target;
	CFolderPickerDialog folderPickerDialog(folderinit, OFN_FILEMUSTEXIST | OFN_ENABLESIZING, _vu, sizeof(OPENFILENAME));
	CString folderPath;
	if(folderPickerDialog.DoModal() == IDOK)
	{
		POSITION pos = folderPickerDialog.GetStartPosition();
		while(pos)
			folderPath = folderPickerDialog.GetNextPathName(pos);
		target = folderPath;
		if(target.Right(1) != L"\\")
			target += L"\\";
		//_vu->UpdateData(0);
	}
	return target;
}

// xxxxxxxxxxxxxx

void CmnView::EnableCommand(int idc, int bEnable, int iOp)
{
	_mapUpt[idc] = bEnable;
	if(iOp != 1)// 1:Don't do to control
		EnableCtrl(idc, bEnable);
}
void CmnView::EnableCtrl(int idc, int bEnable)
{
	KwEnableWindow(_vu, idc, bEnable);
}

void CmnView::UpdateControl(CStringA stat, int iOp)
{

}

//void CallbackOnGET(TSession* ss, SHP<KBinData> shbin, HTTPResponse& res);

void CmnView::CallbackOnStarted(int vuid)
{
	auto ivc = dynamic_cast<KCheckWnd*>(GetMainApp());
	bool bVu = !ivc ? false : ivc->ViewFind(vuid);
	//TRACE("AddCallbackOnStarted\n");
	if(bVu)//_vu && ::IsWindow(_vu->GetSafeHwnd()))
	{
		BACKGROUND(1);
		// button 누를때 하지만, 그때와는 상황이 다르다.
		UpdateControl("started");//자체에서 BeginInvoke한다.
// 		KwBeginInvoke(_vu, ([&]()-> void
// 			{ //?beginInvoke 4
// 				UpdateControl("started");
// 			}));
		StoptAndStart("3.started");
	}
}

int CmnView::CallbackOnStopped(HANDLE hev, int vuid)
{
	TRACE("4. https->AddCallbackOnStopped({...}); %s\n", __FUNCTION__);

	auto ivc = dynamic_cast<KCheckWnd*>(GetMainApp());
	bool bVu = !ivc ? false : ivc->ViewFind(vuid);
	TRACE("5. %d = ivc->ViewFind(vuid); %s\n", bVu ? 1 : 0, __FUNCTION__);
	if(bVu)
	{
		TRACE("6. _vu(%#X)->GetSafeHwnd(); hev(%#X) %s\n", _vu, hev, __FUNCTION__);
		KwBeginInvoke(_vu, ([&, ivc, vuid]()-> void
			{ //?beginInvoke 4
				bool bVu1 = !ivc ? false : ivc->ViewFind(vuid);
				if(bVu1)
				{
					TRACE("9. KwEnableWindow(_vu, IDC_); %s\n", __FUNCTION__);
					UpdateControl("stopped");

					StoptAndStart("2.stopped");
				}
			}));
	}


	if(hev)//이 HANDLE값은 전달 받아야 한다.
	{
		TRACE("7. SetEvent(hev:%#X); %s\n", hev, __FUNCTION__);
		::SetEvent(hev);//?Shutdown //?destroy 5 이걸 안주면 마냥 기다린다.
	}
	return 0;
}

int CmnView::CallbackOnSent(KSessionInfo& inf, int vuid, size_t sent, size_t pending)
{
	//	//TRACE(L"AddCallbackOnSent(sent:%ld, pending:%ld) %s\n", sent, pending, now);
	auto ivc = dynamic_cast<KCheckWnd*>(GetMainApp());
	bool bVu = !ivc ? false : ivc->ViewFind(vuid);
	if(bVu)
	{
		if(pending == 0)/// 아, ss가 clear 되버렸네.
		{
			SHP<KArray<string>> shar = make_shared<KArray<string>>();
			CStringA now(KwGetCurrentTimeFullString());
			shar->Add("E");
			shar->Add((PAS)now);
			shar->Add(inf._method);
			shar->Add(inf._url);
			shar->Add(inf._stCached);
			shar->Add(inf._headers.Get("Content-Type"));
			shar->Add(inf._headers.Get("Content-Length"));
			KwBeginInvoke(_vu, ([&, shar]()-> void
				{
					MonitorRequest(shar);
				}));
		}
	}
	return 0;//(sent:107, pending:0) 2020-10-11 07:35:56
}



int CmnView::CallbackOnGET(KSessionInfo& inf, int vuid, HTTPResponse& res)
{
	BACKGROUND(1);//여기는 백그라운드 이다.
	//TRACE("AddCallbackOnGET\n");
	CmnDoc* doc = GetDocument();
	auto sjd = doc->GetJData();
	//if (inf._sparams.size() == 0) 
	auto chkApi = inf._dir.find(sjd->SA("_ApiURL"));
	if(chkApi == string::npos)// '/api' 를 포함 하고 있으면 파일이 아니다.
	{// '?'가 없으면 파일로 인정
		KBinary fbuf;
		string dirFile = inf._dir;
		if(dirFile == "/")
			dirFile += sjd->SA("_defFile");
		int rv = doc->_svr->ResponseFileForGet(dirFile, fbuf);
		if(rv == 0)
		{
			CStringA dirFileA(dirFile.c_str());
			int ir = dirFileA.ReverseFind('.');
			if(ir >= 0)
			{
				CStringA ext = dirFileA.Mid(ir);
				CStringA cntp = KwContentType(ext);
				//CStringA cntp; cntp.Format("image/%s", ext);
				//string data(fbuf.m_p, (size_t)fbuf.m_len);
				res.MakeGetResponse(fbuf.m_p, (size_t)fbuf.m_len, (PAS)cntp);// "image/jpeg");// "text/plain" default
				return rv;
			}
		}
		else if(rv > 0)
		{
			HttpCmn::MakeStrErrorToJsonResponse(res, "File not found.");
			return rv;
		}
		else
			return rv;
		// 			}	else {
		// 				HttpCmn::MakeStrErrorToJsonResponse(res, "Parameter is not found.");
		// 				return 1;//여기 안온다. 파라미터가 없으면
		// 			}
	}
	if(inf._dir.size() != sjd->Length("_cachedUrl"))
	{// '?'위치가 _cachedUrl 보다 더 있으면
		HttpCmn::MakeStrErrorToJsonResponse(res, "Bad URL.");
		return 2;//여기 안온다. 파일이면
	}
	if(!tchbegin(inf._dir.c_str(), sjd->SA("_cachedUrl")))
	{
		HttpCmn::MakeStrErrorToJsonResponse(res, "Bad directory.");
		// /api 아니면 리턴. 예: url	"/favicon.ico" 처럼 파일인 경우 그냥 return 
		return 3;
	}

	string uuid = inf._urlparam["uuid"];

	int rv = 0;
	stringstream jstrm;//JS.ToJsonStringUtf8() 해서 온 값이다.
	rv = doc->_svr->ResponseForGet(inf, jstrm);
	if(jstrm.str().size() > 0) //결과가 있을때만 Response해야한다.
		HttpCmn::MakeJsonResponse(res, jstrm.str());
	else
	{
		return -100;// -100:Unknown Error 음수를 리턴하면 에러를 받은 쪽에서 넣는다.
	}
	// UI작업은 OnSentKw 에서 한다. KwBeginInvoke
	return rv;

}
	
int CmnView::CallbackOnPOST(KSessionInfo& inf, int vuid, SHP<KBinData> shbin, HTTPResponse& res)
{
	BACKGROUND(1);
	CmnDoc* doc = GetDocument();
	auto sjd = doc->GetJData();

	JObj jres;

	auto cth = inf._headers["Content-Type"];
	auto ctu = inf._urlparam["Content-Type"];
	string ext;
	int ilen = 0;

	string imgContType;
	string contLeng;

	if(tchstrx(cth.c_str(), "image/") == 0)
	{//찾은 인덱스가 0이면 맨앞에서 발견
		imgContType = cth;
		contLeng = inf._headers["Content-Length"];
	}
	else if(tchstrx(ctu.c_str(), "image/") == 0)
	{
		imgContType = ctu;// Content-Type=image%2fjpeg  '/'는 %2f
							//ext = inf._urlparam["ext"]; //old protocol
		contLeng = inf._urlparam["Content-Length"];
	}
	string filename = inf._urlparam["filename"];//이건 사실 올린 로컬파일이라 필요 없다.

	stringstream jstrm;//JS.ToJsonStringUtf8() 해서 온 값이다.
						//string body(req.body());

	int rv = 0;
	int status = eHttp_OK;
	// image라면 10byte는 되야지.
	if(imgContType.size() > 0)
	{
		auto body = shbin->m_p;// req.body();
		auto len = shbin->m_len;//body.size();
		ilen = KwAtoi(contLeng.c_str());
		string uuid = inf._urlparam["uuid"]; //Image Upload때만 URL에 넣어줘야 한다.

		if(len != ilen)
		{
			HttpCmn::MakeStrErrorToJsonResponse(res, "Image length does not match.", eHttp_Not_Found);
			return 2;//여기 안온다. 파일이면
		}
		else /// 이미지 업로드 
		{
			rv = doc->_svr->ResponseImageUpload(inf, uuid, body, len, filename, imgContType, contLeng, jstrm);
			if(rv != 0)
				status = inf._status;
		}
		//rv = app->_svr->ResponseImageUpload(inf._url, uuid, body.data(), body.size(), filename, imgContType, contLeng, jstrm);
	}
	else if(tchbegin(inf._url.c_str(), sjd->SA("_ApiURL")))
	{
		auto chkApi = inf._dir.find(sjd->SA("_ApiURL"));
		if(chkApi == string::npos)// '/api' 를 포함 하고 있으면 파일이 아니다.
		{
			//	rv = doc->_svr->ResponseForPost(inf, shbin, jstrm);
			HttpCmn::MakeStrErrorToJsonResponse(res, "POST request must be API call.", eHttp_Bad_Request);
			return 4;//여기 안온다. 파일이면
		}
		else
		{
			/// ///////////////////////////////////////////////////////////////
			/// 이건 원격함수 호출 
			status = doc->_svr->ResponseForPost(inf, shbin, jstrm);
			/// ///////////////////////////////////////////////////////////////
		}
	}
	else
	{
		HttpCmn::MakeStrErrorToJsonResponse(res, "Not supperted request.");
		return 3;//여기 안온다. 파일이면
	}
	///-1: error메시지 이미 생성. -2: No 오류 메시지 , 0:OK

	int eCache = 0;
	if(status == eHttp_OK)
	{
		if(jstrm.str().size() > 0) //리턴값, 오류이면 아래
			HttpCmn::MakeJsonResponse(res, jstrm.str(), status);
	}
	else
	{
		if(jstrm.str().size() > 0)// eHttp_OK가 아닌데, jstrm이 있다면 JSON 오류가 이미 만들어짐.
		{
			HttpCmn::MakeJsonErrorResponse(res, jstrm.str(), status);
			eCache = -1;
		}
		else
			eCache = -2;//-2 오류 메시지 없음.
	}
	//eCache = status == eHttp_OK ? 0 : jstrm.str().size() > 0 ? -1 : -2;//-2 오류 메시지 없음.
	//	return -100;// 에러 캐시저장 안한다. //-100:Unknown Error 음수를 리턴하면 에러를 받은 쪽에서 넣는다.
	return eCache;// rv;//이게 0이면 캐시에 저장 한다. //양수를 줘야 에러메시지가 응답된다.
}

int CmnView::CallbackOnErrorAsio(int e, string c, string m)
{
	// Stop 하고 다시 Start해야.
	StoptAndStart("1.stop");
	return 0;
}




void CmnView::StoptAndStart(string step)//?stop&start 
{
	CmnDoc* doc = GetDocument();

	if(step == "1.stop")
	{
		if(_stepStopStart.empty())
		{
			if(!_hEventAsioError)
				_hEventAsioError = ::CreateEvent(NULL, TRUE, FALSE, L"StartAndStop");//?destroy 1 여기서 해줘야 람다에 전달 가능
			else
				::ResetEvent(_hEventAsioError);

			if(doc->IsStarted())
			{
				StopServer();///해보니, AsioError나서, OnStopped로 오지 않는다.
				::WaitForSingleObject(_hEventAsioError, 5000);//?StoptAndStart stop먼저 해본다. 
				ResetEvent(_hEventAsioError);//?destroy 6 이벤트 또 Wait할 수 있음.
				///원래는 OnStooped에서 불리지만 안오면 기다려봤다가 여기서 그냥 한다.
			}
			
			_stepStopStart = step;
			KwBeginInvoke(_vu, ([&]()-> void
 			{
				StoptAndStart("2.stopped");
			}));
		}
	}
	else if(step == "2.stopped") 
	{
		if(_stepStopStart == "1.stop")
		{
			StartServer();///여기서 asio service도 새로 하므로 완전히 새로 시작 한다.
			_stepStopStart = "2.start";
			_stepStopStart = step;
		}
	}
	else if(step == "3.started")
	{
		if(_stepStopStart == "2.start")
		{
			_stepStopStart.clear();
		}
	}
}


