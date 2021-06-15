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
	AUTOLOCK(_cs_jtbl); // _jtbl �� ��Ƽ�� ���� �ϹǷ� ���ؾ�.
	_mapCellMode.clear();
	_col.clear();
	_nRows = 48;
	_nCols = 4;
	//CJsonPbj* _pjtbl{ nullptr };
	//CKCriticalSection* _pcs_jtbl{ nullptr };
	//KPtrArray<CJsonPbj> _col;// �ȿ� fOrderSched�� ���� �迭�� �����.
	//CJsonPbj& _jtbl = *_pjtbl;
	JObj& jtbl = *_jtbl.get();
	// { vc1: { :,:,:,}, vc2: { :,:,:, }, 4}
	JObj& jchg = *jtbl.O("charge");
	// { vc1: [ {:,:,:,},{:,:,:,},{:,:,:,},,,,48], vc1: [ {:,:,:,},{:,:,:,},{:,:,:,},,,,48],,, 4}
	JObj& joccs = *jtbl.O("occupy");
	
	KMulMap<double, ShJObj> mpCol;
	
	for(auto& [vc, vcObj] : jchg)//���� �����ʵ� �ӽ� map�� ����� �迭�� �ִ´�.
	{
		auto jobj = vcObj->AsObject();
		JObj& vco = *jobj;
		auto dord = vco.D("fOrderSched");
		//while(mpCol.Has(dord))
		//	dord += 0.001;//�Ǽ��� fOrderSched�� ���� ���� ��Զ� ����
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
	}//�÷� ������ �迭�� �����. �� �ƴ�


	int minu = jtbl.I("fMinuteForCut30");// 30; // work unit minute : 30�� ������ �ϴ����� ����.
	int m48 = jtbl.I("fNumberOfTimeCut48");// 24 * (60 / minu); // �迭 ���� 48��
	CKTimeSpan sp30(0, 0, minu, 0);
	_nRows = m48;
	_nCols = (int)jchg.size();
	_minute = minu;
	_maxIdx = -1;
	// ���� _col[n] �� n��° �÷� ���� �̴�.
	/*
	"fMode":"closed",
	"fNote":"������ �ð�",
	"fOff":1,
	"fOffBegin":"2021-02-09 00:00:00",
	"fOffEnd":"2021-02-09 10:00:00",
	"fRepeat":"once",
	"fSchedOrgID":"biz-0002",
	"fTime":"2021-02-09 03:30:00"
	*/

	for(int c = 0; c < _col.size(); c++)// auto & jbx : _col) // cols ���� cell�� �ƴϴ�
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

			/// fPause ����
			///		holiday, closed, open, work 4������ �ִ�. 
			///		�ݴ°��� ������,�����ð��� �ְ�, Ư���� �ݴ� ���� �׳� �ڽ��������� �Ѵ�.
			///		fPause �� ��� c,r�� �ִ�.
			if(jx.Has("fPause"))
				AddCellMode(c, r, jx.S("fPause"));

			auto fOff = jx.I("fOff");//�������� ������ ������ 1
			if(!jx.Has("fIDX"))
			{
				//ASSERT(fOff == 0);//���� ���� �� �� �ð� ����
				continue;
			}

			auto fIDX = jx.I("fIDX");
			if(_maxIdx < fIDX)// add�Ҷ� �ӽ� unique ���� �̹Ƿ� �׳� �ְ����� ���صд�.
				_maxIdx = fIDX;
			CString fSchedOrgID(jx.S("fSchedOrgID"));
			if(fSchedOrgID == L"schedule-18B6BF54")
				_break;
			CString fVChrgID = jx.S("fVChrgID");//�� ������ �Ҵ�� �����
			CKTime fOffBegin = jx.T("fOffBegin");//������ ���� �ð�
			CKTime fOffEnd = jx.T("fOffEnd");//������ �� �ð�
			CKTime fTime = jx.T("fTime");//���� �ð�
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
			//else if((fMode == L"closed" || fMode == L"schedule") && fPause == 0)//fOff == 0) // Ư�� 
			//{
			//	if(fVChrgID.GetLength() > 0)//Ư�����
			//	{
			//		AddCellMode(c, r, L"work");
			//		continue;
			//	}
			//	else//Ư������
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
					if(fVChrgID.GetLength() > 0)//��������
					{
						bStart = true;
					}
				}

				if(bStart)
				{
					TBox* ptx = new TBox;
					TBox& tx = *ptx;
					tx._jsch = shcol;//�÷�/���
					tx._jocc = shjx;//�÷�/�迭/����
					CKTimeSpan sp = fOffEnd - fOffBegin; // �ϴ� �ð� ������ ����� �ΰ�
					int dr = (int)sp.GetTotalMinutes() / _minute;// ������ �� �� ������
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

			/// ��� �ϴ� ��� �ð��� ���ð� ��¥�� �޶� �������� ������ ������ ��� �Ѵ�.
			CKTime tdy(fTime.GetYear(), fTime.GetMonth(), fTime.GetDay(), 0, 0, 0);
			CKTime tdyNext = tdy + CKTimeSpan(1, 0, 0, 0);
			CKTime tdEnd;
			if(fOffEnd.GetHour() == 0 && fOffEnd.GetMinute() == 0 && fOffEnd.GetSecond() == 0)//������ �ð��� 00:00 ������ 00:00�̴�.
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
				if(fVChrgID.GetLength() > 0)///��������. ������ tbizvirtual�� ����� �ȵ� ����� ������ �ڽ��� �ȱ׷� ����.
				{
					bFinish = true;
				}
				else//���� ����
				{
					if(c == (_col.size() - 1))
						bFinish = true;
				}

				if(bFinish)
				{
					TBox* ptx{ nullptr };
					ptx = _mapBoxes[fIDX];
					auto& txb = *ptx;
					ptx->_rowB = r + 1;//������ ĭ�� bottom��  �Ʒ��� ĭ�̴�.
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
	/// ���� _boxes dp ������ ��Ÿ���� TBox�� �����.
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

	FontFamily myfontFamily(L"����");// Century");
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
	///  �Ǿ��Ƿ� ������
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
	CString fVChrgID = jocc.S("fVChrgID");//�� ������ �Ҵ�� �����
	CString fSchedOrgID = jocc.S("fSchedOrgID");//�� ������ �Ҵ�� �����

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
		txt.Format(L"����:%d", fIDX);
	}
	else if(fMode == L"schedule")
	{
		int cut = 6;
		if(fVChrgID.GetLength() == 0)
		{
			txt = fNote;// L"�� ����"; ���ٿ� ǥ��
			cut *= _nCols; // �÷� ���� ���� ���� ���� ���÷��� 4�ڷ� ���Ѵ�.
		}
		else
			txt.Format(L"����:%d", fIDX);

		for(int i = 0; i < fNote.GetLength(); i += cut)
			artxt.Add(fNote.Mid(i, cut));
	}
	else
		txt = fNote;
	if(rc1.Width > 0.f && rc1.Height > 0.f)
	{
		FontFamily myfontFamily(L"����");// Century");FontStyleBold
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
		int nline = box->_rowB - box->_rowT - 1;//�̹� �ϳ� ������ -1
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
	//Pen pen(&brLine); �귯�ø� ������ ����
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
				//jocc = (*box)._jocc.get();//shared�� shared
				break;
			}
		}
		else
		{
			if(KwIsInRect(box->_rc, point)) // side�� �ƴϰ� ���
			{
				tbox = box;
				bSel = true;
				side = "center";
				//jocc = (*box)._jocc.get();//shared�� shared
				break;
			}
		}
	}
	return tbox;// bSel;
}

