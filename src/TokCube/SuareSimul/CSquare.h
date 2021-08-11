#pragma once

#include <list>
#include "KwLib64/ThreadPool.h"
#include "KwLib64/Lock.h"

enum EDirection
{
	eLF, eRT, eUP, eDN
};
enum ESide
{
	eLeft, eTop, eRight, eBottom
};


class KMove
{
public:
	int dr{-1};// l,r,u,d 방향
	int l{-1};//r or c : 행 또는 열
	int n{0};// 1,2,3 움직인 칸수 
	LONGLONG tik{0};// mix때는 0
};
enum CellSize
{
	eCbR = 6, eCbC = 9,
	eCbRowInit = 6, eCbColInit = 9,
};
class CSquare
{
public:
	int _col{eCbColInit};
	int _row{eCbRowInit};

	LPCSTR _arDirection[4] = {"eLF", "eRT", "eUP", "eDN"};


	/// deprecated
	int sqOld[eCbR][eCbC]{
		{0,0,0,0,0,0,0,0,0,},
		{1,1,1,1,1,1,1,1,1,},
		{2,2,2,2,2,2,2,2,2,},
		{3,3,3,3,3,3,3,3,3,},
		{4,4,4,4,4,4,4,4,4,},
		{5,5,5,5,5,5,5,5,5,},
// 		{6,6,6,6,6,6,6,6,6,},
// 		{7,7,7,7,7,7,7,7,7,},
// 		{8,8,8,8,8,8,8,8,8,},
	};
	/// reset
	int sqsrc[eCbR][eCbC]{
	{0,0,0,1,1,1,2,2,2,},
	{0,0,0,1,1,1,2,2,2,},
	{0,0,0,1,1,1,2,2,2,},
	{3,3,3,4,4,4,5,5,5,},
	{3,3,3,4,4,4,5,5,5,},
	{3,3,3,4,4,4,5,5,5,},
// 	{6,6,6,7,7,7,8,8,8,},
// 	{6,6,6,7,7,7,8,8,8,},
// 	{6,6,6,7,7,7,8,8,8,},
	};
	/// current
	int sq[eCbR][eCbC]{
	{0,0,0,1,1,1,2,2,2,},
	{0,0,0,1,1,1,2,2,2,},
	{0,0,0,1,1,1,2,2,2,},
	{3,3,3,4,4,4,5,5,5,},
	{3,3,3,4,4,4,5,5,5,},
	{3,3,3,4,4,4,5,5,5,},
// 	{6,6,6,7,7,7,8,8,8,},
// 	{6,6,6,7,7,7,8,8,8,},
// 	{6,6,6,7,7,7,8,8,8,},
	};

	/// init position
	//?deprecated never used
	int _leftInit[eCbR][eCbC]{0,};
	int _topInit[eCbR][eCbC]{0,};

	/// Current position moved
	/// 2칸이면 2개 마다 간격이 벌어 진다.
	int _left[eCbR][eCbC]{0,};
	int _top[eCbR][eCbC]{0,};

	/// width and height
	CSize _ltMargin{10, 10};
	CSize _rbMargin{10, 10};

	CSize _size{50, 50};

	/// current tilt stat
	int _hor[eCbR]{0,0,0,0,0,0};// if 1. then next box location, 
	int _ver[eCbC]{0,0,0,0,0,0,0,0,0};// -3, -2, -1, 0, 1, 2, 3, 4

	/// 섞을때 저장
	std::list<KMove> _mix;
	CKCriticalSection _csMix;
	std::list<KMove> _mixOnly;
	std::list<KMove> _moveQ;
	CKCriticalSection _csMove;
	
	LONGLONG _tikStart{0};
	/// 마지막 액션 까지 걸린 시간
	LONGLONG _tElapsed{0};
	/// 몇칸씩 이동?
	int _unit{3};
	CString _mode{L"axis"};// or 'free'

	/// 움직인 횟수
	int _count{0};

	/// color set 0,1
	int _setCl{0};

