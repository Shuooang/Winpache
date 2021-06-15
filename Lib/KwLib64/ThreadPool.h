#pragma once
#include <atlutil.h>//for CThreadPool

#include <functional>//for shared_ptr

#include "ktypedef.h"
#include "Dictionary.h"//for CKRbVal
#include "SrvException.h"

class CMainPool;

#ifdef _DEBUG
#define HDDER_NEW DEBUG_NEW
#else
#define HDDER_NEW new
#endif // _DEBUG

class CTaskBase
{
public:
	CTaskBase()
		: _pool(NULL)
		, _elapsed(0)
		, _thread(0)
		, m_bAutoFree(true)
		, m_bKeepRunning(FALSE)
		//, m_bUIx(true)
		, m_line(-1)
		, _this(NULL)
		, m_lp(NULL)
		, m_lp2(NULL)
	{
		m_tick1 = GetTickCount64();
	}
	virtual ~CTaskBase()
	{
	}
	CMainPool* _pool;
	LONGLONG _elapsed;
	DWORD _thread;
	CStringW _name;

	LPARAM _this;// this 를 넣어 둔다.
	LPARAM m_lp;//추가 parameter를 Queue 에서 알아서 넣어 주도롤 하기 위해
	LPARAM m_lp2;

	ULONGLONG m_tick1;
	bool m_bAutoFree;
	BOOL m_bKeepRunning;
	CString m_sFunc;
	CString m_sExtra;
	int m_line;
	//bool m_bUIx;
	/// pvParam 은 언제나 App* 이다. Initialize 할때 넣어 준다.
	// QueueTask 나 Queue 할때 따라 가는 파라미터 1번에 _this(CObject*) 를 주고 그다음이 m_lp, m_lp2
	virtual void DoTask(void* pvParam, OVERLAPPED* pOverlapped)=0;
	virtual void DoTaskEx(void* pvParam, OVERLAPPED* pOverlapped, LPARAM lp) {};

	virtual void DeleteMe()
	{
		if(m_bAutoFree)
			delete this;
	}
	CString m_sClass;
	virtual PAS _typename() { return typeid(this).name(); }

	virtual PWS _GetName(); //화면에 task 별 걸린 시간 뿌릴때 'CTask????'에서 리턴;
	virtual PWS _GetNameEx(CString& sb) { return NULL; } //화면에 task 별 걸린 시간 뿌릴때 'CTask????'에서 리턴;
	virtual bool GetUI()
	{
		return true;
	}
	virtual PWS _GetExtra() { return m_sExtra.IsEmpty() ? NULL : (PWS)m_sExtra; }
};


class CPushWorker
{
public:
	typedef DWORD_PTR RequestType;

	static LONG g_lCurrId;

	static CKRbVal<CStringW,DWORD> s_mapThredElapsed;
	//CS_STATIC_INSTANCE2(CPushWorkerCS)
	static BOOL s_bExit;
	CWinApp* m_pApp;

	CPushWorker()
		: m_pApp(NULL)
	{
		m_lId = InterlockedIncrement(&g_lCurrId);
	}

	virtual BOOL Initialize(void* pvParam)
	{
		m_pApp = (CWinApp*)pvParam;
		//		KTrace(L"[%d]: CPushWorker.Initialize(%d)\n", (DWORD_PTR)::GetCurrentThreadId(), (DWORD_PTR)pvParam );
		srand((unsigned)time(NULL)+::GetCurrentThreadId());//(unsigned)getpid() + 
		return TRUE;
	}

	virtual void Terminate(void* /*pvParam*/)
	{
		//		KTrace(L"CPushWorker #%d exec'd %d times.\n", m_lId, m_dwExecs );
	}

	void Execute(RequestType dw, void* pvParam, OVERLAPPED* pOverlapped) throw();

	virtual BOOL GetWorkerData(DWORD /*dwParam*/, void** /*ppvData*/)
	{
		return FALSE;
	}
	static int	s_dwExecs;
	//static CKRbLParamStr s_threadStack;
	
