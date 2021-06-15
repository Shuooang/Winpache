#include "pch.h"
#include "TimeTable.h"
#include "KwLib64/KDebug.h"
#include "KwLib64/TimeTool.h"
#include "KwLib64/DlgTool.h"
#include "KwLib64/GdiTool.h"
#include "KwLib64/Kw_tool.h"
#include "KwLib64/KTemple.h"

using namespace Gdiplus;			// The "umbella"


void TimeTable::Rfresh()
{
}


void TimeTable::ArrangeColumn()
{
	TRACE("%s \n", __FUNCTION__);
	AUTOLOCK(_cs_jtbl); // _jtbl 이 멀티로 접근 하므로 락해야.
	_mapCellMode.clear();
	_col.clear();
	_nRows = 48;
	_nCols = 4;
	//CJsonPbj* _pjtbl{ nullptr };
	//CKCriticalSection* _pcs_jtbl{ nullptr };
	//KPtrArray<CJsonPbj> _col;// 안에 fOrderSched에 따라 배열을 만든다.
	//CJsonPbj& _jtbl = *_pjtbl;
	JObj& jtbl = *_jtbl.get();
	// { vc1: { :,:,:,}, vc2: { :,:,:, }, 4}
	JObj& jchg = *jtbl.O("charge");
	// { vc1: [ {:,:,:,},{:,:,:,},{:,:,:,},,,,48], vc1: [ {:,:,:,},{:,:,:,},{:,:,:,},,,,48],,, 4}
	JObj& joccs = *jtbl.O("occupy");
	
	KMulMap<double, ShJObj> mpCol;
	
	for(auto& [vc, vcObj] : jchg)//가로 정렬필드 임시 map을 만들고 배열에 넣는다.
	{
		auto jobj = vcObj->AsObject();
		JObj& vco = *jobj;
		auto dord = vco.D("fOrderSched");
		//while(mpCol.Has(dord))
		//	dord += 0.001;//실수로 fOrderSched가 같은 경우는 어떻게라도 수정
		//mpCol[dord] = &jobj;
		mpCol.Insert(dord, jobj);
		KTrace(L"%s\t%s\t%s\t%.3f\t%.3f\n", vco.S("fPosition"), vco.S("fUsrID"), vco.S("fNickName"), vco.D("fOrderSched"), dord);
	}

	//_col.m_bAutoFree = FALSE;
	//for(auto& [dord, vcObj] : mpCol)
	for(auto itr = mpCol.begin(); itr != mpCol.end(); itr++)
	{
		auto& vcObj = itr->second;
		_col.Add((ShJObj)vcObj);// vcObj);
	}//컬럼 정보를 배열로 만든다. 셀 아님


	int minu = jtbl.I("fMinuteForCut30");// 30; // work unit minute : 30분 간격을 일단위로 전제.
	int m48 = jtbl.I("fNumberOfTimeCut48");// 24 * (60 / minu); // 배열 갯수 48개
	CKTimeSpan sp30(0, 0, minu, 0);
	_nRows = m48;
	_nCols = (int)jchg.size();
	_minute = minu;
	_maxIdx = -1;
	// 이제 _col[n] 은 n번째 컬럼 정보 이다.
	/*
	"fMode":"closed",
	"fNote":"영업외 시간",
	"fOff":1,
	"fOffBegin":"2021-02-09 00:00:00",
	"fOffEnd":"2021-02-09 10:00:00",
	"fRepeat":"once",
	"fSchedOrgID":"biz-0002",
	"fTime":"2021-02-09 03:30:00"
	*/

	for(int c = 0; c < _col.size(); c++)// auto & jbx : _col) // cols 이지 cell이 아니다
	{
		auto shcol = _col[c];//JSONObject
		JObj& jcol = *shcol;
		auto vc = jcol.S("fVChrgID");
		auto shjar = joccs.Array(vc);
		auto& jar = *shjar;
		//JArr jar(arr);
		for(int r = 0; r < jar.size(); r++)
		{
			auto jxv = jar[r];
			ShJObj shjx = jxv->AsObject();
			JObj& jx = *shjx.get();

			/// fPause 정의
			///		holiday, closed, open, work 4가지가 있다. 
			///		닫는것은 공휴일,영업시간만 있고, 특별히 닫는 것은 그냥 박스일정으로 한다.
			///		fPause 는 모든 c,r에 있다.
			if(jx.Has("fPause"))
				AddCellMode(c, r, jx.S("fPause"));

			auto fOff = jx.I("fOff");//점유여부 예약이 어려우면 1
			if(!jx.Has("fIDX"))
			{
				//ASSERT(fOff == 0);//예약 가능 한 빈 시간 영역
				continue;
			}

			auto fIDX = jx.I("fIDX");
			if(_maxIdx < fIDX)// add할때 임시 unique 변수 이므로 그냥 최고점을 구해둔다.
				_maxIdx = fIDX;
			CString fSchedOrgID(jx.S("fSchedOrgID"));
			if(fSchedOrgID == L"schedule-18B6BF54")
				_break;
			CString fVChrgID = jx.S("fVChrgID");//셀 일정에 할당단 담당자
			CKTime fOffBegin = jx.T("fOffBegin");//일정의 시작 시각
			CKTime fOffEnd = jx.T("fOffEnd");//일정의 끝 시가
			CKTime fTime = jx.T("fTime");//셀의 시각
			//if(c == 0 && r == 0)
			//	_theDay = fTime;
			CString fMode = jx.S("fMode");//closed, order, schedule
			CString fNote = jx.S("fNote");//closed, order, schedule
			int ib = fOffBegin.GetHour() * (60 / minu) + (fOffBegin.GetMinute() / minu);//=(HOUR(B46) * (60/A46)) + MINUTE(B46)/A46
			int hourEnd = fOffEnd.GetHour() == 0 ? 24 : fOffEnd.GetHour();
			int ie = hourEnd * (60 / minu) + (fOffEnd.GetMinute() / minu);


			CString sTime(jx.S("fTime"));
			sTime = sTime.Mid(11, 5);
			CString sOffBegin(jx.S("fOffBegin"));
			sOffBegin = sOffBegin.Left(16);
			CString sOffEnd(jx.S("fOffEnd"));
			sOffEnd = sOffEnd.Left(16);
			//KTrace(L"%s\t%s\t%s\t%s\t%s\t%s\n", sTime, sOffBegin, sOffEnd, jocc.S("fMode"), jocc.S("fVChrgID"), jocc.S("fNote"));

			//if(fSchedOrgID.IsEmpty())
			//	continue;

			//if(fSchedOrgID == L"shedule-0a06" && c == (_c.size() - 1))
			//	_break;
			//if(fSchedOrgID == L"shedule-0011")
			//	_break;
			CString sfVChrgID = fVChrgID.IsEmpty() ? L"(NULL)" : fVChrgID;
			//int keyShed = MakeBoxKey(jx);//

			//if(fMode == L"closed" && fPause == 1)
			//{
			//	AddCellMode(c, r, fMode);
			//	continue;
			//}
			//else if((fMode == L"closed" || fMode == L"schedule") && fPause == 0)//fOff == 0) // 특별 
			//{
			//	if(fVChrgID.GetLength() > 0)//특별출근
			//	{
			//		AddCellMode(c, r, L"work");
			//		continue;
			//	}
			//	else//특별오픈
			//	{
			//		AddCellMode(c, r, L"open");
			//		continue;
			//	}
			//}
			if(!_mapBoxes.Has(fIDX))
			{
				bool bStart = false;
				if(c == 0)
					bStart = true;
				else
				{
					if(fVChrgID.GetLength() > 0)//개인일정
					{
						bStart = true;
					}
				}

				if(bStart)
				{
					TBox* ptx = new TBox;
					TBox& tx = *ptx;
					tx._jsch = shcol;//컬럼/담당
					tx._jocc = shjx;//컬럼/배열/일정
					CKTimeSpan sp = fOffEnd - fOffBegin; // 일단 시간 간격을 백업해 두고
					int dr = (int)sp.GetTotalMinutes() / _minute;// 간격을 셀 분 단위로
					tx._rowT = r;
					tx._rowB = r + dr;
					tx._colL = c;//ArrangeColumn
					tx._colR = c + (fVChrgID.IsEmpty() ? (_nCols-1) : 1);
					tx._rc.left = getRcHeaderHorz(c).left;
					tx._rc.top  = getRcHeaderVert(r).top;
					//tx.fSchedOrgID = fSchedOrgID;
					//KTrace(L">>\t%d\t%s\t%s\t%d\t%d\t\t\t%s\n", c, keyShed, sfVChrgID, tx._rc.left, tx._rc.top,  fNote);
					_mapBoxes[fIDX] = ptx;
				}
			}

			/// 통과 하는 장기 시간은 오늘과 날짜가 달라서 같은날로 수정후 같은지 계산 한다.
			CKTime tdy(fTime.GetYear(), fTime.GetMonth(), fTime.GetDay(), 0, 0, 0);
			CKTime tdyNext = tdy + CKTimeSpan(1, 0, 0, 0);
			CKTime tdEnd;
			if(fOffEnd.GetHour() == 0 && fOffEnd.GetMinute() == 0 && fOffEnd.GetSecond() == 0)//끝나는 시간이 00:00 다음날 00:00이다.
				tdEnd = tdyNext;
			else
				tdEnd = CKTime(tdy.GetYear(), tdy.GetMonth(), tdy.GetDay(), fOffEnd.GetHour(), fOffEnd.GetMinute(), fOffEnd.GetSecond());
			CKTime tEdge = fTime + sp30;
			//CString s_tdEnd, s_fTime, s_tEdge;
			//KwCTimeToString(tdEnd, s_tdEnd);
			//KwCTimeToString(fTime, s_fTime);
			//KwCTimeToString(tEdge, s_tEdge);

			if(tdEnd == (fTime + sp30))
			{
				bool bFinish = false;
				if(fVChrgID.GetLength() > 0)///개인일정. 가상담당 tbizvirtual에 등록이 안된 사람의 일정은 박스가 안그려 진다.
				{
					bFinish = true;
				}
				else//공통 일정
				{
					if(c == (_col.size() - 1))
						bFinish = true;
				}

				if(bFinish)
				{
					TBox* ptx{ nullptr };
					ptx = _mapBoxes[fIDX];
					auto& txb = *ptx;
					ptx->_rowB = r + 1;//마지막 칸에 bottom은  아래쪽 칸이다.
					txb._rc.right = getRcHeaderHorz(c).right;
					ptx->_rc.bottom = getRcHeaderVert(r).bottom;
					//KTrace(L"<<\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%s\n", c, fIDX, sfVChrgID, ptx->_rc.left, ptx->_rc.top, ptx->_rc.right, ptx->_rc.bottom, fNote);
					ptx = _mapBoxes[fIDX];
					ptx = nullptr;
					//_boxes.Add(ptx);
					//ptx = nullptr;
				}
			}
		}
	}
	/// 이제 _boxes dp 일정을 나타내는 TBox가 생겼다.
}

