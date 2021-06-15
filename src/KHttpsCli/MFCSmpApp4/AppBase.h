#pragma once
#include "KwLib64/ThreadPool.h"
#include "DockWorkLogList.h"


class AppBase
{
public:
	static void CallClientException(KException* se);
	static void RM_ClientException(ShJObj sjo, int iOp = 0);
	static void AddWorkLog(PAS fObject, PAS fTitle, PWS fObjID, PAS fMode, PWS fObjName, PWS fObjName2 = nullptr, PAS fMemo = nullptr, PWS fObjID2 = nullptr);
 	//static void AddWorkLog(PAS fObject, PAS fTitle, PWS fObjID, PAS fMode, PWS fObjName, PWS fObjName2 = nullptr, PWS fMemo = nullptr, PWS fObjID2 = nullptr)
 	//{// memo PWS : memo ������ ambiguous call�� �ȴ�.
 	//	CStringA fMemoA(fMemo);
 	//	AddWorkLog(fObject, fTitle, fObjID, fMode, fObjName, fObjName2, fMemoA, fObjID2);
 	//}
	static void RM_AddWorkLog(ShJObj sjo, int iOp = 0);
};



#define CATCH_APP catch(KException* e)\
	{	TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTIONW__, __LINE__);\
		throw e;\
	} catch(CException* e)\
	{	auto buf = new TCHAR(1024);\
		KAtEnd d_buf([&]() { delete buf;});\
		e->GetErrorMessage(buf, 1000);\
		TRACE(L"CException:%s - %s %d\n", buf, __FUNCTIONW__, __LINE__);\
		throw new KException("CException", GetLastError(), 0, buf, NULL, __FUNCTION__, __LINE__);\
	} catch(std::exception &e)\
	{	TRACE("catch std::exception %s  - %s %d\n", e.what(), __FUNCTION__, __LINE__);\
		throw new KException("std::exception", -1, 0, CStringW(e.what()), NULL, __FUNCTION__, __LINE__);\
	} catch(PWS e)\
	{	TRACE("catch LPCWSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__);\
		throw new KException("LPCWSTR", GetLastError(), 0, e, NULL, __FUNCTION__, __LINE__);\
	} catch(PAS e)\
	{	TRACE("catch LPCSTR %s  - %s %d\n", e, __FUNCTION__, __LINE__);\
		throw new KException("LPCSTR", GetLastError(), 0, CStringW(e), NULL, __FUNCTION__, __LINE__);\
	} catch(...)\
	{	TRACE("catch ...  - %s %d\n", __FUNCTION__, __LINE__);\
		throw new KException("Unknown", GetLastError(), 0, L"Unknown catch(...) Error.", NULL, __FUNCTION__, __LINE__);\
	}
/* ����
catch(boost::system::system_error ex)\
{	boost::system::error_code ec = ex.code(); \
TRACE("catch boost::systemerror %s  - %s %d\n", ec.message().c_str(), __FUNCTION__, __LINE__); \
throw new KException("boost::systemerror", ec.value(), 0, CStringW(ec.message().c_str()), NULL, __FUNCTION__, __LINE__); \
}*/


///?deprecated ��� ��׶��� �۾� ���� ó�� �Ϸ��� �߸� �Ǵ�. run �Ҷ� ���ξ���. ����� ť�� �ֱ⸸ �Ѵ�.
template<typename TFNC>
void QueueAsyncFunc(TFNC lmda, PAS name = 0, PAS fnc = NULL, int line = -1)
{
	CMainPool::QueueFunc(lmda, name, fnc, line);

	//try
	//{
	//	try
	//	{
	//		CMainPool::QueueFunc(lmda, name, fnc, line);//�߸��� �Ǵ�
	//	} CATCH_APP;
	//}
	//catch(KException* e)/// �׸��� ���Ⱑ 2��°�� ���� ���⼭ ��� ���� ó�� �ؾ� �Ѵ�.
	//{
	//	TRACE(L"KException:%s - %s %d\n", e->m_strError, __FUNCTION__, __LINE__);
	//	//auto pfn = (CMainFrame*)AfxGetMainWnd();
	//	AppBase::CallClientException(e);
	//	//ASSERT(e->IsAutoDelete());
	//	e->Delete();
	//}
}



class CSafeViewBase : public CFormInvokable
{
protected: // serialization������ ��������ϴ�.
	CSafeViewBase(UINT nIDTemplate);
	DECLARE_DYNAMIC(CSafeViewBase)

public:
	// Ư���Դϴ�.
public:
	// �۾��Դϴ�.
public:
	// �������Դϴ�.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

	// �����Դϴ�.
public:
	virtual ~CSafeViewBase();

protected:

	// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
