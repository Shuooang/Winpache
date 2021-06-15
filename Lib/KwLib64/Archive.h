#pragma once

#include "inlinefnc.h"

class CKArchive
{
public:
	int  m_nMode;
public:
	CKArchive(bool nMode = store)
		: m_nMode(nMode)
	{

	}
public:
	enum Mode { store = 0, load = 1, bNoFlushOnDelete = 2, bNoByteSwap = 4 };

	BOOL IsLoading() const { return m_nMode == load; }
	BOOL IsStoring() const { return m_nMode == store; }
	virtual void Flush() {} // necessary in file
	virtual void Close() {} // necessary in file
	virtual UINT Read(void* lpBuf, UINT nMax) = NULL;
	virtual void Write(const void* lpBuf, UINT nMax) = NULL;


	template<typename T>
	CKArchive& WriteVal(T l)
	{
		Write(&l, sizeof(T));
		return *this;
	}

	template<typename T>
	CKArchive& ReadVal(T& l)
	{
		Read((void*)&l, sizeof(T));
		return *this;
	}
	CKArchive& operator<<(short  l) { return WriteVal(l); }
	CKArchive& operator<<(DWORD  l) { return WriteVal(l); }
	CKArchive& operator<<(WORD   l) { return WriteVal(l); }
	CKArchive& operator<<(int    l) { return WriteVal(l); }
	CKArchive& operator<<(UINT   l) { return WriteVal(l); }
	CKArchive& operator<<(float  l) { return WriteVal(l); }
	CKArchive& operator<<(double l) { return WriteVal(l); }
	CKArchive& operator<<(LONGLONG l) { return WriteVal(l); }
	CKArchive& operator<<(ULONGLONG l) { return WriteVal(l); }

	CKArchive& operator>>(short& l) { return ReadVal(l); }
	CKArchive& operator>>(DWORD& l) { return ReadVal(l); }
	CKArchive& operator>>(WORD& l) { return ReadVal(l); }
	CKArchive& operator>>(int& l) { return ReadVal(l); }
	CKArchive& operator>>(UINT& l) { return ReadVal(l); }
	CKArchive& operator>>(float& l) { return ReadVal(l); }
	CKArchive& operator>>(double& l) { return ReadVal(l); }
	CKArchive& operator>>(LONGLONG& l) { return ReadVal(l); }
	CKArchive& operator>>(ULONGLONG& l) { return ReadVal(l); }


	void WriteCount(DWORD_PTR dwCount);

	DWORD_PTR ReadCount();

	// 	CBufArchive& operator<<(const Osp::Base::DateTime& dt)
	// 	{
	// 		//AppLog("<<DateTime  %04d-%02d-%02d %02d:%02d:%02d (%d)", dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(), dt.GetSecond());
	// 		int ar[6] =
	// 		{
	// 			dt.GetYear(),
	// 			dt.GetMonth(),
	// 			dt.GetDay(),
	// 			dt.GetHour(),
	// 			dt.GetMinute(),
	// 			dt.GetSecond(),
	// 		};
	// 		Write(ar, sizeof(int)*6);
	// 		return *this; 
	// 	}
	// 	CBufArchive& operator>>(Osp::Base::DateTime& dt)
	// 	{
	// 		int ar[6];
	// 		Read(ar, sizeof(int)*6);
	// 		dt.SetValue(ar[0],ar[1],ar[2],ar[3],ar[4],ar[5]);
	// 		//AppLog("<<DateTime  %04d-%02d-%02d %02d:%02d:%02d (%d)", ar[0],ar[1],ar[2],ar[3],ar[4],ar[5]);
	// 		return *this; 
	// 	}
};

class CFileArchive
	: public CKArchive
{
public:
	FILE* m_file;
public:
	CFileArchive(bool nMode = store)
		: CKArchive(nMode)
	{
	}
	// file Open Close는 SetFile을 부른 곳에서 처리 한다.
	void SetFile(FILE* file)
	{
		ASSERT(m_file == NULL);
		m_file = file;
	}
	virtual UINT Read(void* lpBuf, UINT nMax);
	virtual void Write(const void* lpBuf, UINT nMax);
};
class CBufArchive
	: public CKArchive
{
public:
	CBufArchive(bool nMode = store)
		: CKArchive(nMode)
		, m_bUserBuf(true) // memmory or file
		, m_lpBufStart(NULL)
		, m_lpBufCur(NULL)
		, m_nBufSize(0)
		, m_nGrowSize(1024)
	{
	}
	~CBufArchive()
	{
		Free();
	}
	void Free()
	{
		DeleteMeSafe(m_lpBufStart);
		m_lpBufCur = m_lpBufStart = NULL;
		m_nBufSize = 0;
	}
	virtual UINT Read(void* lpBuf, UINT nMax);
	virtual void Write(const void* lpBuf, UINT nMax);

	bool m_bUserBuf;
	UINT m_nBufSize;
	BYTE* m_lpBufCur;
	BYTE* m_lpBufStart;
	// advanced parameters (controls performance with large archives)
	UINT m_nGrowSize;

	// Write와 상관 없이 Alloc(uSize) 한 size
	UINT GetBufSize()
	{
		return m_nBufSize;
	}

	// 실제 Write 한 끝 위치
	INT_PTR GetLength()
	{
		return GetCurPos();
	}

	INT_PTR GetCurPos()
	{
		return (int)(m_lpBufCur - m_lpBufStart);
	}

	void Terminate()
	{
		// 항상 크기가 4byte는 여유 있으므로 뒤에 2byte는 0으로 채운다.
		if(m_lpBufCur)
		{
			m_lpBufCur[0] = '\0';
			m_lpBufCur[1] = '\0';
		}
	}

	BYTE* GetPtr()
	{
		return m_lpBufStart;
	}

	BYTE* Alloc(UINT uSize);

	BYTE* Detach()
	{
		BYTE* p = GetPtr();
		m_lpBufCur = m_lpBufStart = NULL;
		m_nBufSize = 0;
		return p;
	}

};