void TimeTable::Draw3DRect(CRect rc, float wth)// = 1.0)
{
	const Color clLT(120, 255, 255, 255);
	const Color clRB(70, 0, 0, 0);
	Pen ptw(clLT, wth);
	Pen ptb(clRB, wth);

	//rc.left++;
	//rc.top++;
	rc.right--;
	rc.bottom--;
	//Gdiplus::Rect rc1(rc.left, rc.top, rc.Width(), rc.Height());
	Point pt1(rc.left, rc.top);
	Point pt2(rc.right, rc.top);
	Point pt3(rc.left, rc.bottom);
	Point pt4(rc.right, rc.bottom);

	_gc->DrawLine(&ptw, pt1, pt2);
	_gc->DrawLine(&ptw, pt1, pt3);
	_gc->DrawLine(&ptb, pt3, pt4);
	_gc->DrawLine(&ptb, pt4, pt2);
}
void TimeTable::DrawTimeTable()
{
	InitGC(_hDC);
	//Graphics gc(_hDC);
	/*
	LinearGradientBrush linGrBrush(Point(0, 10),
		Point(700, 10),
		Color(255, 255, 255, 230),   // opaque red
		Color(255, 230, 255, 255));  // opaque blue

	Pen pen(&linGrBrush);
	graphics.DrawLine(&pen, 0, 10, 200, 10);
	//graphics.FillEllipse(&linGrBrush, 0, 30, 200, 100);
	graphics.FillRectangle(&linGrBrush, 100, 80, 600, 600);

	FontFamily myfontFamily(L"돋움");// Century");
	Gdiplus::Font myfont(&myfontFamily, 26, FontStyleRegular, UnitPixel);
	Pen greenPen(Color(255, 0, 255, 0));
	SolidBrush solidBrush(Color(255, 0, 0, 255));
	const Color color(255, 0, 0, 255);
	SolidBrush brush(color);
	LPCWSTR s1 = L"Smooth Text TBLR";
	graphics.DrawString(s1, lstrlen(s1), &myfont, PointF(0.0f, 0.0f), &solidBrush);
	PointF position(125.0f, 125.0f);
	LPCWSTR s2 = L"DrawString";
	graphics.DrawString(s2, lstrlen(s2), &myfont, PointF(100, 100), &brush);
	*/
	SolidBrush brush(Color(255, 55, 55, 55));
	if(!_bRedraw)
	{
		Gdiplus::Rect rc1 = GetMapRc(_rcc);// (_rcc.left, _rcc.top, _rcc.Width(), _rcc.Height());
		_gc->FillRectangle(&brush, rc1);// .left, rc.top, rc.Width(), rc.Height());
		_gc->Flush();
		return;
	}
	CRect rc = Rc();
	Gdiplus::Rect rc1 = GetMapRc(rc.left, rc.top, rc.Width() + 1, rc.Height() + 1);
	_gc->FillRectangle(&brush, rc1);// .left, rc.top, rc.Width(), rc.Height());

	DrawConer();
	DrawHeadHorz();
	DrawHeadVert();
	DrawCells();
	DrawLines();

	DrawBoxes();

}