/// <summary>
/// 
/// </summary>
/// <param name="scPoint">��ũ�Ѹ� ������ ���� cell�� offset�� ��� ���ߴ�.</param>
/// <param name="nFlags"></param>
/// <returns></returns>
bool TimeTable::ButtonDown(CPoint scPoint, UINT nFlags)
{
	CStringA side;
	TBox* box = IsCursorInBox(scPoint, side);
	if(box)
	{
		box->_hotspot = CPoint(scPoint.x - box->_rc.left, scPoint.y - box->_rc.top);//�ڽ��ȿ��� ������ ��ġ.
			//TBox* box = _tt.IsCursorInBox(spt, side);
		if(nFlags & MK_LBUTTON)// left button �ϋ��� �۵�
			_curBox = box;//RButton ���� _cur�� ��� ����
		else
			_curBox = nullptr;

		if(_selBox == nullptr || box != _selBox)
			_selBox = box;// nullptr; //���� ���� �Ȱ� �ִµ� �ٸ��� ����. UP �Ҷ� �ٽ� ���� ����.
		box->_side = side;
		box->_rcStatic = box->_rc;//�����̱� �� ��ġ
		//TRACE("ButtonDown(%d,%d)\n", box->_rc.left, box->_rc.top);
		//auto jocc = (*box)._jocc;//shared�� shared
		return true;
	}
	else
	{
		CRect mrc = getMainArea();
		CSize szCl = getSzCell();
		CPoint mpt(scPoint.x - mrc.left, scPoint.y - mrc.top);
		/// ��� ���� Ŭ�� �ߴ��� ��� �� �д�. ��Ŭ�� �޴��� ����
		_curColRow = CPoint(mpt.x / szCl.cx, mpt.y / szCl.cy);
		// ����������
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
		///?todo �ӽ�. ������ ������ ������ ��ġ ã�� ����.
		//_tt._curBox->_rc = _tt._curBox->_rcStatic;
			//������ �غ���?
		int scx = _wnd->GetScrollPos(SB_HORZ);
		int scy = _wnd->GetScrollPos(SB_VERT);
		CPoint spt(point.x + scx, point.y + scy);
		CRect ptm = _tt.getMainArea();
		//���� ȭ�� �Ѿ�� ������ �ִ´�.
		CSize szc = _tt.getSzCell();
		CSize szhf(szc.cx / 2, szc.cy / 2);//�� �߾�

		CPoint lst(_tt._nCols - 1, _tt._nRows - 1);
		CPoint col(-1, -1);//���° ĭ
		auto* selBoxBefore = _tt._selBox;
		TBox& tx = *_tt._curBox;
		JObj& jocc = *tx._jocc;
		CKTime fOffBegin = jocc.T("fOffBegin");//������ ���� �ð�
		CKTime fOffEnd = jocc.T("fOffEnd");//������ �� �ð�
		CKTimeSpan sp = fOffEnd - fOffBegin; // �ϴ� �ð� ������ ����� �ΰ�
		if(tx._side == "center")
		{
			CPoint mpt(spt.x - ptm.left, spt.y - ptm.top);// ���� ������ 0,0 ���� ȯ��
			// 1. ��ũ�� ��ŭ ����. 2. ���ο�������. 3. �ֽ������� => ���� ���� ���������� ����.
			CPoint lt(spt.x - ptm.left - tx._hotspot.x, spt.y - ptm.top - tx._hotspot.y);

			col = CPoint(lt.x / szc.cx, lt.y / szc.cy);//���° ĭ
			CPoint dst(lt.x % szc.cx, lt.y % szc.cy);// ���� �Ѿ��� �ȳѾ���
			auto dr = (int)(sp.GetTotalMinutes() / _tt._minute);// ������ �� �� ������
			/// col, row�� ��������
			if(col.x < 0)
				col.x = 0;
			else if(lst.x < col.x)
				col.x = lst.x;
			if(col.y < 0)
				col.y = 0;
			else if(lst.y + 1 < col.y + dr)
				col.y = lst.y + 1 - dr;

			CSize hfav((dst.x < szhf.cx || col.x == lst.x) ? 0 : 1,
				(dst.y < szhf.cy || col.y == lst.y) ? 0 : 1);//����ĭ�� �� ������ ?
	//���° ĭ���� �߰�
			CPoint col1(col.x + hfav.cx, col.y + hfav.cy);
			int c = col1.x;
			int r = col1.y;
			/// �÷�: ���������� vchrg
			/// ���� �ٲ� ��ġ�� ���� TBox�� ������ �ٲ۴�. ���� _rc ��ǥ
			CString fVChrgID = jocc.S("fVChrgID");
			CString fMode = jocc.S("fMode");

			bool bCmn = fVChrgID.GetLength() == 0;//����� ���� ������ �������� �׻� ��ġ�� ��� �÷� �� 0 �̴�.
			if(bCmn)//���������̸�
				c = 0;
			///��� �صд�. �������� ���� ���󺹱� �ؾ� �ϴ�
			if(!tx._joccBefore)
				tx._joccBefore = make_shared<JObj>();//shared_ptr<JObj>(new JObj);
			tx._joccBefore->Clone(jocc, true);

			auto shcol = _tt._col[c];//�÷��� ��� ��ü
			//JObj& vco = *shcol;
			tx._jsch = shcol;///�÷�/����� �ٲ��.
			//CString fVChrgID_new = shcol->S("fVChrgID");
			//CString fUsrID_new = shcol->S("fUsrID");
			///tx._jocc = shjx; ����: �̰� �����̴� ���̴� �Һ�
			///?todo: ���⼭ fVChrgID , fOffBegin�� ���� request�� �ؾ� �Ѵ�. 
			///		UpdateSchedule (�������� fMode�� ���� order ���� schedule���� �����Ѵ�.
			bool bModified = false;
			if(tx._colL != c)
			{
				if(fMode == L"order")
				{
					if(!bCmn)
					{
						jocc.Copy(*shcol, "fVChrgID");
						jocc.Copy(*shcol, "fUsrID");
						//jocc("fVChrgID") = fVChrgID_new; ///�ϴ� ����
						//jocc("fUsrID") = fUsrID_new; ///�ϴ� ����
					}
					tx._colL = c;//�÷� left DoLButtonUp
					tx._colR = bCmn ? lst.x : c + 1;/// �캯�� �������� ���ο� ���� 1ĭ�̳� ��ü��?
					bModified = true;
				}///else ���������� �÷��� ���ٲ۴�.
			}

			CRect rcBu = tx._rc;//�ϴ� ��� �ް�
			if(tx._rowT != r)
			{
				bModified = true;
		
				tx._rowT = r;  /// ���ͷ� �����϶��� ���Ʒ� ���� �����δ�.
				tx._rowB = (int)(r + dr);
				// �� ���� �ð���? r
				CKTimeSpan spm(0, 0, _tt._minute * r, 0);
				CKTime night(fOffBegin.GetYear(), fOffBegin.GetMonth(), fOffBegin.GetDay(), 0, 0, 0);
				CKTime newBegin = night + spm;
				jocc("fOffBegin") = newBegin;
				jocc("fOffEnd") = newBegin + sp;
			}


			tx._rc = CRect(CPoint(_tt.getRcHeaderHorz(tx._colL).left, tx._rc.top = _tt.getRcHeaderVert(tx._rowT).top), rcBu.Size());
			//TRACE("(%d,%d) (%d,%d) (%d,%d,%d,%d)\n", tx._colL, tx._rowT, tx._colR, tx._rowB, tx._rc.left, tx._rc.top, tx._rc.Width(), tx._rc.Height());

			/// tbox�� vchrg(������)�� ���̸� �����̴� �� col�� �׻� 0 �̴�.
			///		���� �ƴϸ� ��col�� vchrg �� ����ڰ� ���� �ȴ�.
			/// box�� ��(�ð� 30���̻�) htspot ������ �����Ƿ� ���� �ڸ��� ���� �ð��� �ȸ� �Ҵ�ð����� ���ð��� ��� �Ѵ�.
			//TRACE("OnLButtonUp(%d,%d)\n", tx._rc.left, tx._rc.top);


			if(bModified)
				(*onUpdate)();/// ����ڰ� �ٲ�ų� ���� �ð��� �ٲ� ���
			
			if(!selBoxBefore || selBoxBefore != _tt._selBox)
				(*onSel)();/// ��â ������ �ٲ��._fncOnSelectedSchedule
			
			_tt.FinishDrag();

			_wnd->InvalidateRect(_tt._rcc, 0);
		}//center
		else if(tx._side == "top" || tx._side == "bottom")
		{
			CPoint lt(spt.x - ptm.left, spt.y - ptm.top);
			CPoint col(lt.x / szc.cx, lt.y / szc.cy);//���° ĭ
			CPoint dst(lt.x % szc.cx, lt.y % szc.cy);// ���� �Ѿ��� �ȳѾ���
			CPoint lst(_tt._nCols - 1, _tt._nRows - 1);
			if(col.y < 0)
				col.y = 0;
			else if(_tt._nRows < col.y)
				col.y = _tt._nRows;
			CSize hfav((dst.x < szhf.cx || col.x == lst.x) ? 0 : 1,
				(dst.y < szhf.cy || col.y == lst.y) ? 0 : 1);//����ĭ�� �� ������ ?
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
				tx._rowT = r;/// ���� Ű���.
			}
			else if(tx._side == "bottom")
			{
				if((lst.y+1) < r)
					r = lst.y+1;
				tx._rc = CRect(tx._rc.left, tx._rc.top, tx._rc.right, tx._rc.bottom = _tt.getRcHeaderVert(r).top);
				tx._rowB = r;/// �Ʒ��� Ű���.
			}

			//CString sBegin, sEnd;
			//KwCTimeToString(fOffBegin, sBegin);
			//KwCTimeToString(fOffEnd, sEnd);

			//// �� ���� �ð���? r
			//CKTimeSpan spt(0, 0, _tt._minute* tx._rowT, 0);
			//CKTimeSpan spb(0, 0, _tt._minute* tx._rowB, 0);
			//CKTime night(fOffBegin.GetYear(), fOffBegin.GetMonth(), fOffBegin.GetDay(), 0, 0, 0);
			//CKTime newBegin = night + spt;
			//CKTime newEnd = night + spb;

			CKTime tb = RowToTime(tx._rowT);
			CKTime te = RowToTime(tx._rowB);
			jocc("fOffBegin") = tb;
			jocc("fOffEnd") = te;


			_tt.FinishDrag();///_selBox �� ���� ���� ����.

			_wnd->InvalidateRect(_tt._rcc, 0);

			if(rowT != tx._rowT || rowB != tx._rowB)
				(*onUpdate)();/// �ð��̳� ����ڰ� �ٲ����.

			if(!selBoxBefore || selBoxBefore != _tt._selBox)
				(*onSel)();/// ������ ������ �ٲ�� ��â ������ �ٲ��.
			TRACE("(%d,%d) (%d,%d) (%d,%d,%d,%d)\n", tx._colL, tx._rowT, tx._colR, tx._rowB, tx._rc.left, tx._rc.top, tx._rc.Width(), tx._rc.Height());
		}// top, bottom
		//TRACE("%s (%d, %d) (%d, %d)\n", tx._side, tx._colL, tx._colR, tx._rowT, tx._rowB);
	}//if(_tt._curBox)

}

