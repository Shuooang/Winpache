#include "pch.h"

#include "KBinary.h"

#include "tchtool.h"
#include "timetool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// iVer = -1 이면 값을 쓰지 않는다.

// KBinary 를 껍질로만 사용 하고 버린다. 당연히 ps는 외부에서 free해야...
void KBinary::Wrap(const KBinary& bin)
{
	Wrap(bin.m_p, bin.m_len);
}
void KBinary::Wrap(LPCSTR ps, UINT_PTR len, int iVer)
{
	ASSERT(len < 100000000); //100메가 짜리가 있나? 있다면 그때 보지
	Free();
	if(len > 0)
	{
		m_len = len;
		m_capa = m_len;
		m_p = (CHAR*)ps;//Alloc(m_len);//new CHAR[m_len];//문자열이 아니고 byte이므로 CHAR 로 할당한다.
		//memcpy(m_p, ps, m_len);
		//m_p[m_len] = '\0'; // Alloc이 뒤에 여분 있게 잡는다. NewAlloc은 아니다.
	}
	SetVer(iVer);
	m_bOwner = false;
}

// ps may not be string.
void KBinary::SetPtr(LPCSTR ps, INT_PTR len, int iVer)
{
	ASSERT(m_bOwner);
	ASSERT(len != 0 && len < 100000000); //100메가 짜리가 있나? 있다면 그때 보지
	Free();

	if(len < 0)
		len = tchlen(ps);
	if(len > 0)
	{
		//m_len = len; Alloc에서 하네
		m_p = Alloc(len);//new CHAR[m_len];//문자열이 아니고 byte이므로 CHAR 로 할당한다.
		memcpy(m_p, ps, m_len);
		m_p[m_len] = '\0'; // Alloc이 뒤에 여분 있게 잡는다. NewAlloc은 아니다.
		m_p[m_len+1] = '\0'; // Alloc이 뒤에 여분 있게 잡는다. NewAlloc은 아니다.
		// m_p[m_len-1] may be '\0'.
	}
	SetVer(iVer);
}


void KBinary::_SerializeVer(CArchive& arc, int iOp)//, double dVersion)//dVersion은 여기서 사용 되지 않는다.
{
	if(arc.IsStoring())
	{
		SetVer(0);//?rule runtime 변수 이므로 초기화
		arc << (UINT_PTR)m_len;//4기가 맥스
		if(m_len > 0)
		{
			ASSERT(m_p);
			arc.Write(m_p, (UINT)m_len);//2
		}
	}
	else
	{
		UINT_PTR len = 0;
		arc >> len;
		if(Alloc(len)) // len+& bytes allocated
		{
			arc.Read(m_p, (UINT)m_len);//2
			m_p[m_len] = (char)'0';
		}
	}
}


#define Clone_Val(M_VAL) M_VAL = pSrc->M_VAL

void KBinary::Clone(const KBinary* pSrc, int iOp)
{
	this->Free();

	m_len = pSrc->m_len;
	m_capa = pSrc->m_capa;
	m_version = pSrc->m_version;
	m_multipleSize = pSrc->m_multipleSize;

	if(m_len > 0)
	{
		ASSERT(m_p == NULL);
		m_p = NewAlloc(m_capa);
		memcpy(m_p, pSrc->m_p, m_capa);
	}
}
KBinary::~KBinary()
{
	Free();
}
void KBinary::Free()
{
	if(m_bOwner) // Detatch 한경우만 free하지 않는다.
		DeleteMeSafe(m_p);
	else
		m_p = NULL;
	m_capa = m_len = 0;
}

CHAR* KBinary::NewAlloc(UINT_PTR len)
{
	ASSERT(0 <= len && len < 100000000); // 100 Mega max
	if(0 < len)
		return new CHAR[len];
	return NULL;
}

