#pragma once
#include "ktypedef.h"


// 16색 의 RGB값을 정의 해둔다.
#define KW_DARKRED RGB(128,0,0)
#define KW_DARKGREEN RGB(0,128,0)
#define KW_DARKBLUE RGB(0,0,128)

#define KW_DARKCYAN RGB(0,128,128)          
#define KW_DARKMAGENTA RGB(128,0,128)
#define KW_DARKYELLOW RGB(128,128,0)

#define KW_RED RGB(255,0,0)
#define KW_GREEN RGB(0,255,0)
#define KW_BLUE RGB(0,0,255)

#define KW_CYAN RGB(0,255,255)              
#define KW_MAGENTA RGB(255,0,255)
#define KW_YELLOW RGB(255,255,0)

#define KW_BLACK RGB(0,0,0)
#define KW_GRAY RGB(192,192,192)
#define KW_DARKGRAY RGB(128,128,128)
#define KW_WHITE RGB(255,255,255)

class CERMemDC : public CDC
{
public:
	DECLARE_DYNAMIC(CERMemDC);

	CERMemDC(CDC* pDC,const CRect& rect,COLORREF clrColor = RGB(255, 255, 255));//GetSysColor(COLOR_3DFACE));

	virtual ~CERMemDC();

	void Discard()
	{
		m_bValid = FALSE;
	}

	// Get content from the given DC
	void FromDC()
	{
		BitBlt(0, 0, m_rc.Width(), m_rc.Height(), m_pDC, m_rc.left, m_rc.top, SRCCOPY);
	}

	CBitmap& GetBitmap() { return m_bitmap; }

protected:
	CDC* m_pDC;        // Saves CDC passed in constructor
	CRect    m_rc;         // Rectangle of drawing area.
	CBitmap  m_bitmap;     // Offscreen bitmap
	COLORREF m_clrColor;   // Background fill color

	CBitmap* m_pOldBitmap; // Original GDI object
	BOOL	 m_bValid;	   // flag for autodraw in dtor
};
/*

	CPaintDC dc(this); // device context for painting
	// 그리기메시지에대해서는__super::OnPaint()을(를) 호출하지마십시오.

	CDC* pDC = (CDC*)&dc;
	CAutoSmoothDraw _asd(m_rcDraw, pDC, pDC->GetBkColor());
	pDC = _asd.GetDrawDC();
	{
		CAutoPen aupen(KW_BLUE, pDC);
		pDC->Rectangle(m_rcDraw);
	}
	===========================================
void CTetrisDyView::OnDraw(CDC* pDC)
{
	CRect rcc;
	GetClientRect(rcc);
#ifdef _Use_Smooth
	CAutoSmoothDraw _asd(rcc, pDC);
	pDC = _asd.GetDrawDC();
#endif // _Use_Smooth

*/
class CAutoSmoothDraw
{
public:
	CDC* m_pDC;
	CERMemDC m_dcMem;
	int m_nSavedDC;

	/// clrColor =  pDC->GetBkColor();
	CAutoSmoothDraw(CRect rc, CDC* pDC)//, COLORREF clrColor = 0)
		: m_pDC(pDC)
		, m_dcMem(pDC, rc)
		, m_nSavedDC(0)
	{
		//CERMemDC dcm(pDC, rcClient);
		m_nSavedDC = m_dcMem.SaveDC();
		//pDC = &dcm;
	}

	~CAutoSmoothDraw()
	{
		VERIFY(m_dcMem.RestoreDC(m_nSavedDC));//kdw
	}

	CDC* GetDrawDC()
	{
		return &m_dcMem;
	}

};


inline CRect KwGetRectSideLeft(CRect rc, int wfrm = 2)
{
	return CRect(rc.left - wfrm, rc.top, rc.left + wfrm, rc.bottom);
}
inline CRect KwGetRectSideTop(CRect rc, int m = 2)
{
	return CRect(rc.left, rc.top - m, rc.right, rc.top + m);
}
inline CRect KwGetRectSideRight(CRect rc, int m = 2)
{
	return CRect(rc.right - m, rc.top, rc.right + m, rc.bottom + m);
}
inline CRect KwGetRectSideBottom(CRect rc, int m = 2)
{
	return CRect(rc.left, rc.bottom - m, rc.right, rc.bottom + m);
}

bool KwIsInRect(CRect rc, CPoint pt);

// rect 엣지 부분 어디서 마우스가 얼쩡거리느냐 
PAS KwIsInRectSide(CRect rc, CPoint pt, int wfrm = 2);
IStream* KwCreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType = L"PNG");
inline IStream* KwCreateStreamOnResource(int idc, LPCTSTR lpType = L"PNG")
{
	return KwCreateStreamOnResource(MAKEINTRESOURCE(idc), lpType);
}