void TimeTable::MouseMove(UINT nFlags, CPoint point)
{
	// box�� top�̳� bottom �����ȿ��� 1�ʰ� �ӹ��� 
	// Ŀ���� ũ������ ���������� �ٲ�� �ű⼭ ������ 30�� ���� �������� �ȿ��� 
	// �ڽ� ũ�� ���� �Ǹ�, ��ư Ŭ���� ������ �ϰ� 3�� ����� ������ ���� ������ �ٷ� ��� ����.

	// �ڽ� �ȿ��� ����� ������ Ŭ�� ������ ���� �ո�� ���������� �ٲ�� �ٸ� �������� �̵��� �� �ִ�.
	// �̵��� 3�� ���߸� ���� ���� ��û �Ѵ�.
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
	else if(nFlags & MK_LBUTTON)// left button �ϋ��� �۵�
	{
		//_tt._curBox = box;
		TBox* box = _tt._curBox;// _tt.GetBox(scheID);

		JObj& jocc = *box->_jocc;//shared�� shared
		//auto keyShed = _tt.MakeBoxKey(*box->_jocc);//
		CString fVChrgID = jocc.S("fVChrgID");
		bool bCmnSched = fVChrgID.IsEmpty();//���� �����̸� fVChrgID == null �а� ������ �ִ�.
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
		rcInv.top += 30;//30�� ��ü ���� �� 30��ŭ �ȹ�½�̰� �ٽ� �ȱ׷� ������
		_wnd->InvalidateRect(rcInv, 0);

		PAS pside = KwIsInRectSide(box->_rc, point);
	}
}

