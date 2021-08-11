
// SuareSimul.cpp : Defines the class behaviors for the application.
//

#include "pch.h"

// #ifdef _Use_OpenSsl
// #pragma message( "cpp 1 #define _Use_OpenSsl")
// #endif

#include "SuareSimul.h"
#include "MainDlg.h"
#include <gdiplus.h>				// Base include
using namespace Gdiplus;			// The "umbella"
#pragma comment(lib, "gdiplus.lib")	// The GDI+ binary

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// #ifdef _Use_OpenSsl
// #pragma message( "cpp 2 #define _Use_OpenSsl")
// #endif

// CSuareSimulApp

BEGIN_MESSAGE_MAP(CSuareSimulApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSuareSimulApp construction

CSuareSimulApp::CSuareSimulApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// Place all significant initialization in InitInstance
}


// The one and only CSuareSimulApp object

CSuareSimulApp theApp;

// AfxGetApp()는 BACKGROUND에서 에러 난다.
CWinApp* GetMainApp()
{
	return &theApp;
}


// CSuareSimulApp initialization

BOOL CSuareSimulApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if(!AfxOleInit())
	{
		// CString s;s.LoadStringW(IDP_OLE_INIT_FAILED); see KwRsc(idc)
		::MessageBoxW(NULL, KwRsc(IDP_OLE_INIT_FAILED), L"Error", MB_ICONERROR|MB_OK);// IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));


	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, NULL);


	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Keepspeed"));
	free((void*)m_pszProfileName);
	m_pszProfileName = _tcsdup(KwRsc(IDS_AppName));// L"TokCube");// m_pszAppName);는 어디서 초기화 되는지 몰라서 긱접.



	CMainPool::Pool(2);//?zzz

	   // Seed the random-number generator with the current time so that  
   // the numbers will be different every time we run.  
	srand((unsigned)time(NULL));
	auto dm = rand();

	_docApp.StartApp();




	CMainDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}













/// //////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(KAppDoc, CDocument)

// BEGIN_MESSAGE_MAP(KAppDoc, CDocument)
// END_MESSAGE_MAP()

// KAppDoc construction/destruction
void KAppDoc::StartApp()
{

	LoadData();
	CubeRegisterUser(_json, true);
	//CubeCheckCubeScore(_json);

}

/// <summary>
/// AddGame은 로컬에도 서버에도 저장 하지 않는다. 게임이 끝났을때만 저장 된다.
/// 게임 도중 죽으면 남는게 없다.
/// </summary>
/// <param name="initGGUID">랭킹에서 가져온 GGUID인 경우</param>
/// <param name="bSave">this->_json이 아니고 임시 복사한 거면 저장 하면 안되에</param>
/// <returns>생성한 게임객체. 끝나면 _json에 "GamesRun"에 추가 한다.</returns>
SHP<JObj> KAppDoc::AddGame(PAS cube, int unit, CString mode, CString initGGUID, bool bSave)
{
	AUTOLOCK(_csAppDoc);
	auto& jobj = *_json;
	auto& arGame = *jobj.A("GamesRun", true);
	/// 게임 하나 등록
	auto sjGame = make_shared<JObj>();
	/// Game GUID
	CString GGUID = KwGetFormattedGuid(FALSE);// 이건 상황에 따라
	(*sjGame)("GGUID") = GGUID;/// 게임 아이디
	///(*sjGame)("startIndex") = (int)_sq._mixOnly.size();/// mixed 에서 액션으로 넘어간 부분
	///(*sjGame)("time") = 0;/// 100nano tik. 초로 바꿀려면 /10000000 %.3f 의 초단위
	(*sjGame)("stat") = L"started";/// -> finished -> pending -> saved
	(*sjGame)("cube") = cube;/// 초기화한 테이블을 ','와 '\n'으로 구분한 테이블 스트링.
	(*sjGame)("unit") = unit;/// 1,2,3
	(*sjGame)("mode") = mode;/// axis, free
	(*sjGame)("initGGUID") = initGGUID;/// axis, free
	
	//(*sjGame)("tmStart") = KwGetCurrentTimeFullString(eCtLocal); 기계날짜를 속일수 있으므루 무용
	//서버에서 할수 밖에
	if(bSave)
		arGame.Add(sjGame, false);

	/// <summary>
	/// "actions" 배열 안에, 모든 액션이 들어 있고 
	/// "startIndex" 부터 사용자 액션이며 시각도 들어 가고
	/// 그 이전은 믹스 된것이고, 시각은 0
	/// "time" 에 전체 푸는데 걸린 시간이며 마지박 액션의 "elapsed"와 같다.
	/// </summary>
	return sjGame;
}

