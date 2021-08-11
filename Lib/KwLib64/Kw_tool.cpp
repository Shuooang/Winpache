#include "Kw_tool.h"
#include "TimeTool.h"
#include "pch.h"

#include <atlimage.h>

#include "inlinefnc.h"

#include "tchtool.h"
#include "Kw_tool.h"
#include "KJson.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



PAS KwWcharToUTF8(LPCWSTR sWstr, CStringA& sUtf8)
{
	char* pUtf8 = NULL;
	// 	nLength = MultiByteToWideChar(CP_ACP, 0, pszCode, lstrlen(pszCode), NULL, NULL); 
	// 	bstrCode = SysAllocStringLen(NULL, nLength); 
	// 	MultiByteToWideChar(CP_ACP, 0, pszCode, lstrlen(pszCode), bstrCode, nLength);

	int nLength2 = WideCharToMultiByte(CP_UTF8, 0, sWstr, -1, pUtf8, 0, NULL, NULL);
	pUtf8 = sUtf8.GetBuffer(nLength2 +1);//char*)malloc(nLength2+1); 
	WideCharToMultiByte(CP_UTF8, 0, sWstr, -1, pUtf8, nLength2, NULL, NULL);
	sUtf8.ReleaseBuffer();

	return sUtf8;
}
/// <summary>
/// 위랑 똑같지만 KBinary만 다름.
/// </summary>
PAS KwWcharToUTF8(LPCWSTR sWstr, KBinary& sUtf8)
{
	char* pUtf8 = NULL;
	int nLength2 = WideCharToMultiByte(CP_UTF8, 0, sWstr, -1, pUtf8, 0, NULL, NULL);
	pUtf8 = sUtf8.Alloc(nLength2);//char*)malloc(nLength2+1); 
	WideCharToMultiByte(CP_UTF8, 0, sWstr, -1, pUtf8, nLength2, NULL, NULL);
	return pUtf8;
}
PAS KwCharToUTF8(PAS sAstr, CStringA& sUtf8)
{
	CStringW sWstr(sAstr);
	return KwWcharToUTF8(sWstr, sUtf8);
}

PAS KwUTF8ToChar(PAS sUtf8, CStringA& sAstr)
{
	CStringW sWstr;
	KwUTF8ToWchar(sUtf8, sWstr);
	sAstr = CStringA(sWstr);
	return sAstr;
}
PWS KwUTF8ToWchar(PAS sUtf8, CStringW& sWstr)
{
	int lenUtf8 = lstrlenA(sUtf8) + 1;
	int nLength = MultiByteToWideChar(CP_UTF8, 0, sUtf8, lenUtf8, NULL, NULL);
	LPWSTR pw = sWstr.GetBuffer(nLength);
	MultiByteToWideChar(CP_UTF8, 0, sUtf8, lenUtf8, pw, nLength);
	sWstr.ReleaseBuffer();

	// 	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	// 	pszAnsi = new char[nLength];
	// 	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
	// 	SysFreeString(bstrWide);
	return sWstr;
}
PAS KwHtmlUrlToChar(CStringA& shtml, CStringA& sa)
{
	char c1[3] ={'\0',};
	int n = shtml.GetLength();
	for_each0(n)
	{
		char ch = shtml[_i];
		if(ch == '%' && (_i+2) < n)
		{
			c1[0] = shtml[_i+1];
			c1[1] = shtml[_i+2];
			c1[2] = '\0';

			int cv = '\0';
			sscanf_s(c1, "%02x", &cv);
			sa += (char)cv;
			_i += 2;
		}
		else// if(KwIsAlNum(ch))
		{
			sa += ch;
		}
	}
	return sa;
}
PWS KwHtmlUrlToWChar(CStringA& shtml, CStringW& sw)
{
	CStringA sa;
	KwHtmlUrlToChar(shtml, sa);
	sw = CString(sa);
	return sw;
}

PWS KwWcharToUTF8ToHtmlUrl(CStringW& sWchar, CString& sWUrl)
{
	CStringA sUtf8;
	KwWcharToUTF8(sWchar, sUtf8);
	KwUTF8ToHtmlUrl(sUtf8, sWUrl);
	return sWUrl;
}

PAS KwUTF8ToHtmlUrlA(CStringA& sUtf8, CStringA& sAhtml)
{
	CString sWstr;
	KwUTF8ToHtmlUrl(sUtf8, sWstr);
	sAhtml = CStringA(sWstr);
	return sAhtml;
}
PWS KwUTF8ToHtmlUrl(CStringA& sUtf8, CString& sWstr)
{
	//eWstr.Empty();
	CString s;
	for_each0(sUtf8.GetLength())
	{
		char ch = sUtf8[_i];
		if(KwIsAlNum(ch)
// 			('A' <= ch && ch <= 'Z') ||
// 			('a' <= ch && ch <= 'z') ||
// 			('0' <= ch && ch <= '9') 
)
		{
			sWstr += (WCHAR)ch;
		}
		else
		{
			s.Format(L"%%%02X", (BYTE)ch);
			ASSERT(s.GetLength() == 3);
			sWstr += s;
		}
	}
	return sWstr;
}
/*
PWS KwStringToWString(std::string& str, std::wstring& wstr)
{
	wstr.assign(str.begin(), str.end());
	return wstr.c_str();
}
PAS KwWStringToString(std::wstring& wstr, std::string& str)
{
	str.assign(wstr.begin(), wstr.end());
	return str.c_str();
}
void KwCutByToken(LPCTSTR psSrc, LPCTSTR seps, CStringArray& strArray, bool bTrim)
{
	//	ASSERT(lstrlen(psSrc) > 0);
	if(psSrc == NULL || lstrlen(psSrc) == 0)
		return;
	TCHAR* pSrc = new TCHAR[tchlen(psSrc)+1];
	CAutoFreePtr<TCHAR> _d(pSrc);
	ASSERT(pSrc);
	tchcpy(pSrc, psSrc);
	TCHAR* tok = _tcstok(pSrc, seps);
	for(int i=0; tok != NULL;i++)
	{
		if(bTrim)
		{
			CString stok(tok);
			stok.Trim();
			strArray.SetAtGrow(i, stok);
		}
		else
			strArray.SetAtGrow(i, tok);
		tok = _tcstok(NULL, seps);
	}
	//	delete pSrc;
}*/

void KwCutByToken(PWS psSrc, PWS seps, CStringArray& ars, bool bTrim)
{
	if(lstrlen(psSrc) == 0)
		return;
	WCHAR* pSrc = new WCHAR[wcslen(psSrc)+1];
	shared_ptr<WCHAR> _d(pSrc);
	WCHAR* next_token1 = NULL;

	ASSERT(pSrc);
	tchcpy(pSrc, psSrc);
	WCHAR* tok = wcstok_s(pSrc, seps, &next_token1);
	for(int i=0; tok != NULL;i++)
	{
		if(bTrim)
		{
			CString stok(tok);
			stok.Trim();
			ars.SetAtGrow(i, stok);
		}
		else
			ars.SetAtGrow(i, tok);
		tok = wcstok_s(NULL, seps, &next_token1);
	}
}
void KwCutByToken(PWS psSrc, PWS seps, std::vector<std::wstring>& ars, bool bTrim )
{
	if(lstrlen(psSrc) == 0)
		return;
	WCHAR* pSrc = new WCHAR[wcslen(psSrc) + 1];
	shared_ptr<WCHAR> _d(pSrc);
	WCHAR* next_token1 = NULL;

	ASSERT(pSrc);
	tchcpy(pSrc, psSrc);
	WCHAR* tok = wcstok_s(pSrc, seps, &next_token1);
	for(int i = 0; tok != NULL; i++)
	{
		if(bTrim)
		{
			CString stok(tok);
			stok.Trim();
			ars.push_back((PWS)stok);// SetAtGrow(i, stok);
		}
		else
			ars.push_back((PWS)tok);// SetAtGrow(i, stok);
			//ars.push_back();//ars.SetAtGrow(i, tok);
		tok = wcstok_s(NULL, seps, &next_token1);
	}
}


