#pragma once


#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif



class CPointF;

template<typename TFloat>
class CSizeFT
{
public:
	CSizeFT()
		: cx(0.f), cy(0.f)
	{
	}
	TFloat cx, cy;

	CSizeFT(const CPointF& point);

	CSizeFT(IN const CSizeFT& point);

	CSizeFT(IN const SIZE& point)
	{
		cx = (TFloat)point.cx;
		cy = (TFloat)point.cy;
	}
	CSizeFT(IN TFloat x1, IN TFloat y1)
		:cx(x1), cy(y1)
	{
	}
	CSizeFT(IN int x1, IN int y1)
		:cx((TFloat)x1), cy((TFloat)y1)
	{
	}

	CSizeFT operator+(IN const CSizeFT& point) const
	{
		return CSizeFT(cx + point.cx, cy + point.cy);
	}
	CSizeFT operator+(IN const CSize& point) const
	{
		return CSizeFT(cx + point.cx, cy + point.cy);
	}
//	CPointF operator+(IN const PointF& point) const;

	bool operator==(const CSizeFT& p) const { return cx == p.cx && cy == p.cy; }
	bool operator!=(const CSizeFT& p) const { return cx != p.cx || cy != p.cy; }

	CSizeFT operator-(IN const CSizeFT& point) const
	{
		return CSizeFT(cx - point.cx, cy - point.cy);
	}

	void operator/=(TFloat dv)
	{
		ASSERT(dv != 0.f);
		cx /= dv, cy /= dv;
	}
	void operator*=(TFloat dv)
	{
		cx *= dv, cy *= dv;
	}
	void operator+=(TFloat dv)
	{
		cx += dv, cy += dv;
	}
	void operator+=(CSizeFT dv)
	{
		cx += dv.cx, cy += dv.cy;
	}

	BOOL Equals(IN const CSizeFT& point)
	{
		return (cx == point.cx) && (cy == point.cy);
	}
	bool IsZero()
	{
		return cx == 0 && cy == 0;
	}

	TFloat Scalar()
	{
		CSizeFT& p0 = *this;
		return sqrt(J2(0 - p0.cx) + J2(0 - p0.cy));
	}
	TFloat Scalar(const CSizeFT& p1) const
	{
		const CSizeFT& p0 = *this;
		return sqrt(J2(p1.cx - p0.cx) + J2(p1.cy - p0.cy));
	}
	void Set(TFloat x0, TFloat y0)
	{
		cx = x0, cy = y0;
	}

};

class CSizeF
	: public CSizeFT<float>
{
public:
	CSizeF(float x = 0.f, float y = 0.f)
		: CSizeFT(x, y)
	{
	}
	CSizeF(IN const CPointF& point);
};

class CSizeD
	: public CSizeFT<double>
{
public:
	CSizeD(double x = 0., double y = 0.)
		: CSizeFT(x, y)
	{
	}
};

template<typename TFloat>
class CPoint3FT
{
public:
	TFloat x, y, z;
	CPoint3FT(IN TFloat x1, IN TFloat y1, IN TFloat z1)
		:x(x1), y(y1), z(z1)
	{
	}
};

class CPoint3F
	: public CPoint3FT<float>
{
public:
	CPoint3F(IN float x1, IN float y1, IN float z1)
		: CPoint3FT(x1, y1, z1)
	{
	}
};

template<typename TFloat>
class CPointFT
{
public:
	TFloat x, y;
	CPointFT()
	{
		x = y = 0.0f;
	}

	CPointFT(IN const CPointFT& point)
	{
		x = point.x;
		y = point.y;
	}
	// 	CPointFT(IN const PointF &point)
	// 	{
	// 		x = point.X;
	// 		y = point.Y;
	// 	}
	CPointFT(IN const POINT& point)
	{
		x = (TFloat)point.x;
		y = (TFloat)point.y;
	}
	CPointFT(IN const POINTF& point)
	{
		x = (TFloat)point.x;
		y = (TFloat)point.y;
	}

	//	CPointFT(IN const SizeF &size)
	//	{
	//		X = size.Width;
	//		Y = size.Height;
	//	}

	CPointFT(IN TFloat x1, IN TFloat y1)
		:x(x1), y(y1)
	{
	}
	CPointFT(IN int x1, IN int y1)
		:x((TFloat)x1), y((TFloat)y1)
	{
	}
	// 	CPointFT(IN double x1, IN double y1)
	// 		:x((TFloat)x1),y((TFloat)y1)
	// 	{
	// 	}

