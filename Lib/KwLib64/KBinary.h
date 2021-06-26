#pragma once

#include "inlinefnc.h"
class KBinary
{
public:

	~KBinary();
	KBinary(LPCWSTR ps)//, size_t len = -1, int iVer = 0) 
	{
		Set((LPWSTR)ps);//len, 
	}
	KBinary(CStringA& sa)//, size_t len = -1, int iVer = 0) 
	{
		Set((char*)(PAS)sa);//len, 
	}

	// �״�� ���� �ϴ��� �Լ� �Ķ� ���ͷ� �Ѿ� ������ �θ����� �������� ������ �־�� �Ѵ�.
	// �׷��� ������ ���� �� �������� ������ �ǰ� �Ҹ� �Լ��� �����Ҷ� free �ع�����. �׷��� �θ��ʿ����� ���� ���� �ȴ�.
	KBinary(const KBinary& bin)
	{
		Clone(&bin);
	}

	// �׳� �Ҵ縸 �ҋ�
	KBinary(UINT_PTR len = 0, int iVer = 0)
		: m_bOwner(true)
		, m_len(len)
	{
		m_p = Alloc(len);
	}

	bool m_bOwner{true};
	char* m_p{NULL};//���ڿ��ƴϴ�. ���� ������ WCHAR�ϼ��� CHAR�ϼ��� �ִ�.
	// byte�� ���ڿ����̰� �ƴϴ�.(UNICODE�ΰ�� �޶���)
	//DWORD 
	UINT_PTR m_len{0};
	UINT_PTR m_capa{0};
	UINT m_multipleSize{8};

	UINT_PTR Size() { return m_len; }
	UINT_PTR Capa() { return m_capa; }

private:
	CHAR* NewAlloc(UINT_PTR len);

public:	
// 	void Set(LPCWSTR ps);
// 	void Set(LPCWSTR ps, int iVer);
	template<typename TPSTR>
	void Set(TPSTR ps, int iVer)
	{
		Set(ps);
		SetVer(iVer);
	}

	template<typename Tchar>
	void Set(Tchar* ps)
	{
		ASSERT(m_bOwner);
		ASSERT(m_len >= 0);
		//	ASSERT(len < 100000000); //100�ް� ¥���� �ֳ�? �ִٸ� �׶� ����
		size_t l0 = ps == NULL ? 0 : tchlen(ps);

		UINT_PTR len = (DWORD)((l0 == 0) ? 0 : (l0 + 1) * sizeof(Tchar));


		// ���̰� ���ų�, 
		if(len == m_len || (m_len > len && (m_len <= 100 || (m_len - len) <= 20)))
		{
		}
		else
		{
			if(m_len > 0)
				Free();
			ASSERT(len >= 0);
			m_p = Alloc(len);//new CHAR[m_len];//		memcpy(m_p, ps, m_len);
		}

		if(len > 0)
			tchcpy((Tchar*)m_p, ps);//(TCHAR)'\0' ���� ����
		else if(m_len > 0)
			*((Tchar*)m_p) = (Tchar)0;

		m_len = len;
	}
	
	
	
	
	
	
	operator LPCTSTR() const { return (LPCTSTR)m_p; }

	void Free();

	CHAR* Detach(UINT_PTR* plen = nullptr)
	{
		m_bOwner = false;
		if (plen)
			*plen = m_len;
		return m_p;
	}

	  //����
  // 	KBinary binr;
  // 	pak->m_binr.Detach();//����������
  // 	binr.Attach(pak->m_binr.m_p, pak->m_binr.m_len);//����������
	  void Attach(KBinary& bin)
	  {
		  //ASSERT(bin.m_bOwner == false); // bin.Detach() ���� �� �� �־� ���...
		  Free();

		  m_p = bin.m_p;
		  m_len = bin.m_len;
		  m_capa = bin.m_capa;
		  if(m_len > 0)
			  m_bOwner = true;

		  bin.m_bOwner = false;//�̰� ������ �� ������� ���ϸ� �ɰ��� �޸� ����...
		  bin.Free();
	  }

	  void Attach(LPSTR p, UINT_PTR len)
	  {
		  ASSERT(p);
		  ASSERT(len >= 0);
		  Free();

		  if(len > 0)
		  {
			  m_p = p;
			  m_len = len;
			  m_capa = len;
			  m_bOwner = true;
		  }
	  }
	  CHAR* Alloc(UINT_PTR len, bool bZeroFill = false, int valset = 0);