//#include <tuple>


void TimeTable::DrawBoxes()
{
	//TRACE("%s \n", __FUNCTION__);
	for(auto& [sch, box] : _mapBoxes)
	{
		DrawOneBox(box, 0.5f);
	}
	///  맨앞의로 보내기
	if(_selBox)
		DrawOneBox(_selBox, 2.f);
//		Draw3DRect(_selBox->_rc, 3.);
	if(_curBox)
		DrawOneBox(_curBox, 1.f);
	//Draw3DRect(_curBox->_rc, 2.);
}


void TimeTable::DrawOneBox(TBox* box, float wth)
{
	//TRACE("%s \n", __FUNCTION__);
	TBox& txb = *box;
	JObj& jsch = *txb._jsch.get();
	JObj& jocc = *txb._jocc.get();
	CString fNote = jocc.S("fNote");
	CString fMode = jocc.S("fMode");
	CString fVChrgID = jocc.S("fVChrgID");//셀 일정에 할당단 담당자
	CString fSchedOrgID = jocc.S("fSchedOrgID");//셀 일정에 할당단 담당자

	Gdiplus::Rect rc1 = GetMapRc(box->_rc.left, box->_rc.top, box->_rc.Width() - 1, box->_rc.Height() - 1);
	Color cl;

	const Color clOrd(155, 250, 100, 97);
	const Color clSch(155, 204, 192, 217);
	const Color clCmn(155, 150, 190, 200);
	if(fVChrgID.IsEmpty())
		cl = clCmn;
	else if(fMode == L"order")
		cl = clOrd;
	else //if(fMode == L"schedule")
		cl = clSch;

	SolidBrush br(cl);
	TBox& tx = *box;
	//TRACE("(%d,%d) (%d,%d) (%d,%d,%d,%d) %s\n", tx._colL, tx._rowT, tx._colR, tx._rowB, tx._rc.left, tx._rc.top, tx._rc.Width(), tx._rc.Height(), fSchedOrgID);
	int fIDX = tx._jocc->I("fIDX");
	if(rc1.Width > 0.f && rc1.Height > 0.f)
	{
		_gc->FillRectangle(&br, rc1); //getBrushBox()
//	_gc->DrawRectangle(getPenBox(), rc1);
		Draw3DRect(box->_rc, wth);
	}
	CStringArray artxt;
	CString txt;
	if(fMode == L"order")
	{
		KwCutByToken(fNote, L"\t", artxt);
		txt.Format(L"예약:%d", fIDX);
	}
	else if(fMode == L"schedule")
	{
		int cut = 6;
		if(fVChrgID.GetLength() == 0)
		{
			txt = fNote;// L"샵 일정"; 한줄에 표현
			cut *= _nCols; // 컬럼 폭에 따라 글자 수를 한컬럼에 4자로 정한다.
		}
		else
			txt.Format(L"일정:%d", fIDX);

		for(int i = 0; i < fNote.GetLength(); i += cut)
			artxt.Add(fNote.Mid(i, cut));
	}
	else
		txt = fNote;
	if(rc1.Width > 0.f && rc1.Height > 0.f)
	{
		FontFamily myfontFamily(L"돋움");// Century");FontStyleBold
		Gdiplus::Font myfont(&myfontFamily, 13, FontStyleRegular, UnitPixel);
		Gdiplus::Font myfont2(&myfontFamily, 11, FontStyleRegular, UnitPixel);
		const Color clt(255, 0, 0, 10);
		const Color clt2(255, 30, 30, 60);
		SolidBrush brush(clt);
		SolidBrush brush2(clt2);
		CSize szmg(3, 3);
		PointF pt((float)rc1.GetLeft() + szmg.cx, (float)rc1.GetTop() + szmg.cy);
		_gc->DrawString(txt, txt.GetLength(), &myfont, pt, &brush);
		CSize szCell = getSzCell();
		int nline = box->_rowB - box->_rowT - 1;//이미 하나 썼으니 -1
		for(int l = 0; l < artxt.GetCount(); l++)
		{
			if(l > nline - 1)
				return;
			CString t = artxt[l];
			pt.Y += szCell.cy;
			_gc->DrawString(t, t.GetLength(), &myfont2, pt, &brush2);
		}
	}
}

