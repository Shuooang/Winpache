#include "pch.h"
#include "KVal.h"
#include "tchtool.h"
#include "Kw_tool.h"
#include "TimeTool.h"

CLocalTime CLocalTime::s_ltime;
LONG CUbj::s_numU = 0;


void CUbj::Reset()
{
	switch(m_type)
	{
	case eWstr:
	{
#ifdef _DEBUGx
		KTrace(L"Reset() m_u.wstr: (%#0X) %s\n", m_u.wstr, CTSTOAS(m_u.wstr));
		if(m_u.wstr)
			if(tchcmp(m_u.wstr, L"-102") == 0)
				_break;
#endif // _DEBUG
		DeleteMeSafe(m_u.wstr);
	}	break;
	case eUtf8:
	case eMbcs:DeleteMeSafe(m_u.str); break;
	/*case eObj:
		// 이미 삭제 되었을수도 있는 obj를 dynamic_cast만 해도 오류 난다.
		//if(dynamic_cast<IXmlMap*>(m_u.obj) == NULL)
		if(KwAttr(m_u.obj.m_option, eEnc_Owner) && m_u.obj.pbj)
		{
			//?주의: 삭제 하지 않아야 하는 이유
			//       각 obj는 사실 m_pChild와 m_pRight로 연결 되는 CKXmlBase로 이어 진다.
			//       결국 각 링크에 따라 DeleteAll 에서 삭제 되므로 
			//       각 map 안에서 val이 pbj인 경우 어느 경우도 삭제 해서는 안된다.
			//       여기 뿐만 아니라 CMapFound 에서 CXmlProj 도 삭제 되어서는 안되므로
			//       CMapFound.m_bOwner를 false로 해야 한다.
			//?정책변경 
			//       link에서 삭제 하지 않고 map 안에서 연결된 
			//       value에서 할당된 m_u.obj에서 삭제 해야 한다.

			//?주의 2 IXmlMap* pbj 는 반드시 virtual ~destructor를 가져야 제대로 삭제 된다.
			ISerializable* psrl = dynamic_cast<ISerializable*>(m_u.obj.pbj);
			DeleteMeSafePbjU(psrl);//m_u.obj.pbj);
			m_u.obj.pbj = NULL;
			m_u.obj.m_option &= ~eEnc_Owner;//FALSE;
		}
		else
		{
			m_u.obj.pbj = NULL;
			m_u.obj.m_option &= ~eEnc_Owner;//FALSE;
		}
		//	m_u.obj->_DestroyMe(); // XmlMap 인 경우 CKXmlBase::DeleteAll 에서 recursive로 삭제 된다.
		break;*/
	case eBin:
		DeleteMeSafe(m_u.bin.data);
		m_u.bin.len = 0;
		break;
	}
	KwZeroMemory(m_u);
}

