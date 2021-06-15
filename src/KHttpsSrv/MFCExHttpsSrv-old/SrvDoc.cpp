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

#include "SrvDoc.h"

#include <propkey.h>

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

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
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
#endif //_DEBUG


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