CHAR* KBinary::Alloc(UINT_PTR len, bool bZeroFill, int valset)
{
	Free();
	if(len > 0)
	{	// 2byte(WORD)는 최소한 더 확보된(\0') 8배수로 할당
		UINT_PTR len1 = MUTIPLELEN(len+2, m_multipleSize);//(( (len+1) /m_multipleSize) +1) * m_multipleSize; // 2(sizeof(wchar) 여분있는4배수형태로 2:4 3:4 4:8
		ASSERT(m_p == NULL);
		m_p = NewAlloc(len1); ASSERT((len+1) < len1);
		m_capa = len1;

		if(bZeroFill)
			memset(m_p, valset, len1);
		else
		{
			// 뒤 8바이트를 0 으로 채워
			// 			char* pz = len1 > 4 ? m_p+(len1 -4) : m_p;
			// 			int lenz = len1 > 4 ? 4 : len1;
			m_p[0] = m_p[len] = m_p[len -1] = m_p[len1 -1] = '\0';//데이터 끝이 일단 넣어 둔다.
		}
		m_len = len;
	}
	return m_p; // can be NULL
}
// 데이터 복사
CHAR* KBinary::Resize(UINT_PTR len, bool bZeroFill)
{
	if(len > 0)
	{
		UINT_PTR len1 = MUTIPLELEN(len+2, m_multipleSize);//(( (len+1) /m_multipleSize) +1) * m_multipleSize; // 2(sizeof(wchar) 여분있는4배수형태로 2:4 3:4 4:8
		char* p = NewAlloc(len1); ASSERT((len+2) < len1);

		if(m_p)
		{
			if(m_capa <= len1)
			{
				memcpy(p, m_p, m_capa);
				if(bZeroFill)
					memset(p+m_len, 0, len1-m_len);
			}
			else // 더 작아져서 잘린다. 실제 보다 2바이트 널로 채워지므로 더 잘릴수 있다.
			{
				memcpy(p, m_p, len1);
				if(bZeroFill)
				{
					ASSERT(len1 >= 2);
					p[len1-1] = p[len-2] = '\0';
				}
				m_len = len1-2;
			}
			DeleteMeSafe(m_p);
		}
		m_capa = len1;
		m_p = p;
	}
	else
		Free();
	return m_p; // can be NULL
}
CHAR* KBinary::ReAlloc(UINT_PTR len)
{
	Free();
	if(len > m_len)
	{
		UINT_PTR len1 = MUTIPLELEN(len+2, m_multipleSize);//(( (len+1) /m_multipleSize) +1) * m_multipleSize; // 2(sizeof(wchar) 여분있는4배수형태로 2:4 3:4 4:8
//		UINT_PTR len1 = (( (len+1) /4) +1) * 4;
		char* p = m_p;
		UINT_PTR lenp = m_len;
		m_p = NewAlloc(len1); ASSERT((len+2) < len1);
		m_capa = len1;

		if(m_p)
		{
#ifdef _DEBUG
			ASSERT(len1 >= (len+2));
#endif // _DEBUG
			memcpy(m_p, p, lenp);
			DeleteMeSafe(p);
			m_p[len] = '\0';//경계밖 끝... not null terminated data인 경우
			m_p[len+1] = '\0';//경계밖 끝... not null terminated data인 경우
			m_len = len;
		}
		else
		{
			m_p[0] = '\0';
			m_p[len] = '\0';//경계밖 끝... not null terminated data인 경우
			m_p[len+1] = '\0';//경계밖 끝... not null terminated data인 경우
			m_p[len -1] = '\0';//데이터 끝이 일단 넣어 둔다.
			m_len = len;
		}
	}
	return m_p; // can be NULL
}

// 적어도 하나의 '\0'가 들어 있나? .. 안들어 있을수도 있지.. 바이너리 이면.. 가급적 맨뒤에는 넣어 두는게 좋을듯

bool KBinary::CheckNullTerm()
{
	return CheckNullTermT((TCHAR)0);
}

bool KBinary::CheckNullTermA()
{
	return CheckNullTermT((CHAR)0);
}
bool KBinary::CheckNullTermW()
{
	return CheckNullTermT((WCHAR)0);
}
CString& KBinary::GetStr(CString& sBuf)
{
	ASSERT(CheckNullTerm());
	if(m_p)
		sBuf = (LPCTSTR)m_p;
	else
		sBuf.Empty();
	return sBuf;//CString(m_p);
}

bool KBinary::IsSame(KBinary* pSrc, int iOp )
{
	KBinary* pbin = (KBinary*)pSrc;
	return m_len == pbin->m_len && memcmp(m_p, pbin->m_p, pbin->m_len) == 0;
}


ULONGLONG KBinary::GetUL()
{
	ASSERT(m_p);
	return _tcstoui64(GetP(), NULL, 10);
}
__int64 KBinary::GetI64()
{
	ASSERT(m_p);
	return _tcstoi64(GetP(), NULL, 10);
}

UINT KBinary::GetU()
{
	ASSERT(m_p);
	return (UINT)_ttoi64(GetP());
}
DWORD KBinary::GetDW()
{
	ASSERT(m_p);
	return (DWORD)_ttoi64(GetP());
}
int KBinary::GetI()
{
	ASSERT(m_p);
	return KwAtoi(GetP());
}
int KBinary::GetId(int idef)
{
	if(m_p && lstrlen((LPCTSTR)m_p))
		return KwAtoi(GetP());
	else
		return idef;
}

double KBinary::GetD()
{
	ASSERT(m_p);
	return _wtof(GetP());
}
CTime KBinary::GetT()
{
	ASSERT(m_p);
	CTime t = KwParseTimeStr4(GetP());
	return t;
}





UINT CFileArchive::Read(void* lpBuf, UINT nMax)
{
	ASSERT(lpBuf != NULL);
	ASSERT(m_file == NULL);

	return (UINT)fread(lpBuf, nMax, 1, m_file);
}

void CFileArchive::Write(const void* lpBuf, UINT nMax)
{
	ASSERT(lpBuf != NULL);
	ASSERT(m_file == NULL);
	fwrite(lpBuf, nMax, 1, m_file);
}

