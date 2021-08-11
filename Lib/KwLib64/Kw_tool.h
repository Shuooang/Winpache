#pragma once
#include "ktypedef.h"

#include "enumval.h"
#include "Dictionary.h"
#include "stdecl.h"
#include "KJson.h"
//#include "JSON/JSONValue.h"



/// C:\Dropbox\Proj\STUDY\boostEx\CppServer\CppServer-master\include\server\kwadjust.h
///    ���� �ߺ� �Ǿ� �ִ�. ���� �Ϸ��ŵ� ���� �����.
/// 
#ifndef KTRACEOUTPUT
#define KTRACEOUTPUT

#  ifdef _MSC_VER
#    include <windows.h>
#    include <sstream>
#    define TRACEX(x)                           \
     do {  std::stringstream s;  s << (x);     \
           OutputDebugStringA(s.str().c_str()); \
        } while(0)
#  else
#    include <iostream>
#    define TRACEX(x)  std::clog << (x)
#  endif        // or std::cerr << (x) << std::flush
/// <summary>
/// �� class�� std consol�� << �Ͽ� ��� �ϴ� stream �ڵ带 �״�� �츱 �������� ���� �Ŵ�.
/// std_cout << "~CMyHttp() destroyed." << std_endl;  ó��. CKTrace std_cout; �� �̸� ���� �ϰ�.����. std_endl �� ����� ���´�.
/// Usage: extern CKTrace std_cout;
///               CKTrace std_cout;
/// </summary>
class CKTrace
{
public:
	CKTrace(bool bDebug = false)
		: _debug(bDebug)
	{
	}
	bool _debug;
	std::stringstream _s;
	//std::stringstream _sPrev;

	std::string str()
	{
		return _s.str();
	}

	std::shared_ptr<std::function<void(std::string)>> _fncTrace;
	void AddCallbackOnTrace(std::shared_ptr<std::function<void(std::string)>> fnc)//?ExTrace 5 CKTrace::fnc �� ����
	{
		if(!_fncTrace)
			_fncTrace = fnc;
	}

	CKCriticalSection _csTrace;
	void Output(const char* txt);

	CKTrace& operator<<(const char* ctr)
	{
		Output(ctr);
		return *this;
	}
	CKTrace& operator<<(const CStringA& ctr)
	{
		Output((PAS)ctr);
		return *this;
	}
	CKTrace& operator<<(const CStringW& ctr)
	{
		CStringA ctra(ctr);
		Output((PAS)ctra);
		return *this;
	}
	CKTrace& operator<<(const std::wstring& ctr)
	{
// 		std::string ctra;
// 		Ucode(ctr, ctra);
// 		Output(ctra.c_str());
		CStringA ctra(ctr.c_str());
		Output((PAS)ctra);
		return *this;
	}
	CKTrace& operator<<(const std::string& ctr) {
		Output(ctr.c_str());
		return *this;
	}
	CKTrace& operator<<(const std::stringstream& ctr) {
		Output(ctr.str().c_str());
		return *this;
	}
// 	CKTrace& operator<<(int ctr) {
// 		char buf[24];
// 		_itoa_s(ctr, buf, 10);
// 		TRACEX(buf);
// 		return *this;
// 	}
	template<typename TNUM>
	CKTrace& operator<<(TNUM ctr)
	{
		std::string str = std::to_string(ctr);
		Output(str.c_str());
		return *this;
	}
// 	CKTrace& operator<<(unsigned long long ctr) {
// 		std::string str = std::to_string(ctr);
// 		TRACEX(str.c_str());
// 		return *this;
// 	}
// 	CKTrace& operator<<(double ctr) {
// 		char buf[100];
// 		sprintf_s(buf, "%.6f", ctr);
// 		TRACEX(buf);
// 		return *this;
// 	}
};
#define std_endl "\r\n" //std::endl

//CKTrace _trace;
#endif