int IsDateHan(WCHAR cd)
{
	//"수요일, 17 10 2007 16:34:17 +0900"
	PWS s_date[7] ={L"금",L"목",L"수",L"월",L"일",L"토",L"화",};//정렬됨
	WCHAR key[2] ={0,0};
	key[0] = cd;
	return KwBSearchP(key, s_date, _countof(s_date), FALSE);
}
int IsDateEng(PWS cd)
{
	static PWS s_dateE[] ={L"Fri", L"Mon", L"Sat",L"Sun", L"Thu", L"Tue", L"Wed",};//정렬됨
	return KwBSearchP(cd, s_dateE, _countof(s_dateE), FALSE);
}

//?warning pm 은 오후 12:?? 인 경우 를 제외 하고 12를 더하면 된다.
bool KwIsPmHour(PWS pstr, int& pm)
{
	if(tchcmp(pstr, L"오후") == 0 || tchicmp(pstr, L"PM") == 0)
	{
		pm = 12;
		return true;
	}
	else if(tchcmp(pstr, L"오전") == 0 || tchicmp(pstr, L"AM") == 0)//if(ps.ExistInI(_T("오전"), _T("AM")))
	{
		pm = 0;
		return true;
	}
	return false;
}






LPVOID KwBsearch(const void* key, const void* base, size_t num, size_t width,
				  int (*compare)(const void*, const void*, BOOL), BOOL bCaseSensitive)
{
	char* lo = (char*)base; //?warning TCHAR 로 하면 안된다.
	char* hi = (char*)base + (num - 1) * width;
	char* mid;
	size_t half;

	int result;
	while(lo <= hi)
	{
		if((half = num / 2) != 0)
		{
			mid = lo + (num & 1 ? half : (half - 1)) * width;
			result = compare(key, mid, bCaseSensitive);
			if(result == 0)
			{
				return(mid);
			}
			else if(result < 0)
			{
				hi = mid - width;
				num = num & 1 ? half : half-1;
			}
			else
			{
				lo = mid + width;
				num = half;
			}
		}
		else if(num)
			return (compare(key, lo, bCaseSensitive) ? NULL : lo);
		else
			break;
	}
	return NULL;
}



int KwTextCmp(PWS ps1, PWS ps2, int iOp, size_t len)
{
	int rv = 0;
	bool bChoseong = false;
	if(len < 0)//== eInvalid) eCmprInCase
	{
		if(KwAttr(iOp, eCmprCase))
			rv = tchcmp(ps1, ps2);
		else //if(KwAttr(iOp, eCmprInCase))wcscmp wcsicmp
			rv = tchicmp(ps1, ps2);
	}
	else
	{
		ASSERT(len > 0);
		if(KwAttr(iOp, eCmprCase))
			rv = tchncmp(ps1, ps2, len);
		else //if(KwAttr(iOp, eCmprInCase))wcsncmp wcsnicmp
			rv = tchnicmp(ps1, ps2, len);
	}
	return rv;
}


int _KStrCmp0(const void* p1, const void* p2, BOOL bCaseSensitive)
{
	return KwTextCmp(*(PWS*)p1, *(PWS*)p2, bCaseSensitive == TRUE ? eCmprCase : eCmprNone);
}

int KwBSearchP(PWS key, PWS* ppBase, int count, BOOL bCaseSensitive)
{
	//	_CRTIMP void __cdecl* bsearch (const void *, const void *, size_t, size_t, int (__cdecl *)(const void *, const void *))
	PWS* pp = (PWS*)KwBsearch((void*)&key, ppBase, count, sizeof(PWS), (int(*)(const void*, const void*, BOOL))_KStrCmp0, bCaseSensitive);
	if(pp)
	{
#ifdef _DEBUG
		size_t offset = pp - ppBase;
		size_t sz = sizeof(PWS);
		size_t idx = offset / sz;
#endif // _DEBUG
		return (int)(pp - ppBase);///sizeof(PAS);
	}
	return -1;
	//	return (PAS)bsearch((void*)key, (void*)base[0], count, sizeof(PAS), (int(*)(const void*, const void*))lstrcmp);
}
PWS KwBSearchPstr(PWS key, PWS* ppBase, int count, BOOL bCaseSensitive)
{
	//	_CRTIMP void __cdecl* bsearch (const void *, const void *, size_t, size_t, int (__cdecl *)(const void *, const void *))
	PWS* pp = (PWS*)KwBsearch((void*)&key, ppBase, count, sizeof(PWS), (int(*)(const void*, const void*, BOOL))_KStrCmp0, bCaseSensitive);
	return *pp;
}


BOOL KwSetReadOnly(CString sOut, bool bReadOnly)
{
	CFileStatus rStatus;
	CFile::GetStatus(sOut, rStatus);

	if(bReadOnly)
		rStatus.m_attribute |= CFile::readOnly;
	else
		rStatus.m_attribute &= ~CFile::readOnly;

	try
	{
		CFile::SetStatus(sOut, rStatus);
	}
	catch(CFileException*)
	{
		return FALSE;
	}
	catch(CException*)
	{
		return FALSE;
	}
	return 1;
}



// 피일속성을 리턴 한다. WIN32_FIND_DATA::dwFileAttributes
bool KwIfFileExist(LPCWSTR sFull)
{
	DWORD dw = ::GetFileAttributes(sFull);
	return dw != INVALID_FILE_ATTRIBUTES && !(dw & FILE_ATTRIBUTE_DIRECTORY);
}
int KwFileSafeRemove(PWS source)
{
	if(KwIfFileExist(source))
	{
		if(!KwSetReadOnly(source, false))
			return -1;
		try { CFile::Remove(source); }
		catch(...)
		{
			return 4; //있느거 확인했는데 안지워 지면 문제지
		}
	}
	return 0;
}


// 문자열을 "0A0B0C" 와 같이 헥사 스트링을 만들어준다.
LPSTR KwBinToHexStr(LPSTR bin, int len, LPSTR rbuf)
{
	ASSERT(rbuf != NULL);
	char buf[3]={0,0,0};// 낱자
	rbuf[0] = '\0';// 처음부터 cat 해서 붙여 간다.
	for(int i=0;i<len;i++)
	{
		int c = (int)(unsigned char)bin[i];
		int v1 = (int)(unsigned char)c /16;
		int v2 = (int)(unsigned char)c % 16;
		if((char)v1 < 10)
			buf[0] = (char)('0' + (char)v1);
		else
			buf[0] = (char)('A' + (char)v1 - 10);
		if((char)v2 < 10)
			buf[1] = (char)('0' + (char)v2);
		else
			buf[1] = (char)('A' + (char)v2 - 10);
		buf[2]= '\0';

		tchcat(rbuf, buf);// 0A0B0C...
	}
	return rbuf;
}


/// url 에서 & 로 분리된 파라미터를 = 로 나쥔 키와 값으로 분리
int KwGetUrlParams(PAS pUrl, CKRbVal<CString,CString>& params)
{
	//	SynchThis();
	CString sUrl(pUrl);//this->m_pReq->pRawUrl);//+	pUrl "/gps/?Req_10NewUser=0"
	int i0 = sUrl.Find('?');//ReverseFind('/');
	if(i0 < 0)
		return NULL;

	CString sParams = sUrl.Mid(i0 +1);
	CStringArray ar;
	KwCutStrByChar('&', sParams, ar);
	int npr = 0;
	for_count(ar)
	{
		CString k, v;
		int i1 = ar[_i].Find('=');
		if(i1 >= 0)
		{
			k = ar[_i].Left(i1);
			v = ar[_i].Mid(i1+1);
			params.SetAt(k, v);
			npr++;
		}
	}
	return npr;
}