ShJArr KAppDoc::GetAction(ShJObj sjGame)
{
	AUTOLOCK(_csAppDoc);
	ShJArr arAct = sjGame->A("actions", true); //action 하나
	return arAct;
}
ShJObj KAppDoc::AddAction(ShJArr arAct, int index, int direction, int line, int space, LONGLONG tik)
{
	AUTOLOCK(_csAppDoc);
	///[ 슬라이드 한번 밀때 마다 기록
	/// 배열 동작항목 생성
	auto sjact = make_shared<JObj>();
	(*sjact)("index") = index;//
	(*sjact)("direction") = direction;///[4] enum EDirection{eLF, eRT, eUP, eDN	};
	(*sjact)("line") = line; // eLF, eRT 때는 column. 세로방향일때는 row
	(*sjact)("space") = space;//몇칸 이동 했나: 모두 같은거 같지만 나중에, 3의 배수가 될수 있다.
	(*sjact)("elapsed") = tik;//
	arAct->Add(sjact, false);/// 바로 _json에 들어 간다.
	///] action 하나 추가
	return sjact;
}

#include "KwLib64/HTTPS/HttpClient.h"

CString KAppDoc::GetCubeUrl(CString func)
{
	///[ url + guid
#ifdef _DEBUG
	PWS ssl = L"";
	int port =	80;
	CString ip = L"localhost";
#else
	//	for 19471~19475	 fire 19471~19490
	//	19476~19480
	PWS ssl = L"s";
	int port = 19472;
	CString ip = L"www.parent.co.kr";
#endif // _DEBUG
		CString surl;
	surl.Format(L"http%s://%s:%d/api?func=%s&uuid=%s", ssl, ip, port, (PWS)func, _json->S("GUID"));
	///] url + guid
	return surl;
}



