
// SuareSimul.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

// #ifdef _Use_OpenSsl
// #pragma message( "h 1 #define _Use_OpenSsl")
// #endif
#include "KwLib64/ktypedef.h"

// #ifdef _Use_OpenSsl
// #pragma message( "h 2 #define _Use_OpenSsl")
// #endif

#include "KwLib64/OpenSSL.h"

#include "KwLib64/stdecl.h"
#include "KwLib64/KJson.h"

#include "resource.h"		// main symbols






class KAppDoc : public CDocument
{
public:
	DECLARE_DYNCREATE(KAppDoc)
	KAppDoc() noexcept
	{
	}
	virtual ~KAppDoc()
	{
	}
	virtual void Serialize(CArchive& ar);

	//SHP<KDatabase> _dbMain{make_shared<KDatabase>()};//�̷��� _dbLog �� share�Ҽ� ����
	void StartApp();

	void LoadData();
	CString GetFilePath();
	void SaveData();

	CKCriticalSection _csAppDoc;
	ShJObj _json{make_shared<JObj>()};

	/// <summary>
	/// ���� �ϳ��� �����Ͽ� �迭�� �߰� �ϰ� ���� SHP�� �����Ѵ�.
	/// </summary>
	/// <returns></returns>
	ShJObj AddGame(PAS cube, int unit, CString mode, CString initGGUID, bool bSave = true);

	ShJArr GetAction(SHP<JObj> sjGame);
	ShJObj AddAction(ShJArr arAct, int index, int direction, int line, int space, LONGLONG tik);

	hres RemoteRequest(CString func, ShJObj sjo, ShJObj& sjor);
	CString GetCubeUrl(CString func);
	void InitDoc();

	///[ remote call
	int CubeSaveCubeScore(ShJObj sjGame);
	int CubeGetGameActions(CString gguid, ShJArr& sjTbl);
	//int CubeCheckCubeScore(ShJObj sjGame);
	int CubeRegisterUser(ShJObj sjUser, bool bSaveData = true);
	int CubeGetRankingList(ShJArr& sjTbl);
	///]


	CKCriticalSection _csRecover;
	/// site server start �Ȱ��� ���� ������ ���� �Ѵ�. 
	/// �������� ���� ��ġ�� ���� ���� �ڵ� ���� �ϵ���

	void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE) override
	{
		__super::SetPathName(lpszPathName, FALSE);//������ FALSE
	}



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


	/// ssid vs nano tick
	KStdMap<string, SHP<KReqRes>> _mapReq;

	ULONGLONG _lastCheckSpeed{0};
	ULONGLONG _lastCheckElapsed{0};

};








/// <summary>
/// AfxGetApp()�� background���� �θ��� �״´�. 
/// </summary>
/// <returns>theApp�� ����</returns>
CWinApp* GetMainApp();

// CSuareSimulApp:
// See SuareSimul.cpp for the implementation of this class
//

class CSuareSimulApp : public CWinApp
{
public:
	CSuareSimulApp();
	KAppDoc _docApp;

	ULONG_PTR _gdiplusToken{0};

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CSuareSimulApp theApp;
