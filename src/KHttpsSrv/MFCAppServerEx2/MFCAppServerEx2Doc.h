
// MFCAppServerEx2Doc.h: CMFCAppServerEx2Doc 클래스의 인터페이스
//


#pragma once

#include "HttpsSvr.h"
#include "HttpSvr.h"
#include "Response1.h"
#include "CmnDoc.h"

class CMFCAppServerEx2Doc 
	: public CDocument
	, public CmnDoc
{
protected: // serialization에서만 만들어집니다.
	CMFCAppServerEx2Doc() noexcept;
	DECLARE_DYNCREATE(CMFCAppServerEx2Doc)

// 특성입니다.
public:

	//CString _SrcTable{ L"style_category" };
//	CString _SrcKeyField{ L"idx" };

// 작업입니다.
public:

	//shared_ptr<CHttpSvr> _bodyWin;

// 재정의입니다.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL DoFileSave();
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CMFCAppServerEx2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