int KAppDoc::CubeGetRankingList(ShJArr& sjTbl)
{
	AUTOLOCK(_csAppDoc);
	int rv = 0;
	ShJObj sjo, sjor;
	hres hr = RemoteRequest(L"CubeGetRankingList", sjo, sjor);
	if(hr == S_OK)
	{
		if(sjor && sjor->I("return", -1) == 0)//request 성공 return
		{
			auto response = sjor->O("response");
			if(response->SameS("Return", L"OK"))//func 의 논리적 리턴
				sjTbl = response->Array("table");
		}
		else
			rv = -2;
	}
	else
		rv = -1;
	return rv;
}
int KAppDoc::CubeRegisterUser(ShJObj sjAll, bool bSaveData)
{
	AUTOLOCK(_csAppDoc);
	///	auto sjUser = make_shared<JObj>(sjAll);//복사한 후
	// 	if(sjUser->Has("GamesRun"))
	// 		sjUser->DeleteKey("GamesRun");
	auto uuid = sjAll->S("GUID");
	auto sjUser = make_shared<JObj>();//껍질만 만들고
	JObj::CloneObjCond(*sjAll, *sjUser, [](auto k, auto sjv) -> bool
		{
			return k != L"GamesRun";
		}, false);//참조만 복사하여
	/// pending game 담아 가자.
	ShJArr gms = sjAll->Array("GamesRun");
	/// GamesRun 배열의 각 항목 JObj에서 GGUID 만 챙긴다.
	if(gms && gms->size() > 0)
	{
		ShJArr pnGm = make_shared<JArr>();
		for(auto& sjv : *gms)
		{
			auto sgm = sjv->AsObject();
			if(sgm->SameS("stat", L"pending"))// 게임완료 직후 CubeSaveCubeScore 가 실패 하면 pending 표시 해둔다.
			{
				(*sgm)("GUID") = uuid; //array 위 user 객체에 있는데 왜
				/// 게임만 하나 보낼때와 형평성을 맞추기 위해 넣자.
				pnGm->Add(sjv);// sgm->S("GGUID"));//GGUID만 가져 가는게 아니다.
			}
		}
		(*sjUser)("PendingGames") = pnGm;
	}

	ShJObj sjor;
	hres hr = RemoteRequest(L"CubeRegisterUser", sjUser, sjor);
	int rv = 0;
	if(hr != S_OK && !sjor)
	{
		(*sjAll)("stat") = L"registerError";/// started -> finished -> (pending) -> saved
		rv = -1;
	}
	else
	{
/// 	+ m_strError	L"Duplicate entry 'dwkang' for key 'fNickname'\n"	
/// 	+m_strStateNativeOrigin	L"State:23000,Native:1062,Origin:[ma-3.1.12][10.5.10-MariaDB]\n"	
		if(sjor)
		{
			rv = sjor->I("return", -1);
			if(rv == 0)//
			{
				(*sjAll)("stat") = L"registered";// sjUser는 복사본
			
				auto response = sjor->O("response");
				CString rvs = response->S("Return");
				if(response->SameS("ReturnPendingGames", L"OK"))
				{
					ShJArr gms = sjAll->Array("GamesRun");
					if(gms && gms->size() > 0)
					{
						for(auto& sjv : *gms)
						{
							auto sgm = sjv->AsObject();
							(*sgm)("stat") = L"saved";// 저장 되었으니 
						}
					}
					auto n = (int)gms->size();
					for(int i = n-1;i>=0;i--)
					{
						auto sjv = gms->GetAt(i);
						auto sgm = sjv->AsObject();
						if(sgm->SameS("stat", L"saved"))
							gms->RemoveAt(i);
					}
				}
			}
			else
			{
				CString serr = sjor->S("error");
				if(serr.Find(L"23000") >= 0)
				{
					(*sjAll)("stat") = L"IdDuplicated";// sjUser는 복사본
					rv = -10;
				}
			}
			//throw new KException("JSON Parse Error.", GetLastError(), 0, L"JSON syntax is wrong.", NULL, __FUNCTION__, __LINE__);
		}
		else
			rv = -2;
	}
	if(bSaveData)
		SaveData();
	return rv;
}

/// <summary>
/// stat == pending 인 게임이 제대로 저장 되었는지 확인 하고 
/// pending -> saved 로 바꾸고 저장
/// 지금은 한번에 하나만 되네? 여러개 배열로 가져 가도록 해야지.
/// </summary>
/// <param name="sjGame"></param>
/// <returns></returns>
int KAppDoc::CubeSaveCubeScore(ShJObj sjGame)
{
	AUTOLOCK(_csAppDoc);
	(*sjGame)("stat") = L"finished";
#ifdef _DEBUG
	(*sjGame)("note") = L"한글 테스트 test 1234";
#endif // _DEBUG
///	(*sjGame)("GUID") = _json->S("GUID"); /// gamer ID : GUID (game ID GGUID와 다름)
	/// user ID는 미리 넣어 두지 않는다. 보낼때 키가 필요 하면 넣어서 보낸다.

	//sjGame->CopyAt("GUID", _json); 이건 JVal끼리만 복사 하네
	(*sjGame)("GUID") = _json->S("GUID"); //array 위 user 객체에 있는데 왜
	ShJObj sjor;
	hres hr = RemoteRequest(L"CubeSaveCubeScore", sjGame, sjor);
	int rv = 0;
	if(hr != S_OK && !sjor)
	{
		//sAstrR = "Server Error!";
		(*sjGame)("stat") = L"pending";/// started -> finished -> (pending) -> saved
		///  뜰때 pending 인거 찾아서 서버에 바로 다시 등록 하지 말고, 체크만 해본다. tgame, taction
		/// CubeCheckCubeScore(GGUID)
		rv = -1;
	}
	else
	{
		(*sjGame)("stat") = L"saved";

		if(sjor)
		{
			rv = sjor->I("return", -1);
			if(rv == 0)//
			{
				auto response = sjor->O("response");
				CString rvs = response->S("Return");
			}
			//throw new KException("JSON Parse Error.", GetLastError(), 0, L"JSON syntax is wrong.", NULL, __FUNCTION__, __LINE__);
		}
		else
			rv = -2;
	}
	SaveData();
	return rv;
}



