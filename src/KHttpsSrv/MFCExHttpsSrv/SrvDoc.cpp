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

// SrvDoc.cpp : implementation of the SrvDoc class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MFCExHttpsSrv.h"
#endif

#include "ApiSite1.h"
#include "SrvDoc.h"

//#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// SrvDoc

IMPLEMENT_DYNCREATE(SrvDoc, CDocument)

BEGIN_MESSAGE_MAP(SrvDoc, CDocument)
END_MESSAGE_MAP()


// SrvDoc construction/destruction

SrvDoc::SrvDoc() noexcept
{
	// TODO: add one-time construction code here

}

SrvDoc::~SrvDoc()
{
}

BOOL SrvDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	auto app = (CMFCExHttpsSrvApp*)AfxGetApp();
	auto& appd = (app)->_docApp;
	//_fullPath = GetPathName();
	if(_GUID.IsEmpty())
	{
		CString guid;
		if(appd.PopRecoveringServer(guid))//?server recover 4
			_GUID = guid;
		else
			_GUID = KwGetFormattedGuid();
	}

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}

BOOL SrvDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	BOOL b = __super::OnOpenDocument(lpszPathName);
	_fullPath = lpszPathName;
// 	auto app = (CMFCExHttpsSrvApp*)AfxGetApp();
// 	auto& appd = (app)->_docApp;
// 	auto& jobj = *appd._json;
// 	auto srsv = jobj.O("RunningServers");
// 	auto sjo = srsv->O(_GUID);
// 	if(sjo)
	{
		//원래 파일데이터 보다 RunningServers가 new일수 있다. 저장 안했으면
// 		if(sjo->I("_bRecover"))
// 		JsonToData(sjo, false);
// 		if(_bRecover)
		{
// 			GetActive
// 			if(_bDbConnected)
// 			{
// 				ASSERT(appd._dbMain->IsOpen());
// 				CString ODBCDSN(_ODBCDSN);
// 				auto rs = _svr->_api->CheckDB(ODBCDSN, appd._dbMain);
// 			}
// 			StartServer();

		}

	}
	return b;
}




// SrvDoc serialization

void SrvDoc::Serialize(CArchive& ar)
{
	auto cdoc = dynamic_cast<CmnDoc*>(this);
	cdoc->Serialize(ar);//?zzz

	if (ar.IsStoring())	{	}
	else	{	}
}

#ifdef SHARED_HANDLERS

// Support for thumbnails
void SrvDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Modify this code to draw the document's data
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Support for Search Handlers
void SrvDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Set search contents from document's data.
	// The content parts should be separated by ";"

	// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void SrvDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// SrvDoc diagnostics

#ifdef _DEBUG
void SrvDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void SrvDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif

void SrvDoc::JsonToData(ShJObj& sjobj, bool bToJson)
{

	if(bToJson)//ar.IsStoring())
	{
		if(!sjobj)
			sjobj = std::make_shared<JObj>();///이거 때문에 파라미터를 ShJObj&
		JObj& js = *sjobj;
		//js("_port") = _port;
		ASSERT(!_GUID.IsEmpty());
		//	_GUID = KwGetFormattedGuid();
		KJSPUT(_GUID);
		KJSPUT(_port);
		KJSPUT(_bSSL);
		KJSPUT(_bStaticCache);
		KJSPUT(_CacheLife);
		//		KJSPUT(_Title);
		// 		KJSPUT(_cachedPath);
		// 		KJSPUT(_cachedUrl);
		KJSPUT(_certificate);
		KJSPUT(_privatekey);
		KJSPUT(_prvpwd);
		KJSPUT(_dhparam);
		KJSPUT(_ODBCDSN);
		KJSPUT(_rootLocal);
		KJSPUT(_defFile);
		KJSPUT(_uploadLocal);
		KJSPUT(_rootURL);
		KJSPUT(_ApiURL);
		// 		KJSPUT(_UdpSvr);
		// 		KJSPUT(_portUDP);
		KJSPUT(_SQL);
		KJSPUT(_SrcImagePath);
		KJSPUT(_note);
		//	JSPUT(_SrcTable);
		//	JSPUT(_SrcKeyField);

		KJSPUT(_bDbConnected);
		KJSPUT(_bRecover);
		KJSPUT(_tLastRunning);
		KJSPUT(_fullPath);
	}
	else
	{
		//auto& js = *jdoc->AsObject().get();
		JObj& js = *sjobj;

		KJSGETS(_GUID);
		//ASSERT(!_GUID.IsEmpty());
		if(_GUID.IsEmpty())
			_GUID = KwGetFormattedGuid();
		KJSGETI(_port);// 숫자인 경우
		KJSGETI(_bSSL);// 숫자인 경우
		KJSGETI(_bStaticCache);// 숫자인 경우
		KJSGETI(_CacheLife);// 숫자인 경우
// 		KJSGETS(_Title);
// 		KJSGETS(_cachedPath);
// 		KJSGETS(_cachedUrl);
		KJSGETS(_certificate);
		KJSGETS(_privatekey);
		KJSGETS(_dhparam);
		KJSGETS(_prvpwd);
		KJSGETS(_ODBCDSN);
		if(_ODBCDSN.Find('=') < 0)
		{//원래 DSN명만 담았으나, "DSN=dsname;UID=%s;PWD=..." 연결 문장 
			_ODBCDSN.Format(L"DSN=%s", _ODBCDSN);
			js("_ODBCDSN") = _ODBCDSN;
		}
		KJSGETS(_rootLocal);
		KJSGETS(_defFile);
		KJSGETS(_uploadLocal);
		//if (_rootURL.IsEmpty())
		KJSGETS(_rootURL);
		KJSGETS(_ApiURL);

		KJSGETS(_SQL);
		KJSGETS(_SrcImagePath);
		KJSGETS(_note);
		// 		KJSGETS(_UdpSvr);
		// 		KJSGETI(_portUDP);// 숫자인 경우
		KJSGETI(_bDbConnected);// 숫자인 경우
		KJSGETI(_bRecover);// 숫자인 경우
		KJSGETS(_tLastRunning);
		KJSGETS(_fullPath);

	}
}


// SrvDoc commands
BOOL SrvDoc::DoFileSave()//?zzz
{
	auto mf = (CMDIFrameWnd*)AfxGetMainWnd();
	if(mf)
	{
		auto cf = mf->GetActiveFrame();
		if(cf)
		{
			auto vu = (CView*)cf->GetActiveView();
			if(vu)
				vu->UpdateData();
			ASSERT(vu);
		}
		ASSERT(cf);
	}
	ASSERT(mf);

	return __super::DoFileSave();
}
