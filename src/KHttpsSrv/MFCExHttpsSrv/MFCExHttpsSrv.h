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

/// main DB�� ���� ������ ���� ������ �ϰ� �ִ�(StartServer��) 
/// ���� ������ "RunningServers" �����ϸ�, �ֱ� ������ 5�ʸ��� �Ͽ�
/// �ֱٿ� ����ġ ���ϰ� ũ������ ������ �ٽ� ���۵ɶ� �ڵ����� ���� �ȴ�.
class KAppDoc : public CDocument
{
public:
	DECLARE_DYNCREATE(KAppDoc)
	KAppDoc() noexcept;
	virtual ~KAppDoc();
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	SHP<KDatabase> _dbMain{make_shared<KDatabase>()};//�̷��� _dbLog �� share�Ҽ� ����

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
		jobj("LoadCount") = 0;// �̰ŷ� bFirst  �Ǻ�
		jobj("LastSpeed") = 0.;//�д� ó�� Ƚ��: �ֱ� 5�� �ȿ� ������ ��û�� ���
		jobj("Elapsed") = 0.;//��� ó�� �ӵ�: ��û ���ͼ� ������ �ð����� ��� (�ջ�/ī����)
		jobj("RunningServers") = JObj(); // RunningServers : { GUID1 : { }, GUID2:{} }
		jobj("StatDB") = L"none";
		/// �Ʒ��� regitry�� ��¥ �ִ� ��
		jobj("_DSN") = L"Winpache";
		jobj("_SERVER") = L"localhost";
		jobj("_UID") = L"root";
		jobj("_PWD") = L"";//�ִ°�� ���� DB ���
		jobj("_database") = L"winpache";

		/// KDatabase::RegGetODBCMySQL �� �о� ����
// +�Է�		[L"DSN"]	L"Winpache"
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
		dsnLog.Format(L"DSN=%s;UID=%s;PWD=%s;database=%s", jobj.S("_DSN"), jobj.S("_UID"), jobj.S("_PWD"), jobj.S("_database"));//PWD�� �ִ°� ODBC�� Ȯ�� ������, DSN������ �α��� �õ�
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
	/// site server start �Ȱ��� ���� ������ ���� �Ѵ�. 
	/// �������� ���� ��ġ�� ���� ���� �ڵ� ���� �ϵ���
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
			// start�Ҷ� �� ���� �ϰ�, stop ���� ó�� �Ǹ� ���� �Ѵ�.
			"c:\svr\fullpath.ext" : // ������ ����, ������ ���� ����
			{
				// file ���� ����
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
			"Winpache1" : // ���� ���� �ȵȰ�
			{
			}
		}
	}
	* */
};


class CMFCExHttpsSrvApp 
	: public CWinAppEx
	, public KCheckWnd
//	, public KAppDoc ON_COMMAND(...)���� error C2594: 'static_cast': ambiguous conversions from 'void (__cdecl CMFCExHttpsSrvApp::* )(void)' to 'AFX_PMSG'
// ���Ƿ�  CDocument�� App�� ��ġ�� �ȵǰڴ�.
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
