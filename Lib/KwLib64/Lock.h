#pragma once
#include <afxmt.h>


class CKCriticalSection : public CCriticalSection
{
public:
	CKCriticalSection()
		: m_nLocked(0)
	{
	}
	LONG m_nLocked;
	void Increase() { InterlockedIncrement(&m_nLocked); }
	void Decrease() { InterlockedDecrement(&m_nLocked); }

	///?����: �Ʒ��� �Ⱦ��� CCriticalSection::Lock() �ȿ����� 
	/// virtual �� �ƴϹǷ� ���� �� ������� overrided Lock�� �� �θ���.
	virtual BOOL Lock(DWORD dwTimeout)
	{
		ASSERT(dwTimeout == INFINITE);
		(void)dwTimeout;
		return Lock();
	}

	/// �̰� ��� ��ü�� override�ؾ� ��� ������ lock �ɸ��� �˼� �ִ�.
	virtual BOOL Lock()
	{
		::EnterCriticalSection(&m_sect);
		return TRUE;
	}
};



class CSyncAutoLock : public CSingleLock
{
public:
	// CSyncObject �� CCriticalSection CKCriticalSection�� ����
	explicit CSyncAutoLock(CSyncObject* pObject, BOOL bInitialLock = TRUE, LPCSTR sFile=NULL, int iLine = 0, LPCSTR sobj = NULL);
	virtual ~CSyncAutoLock();

	UINT64 m_token;
	ULONGLONG m_ull;
	CTime m_tLocked;
	CString m_sObj;
	CStringA m_sFile;
	int m_iLine;

	virtual BOOL Lock(DWORD dwTimeOut = INFINITE);
	virtual BOOL Unlock();
	virtual BOOL Unlock(LONG lCount, LPLONG lPrevCount = NULL);
	virtual BOOL IsLocked();
};
inline BOOL(::CSyncAutoLock::IsLocked())
{
	return m_bAcquired;
}


class KSyncBool
{
public:
	ULONG _val{0};
	/// Set �Ǿ����� ����
#ifdef _DEBUG
	CStringA _note;
#endif // _DEBUG
	/// 0�̸� 1�� �����.
	void Set(LPCSTR note = nullptr)
	{
		InterlockedCompareExchange(&_val, 1, 0);
#ifdef _DEBUG
		if(note)
			_note = note;
#endif // _DEBUG
	}
	void Reset()
	{
		InterlockedCompareExchange(&_val, 0, 1);
	}
	explicit operator bool() const noexcept {
		return _val != 0;
	}
};




#define AUTOLOCKN(sobj, n) CSyncAutoLock __lock##n(&(sobj), TRUE, __FUNCTION__, __LINE__, #sobj)
#define AUTOLOCK(sobj) AUTOLOCKN((sobj), 1) // << �ַ� ����.
#define AUTOLOCK2(sobj) AUTOLOCKN((sobj), 2)
#define AUTOLOCK3(sobj) AUTOLOCKN((sobj), 3)

// pbj�� _GetCS() �� �ִٴ� ������ ���� �� ��ũ��
#define SyncFnc(pbj)     CSingleLock __sync((pbj)->_GetCS(), TRUE)
#define SyncFncN(pbj, n) CSingleLock __sync##n((pbj)->_GetCS(), TRUE)


