#pragma once


extern int g_opTrace;// = (1 << 8 | 1 << 2); // 256 KTraceDb(8)

void KTraceA(LPCSTR lpFormat, ...);
void KTrace(LPCTSTR lpFormat, ...);
void KTraceErr(LPCTSTR lpFormat, ...);
void KTrace1(LPCTSTR lpFormat, ...);
void KTrace2(LPCTSTR lpFormat, ...);
void KTrace3(LPCTSTR lpFormat, ...);
void KTrace4(LPCTSTR lpFormat, ...);
void KTrace5(LPCTSTR lpFormat, ...);
void KTrace6(LPCTSTR lpFormat, ...);
void KTrace7(LPCTSTR lpFormat, ...);
void KTrace8(LPCTSTR lpFormat, ...);
void KTraceDb(LPCTSTR lpFormat);
void KTraceDbV(LPCTSTR lpFormat, ...);
void KTraceN(int mode, LPCTSTR lpFormat, ...);



//////////////////////////////////////////////////////////////
//?주의: CSrcException 은 자동 으로 삭제 하도록 하여서 Delete() 하면 안된다.
// 하지만 catch (CException* e) 로 받은경우 Delete()를 하면 이중 삭제가 되어서 죽는다.
// 따라서 안전 하게 모두 아래 함수를 써서 삭제 하면 CSrcException가 아닌 경우만 삭제 하게 하여 
// 안전 하다.
void DeleteException(CException* e);
class CAutoDelException
{
public:
	CException* m_pObj;
	CAutoDelException(CException* pObj)
		: m_pObj(pObj)
	{
	}
	~CAutoDelException()
	{
		Free();
	}
	void Free();
};
#define DelException(e) CAutoDelException __aux(e)

// 사용법 ->Delete()
#ifdef _Use_Sample
try
{
	// 거시기 허다가.. 
	throwline0;                       // 함수와 줄번호만 넘길때
	throwline(eErr_UnknownException); // 에러 번호도 넘길때
	throwline(_T("니 잘났다"));       // 에러 메시지도 넘길때
}
catch(CSrcException* pl)
{
	KTrace(L"%s, (%d) err(%d)\n", (LPCTSTR)e->m_sFunc, e->m_iLine, e->m_err);
	// 또는 부른 함수로 CSrcException 를 넘기려면
	throwpl;
}
#endif // _Use_Sample


#define CheckHR(hr) KwCheckHRESULT(hr, __FUNCTIONT__, __LINE__)
#define CheckHR2(hr, s2) KwCheckHRESULT(hr, __FUNCTIONT__, __LINE__, s2)
#define CheckHrFailed(hr) if(FAILED(hr)){CheckHR(hr);return hr;}
#define CheckHrBreak(hr) if(FAILED(hr)){CheckHR(hr);break;}
#define CheckHrContinue(hr) if(FAILED(hr)){CheckHR(hr);continue;}
#define ThrowCheck(hr) if(FAILED(hr)){CheckHR(hr); throw hr;}
#define ThrowNull1(p, hr) if((p) == NULL){CheckHR(hr); throw hr;}
#define ThrowNull(p) ThrowNull1((p), -1)
#define ThrowIfNull(p) if((p) == NULL){throw (int)__LINE__;}

#define ThrowRvOk(rv) if((rv) != 0) throw rv;