void TimeTable::DrawLines()
{
	CRect rc = getRcCell(0, 0);
	CRect rcbt = getRcCell(_nCols-1, _nRows-1);
	Point pt1(rc.left   , rc.bottom);
	Point pt2(rcbt.right, rc.bottom);
	for(int r = 0; r < _nRows; r++)
	{
		Pen* pen = getPenCell(r);
		_gc->DrawLine(pen, pt1, pt2);
		pt1.Y += rc.Height();
		pt2.Y += rc.Height();
	}
	Point pt3(rc.right, rc.top);
	Point pt4(rc.right, rcbt.bottom);
	for(int c = 0; c < _nCols; c++)
	{
		Pen* pen = getPenCell(0);
		_gc->DrawLine(pen, pt3, pt4);
		pt3.X += rc.Width();
		pt4.X += rc.Width();
	}


}
void TimeTable::DrawConer()
{
	LinearGradientBrush brLine(Point(0, 10),
		Point(700, 10),
		Color(255, 255, 230),   // opaque red
		Color(230, 255, 255));  // opaque blue
	Pen p1(Color(192, 255, 192));

	CRect rc = getRcConer();
	Gdiplus::Rect rc1 = GetMapRc(rc.left, rc.top, rc.Width()-1, rc.Height());
	//Pen pen(&brLine); 브러시를 펜으로 쓰네
	_gc->FillRectangle(&brLine, rc1);// .left, rc.top, rc.Width(), rc.Height());
	_gc->DrawRectangle(getPenHeaderHorz(), rc1);
}

void TimeTable::DrawHeadHorz()
{
	//Pen p1(Color(255, 192, 255, 192));
	//SolidBrush brF(Color(255, 200, 200, 245));
	for(int i = 0; i < _nCols; i++)
	{
		CRect rc = getRcHeaderHorz(i);
		Gdiplus::Rect rc1 = GetMapRc(rc.left, rc.top, rc.Width(), rc.Height());
		_gc->FillRectangle(getBrushHeaderHorz(), rc1);
		_gc->DrawRectangle(getPenHeaderHorz(), rc1);
		if(_col.size() > i)
		{
			auto col = _col[i];
#ifdef DEBUG
			CString txt = col->S("fVChrgID");
			txt = txt.Mid(8);
#else
			CString txt = col->S("fNickName");
#endif
			FontFamily myfontFamily(L"Consolas");// Century");
			Gdiplus::Font myfont(&myfontFamily, 12, FontStyleRegular, UnitPixel);
			SolidBrush brush(Color(255, 55, 55, 55));
			_gc->DrawString(txt, txt.GetLength(), &myfont, PointF((float)(rc1.GetLeft() + 13), (float)(rc1.GetTop() + 3)), &brush);
		}
	}
}

void TimeTable::DrawHeadVert()
{
	//Pen p1(Color(255, 192, 255, 192));
	//SolidBrush brF(Color(255, 200, 200, 245));
	CKTime t = _day;
	CKTimeSpan sp30(0, 0, _minute, 0);
	for(int i = 0; i < _nRows; i++)
	{
		CRect rc = getRcHeaderVert(i);
		Gdiplus::Rect rc1 = GetMapRc(rc.left, rc.top, rc.Width()-1, rc.Height());
		_gc->FillRectangle(getBrushHeaderVert(), rc1);
		_gc->DrawRectangle(getPenHeaderVert(), rc1);

		CString txt; txt.Format(L"%02d:%02d", t.GetHour(), t.GetMinute());

		FontFamily myfontFamily(L"Consolas");// Century");
		Gdiplus::Font myfont(&myfontFamily, 12, FontStyleRegular, UnitPixel);
		SolidBrush brush(Color(255, 55, 55, 55));
		_gc->DrawString(txt, txt.GetLength(), &myfont, PointF((float)(rc1.GetLeft() + 13), (float)(rc1.GetTop() + 3)), &brush);

		t += sp30;
	}
}

void TimeTable::DrawCells()
{
	//Pen p1(Color(255, 192, 255, 192));
	//SolidBrush brF(Color(255, 255, 192, 192));
	for(int r = 0; r < _nRows; r++)
	{
		for(int c = 0; c < _nCols; c++)
		{
			CRect rc = getRcCell(c, r);
			int adj = r == 0 ? 1 : 0;
			Gdiplus::Rect rc1 = GetMapRc(rc.left, rc.top+ adj, rc.Width(), rc.Height()- adj);
			Brush* br = getBrushCell(c, r);

			_gc->FillRectangle(br, rc1);
			//_gc->DrawRectangle(getPenCell(), rc1);
#ifdef DEBUG1
			if(r == (_nRows - 1))
			{
				CPoint scpos(rc.TopLeft());
				///KwGetViewScrolledPos((CScrollView*)_wnd, scpos); 
				CString txt; txt.Format(L"%d,%d", scpos.x, scpos.y);
				FontFamily myfontFamily(L"Consolas");// Century");
				Gdiplus::Font myfont(&myfontFamily, 12, FontStyleRegular, UnitPixel);
				SolidBrush brush(Color(255, 55, 55, 55));
				_gc->DrawString(txt, txt.GetLength(), &myfont, PointF((float)(rc1.GetLeft() + 13), (float)(rc1.GetTop() + 3)), &brush);
			}
#endif
		}
	}
}



