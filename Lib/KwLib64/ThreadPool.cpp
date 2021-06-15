#include "pch.h"
#include "ThreadPool.h"
#include "Dictionary.h"
#include "Kw_tool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//CMainPool* CMainPool::s_pPool = NULL; //?thread pool global

std::shared_ptr<CMainPool> CMainPool::s_pPool;
LONG CPushWorker::g_lCurrId = -1;
int CPushWorker::s_dwExecs;
CKRbPtr<LPARAM, CTaskBase> CPushWorker::s_threadStack;
std::function<void(KException*)>* CMainPool::s_pLambdaException{ nullptr };/// not owner 

//FOREGROUND(), BACKGROUND(n)에서 쓰이게
DWORD CMainPool::s_UiThreadId = 0;
//static 
CMainPool* CMainPool::Pool(int nthread)
{
	// 최초 부를땐 0 > nth이고 나중에 쓸때는 0 빈값으로 온다.
	ASSERT(nthread == 0 || !s_pPool);
	if(!s_pPool)
	{
		/// FOREGROUND(), BACKGROUND(n)에서 쓰이게
		// 스레드품 초기화믄 UI스레드에서 초기화 한다는 전제 아래 함.
		s_UiThreadId = ::GetCurrentThreadId();

		s_pPool = std::shared_ptr<CMainPool>(new CMainPool());
		CMainPool* pool = s_pPool.get();
		HRESULT hr = pool->Initialize((void*)AfxGetApp(), nthread); //?order-push 1
		ASSERT(SUCCEEDED(hr));
	}
	return s_pPool.get();
}

BOOL CMainPool::Queue(CTaskBase* pTask, LPARAM pObj, LPARAM lp, LPARAM lp2)
{
	pTask->_this = pObj;
	pTask->m_lp = lp;
	pTask->m_lp2 = lp2;

	// 넣을떄, 런전, 런후 이렇게 3번 부르는데 넣을떄는 없앴다.

	//if(pTask->GetUI() && m_pui)
	//{
	//	//_Format(L"%s", pTask->_GetName());//::GetCurrentThreadId(), 
	//	PWS p = pTask->_GetName();
	//	m_pui->_UiPool(0, (LPARAM)pTask, p);//CTaskBase 가 찍히네...
	//}
	return this->QueueRequest((CPushWorker::RequestType)pTask);
}


/// 현재 pooling 중인 thread중에서 실행 되고 있는 task를 실시간으로 보관 한다. 
/// 그걸 UI에 뿌리면 현재 몇개를 처리중인지 실시간으로 볼수 잇다.
// thread stack에 작업중인 테스크 기록을 자동으로 넣어주고 빼주는 역할
class CAutoThreadCheck : public CAutoIncr //?thread 스택상황을 저장 자동증감
{
public:
	// 정보를 넣고, 사용 스레드 갯수도 자동 증감
	CAutoThreadCheck(int& nDept, bool bAutoInc = true, bool bAutoDec = true, CTaskBase* pTask = NULL)// PWS tag = L"")
		: CAutoIncr(nDept, bAutoInc, bAutoDec)
	{
		CPushWorker::s_threadStack.m_bAutoFree = false;
		CPushWorker::s_threadStack.SetAt(::GetCurrentThreadId(), pTask);
	}
	// dispose 역할: 다시 작업이 끝나서 원상 복귀 작업
	virtual ~CAutoThreadCheck()
	{
		CPushWorker::s_threadStack.RemoveKey(::GetCurrentThreadId());
	}
};

void CPushWorker::Execute(RequestType dw, void* pvParam, OVERLAPPED* pOverlapped) throw()
{
	ATLASSERT(pvParam != NULL);

	CTaskBase* pTask = (CTaskBase*)(DWORD_PTR)dw;
	pTask->_thread = ::GetCurrentThreadId();

	ULONGLONG t1 = GetTickCount64();

	if(pTask->_pool->_fncOnBeforeTask.get())
		(*pTask->_pool->_fncOnBeforeTask)(pTask);

	/// //////////////////////////////////////////////////////////
	pTask->DoTask(pvParam, pOverlapped);
	/// //////////////////////////////////////////////////////////
	
	pTask->_elapsed = GetTickCount64() - t1;
	if(pTask->_pool->_fncOnAfterTask.get())
		(*pTask->_pool->_fncOnAfterTask)(pTask);//_GetExtra(), _elapsed 등 을 이용 하여 UI에 보일거 보인다.

	pTask->DeleteMe();
}