	// CTaskBase free에 전혀 관혀 하지 않고 단지 pointer값만 쓰므로 Val로 취급 한다.
	static CKRbPtr<LPARAM, CTaskBase> s_threadStack;// m_bAutoFree = false 롸 해야 

	// CKRbLParamPbj가 include 범위 밖이라
	//static CKRbLParamPtr<CKRbUINT64Str>* s_thrdLock;
	static CKRbPtr<LPARAM, CKRbVal<UINT64, CStringW>>* s_thrdLock;

protected:
	LONG	m_lId;
}; // CMyWorker





class CTaskLambdaEx : public CTaskBase
{
public:
	std::function<void()>* m_pLambda{ nullptr };
	std::function<void(KException*)>* m_pLambdaException{ nullptr };/// not owner 
	CStringA m_fnc;
	int m_line;
	CString m_sWork;

	CTaskLambdaEx(std::function<void()>* pLambda, LPCSTR fnc = NULL, int line = 0, PWS sWork = NULL)
		: m_pLambda(pLambda)
		, m_fnc(fnc)
		, m_line(line)
		, m_sWork(sWork)
	{
	}

	~CTaskLambdaEx()
	{
		DeleteMeSafe(m_pLambda);
	}
	//std::function<LPARAM(LPARAM, LPARAM)> m_fnc; //#include <functional>
	virtual PAS _typename() { return typeid(this).name(); }
	virtual void DoTask(void* pvParam, OVERLAPPED* pOverlapped);
};


class CMainPool : public CThreadPool<CPushWorker>
{
public:
	static shared_ptr<CMainPool> s_pPool;
	static DWORD s_UiThreadId;
	
#ifdef _Usages
	// 최초에  CMainPool::Pool(6); 처럼 초기화 하고, 초기화 한후
	// CMainPool::Pool()->QueueTask(...)처럼 쓴다. 어차피 매크로 쓸거니까
#endif // _Usages

	static CMainPool* Pool(int nthread = 0);

	static void QueueTask(CTaskBase* pTask, LPARAM pObj = 0, LPARAM lp = 0, LPARAM lp2 = 0)
	{
		Pool()->Queue(pTask, pObj, lp, lp2);
	}


	template<typename TFNC> //std::function<LPARAM()>
	static void QueueFunc(TFNC lmda, PAS name = 0, PAS fnc = NULL, int line = -1)
	{
		//CTaskLambda<TFNC>* pTask = new CTaskLambda<TFNC>(lmda, fnc, line); lmda가 로컬변수라 사라진다.
		std::function<void()>* pLambda = HDDER_NEW
//#ifdef _DEBUG
//			DEBUG_NEW
//#else
//			new
//#endif // _DEBUG
			std::function<void()>(lmda);
		CTaskLambdaEx* pTask = HDDER_NEW
//#ifdef _DEBUG
//			DEBUG_NEW
//#else
//			new
//#endif // _DEBUG
			CTaskLambdaEx(pLambda, fnc, line);
		pTask->_pool = Pool();
		pTask->_name = name;
		pTask->m_pLambdaException = s_pLambdaException;

		Pool()->Queue(pTask, NULL, NULL, NULL);
	}


	///?주의: 람다식에서 [&] 는 괜찮은데, [&,val] 처럼 더 들어 가는 경우 람다식 전체를 가로로 더 묶어 줘야 한다.
	// 2020-06-11 이게 최종 람다 사용 스레드 사용법
		// 가로(())가 두번 해야 매크로 에러 안난다.
	// 최종적으로 람다 함수를 백그라운드 thread큐에 넣는다.
#define QueueFUNC(lmda)        CMainPool::QueueFunc(((lmda)), NULL, __FUNCTION__, __LINE__)

	/// BEST. PAS name
#define QueueFUNCN(lmda, name) CMainPool::QueueFunc(((lmda)), name, __FUNCTION__, __LINE__)
	//ex: QueueFUNC( [&]() -> void {	SomeAsyncFunc(); } );
	//ex: QueueFUNCN([&]() -> void {	SomeAsyncFunc(); }, "saveDB" );

