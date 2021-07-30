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
	auto app = (CMFCExHttpsSrvApp*)GetMainApp();
	auto& appd = (app)->_docApp;
	//_fullPath = GetPathName();
	AUTOLOCK(_csJdata);
	if(!_jdata.Len("_GUID"))//.IsEmpty())
	{
		CString guid;
		if(appd.PopRecoveringServer(guid))//?server recover 4
			_jdata("_GUID") = guid;
		else
			_jdata("_GUID") = KwGetFormattedGuid();
	}

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}

BOOL SrvDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	BOOL b = __super::OnOpenDocument(lpszPathName);
	AUTOLOCK(_csJdata);
	_jdata("_fullPath") = lpszPathName;
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

//#define KJJPUT(k) js.CopyAt(#k, _jdata.Get(#k))

void SrvDoc::JsonToData(ShJObj& sjobj, bool bToJson)
{
	AUTOLOCK(_csJdata);
	if(bToJson)//ar.IsStoring())
	{
		if(!sjobj)
			sjobj = std::make_shared<JObj>();///이거 때문에 파라미터를 ShJObj&
//		JObj& js = *sjobj;
		ASSERT(!_jdata.IsEmpty("_GUID"));

		sjobj->Clone(_jdata, true);

#ifdef _DEBUGxxxxxxx
		//	_jdata("_GUID") = KwGetFormattedGuid();
//#define KJSPUT(val) js(#val) = val
		js("_GUID") = _jdata.Get("_GUID");
//		KJJPUT(_GUID);
		KJJPUT(_port);
		KJJPUT(_bSSL);
		KJJPUT(_bStaticCache);
		KJJPUT(_CacheLife);
		//		KJSPUT(_Title);
		// 		KJSPUT(_cachedPath);
		// 		KJSPUT(_cachedUrl);
		KJJPUT(_certificate);
		KJJPUT(_privatekey);
		KJJPUT(_prvpwd);
		KJJPUT(_dhparam);
		KJJPUT(_ODBCDSN);
		KJJPUT(_rootLocal);
		KJJPUT(_defFile);
		KJJPUT(_uploadLocal);
		KJJPUT(_rootURL);
		KJJPUT(_ApiURL);
		// 		KJSPUT(_UdpSvr);
		// 		KJSPUT(_portUDP);
		KJJPUT(_SQL);
		KJJPUT(_SrcImagePath);
		KJJPUT(_note);
		//	JSPUT(_SrcTable);
		//	JSPUT(_SrcKeyField);

		KJJPUT(_bDbConnected);
		KJJPUT(_bRecover);
		KJJPUT(_tLastRunning);
		KJJPUT(_fullPath);
#endif // _DEBUGxxxxxxx
	}
	else
	{
		ASSERT(!sjobj->IsEmpty("_GUID"));
		_jdata.Clone(sjobj, true);

#ifdef _DEBUGxxxxx
		//auto& js = *jdoc->AsObject().get();
		JObj& js = *sjobj;
#ifdef _DEBUGxxxxx
#define KJSGETS(val) val = js.S(#val)
#define KJSGETSA(val) val = CStringA(js.S(#val))
#define KJSGETN(val) val = js.N(#val)
#define KJSGETI(val) val = js.I(#val)
#endif // _DEBUGxxxxx
//#define KJJPUT(val) js(#val) = _jdata.Get(#val)
#define KJJGETS(val) _jdata(#val) = js.Get(#val)

		KJSGETS(_GUID);
		//ASSERT(!_GUID.IsEmpty());
		if(_GUID.IsEmpty())
			_jdata("_GUID") = KwGetFormattedGuid();
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
#endif // _DEBUGxxxxx
	}
}


/// 저장할때 UpdateData로 최신 건드린거 읽어 들이기 위해
// SrvDoc commands
BOOL SrvDoc::DoFileSave()//?zzz
{
	AUTOLOCK(_csJdata);
	auto mf = (CMDIFrameWnd*)AfxGetMainWnd();
	if(mf)
	{
		auto cf = mf->GetActiveFrame();
		if(cf)
		{
			auto vu = (CView*)cf->GetActiveView();
			if(vu)
				vu->UpdateData();
		}
	}

	return __super::DoFileSave();
}
