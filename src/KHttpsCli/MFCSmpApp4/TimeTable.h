#pragma once
//#include "KwLib64/JSON/JSON.h"
#include "KwLib64/TimeTool.h"

class TBox
{
public:
	ShJObj _jsch;///컬럼/담당
	ShJObj _jocc;///컬럼/배열/일정

	///dragging중인 셀.
	int _rowT{ -1 };
	int _rowB{ -1 };//
	int _colL{ -1 };//
	int _colR{ -1 };//

	/// 드래깅중 위치
	CRect _rc{ 0,0,0,0 };
	
	///드래깅 전 위치
	CRect _rcStatic{ 0,0,0,0 };
	
	/// 서버 변경 요청전 백업 _rcStatic 쓰면 된다.
	//CRect _rcBefore{ 0,0,0,0 };//서버 OK 안떨어 지면 원상 복구때 씌인다. request전에 보관
	

	CPoint _hotspot;///박스안에서 눌러진 위치. 마우스 므브시 그릴 사각형 위치. 그 offset에 그린다.
	CStringA _side;// 박스 누를때. center , left, right, top, bottom 5가지 중 하나
	/// 마우스 놓을떄는 그 사각형과 가장 근접한 셀을 찾는다.
	//CString fSchedOrgID;//?deprecated _jocc가 있어서 불필요
	ShJObj _joccBefore;///마우스로 움직여서 놓으면 값들이 변경 되기 전 백업 해둔다. 서버에러 나면 원상복귀 해야 하니
};
//_selSchedID == box.fSchedOrgID 이면 현재 선택된 박스
class TimeTable
{
public:
	
	TimeTable(CWnd* wnd)
		: _wnd(wnd)
	{
	}
	~TimeTable()
	{
		DeleteBrush();
		DeleteMeSafe(_gc);
	}
	CWnd* _wnd{ nullptr };

	int _nCols{ 6 };
	int _nRows{ 48 };

	/// redraw switch
	bool _bRedraw{ false };
	HDC _hDC{ nullptr };
	CSize _szHorz{ 75, 25 };
	CSize _szVert{ 80, 20 };
	CRect _rcc{ 0,0,100,100 };

	//CRect _rcDraw{ CPoint(50,50), CSize(_szVert.cx + (_szHorz.cx * _nCols), _szHorz.cy + (_nRows * _szVert.cy))};//전체
	//CRect _rc{CRect(CPoint(50,50), CSize(_rcDraw.Width(), _rcDraw.bottom))};//전체
	CRect RcDraw()
	{
		return CRect(CPoint(50, 50), CSize(_szVert.cx + (_szHorz.cx * _nCols), _szHorz.cy + (_nRows * _szVert.cy)));
	}
	CRect Rc()
	{
		return CRect(CPoint(50, 50), CSize(_szVert.cx + (_szHorz.cx * _nCols), _szHorz.cy + (_nRows * _szVert.cy)));
	}
	Brush* _brHorz{ nullptr };
	Brush* _brVert{ nullptr };
	Brush* _brCell{ nullptr };
	Brush* _brBox{ nullptr };

	Pen* _penHorz{ nullptr };
	Pen* _penVert{ nullptr };
	Pen* _penCell{ nullptr };
	Pen* _penCell2{ nullptr };
	Pen* _penBox{ nullptr };
	void DeleteBrush()
	{
		DeleteMeSafe(_brHorz);
		DeleteMeSafe(_brVert);
		DeleteMeSafe(_brCell);
		DeleteMeSafe(_brBox);

		DeleteMeSafe(_penHorz);
		DeleteMeSafe(_penVert);
		DeleteMeSafe(_penCell);
		DeleteMeSafe(_penCell2);
		DeleteMeSafe(_penBox);
	}

	/// 서버에서 읽어온 데이터 링크가 멀티스레드 접근이라 락 하기 위함
	
	/// 타임테이블 날짜
	CKTime _day;

	CKCriticalSection _cs_jtbl;//여러 스레드가 이용 하므로
	/// 서버에서 읽어온 데이터 링크
	ShJObj _jtbl;

	/// 가상담당키로 map 인 컬럼을 fOrderSched 에 의해 배열로 정렬
	
	KArray<ShJObj> _col;// 정렬을 위해 공유된 포인터 보유.
	KPtrArray<TBox> _boxes;
	KStdMapPtr<int, TBox> _mapBoxes;

	void Reset();

