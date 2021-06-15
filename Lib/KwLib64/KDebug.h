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
//?����: CSrcException �� �ڵ� ���� ���� �ϵ��� �Ͽ��� Delete() �ϸ� �ȵȴ�.
// ������ catch (CException* e) �� ������� Delete()�� �ϸ� ���� ������ �Ǿ �״´�.
// ���� ���� �ϰ� ��� �Ʒ� �Լ��� �Ἥ ���� �ϸ� CSrcException�� �ƴ� ��츸 ���� �ϰ� �Ͽ� 
// ���� �ϴ�.
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

// ���� ->Delete()
#ifdef _Use_Sample
try
{
	// �Žñ� ��ٰ�.. 
	throwline0;                       // �Լ��� �ٹ�ȣ�� �ѱ涧
	throwline(eErr_UnknownException); // ���� ��ȣ�� �ѱ涧
	throwline(_T("�� �߳���"));       // ���� �޽����� �ѱ涧
}
catch(CSrcException* pl)
{
	KTrace(L"%s, (%d) err(%d)\n", (LPCTSTR)e->m_sFunc, e->m_iLine, e->m_err);
	// �Ǵ� �θ� �Լ��� CSrcException �� �ѱ����
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