	// 람다 systax부분 빼고 { ... } 부분만 쓸수 있는 식.(불필요할듯)
	// [&]일떄만 사용
#define QueueFUNCX(lmda)        CMainPool::QueueFunc((([&]() -> void lmda)), NULL, __FUNCTION__, __LINE__, NULL)
#define QueueFUNCXN(lmda, name) CMainPool::QueueFunc((([&]() -> void lmda)), name, __FUNCTION__, __LINE__, NULL)
	//ex: QueueFUNCX(  {	SomeAsyncFunc(); } );
	//ex: QueueFUNCXN( {	SomeAsyncFunc(); }, L"saveDB" );

	/* 사용법:
	#include "KwLib32/ThreadPool.h"

	// App에 변수 선언 하고
	InitInstance() {에서
	//1. 초기화
	CMainPool::Pool(6);// 풀링 하는 thread갯수와 함께 초기화 한다.
	}
	//2. 큐에 람다를 넣기만 하면 끝. 어디서나 가로 안에 내용은 간단히.
		QueueFUNCN(([&, i]() -> void { Send(i); }), L"HTTPS 테스트.");
	);
	*/
	//참고: BiginInvoke( 람다); 는 DlgTool.h에 있다.




	CMainPool()
	{
	}
	~CMainPool()
	{
		// 		if(m_bOwner)
		// 			DeleteMeSafe(s_pPool);
	}
	int m_bOwner;
	
	static std::function<void(KException*)>* s_pLambdaException;/// not owner 

	shared_ptr<function<void(KException*)>> _fncOnException;
	shared_ptr<function<void(CTaskBase*)>> _fncOnBeforeTask;
	shared_ptr<function<void(CTaskBase*)>> _fncOnAfterTask;
	template<typename TFNC> void AddExceptionTaskCB(TFNC fnc)
	{
		_fncOnException = shared_ptr<function<void(KException*)>>(HDDER_NEW function<void(KException*)>(fnc));
	}
	/* ex:
 	auto mp = CMainPool::Pool(4);//FOREGROUND() 할때도 필요 하다.
	mp->AddExceptionTaskCB([&](KException* e)
		{
			AppBase::CallClientException(e);
		});

	*/
	template<typename TFNC> void AddBeforeTaskCB(TFNC fnc)
	{
		_fncOnBeforeTask = shared_ptr<function<void(CTaskBase*)>>(HDDER_NEW
			function<void(CTaskBase*)>(fnc));
		// xxx.AdTaskCB([&](CTaskBase*)->void { return *p1 - *p2; } );
	}
	template<typename TFNC> void AddAfterTaskCB(TFNC fnc)
	{
		_fncOnAfterTask = shared_ptr<function<void(CTaskBase*)>>(HDDER_NEW
			function<void(CTaskBase*)>(fnc));
	}

	//IPoolUI* m_pui;
	//void SetUI(IPoolUI* th, int idx)
	//{
	//	m_pui = th;
	//	m_i = idx;
	//}
	//int m_i;

	BOOL Queue(CTaskBase* pTask, LPARAM pObj = 0, LPARAM lp1 = 0, LPARAM lp2 = 0);
};

//::GetCurrentThreadId() == AfxGetApp()

// 이걸 쓸려면 App시작 에서 InitInstance(..)
// CMainPool::Pool(4); 를 해줘야 한다.
#define FOREGROUND()    ASSERT(CMainPool::s_UiThreadId == ::GetCurrentThreadId()) // ::IsGUIThread(FALSE));
// nth는 그저 참조. 몇번째 백그라운드인가? 2이상 이면 백그라은드에서 다시 또 스레드큐에 넣음.
#define BACKGROUND(nth) ASSERT(CMainPool::s_UiThreadId != ::GetCurrentThreadId()) //ASSERT(AfxGetApp()->m_hThread !::IsGUIThread(FALSE));