	  // iVer = -1 �̵� �ƴϵ� ������ �� �ִ´�.
	  void SetPtr(LPCSTR ps, INT_PTR len, int iVer = -1);

	  void Wrap(const KBinary& bin);

	  // owner ���� ������ ���, Free���� �ʴ´�.
	  void Wrap(LPCSTR ps, UINT_PTR len, int iVer = -1);


	  // ���� �Ҵ�� memory�� Set character �Ѵ�.
	  void SetCh(int idx, char ch) // �ݵ�� char�̾�� �Ѵ�. m_p�� byte ���� �̹Ƿ�
	  {
		  ASSERT(idx < (int)m_len-1);
		  m_p[idx] = ch;
	  }

	  template<typename TYPECHAR>
	  bool CheckNullTermT(TYPECHAR)
	  {
		  typedef TYPECHAR* LTYPESTR;
		  if(m_len == 0)
			  return true;
		  LTYPESTR pt = (LTYPESTR)m_p;
		  for(DWORD i=0;i<= (m_len/sizeof(TYPECHAR));i++, pt++) //?����: <= m_len �� ���� �Ҵ��� '\0' ��ŭ �� �Ҵ� �Ǿ��־�� �Ѵ�.
		  {
			  if(*pt == '\0')
				  return true;
		  }
		  return false;
	  }

	  bool CheckNullTerm();
	  bool CheckNullTermA();
	  bool CheckNullTermW();

	  CString& GetStr(CString& sBuf);

	  LPCSTR GetPtr(UINT_PTR& len)
	  {
		  len = m_len;
		  return (LPCSTR)m_p;
	  }

	  CString GetS()
	  {
		  CString s = (LPCTSTR)m_p;
		  return s;
	  }
	  LPCTSTR GetP()
	  {
		  ASSERT(CheckNullTerm());
		  return (LPCTSTR)m_p;
	  }
	  LPCSTR GetPA()
	  {
		  ASSERT(CheckNullTermA());
		  return (LPCSTR)m_p;
	  }
	  LPCWSTR GetPW()
	  {
		  ASSERT(CheckNullTermW());
		  return (LPCWSTR)m_p;
	  }

	  ULONGLONG GetUL();
	  __int64 GetI64();
	  DWORD GetDW();
	  UINT GetU();
	  int GetI();
	  int GetId(int idef = 0);
	  double GetD();
	  CTime GetT();

	  void _SerializeVer(CArchive& arc, int iOp = 0);
	  void Clone(const KBinary* pSrc0, int iOp = 0);
	  bool IsSame(KBinary* pSrc, int iOp = 0);
	  CHAR* ReAlloc(UINT_PTR len);
	  CHAR* Resize(UINT_PTR len, bool bZeroFill = false);

	  int m_version;
	  virtual int _Upgrade()//int verInit = -1)
	  {
		  return ++m_version;
	  }
	  // ���� ������ ���� ������ ���ų� ������ ��� �װͰ� ���� ���ش�.
	  void SetVer(int verUp)
	  {
		  m_version = verUp;
	  }

};

#define MUTIPLELEN(len, mul)  ((len) % mul == 0 ? ((len)/mul)*mul : (((len)/mul)+1)*mul)
#define MUTIPLELEN8(len)  MUTIPLELEN(len, 8)


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
	// file Open Close�� SetFile�� �θ� ������ ó�� �Ѵ�.
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
		//, m_nGrowSize(1024)
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
	UINT_PTR m_nBufSize;
	BYTE* m_lpBufCur;
	BYTE* m_lpBufStart;
	// advanced parameters (controls performance with large archives)
	//UINT m_nGrowSize; 2�辿 �Ҿ� ������ ���� 2020-10-25

	// Write�� ��� ���� Alloc(uSize) �� size
	UINT_PTR GetBufSize()
	{
		return m_nBufSize;
	}

	// ���� Write �� �� ��ġ
	UINT_PTR GetLength()
	{
		return GetCurPos();
	}

	UINT_PTR GetCurPos()
	{
		return (int)(m_lpBufCur - m_lpBufStart);
	}

	void Terminate()
	{
		// �׻� ũ�Ⱑ 4byte�� ���� �����Ƿ� �ڿ� 2byte�� 0���� ä���.
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

	BYTE* Alloc(UINT_PTR uSize);

	BYTE* Detach()
	{
		BYTE* p = GetPtr();
		m_lpBufCur = m_lpBufStart = NULL;
		m_nBufSize = 0;
		return p;
	}

};