	// 	void Set()
	// 	{
	// 		x = y = (TFloat)0.f;
	// 	}

	CPointFT& operator=(const POINTF& pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	}
	CPointFT& operator=(const CPointFT& pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	}
	CPointFT operator+(IN const CPointFT& point) const
	{
		return CPointFT(x + point.x, y + point.y);
	}
	CPointFT operator+(IN const CPoint& point) const
	{
		return CPointFT(x + point.x, y + point.y);
	}
	CPointFT operator+(IN const CSizeF& point) const
	{
		return CPointFT(x + point.cx, y + point.cy);
	}
	// 	CPointFT operator+(IN const PointF& point) const
	// 	{
	// 		return CPointFT(x + point.X, y + point.Y);
	// 	}
	bool operator==(const CPointFT& p) const { return x == p.x && y == p.y; }
	bool operator!=(const CPointFT& p) const { return x != p.x || y != p.y; }

	CPointFT operator-(IN const CPointFT& point) const
	{
		return CPointFT(x - point.x, y - point.y);
	}
	CPointFT operator-(IN const CSizeF& point) const
	{
		return CPointFT(x - point.cx, y - point.cy);
	}
	CPointFT operator*(IN const float f) const
	{
		return CPointFT(x * f, y * f);
	}
	CPointFT operator*(IN const CPointFT& point) const
	{
		return CPointFT(x * point.x, y * point.y);
	}
	CPointFT operator/(IN const float f) const
	{
		ASSERT(f != 0.f);
		return CPointFT(x / f, y / f);
	}
	CPointFT operator/(IN const CPointFT point) const
	{
		ASSERT(point.x != 0.f);
		ASSERT(point.y != 0.f);
		return CPointFT(x / point.x, y / point.y);
	}

	void operator/=(TFloat dv)
	{
		ASSERT(dv != 0.f);
		x /= dv, y /= dv;
	}
	void operator*=(TFloat dv)
	{
		x *= dv, y *= dv;
	}
	void operator+=(TFloat dv)
	{
		x += dv, y += dv;
	}
	void operator+=(CPointFT dv)
	{
		x += dv.x, y += dv.y;
	}
	void operator+=(CSizeF dv)
	{
		x += dv.cx, y += dv.cy;
	}
	void operator+=(CSize dv)
	{
		x += dv.cx, y += dv.cy;
	}
	void operator-=(CSizeF dv)
	{
		x -= dv.cx, y -= dv.cy;
	}

	BOOL Equals(IN const CPointFT& point)
	{
		return (x == point.x) && (y == point.y);
	}
	bool IsZero()
	{
		return x == 0 && y == 0;
	}

	TFloat Scalar()
	{
		CPointFT& p0 = *this;
		return sqrt(J2(0 - p0.x) + J2(0 - p0.y));
	}
	TFloat Scalar(const CPointFT& p1) const
	{
		const CPointFT& p0 = *this;
		return sqrt(J2(p1.x - p0.x) + J2(p1.y - p0.y));
	}
	void Set(TFloat x0, TFloat y0)
	{
		x = x0, y = y0;
	}

};

class CPointF
	: public CPointFT<float>
{
public:
	CPointF(float x = 0.f, float y = 0.f)
		: CPointFT(x, y)
	{
	}
	CPointF(const CPoint& pt)
		: CPointFT((float)pt.x, (float)pt.y)
	{
	}

	CPointF& operator=(const POINTF& pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	}
	CPointF& operator=(const CPointF& pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	}
};

class CPointD
	: public CPointFT<double>
{
public:
	CPointD(double x = 0., double y = 0.)
		: CPointFT(x, y)
	{
	}
	CPointD(const CPoint& pt)
		: CPointFT((double)pt.x, (double)pt.y)
	{
	}
	CPointD(const POINT& pt)
		: CPointFT((double)pt.x, (double)pt.y)
	{
	}
};

inline CSizeF::CSizeF(IN const CPointF& point)
	: CSizeFT(point.x, point.y)
{
}

