#include "pch.h"
#include "GdiTool.h"
#include "tchtool.h"
//C:\Dropbox\_gdrv\_box\[]Proj\DYTC\Src\TetrisDy\KGdiTool.cpp




IMPLEMENT_DYNAMIC(CERMemDC, CDC);

//////////////////////////////////////////////////////////////////////
// CERMemDC - implementation

CERMemDC::CERMemDC(CDC* pDC, const CRect& rect, COLORREF clrColor/*=GetSysColor(COLOR_3DFACE)*/)
{
	ASSERT(pDC != NULL);
	m_pDC = pDC;

	// If rect is NULL, use the device context's clip box.
	if(rect.IsRectEmpty())
		m_pDC->GetClipBox(&m_rc);//원래 Window DC 였던 pDC의 영역을 m_rc로 가져 온다.
	else
		m_rc.CopyRect(&rect);

//	if(clrColor == NULL)
		//m_clrColor = pDC->GetBkColor();
//	else
	m_clrColor = clrColor;

	/// Create the memory DC, set Map Mode
	this->CreateCompatibleDC(m_pDC);
	this->SetMapMode(pDC->GetMapMode());

	/// Create a bitmap big enough to hold the window's image
	m_bitmap.CreateCompatibleBitmap(m_pDC, m_rc.Width(), m_rc.Height());

	/// this가 새 비트맵을 선택한다. Select the bitmap into the memory DC
	m_pOldBitmap = this->SelectObject(&m_bitmap);

	// Repaint the background, this takes the place of WM_ERASEBKGND.
	if(clrColor != -1)
	{
		FillSolidRect(m_rc, m_clrColor);
	}

	m_bValid = TRUE;
}

CERMemDC::~CERMemDC()
{
	if(m_bValid)
	{
		/// this(메모리DC)가 선택했던 내부의 m_bitmap을 원래 창DC(m_pDC)에 자동 복사 한다.
		m_pDC->BitBlt(m_rc.left, m_rc.top, m_rc.Width(), m_rc.Height(),
			this, 0, 0, SRCCOPY);
	}
	// Select the original bitmap back in
	SelectObject(m_pOldBitmap);

	// Clean up
	VERIFY(m_bitmap.DeleteObject());
	VERIFY(DeleteDC());
}



COLORREF KwHexStrToColor(LPCWSTR pHex)
{
	//CString hex(pHex);
	//	COLORREF lv = 0;
	try
	{
		int n = (int)_tcslen(pHex);
		int is = 0;
		if(n < 6)
			return 0;
		else if(n > 6) //길면 앞에것 무시 "00ff22aa" 00 무시
			is = n - 6;

		int r, g, b;
		WCHAR buf[3] = { '\0','\0','\0', };
		static LPCWSTR _fmt = _T("%x");
		tchncpy(buf, pHex + is, 3); is += 2;
		swscanf_s(buf, _fmt, &r);
		tchncpy(buf, pHex + is, 3); is += 2;
		swscanf_s(buf, _fmt, &g);
		tchncpy(buf, pHex + is, 3); is += 2;
		swscanf_s(buf, _fmt, &b);
		return RGB(r, g, b);
		//		sscanf(pHex, "%X", &lv);
	}
	catch(...)
	{
		ASSERT(0);
	}
	return 0;
	//	sscanf("11bbCC", "%X", &lv);
	//	sscanf("00bbff", "%X", &lv);
	//	return RGB(Hex2Dec(hex.Left(2)),
	//		Hex2Dec(hex.Mid(2, 2)),
	//		Hex2Dec(hex.Right(2)));
}


