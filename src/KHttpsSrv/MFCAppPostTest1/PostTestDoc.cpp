
// PostTestDoc.cpp: CPostTestDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MFCAppPostTest1.h"
#endif

#include "PostTestDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPostTestDoc

IMPLEMENT_DYNCREATE(CPostTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CPostTestDoc, CDocument)
END_MESSAGE_MAP()


// CPostTestDoc 생성/소멸

CPostTestDoc::CPostTestDoc() noexcept
{
}

CPostTestDoc::~CPostTestDoc()
{
}

BOOL CPostTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}



#include "KwLib64/KJson.h"
using namespace Kw;

// CPostTestDoc serialization

void CPostTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		//CJsonPbj js;
		JObj js;
		//js["xxx"] = xxx;
		KJSPUT(_Method);
		KJSPUT(_URL);
		KJSPUT(_Request);
		KJSPUT(_Response);
		KJSPUT(_Count);
		KJSPUT(_Delay);
		KJSPUT(_Thread);
		KJSPUT(_noCache);
		KJSPUT(_Parallel);
		KJSPUT(_AddSrl);
		KJSPUT(_AddUuid);
		CFile* fr = ar.GetFile();

		CStringA sUtf8 = js.ToJsonStringUtf8();
		fr->Write((PAS)sUtf8, sUtf8.GetLength());

	}
	else
	{
		CFile* fr = ar.GetFile();
		auto len = fr->GetLength();
		CStringA sa;
		char* buf = sa.GetBufferSetLength((int)len);
		fr->Read(buf, (int)len);
		CString sWstr;
		KwUTF8ToWchar(buf, sWstr);

		//JSONValue* jdoc = JSON::Parse((PWS)sWstr);
		//KAtEnd d_jdoc([&]() {
		//	delete jdoc; });
		//CJsonPbj js(jdoc->AsObject());
		auto jdoc = ShJVal(Json::Parse((PWS)sWstr));
		if(jdoc.get() == nullptr)
		{
			AfxMessageBox(L"파일 포맷 오류.");
			return;
		}
		auto& js = *jdoc->AsObject().get();

		KJSGETS(_Method);
		KJSGETS(_URL);
		KJSGETS(_Request);
		KJSGETS(_Response);
		// 숫자인 경우
		KJSGETI(_Count);
		KJSGETI(_Delay);
		KJSGETI(_Thread);
		KJSGETI(_noCache);
		KJSGETI(_Parallel);
		KJSGETI(_AddSrl);
		KJSGETI(_AddUuid);
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CPostTestDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
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

// 검색 처리기를 지원합니다.
void CPostTestDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CPostTestDoc::SetSearchContent(const CString& value)
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

// CPostTestDoc 진단

#ifdef _DEBUG
void CPostTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPostTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CPostTestDoc 명령


BOOL CPostTestDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(m_viewList.IsEmpty()) 
		return FALSE;//nullptr;
	CFormView* view = dynamic_cast<CFormView*>((CView*)m_viewList.GetHead());
	view->UpdateData();

//출처: https://swjman.tistory.com/17 [JMAN's SW Lab.]
	return CDocument::OnSaveDocument(lpszPathName);
}
BOOL CPostTestDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	return CDocument::OnOpenDocument(lpszPathName);
}