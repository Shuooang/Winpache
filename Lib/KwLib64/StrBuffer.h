#pragma once

#include "Lock.h"


template<typename TYPE, typename ARG_TYPE = const TYPE&>
class CStrBufferT
	//: public ISynchronized �̰� ���� �ȵȴ�. �ߺ� ��� �߻� dbsmo.h(29): warning C4584: 'CDbSmo' : 'ISynchronized' �⺻ Ŭ������ �̹� 'CKDBBase'�� �⺻ Ŭ�����Դϴ�.
{
public:
	CStrBufferT(int nmax = 5000)
		: m_nMax(nmax)
	{
	}
	int m_nMax;
	void SetMaxBufCount(int nMax)
	{
		m_nMax = nMax;
	}
//	CS_DEF_MEMBER_M(m_csBuf);
	
	CKCriticalSection m_csBuf;//AUTOLOCK(m_csBuf);
	CList<TYPE, ARG_TYPE> m_arBuf;

	TYPE& GetBuf(LPCSTR fnc = NULL, int line = 0)
	{
		// �̰� �Ⱓ ���̺귯�� �̴� �η����� ó�� ����
		//CSingleLock __synchthis(&m_csBuf, TRUE);//, __FUNCTIONW__, __LINE__, typeid(this).name());
		AUTOLOCK(m_csBuf);
		//InterlockedIncrement(&CStrBuffer::s_nSBuf);

		INT_PTR n = m_arBuf.GetCount();
		if (n >= m_nMax)
			m_arBuf.RemoveHead();

		POSITION pos = m_arBuf.AddTail((ARG_TYPE)L"");
		//if (fnc)
		//	Used(fnc, line);
		return m_arBuf.GetAt(pos);
	}
	void ReleaseBuf()
	{
		m_arBuf.RemoveAll();
	}
	//virtual void Used(PS fnc, int line) {}
};




class CStrBuffer : public CStrBufferT<CString, LPCTSTR>
{
public:
	CStrBuffer(int nmax = 500)
		: CStrBufferT<CString, LPCTSTR>(nmax)
	{
	}
	static CStrBuffer* s_pBuf;
	static CStrBuffer* Global()
	{
		if(s_pBuf == NULL)
			s_pBuf = new CStrBuffer(1000);
		return s_pBuf;
	}
	static CString& GlobalBuf()
	{
		return Global()->GetBuf();
	}
};

class CStrBufferA : public CStrBufferT<CStringA, LPCSTR>
{
public:
	CStrBufferA(int nmax = 5000)
		: CStrBufferT<CStringA, LPCSTR>(nmax)
	{
	}
	static CStrBufferA* s_pBuf;
	static CStrBufferA* Global()
	{
		if(s_pBuf == NULL)
			s_pBuf = new CStrBufferA(1000);
		return s_pBuf;
	}
	static CStringA& GlobalBuf()
	{
		return Global()->GetBuf();
	}
};