void CUbj::Clone(CUbj* pSrc0, int iOp /*= eCloneDefault*/)
{
	//__super::_Clone(pSrc0, iOp);
	CUbj* pSrc = dynamic_cast<CUbj*>(pSrc0);
	Reset();

	m_type = pSrc->m_type;
	ASSERT(m_type != eULLong);

	switch(m_type)
	{
	case eWstr:
	{
		if(pSrc->m_u.wstr)
		{
			size_t ul = tchlen(pSrc->m_u.wstr);
			m_u.wstr = new wchar_t[ul + 1];// including '\0'
			tchcpy(m_u.wstr, pSrc->m_u.wstr);
		}
		else
			DeleteMeSafe(m_u.wstr);
	}	break;
	case eUtf8:
	case eMbcs:
	{
		ASSERT(m_u.str == NULL);
		size_t ul = strlen(pSrc->m_u.str);
		m_u.str = new char[ul + 1];// including '\0'
		tchcpy(m_u.str, pSrc->m_u.str);
	}	break;
	/*case eObj:
	{
		IXmlMap* pxlT = (m_u.obj.pbj);
		IClonable* picT = dynamic_cast<IClonable*>(pxlT);

		ISerializable* psrlS = dynamic_cast<ISerializable*>(pSrc->m_u.obj.pbj);
		if(m_u.obj.pbj == NULL)
		{
			ISerializable* psrlT = psrlS->_CreateMe(iOp, __FUNCTION__);
			//pxlT = dynamic_cast<IXmlMap*>(psrlT);
			m_u.obj.pbj = dynamic_cast<IXmlMap*>(psrlT);
			m_u.obj.m_option |= eEnc_Owner;//TRUE;
			//TPtrObj pvoid = psrl->_CreateThis(iOp);
			//m_u.obj.pbj = (TPtrObj)pvoid;//새 생성
			picT = dynamic_cast<IClonable*>(psrlT);
		}
		ASSERT(picT); // clone target object는 미리 create되어 있어야 한다.

		IClonable* picS = dynamic_cast<IClonable*>(pSrc->m_u.obj.pbj);
		picT->_Clone(picS, iOp);
	}
	break;*/
	case eBin:
	{
		CopyBin(&pSrc->m_u.bin, &m_u.bin);
	}	break;
	break;
	default:
		m_u.ql = pSrc->m_u.ql;
		break;
	}
}


LPCWSTR CUbj::GetP()
{
	if(m_type == eWstr)
		return m_u.wstr;
	else
	{
		CString& sbuf = _buf.GetBuf();
		return GetTxt(sbuf);
	}
}

std::string CUbj::GetString()
{
	//CStringW sval;
	//GetTxt(sval);
	std::string str = GetTxtA();// (PAS)CStringA(sval);
	return str;
}

LPCSTR CUbj::GetTxtA()
{
	switch(m_type)
	{
	case eMbcs:	return m_u.str;
	default:
	{
		CStringA sbuf = _bufa.GetBuf();
		return GetTxtA(sbuf);
	}
	}
}
LPCWSTR CUbj::GetTxtW()
{
	switch(m_type)
	{
	case eWstr:	
		return m_u.wstr;// NULL일수도 있다.
	default:
	{
		CString& sbuf = _buf.GetBuf();
		return GetTxt(sbuf);
	}
	}
	return nullptr;
}

LPCSTR CUbj::GetTxtA(CStringA& sval)
{
	CStringA sbuf = _bufa.GetBuf();
	switch(m_type)
	{
	case eWstr:
		if(m_u.wstr == nullptr)// NULL일수도 있다.
			return nullptr;
		else
		{
			sbuf = CStringA(m_u.wstr);
		}
	case eUtf8:{
		CString ws;
		KwUTF8ToWchar(m_u.str, ws);
		sval = CStringA(ws);
	}  break;
	case eMbcs:	sval = CStringW(m_u.str);		break;
	default:
		throw_str(L"Not supported");
	}
	return nullptr;
}
LPCWSTR CUbj::GetTxt(CStringW& sval)
{
	switch(m_type)
	{
	case eWstr:	sval = m_u.wstr;			break;
	case eUtf8:	KwUTF8ToWchar(m_u.str, sval);	break;
	case eMbcs:	sval = CStringW(m_u.str);		break;
	case eInt:	sval.Format(L"%d", m_u.i);	break;
	case eShort:	sval.Format(L"%d", m_u.si);	break;
	case eLLong:	//	(EField0)iField	f0264TimeCoord	EField0
	{	sval.Format(L"%I64d", m_u.ql);
	}break;
	case eULLong:
	{	sval.Format(L"%I64u", m_u.uql);
	}break;
	case eUint:	sval.Format(L"%u", m_u.ui);	break;
	case eUShort:	sval.Format(L"%u", m_u.usi);	break;
	case eBool:	sval = m_u.b ? L"Y" : L"N";	break;
	case eChar:	sval.Format(L"%c", m_u.c);	break;
	case eByte:	sval.Format(L"%c", m_u.uc);	break;
	case eFloat:	sval.Format(L"%.8f", m_u.d);	KwTrimNumDotUnder(sval); break; // %.11f was
	case eDouble:	sval.Format(L"%.11f", m_u.d);	KwTrimNumDotUnder(sval); break;
	case eTime:
	{
		CKwTime tval(m_u.t);
		//wchar_t buf[20];
		sval = tval.TimeStr(); // "2010-10-18 17:12:00"
	}break;
	}
	return sval;
}