// 구분자 갯수 만큼 배열항이 늘어 난다.
// 즉 222;;333;444 이면 항이 4개 이다. 중간에 ;; 가 ; 로 줄지 않는다.
// par이 NULL이면 갯수만 리턴 한다.
int KwCutStrByChar(TCHAR c, CString s, CStringArray& ar, bool bIgnorFirst, bool bIgnorLast)
{
	if(s.IsEmpty())
		return 0;

	int i0 = 0, i1= -1;
	for(int i=0;;i++)
	{
		i1 = s.Find(c, i0);
		if(i1 >= 0)
		{
			if(i1 == 0)
			{
				if(!bIgnorFirst)
					ar.Add(L"");
			}
			else//if(i1 != 0) //맨앞이 '~'이면 '~' 앞에는 널이므로 넣지 않는다. 즉 맨앞의 '~'는 무시 한다.
			{
				ar.Add(s.Mid(i0, i1 -i0));
				if(bIgnorLast && i1 == s.GetLength()-1)
					return (int)ar.GetCount();
			}
			i0 = i1+1;
		}
		else
			break;
	}

	if(i0 == s.GetLength())
		ar.Add(L"");
	else
		ar.Add(s.Mid(i0));
	return (int)ar.GetCount();
}


//#ifdef _Use_GUID
#pragma comment(lib, "Rpcrt4.lib")  //UuidCreateSequential