int KAppDoc::CubeGetGameActions(CString gguid, ShJArr& sjTbl)
{
	AUTOLOCK(_csAppDoc);
	int rv = 0;
	ShJObj sjo, sjor;
	sjo = make_shared<JObj>();
	(*sjo)("GGUID") = gguid;
	hres hr = RemoteRequest(L"CubeGetGameActions", sjo, sjor);
	if(hr == S_OK && sjor)
	{
		if(sjor->I("return", -1) == 0)//request 성공 return
		{
			auto response = sjor->O("response");
			if(response && response->SameS("Return", L"OK"))//func 의 논리적 리턴
			{
				sjTbl = response->A("table");
			}
		}
		else
			rv = -2;
	}
	return rv;
}

hres KAppDoc::RemoteRequest(CString func, ShJObj sjo, ShJObj& sjor)
{
	AUTOLOCK(_csAppDoc);
	CHttpClient cl;
#ifdef _DEBUG
	cl.m_nMilSecTimeoutRcv = 60000;
	cl.m_nMilSecTimeoutConnect = 60000;
#endif // _DEBUG
	JObj jreq;
	jreq("func") = func;
	if(sjo)
		jreq("params") = sjo;
	else
		jreq("params") = JObj(); /// 빈 params : { }

	SHP<KBinary> bin = jreq.ToJsonBinaryUtf8();
#ifdef _ReqData
	{
		"func":"CubeRegisterUser",
		"params" : {
			"FName":"TokCube.cfg",
			"GUID" : "5217BE0579D0401FA5E550468AE16448",
			"stat" : "unregistered"
		}
	}
	/// response
	{
		"error":"Function is not found.(CubeRegisterUser)",
		"return" : 0,
		"status" : 404
	}
	{
		"error":"Column 'femail' cannot be null\n",
		"return" : 122,
		"status" : 417
	}
	{
		"response":{
			"Return":"OK"
		},
		"return" : 0
	}
#endif // _ReqData
	KBinary binr;

	CString surl = GetCubeUrl(func);// L"SaveCubeScore");
	/// L"http:://localhost:80/api?func=CubeRegisterUser&uuid=C9C488D18AF748C497999A9ECA0A6881"	

	///[ header
	KStdMap<string, string> hdrs;
	hdrs["Cache-Control"] = "no-cache";
	hdrs["Content-Type"] = "application/json";
	///] header
	hres hr = cl.RequestPostSSL(surl, &binr, bin.get(), &hdrs);
	CStringA sAstrR;//다시 멀티바이트로 해야 저장 하지.
	if(hr != S_OK && binr.Size() == 0)
	{
		sAstrR = "Server Error!";
		return -1;
	}
	else
	{
		sjor = Json::ParseUtf8(binr.GetPA());
	}
	return 0;
}


void KAppDoc::InitDoc()
{
	AUTOLOCK(_csAppDoc);
	auto& jobj = *_json;
	if(!jobj.Len("GUID"))
		jobj("GUID") = KwGetFormattedGuid(FALSE);//main cfg GUID

	//jobj("_GUID") = L"";
	CString fcfg; fcfg.Format(L"%s.cfg", KwRsc(IDS_AppName));
	jobj("FName") = fcfg;// L"TokCube.cfg";
//	jobj("countGame") = 0;// 단순히 게임시작 횟수
// 	jobj("email") = ; // 기록이 높으면 입력을 받아야 한다.
// 	jobj("SNS") = ; // 기록이 높으면 입력을 받아야 한다.
	jobj("stat") = L"unregistered";/// started -> finished -> (pending) -> saved

	/// 배열: 게임 기록
	jobj("GamesRun") = JArr(); // RunningServers : { GUID1 : { }, GUID2:{} }
	/// & 과 = * 두개를 다해 줘야 내부 객체의 alias가 된다.
	auto& arGame = *jobj.A("GamesRun", true);

	/// <summary>
	/// SHP<JObj> sGame = AddGame();
	/// </summary>

// 	/// 게임 하나 벌크로 등록
// 	auto jGame = JObj();
// 	/// Game GUID
// 	jGame("GGUID") = KwGetFormattedGuid(FALSE);// 이건 상황에 따라
// 	/// 게임기록(걸린 시간)
// 	jGame("time") = 0.;/// %.3f 의 초단위


	/// <summary>
	/// SHP<JObj> sjAct = AddAction(sjGame, direction, line, space);
	/// </summary>

	///[ 슬라이드 한번 밀때 마다 기록
// 	auto& arAct = *sjGame.AMake("actions"); //action 하나
// 	/// 배열 동작항목 생성
// 	auto jact = JObj();
// 	jact("direction") = eLF;///[4] enum EDirection{eLF, eRT, eUP, eDN	};
// 	jact("line") = 0; // eLF, eRT 때는 column. 세로방향일때는 row
// 	jact("space") = 3;//몇칸 이동 했나
// 	arAct.Add(jact);
	///] action 하나 추가
	// jGame["actions"] = arAct; 이미 넣어 참조 중이니 안해도 된다.
	/// Game 하나 추가
//	arGame.Add(jGame);

}