int CUbj::GetI()
{
	if(m_type == eInt)
		return m_u.i;
	else if(IsString())
	{
		switch(m_type)
		{
		case eWstr:
			if(KwIsDigit(m_u.wstr[0]) || m_u.wstr[0] == '-')
				return KwAtoi(m_u.wstr);
			break;
		case eUtf8:
		case eMbcs:
			if(KwIsDigit(m_u.str[0]) || m_u.str[0] == '-')
				return atoi(m_u.str);
			break;
		default:	ASSERT(0);
			throw - 10;
		}
	}
	else
	{
		try
		{
#ifdef _DEBUG
			switch(m_type)
			{
			case eLLong:
			case eULLong:
				throw m_type;
				break;

			default:
				return GetNum<int>();
				break;
			}
#else
			return GetNum<int>();
#endif // _DEBUG
		}
		catch(int)
		{
			ASSERT(0);//사례: m_type == eLLong 인데 int로 달래서...
		}
	}
	return 0;
}

int CUbj::GetNum()
{
	if(m_type == eInt)
		return m_u.i;
	else if(IsString())
	{
		switch(m_type)
		{
		case eWstr:
			if(KwIsDigit(m_u.wstr[0]) || m_u.wstr[0] == '-')
				return tchatonum(m_u.wstr);
			break;
		case eUtf8:
		case eMbcs:
			if(KwIsDigit(m_u.str[0]) || m_u.str[0] == '-')
				return tchatonum(m_u.str);
			break;
		default:	ASSERT(0);
			throw - 10;
		}
	}
	else
	{
		return GetI();
	}
	return 0;
}

__int64 CUbj::GetI64()
{
	if(m_type == eLLong)
		return m_u.ql;
	else if(IsString())
	{
		switch(m_type)
		{
		case eWstr:
			if(KwIsDigit(m_u.wstr[0]) || m_u.wstr[0] == '-')
				return _wtoi64(m_u.wstr);
			break;
		case eUtf8:
		case eMbcs:
			if(KwIsDigit(m_u.str[0]) || m_u.str[0] == '-')
				return _atoi64(m_u.str);
			break;
		default:	ASSERT(0);
			throw - 10;
		}
	}
	else
	{
		try
		{
			return GetNum<INT64>();
		}
		catch(int)
		{
			ASSERT(0);
		}
	}
	return 0;
}

UINT CUbj::GetU()
{
	if(m_type == eUint)
		return m_u.ui;
	return (UINT)GetI();
}
// double CUbj::GetD(  )
// {
// 	try
// 	{
// 		return GetNum((double)0.);
// 	}
// 	catch (int e)
// 	{
// 		if(e == 0)
// 		{
// 			switch (m_type)
// 			{
// 			case eWstr  :	if(isdigit(m_u.wstr[0])) return _wtof(m_u.wstr);break;
// 			case eUtf8  :	if(isdigit(m_u.str[0])) return atof(m_u.str);break;
// 			default		:	ASSERT(0);
// 				throw -10;
// 			}
// 		}
// 		else
// 			throw e;
// 	}
// 	return 0;
// }