	/// 읽어온 데이터를 컬럼 배열 _col로 재배치
	void ArrangeColumn();
	CKTime _theDay;
	CSize getSzConer() { return CSize(_szVert.cx, _szHorz.cy); }
	CRect getRcConer() { CRect rc = Rc(); return CRect(CPoint(rc.left, rc.top), getSzConer()); }
	CSize getSzCell() { return CSize(_szHorz.cx, _szVert.cy); }
	CRect getRcHeaderHorz(int col)
	{
		CRect rc = Rc();
		return CRect(CPoint(rc.left + _szVert.cx + (col * _szHorz.cx), rc.top), _szHorz);
	}
	CRect getRcHeaderVert(int row)
	{
		CRect rc = Rc();
		return CRect(
			CPoint(rc.left, rc.top + _szHorz.cy + (row * _szVert.cy)),
			_szVert);
	}

	CPoint getBasePoint()
	{
		CRect rc = Rc();
		CSize szCnr = getSzConer();
		return CPoint(rc.left + szCnr.cx, rc.top + szCnr.cy);
	}
	CRect getMainArea()
	{
		CRect rc = Rc();
		CSize szCell = getSzCell();
		return CRect(getBasePoint(), CSize(rc.Width() - szCell.cx, rc.Height() - szCell.cy));
	}
	CRect getRcCell(int col, int row)
	{
		CRect rc = Rc();
		CSize szCnr = getSzConer();
		CPoint pt(rc.left + szCnr.cx + (col * _szHorz.cx), rc.top + szCnr.cy + (row * _szVert.cy));
		CSize szCell = getSzCell();
		return CRect(pt, szCell);
	}

	HDC _hdc{ nullptr };
	Graphics* _gc{ nullptr };

	void InitGC(HDC hDC)
	{
		_hDC = hDC;
		_gc = new Graphics(_hDC);
	}

	Brush* getBrushHeaderHorz()
	{
		if(_brHorz == nullptr)
			_brHorz = new SolidBrush(Color(230, 240, 255));
		return _brHorz;
	}
	Brush* getBrushHeaderVert()
	{
		if(_brVert == nullptr)
			_brVert = new SolidBrush(Color(245, 250, 250));
		return _brVert;
	}


	/// ///////////////////////////////////////////////////
	/// 데이터에서 셋팅한 변수
	int _minute{ 30 };//fMinuteForCut30
	//_nRows fNumberOfTimeCut48


	/// ///////////////////////////////////////////////////
	/// 데이터에 따라 brush구분
	KStdMapPtr<wstring, Brush> _mapBr;
	/// col vs <row vs fPause>
	KStdMapPtr<int, KStdMap<int, wstring>> _mapCellMode;
	/// ///////////////////////////////////////////////////

	/// 영업여부(fPause)를 셀 색깔로 구분. 일정(fOff)은 위에 박스로 영업여부와 일정은 겹칠수 있다.
	void AddCellMode(int c, int r, PWS mode)
	{
		if(tchlen(mode) == 0)
			return;
		KStdMap<int, wstring>* col = nullptr;
		if(!_mapCellMode.Lookup(c, col))
		{
			col = new KStdMap<int, wstring>();
			_mapCellMode[c] = col;
		}
		if(!col->Has(r))//없으면 넣는다? 덮어 쓰지는 않는다.
			(*col)[r] = mode;
	}

	wstring GetCellMode(int c, int r)
	{
		wstring wsm;
		KStdMap<int, wstring>* col = nullptr;
		if(_mapCellMode.Lookup(c, col))
			col->Lookup(r, wsm);
		return wsm;
	}

	Brush* getBrushCell(int c, int r)
	{
		wstring cst = GetCellMode(c, r);
		Brush* br = getBrushCell(cst.length() > 0 ? cst.c_str() : nullptr);// .c_str());
		ASSERT(br);
		return br;
	}
	Brush* getBrushCell(PWS cst = nullptr)
	{
		if(cst == nullptr)//기본색
		{
			if(_brCell == nullptr)
				_brCell = new SolidBrush(Color(255, 255, 237));
			return _brCell;
		}
		else
		{
			if(!_mapBr.Has(cst))
			{
				Color cl(192, 192, 192);//기본 
				if(tchsame(cst, L"closed"))//영업외시간
					cl = Color(216, 216, 216);
				else if(tchsame(cst, L"holiday"))//휴일
					cl = Color(129, 129, 129);
				else if(tchsame(cst, L"work"))//특별출근
					cl = Color(245, 247, 249);
				else if(tchsame(cst, L"open"))//특별오픈
					cl = Color(255, 255, 192);
				Brush* br = new SolidBrush(cl);
				_mapBr[cst] = br;
			}
			return _mapBr[cst];
		}
	}
	Brush* getBrushBox()
	{
		if(_brBox == nullptr)
			_brBox = new SolidBrush(Color(210, 200, 187));// 237, 125, 49));붉은계열
		return _brBox;
	}
	/// 데이터에 따라 brush구분
	/// ///////////////////////////////////////////////////