// side -1: all, 0:top 1:right 2:botttom 3:left
void KwDraw3DFrame(CDC& dc, CRect rc, int side)
{
	//	logPen.lopnStyle = PS_DOT;
	if(side == -1 || side == 0)
	{
		LOGBRUSH logBrush;
		logBrush.lbStyle = BS_SOLID;
		logBrush.lbColor = RGB(255, 255, 192);

		CBrush brush, * pOldBrush;
		brush.CreateBrushIndirect(&logBrush);
		pOldBrush = (CBrush*)dc.SelectObject(&brush);
		dc.Rectangle(rc);
		if(pOldBrush) dc.SelectObject(pOldBrush);
		brush.DeleteObject();
	}

	CPen* pOldPen = NULL;

	static LOGPEN logPenGray = { PS_SOLID, {0,0}, KW_GRAY };
	static LOGPEN logPenDarkgray = { PS_SOLID, {0,0}, KW_DARKGRAY };

	HPEN hpenWhite = (HPEN)::GetStockObject(WHITE_PEN);
	HPEN hpenBlack = (HPEN)::GetStockObject(BLACK_PEN);

	CPen* penWhite = CPen::FromHandle(hpenWhite);
	CPen* penBlack = CPen::FromHandle(hpenBlack);

	CPen penGray, penDarkgray;

	BOOL ok = penGray.CreatePenIndirect(&logPenGray);
	ok &= penDarkgray.CreatePenIndirect(&logPenDarkgray);
	CPoint p1, p2;
	//////////////// white /////////////////////
	pOldPen = dc.SelectObject(penWhite);
	if(side == -1 || side == 0)
	{
		p1 = CPoint(rc.left + 1, rc.top + 1);
		p2 = CPoint(rc.right - 2, rc.top + 1);
	}
	else if(side == -1 || side == 1)
	{
		p1 = CPoint(rc.right - 4, rc.top + 4);
		p2 = CPoint(rc.right - 4, rc.bottom - 4);
	}
	else if(side == -1 || side == 2)
	{
		p1 = CPoint(rc.left + 4, rc.bottom - 4);
		p2 = CPoint(rc.right - 4, rc.bottom - 4);
	}
	else if(side == -1 || side == 3)
	{
		p1 = CPoint(rc.left + 1, rc.top + 1);
		p2 = CPoint(rc.left + 1, rc.bottom - 2);
	}
	dc.MoveTo(p1);
	dc.LineTo(p2);

	//////////////// black /////////////////////
	dc.SelectObject(penBlack);
	if(side == -1 || side == 0)
	{
		p1 = CPoint(rc.left + 5, rc.top + 5);
		p2 = CPoint(rc.right - 5, rc.top + 5);
	}
	else if(side == -1 || side == 1)
	{
		p1 = CPoint(rc.right, rc.top);
		p2 = CPoint(rc.right, rc.bottom);
	}
	else if(side == -1 || side == 2)
	{
		p1 = CPoint(rc.left, rc.bottom);
		p2 = CPoint(rc.right, rc.bottom);
	}
	else if(side == -1 || side == 3)
	{
		p1 = CPoint(rc.left + 5, rc.top + 5);
		p2 = CPoint(rc.left + 5, rc.bottom);
	}
	dc.MoveTo(p1);
	dc.LineTo(p2);

	//////////////// gray /////////////////////
	dc.SelectObject(&penGray);
	if(side == -1 || side == 0)
	{
		p1 = CPoint(rc.left, rc.top);
		p2 = CPoint(rc.right - 1, rc.top);
		dc.MoveTo(p1); dc.LineTo(p2);
		p1 = CPoint(rc.left + 2, rc.top + 2);
		p2 = CPoint(rc.right - 2, rc.top + 2);
		dc.MoveTo(p1); dc.LineTo(p2);
		p1 = CPoint(rc.left + 2, rc.top + 3);
		p2 = CPoint(rc.right - 2, rc.top + 3);
		dc.MoveTo(p1); dc.LineTo(p2);
	}
	else if(side == -1 || side == 1)
	{
		p1 = CPoint(rc.right - 5, rc.top + 5);
		p2 = CPoint(rc.right - 5, rc.bottom - 5);
		dc.MoveTo(p1); dc.LineTo(p2);
		p1 = CPoint(rc.right - 3, rc.top + 4);
		p2 = CPoint(rc.right - 3, rc.bottom - 2);
		dc.MoveTo(p1); dc.LineTo(p2);
		p1 = CPoint(rc.right - 2, rc.top + 5);
		p2 = CPoint(rc.right - 2, rc.bottom - 2);
		dc.MoveTo(p1); dc.LineTo(p2);
	}
	else if(side == -1 || side == 2)
	{
		p1 = CPoint(rc.left + 5, rc.bottom - 5);
		p2 = CPoint(rc.right - 5, rc.bottom - 5);
		dc.MoveTo(p1); dc.LineTo(p2);
		p1 = CPoint(rc.left + 2, rc.bottom - 3);
		p2 = CPoint(rc.right - 4, rc.bottom - 3);
		dc.MoveTo(p1); dc.LineTo(p2);
		p1 = CPoint(rc.left + 2, rc.bottom - 2);
		p2 = CPoint(rc.right - 4, rc.bottom - 2);
		dc.MoveTo(p1); dc.LineTo(p2);
	}
	else if(side == -1 || side == 3)
	{
		p1 = CPoint(rc.left, rc.top);
		p2 = CPoint(rc.left, rc.bottom - 1);
		dc.MoveTo(p1); dc.LineTo(p2);
		p1 = CPoint(rc.left + 2, rc.top + 2);
		p2 = CPoint(rc.left + 2, rc.bottom - 2);
		dc.MoveTo(p1); dc.LineTo(p2);
		p1 = CPoint(rc.left + 3, rc.top + 2);
		p2 = CPoint(rc.left + 3, rc.bottom - 2);
		dc.MoveTo(p1); dc.LineTo(p2);
	}
	//////////////// dark gray /////////////////////
	dc.SelectObject(&penDarkgray);
	if(side == -1 || side == 0)
	{
		p1 = CPoint(rc.left + 5, rc.top + 5);
		p2 = CPoint(rc.right - 5, rc.top + 5);
	}
	else if(side == -1 || side == 1)
	{
		p1 = CPoint(rc.right - 1, rc.top + 1);
		p2 = CPoint(rc.right - 1, rc.bottom - 1);
	}
	else if(side == -1 || side == 2)
	{
		p1 = CPoint(rc.left + 1, rc.bottom - 1);
		p2 = CPoint(rc.right - 1, rc.bottom - 1);
	}
	else if(side == -1 || side == 3)
	{
		p1 = CPoint(rc.left + 5, rc.top + 5);
		p2 = CPoint(rc.left + 5, rc.bottom - 1);
	}
	dc.MoveTo(p1);
	dc.LineTo(p2);

	if(pOldPen) dc.SelectObject(pOldPen);
	penGray.DeleteObject();
	penDarkgray.DeleteObject();
}