void TimeTable::FinishDrag()
{
	if(_curBox)
		_selBox = _curBox;///�����̴� cur�� reset�ϰ� ���������� ���õ� sel�� ���� �ٲ۴�.
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

/// request ���� �� ��� ���� ��ġ�� ���� �Ѵ�.
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

	tx._rc = getRcCell(tx._colL, tx._rowT);  /// �����ô� �ϳ��� ���� �߰� �ǹǷ� ����Ұ� ���� �� �ϳ�ũ���� ���� �´�.
	//�̰� �Ʒ����� _secBox �� ���Եȴ�.

	CKTime tb = RowToTime(tx._rowT);
	CKTime te = RowToTime(tx._rowB);/// 30�� ¥���� �⺻���� ����� ����.
	auto newIDX = _maxIdx++;
	jocc("fIDX") = newIDX;
	/// ���� 5������ �̰� DB���� ������ ������ �̰��̴�.
	jocc("fOrder") = 1140;
	jocc("fSrcDesc") = L"����";
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
	
	_selBox = ptx; ///��� ����� ���õȴ�.

	/// ������ �߰� �� ���� �ϸ� newIDX �� _mapBoxes ���� ��� ���� �ؾ� ȭ�鿡�� ��������. �־��� ������ ������ �ȴ�.
	/// ���� �ϸ� ���� fSchedOrgID �� _requestedIDX ������ ���� �Ѵ�.
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

	tx._rc = getRcCell(tx._colL, tx._rowT);  /// �����ô� �ϳ��� ���� �߰� �ǹǷ� ����Ұ� ���� �� �ϳ�ũ���� ���� �´�.
	//�̰� �Ʒ����� _secBox �� ���Եȴ�.

	CKTime tb = RowToTime(tx._rowT);
	CKTime te = RowToTime(tx._rowB);/// 30�� ¥���� �⺻���� ����� ����.
	auto newIDX = _maxIdx++;
	jocc("fIDX") = newIDX;
	/// ���� 5������ �̰� DB���� ������ ������ �̰��̴�.
	jocc("fOrder") = 1130;
	jocc("fSrcDesc") = L"����";
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

	_selBox = ptx; ///��� ����� ���õȴ�.

	/// ������ �߰� �� ���� �ϸ� newIDX �� _mapBoxes ���� ��� ���� �ؾ� ȭ�鿡�� ��������. �־��� ������ ������ �ȴ�.
	/// ���� �ϸ� ���� fSchedOrgID �� _requestedIDX ������ ���� �Ѵ�.
}
