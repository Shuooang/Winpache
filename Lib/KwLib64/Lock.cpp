#include "pch.h"
#include "Lock.h"



CSyncAutoLock::CSyncAutoLock(CSyncObject* pObject, BOOL bInitialLock,
	LPCSTR sFile, int iLine, LPCSTR sobj)
	: CSingleLock(pObject, FALSE)//FALSE �̹Ƿ� ���⼭ Lock���Ѵ�.
	, m_sFile(sFile)
	, m_iLine(iLine)
	, m_sObj(sobj)
{

	CKCriticalSection* pCS = (CKCriticalSection*)m_pObject;
	CRITICAL_SECTION& sect = pCS->m_sect;
	long lc = sect.LockCount;// -1 => lock -2
	long rc = sect.RecursionCount;// 0,1,2 ��� ���� �Ѵ�.
	HANDLE pth = sect.OwningThread;// null -> lock thread id
	if(bInitialLock)
		Lock();//m_nLocked++
	m_ull = GetTickCount64();

}

CSyncAutoLock::~CSyncAutoLock()
{

	ULONGLONG ull = GetTickCount64();
	DWORD usp = (DWORD)(ull - m_ull);
	if(usp > 100)
	{
		// 		KTrace(L"Unlock\t%u\t%s %s(%d)\n", usp, m_sObj, m_sFile, m_iLine);
	}
	CKCriticalSection* pObject = (CKCriticalSection*)m_pObject;
	pObject->Decrease();//m_nLocked--;
}


BOOL CSyncAutoLock::Lock(DWORD dwTimeOut /*= INFINITE*/)
{
	CKCriticalSection* pCS = (CKCriticalSection*)m_pObject;
	CRITICAL_SECTION& sect = pCS->m_sect;
	long lc = sect.LockCount;// -1 => lock -2
	long rc = sect.RecursionCount;// 0,1,2 ��� ���� �Ѵ�.
	void* pth = sect.OwningThread;// null -> lock thread id
	pCS->Increase(); //m_nLocked++;
	// 	BOOL b = __super::Lock(dwTimeOut);
	// 	return b;

	ASSERT(m_pObject != NULL || m_hObject != NULL);
	ASSERT(!m_bAcquired);

	//	m_bAcquired = m_pObject->Lock(dwTimeOut);
	///	m_bAcquired = ((CCriticalSection*)pCS)->Lock(dwTimeOut); �̷��� �ϸ� CCriticalSection::Lock()���� overrided Lock�� �� �θ���.
	m_bAcquired = pCS->Lock(dwTimeOut);
	return m_bAcquired;
}


BOOL CSyncAutoLock::Unlock()
{
	ASSERT(m_pObject != NULL);
	if(m_bAcquired)
		m_bAcquired = !m_pObject->Unlock();

	// successfully unlocking means it isn't acquired
	return !m_bAcquired;
}

BOOL CSyncAutoLock::Unlock(LONG lCount, LPLONG lpPrevCount /* = NULL */)
{
	ASSERT(m_pObject != NULL);
	if(m_bAcquired)
		m_bAcquired = !m_pObject->Unlock(lCount, lpPrevCount);

	// successfully unlocking means it isn't acquired
	return !m_bAcquired;
}