void CUbj::Set(const wchar_t* v, UType type)
{
	Reset();
	//	ASSERT(v);
	switch(type)
	{
	case eWstr:
	{
		Set(v);
	}break;
	case eInt:
	{
		Set(_wtoi(v));
	}break;
	case eLLong:
	{
		ASSERT(0);
	}break;
	case eShort:
	{
		if(v[0] != '\0')
			Set((short)_wtoi(v));
		else
			throw - 1;
	}break;
	case eUint:
	{
		Set((UINT)_wtoi(v));
	}break;
	case eUShort:
	{
		Set((unsigned short)_wtoi(v));
	}break;
	case eULLong:
	{
		ASSERT(0);
		throw - 10;
	}break;
	case eBool:
	{
		// 'Y' 또는 '0'만 아니면
		// || (v[0] != '\0' && v[0] != '0'&& v[1] == '\0')
		Set((v[0] == 'Y') ? 1 : 0);
	}break;
	case eChar:
	{
		if(tchlen(v) == 1 && v[0] < 127)
			Set((char)v[0]);
		else
			throw - 3;
	}break;
	case eByte:
	{
		if(tchlen(v) == 1 && v[0] < 127)
			Set((byte)v[0]);
		else
			throw - 4;
	}break;
	case eFloat:
	{
		try
		{
			Set((float)_wtof(v));
		}
		catch(...)
		{
			throw - 5;
		}
	}break;
	case eDouble:
	{
		try
		{
			Set((double)_wtof(v));
		}
		catch(...)
		{
			throw - 6;
		}
	}break;
	case eTime:
	{
		// 			int warr[8]= {1980,1,1,0,0,0,0,0}; // static 이면 이전 값이 그래로 있잖아.
		// 			swscanf(v, L"%04d-%02d%-02d %02d:%02d:%02d", warr, warr+1, warr+2, warr+3, warr+4, warr+5);		
		CTm t(v);
		//swscanf(v, L"%04d-%02d%-02d %02d:%02d:%02d", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec);		
		//t.tm_year -= 1900;
		//t.tm_mon -= 1;
		//CKwTime t0(&t);
		Set(&t);//t0);
	}break;
	default:
		throw - 20;
		break;
	}
}

void CUbj::Set(const char* v, UType type /*= eUtf8*/)
{
	Reset();
	size_t l = tchlen(v);
	m_u.str = new char[l + 1];
	tchcpy(m_u.str, v);
	m_type = type;
	ASSERT(m_type != eULLong);
}

void CUbj::Set(const wchar_t* v)
{
	Reset();
	if(v)
	{
		size_t l = tchlen(v);
		m_u.wstr = new wchar_t[l + 1];
		tchcpy(m_u.wstr, v);
	}
	else
		m_u.wstr = NULL;//  [5/30/2015 dwkang]
// >	SmoSvr.exe!CUbj::Set(const wchar_t * v)  줄 529	C++
// 	SmoSvr.exe!CUbj::Set(const wchar_t * v, UType type)  줄 432	C++
// 	SmoSvr.exe!CXmlMapObjBase::Set(const wchar_t * skey, const wchar_t * str, UType type)  줄 791	C++
// 	SmoSvr.exe!CXmlMapObj::Set(int key, const wchar_t * val, int type)  줄 1677	C++
// 	SmoSvr.exe!CValUnit::operator=(const wchar_t * val)  줄 1259 + 0x3b 바이트	C++
// 	SmoSvr.exe!CHttpSmo::Test_ReqPrice(CWnd * pw)  줄 582 + 0x4d 바이트	C++
/// NULL returns;
//	hs07(f0705TimeToCall) = rec.GetCellStr(1, _i);


	m_type = eWstr;
}

void CUbj::Set(void* p, UINT_PTR len)
{
	Reset();
	if(p)
	{
		m_u.bin.data = (void*)new char[len];
		m_u.bin.len = len;
		memcpy(m_u.bin.data, p, len);
	}
	m_type = eBin;
}

void CUbj::Set(C_bindata val, bool bCopy /*= true*/)
{
	Reset();
	if(bCopy)
		Set(val.data, val.len);
	else
	{
		m_u.bin.data = val.data;
		m_u.bin.len = val.len;
	}
}