//TBox* TimeTable::GetBox(CString scheKey)
//{
//	TBox* tbx = _mapBoxes[scheKey];
//	return tbx;
//}

TBox* TimeTable::IsCursorInBox(CPoint point, CStringA& side)
{
	TBox* tbox = nullptr;
	bool bSel = false;
	JObj* jocc = nullptr;
	for(auto& [sch, box] : _mapBoxes)
	{
		PAS pside = KwIsInRectSide(box->_rc, point, 3);
		if(pside)
		{
			side = pside;
			if(side == "top" || side == "bottom")
			{
				tbox = box;
				bSel = true;
				//jocc = (*box)._jocc.get();//shared의 shared
				break;
			}
		}
		else
		{
			if(KwIsInRect(box->_rc, point)) // side가 아니고 가운데
			{
				tbox = box;
				bSel = true;
				side = "center";
				//jocc = (*box)._jocc.get();//shared의 shared
				break;
			}
		}
	}
	return tbox;// bSel;
}

/// <summary>
/// 
/// </summary>
/// <param name="scPoint">스크롤만 감안한 거지 cell의 offset은 고려 안했다.</param>
/// <param name="nFlags"></param>
/// <returns></returns>
bool TimeTable::ButtonDown(CPoint scPoint, UINT nFlags)
{
	CStringA side;
	TBox* box = IsCursorInBox(scPoint, side);
	if(box)
	{
		box->_hotspot = CPoint(scPoint.x - box->_rc.left, scPoint.y - box->_rc.top);//박스안에서 눌러진 위치.
			//TBox* box = _tt.IsCursorInBox(spt, side);
		if(nFlags & MK_LBUTTON)// left button 일떄만 작동
			_curBox = box;//RButton 때도 _cur가 들어 가네
		else
			_curBox = nullptr;

		if(_selBox == nullptr || box != _selBox)
			_selBox = box;// nullptr; //현재 선택 된게 있는데 다르면 리셋. UP 할때 다시 정해 진다.
		box->_side = side;
		box->_rcStatic = box->_rc;//움직이기 전 위치
		//TRACE("ButtonDown(%d,%d)\n", box->_rc.left, box->_rc.top);
		//auto jocc = (*box)._jocc;//shared의 shared
		return true;
	}
	else
	{
		CRect mrc = getMainArea();
		CSize szCl = getSzCell();
		CPoint mpt(scPoint.x - mrc.left, scPoint.y - mrc.top);
		/// 어느 셀에 클릭 했는지 계산 해 둔다. 우클릭 메뉴때 쓰게
		_curColRow = CPoint(mpt.x / szCl.cx, mpt.y / szCl.cy);
		// 범위안으로
		//KwSetMinMax(_curColRow.x, 0L, _nCols - 1L);
		//KwSetMinMax(_curColRow.y, 0L, _nRows - 1L);
		//if(_curColRow.x >= _nCols)
		//	_curColRow.x = _nCols - 1;
		//if(_curColRow.x < 0)
		//	_curColRow.x = 0;
		//if(_curColRow.y >= _nRows)
		//	_curColRow.y = _nRows - 1;
		//if(_curColRow.y < 0)
		//	_curColRow.y = 0;
	}
	return false;
}
//_selSchIdBox = std::make_tuple(key, side); //https://aossuper8.tistory.com/92