// recursive�Լ����� ���̸� �ڵ����� �߰� ���� �ǵ��� �ϴµ� ���δ�.
// sample: see int CDlgCmmn::SaveXDataWithPath()
// using:	CDeptNum d(m_dept);
class CAutoIncr
{
public:
	CAutoIncr(int& nDept, bool bAutoInc = true, bool bAutoDec = true)
		: m_dept(nDept)
		, m_bAutoInc(bAutoInc)
		, m_bAutoDec(bAutoDec)
	{
		if(m_bAutoInc)
			Increase();
	}
	~CAutoIncr()
	{
		if(m_bAutoDec)
			Decrease();
	}
	void Increase()
	{
		m_dept++;
	}
	void Decrease()//bool bAuto=false)
	{
		//m_bAuto = bAuto;// �������� ���� �������Ƿ� �������� ��ȯ ��Ų��.
		m_dept--;
	}
	void SetAuto(bool bAutoInc = true, bool bAutoDec = true)
	{
		m_bAutoInc = bAutoInc;
		m_bAutoDec = bAutoDec;
	}
#ifdef _DEBUG
protected:
#endif // _DEBUG
	bool m_bAutoInc;
	bool m_bAutoDec;
	int& m_dept;
};





class CAutoCoInit
{
public:
	CAutoCoInit(COINIT eCinit = COINIT_APARTMENTTHREADED, bool bInit = true)
		: m_bInit(false)
		, m_dwCoInit(eCinit)
	{
		if(bInit)
			this->Initialize();
	}

	~CAutoCoInit()
	{
		this->Uninitialize();
	}

	DWORD m_dwCoInit;
	bool m_bInit;


	void Initialize()
	{
		ASSERT(!m_bInit);
		::CoInitializeEx(NULL, m_dwCoInit);
		m_bInit = true;
	}
	void Uninitialize()
	{
		if(m_bInit)
		{
			::CoUninitialize();
			m_bInit = false;
		}
	}
};






#define _Formatl CString _s;_s.Format
#define _Formatl1 CString _s1;_s1.Format

//#define _Format CString& _s=STRBUF;_s.Format STRBUF STRBUF


#define CSTRING_SPRINTF(lpFormat_,str_) \
{ATLASSERT( AtlIsValidString( lpFormat_ ) );\
va_list argList;\
va_start( argList, lpFormat_ );\
str_.FormatV( lpFormat_, argList );\
va_end( argList );\
}
#define CSTRINGA_SPRINTF(lpFormat_,str_) \
{ATLASSERT( AtlIsValidString( lpFormat_ ) );\
va_list argList;\
va_start( argList, lpFormat_ );\
str_.FormatV( lpFormat_, argList );\
va_end( argList );\
}




PWS KwUTF8ToWchar(PAS sUtf8, CStringW& sWstr);
PWS KwUTF8ToHtmlUrl(CStringA& sUtf8, CString& sWstr);
// PWS KwStringToWString(std::string& str, std::wstring& wstr);
// PAS KwWStringToString(std::wstring& wstr, std::string& str);
PWS KwHtmlUrlToWChar(CStringA& shtml, CStringW& sw);
PAS KwHtmlUrlToChar(CStringA& shtml, CStringA& sa);
PAS KwWcharToUTF8(PWS sWstr, CStringA& sUtf8);
PAS KwWcharToUTF8(LPCWSTR sWstr, KBinary& sUtf8);
PAS KwCharToUTF8(PAS sWstr, CStringA& sUtf8);
PAS KwUTF8ToChar(PAS sUtf8, CStringA& sAstr);


int IsDateHan(WCHAR cd);
int IsDateEng(PWS cd);
bool KwIsPmHour(PWS pstr, int& pm);

LPVOID KwBsearch(const void* key, const void* base, size_t num, size_t width,
						 int (*compare)(const void*, const void*, BOOL), BOOL bCaseSensitive = TRUE);

int KwBSearchP(PWS key, PWS* base, int count, BOOL bCaseSensitive = TRUE);
PWS KwBSearchPstr(PWS key, PWS* ppBase, int count, BOOL bCaseSensitive = TRUE);


bool KwIsPmHour(PWS pstr, int& pm);
int KwTextCmp(PWS ps1, PWS ps2, int iOp = eCmprCase, size_t len = -1);

BOOL KwSetReadOnly(CString sOut, bool bReadOnly = true);

bool KwIfFileExist(LPCWSTR sFull);
inline bool KwIfFileExist(LPCSTR sFullA) {
	CStringW sFull(sFullA);
	return KwIfFileExist(sFull);
}

int KwFileSafeRemove(PWS target);

LPSTR KwBinToHexStr(LPSTR bin, int len, LPSTR rbuf);
int KwGetUrlParams(PAS pUrl, CKRbVal<CString, CString>& params);
int KwCutStrByChar(TCHAR c, CString s, CStringArray& ar, bool bIgnorFirst = false, bool bIgnorLast = false);