//?주의 v는 IXmlSerializable* pXml = dynamic_cast<IXmlSerializable*>(ptrCreated);	
//		처럼 한 값을 받아 와야 한다.

/*
void CUbj::Set( TPtrObj v, BOOL bOwner / *= FALSE* / )
{
//	Reset();
	ASSERT(dynamic_cast<IXmlSerializable*>(v));
	Reset();
	m_u.obj.pbj = v;
	if(bOwner)
		m_u.obj.m_option |= eEnc_Owner;
	else
		m_u.obj.m_option &= ~eEnc_Owner;
	m_type = eObj;
	//?주의 xml parsing중에 만들어지는 객체는 반드시 _CreateMe 가 있어야 한다.
	// 		m_u.obj = obj->_CreateMe(0);
	// 		m_u.obj->_Clone(obj);
}

void CUbj::Set(TPtrObj v, int iOp)
{
	//	Reset();
	ASSERT(dynamic_cast<IXmlSerializable*>(v));
	Reset();
	m_u.obj.pbj = v;
	m_u.obj.m_option = iOp;
	m_type = eObj;
	//?주의 xml parsing중에 만들어지는 객체는 반드시 _CreateMe 가 있어야 한다.
	// 		m_u.obj = obj->_CreateMe(0);
	// 		m_u.obj->_Clone(obj);
}
*/
void CUbj::CopyBin(C_bindata* pbinS, C_bindata* pbin)
{
	DeleteMeSafe(pbin->data);

	pbin->data = (void*)new char[pbinS->len];
	pbin->len = pbinS->len;
	memcpy(pbin->data, pbinS->data, pbinS->len);
}

C_bindata* CUbj::GetBin(C_bindata* pbin)
{
	ASSERT(m_type == eBin);
	ASSERT(pbin);
	CopyBin(&m_u.bin, pbin);
	return &m_u.bin;
}

