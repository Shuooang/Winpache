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

	///?주의: 아래걸 안쓰면 CCriticalSection::Lock() 안에서는 
	/// virtual 이 아니므로 내부 걸 써버려서 overrided Lock을 못 부른다.
	virtual BOOL Lock(DWORD dwTimeout)
	{
		ASSERT(dwTimeout == INFINITE);
		(void)dwTimeout;
		return Lock();
	}

	/// 이걸 모든 객체가 override해야 어디서 스레드 lock 걸린줄 알수 있다.
	virtual BOOL Lock()
	{
		::EnterCriticalSection(&m_sect);
		return TRUE;
	}
};



class CSyncAutoLock : public CSingleLock
{
public:
	// CSyncObject 는 CCriticalSection CKCriticalSection이 대중
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
	/// Set 되었을때 상태
#ifdef _DEBUG
	CStringA _note;
#endif // _DEBUG
	/// 0이면 1로 만든다.
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
#define AUTOLOCK(sobj) AUTOLOCKN((sobj), 1) // << 주로 쓴다.
#define AUTOLOCK2(sobj) AUTOLOCKN((sobj), 2)
#define AUTOLOCK3(sobj) AUTOLOCKN((sobj), 3)

// pbj가 _GetCS() 가 있다는 전제로 만든 락 매크로
#define SyncFnc(pbj)     CSingleLock __sync((pbj)->_GetCS(), TRUE)
#define SyncFncN(pbj, n) CSingleLock __sync##n((pbj)->_GetCS(), TRUE)