bool KwIsInRect(CRect rc, CPoint pt)
{
	rc.NormalizeRect();
	return rc.left <= pt.x && pt.x < rc.right&& rc.top <= pt.y && pt.y < rc.bottom;
}

PAS KwIsInRectSide(CRect rc, CPoint pt, int m)
{
//	int m{ 2 };
	PAS side = nullptr;
	CRect rcL = KwGetRectSideLeft(rc, m);// (rc.left - m, rc.top, rc.left + m, rc.bottom);
	if(KwIsInRect(rcL, pt))
		side = "left";
	else
	{
		CRect rcT = KwGetRectSideTop(rc, m);// (rc.left, rc.top - m, rc.right, rc.top + m);
		if(KwIsInRect(rcT, pt))
			side = "top";
		else
		{
			CRect rcR = KwGetRectSideRight(rc, m);// (rc.right - m, rc.top, rc.right + m, rc.bottom + m);
			if(KwIsInRect(rcR, pt))
				side = "right";
			else
			{
				CRect rcB = KwGetRectSideBottom(rc, m);// (rc.left, rc.bottom - m, rc.right, rc.bottom + m);
				if(KwIsInRect(rcB, pt))
					side = "bottom";
			}
		}
	}
	return side;
}

IStream* KwCreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType)
{
	IStream* ipStream = NULL;
	HRSRC hrsrc = FindResource(NULL, lpName, lpType);	if(hrsrc == NULL)	goto Return;
	DWORD dwResourceSize = SizeofResource(NULL, hrsrc);
	HGLOBAL hglbImage = LoadResource(NULL, hrsrc);	if(hglbImage == NULL)	goto Return;
	LPVOID pvSourceResourceData = LockResource(hglbImage);	if(pvSourceResourceData == NULL) goto Return;
	HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);	if(hgblResourceData == NULL) goto Return;

	LPVOID pvResourceData = GlobalLock(hgblResourceData);	if(pvResourceData == NULL)	goto FreeData;
	CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
	GlobalUnlock(hgblResourceData);
	if(SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))		goto Return;

FreeData:
	GlobalFree(hgblResourceData);

Return:
	return ipStream;
}