	/// 1, 4, 7, 처럼 3칸씩 돌릴때 줌심이 되어서 움직이지 않는 고정 행이나 열.
	bool IsStaticLine(int i)
	{
		//모든단위 일때 가운데 고정
		return //_unit == 3 && 
			(i % 3) == 1;
	}

	/// 마지막으로 움
	KMove _lastMv;
	/// replay 때 속도. 몇배속
	int _multiSpeed{1};
	/// animaation frame 수.
	int _nFramme{5};// 5;

	/// <summary>
	/// 애니메이션의 한프레임 움직임
	/// </summary>
	/// <param name="rdr">방향</param>
	/// <param name="mvl">몇번째 행 또는 열</param>
	void Sliding(int rdr, int mvl)//KMove& mv)
	{
		switch(rdr)
		{
		case eLF: _hor[mvl]--; break;/// 조금씩 이동
		case eRT: _hor[mvl]++; break;
		case eUP: _ver[mvl]--; break;
		case eDN: _ver[mvl]++; break;
		}
	}
	/// <summary>
	/// 애니메이션에 움직임 초기화
	/// </summary>
	/// <param name="rdr">방향</param>
	/// <param name="mvl">몇번째 행 또는 열</param>
	void SlidingInit(int rdr, int mvl)//KMove& mv)
	{
		switch(rdr)
		{
		case eLF: _hor[mvl] = 0; break;/// 원위치
		case eRT: _hor[mvl] = 0; break;
		case eUP: _ver[mvl] = 0; break;
		case eDN: _ver[mvl] = 0; break;
		}
	}
	/// <summary>
	/// 애니메이션에 움직임 초기화
	/// </summary>
	/// <param name="mv">움직임 정보</param>
	/// <param name="bPush">_mix에 추가 여부</param>
	void MoveFinish(KMove& mv, int bPush)
	{
		switch(mv.dr)
		{
		case eLF: lf(mv, bPush); _hor[mv.l] = 0; break; /// 드디어 이동
		case eRT: rt(mv, bPush); _hor[mv.l] = 0; break;
		case eUP: up(mv, bPush); _ver[mv.l] = 0; break;
		case eDN: dn(mv, bPush); _ver[mv.l] = 0; break;
		}
	}

	template <typename TLB>
	void MoveAny(std::list<KMove>& mx, TLB fnc)
	{
		//int n = mx.size();
		int nFramme = _nFramme - (_unit == 1 ? 2 : 0);// 1칸 이동때는 애니를 2개 덜한다.
		for(int i = 0; ; i++)
		{
			KMove mv;
			{
				AUTOLOCK(_csMove);
				mv = mx.front();///웬만하면 복사해서 써야. 백그라운드 작업이니.
				mx.pop_front();//바로제거
			}
			for(int a = 0; a < nFramme; a++)///애니메이션 프레임 5개
			{
				if(a < (nFramme - 1))
				{
					Sliding(mv.dr, mv.l);
					/// 이동칸수 작을 수록 Sleep 짧아
					Sleep(30);// _unit * 10);//여기 놓아야. 프레임 사이일때만 지연 되지
				}
				else
				{
					MoveFinish(mv, 1);
				}
				if(mx.size() == 0)/// 그 사이에 또 액션이 누적되지 않았으면 화면을 다시 그린다.
					fnc(mv.dr, mv.l, a); // invaludateRect 포그라운드로 통보
				else if(i < (nFramme - 2))/// 4이면 마지막이고. 그사이에 누적된 액션이 있으면 나머지 프레임은 그리는거 포기 한다.
					i = (nFramme - 2);/// 3이면 4(마지막)이 될꺼고, 2이하이면 3으로 셋 하면 다음 ++ 에서 4가 되므로 건너 띈다.
			}
			_lastMv = mv;;
			if(mx.size() == 0)
				break;/// 그 사이에 또 액션이 누적되지 않았으면 끝낸다. 더 이동 버퍼가 없으므로
		}
	}

	int _aniStep{3};