void TimeTable::DoLButtonUp(UINT nFlags, CPoint point, 
	shared_ptr<function<void()>> onSel, shared_ptr<function<void()>> onUpdate)
{
	TRACE("%s \n", __FUNCTION__);
	//auto* tview = (CSmpView*)_wnd;
	auto& _tt = *this;
	if(_tt._curBox)
	{
		///?todo 임시. 원래는 적당히 정착할 위치 찾아 들어간다.
		//_tt._curBox->_rc = _tt._curBox->_rcStatic;
			//꼭지로 해볼까?
		int scx = _wnd->GetScrollPos(SB_HORZ);
		int scy = _wnd->GetScrollPos(SB_VERT);
		CPoint spt(point.x + scx, point.y + scy);
		CRect ptm = _tt.getMainArea();
		//메인 화면 넘어가면 안으로 넣는다.
		CSize szc = _tt.getSzCell();
		CSize szhf(szc.cx / 2, szc.cy / 2);//셀 중앙

		CPoint lst(_tt._nCols - 1, _tt._nRows - 1);
		CPoint col(-1, -1);//몇번째 칸
		auto* selBoxBefore = _tt._selBox;
		TBox& tx = *_tt._curBox;
		JObj& jocc = *tx._jocc;
		CKTime fOffBegin = jocc.T("fOffBegin");//일정의 시작 시각
		CKTime fOffEnd = jocc.T("fOffEnd");//일정의 끝 시가
		CKTimeSpan sp = fOffEnd - fOffBegin; // 일단 시간 간격을 백업해 두고
		if(tx._side == "center")
		{
			CPoint mpt(spt.x - ptm.left, spt.y - ptm.top);// 메인 영역을 0,0 으로 환산
			// 1. 스크롤 만큼 제거. 2. 메인영역제거. 3. 핫스폿제거 => 메인 영역 꼭지점으로 유도.
			CPoint lt(spt.x - ptm.left - tx._hotspot.x, spt.y - ptm.top - tx._hotspot.y);

			col = CPoint(lt.x / szc.cx, lt.y / szc.cy);//몇번째 칸
			CPoint dst(lt.x % szc.cx, lt.y % szc.cy);// 절반 넘었나 안넘었나
			auto dr = (int)(sp.GetTotalMinutes() / _tt._minute);// 간격을 셀 분 단위로
			/// col, row를 범위내로
			if(col.x < 0)
				col.x = 0;
			else if(lst.x < col.x)
				col.x = lst.x;
			if(col.y < 0)
				col.y = 0;
			else if(lst.y + 1 < col.y + dr)
				col.y = lst.y + 1 - dr;

			CSize hfav((dst.x < szhf.cx || col.x == lst.x) ? 0 : 1,
				(dst.y < szhf.cy || col.y == lst.y) ? 0 : 1);//다음칸에 더 가깝나 ?
	//몇번째 칸인지 발견
			CPoint col1(col.x + hfav.cx, col.y + hfav.cy);
			int c = col1.x;
			int r = col1.y;
			/// 컬럼: 가상담당정보 vchrg
			/// 이제 바뀐 위치에 따라 TBox의 정보를 바꾼다. 먼저 _rc 좌표
			CString fVChrgID = jocc.S("fVChrgID");
			CString fMode = jocc.S("fMode");

			bool bCmn = fVChrgID.GetLength() == 0;//담당이 없는 일정은 공통으로 항상 위치는 모든 컬럼 즉 0 이다.
			if(bCmn)//공통일정이면
				c = 0;
			///백업 해둔다. 서버에러 나면 원상복귀 해야 하니
			if(!tx._joccBefore)
				tx._joccBefore = make_shared<JObj>();//shared_ptr<JObj>(new JObj);
			tx._joccBefore->Clone(jocc, true);

			auto shcol = _tt._col[c];//컬럼의 담당 객체
			//JObj& vco = *shcol;
			tx._jsch = shcol;///컬럼/담당이 바뀐다.
			//CString fVChrgID_new = shcol->S("fVChrgID");
			//CString fUsrID_new = shcol->S("fUsrID");
			///tx._jocc = shjx; 일정: 이건 움직이는 중이니 불변
			///?todo: 여기서 fVChrgID , fOffBegin의 변경 request를 해야 한다. 
			///		UpdateSchedule (서버에서 fMode를 보고 order 인지 schedule인지 구분한다.
			bool bModified = false;
			if(tx._colL != c)
			{
				if(fMode == L"order")
				{
					if(!bCmn)
					{
						jocc.Copy(*shcol, "fVChrgID");
						jocc.Copy(*shcol, "fUsrID");
						//jocc("fVChrgID") = fVChrgID_new; ///일단 변경
						//jocc("fUsrID") = fUsrID_new; ///일단 변경
					}
					tx._colL = c;//컬럼 left DoLButtonUp
					tx._colR = bCmn ? lst.x : c + 1;/// 우변은 공통일정 여부에 따라 1칸이냐 전체냐?
					bModified = true;
				}///else 개인일정은 컬럼을 못바꾼다.
			}

			CRect rcBu = tx._rc;//일단 백업 받고
			if(tx._rowT != r)
			{
				bModified = true;
		
				tx._rowT = r;  /// 센터로 움직일때는 위아래 같이 움직인다.
				tx._rowB = (int)(r + dr);
				// 새 시작 시각은? r
				CKTimeSpan spm(0, 0, _tt._minute * r, 0);
				CKTime night(fOffBegin.GetYear(), fOffBegin.GetMonth(), fOffBegin.GetDay(), 0, 0, 0);
				CKTime newBegin = night + spm;
				jocc("fOffBegin") = newBegin;
				jocc("fOffEnd") = newBegin + sp;
			}


			tx._rc = CRect(CPoint(_tt.getRcHeaderHorz(tx._colL).left, tx._rc.top = _tt.getRcHeaderVert(tx._rowT).top), rcBu.Size());
			//TRACE("(%d,%d) (%d,%d) (%d,%d,%d,%d)\n", tx._colL, tx._rowT, tx._colR, tx._rowB, tx._rc.left, tx._rc.top, tx._rc.Width(), tx._rc.Height());

			/// tbox의 vchrg(가상담당)가 널이면 공통이니 새 col은 항상 0 이다.
			///		널이 아니면 새col의 vchrg 로 담당자가 변경 된다.
			/// box가 길어도(시간 30분이상) htspot 역추적 했으므로 놓은 자리가 시작 시간이 된며 할당시간으로 끝시각을 계산 한다.
			//TRACE("OnLButtonUp(%d,%d)\n", tx._rc.left, tx._rc.top);


			if(bModified)
				(*onUpdate)();/// 담당자가 바뀌거나 시작 시간이 바뀐 경우
			
			if(!selBoxBefore || selBoxBefore != _tt._selBox)
				(*onSel)();/// 상세창 내용이 바뀐다._fncOnSelectedSchedule
			
			_tt.FinishDrag();

			_wnd->InvalidateRect(_tt._rcc, 0);
		}//center
		else if(tx._side == "top" || tx._side == "bottom")
		{
			CPoint lt(spt.x - ptm.left, spt.y - ptm.top);
			CPoint col(lt.x / szc.cx, lt.y / szc.cy);//몇번째 칸
			CPoint dst(lt.x % szc.cx, lt.y % szc.cy);// 절반 넘었나 안넘었나
			CPoint lst(_tt._nCols - 1, _tt._nRows - 1);
			if(col.y < 0)
				col.y = 0;
			else if(_tt._nRows < col.y)
				col.y = _tt._nRows;
			CSize hfav((dst.x < szhf.cx || col.x == lst.x) ? 0 : 1,
				(dst.y < szhf.cy || col.y == lst.y) ? 0 : 1);//다음칸에 더 가깝나 ?
			CPoint col1(col.x + hfav.cx, col.y + hfav.cy);
			int r = col1.y;
			
			CRect rcBu = tx._rc;
			int rowT = tx._rowT;
			int rowB = tx._rowB;

			if(tx._side == "top")
			{
				if(r < 0)
					r = 0;
				tx._rc = CRect(tx._rc.left, tx._rc.top = _tt.getRcHeaderVert(r).top, tx._rc.right, tx._rc.bottom);
				tx._rowT = r;/// 위만 키운다.
			}
			else if(tx._side == "bottom")
			{
				if((lst.y+1) < r)
					r = lst.y+1;
				tx._rc = CRect(tx._rc.left, tx._rc.top, tx._rc.right, tx._rc.bottom = _tt.getRcHeaderVert(r).top);
				tx._rowB = r;/// 아래만 키운다.
			}

			//CString sBegin, sEnd;
			//KwCTimeToString(fOffBegin, sBegin);
			//KwCTimeToString(fOffEnd, sEnd);

			//// 새 시작 시각은? r
			//CKTimeSpan spt(0, 0, _tt._minute* tx._rowT, 0);
			//CKTimeSpan spb(0, 0, _tt._minute* tx._rowB, 0);
			//CKTime night(fOffBegin.GetYear(), fOffBegin.GetMonth(), fOffBegin.GetDay(), 0, 0, 0);
			//CKTime newBegin = night + spt;
			//CKTime newEnd = night + spb;

			CKTime tb = RowToTime(tx._rowT);
			CKTime te = RowToTime(tx._rowB);
			jocc("fOffBegin") = tb;
			jocc("fOffEnd") = te;


			_tt.FinishDrag();///_selBox 가 이제 정해 진다.

			_wnd->InvalidateRect(_tt._rcc, 0);

			if(rowT != tx._rowT || rowB != tx._rowB)
				(*onUpdate)();/// 시간이나 담당자가 바뀌었다.

			if(!selBoxBefore || selBoxBefore != _tt._selBox)
				(*onSel)();/// 선택한 일정이 바뀌어 상세창 내용이 바뀐다.
			TRACE("(%d,%d) (%d,%d) (%d,%d,%d,%d)\n", tx._colL, tx._rowT, tx._colR, tx._rowB, tx._rc.left, tx._rc.top, tx._rc.Width(), tx._rc.Height());
		}// top, bottom
		//TRACE("%s (%d, %d) (%d, %d)\n", tx._side, tx._colL, tx._colR, tx._rowT, tx._rowB);
	}//if(_tt._curBox)

}

