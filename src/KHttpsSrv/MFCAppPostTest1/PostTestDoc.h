
// PostTestDoc.h: CPostTestDoc 클래스의 인터페이스
//

#include "KwLib64/DlgTool.h"

#pragma once
/* DlgTool.h 에
#define KDDX_Text(field)  DDX_Text( pDX, IDC##field, doc->field)
#define KDDX_Int(field)  DDX_Text( pDX, IDC##field, doc->field) // text랑 동일

#define KDDX_CBBox(field) DDX_CBString(pDX, IDC##field, doc->field)
#define KDDX_Radio(field) DDX_Radio(pDX, IDC##field, doc->field)

#define KDDX_TextA(field)  { if(pDX->m_bSaveAndValidate) { CString v; \
    DDX_Text(pDX, IDC##field, v); doc->field = CStringA(v);} else { \
    DDX_Text(pDX, IDC##field, CStringW(doc->field)); }}

#define KDDX_CBBoxA(field) { if(pDX->m_bSaveAndValidate) {\
    CString v; DDX_CBString(pDX, IDC##field, v); doc->field = CStringA(v);} else { \
    DDX_CBString(pDX, IDC##field, CStringW(doc->field)); }}
*/

class CPostTestDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CPostTestDoc() noexcept;
	DECLARE_DYNCREATE(CPostTestDoc)

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);

	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CPostTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


	CStringA _Method{ "POST" };
#ifdef _DEBUG
	CString  _URL{L"https://localhost:443/api?func=ExGetApiDesc" };
#else
	CString  _URL{L"http://localhost/api?func=ExGetApiDesc"};
#endif
	CStringA _Request{ "{\"func\":\"ExGetApiDesc\", \"params\": {\"pa1\":\"data1\", \"pa2\":100}}" };
	CStringA _Response;
	UINT      _Count{ 1 };
	int      _Delay{ 0 };
	int      _Thread{ 4 };
	BOOL     _noCache{ TRUE };
	BOOL     _Parallel{ TRUE };
	BOOL     _AddSrl{ TRUE };
	BOOL     _AddUuid{ TRUE };

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
};