PWS CTaskBase::_GetName() /*화면에 task 별 걸린 시간 祺굡?'CTask????'에서 리턴 */
{
	return (PWS)_name;
}

void CTaskLambdaEx::DoTask(void* pvParam, OVERLAPPED* pOverlapped)
{
	try
	{
		(*m_pLambda)();
	}
	catch(KException* e)
	{	
		if(_pool->_fncOnException.get())
		{
			TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTION__, __LINE__);
			e->m_strStateNativeOrigin = "CTaskLambdaEx";
			(*_pool->_fncOnException)(e);
			//(*CMainPool::s_pLambdaException)(e);
		}
	}
	catch(CException* e)
	{
		if(_pool->_fncOnException.get())//CMainPool::s_pLambdaException)
		{
			auto buf = new TCHAR(1024);
			KAtEnd d_buf([&]() { delete buf; });
			e->GetErrorMessage(buf, 1000);
			TRACE(L"CException:%s - %s %d\n", buf, __FUNCTIONW__, __LINE__);
			auto ek = new KException("CException", GetLastError(), 0, buf, NULL, __FUNCTION__, __LINE__); 
			ek->m_strStateNativeOrigin = "CTaskLambdaEx";
			(*_pool->_fncOnException)(ek);
		}
		else
			TRACE(L"CException: - %s %d\n", __FUNCTIONW__, __LINE__);
	}
	catch(std::exception& e)
	{
		if(_pool->_fncOnException.get())//CMainPool::s_pLambdaException)
		{
			TRACE("catch std::exception %s  - %s %d\n", e.what(), __FUNCTION__, __LINE__); 
			auto ek = new KException("std::exception", -1, 0, CStringW(e.what()), NULL, __FUNCTION__, __LINE__); 
			(*_pool->_fncOnException)(ek);
		}
		else
			TRACE(L"catch std::exception: - %s %d \n", __FUNCTIONW__, __LINE__);
	}
	catch(PWS e)
	{	
		if(_pool->_fncOnException.get())//CMainPool::s_pLambdaException)
		{
			TRACE("catch LPCWSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__); 
			auto ek = new KException("LPCWSTR", GetLastError(), 0, e, NULL, __FUNCTION__, __LINE__);
			ek->m_strStateNativeOrigin = "CTaskLambdaEx";
			(*_pool->_fncOnException)(ek);
		}
		else
			TRACE(L"catch LPCWSTR: %s - %s %d\n", e, __FUNCTIONW__, __LINE__);
	}
	catch(PAS e)
	{	
		if(_pool->_fncOnException.get())//CMainPool::s_pLambdaException)
		{
			TRACE("catch LPCSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__); 
			auto ek = new KException("LPCSTR", GetLastError(), 0, CStringW(e), NULL, __FUNCTION__, __LINE__);
			ek->m_strStateNativeOrigin = "CTaskLambdaEx";
			(*_pool->_fncOnException)(ek);
		}
		else
			TRACE("catch LPCWSTR: %s - %s %d\n", e, __FUNCTION__, __LINE__);
	}
	catch(int ln)
	{
		if(_pool->_fncOnException.get())//CMainPool::s_pLambdaException)
		{
			TRACE("catch ...  - %s catch(%d)\n", __FUNCTION__, ln);
			auto ek = new KException("Unknown", GetLastError(), 0, L"catch(int) Error.", NULL, __FUNCTION__, __LINE__);
			ek->m_strStateNativeOrigin = "CTaskLambdaEx";
			(*_pool->_fncOnException)(ek);
		}
		else
			TRACE("catch ...: - %s %d\n", __FUNCTION__, __LINE__);
	}
	catch(...)
	{
		if(_pool->_fncOnException.get())//CMainPool::s_pLambdaException)
		{
			TRACE("catch ...  - %s %d\n", __FUNCTION__, __LINE__);
			auto ek = new KException("Unknown", GetLastError(), 0, L"Unknown catch(...) Error.", NULL, __FUNCTION__, __LINE__);
			ek->m_strStateNativeOrigin = "CTaskLambdaEx";
			(*_pool->_fncOnException)(ek);
		}
		else
			TRACE("catch ...: - %s %d\n", __FUNCTION__, __LINE__);
	}

}