void TimeTable::MouseMove(UINT nFlags, CPoint point)
{
	// box의 top이나 bottom 영역안에서 1초간 머물면 
	// 커서가 크기조절 아이콘으로 바뀌고 거기서 누르면 30분 단위 영역으로 안에서 
	// 박스 크기 조절 되며, 버튼 클릭을 릴리즈 하고 3초 지사면 서버에 일정 변경이 바로 들어 간다.

	// 박스 안에서 가운데즘 영역을 클릭 움직임 모드로 손모양 아이콘으로 바뀌며 다른 영역으로 이동할 수 있다.
	// 이동후 3초 멈추면 일정 변경 요청 한다.
	auto& _tt = *this;
	CPoint scpos(point);
	KwGetViewScrolledPos((CScrollView*)_wnd, scpos);

	CSize szc = _tt.getSzCell();

	HCURSOR hCurs1 = nullptr;
	CStringA side;
	if(!_tt._curBox)
	{
		TBox* box = _tt.IsCursorInBox(scpos, side);
		if(side == "center")
			hCurs1 = LoadCursor(NULL, IDC_HAND);
		else if(side == "top")
			hCurs1 = LoadCursor(NULL, IDC_SIZENS);
		else if(side == "bottom")
			hCurs1 = LoadCursor(NULL, IDC_SIZENS);

		if(hCurs1)
			SetCursor(hCurs1);
	}
	else if(nFlags & MK_LBUTTON)// left button 일떄만 작동
	{
		//_tt._curBox = box;
		TBox* box = _tt._curBox;// _tt.GetBox(scheID);

		JObj& jocc = *box->_jocc;//shared의 shared
		//auto keyShed = _tt.MakeBoxKey(*box->_jocc);//
		CString fVChrgID = jocc.S("fVChrgID");
		bool bCmnSched = fVChrgID.IsEmpty();//공통 일정이면 fVChrgID == null 넓게 펼쳐져 있다.
		if(box->_side == "center")
		{
			hCurs1 = LoadCursor(NULL, IDC_HAND);
			SetCursor(hCurs1);
			//box->_rc.left = spt.x - box->_hotspot.x;
			//box->_rc.top = spt.y - box->_hotspot.y;
			box->_rc = CRect(CPoint(scpos.x - box->_hotspot.x, scpos.y - box->_hotspot.y), box->_rcStatic.Size());
		}
		else if(box->_side == "top")
		{
			//CRect rcT = KwGetRectSideTop(box->_rc);
			hCurs1 = LoadCursor(NULL, IDC_SIZENS);
			SetCursor(hCurs1);
			if(box->_rc.bottom - szc.cy < scpos.y)
				box->_rc.top = box->_rc.bottom - szc.cy;
			else
				box->_rc.top = scpos.y;
			//box->_rc = CRect(CPoint(spt.x - box->_hotspot.x, spt.y - box->_hotspot.y), box->_rcStatic.Size());
		}
		else if(box->_side == "bottom")
		{
			//CRect rcT = KwGetRectSideTop(box->_rc);
			hCurs1 = LoadCursor(NULL, IDC_SIZENS);
			SetCursor(hCurs1);
			if(box->_rc.top + szc.cy > scpos.y)
				box->_rc.bottom = box->_rc.top + szc.cy;
			else
				box->_rc.bottom = scpos.y;
		}
		CRect rcInv = _tt._rcc;
		rcInv.top += 30;//30은 전체 뷰중 위 30만큼 안번쩍이게 다시 안그려 볼려고
		_wnd->InvalidateRect(rcInv, 0);

		PAS pside = KwIsInRectSide(box->_rc, point);
	}
}

void TimeTable::FinishDrag()
{
	if(_curBox)
		_selBox = _curBox;///움직이던 cur는 reset하고 최종적으로 선택된 sel은 이제 바꾼다.
	else
		_selBox = nullptr;
	_curBox = nullptr;
}

