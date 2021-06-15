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

	LPARAM _this;// this �� �־� �д�.
	LPARAM m_lp;//�߰� parameter�� Queue ���� �˾Ƽ� �־� �ֵ��� �ϱ� ����
	LPARAM m_lp2;

	ULONGLONG m_tick1;
	bool m_bAutoFree;
	BOOL m_bKeepRunning;
	CString m_sFunc;
	CString m_sExtra;
	int m_line;
	//bool m_bUIx;
	/// pvParam �� ������ App* �̴�. Initialize �Ҷ� �־� �ش�.
	// QueueTask �� Queue �Ҷ� ���� ���� �Ķ���� 1���� _this(CObject*) �� �ְ� �״����� m_lp, m_lp2
	virtual void DoTask(void* pvParam, OVERLAPPED* pOverlapped)=0;
	virtual void DoTaskEx(void* pvParam, OVERLAPPED* pOverlapped, LPARAM lp) {};

	virtual void DeleteMe()
	{
		if(m_bAutoFree)
			delete this;
	}
	CString m_sClass;
	virtual PAS _typename() { return typeid(this).name(); }

	virtual PWS _GetName(); //ȭ�鿡 task �� �ɸ� �ð� �Ѹ��� 'CTask????'���� ����;
	virtual PWS _GetNameEx(CString& sb) { return NULL; } //ȭ�鿡 task �� �ɸ� �ð� �Ѹ��� 'CTask????'���� ����;
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
	
	// CTaskBase free�� ���� ���� ���� �ʰ� ���� pointer���� ���Ƿ� Val�� ��� �Ѵ�.
	static CKRbPtr<LPARAM, CTaskBase> s_threadStack;// m_bAutoFree = false �� �ؾ� 

	// CKRbLParamPbj�� include ���� ���̶�
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
	// ���ʿ�  CMainPool::Pool(6); ó�� �ʱ�ȭ �ϰ�, �ʱ�ȭ ����
	// CMainPool::Pool()->QueueTask(...)ó�� ����. ������ ��ũ�� ���Ŵϱ�
#endif // _Usages

	static CMainPool* Pool(int nthread = 0);

	static void QueueTask(CTaskBase* pTask, LPARAM pObj = 0, LPARAM lp = 0, LPARAM lp2 = 0)
	{
		Pool()->Queue(pTask, pObj, lp, lp2);
	}


	template<typename TFNC> //std::function<LPARAM()>
	static void QueueFunc(TFNC lmda, PAS name = 0, PAS fnc = NULL, int line = -1)
	{
		//CTaskLambda<TFNC>* pTask = new CTaskLambda<TFNC>(lmda, fnc, line); lmda�� ���ú����� �������.
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


	///?����: ���ٽĿ��� [&] �� ��������, [&,val] ó�� �� ��� ���� ��� ���ٽ� ��ü�� ���η� �� ���� ��� �Ѵ�.
	// 2020-06-11 �̰� ���� ���� ��� ������ ����
		// ����(())�� �ι� �ؾ� ��ũ�� ���� �ȳ���.
	// ���������� ���� �Լ��� ��׶��� threadť�� �ִ´�.
#define QueueFUNC(lmda)        CMainPool::QueueFunc(((lmda)), NULL, __FUNCTION__, __LINE__)

	/// BEST. PAS name
#define QueueFUNCN(lmda, name) CMainPool::QueueFunc(((lmda)), name, __FUNCTION__, __LINE__)
	//ex: QueueFUNC( [&]() -> void {	SomeAsyncFunc(); } );
	//ex: QueueFUNCN([&]() -> void {	SomeAsyncFunc(); }, "saveDB" );

	// ���� systax�κ� ���� { ... } �κи� ���� �ִ� ��.(���ʿ��ҵ�)
	// [&]�ϋ��� ���
#define QueueFUNCX(lmda)        CMainPool::QueueFunc((([&]() -> void lmda)), NULL, __FUNCTION__, __LINE__, NULL)
#define QueueFUNCXN(lmda, name) CMainPool::QueueFunc((([&]() -> void lmda)), name, __FUNCTION__, __LINE__, NULL)
	//ex: QueueFUNCX(  {	SomeAsyncFunc(); } );
	//ex: QueueFUNCXN( {	SomeAsyncFunc(); }, L"saveDB" );

	/* ����:
	#include "KwLib32/ThreadPool.h"

	// App�� ���� ���� �ϰ�
	InitInstance() {����
	//1. �ʱ�ȭ
	CMainPool::Pool(6);// Ǯ�� �ϴ� thread������ �Բ� �ʱ�ȭ �Ѵ�.
	}
	//2. ť�� ���ٸ� �ֱ⸸ �ϸ� ��. ��𼭳� ���� �ȿ� ������ ������.
		QueueFUNCN(([&, i]() -> void { Send(i); }), L"HTTPS �׽�Ʈ.");
	);
	*/
	//����: BiginInvoke( ����); �� DlgTool.h�� �ִ�.




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
 	auto mp = CMainPool::Pool(4);//FOREGROUND() �Ҷ��� �ʿ� �ϴ�.
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

// �̰� ������ App���� ���� InitInstance(..)
// CMainPool::Pool(4); �� ����� �Ѵ�.
#define FOREGROUND()    ASSERT(CMainPool::s_UiThreadId == ::GetCurrentThreadId()) // ::IsGUIThread(FALSE));
// nth�� ���� ����. ���° ��׶����ΰ�? 2�̻� �̸� ��׶����忡�� �ٽ� �� ������ť�� ����.
#define BACKGROUND(nth) ASSERT(CMainPool::s_UiThreadId != ::GetCurrentThreadId()) //ASSERT(AfxGetApp()->m_hThread !::IsGUIThread(FALSE));