// maxDiff 0.1 이면 0.? 까지 같으면 TRUE
BOOL CUbj::IsDoubleClose(CUbj* pbj, double maxDiff)
{
	double df = GetD() - pbj->GetD();
	return (abs(df) < maxDiff); //미세한 차이는 같은 걸로
}
BOOL CUbj::IsSameValue(CUbj* pbj)
{
	return IsSame(pbj, 1);
}
// 1이면 pbj==NULL 인경우 값이 0, false 와 같다고 인식 한다.
BOOL CUbj::IsSame(CUbj* pbj, int iOp)
{
	if(pbj == NULL)
	{
		if(iOp == eSmNullDff)
			return FALSE;
		ASSERT(iOp == eSmNullAs0);
		switch(m_type)
		{
		case eFloat:
		case eDouble:
			return m_u.d == 0.;
		case eInt:
			return m_u.i == 0;
		case eShort:
			return m_u.si == 0;
		case eLLong:
			return m_u.ql == 0;
		case eULLong:
			return m_u.uql == 0;
		case eUint:
			return m_u.ui == 0;
		case eUShort:
			return m_u.usi == 0;
		case eChar:
			return m_u.c == 0;
		case eByte:
			return m_u.uc == 0;
		case eBool:
			return m_u.b == false;
		case eTime:
			return m_u.t == 0;
		//case eObj:			return m_u.obj.pbj == 0;
		case eBin:
			return m_u.bin.len == 0;
		case eWstr:
			return (m_u.wstr[0] == '\0');
		case eUtf8:
		case eMbcs:
			return (m_u.str[0] == '\0');
		default:
			return FALSE;
		}
	}
	else if(pbj->m_type == m_type)
	{
		if(KwOp(eSmFloatClose))
		{
			try
			{
				//double df = GetD() - pbj->GetD();
				//(abs(df) < 0.0000001); //미세한 차이는 같은 걸로
				return IsDoubleClose(pbj);
			}
			catch(int)
			{
				return FALSE;// 문자를 숫자로 변경 하다 오류나면
			}
		}
		switch(m_type)
		{
		case eFloat:
		case eDouble:
			return m_u.d == pbj->m_u.d;
		case eInt:case eShort:case eLLong:
		case eULLong:case eUint:case eUShort:
		case eChar:case eByte:
		case eBool:case eTime:
			return memcmp(&m_u, &pbj->m_u, sizeof(UVal)) == 0;
		case eWstr:
			if(m_u.wstr && pbj->m_u.wstr)
				return tchcmp(m_u.wstr, pbj->m_u.wstr) == 0;
			return false;
		case eUtf8:
		case eMbcs:
			if(m_u.str && pbj->m_u.str)
				return tchcmp(m_u.str, pbj->m_u.str) == 0;
			return false;
		default:
			ASSERT(0);
			return FALSE;
		}
	}
	else
	{
		if(KwOp(4))
		{
			try
			{
				return IsDoubleClose(pbj);
			}
			catch(int)
			{
				return FALSE;// 문자를 숫자로 변경 하다 오류나면
			}
		}
		switch(m_type)
		{
		case eLLong:case eULLong:
			//	return GetI64() == pbj->GetI64();
		case eInt:case eShort:
		case eChar:case eByte:
			//	return GetI() == pbj->GetI();
		case eUint:case eUShort:
			//	return GetU() == pbj->GetU();
		case eBool:
		case eTime:
		{
			if(pbj->IsDigit())
				return GetI64() == pbj->GetI64();
			else
				return GetD() == pbj->GetD();
		}
		break;
		case eFloat:case eDouble:
			try
			{
				return GetD() == pbj->GetD();
			}
			catch(int)
			{
				return FALSE;
			}

		case eWstr:
		case eWstrL:
		case eUtf8:
		case eMbcs:
			if(pbj->IsString()) // 둘다 인코딩 다른 문자면 UNICODE로 바꿔서 비교 하면 끝
			{
				CStringW s1, s2;
				GetTxt(s1);
				pbj->GetTxt(s2);
				return s1 == s2;
			}
			else // b가 숫자면.. 숫자로 비교 하면 끝
			{
				try
				{
					return GetD() == pbj->GetD();
				}
				catch(int)
				{
					return FALSE;// 문자를 숫자로 변경 하다 오류나면
				}
			}
		default:
			ASSERT(0);
			return FALSE;
		}

	}
	//return pbj && m_type == pbj->m_type && memcmp(&m_u, &pbj->m_u, sizeof(UVal));
}

size_t CUbj::GetLength()
{
	switch(m_type)
	{
	case eWstr:
	case eWstrL:
		return tchlen(m_u.wstr);
	case eUtf8:
	case eMbcs:
		return strlen(m_u.str);
	default:
		ASSERT(0);
		break;
	}
	return 0;
}

TimeT CUbj::GetT()
{
	switch(m_type)
	{
	case eWstr:
		if(tchlen(m_u.wstr) == 0)
			return 0;
		return KwParseTimeStr4(m_u.wstr);
	case eTime:
		return m_u.t;
	case eLLong: // int64
		return (TimeT)m_u.ql;
	case eInt:
		return (TimeT)m_u.i;
	default:
		break;
	}
	ASSERT(0);
	return m_u.t;
}
/*
ISerializable* CUbj::_CreateMe(int iOp, PAS pfr )
{
	return static_cast<ISerializable*>(new CUbj());
}
*/
CUbj::~CUbj(void)
{
	InterlockedDecrement(&s_numU);
	Reset();
}

CUbj::CUbj(void) : m_type(eUndefined)
{
#ifdef _DEBUGx
	s_i++;
	//	KTrace(L"\t\t\t CUbj(%d) (%d, %d)\n", m_type, s_i/(1106*6), s_i%(1106*6));
#endif // _DEBUG
	InterlockedIncrement(&s_numU);
	KwZeroMemory(m_u);
}