/*
int TimeTable::MakeBoxKey(JObj& jx)
{
	CString keyShed;
	//CString fSchedOrgID(jx.S("fSchedOrgID"));
	//CString sOffBegin(jx.S("fOffBegin"));
	//sOffBegin = sOffBegin.Left(16);
	//keyShed.Format(L"%s%s", (PWS)fSchedOrgID, (PWS)sOffBegin);
	keyShed = jx.S("fIDX");// fSchedOrgID, (PWS)sOffBegin);
	return keyShed;
}
*/
void TimeTable::Reset()
{
	_selBox = nullptr;
	_curBox = nullptr;
	_jtbl.reset();
	_col.clear();
	_boxes.DeleteAll();
	_mapBoxes.DeleteAll();

	_mapBr.DeleteAll();
	_mapCellMode.DeleteAll();
}

/// request 에러 난 경우 이전 위치로 복구 한다.
void TimeTable::RecoverUpdate()
{
	if(_selBox)
	{
		_selBox->_jocc = _selBox->_joccBefore;
		_selBox->_rc = _selBox->_rcStatic;
	}
}


void TimeTable::DeleteSelBox()// _selBox
{
	if(!_selBox)
		return;
	//ASSERT(_curBox == nullptr);
	auto key = _selBox->_jocc->I("fIDX");
	//CString key = MakeBoxKey(*_selBox->_jocc);
	_mapBoxes.DeleteKey(key);//if(ptx == nullptr)//fTime == fOffBegin)
	_selBox = nullptr;
	_curBox = nullptr;
}

void TimeTable::AddOrder(int col, int row)
{
	ShJObj vch = _col[col];
	TBox* ptx = new TBox;
	TBox& tx = *ptx;
	tx._jsch = vch;
	tx._jocc = make_shared<JObj>();
	auto& jocc = *tx._jocc;

	tx._colL = col;
	tx._colR = col + 1;
	tx._rowT = row;
	tx._rowB = row + 1;

	tx._rc = getRcCell(tx._colL, tx._rowT);  /// 생성시는 하나의 셀이 추가 되므로 계산할거 없이 셀 하나크리고 가져 온다.
	//이게 아래에서 _secBox 에 대입된다.

	CKTime tb = RowToTime(tx._rowT);
	CKTime te = RowToTime(tx._rowB);/// 30분 짜리가 기본으로 만들어 진다.
	auto newIDX = _maxIdx++;
	jocc("fIDX") = newIDX;
	/// 다음 5가지는 이건 DB서버 로직상 예약은 이값이다.
	jocc("fOrder") = 1140;
	jocc("fSrcDesc") = L"예약";
	jocc("fOff") = L"off";
	jocc("fMode") = L"order";
	jocc("fRepeat") = L"once";

#ifdef DEBUG
	CString uuid = KwGetFormattedGuid(true, L"order", 8);
#else
	CString uuid = KwGetFormattedGuid();
#endif
	jocc("fSchedOrgID") = uuid;
	jocc.Copy(*vch, "fVChrgID");
	jocc("fOffBegin") = tb;
	jocc("fOffEnd") = te;
	jocc("fOrderID") = ONULL();

	_mapBoxes[newIDX] = ptx;
	_requestedIDX[(PWS)uuid] = newIDX;
	
	_selBox = ptx; ///방금 만든게 선택된다.

	/// 서버에 추가 후 실패 하면 newIDX 로 _mapBoxes 에서 모두 제거 해야 화면에서 없어진다. 애쓴게 없으니 지워도 된다.
	/// 성공 하면 받은 fSchedOrgID 로 _requestedIDX 에서만 제거 한다.
}

void TimeTable::AfterAddOrder(bool bOK, PWS fSchedOrgID)// _selBox
{
	if(!bOK)
	{
		auto newIDX = _requestedIDX[fSchedOrgID];
		_mapBoxes.DeleteKey(newIDX);
		_selBox = nullptr;
	}
	_requestedIDX.erase(fSchedOrgID);
}


void TimeTable::AddSchedule(int col, int row)
{
	ShJObj vch = _col[col];
	TBox* ptx = new TBox;
	TBox& tx = *ptx;
	tx._jsch = vch;
	tx._jocc = make_shared<JObj>();
	auto& jocc = *tx._jocc;

	tx._colL = col;
	tx._colR = col + 1;
	tx._rowT = row;
	tx._rowB = row + 1;

	tx._rc = getRcCell(tx._colL, tx._rowT);  /// 생성시는 하나의 셀이 추가 되므로 계산할거 없이 셀 하나크리고 가져 온다.
	//이게 아래에서 _secBox 에 대입된다.

	CKTime tb = RowToTime(tx._rowT);
	CKTime te = RowToTime(tx._rowB);/// 30분 짜리가 기본으로 만들어 진다.
	auto newIDX = _maxIdx++;
	jocc("fIDX") = newIDX;
	/// 다음 5가지는 이건 DB서버 로직상 예약은 이값이다.
	jocc("fOrder") = 1130;
	jocc("fSrcDesc") = L"일정";
	jocc("fOff") = L"off";
	jocc("fMode") = L"schedule";
	jocc("fAttr") = L"private";
	jocc("fRepeat") = L"once";

#ifdef DEBUG
	CString uuid = KwGetFormattedGuid(true, L"schedule", 8);
#else
	CString uuid = KwGetFormattedGuid();
#endif
	jocc("fSchedOrgID") = uuid;
	//jocc.Copy(*vch, "fVChrgID");
	jocc.Copy(*vch, "fUsrID");
//	jocc.Copy(*vch, "fBizID");
	jocc("fOffBegin") = tb;
	jocc("fOffEnd") = te;

	_mapBoxes[newIDX] = ptx;
	_requestedIDX[(PWS)uuid] = newIDX;

	_selBox = ptx; ///방금 만든게 선택된다.

	/// 서버에 추가 후 실패 하면 newIDX 로 _mapBoxes 에서 모두 제거 해야 화면에서 없어진다. 애쓴게 없으니 지워도 된다.
	/// 성공 하면 받은 fSchedOrgID 로 _requestedIDX 에서만 제거 한다.
}