void KAppDoc::Serialize(CArchive& ar)
{
	AUTOLOCK(_csAppDoc);
	const CString& full0 = GetPathName();//load한 파일 이면
	CString full1 = GetFilePath();	//full += L"WinpacheMain.cfg";

	char* ecdkey = "8369628";
	if(ar.IsStoring())
	{
		//JObj js;
		auto& jobj = *_json;// ->AsObject();// .get();

		ASSERT(jobj.Len("GUID"));//		jobj("GUID") = KwGetFormattedGuid(FALSE);
		CStringA guidA(jobj.S("GUID"));
		OSslEncrypt ose((PAS)guidA.Left(24), (PAS)guidA.Right(8));//key, salt

		CFile* fr = ar.GetFile();
		CStringA sUtf8 = jobj.ToJsonStringUtf8();

		int descLen = sUtf8.GetLength() * 2;

		KBinary bin8(descLen);
		PUCHAR pu8 = (PUCHAR)bin8.GetPA();

		BOOL bec = ose.Encrypt((PUCHAR)(PAS)sUtf8, sUtf8.GetLength(), pu8, &descLen);
		pu8[descLen] = '\0';//binary인데 뒤에 '\0' 넣는거는 넌센스

		if(bec)
		{
			KBinary guidB(guidA);
			KBinary guidBE(guidA.GetLength());
			EncodeBinary(guidB, ecdkey, true, &guidBE);

			fr->Write(guidBE.m_p, guidA.GetLength());
			fr->Write(pu8, descLen);
#ifdef _DEBUG ///디버그 할때 text로 들여야 볼수 있게
			CString full2 = full1 + L".json";
			KwFileSafeRemove(full2);
			CStringA json; KwUTF8ToChar(sUtf8, json);
			CFile fj(full2, CFile::modeCreate | CFile::modeWrite);
			fj.Write((PAS)json, json.GetLength());
			fj.Close();
#endif // _DEBUG
		}
		else
			TRACE("Encoding error.");
	}
	else
	{
		int glen = 32;
		CFile* fr = ar.GetFile();
		int lenData = (int)fr->GetLength() - glen;

		KBinary guidBE(glen);
		fr->Read(guidBE.m_p, glen);// guidA.GetBufferSetLength(32), 32);
		KBinary guidB(glen);
		EncodeBinary(guidBE, ecdkey, false, &guidB);

		CStringA guidA(guidB.m_p);//header 32byte

		KBinary bUtf8(lenData);
		fr->Read(bUtf8.m_p, lenData);

		OSslEncrypt ose((PAS)guidA.Left(24), (PAS)guidA.Right(8));
		int descLen = lenData * 2;//그냥 넉넉히
		KBinary bin8(descLen);
		PUCHAR pu8 = (PUCHAR)bin8.GetPA();

		bool bError = false;
		BOOL bdc = ose.Decrypt((PUCHAR)bUtf8.m_p, lenData, pu8, &descLen);
		if(!bdc)
			bError = true;
		CString sWstr;
		KwUTF8ToWchar((PAS)pu8, sWstr);

		auto jdoc = ShJVal(Json::Parse((PWS)sWstr));
		if(!jdoc)//.get() == nullptr)
			bError = true;
		_json = jdoc->AsObject();// .get();
		
		if(bError)
			KwMessageBoxError(L"%s %s.", _json->S("FName"), KwRsc(IDS_FileFormatError));
	}
}


