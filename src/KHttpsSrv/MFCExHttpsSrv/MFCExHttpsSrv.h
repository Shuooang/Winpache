// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface
// (the "Fluent UI") and is provided only as referential material to supplement the
// Microsoft Foundation Classes Reference and related electronic documentation
// included with the MFC C++ library software.
// License terms to copy, use or distribute the Fluent UI are available separately.
// To learn more about our Fluent UI licensing program, please visit
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MFCExHttpsSrv.h : main header file for the MFCExHttpsSrv application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "KwLib64/DlgTool.h"//KCheckWnd
// CMFCExHttpsSrvApp:
// See MFCExHttpsSrv.cpp for the implementation of this class
//

/// main DB에 관한 정보와 현재 리스닝 하고 있는(StartServer한) 
/// 서버 정보를 "RunningServers" 보관하며, 최근 정보를 5초마다 하여
/// 최근에 예상치 못하게 크러시한 서버를 다시 시작될때 자동으로 시작 된다.
class KAppDoc : public CDocument
{
public:
	DECLARE_DYNCREATE(KAppDoc)
	KAppDoc() noexcept;
	virtual ~KAppDoc();
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	SHP<KDatabase> _dbMain{make_shared<KDatabase>()};//이래야 _dbLog 와 share할수 있음

	void LoadData();
	CString GetFilePath();
	void SaveData();

// 	CString _GUID;
// 	PWS _fname{ L"Winpache.json" };
// 	CString _DSN{ L"Winpache" };
// 	CString _UID{ L"root" };
// 	CString _PWD;
// 	CString _database{ L"winpache" };
// 	CStringA _statDB{ "none" };// "MariaDB", "ODBC", "conected"

	ShJObj _json{make_shared<JObj>()};

	void InitDoc()
	{
		auto& jobj = *_json;
		if(!jobj.Len("_GUID"))
			jobj("_GUID") = KwGetFormattedGuid(FALSE);//main cfg GUID

		//jobj("_GUID") = L"";
		jobj("FName") = L"WinpacheMain.cfg";
		jobj("LoadCount") = 0;// 이거로 bFirst  판별
		jobj("LastSpeed") = 0.;//분당 처리 횟수: 최근 5초 안에 응답한 요청만 계산
		jobj("Elapsed") = 0.;//평균 처리 속도: 요청 들어와서 응답한 시간까지 계속 (합산/카운터)
		jobj("RunningServers") = JObj(); // RunningServers : { GUID1 : { }, GUID2:{} }
		jobj("StatDB") = L"none";
		/// 아래는 regitry에 진짜 있는 값
		jobj("_DSN") = L"Winpache";
		jobj("_SERVER") = L"localhost";
		jobj("_UID") = L"root";
		jobj("_PWD") = L"";//있는경우 메인 DB 비번
		jobj("_database") = L"winpache";

		/// KDatabase::RegGetODBCMySQL 로 읽어 오면
// +입력		[L"DSN"]	L"Winpache"
// +		[L"Driver"]	L"MariaDB ODBC 3.1 Driver"
// +		[L"PORT"]	L"3306"
// +		[L"PWD"]	L"******"bnmnnm,
// +		[L"SERVER"]	L"localhost"
// +		[L"TCPIP"]	L"1"
// +		[L"UID"]	L"root"
	}
	CString MakeDsnString()
	{
		auto& jobj = *_json;
		CString dsnLog;
		dsnLog.Format(L"DSN=%s;UID=%s;PWD=%s;database=%s", jobj.S("_DSN"), jobj.S("_UID"), jobj.S("_PWD"), jobj.S("_database"));//PWD가 있는거 ODBC에 확인 했으니, DSN만으로 로그인 시도
		return dsnLog;
	}
#ifdef _DEBUG
	Vake FName;
	Vake LoadCount;
	Vake LastSpeed;
	Vake Elapsed;
	Vake RunningServers;
	Vake StatDB;
	Vake _DSN;
	Vake _SERVER;
	Vake _UID;
	Vake _PWD;
	Vake _database;
#endif // _DEBUG

	CKCriticalSection _csRecover;
	/// site server start 된것을 메인 설정에 저장 한다. 
	/// 예상하지 않은 정치때 다음 런에 자동 시작 하도록
	void RegisterServerStart(ShJObj sjsvr);

	void UnregisterServerStart(CStringA GUID);

	void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE) override;

	KList<wstring> _pendingNewServer;

	void PushRecoveringServer(wstring guid);
	bool PopRecoveringServer(CString& guid);

	CKCriticalSection _csReqs;
	//KList<ULONGLONG> _lstReqs;
	class KReqRes
	{
	public:
		KReqRes(ULONGLONG tReq, string ssid)
			: _tReq(tReq), _ssid(ssid) {}

		ULONGLONG _tReq{0};
		ULONGLONG _tRes{0};
		string _ssid;
	};

	KList<SHP<KReqRes>> _lstReqs2;

	void ReqOccured(string ssid);

	/// ssid vs nano tick
	KStdMap<string, SHP<KReqRes>> _mapReq;

	double OnResponse(string ssid);
	ULONGLONG _lastCheckSpeed{0};
	ULONGLONG _lastCheckElapsed{0};
	double GetSpeedPerSec();

	/*
	{
		_fname:"Winpache.json"
		;
		;
		unstopservers:
		{
			// start할때 다 저장 하고, stop 정상 처리 되면 제거 한다.
			"c:\svr\fullpath.ext" : // 파일을 열고, 내용은 여기 꺼로
			{
				// file 내용 전부
				  "_ApiURL":"\/api",
				  "_CacheLife":3000,
				  "_GUID":"C00E9690-D32B-47C7-9000-127DB928C495",
				  "_ODBCDSN":"FatUs",
				  "_SQL":"",
				  "_SrcImagePath":"C:\/svr\/upload",
				  "_bSSL":0,
				  "_bStaticCache":1,
				  "_certificate":"C:\\Dropbox\\Proj\\KHttpData\\certcertificate.pem",
				  "_defFile":"index.html",
				  "_dhparam":"C:\\Dropbox\\Proj\\KHttpData\\certdh4096.pem",
				  "_note":"",
				  "_port":80,
				  "_privatekey":"C:\\Dropbox\\Proj\\KHttpData\\certprivate.pem",
				  "_prvpwd":"",
				  "_rootLocal":"C:\\Dropbox\\Proj\\ODISO\\Src\\IIS",
				  "_rootURL":"",
				  "_uploadLocal":"C:\\srv\\upload"
			}
			"Winpache1" : // 아직 저장 안된거
			{
			}
		}
	}
	* */
};


class CMFCExHttpsSrvApp 
	: public CWinAppEx
	, public KCheckWnd
//	, public KAppDoc ON_COMMAND(...)에서 error C2594: 'static_cast': ambiguous conversions from 'void (__cdecl CMFCExHttpsSrvApp::* )(void)' to 'AFX_PMSG'
// 나므로  CDocument를 App과 합치면 안되겠다.
{
public:
	CMFCExHttpsSrvApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void NewFile()
	{
		OnFileNew();
	}
	
	
	// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	KAppDoc _docApp;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCExHttpsSrvApp theApp;