	Pen* getPenHeaderHorz()
	{
		if(_penHorz == nullptr)
			_penHorz = new Pen(Color(152, 152, 152));
		return _penHorz;
	}
	Pen* getPenHeaderVert()
	{
		if(_penVert == nullptr)
			_penVert = new Pen(Color(152, 152, 152));
		return _penVert;
	}
	Pen* getPenCell(int r)
	{
		if(_penCell == nullptr)
			_penCell = new Pen(Color(40, 0, 0, 25));
		if(_penCell2 == nullptr)
			_penCell2 = new Pen(Color(20, 0, 0, 25));
		return (r % 2 == 0) ? _penCell : _penCell2;
	}
	Pen* getPenBox()
	{
		if(_penBox == nullptr)
			_penBox = new Pen(Color(22, 92, 92));
		return _penBox;
	}

	void Draw3DRect(CRect rc, float wth = 1.0f);


	void DrawTimeTable();

	void Rfresh();
	void DrawLines();
	//다시 읽어 온다.
	void DrawConer();
	void DrawHeadHorz();
	void DrawHeadVert();
	void DrawCells();
	void DrawBoxes();

	void DrawOneBox(TBox* box, float wth = 1.f);



	/// 몇번째 박스 어느쪽. [6, "left"]
	/// auto[wstring, side] = ButtonDown(point);
	bool ButtonDown(CPoint point, UINT nFlags);

	template<typename FncSel, typename FnxUpdate>
	void LButtonUp(UINT nFlags, CPoint point, FncSel onSelected, FnxUpdate onUpdated)
	{
		auto OnSelected = make_shared<function<void()>>(onSelected);
		auto OnUpdated  = make_shared<function<void()>>(onUpdated);
		
		/// 이렇게 functions 변수로 바꾸면 아래 함수를 template로 헤더 파일에 정의 하지 않아도 된다.
		DoLButtonUp(nFlags, point, OnSelected, OnUpdated);
	}
	void DoLButtonUp(UINT nFlags, CPoint point, 
		shared_ptr<function<void()>> onSel, shared_ptr<function<void()>> onUpdate);

	void MouseMove(UINT nFlags, CPoint point);

	TBox* _selBox{ nullptr };
	TBox* _curBox{ nullptr };
	TBox* IsCursorInBox(CPoint point, CStringA& side);
	//TBox* GetBox(CString scheKey);

	void FinishDrag();
	void RecoverUpdate();

	//int MakeBoxKey(JObj& jocc);

	void DeleteSelBox();

	/// 최근에 누른 커서위치로 셀 col row 기억 => 일정추가 할때 쓴다.
	CPoint _curColRow{ -1, -1 };

	/// fIDX중 최고 값을 넣어 둔다. AddOrder할때 화면에서 추가 되게 하려면 Unique해야 하기 때문에.
	int _maxIdx{ -1 };
	
	/// AddOrder로 요청하기 전 넣어 두고 성공 하면 없애고,
	KStdMap<wstring, int> _requestedIDX;

	void AddOrder(int col, int row, ShJObj sjo);
	void AddSchedule(int col, int row);

	CKTime RowToTime(int row)
	{
		ASSERT(_day.GetHour() == 0 && _day.GetMinute() == 0);
		CKTimeSpan spt(0, 0, _minute * row, 0);
		CKTime newTm = _day + spt;
		return newTm;
	}
	int TimeToRow(CKTime tm)
	{
	}
	int TimeToRow(CKTime tm1, CKTime tm2)
	{
		CKTimeSpan sp = tm2 - tm1; // 일단 시간 간격을 백업해 두고
		int dr = (int)(sp.GetMinutes() / _minute);// 간격을 셀 분 단위로
		CKTime night(tm1.GetYear(), tm1.GetMonth(), tm1.GetDay(), 0, 0, 0);
	}
//	void RotToTime(CKTime day, int row1, int row2, CKTime& newTm1, CKTime& newTm2)
//	{
//		ASSERT(_day.GetHour() == 0 && _day.GetMinute() == 0);
////		CKTime night(day.GetYear(), day.GetMonth(), day.GetDay(), 0, 0, 0);
//		CKTimeSpan spt(0, 0, _minute * row1, 0);
//		CKTimeSpan spb(0, 0, _minute * row2, 0);
//		newTm1 = _day + spt;
//		newTm2 = _day + spb;
//	}

	void AfterAddOrder(bool bOK, PWS fSchedOrgID);

};