//#ifdef _Use_GUID
int KwGetFormattedGUID(GUID& guid, CString& rString, bool bHipn = true);
CString KwGetFormattedGuid(bool bHipn = true, PWS title = nullptr, int left = 0);
//#endif

//void KwCutByToken(LPCTSTR src, LPCTSTR seps, CStringArray& strArray, bool bTrim = false);
void KwCutByToken(PWS psSrc, PWS seps, CStringArray& ars, bool bTrim = false);
void KwCutByToken(PWS psSrc, PWS seps, std::vector<std::wstring>& ar, bool bTrim = false);

int KwCutByToken(std::string& src, std::string dl, std::vector<std::string>& ar, __int64 offset = 0);
inline int KwCutByToken(PAS psrc, std::string dl, std::vector<std::string>& ar, __int64 offset = 0) {
	std::string src = psrc;
	return KwCutByToken(src, dl, ar, offset);
}

int KwKeyValueStrToMap(PAS buf, PAS dl1, PAS dlkv, std::map<string, string>& kv);

int KwUrlParamToMap(std::string& src, std::map<std::string, std::string>& mp);

template<class TJSON>
int KwUrlParamToJsonT(string& src, TJSON& jmp)
{
	vector<string> arp;
	auto np = KwCutByToken(src, "&", arp, 0);
	if(np == 0)
		return 0;
	for(int i = 0; i < np; i++)
	{
		vector<string> kv;
		auto pr = KwCutByToken(arp[i], "=", kv);
		if(pr >= 2)
			jmp(CStringW(kv[0].c_str())) = kv[1].c_str();
		//mp[kv[0]] = kv[1];
	}
	return (int)jmp.size();
}

int KwUrlParamToJson(string& src, Kw::JObj& jmp);
inline int KwUrlParamToMap(PAS psrc, std::map<std::string, std::string>& mp) {
	std::string src = psrc;
	return KwUrlParamToMap(src, mp);
}


PAS KwUrlDirToVpath(PAS dir, PAS vpath, std::string& full);

float KwCalcInplaceRate(CSize szArea, CSize szObj);
CRect KwCalcInplaceRect(CRect rcArea, CSize szImage, PAS sAlign = NULL);//bool bClearRate = false, 
IStream* KwMemoryToStream(LPVOID data, size_t size, HGLOBAL& hgm);
/// extSave = "jpg" or png. nullptr : �״��
int KwSaveImageMultiSize(LPVOID idata, size_t isize, CStringArray& arPath, CSize* arSz, PAS extSave = nullptr);
//"sAlign hcenter vcenter"

// bToEnd�� FALSE�̸� �� ������ ���� FILE NAME �̹Ƿ� ���� �ʿ� ���ٴ� ��.
int KwCheckTargetDir(PWS sFull, BOOL bToEnd=TRUE, BOOL bCreate = TRUE);

CStringA KwContentType(PAS extension);

bool KwTrimNumDotUnder(CString& sd);

int KwRangedRand(int range_min, int range_max);

CStringA KwCreateKeyStr(int len, bool bLong = false);
CStringA KwRandomEnNumStr(int len, int eType = 1 | 2);

CString KwGetWindowVersion();
CStringA KwComputerNameA();
inline CStringW KwComputerName()
{
	return CStringW(KwComputerNameA());
}
CString KwGetProductVersion();
CString KwGetConnectedState();
CString KwSystemInfo();

// ������ ���� ��ŭ �迭���� �þ� ����.
// �� 222;;333;444 �̸� ���� 4�� �̴�. �߰��� ;; �� ; �� ���� �ʴ´�.
// par�� NULL�̸� ������ ���� �Ѵ�.
template<typename TChar, typename TString, typename TArray>
int KwCutStrByCharT(TChar c, TString s, TArray& ar, bool bIgnorFirst = false, bool bIgnorLast = false)
{
	if(s.IsEmpty())
		return 0;
	TChar dstr[1] = { (TChar)'\0' };
	int i0 = 0, i1 = -1;
	for(int i = 0;; i++)
	{
		i1 = s.Find(c, i0);
		if(i1 >= 0)
		{
			if(i1 == 0)
			{
				if(!bIgnorFirst)
					ar.Add(dstr);
			}
			else//if(i1 != 0) //�Ǿ��� '~'�̸� '~' �տ��� ���̹Ƿ� ���� �ʴ´�. �� �Ǿ��� '~'�� ���� �Ѵ�.
			{
				ar.Add(s.Mid(i0, i1 - i0));
				if(bIgnorLast && i1 == s.GetLength() - 1)
					return (int)ar.GetCount();
			}
			i0 = i1 + 1;
		}
		else
			break;
	}

	if(i0 == s.GetLength())
		ar.Add(dstr);
	else
		ar.Add(s.Mid(i0));
	return (int)ar.GetCount();
}
#ifdef _DEBUG
// �� class�� ����� std::map ���·� �ٲٸ鼭, �ڵ��� � �ʵ尡 �ִ��� �ٷ� ���� �ϱ� ����
class Vake { };
#endif // _DEBUG