//C:\Program Files(x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30037
// \atlmfc\src\mfc\afximpl.h
//#include <afximpl.h>
void KAppDoc::SaveData()
{
	AUTOLOCK(_csAppDoc);
	try
	{
		const CString& full0 = GetPathName();//load한 파일 이면
		if(full0.GetLength() > 0)
		{
			VERIFY(DoFileSave()); // 그대로 덮어쓰기.
		}
		else // 새파일 처음
		{
			CString full = GetFilePath();	//full += L"WinpacheMain.cfg";
			VERIFY(DoSave(full, 0));
		}
	}
	catch(CFileException* e)
	{
		CString sErrorW;
		e->GetErrorMessage(sErrorW.GetBuffer(1024), 1024); sErrorW.ReleaseBuffer();
		CStringA err(sErrorW);
		TRACE("CFileException(%d, %ld, %s)\n", e->m_cause, e->m_lOsError, err);
	}
	catch(CException* e)
	{
		CString sErrorW;
		e->GetErrorMessage(sErrorW.GetBuffer(1024), 1024); sErrorW.ReleaseBuffer();
		CStringA err(sErrorW);
		auto rc = e->GetRuntimeClass();

		TRACE("CException<%s>(%s)\n", rc->m_lpszClassName, err);
	}
}
#ifdef _DEBUG_JsonData
// appdoc
{
	"FName":"TokCube.cfg",
		"GUID" : "6547C62090904D9396A35DD279DDEC1C",
		"GamesRun" : [
	{
		"GGUID":"04719BACA0474A2287789290C0203ED1",
			"actions" : [
		{
			"direction":0,
				"elapsed" : 0,
				"line" : 3,
				"space" : 3
		},
		{
		  "direction":1,
		  "elapsed" : 0,
		  "line" : 5,
		  "space" : 3
		},
		{
		  "direction":1,
		  "elapsed" : 178237677,
		  "line" : 3,
		  "space" : 3
		},
		{
		  "direction":0,
		  "elapsed" : 206643866,
		  "line" : 5,
		  "space" : 3
		}
			],
			"startIndex":2,
				"stat" : "pending",
				"time" : 206643861
	}
		],
		"StartCount":0
}

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
#endif // _DEBUG_JsonData

CString KAppDoc::GetFilePath()
{
	AUTOLOCK(_csAppDoc);
	TCHAR curDir[1002];
	GetCurrentDirectory(1000, curDir);//C:\Users\dwkang\AppData\Roaming\Outbin\KHttpsSrv\x64\Debug
	WCHAR appdata[MAX_PATH];//CSIDL_PERSONAL
	HRESULT result = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, appdata);
	auto& jobj = *_json;
	if(jobj.size() == 0)
		InitDoc();

	CString fl; fl.Format(L"%s\\%s\\", appdata, KwRsc(IDS_AppName));//TokCube
	int rv = KwCheckTargetDir(fl, TRUE, TRUE);
	if(rv == 0)
	{
		ASSERT(jobj.Len("FName"));
		CString fname = jobj.S("FName"); //	fname = L"WinpacheMain.cfg";
		CString full = fl + fname;
		return full;
	}
	return L"";
}



void KAppDoc::LoadData()
{
	AUTOLOCK(_csAppDoc);
	if(_json->size() == 0)
	{
		InitDoc();
	}

	CString full = GetFilePath();
	if(KwIfFileExist(full))
	{
		if(!OnOpenDocument(full))
		{
			TRACE("Error! File Winpache.json loading.");
			return;
		}
		SetPathName(full, FALSE);
	}
}


int CSuareSimulApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	GdiplusShutdown(_gdiplusToken);/// 이거 너무 먼저 하면 안되. OnDestroy 같은데서

	return CWinApp::ExitInstance();
}