CString KwGetFormattedGuid(bool bHipn, PWS title, int left)//=true
{
	GUID guid;
	CString uuid;
	KwGetFormattedGUID(guid, uuid, bHipn);
	CString dvguid;
	if(title)
	{
		if(left > 0)
			dvguid = CString(title) + L"-" + uuid.Left(left);
		else
			dvguid = CString(title) + L"-" + uuid;
	}
	else
		dvguid = uuid;
	return dvguid;
}
int KwGetFormattedGUID(GUID& guid, CString& rString, bool bHipn)//=true
{
	guid = GUID_NULL;
	HRESULT hr = UuidCreate(&guid);
	if(HRESULT_CODE(hr) != RPC_S_OK)
		return 1;
	if(guid == GUID_NULL)
		return 2;
	if(HRESULT_CODE(hr) == RPC_S_UUID_NO_ADDRESS)
		return 3;
	if(HRESULT_CODE(hr) == RPC_S_UUID_LOCAL_ONLY)
		return 4;
	//		throw "Warning: Unable to determine your network address.\r\n  The UUID generated is unique on this computer only.\r\n  It should not be used on another computer.";

	//9EEFC1C5-C8E5-4d92-960B-7ACE4B953268
	PWS strFormat = bHipn ?
		L"%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"
		:L"%08lX%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X";
	rString.Format(strFormat,
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	return 0;
}
//#endif // _Use_GUID

/// <summary>
/// dl 로 문자열을 끊어서 ar에 넣어 보낸다.
/// </summary>
/// <param name="src"></param>
/// <param name="del"></param>
/// <param name="ar">clear한후 써야</param>
/// <param name="offset"></param>
/// <returns></returns>
int KwCutByToken(string& src, string dl, vector<string>& ar, __int64 offset)
{
	size_t off = (size_t)offset;
	size_t i0 = __1;// string::npos;
	while(1) 
	{
		i0 = src.find(dl, (size_t)off);
		if(i0 == __1)
		{
			ar.push_back(src.substr((size_t)off));
			break;
		}
		else
			ar.push_back(src.substr(off, i0 - off));
		off = i0 + dl.size();
	} //i0 != string::npos); //-1
	return (int)ar.size();
}

/// <summary>
/// 
/// </summary>
/// <param name="buf"></param>
/// <param name="dl1">"\r\n"</param>
/// <param name="dlkv">": "</param>
/// <param name="kv"></param>
/// <returns></returns>
int KwKeyValueStrToMap(PAS buf, PAS dl1, PAS dlkv, std::map<string, string>& kv)
{
	vector<string> ar;
	KwCutByToken(buf, dl1, ar);
	for(size_t i=0;i<ar.size();i++)
	{
		vector<string> ar1;
		KwCutByToken(ar[i], dlkv, ar1);
		if(ar1.size() == 2)
			kv[ar1[0]] = ar1[1];
	}
	return (int)kv.size();
}
/// <summary>
/// URL파라미터를 '&'로 구분 하여 '='로 키와 값으로 map에 넣는다.
/// </summary>
/// <param name="src">ex: "key=xxx&srl=5111"</param>
/// <param name="mp"></param>
/// <returns></returns>
int KwUrlParamToMap(string& src, std::map<string, string>& mp)
{
	vector<string> arp;
	auto np = KwCutByToken(src, "&", arp, 0);
	if(np == 0)
		return 0;
	for(int i=0;i<np;i++)
	{
		vector<string> kv;
		auto pr = KwCutByToken(arp[i], "=", kv);
		if(pr == 2)
			mp[kv[0]] = kv[1];
	}
	return (int)mp.size();
}


int KwUrlParamToJson(string& src, Kw::JObj& jmp)
{
	vector<string> arp;
	auto np = KwCutByToken(src, "&", arp, 0);
	if(np == 0)
		return 0;
	for(int i=0;i<np;i++)
	{
		vector<string> kv;
		auto pr = KwCutByToken(arp[i], "=", kv);
		if(pr >= 2)
			jmp(CStringW(kv[0].c_str())) = kv[1].c_str();
		//mp[kv[0]] = kv[1];
	}
	return (int)jmp.size();
}

/// <summary>
/// dir에서 맨앞 path가 lpath라 설정 하고 URL경로를 로컬경로로 바꾼다.
/// </summary>
/// <param name="vpath">가상경로</param>
/// <param name="lpath">로컬경로</param>
/// <param name="full">적용한 풀 로컬경로</param>
/// <returns>맨뒤가 파일명이 오면 full path를 리턴 한다. '.'확인</returns>
PAS KwUrlDirToVpath(PAS vpath, PAS lpath, string& full)
{
	vector<string> arPath; // api, somdir, file.ext
	auto np = KwCutByToken(vpath, "/", arPath, 0);
	if(np < 2)
		return nullptr;
	//ASSERT(arPath[0] == "api");
	full = lpath;
	if(full.substr(full.size()-1, 1) != "/")
		full += "/";
	for(size_t i=2;i<arPath.size();i++) // '/'로 시작 하니, 0:"", 1:"api", 2:부터 붙인다.
	{
		full += arPath[i].c_str();
		if(i != arPath.size() -1)
			full += "/";
		else if(arPath[i].find('.') < 0)
			return nullptr;
	}
	return full.c_str();
}

//https://docs.microsoft.com/en-us/windows/win32/wic/-wic-bitmapsources-howto-scale
HRESULT KwScaleBitmap(PWS full, int w, int h)
{

	HRESULT hr = S_OK;
	// Create WIC factory
	/*hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_pIWICFactory)
	);

	IWICBitmapDecoder* pIDecoder = NULL;
	IWICBitmapFrameDecode* pIDecoderFrame  = NULL;
	IWICBitmapScaler* pIScaler = NULL;


	hr = m_pIWICFactory->CreateDecoderFromFilename(
	   full,                  // Image to be decoded
	   NULL,                           // Do not prefer a particular vendor
	   GENERIC_READ,                   // Desired read access to the file
	   WICDecodeMetadataCacheOnDemand, // Cache metadata when needed
	   &pIDecoder                      // Pointer to the decoder
	);
	if(SUCCEEDED(hr))
		hr = pIDecoder->GetFrame(0, &pIDecoderFrame);

	// Create the scaler.
	if(SUCCEEDED(hr))
		hr = m_pIWICFactory->CreateBitmapScaler(&pIScaler);

	// Initialize the scaler to half the size of the original source.
	if(SUCCEEDED(hr))
	{
		hr = pIScaler->Initialize(
		   pIDecoderFrame,           // Bitmap source to scale.
		   w,                        // Scale width to half of original.
		   h,                        // Scale height to half of original.
		   WICBitmapInterpolationModeFant);   // Use Fant mode interpolation.
	}
	m_pIWICFactory->
		*/
	/*
	CImage img;
	CImage sm_img;
	HRESULT res = img.Load(full);
	CDC* screenDC = GetDC();

	int iNewWidth = 160;
	int iNewHeight = 120;

	sm_img.Create(iNewWidth, iNewHeight, 32);

	SetStretchBltMode(sm_img.GetDC(), COLORONCOLOR);
	img.StretchBlt(sm_img.GetDC(), 0, 0, iNewWidth, iNewHeight, SRCCOPY);
	// The next two lines test the image on a picture control.
	// The output has wrong color palette, like my initial version
	HDC hdcpic = ::GetDC(m_ClipPreview.m_hWnd);

	img.StretchBlt(hdcpic, 0, 0, iNewWidth, iNewHeight, SRCCOPY);

	sm_img.Save(jpgFileName);
	ReleaseDC(screenDC);

	*/
	return S_OK;

}


// szArea 안에 szObj 가 쏙 들어 가려면 szObj * dv 하면 안으로 쏙 들어 간다.
float KwCalcInplaceRate(CSize szArea, CSize szObj)
{
	//              500
	//       +------------------+
	//       |     image        | 150
	//       |                  |
	//       +------------------+
	// 
	//       +--------------+
	//       | draw area    |
	//       |              | 21000
	//       |    28000     |
	//       +--------------+
	// 
	// * 영역과 그림의 가로/세로 비율이 달라서 그림영역으로 쏙 들어 가게 하려면 
	//   가로의 비율로 할건지 세로의 비율로 할건지 결정 해야 한다.
	// 영역과 그림의 각각 가로/세로 비율 구한다.
	float di = fabsf((float)szObj.cx / szObj.cy);
	float dp = fabsf((float)szArea.cx / szArea.cy);

	float dv = 1.f;
	if(dp < di) //비율이 더 크면 더 넓적 하드는 뜻임: 그림이 넗으면 가로의 비율로 맞추어야 한다.
		dv = fabsf((float)szArea.cx / szObj.cx);
	else
		dv = fabsf((float)szArea.cy / szObj.cy);//세로 비율
	//if(bClearRate)//이게 뭐지? 필요한가?
	//	dv = dv;
	return dv;
}


// mapmode에 따라 rcArea.top, rcArea.bottom 이 < 0 일수 있다.
CRect KwCalcInplaceRect(CRect rcArea, CSize szImage, PAS sAlign)
{
	float dv = KwCalcInplaceRate(rcArea.Size(), szImage);
	CSize szdr = rcArea.Size();

	// 그림의 크기
	CSize szi((int)(szImage.cx * dv), (int)(szImage.cy * dv));
	if(szdr.cy < 0)
		szi.cy *= -1;
	//rc = CRect(CPoint(0,0), CSize((int)(m_image.GetWidth() * dv), (int)(m_image.GetHeight() * dv)));

	// 그림을 센터에 둘때 영역과 그림의 가로세로 비율에 의해 생기는 공간
	CPoint ptDest;
	CSize szOffset((szdr.cx - szi.cx) / 2, (szdr.cy - szi.cy) / 2);// hcenter vcenter
	if(tchstr(sAlign, "left"))
		szOffset.cx = 0;
	else if(tchstr(sAlign, "right"))
		szOffset.cx = (szdr.cx - szi.cx);

	if(tchstr(sAlign, "top"))
		szOffset.cy = (szdr.cy - szi.cy) / 2;
	else if(tchstr(sAlign, "bottom"))
		szOffset.cy = (szdr.cy - szi.cy);

	ptDest = CPoint(rcArea.left +szOffset.cx, rcArea.top +szOffset.cy);

	//CPoint ptSrc(0,0);

	CRect rcImage(ptDest, szi);
	return rcImage;
}


// 복사 한 후 IStream* 으로 리턴. 반드시 Release() 해줘야 한다.
IStream* KwMemoryToStream(LPVOID data, size_t size, HGLOBAL& hgm)
{
	IStream* p_stream = NULL;

	hgm = ::GlobalAlloc(GMEM_MOVEABLE, size);
	if(hgm == NULL)
		return NULL;//throw_str(L"GlobalAlloc Error!");
	void* p_buffer = ::GlobalLock(hgm);
	if(p_buffer == NULL)
		return NULL;//throw_str(L"GlobalLock Error!");

	CopyMemory(p_buffer, data, size);
	::GlobalUnlock(hgm);
	::CreateStreamOnHGlobal(hgm, FALSE, &p_stream);

	return p_stream;
}

// memory 있는 data를 원 크기와 여러 작은 크기로 저장한다.
// arSz는 반드시 [0] 에 CSize(0,0) 를 더미로 넣어 줘야 순서가 맞다.
int KwSaveImageMultiSize(LPVOID idata, size_t isize, CStringArray& arPath, CSize* arSz, PAS extSave)
{
	for(int i=0;i<arPath.GetCount();i++)
		KwCheckTargetDir(arPath[i], FALSE, TRUE);

	bool bSaveToJpeg = tchstr(extSave, "jpg") || tchstr(extSave, "jpeg");
	bool bSaveToPng = tchstr(extSave, "png");
	bool bResizeIfLarge = tchstr(extSave, "large");

	{
		CFile f(arPath[0], CFile::modeCreate|CFile::modeWrite);
		KAtEnd d_f([&]() { f.Close(); });
		f.Write(idata, (UINT)isize);
	}
	
	{//여기는 백그로 해야
		CImage img;
		HRESULT res = NULL;

		// 1. 이미 저장된 파일에서 읽어 하는 방법
		//img.Load(full);
		// 2. memory 복사하여 IStream으로 하는 방법
		HGLOBAL h_buffer = nullptr;
		IStream* p_stream = KwMemoryToStream((LPVOID)idata, isize, h_buffer);

		if(p_stream == NULL)
			return 1;//throw_str(L"IStream Error!");
		KAtEnd d_bust([&]() {
			if(p_stream)
				p_stream->Release();
			if(h_buffer)//쓰기 전에 이거 해도 되나? 안된다.
				::GlobalFree(h_buffer);
		});

		img.Load(p_stream);
		if (img.IsNull())
			return 2;

		// 0은 원래크기 1부터 축소하여 저장
		for(int i=1;i<arPath.GetCount();i++)
		{
			CImage sm_img;

			CSize isz(img.GetWidth(), img.GetHeight());
			CRect rc2(CPoint(0, 0), arSz[i - 1]);//크기는 배열에 원본 크기가 빠지 므로 -1
			CRect rc3;
			if (!bResizeIfLarge || isz.cx > rc2.Width() || isz.cy > rc2.Height())
				rc3 = KwCalcInplaceRect(rc2, isz);//원본이 더 클때만
			else
				rc3 = CRect(CPoint(0, 0), isz);//원본이 작으면 그대로

			sm_img.Create(rc3.Width(), rc3.Height(), 32);
			HDC smdc = sm_img.GetDC();
			KAtEnd d_DC([&]() {
				sm_img.ReleaseDC();
			});
			SetStretchBltMode(smdc, COLORONCOLOR);
			img.StretchBlt(smdc, 0, 0, rc3.Width(), rc3.Height(), SRCCOPY);
			//CDC* screenDC = GetDC();
			//HDC hdcpic = ::GetDC(m_ClipPreview.m_hWnd);
			//img.StretchBlt(hdcpic, 0, 0, iNewWidth, iNewHeight, SRCCOPY);
			//ReleaseDC(screenDC);
			if(!bSaveToJpeg && !bSaveToPng)
				sm_img.Save(arPath[i]);
			else
			{
				auto iext = GUID_NULL;
				if (bSaveToJpeg)
					iext = Gdiplus::ImageFormatJPEG;
				else if (bSaveToPng)
					iext = Gdiplus::ImageFormatPNG; //ImageFormatGIF, 
				sm_img.Save(arPath[i], iext);
			}
		}
		return 0;
	}
}


// full path 의 디렉토리를 만들어 준다.
// bToEnd가 FALSE이면 맨 마지막 것은 FILE NAME 이므로 만들 필요 없다는 뜻.
// reffer to: File::createDirectory : recursive call
int KwCheckTargetDir(PWS sFull, BOOL bToEnd, BOOL bCreate)
{
	CStringArray ar;
	KwCutByToken(sFull, L"\\", ar);
	CString sPath=ar[0];
	for(int i=1;i<=ar.GetUpperBound();i++)
	{
		if(i == ar.GetUpperBound() && bToEnd == FALSE) // last name is file not directory
			break;
		sPath += (TCHAR)'\\';
		sPath += ar[i];

		DWORD dw = ::GetFileAttributes(sPath);
		if(dw != INVALID_FILE_ATTRIBUTES)
		{
			if(dw & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			else
				return -1;// 디렉토리가 아냐?
		}
		else { // 없어
			if(bCreate) {
				if(CreateDirectory(sPath, NULL) == FALSE)
					return GetLastError();
			}
			else
				return -2;//없어
		}
	}
	return 0;
}


CStringA KwContentType(PAS ext)
{
	CStringA extension = tchtolower(ext);
	// Base content types
	if (extension == ".html")
		return ("text/html");
	else if (extension == ".css")
		return ("text/css");
	else if (extension == ".js")
		return ("text/javascript");
	else if (extension == ".xml")
		return ("text/xml");

	// Common content types
	if (extension == ".gzip")
		return ("application/gzip");
	else if (extension == ".json")
		return ("application/json");
	else if (extension == ".map")
		return ("application/json");
	else if (extension == ".pdf")
		return ("application/pdf");
	else if (extension == ".zip")
		return ("application/zip");
	else if (extension == ".mp3")
		return ("audio/mpeg");
	else if (extension == ".jpg" || extension == ".jpeg")
		return ("image/jpeg");
	else if (extension == ".gif")
		return ("image/gif");
	else if (extension == ".png")
		return ("image/png");
	else if (extension == ".svg")
		return ("image/svg+xml");
	else if (extension == ".mp4")
		return ("video/mp4");

	// Application content types
	if (extension == ".atom")
		return ("application/atom+xml");
	else if (extension == ".fastsoap")
		return ("application/fastsoap");
	else if (extension == ".ps")
		return ("application/postscript");
	else if (extension == ".soap")
		return ("application/soap+xml");
	else if (extension == ".sql")
		return ("application/sql");
	else if (extension == ".xslt")
		return ("application/xslt+xml");
	else if (extension == ".zlib")
		return ("application/zlib");

	// Audio content types
	if (extension == ".aac")
		return ("audio/aac");
	else if (extension == ".ac3")
		return ("audio/ac3");
	else if (extension == ".ogg")
		return ("audio/ogg");

	// Font content types
	if (extension == ".ttf")
		return ("font/ttf");

	// Image content types
	if (extension == ".bmp")
		return ("image/bmp");
	else if (extension == ".jpm")
		return ("image/jpm");
	else if (extension == ".jpx")
		return ("image/jpx");
	else if (extension == ".jrx")
		return ("image/jrx");
	else if (extension == ".tiff")
		return ("image/tiff");
	else if (extension == ".emf")
		return ("image/emf");
	else if (extension == ".wmf")
		return ("image/wmf");

	// Message content types
	if (extension == ".http")
		return ("message/http");
	else if (extension == ".s-http")
		return ("message/s-http");

	// Model content types
	if (extension == ".mesh")
		return ("model/mesh");
	else if (extension == ".vrml")
		return ("model/vrml");

	// Text content types
	if (extension == ".csv")
		return ("text/csv");
	else if (extension == ".plain")
		return ("text/plain");
	else if (extension == ".richtext")
		return ("text/richtext");
	else if (extension == ".rtf")
		return ("text/rtf");
	else if (extension == ".rtx")
		return ("text/rtx");
	else if (extension == ".sgml")
		return ("text/sgml");
	else if (extension == ".strings")
		return ("text/strings");
	else if (extension == ".url")
		return ("text/uri-list");

	// Video content types
	if (extension == ".H264")
		return ("video/H264");
	else if (extension == ".H265")
		return ("video/H265");
	else if (extension == ".mpeg")
		return ("video/mpeg");
	else if (extension == ".raw")
		return ("video/raw");

	return NULL;
}





bool KwTrimNumDotUnder(CString& sd)
{
	int i0 = sd.Find('.');
	if(i0 >= 0 && sd.GetLength() > (i0 + 1))
	{
		TCHAR buf[128];
		ASSERT(sd.GetLength() < 128);
		tchcpy(buf, (LPCTSTR)sd);
		for(int ipos = sd.GetLength() - 1; ipos >= i0; ipos--)
		{
			if(buf[ipos] == '0' || buf[ipos] == '.')
			{
				buf[ipos] = '\0';

				if(buf[ipos] == '.')
					break;
			}
			else
				break;
		}
		sd = buf;
		return true;
	}
	return false;
}



int KwRangedRand(int range_min, int range_max)
{
	//srand( (unsigned)time( NULL ) );

	// Generate random numbers in the half-closed interval [range_min, range_max). 
	// In other words, range_min <= random number < range_max
	int u = (int)((double)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min);
	return u;
}


//  62^7 =       3,521,614,606,208 :      3조
//  62^8 =     218,340,105,584,896 :    218조
//  62^9 =  13,537,086,546,263,552 :     13경
//  36^11= 238,572,050,223,552,512 : 23경
//  36^10=   3,656,158,440,062,976 :   3600조
//      9=     101,559,956,668,416 :    101조
//      8        2,821,109,907,456
static int s_jin = 36;//62;
static int s_jinC = 62;     //         10     + 26                      + 26
static const CHAR* s_cd = "0123456789abcdefghijklmnopqrstuvwxyz";
static const CHAR* s_cdC = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
CStringA KwCreateKeyStr(int len, bool bLong)
{
	return KwRandomEnNumStr(len, bLong ? 1 | 2 | 4 : 1 | 2);
}
CStringA KwRandomEnNumStr(int len, int eType)
{
	ASSERT(eType != 5);
	// 1: 0~9, 2:a~z, 4:A~Z
	//static CKSection s_cs;
	//SynchLock(s_cs);
	//	srand( (unsigned)time( NULL ) );
	int start = (eType & 1) ? 0 : 10;
	int jin = (eType & 1) ? 10 : 0;//숫자만
	if(eType & 2) jin += 26;
	if(eType & 4) jin += 26;

	PAS cd = s_cdC;//bLong ? :s_cd;

	CStringA ss1;
	char* s1 = ss1.GetBuffer(len + 1);
	s1[len] = '\0';
	int ndup = 0;
	for(int i = 0; i < len; i++)
	{
		int ri = KwRangedRand(0, jin);
		s1[i] = cd[ri + start];
	}
	ss1.ReleaseBuffer();
	return ss1;//CStringA(s1);
}

#pragma warning(disable: 4996)
//Project Properties > Configuration Properties > C/C++ > General > SDL checks [set to No]
//출처: https://driz2le.tistory.com/138 [홀로 떠나는 여행]
#pragma comment (lib, "version.lib")
CString KwGetWindowVersion()
{
	//DWORD dwVersion = 0;
	//DWORD dwMajorVersion = 0;
	//DWORD dwMinorVersion = 0;
	//DWORD dwBuild = 0;
	OSVERSIONINFOW VersionInformation{ 0 };
	VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
	GetVersionEx(&VersionInformation);
	
	// Get the Windows version.

	//VersionInformation.dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
	//VersionInformation.dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

	// Get the build number.
	//if(dwVersion < 0x80000000)
	//	dwBuild = (DWORD)(HIWORD(dwVersion));

	CString sv;
	sv.Format(L"%d.%d (%d)",
		VersionInformation.dwMajorVersion,
		VersionInformation.dwMinorVersion,
		VersionInformation.dwBuildNumber);
	return sv;
}

CStringA KwComputerNameA()
{
	CStringA s;
	DWORD nSize = 1024;
	LPSTR lpBuffer = s.GetBuffer(nSize);
	GetComputerNameA(lpBuffer, &nSize);
	s.ReleaseBuffer();
	return s;
}

CString KwGetProductVersion()
{
	CString strResult;

	WCHAR szModPath[MAX_PATH];
	szModPath[0] = '\0';
	GetModuleFileName(NULL, szModPath, sizeof(szModPath));
	DWORD dwHandle;
	DWORD dwSize = GetFileVersionInfoSize(szModPath, &dwHandle);

	if(dwSize > 0)
	{
		BYTE* pbBuf = static_cast<BYTE*>(alloca(dwSize));
		if(GetFileVersionInfo(szModPath, dwHandle, dwSize, pbBuf))
		{
			UINT uiSize;
			BYTE* lpb = nullptr;
			if(VerQueryValue(pbBuf,
				L"\\VarFileInfo\\Translation",
				(void**)&lpb,
				&uiSize))
			{
				WORD* lpw = (WORD*)lpb;
				CString strQuery;
				strQuery.Format(L"\\StringFileInfo\\%04x%04x\\ProductVersion", lpw[0], lpw[1]);

				if(VerQueryValue(pbBuf, const_cast<LPWSTR>((LPCWSTR)strQuery), (void**)&lpb,
					&uiSize) && uiSize > 0)
				{
					strResult = (LPCSTR)lpb;
				}
			}
		}
	}

	return strResult;
}

#include <WinInet.h>
CString KwGetConnectedState()
{
	DWORD dwFlags{ 0 };
	InternetGetConnectedState(&dwFlags, 0);
	CString sErr;
	switch(dwFlags)
	{
		CASE_ERR(INTERNET_CONNECTION_CONFIGURED);
		CASE_ERR(INTERNET_CONNECTION_LAN);
		CASE_ERR(INTERNET_CONNECTION_MODEM);
		CASE_ERR(INTERNET_CONNECTION_MODEM_BUSY);
		CASE_ERR(INTERNET_CONNECTION_OFFLINE);
		CASE_ERR(INTERNET_CONNECTION_PROXY);
		CASE_ERR(INTERNET_RAS_INSTALLED);
	}
	return sErr;
}

CString KwSystemInfo()
{
	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	CString sif;
	CStringA s;
	//printf("Hardware information: \n");
	s.Format("OEM: %u\n", siSysInfo.dwOemId); 
	sif += s;
	s.Format("proc: %u, %u\n",siSysInfo.dwNumberOfProcessors, siSysInfo.dwProcessorType); 
	sif += s;
	s.Format("Page: %u\n", siSysInfo.dwPageSize); 
	sif += s;
	s.Format("MinMax Addr: %#lx,%#lx\n",siSysInfo.lpMinimumApplicationAddress, siSysInfo.lpMaximumApplicationAddress);
	sif += s;
	s.Format("ProcMask: %u\n",siSysInfo.dwActiveProcessorMask);
	sif += s;
	return sif;
}



BOOL KwTextToVar(COleVariant& m_varValue, const CString& strText)
{
	CString strVal = strText;

	switch(m_varValue.vt)
	{
	case VT_BSTR:
		m_varValue = (LPCTSTR)strVal;
		return TRUE;

	case VT_UI1:
		m_varValue = strVal.IsEmpty() ? (BYTE)0 : (BYTE)strVal[0];
		return TRUE;

	case VT_I2:
		m_varValue = (short)_ttoi(strVal);
		return TRUE;

	case VT_INT:
	case VT_I4:
		m_varValue = _ttol(strVal);
		return TRUE;

	case VT_UI2:
		m_varValue.uiVal = unsigned short(_ttoi(strVal));
		return TRUE;

	case VT_UINT:
	case VT_UI4:
#ifdef _UNICODE
		m_varValue.ulVal = wcstoul(strText, NULL, 10);
#else
		m_varValue.ulVal = strtoul(strText, NULL, 10);
#endif // _UNICODE
		return TRUE;

	case VT_R4:
	{
		float fVal = 0.;

		strVal.TrimLeft();
		strVal.TrimRight();

		if(!strVal.IsEmpty())
		{
			_stscanf_s(strVal, L"%f", &fVal);//AFX_FORMAT_FLOAT
		}

		m_varValue = fVal;
	}
	return TRUE;

	case VT_R8:
	{
		double dblVal = 0.;

		strVal.TrimLeft();
		strVal.TrimRight();

		if(!strVal.IsEmpty())
		{
			_stscanf_s(strVal, L"%lf", &dblVal);//AFX_FORMAT_DOUBLE
		}

		m_varValue = dblVal;
	}
	return TRUE;

	case VT_BOOL:
		strVal.TrimRight();
		CString stri(strVal);
		stri.MakeLower();
		m_varValue = (VARIANT_BOOL)(stri == L"true" || stri == L"yes"
			|| strVal == L"예" || stri == L"y" || stri == L"t");// m_pWndList->m_strTrue);
		return TRUE;
	}

	return FALSE;
}


BOOL KwNullToVar(COleVariant& m_varValue)
{
	switch(m_varValue.vt)
	{
	case VT_BSTR:
		m_varValue = (LPCTSTR)L"";
		return TRUE;

	case VT_UI1:
		m_varValue = (BYTE)0;
		return TRUE;

	case VT_I2:
		m_varValue = (short)0;
		return TRUE;

	case VT_INT:
	case VT_I4:
		m_varValue = (long)0;
		return TRUE;

	case VT_UI2:
		m_varValue.uiVal = unsigned short(0);
		return TRUE;

	case VT_UINT:
	case VT_UI4:
		m_varValue.ulVal = (ULONG)0;
		return TRUE;

	case VT_R4:
	{
		float fVal = 0.;
		m_varValue = fVal;
	}
	return TRUE;

	case VT_R8:
	{
		double dblVal = 0.;
		m_varValue = dblVal;
	}
	return TRUE;

	case VT_BOOL:
		m_varValue = (VARIANT_BOOL)0;
		return TRUE;
	}
	ASSERT(0);
	return FALSE;
}

void CKTrace::Output(const char* txt)
{
	AUTOLOCK(_csTrace);
	if(strcmp(txt, "\r\n") == 0)
	{
		if(_fncTrace)
		{
			if(!_debug)
				(*_fncTrace)(_s.str());//?ExTrace 6 Output에서 실행. 추가 출력은 줄바꿈을 안 넣고 내 보낸다.
									   // 				_sPrev.clear();
									   // 				_sPrev << _s.str();
		}
		_s << txt;
		//TRACEX(txt);
		OutputDebugStringA(_s.str().c_str());
		_s.str("");// = "";//.clear(); 이게 말을 안듣네.
	}
	else
		_s << txt;
}


BOOL KwCopyTextClipboad(CWnd* pWnd, LPCWSTR text)
{
	if(!pWnd->OpenClipboard())
	{
		//NTRACE(_T("클립보드 열기 실패!\n"));
		return FALSE;
	}
	if(!::EmptyClipboard())
	{
		//NTRACE(_T("클립보드 기존 데이타 지우기 실패!\n"));
		::CloseClipboard();
		return FALSE;
	}

	CStringA ta(text);

	size_t len = strlen((LPCSTR)ta);//tchlen
	HGLOBAL hData = ::GlobalAlloc(GMEM_DDESHARE, (len + 1));//*sizeof(TCHAR)
	if(hData == NULL)
	{
		//NTRACE(_T("클립보드로 복사중 메모리 할당 오류!\n");
		::CloseClipboard();
		return FALSE;
	}
	// now copy the strings, terminate each with CR/LF
	LPVOID lpOut = (LPVOID)GlobalLock(hData);
	if(lpOut == (LPVOID)NULL)
	{
		//NTRACE(_T("클립보드로 복사중 메모리 잠금 오류!\n");
		::CloseClipboard();
		return FALSE;
	}

	//KwStrcpy(lpOut, text);
	memcpy(lpOut, (LPCSTR)ta, len + 1);
	GlobalUnlock(hData);

	if(hData == NULL || ::SetClipboardData(CF_TEXT, hData) == NULL)
	{
		//NTRACE(_T("클립보드로 텍스트를 복사하는데 실패!\n");
		::CloseClipboard();
		return FALSE;
	}
	if(!::CloseClipboard())
	{   //NTRACE(_T("클립보드 닫기 실패!\n");
		return FALSE;
	}
	return TRUE;
}


int KwFolderFileList(LPCTSTR pDir, CStringArray& ar, BOOL bRecursive)
{
	CString path1 = pDir;

	if(path1.Right(1) != '\\')
		path1 += '\\';

	WIN32_FIND_DATA wfd;

	HANDLE h = FindFirstFile(path1 + L"*.*", &wfd);
	if(h == INVALID_HANDLE_VALUE)
		return -1;

	CStringArray arSub;
	for(;;)
	{
		CString sFile = wfd.cFileName;
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if(bRecursive)
			{
				if(sFile != L"." && sFile != L"..")
					arSub.Add(sFile);
			}
		}
		else
		{
			CString sFile1 = path1 + sFile;
			ar.Add(sFile1);
		}

		if(FindNextFile(h, &wfd) == FALSE)
			break;
	}
	FindClose(h);

	if(bRecursive)
	{
		for(int i = 0; i <= arSub.GetUpperBound(); i++)
		{
			KwFolderFileList(path1 + arSub[i], ar, bRecursive);
		}
	}

	return 0;
}


LPCSTR KwReadSmallTextFileA(PWS fileName, CStringA& str)
{
	//	ASSERT(sizeof(TCHAR) == 1);
	CFile stream;
	if(stream.Open((LPCTSTR)fileName, CFile::modeRead) == 0) //|CFile::typeText) == 0)
	{
		DWORD dwErr = GetLastError(); // 32:ERROR_SHARING_VIOLATION
		return NULL;
	}

	DWORD fileLength = (DWORD)stream.GetLength();
	LPSTR fileBuf = NULL;
	try
	{
		bool bUnicode = false;
		if(fileLength == 0)
			throw 1;
		const WORD s_wfeff = 0xfeff;
		if(fileLength > 2)
		{
			WORD wfeffr;
			if(stream.Read(&wfeffr, (UINT)sizeof(WORD)) != (UINT)sizeof(WORD))
				throw 2;
			bUnicode = wfeffr == s_wfeff;
		}

		if(bUnicode)
		{
			stream.Close();
			ASSERT(0);//KwIsUnicodeTextFile 를 먼저 불러 봐야 한다.
			return NULL;
		}

		UINT len = 0;
		UINT lenChar = 0;

		{
			int hd = bUnicode ? 2 : 0;
			len = lenChar = fileLength - hd; // 0xfeff 를 뺀 것에서 2(WORD)로 나누면
			stream.Seek(CFile::begin, hd); // 0xfeff 없으니 다시 앞으로
		}
		fileBuf = (LPSTR)str.GetBuffer(len + 1);
		if(stream.Read(fileBuf, (UINT)len) != (UINT)len)
			throw 3;

		fileBuf[lenChar] = '\0';
		str.ReleaseBuffer();//lenChar
	}
	catch(...)
	{
		stream.Close();
		return NULL;
	}

	stream.Close();
	return (LPCSTR)str;
}

/// <summary>
/// 없는 파일이나 경로 여도 값은 잘라 준다.
/// </summary>
/// <param name="lpszFileIn"></param>
/// <param name="path"></param>
/// <param name="pFilePart"></param>
/// <returns>If the function succeeds, the return value is the length.fails for zero. </returns>
DWORD KwGetFullPathName(PWS lpszFileIn, CStringW& path, PWS* pFilePart)
{
	LPTSTR lpszFilePart;
	DWORD dwRet = GetFullPathName(lpszFileIn, 1022, path.GetBuffer(1024), &lpszFilePart);
	path.ReleaseBuffer();
	if (pFilePart)
		*pFilePart = lpszFilePart;
	return dwRet;
}


UINT_PTR EncodeBinary(KBinary& bin, LPCSTR key0, bool bEncode, KBinary* pbinr)
{
	BYTE* key = (BYTE*)key0;
	DWORD j;
	BYTE cf, cl;
	int lk = lstrlenA(key0);

	KBinary src;
	KBinary* psrc;

	psrc = &bin;//소스
	pbinr->Alloc(psrc->m_len);
	// pbinr 이 타겟
	for(j = 0; j < psrc->m_len; j++)
	{
		// key 앞에서 뒤쪽으로, 뒤쪽에서 앞으로 한글자씩 두자를 골라
		//       j % 4
		cf = key[j % lk];            // 0123 0123 0123
		cl = key[lk - (j % lk) - 1]; // 3210 3210 3210

		if(bEncode)
			pbinr->m_p[j] = (~(((BYTE)psrc->m_p[j]) ^ cf)) ^ cl;
		else
			pbinr->m_p[j] = (~(((BYTE)psrc->m_p[j]) ^ cl)) ^ cf;
	}
	return pbinr->m_len;;
}

LONGLONG KwGetTickCount100Nano()
{
	LARGE_INTEGER li{0};
	auto bnow = QueryPerformanceCounter(&li);
	//auto tik = GetTickCount64();
	return li.QuadPart;
}


#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

hres KwGetMacInfo(CString& localIP, CString& macAddr)
{
	PIP_ADAPTER_INFO ainf;
	DWORD dwBufLen = sizeof(ainf);
	//CStringchar *mac_addr = (char*)malloc(17);

	ainf = (IP_ADAPTER_INFO*) new char[sizeof(IP_ADAPTER_INFO)];
	if(ainf == NULL) {
		TRACE(L"Error allocating memory needed to call GetAdaptersinfo\n");
	}
	CAutoFreePtr<IP_ADAPTER_INFO> _au(ainf);

	// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen     variable
	if(GetAdaptersInfo(ainf, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
		_au.Free();
		ainf = (IP_ADAPTER_INFO*)malloc(dwBufLen);
		if(ainf == NULL) {
			TRACE(L"Error allocating memory needed to call GetAdaptersinfo\n");
		}
	}

	CString adapter, desc;
#ifdef _DEBUGx
	이더넷 어댑터 로컬 영역 연결 :

	연결별 DNS 접미사. . . . :
		설명. . . . . . . . . . . . : Intel(R) Ethernet Connection I217 - V
		물리적 주소 . . . . . . . . : D0 - 50 - 99 - 66 - DC - 79
		DHCP 사용 . . . . . . . . . : 예
		자동 구성 사용. . . . . . . : 예
		링크 - 로컬 IPv6 주소 . . . . : fe80::9cef : 5b71 : 230e : 1f1a % 11(기본 설정)
		IPv4 주소 . . . . . . . . . : 192.168.2.100(기본 설정)
		서브넷 마스크 . . . . . . . : 255.255.255.0
		임대 시작 날짜. . . . . . . : 2016년 10월 3일 월요일 오전 7 : 52 : 26
		임대 만료 날짜. . . . . . . : 2016년 10월 13일 목요일 오전 7 : 55 : 54
		기본 게이트웨이 . . . . . . : 211.215.49.193
		110.12.134.1
		192.168.2.1
		DHCP 서버 . . . . . . . . . : 192.168.2.1
		DHCPv6 IAID . . . . . . . . : 248533145
		DHCPv6 클라이언트 DUID. . . : 00 - 01 - 00 - 01 - 1C - 9B - B7 - 68 - D0 - 50 - 99 - 66 - DC - 79
		DNS 서버. . . . . . . . . . : 192.168.2.1
		Tcpip를 통한 NetBIOS. . . . : 사용
		+ AdapterName	0x09aa62b0 "{41E0649D-21A6-40D0-8D04-7166DED6B5B5}"	char[260]
		+ Description	0x09aa63b4 "Intel(R) Ethernet Connection I217-V"	char[132]
		AddressLength	6	unsigned int
		- Address	0x09aa643c "?셟?"	unsigned char[8]
		[0x0]	0xd0 '?'	unsigned char
		[0x1]	0x50 'P'	unsigned char
		[0x2]	0x99 '?'	unsigned char
		[0x3]	0x66 'f'	unsigned char
		[0x4]	0xdc '?'	unsigned char
		[0x5]	0x79 'y'	unsigned char
		[0x6]	0x00	unsigned char
		[0x7]	0x00	unsigned char
		+ macAddr	"D0:50:99:66:DC:79"	ATL::CStringT<wchar_t, StrTraitMFC<wchar_t, ATL::ChTraitsCRT<wchar_t> > > &

		// not twisted wan
		-IpAddressList{Next = 0x00000000 IpAddress = {...} IpMask = {...} ...}	_IP_ADDR_STRING
		+ IpAddress{String = 0x09aa6458 "110.12.134.50"}	IP_ADDRESS_STRING
		+ IpMask{String = 0x09aa6468 "255.255.255.0"}	IP_ADDRESS_STRING
		Context	0x32860c6e	unsigned long
		- GatewayList{Next = 0x09aa6528 IpAddress = {...} IpMask = {...} ...}	_IP_ADDR_STRING
		+ IpAddress{String = 0x09aa6480 "211.215.49.193"}	IP_ADDRESS_STRING
		+ IpMask{String = 0x09aa6490 "255.255.255.255"}	IP_ADDRESS_STRING
		Context	0xc131d7d3	unsigned long
		- DhcpServer{Next = 0x00000000 IpAddress = {...} IpMask = {...} ...}	_IP_ADDR_STRING
		+ IpAddress{String = 0x099e04b0 "172.23.237.97"}	IP_ADDRESS_STRING
		+ IpMask{String = 0x099e04c0 "255.255.255.255"}	IP_ADDRESS_STRING
		Context	0x61ed17ac	unsigned long

		//  twisted wan
		- IpAddressList{Next = 0x00000000 IpAddress = {...} IpMask = {...} ...}	_IP_ADDR_STRING
		+ IpAddress{String = 0x09766458 "192.168.2.100"}	IP_ADDRESS_STRING
		+ IpMask{String = 0x09766468 "255.255.255.0"}	IP_ADDRESS_STRING
		Context	0x6402a8c0	unsigned long
		- GatewayList{Next = 0x09766528 IpAddress = {...} IpMask = {...} ...}	_IP_ADDR_STRING
		+ IpAddress{String = 0x09766480 "211.215.49.193"}	IP_ADDRESS_STRING
		+ IpMask{String = 0x09766490 "255.255.255.255"}	IP_ADDRESS_STRING
		Context	0xc131d7d3	unsigned long
		- DhcpServer{Next = 0x00000000 IpAddress = {...} IpMask = {...} ...}	_IP_ADDR_STRING
		+ IpAddress{String = 0x097664a8 "192.168.2.1"}	IP_ADDRESS_STRING
		+ IpMask{String = 0x097664b8 "255.255.255.255"}	IP_ADDRESS_STRING
		Context	0x0102a8c0	unsigned long

#endif // _DEBUGx
		if(GetAdaptersInfo(ainf, &dwBufLen) == NO_ERROR) {
			PIP_ADAPTER_INFO pAinf = ainf;// Contains pointer to current adapter info
			int i = 1;
			do {
				if(pAinf)
				{
					macAddr.Format(L"%02X:%02X:%02X:%02X:%02X:%02X",
						pAinf->Address[0], pAinf->Address[1],
						pAinf->Address[2], pAinf->Address[3],
						pAinf->Address[4], pAinf->Address[5]);
					localIP = CString(pAinf->IpAddressList.IpAddress.String);
					adapter = CString(pAinf->AdapterName);
					desc = CString(pAinf->Description);
					TRACE(L"%d. %s, %s, %s\n", i++, localIP, macAddr, desc);
					return S_OK;
				}
				pAinf = pAinf->Next;
			} while(pAinf);
		}
	return -1;
}



#include <map>
/// <summary>
/// char* 를 static wchar_t* 로 리턴한다.
/// 스택변수 포인터가 아니고 static으로 보유하고 재사용 한다.
/// </summary>
/// <param name="k"></param>
/// <returns></returns>
LPCWSTR Pws(LPCSTR k)
{
	static std::map<std::string, std::wstring> smap;
	auto it = smap.find(k);
	if(it == smap.end())
	{
		CStringW sw(k);
		smap[k] = (LPCWSTR)sw;
		it = smap.find(k);
	}
	return (LPCWSTR)it->second.c_str();
}
/// 반대방향으로 static pointer를 리턴. 별로 쓸일이 없지만
LPCSTR Pas(LPCWSTR k)
{
	static std::map<std::wstring, std::string> smap;
	auto it = smap.find(k);
	if(it == smap.end())
	{
		CStringA sa(k);
		smap[k] = (LPCSTR)sa;
		it = smap.find(k);
	}
	return (LPCSTR)it->second.c_str();
}

/// <summary>
/// resource 에서 읽은 문자열 static 문자열 포인터가 안전하게 리턴 하도록 static map에 보유 한다.
/// </summary>
/// <param name="idc"></param>
/// <returns></returns>
PWS KwRsc(int idc)
{
	static KStdMap<int, wstring> mapRsc;
	auto it = mapRsc.Find(idc);
	if(it == mapRsc.end())
	{
		CStringW s;
		BOOL b = s.LoadString(idc);
		ASSERT(b);
		mapRsc[idc] = (PWS)s;
		it = mapRsc.Find(idc);
	}
	return it->second.c_str();
}

PAS KwRscA(int idc)
{
	PWS ps = KwRsc(idc);
	return Pas(ps);
	// 	CStringA s;
	// 	s.LoadString(idc);
	// 	return s;
}