void CBufArchive::Write(const void* lpBuf, UINT nMax)
{
	// 	ASSERT(m_lpBufStart);
	// 	ASSERT(m_lpBufCur);
	UINT_PTR iCur = GetCurPos();
	//AppLog("CBufArchive::Write. lpBuf(%x) nMax(%u) m_nBufSize(%u) iCur(%u) %x, %x", lpBuf, nMax, m_nBufSize, iCur, m_lpBufStart, m_lpBufCur);
	if(nMax == 0)
		return;
	ASSERT(lpBuf != NULL);
	if(lpBuf == NULL)
		return;
	ASSERT(IsStoring());
	UINT_PTR nBufSize = m_nBufSize == 0 ? 256 : m_nBufSize;
	// AppLog("1. nMax(%u) m_nBufSize(%u) iCur(%u) %x, %x", nMax, m_nBufSize, iCur, m_lpBufStart, m_lpBufCur);
	while(nBufSize < (iCur + nMax + 4)) // too small. 4byte는 여유 있게
		nBufSize += nBufSize; // 증가는 2배씩 기하 급수적으로. m_nGrowSize는 deprecated
	// AppLog("2. nMax(%u) m_nBufSize(%u)m_nGrowSize(%u) iCur(%u) %x, %x", nMax, m_nBufSize, m_nGrowSize,iCur, m_lpBufStart, m_lpBufCur);
	if(nBufSize != m_nBufSize)
	{
		BYTE* pPrev = m_lpBufStart;
		m_lpBufStart = new BYTE[nBufSize];
		if(iCur > 0)
			memcpy(m_lpBufStart, pPrev, iCur);//, nBufSize
		DeleteMeSafe(pPrev);
		m_lpBufCur = m_lpBufStart +iCur;
		//  AppLog("21. nMax(%u) m_nBufSize(%u)m_nGrowSize(%u) iCur(%u) %x, %x", nMax, m_nBufSize, m_nGrowSize,iCur, m_lpBufStart, m_lpBufCur);
		m_nBufSize = nBufSize;
	}
	// AppLog("3. nMax(%u) m_nBufSize(%u) iCur(%u) %x, %x", nMax, m_nBufSize, iCur, m_lpBufStart, m_lpBufCur);
	memcpy(m_lpBufCur, lpBuf, nMax);//, m_nBufSize
	m_lpBufCur += nMax;
	Terminate();//m_lpBufCur[0] = '\0';//뒤에 문자열 끝 볼수 있게
}
UINT CBufArchive::Read(void* lpBuf, UINT nMax)
{//LogLine();
	UINT_PTR iCur = GetCurPos();
	if(nMax == 0)
		return 0;
	ASSERT(lpBuf != NULL);
	if(lpBuf == NULL)
		return 0;
	ASSERT(IsLoading());

	UINT_PTR nToRead = nMax;
	if(m_nBufSize < (iCur + nMax)) // too small
		nToRead = m_nBufSize -iCur;
	memcpy(lpBuf, m_lpBufCur, nToRead);//, nMax);
	m_lpBufCur += nToRead;
	//AppLog("CBufArchive::Read. lpBuf(%x) nMax(%u == %u) nMax(%u) m_nBufSize(%u) iCur(%u) %x, %x", lpBuf, nMax, nToRead, m_nBufSize, iCur, m_lpBufStart, m_lpBufCur);
	ASSERT(nToRead <= 0xffffU);
	return (UINT)nToRead;
}

BYTE* CBufArchive::Alloc(UINT_PTR uSize)
{
	Free();
	if(uSize > 0)
	{
		m_lpBufStart = new BYTE[uSize];
		m_lpBufCur = m_lpBufStart;
		m_nBufSize = uSize;
	}
	return m_lpBufStart;
}

void CKArchive::WriteCount(DWORD_PTR dwCount)
{
	ASSERT(0);//무조건64비트로 저장 하도록 수정
	if(dwCount < 0xFFFF)
		*this << (WORD)dwCount;  // 16-bit count
	else
	{
		*this << (WORD)0xFFFF;
#ifndef _WIN64
		* this << (DWORD)dwCount;  // 32-bit count
#else  // _WIN64
		if(dwCount < 0xFFFFFFFF)
			*this << (DWORD)dwCount;  // 32-bit count
		else
		{
			*this << (DWORD)0xFFFFFFFF;
			*this << dwCount;
		}
#endif  // _WIN64
	}
}

DWORD_PTR CKArchive::ReadCount()
{
	ASSERT(0);
	WORD wCount;
	*this >> wCount;
	if(wCount != 0xFFFF)
		return wCount;

	DWORD dwCount;
	*this >> dwCount;
#ifndef _WIN64
	return dwCount;
#else  // _WIN64
	if(dwCount != 0xFFFFFFFF)
		return dwCount;

	DWORD_PTR qwCount;
	*this >> qwCount;
	return qwCount;
#endif  // _WIN64
}