template<typename TFloat>
class CRectFT
{
public:
	CRectFT()
		:l(0.f), t(0.f), r(0.f), b(0.f)
	{
	}
	CRectFT(TFloat l0, TFloat t0, TFloat r0, TFloat b0)
		:l(l0), t(t0), r(r0), b(b0)
	{
		;
	}

	CRectFT(IN const CRectFT& point)
		:l(point.l), t(point.t), r(point.r), b(point.b)
	{
	}
	CRectFT(IN const RECT& point)
		:l((TFloat)point.left), t((TFloat)point.top), r((TFloat)point.right), b((TFloat)point.bottom)
	{
	}
	CRectFT(CPointFT<TFloat> pt, CSizeFT<TFloat> sz)
		:l((TFloat)pt.x), t((TFloat)pt.y), r((TFloat)l + sz.cx), b((TFloat)t + sz.cy)
	{
	}

	bool IsRectEmpty()
	{
		return l == r || t == b;
	}

	// 	CRectFT(IN TFloat x1, IN TFloat y1)
	// 		:x(x1),y(y1)
	// 	{
	// 	}
	// 	CRectFT(IN int x1, IN int y1)
	// 		:x((TFloat)x1),y((TFloat)y1)
	// 	{
	// 	}

	CRectFT operator+(IN const CRectFT& rc) const
	{
		return CRectFT(
			l + rc.l,
			t + rc.t,
			r + rc.r,
			b + rc.b);
	}
	bool operator==(const CRectFT& p) const { return l == p.l && t == p.t && r == p.r && b == p.b; }
	bool operator!=(const CRectFT& p) const { return l != p.l || t != p.t || r != p.r || b != p.b; }

	// 	CRectFT operator-(IN const CPointF& point) const
	// 	{
	// 		return CPointF(x - point.x, y - point.y);
	// 	}


	BOOL Equals(IN const CRectFT& rc)
	{
		return (*this == rc);
	}
	bool IsZero()
	{
		return l == 0 && t == 0 && r == 0 && b == 0;
	}

	void Set(TFloat l0, TFloat t0, TFloat r0, TFloat b0)
	{
		l = l0, t = t0, r = r0, b = b0;
	}

	// x, y, w, h : 좌표일때는 h0는 -로 줘야 한다.
	void SetRc(TFloat l0, TFloat t0, TFloat w0, TFloat h0)
	{
		l = l0, t = t0, r = l0 + w0, b = t0 + h0;
	}

	TFloat Width()
	{
		return fabs(r - l);
	}
	TFloat Height()
	{
		return fabs(b - t);
	}

	CPointF Center()
	{
		return CPointF((l + r) / 2, (t + b) / 2);
	}
	CPointF RightBottom()
	{
		return CPointF(r, b);
	}
	CPointF LeftTop()
	{
		return CPointF(l, t);
	}



	bool PtInRect(CPointF pt)
	{
		return (IsBetween(l, r, pt.x) && IsBetween(t, b, pt.y));
	}
	bool PtOverRect(CPointF pt, bool bInclusive = true)
	{
		if (bInclusive)
			return (IsNotIn(l, r, pt.x) || IsNotIn(t, b, pt.y));
		else
			return (IsOver(l, r, pt.x) || IsOver(t, b, pt.y));
	}

public:
	TFloat l, t, r, b;
};

class CRectF
	: public CRectFT<float>
{
public:
	CRectF(float l0 = 0.f, float t0 = 0.f, float r0 = 0.f, float b0 = 0.f)
		: CRectFT(l0, t0, r0, b0)
	{
	}
};
class CRectD
	: public CRectFT<double>
{
public:
	CRectD(double l0 = 0, double t0 = 0, double r0 = 0, double b0 = 0)
		: CRectFT(l0, t0, r0, b0)
	{
	}
	CRectD(CPointD pt, CSizeD sz)
		:CRectFT(pt, sz)
	{
	}
};



template<typename TFloat>
inline CSizeFT<TFloat>::CSizeFT(IN const CPointF& point)
{
	cx = (TFloat)point.x;
	cy = (TFloat)point.y;
}
template<typename TFloat>
inline CSizeFT<TFloat>::CSizeFT(IN const CSizeFT& point)
{
	cx = (TFloat)point.cx;
	cy = (TFloat)point.cy;
}
/*
template<typename TFloat>
inline CSizeFT<TFloat>::operator+(IN const PointF& point) const
{
	return CPointF(x + point.X, y + point.Y);
}
*/

