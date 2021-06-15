#pragma once
//#include "KwLib64/JSON/JSON.h"
#include "KwLib64/TimeTool.h"

class TBox
{
public:
	ShJObj _jsch;///�÷�/���
	ShJObj _jocc;///�÷�/�迭/����

	///dragging���� ��.
	int _rowT{ -1 };
	int _rowB{ -1 };//
	int _colL{ -1 };//
	int _colR{ -1 };//

	/// �巡���� ��ġ
	CRect _rc{ 0,0,0,0 };
	
	///�巡�� �� ��ġ
	CRect _rcStatic{ 0,0,0,0 };
	
	/// ���� ���� ��û�� ��� _rcStatic ���� �ȴ�.
	//CRect _rcBefore{ 0,0,0,0 };//���� OK �ȶ��� ���� ���� ������ ���δ�. request���� ����
	

	CPoint _hotspot;///�ڽ��ȿ��� ������ ��ġ. ���콺 �Ǻ�� �׸� �簢�� ��ġ. �� offset�� �׸���.
	CStringA _side;// �ڽ� ������. center , left, right, top, bottom 5���� �� �ϳ�
	/// ���콺 �������� �� �簢���� ���� ������ ���� ã�´�.
	//CString fSchedOrgID;//?deprecated _jocc�� �־ ���ʿ�
	ShJObj _joccBefore;///���콺�� �������� ������ ������ ���� �Ǳ� �� ��� �صд�. �������� ���� ���󺹱� �ؾ� �ϴ�
};
//_selSchedID == box.fSchedOrgID �̸� ���� ���õ� �ڽ�
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

	//CRect _rcDraw{ CPoint(50,50), CSize(_szVert.cx + (_szHorz.cx * _nCols), _szHorz.cy + (_nRows * _szVert.cy))};//��ü
	//CRect _rc{CRect(CPoint(50,50), CSize(_rcDraw.Width(), _rcDraw.bottom))};//��ü
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

	/// �������� �о�� ������ ��ũ�� ��Ƽ������ �����̶� �� �ϱ� ����
	
	/// Ÿ�����̺� ��¥
	CKTime _day;

	CKCriticalSection _cs_jtbl;//���� �����尡 �̿� �ϹǷ�
	/// �������� �о�� ������ ��ũ
	ShJObj _jtbl;

	/// ������Ű�� map �� �÷��� fOrderSched �� ���� �迭�� ����
	
	KArray<ShJObj> _col;// ������ ���� ������ ������ ����.
	KPtrArray<TBox> _boxes;
	KStdMapPtr<int, TBox> _mapBoxes;

	void Reset();

	/// �о�� �����͸� �÷� �迭 _col�� ���ġ
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
	/// �����Ϳ��� ������ ����
	int _minute{ 30 };//fMinuteForCut30
	//_nRows fNumberOfTimeCut48


	/// ///////////////////////////////////////////////////
	/// �����Ϳ� ���� brush����
	KStdMapPtr<wstring, Brush> _mapBr;
	/// col vs <row vs fPause>
	KStdMapPtr<int, KStdMap<int, wstring>> _mapCellMode;
	/// ///////////////////////////////////////////////////

	/// ��������(fPause)�� �� ����� ����. ����(fOff)�� ���� �ڽ��� �������ο� ������ ��ĥ�� �ִ�.
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
		if(!col->Has(r))//������ �ִ´�? ���� ������ �ʴ´�.
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
		if(cst == nullptr)//�⺻��
		{
			if(_brCell == nullptr)
				_brCell = new SolidBrush(Color(255, 255, 237));
			return _brCell;
		}
		else
		{
			if(!_mapBr.Has(cst))
			{
				Color cl(192, 192, 192);//�⺻ 
				if(tchsame(cst, L"closed"))//�����ܽð�
					cl = Color(216, 216, 216);
				else if(tchsame(cst, L"holiday"))//����
					cl = Color(129, 129, 129);
				else if(tchsame(cst, L"work"))//Ư�����
					cl = Color(245, 247, 249);
				else if(tchsame(cst, L"open"))//Ư������
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
			_brBox = new SolidBrush(Color(210, 200, 187));// 237, 125, 49));�����迭
		return _brBox;
	}
	/// �����Ϳ� ���� brush����
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
	//�ٽ� �о� �´�.
	void DrawConer();
	void DrawHeadHorz();
	void DrawHeadVert();
	void DrawCells();
	void DrawBoxes();

	void DrawOneBox(TBox* box, float wth = 1.f);



	/// ���° �ڽ� �����. [6, "left"]
	/// auto[wstring, side] = ButtonDown(point);
	bool ButtonDown(CPoint point, UINT nFlags);

	template<typename FncSel, typename FnxUpdate>
	void LButtonUp(UINT nFlags, CPoint point, FncSel onSelected, FnxUpdate onUpdated)
	{
		auto OnSelected = make_shared<function<void()>>(onSelected);
		auto OnUpdated  = make_shared<function<void()>>(onUpdated);
		
		/// �̷��� functions ������ �ٲٸ� �Ʒ� �Լ��� template�� ��� ���Ͽ� ���� ���� �ʾƵ� �ȴ�.
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

	/// �ֱٿ� ���� Ŀ����ġ�� �� col row ��� => �����߰� �Ҷ� ����.
	CPoint _curColRow{ -1, -1 };

	/// fIDX�� �ְ� ���� �־� �д�. AddOrder�Ҷ� ȭ�鿡�� �߰� �ǰ� �Ϸ��� Unique�ؾ� �ϱ� ������.
	int _maxIdx{ -1 };
	
	/// AddOrder�� ��û�ϱ� �� �־� �ΰ� ���� �ϸ� ���ְ�,
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
		CKTimeSpan sp = tm2 - tm1; // �ϴ� �ð� ������ ����� �ΰ�
		int dr = (int)(sp.GetMinutes() / _minute);// ������ �� �� ������
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

