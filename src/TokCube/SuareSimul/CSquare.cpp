#include "pch.h"
#include "CSquare.h"
#include <cstdlib>
#include "KwLib64/Kw_tool.h"

void CSquare::Reset()
{
	AUTOLOCK(_csMix);
	for(int r = 0; r < _row; r++)
	{
		for(int c = 0; c < _col; c++)
		{
			sq[r][c] = sqsrc[r][c];// r;
			if(r == 0)
				_ver[c] = 0;
		}
		_hor[r] = 0;
	}
	_mix.clear();
	_mixOnly.clear();
	_count = 0;
}
bool CSquare::Check()
{
	for(int r = 0; r < _row; r++)
	{
		for(int c = 0; c < _col; c++)
		{
			if(sq[r][c] != sqsrc[r][c])
				return false;
		}
	}
	return true;
}
double CSquare::CheckEx()
{
	int score = 0;
	for(int r = 0; r < _row; r++)
	{
		for(int c = 0; c < _col; c++)
		{
			if(sq[r][c] == sqsrc[r][c])
				score++;
		}
	}
	double rate = (double)score / (_row * _col);
	return rate;
}


//?deprecated never used
void CSquare::MoveUnit()
{
// 	int du = Narrow();
// 	/// 초기 간격
// 	for(int r = 0; r < _row; r++)
// 	{
// 		for(int c = 0; c < _col; c++)
// 		{
// 			if(_unit == 1)
// 			{
// 				_left[r][c] = _leftInit[r][c];
// 				_top[r][c] = _topInit[r][c];
// 			}
// 			else// if(_unit == 1)
// 			{
// 				int dc = c / _unit;
// 				int dr = r / _unit;
// 				_left[r][c] = _leftInit[r][c] + (dc * du);
// 				_top[r][c] = _topInit[r][c] + (dr * du);
// 			}
// 		}
// 	}
}

int CSquare::Narrow()
{
	int du = 4;
// #ifdef _DEBUG
// #else
// 	int du = _leftInit[0][1] - (_leftInit[0][0] + _size);
// #endif // _DEBUG
	return du;
}

void CSquare::Mix(int n)
{
	AUTOLOCK(_csMix);
	_tikStart = 0;
	int nTry = 0;
	for(int i = 0; i < n; nTry++)
	{
		KMove mv;
		int r1 = std::rand();
		mv.dr = r1 % 4; // l,r,u,d
		int r2 = std::rand();
		mv.l = r2 % (mv.dr < 2 ? _row : _col); // 0:left, 몇번째 row

		if(IsStaticLine(mv.l))
			continue;

		int r3 = std::rand();

		mv.n = _unit;// r3 % (mv.dr < 2 ? _col : _row);
		i++;

		/// <summary>
		_mixOnly.push_back(mv);
		_mix.push_back(mv);
		/// </summary>
		TRACE("Mix\t%d\t%d\t%d\n", i, mv.dr, mv.l);// , mv.n);
		switch(mv.dr)
		{
		case 0: lf(mv, 0); break;
		case 1: rt(mv, 0); break;
		case 2: up(mv, 0); break;
		case 3: dn(mv, 0); break;
		}
		//TRACE("Mix: %d, %d, %d\n", mv.dr, mv.l, mv.n);
	}
}
void CSquare::Mix(PWS sInit)
{
	AUTOLOCK(_csMix);
	_tikStart = 0;
	vector<wstring> rows;
	KwCutByToken(sInit, L"\n", rows, true);
	for(int r = 0; r < rows.size(); r++)
	{
		auto row = rows[r];
		
		vector<wstring> cols;
		KwCutByToken(row.c_str(), L",", cols, true);
		for(int c = 0; c < cols.size(); c++)
		{
			auto col = cols[c];
			int icl = KwAtoi(col.c_str());
			sq[r][c] = icl;
		}
	}
}

void CSquare::lf(KMove mv, int bPush)
{
	for(int i = 0; i < mv.n; i++)
	{
		int tmp = sq[mv.l][0];
		for(int ic = 0; ic < (_col - 1); ic++)
			sq[mv.l][ic] = sq[mv.l][ic + 1];
		sq[mv.l][_col - 1] = tmp;
	}
	AUTOLOCK(_csMix);
	if(bPush)
		_mix.push_back(mv);
}

void CSquare::rt(KMove mv, int bPush)
{
	for(int i = 0; i < mv.n; i++)
	{
		int tmp = sq[mv.l][_col - 1];
		for(int ic = (_col - 1); ic > 0; ic--)
			sq[mv.l][ic] = sq[mv.l][ic - 1];
		sq[mv.l][0] = tmp;
	}
	AUTOLOCK(_csMix);
	if(bPush)
		_mix.push_back(mv);
}

void CSquare::up(KMove mv, int bPush)
{
	for(int i = 0; i < mv.n; i++)
	{
		int tmp = sq[0][mv.l];
		for(int ic = 0; ic < (_row -1); ic++)
			sq[ic][mv.l] = sq[ic + 1][mv.l];
		sq[_row - 1][mv.l] = tmp;
	}
	AUTOLOCK(_csMix);
	if(bPush)
		_mix.push_back(mv);
}

void CSquare::dn(KMove mv, int bPush)
{
	for(int i = 0; i < mv.n; i++)
	{
		int tmp = sq[_row - 1][mv.l];
		for(int ic = (_row - 1); ic > 0; ic--)
			sq[ic][mv.l] = sq[ic - 1][mv.l];
		sq[0][mv.l] = tmp;
	}
	AUTOLOCK(_csMix);
	if(bPush)
		_mix.push_back(mv);
}

void CSquare::InitCellRects()
{
	CPoint lt(0, 0);/// 상대 위치만 가지고 있자. _ltMargin);
	CSize sz(_size);// , _size);
	CRect rc(lt, sz);
	for(int r = 0; r < _row; r++)
	{
		for(int c = 0; c < _col; c++)
		{
			_left[r][c] = _leftInit[r][c] = rc.left;
			_top[r][c] = _topInit[r][c] = rc.top;
			rc.MoveToXY(rc.left + sz.cy, rc.top);
		}
		rc.MoveToXY(lt.x, rc.top + sz.cy);
	}
}
