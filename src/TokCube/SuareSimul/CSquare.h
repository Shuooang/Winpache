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
	int dr{-1};// l,r,u,d ����
	int l{-1};//r or c : �� �Ǵ� ��
	int n{0};// 1,2,3 ������ ĭ�� 
	LONGLONG tik{0};// mix���� 0
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
	/// 2ĭ�̸� 2�� ���� ������ ���� ����.
	int _left[eCbR][eCbC]{0,};
	int _top[eCbR][eCbC]{0,};

	/// width and height
	CSize _ltMargin{10, 10};
	CSize _rbMargin{10, 10};

	CSize _size{50, 50};

	/// current tilt stat
	int _hor[eCbR]{0,0,0,0,0,0};// if 1. then next box location, 
	int _ver[eCbC]{0,0,0,0,0,0,0,0,0};// -3, -2, -1, 0, 1, 2, 3, 4

	/// ������ ����
	std::list<KMove> _mix;
	CKCriticalSection _csMix;
	std::list<KMove> _mixOnly;
	std::list<KMove> _moveQ;
	CKCriticalSection _csMove;
	
	LONGLONG _tikStart{0};
	/// ������ �׼� ���� �ɸ� �ð�
	LONGLONG _tElapsed{0};
	/// ��ĭ�� �̵�?
	int _unit{3};
	CString _mode{L"axis"};// or 'free'

	/// ������ Ƚ��
	int _count{0};

	/// color set 0,1
	int _setCl{0};

	/// 1, 4, 7, ó�� 3ĭ�� ������ �ܽ��� �Ǿ �������� �ʴ� ���� ���̳� ��.
	bool IsStaticLine(int i)
	{
		//������ �϶� ��� ����
		return //_unit == 3 && 
			(i % 3) == 1;
	}

	/// ���������� ��
	KMove _lastMv;
	/// replay �� �ӵ�. ����
	int _multiSpeed{1};
	/// animaation frame ��.
	int _nFramme{5};// 5;

	/// <summary>
	/// �ִϸ��̼��� �������� ������
	/// </summary>
	/// <param name="rdr">����</param>
	/// <param name="mvl">���° �� �Ǵ� ��</param>
	void Sliding(int rdr, int mvl)//KMove& mv)
	{
		switch(rdr)
		{
		case eLF: _hor[mvl]--; break;/// ���ݾ� �̵�
		case eRT: _hor[mvl]++; break;
		case eUP: _ver[mvl]--; break;
		case eDN: _ver[mvl]++; break;
		}
	}
	/// <summary>
	/// �ִϸ��̼ǿ� ������ �ʱ�ȭ
	/// </summary>
	/// <param name="rdr">����</param>
	/// <param name="mvl">���° �� �Ǵ� ��</param>
	void SlidingInit(int rdr, int mvl)//KMove& mv)
	{
		switch(rdr)
		{
		case eLF: _hor[mvl] = 0; break;/// ����ġ
		case eRT: _hor[mvl] = 0; break;
		case eUP: _ver[mvl] = 0; break;
		case eDN: _ver[mvl] = 0; break;
		}
	}
	/// <summary>
	/// �ִϸ��̼ǿ� ������ �ʱ�ȭ
	/// </summary>
	/// <param name="mv">������ ����</param>
	/// <param name="bPush">_mix�� �߰� ����</param>
	void MoveFinish(KMove& mv, int bPush)
	{
		switch(mv.dr)
		{
		case eLF: lf(mv, bPush); _hor[mv.l] = 0; break; /// ���� �̵�
		case eRT: rt(mv, bPush); _hor[mv.l] = 0; break;
		case eUP: up(mv, bPush); _ver[mv.l] = 0; break;
		case eDN: dn(mv, bPush); _ver[mv.l] = 0; break;
		}
	}

	template <typename TLB>
	void MoveAny(std::list<KMove>& mx, TLB fnc)
	{
		//int n = mx.size();
		int nFramme = _nFramme - (_unit == 1 ? 2 : 0);// 1ĭ �̵����� �ִϸ� 2�� ���Ѵ�.
		for(int i = 0; ; i++)
		{
			KMove mv;
			{
				AUTOLOCK(_csMove);
				mv = mx.front();///�����ϸ� �����ؼ� ���. ��׶��� �۾��̴�.
				mx.pop_front();//�ٷ�����
			}
			for(int a = 0; a < nFramme; a++)///�ִϸ��̼� ������ 5��
			{
				if(a < (nFramme - 1))
				{
					Sliding(mv.dr, mv.l);
					/// �̵�ĭ�� ���� ���� Sleep ª��
					Sleep(30);// _unit * 10);//���� ���ƾ�. ������ �����϶��� ���� ����
				}
				else
				{
					MoveFinish(mv, 1);
				}
				if(mx.size() == 0)/// �� ���̿� �� �׼��� �������� �ʾ����� ȭ���� �ٽ� �׸���.
					fnc(mv.dr, mv.l, a); // invaludateRect ���׶���� �뺸
				else if(i < (nFramme - 2))/// 4�̸� �������̰�. �׻��̿� ������ �׼��� ������ ������ �������� �׸��°� ���� �Ѵ�.
					i = (nFramme - 2);/// 3�̸� 4(������)�� �ɲ���, 2�����̸� 3���� �� �ϸ� ���� ++ ���� 4�� �ǹǷ� �ǳ� ���.
			}
			_lastMv = mv;;
			if(mx.size() == 0)
				break;/// �� ���̿� �� �׼��� �������� �ʾ����� ������. �� �̵� ���۰� �����Ƿ�
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
				mv = lstMove.back();//����
			else
				mv = lstMove.front();

			KMove mvr = mv;
			if(bReverse)//�ݴ����
				mvr.dr =
				mv.dr == eLF ? eRT :
				mv.dr == eUP ? eDN :
				mv.dr == eRT ? eLF :
				mv.dr == eDN ? eUP : -1;
			else
				mvr.dr = mv.dr;//���÷��� ������
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
				slp -= (nstep * 50); ///ani frame������ �ּҰ� 50 �̹Ƿ�
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
					//case eLF: lf(mv, 0); _hor[mv.l] = 0; break; /// ���� �̵�
					//case eRT: rt(mv, 0); _hor[mv.l] = 0; break;
					//case eUP: up(mv, 0); _ver[mv.l] = 0; break;
					//case eDN: dn(mv, 0); _ver[mv.l] = 0; break;
					//}
				}
				/// fnc �ϴ� ���̿� Reset ������ _mix.clear, _count �Ǵ°� ��� �ض�.
				//KMove mvt; mvt.dr = rdr; mvt.l = mv.l; mvt.n = mv.n; //mvt.tik = mv.tik;
				//auto mvl = mv.l; // ������ KMove mv = mx.back(); ���� �ؼ� �� ���ʿ� ����.
				if(fnc(mvr, a, i) == 0)
				{
					/// �̹� Reset�� �Ǿ, _mix, mx, mv �� ���� �� ���� ����.
					/// tilt�� ����ġ ��Ű�� �������.
					SlidingInit(mvr.dr, mvr.l);
					//switch(rdr)
					//{
					//case 0: _hor[mvt.l] = 0; break;/// ����ġ
					//case 1: _hor[mvt.l] = 0; break;
					//case 2: _ver[mvt.l] = 0; break;
					//case 3: _ver[mvt.l] = 0; break;
					//}
					bBreak = 1;/// Reset ���� ���� =
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