	template <typename TLB>
	void Replay(std::list<KMove>& lstMove, bool bReverse, bool bElpsed, TLB fnc)
	{
		AUTOLOCK(_csMix);
		int bBreak = 0;
		auto n = lstMove.size();
		LONGLONG prvTik = 0;
		for(int i = 0; i < n; i++)
		{
			KMove mv;
			if(bReverse)
				mv = lstMove.back();//역순
			else
				mv = lstMove.front();

			KMove mvr = mv;
			if(bReverse)//반대방향
				mvr.dr =
				mv.dr == eLF ? eRT :
				mv.dr == eUP ? eDN :
				mv.dr == eRT ? eLF :
				mv.dr == eDN ? eUP : -1;
			else
				mvr.dr = mv.dr;//리플레이 순방향
			double dsl = ((double)mv.tik / 10000) / _multiSpeed;
			TRACE("Replay\t%d\t%d\t%d\n", i, mvr.dr, mvr.l);// , mv.n);
// 			TRACE("Replay(%s):%d. dr(%d), l(%d), n(%d), tk(%d) Sleep(%d)\n",
// 				bReverse? "reverse":"", i, mvr.dr, mvr.l, mvr.n, mvr.tik, (int)dsl);
			const int nstep = 3;

			if(bElpsed)
			{
				int elp = (int)(mvr.tik - prvTik);
				if(elp < 0)
					elp = 10;
				double slp = (elp / 10000) / _multiSpeed;
				slp -= (nstep * 50); ///ani frame간격의 최소가 50 이므로
				slp = slp < 0 ? 0 : slp;
				Sleep((int)slp);//elp = 6000/0000
				prvTik = mvr.tik;
			}
			for(int a = 0; a < _aniStep; a++)
			{
				if(a < (nstep - 1))
				{
					Sliding(mvr.dr, mv.l);
				}
				else
				{
					MoveFinish(mvr, 0);
					//switch(rdr)
					//{
					//case eLF: lf(mv, 0); _hor[mv.l] = 0; break; /// 드디어 이동
					//case eRT: rt(mv, 0); _hor[mv.l] = 0; break;
					//case eUP: up(mv, 0); _ver[mv.l] = 0; break;
					//case eDN: dn(mv, 0); _ver[mv.l] = 0; break;
					//}
				}
				/// fnc 하는 사이에 Reset 누르면 _mix.clear, _count 되는거 명심 해라.
				//KMove mvt; mvt.dr = rdr; mvt.l = mv.l; mvt.n = mv.n; //mvt.tik = mv.tik;
				//auto mvl = mv.l; // 위에서 KMove mv = mx.back(); 복사 해서 또 할필요 없다.
				if(fnc(mvr, a, i) == 0)
				{
					/// 이미 Reset이 되어서, _mix, mx, mv 등 값이 다 날라 갔다.
					/// tilt를 원위치 시키고 멈춰야지.
					SlidingInit(mvr.dr, mvr.l);
					//switch(rdr)
					//{
					//case 0: _hor[mvt.l] = 0; break;/// 원위치
					//case 1: _hor[mvt.l] = 0; break;
					//case 2: _ver[mvt.l] = 0; break;
					//case 3: _ver[mvt.l] = 0; break;
					//}
					bBreak = 1;/// Reset 게임 정지 =
					break;
				}
			}
			if(bBreak)
				break;
			//TRACE("Reverse: %d, %d, %d\n", rdr, mv.l, mv.n);
			if(bReverse)
				lstMove.pop_back();
			else
				lstMove.pop_front();
		}
	}


	void Reset();
	bool Check();
	double CheckEx();
	void MoveUnit();
	int Narrow();
	void Mix(int n);
	void Mix(PWS sInit);
	// 	void lf(int r, int n, int bPush = 1);
// 	void rt(int r, int n, int bPush = 1);
// 	void up(int c, int n, int bPush = 1);
// 	void dn(int c, int n, int bPush = 1);

	void lf(KMove mv, int bPush = 1);
	void rt(KMove mv, int bPush = 1);
	void up(KMove mv, int bPush = 1);
	void dn(KMove mv, int bPush = 1);
	void InitCellRects();
};