BOOL KwTextToVar(COleVariant& m_varValue, const CString& strText);
BOOL KwNullToVar(COleVariant& m_varValue);

BOOL KwCopyTextClipboad(CWnd* pWnd, LPCWSTR text);

LPCSTR KwReadSmallTextFileA(PWS fileName, CStringA& str);
DWORD KwGetFullPathName(PWS lpszFileIn, CStringW& path, PWS* pFilePart = NULL);

UINT_PTR EncodeBinary(KBinary& bin, LPCSTR key0, bool bEncode, KBinary* pbinr);

LONGLONG KwGetTickCount100Nano();



hres KwGetMacInfo(CString& localIP, CString& macAddr);

/// <summary>
/// resource ���� ���� ���ڿ� static ���ڿ� �����Ͱ� �����ϰ� ���� �ϵ��� static map�� ���� �Ѵ�.
/// </summary>
/// <param name="idc"></param>
/// <returns></returns>
PWS KwRsc(int idc);
PAS KwRscA(int idc);


/// <summary>
/// PAS k�� PWS �� ���� �ϰ� �ִٰ�, �״�� pointer�� static���� ������ ü�� ���� �Ѵ�.
/// ���ڿ� �����Ͱ� ������ ����ü �����͸� ���� �ְ� �Ѵ�.
/// </summary>
/// <param name="k"></param>
/// <returns></returns>
LPCWSTR Pws(LPCSTR k);
inline LPCWSTR Pws(LPCWSTR kw)
{
	CStringA k(kw);
	return Pws((LPCSTR)k);
}
inline LPCWSTR Pws(wstring& kw)
{
	CStringA k(kw.c_str());
	return Pws((LPCSTR)k);
}
inline LPCWSTR Pws(CStringA& k)
{
	return Pws((LPCSTR)k);
}
inline LPCWSTR Pws(CStringW& kw)
{
	CStringA k(kw);
	return Pws((LPCSTR)k);
}
inline LPCWSTR Pws(string& k)
{
	return Pws((LPCSTR)k.c_str());
}

/*
* 1>C:\Dropbox\Proj\KHttp\Lib\KwLib64\Kw_tool.h(435,16): message : could be 'LPCWSTR Pws(std::string &)'
1>C:\Dropbox\Proj\KHttp\Lib\KwLib64\Kw_tool.h(430,16): message : or       'LPCWSTR Pws(std::wstring &)'
1>C:\Dropbox\Proj\KHttp\Lib\KwLib64\Kw_tool.h(425,16): message : or       'LPCWSTR Pws(CStringW &)'
1>C:\Dropbox\Proj\KHttp\Lib\KwLib64\Kw_tool.h(421,16): message : or       'LPCWSTR Pws(CStringA &)'
1>C:\Dropbox\Proj\KHttp\Lib\KwLib64\Kw_tool.h(420,9): message : or       'LPCWSTR Pws(LPCSTR)'
1>C:\Dropbox\Proj\KHttp\Lib\KwLib64\KJson.h(325,1): message : while trying to match the argument list '(TKEY)'
*/

LPCSTR Pas(LPCWSTR k);
inline LPCSTR Pas(LPCSTR ka)
{
	CStringW kw(ka);
	return Pas((LPCWSTR)kw);
}
/*
inline LPCSTR Pas(CStringA& ka)
{
	CStringW kw(ka);
	return Pas((LPCWSTR)kw);
}
inline LPCSTR Pas(CStringW& kw)
{
	return Pas((LPCWSTR)kw);
}
inline LPCSTR Pas(wstring& kw)
{
	return Pas(kw.c_str());
}
inline LPCSTR Pas(string& k)
{
	CStringW kw(k.c_str());
	return Pas((LPCWSTR)kw);
}
*/
