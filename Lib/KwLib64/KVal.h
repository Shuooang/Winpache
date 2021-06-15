#pragma once
#include "inlinefnc.h"
#include "tchtool.h"
#include "SrvException.h"
#include "StrBuffer.h"
//#include "KBinary.h"



enum UType
{
	eUndefined = -1,
	eInt, eWstr, eTime, eDouble, eMoney, eBin, eWstrL, eBool, //�� ���� enum�� ȣȯ�� ���

	eLLong = 10, eShort, eUint, eULLong, eUShort, eChar, eByte, eFloat,
	eUtf8 = 20, eMbcs, eObj,
	eMapSS = 30, eMapSI, eMapSD, eMapIS, eMapII, eMapID,
	eMapSO = 40, eMapIO, eMapDO
};
//typedef IXmlMap* TPtrObj;
struct C_bindata
{
	void* data;
	UINT_PTR len;// max 4giga
};
//struct C_obj
//{
//	TPtrObj pbj;
//	int m_option;
//};
/*#define ERROR_RETURN() {_Format(L"UType(%d)", m_type);\
	if(s_pErrorHandler)\
	s_pErrorHandler->_OnError(eErField, L"Ubj", __FUNCTIONW__, __LINE__, L" ", _s, 0);}

#define ERROR_RETURNS(s) {_Format(L"UType(%d::%s)", m_type, (s));\
	if(s_pErrorHandler)\
	s_pErrorHandler->_OnError(eErField, L"Ubj", __FUNCTIONW__, __LINE__, L" ", _s, 0);}
*/



class CUbj
//	: public CSerialClonable
	//	, public IChangableVal
{
public:
//	static IHaveUnknownPtrBase* s_pCreateObj;//?ubjObj
public:
	CStrBufferT<CString, LPCTSTR> _buf;
	CStrBufferT<CStringA, LPCSTR> _bufa;
	//enum { eINTEGER, eTEXT, eDATE, eFLOAT, eMONEY, eBINARY, eLTEXT, eBOOL};
	UType m_type;

	union UVal
	{
		int i;
		__int64 ql;
		short si;

		unsigned int       ui;
		unsigned long long uql;//__int64 �� ��� �ϹǷ� ������ ����.
		unsigned short     usi; // WORD

		bool b;

		char c;
		unsigned char uc; // byte

//		float f;
		double d;

		__time64_t t;//__time64_t

		char* str;
		wchar_t* wstr;

		C_bindata bin;
		//C_obj obj;

		BYTE mem[8]; // debug watch ����
	} m_u;
	static LONG s_numU;

	CUbj(void);
	virtual ~CUbj(void);

	void Reset();

	// 1�̸� pbj==NULL �ΰ�� ���� 0, false �� ���ٰ� �ν� �Ѵ�.
	// 0�̸� pbj==NULL �̸� return FALSE
	enum
	{
		eSmNullDff = 0, //������ NULL�̸� �޶�
		eSmNullAs0 = 1, //�Ǽ��϶� NULL�̸� 0���� ��
		eSmNullAsStr = 2, //���ڿ��϶� NULL�̸� "" ���� ����
		eSmFloatClose = 4,//�Ǽ� �϶� 
	};
	BOOL IsSame(CUbj* pbj, int iOp = 0);
	/*
	//	virtual LPCSTR _GetClassName() { return "CUbj";}
	virtual void _SerializeVer(CArchive& arc, int iOp = eSrlDefault);

	virtual void _Clone(IClonable* pSrc0, int iOp = eCloneDefault);

	virtual ISerializable* _CreateMe(int iOp = 0, PAS pfr = NULL);
	*/
	void Clone(CUbj* pSrc0, int iOp = 0);
	void Set(int                v) { Reset(); m_u.i = v; m_type = eInt; }
	void Set(__int64            v) { Reset(); m_u.ql = v; m_type = eLLong; }
	void Set(short              v) { Reset(); m_u.si = v; m_type = eShort; }
	void Set(unsigned int       v) { Reset(); m_u.ui = v; m_type = eUint; }
	void Set(unsigned __int64   v) { Reset(); m_u.uql = v; m_type = eULLong; }
	void Set(unsigned short     v) { Reset(); m_u.usi = v; m_type = eUShort; }
	void Set(bool               v) { Reset(); m_u.b = v; m_type = eBool; }
	void Set(char               v) { Reset(); m_u.c = v; m_type = eChar; }
	void Set(unsigned char      v) { Reset(); m_u.uc = v; m_type = eByte; }
	void Set(float              v) { Reset(); m_u.d = v; m_type = eFloat; }
	void Set(double             v) { Reset(); m_u.d = v; m_type = eDouble; }
	void Set(const char* v, UType type = eUtf8);
	void Set(const wchar_t* v, UType type);
	void Set(const wchar_t* v);

	//void Set(CKwTime            v) { Reset(); m_u.t = v.m_ltime; m_type = eTime; }
	void Set(CTime            v) { Reset(); m_u.t = v.GetTime(); m_type = eTime; }
	void SetT(__time64_t v) { Reset(); m_u.t = v; m_type = eTime; }
	/*void Set(CTm* v)
	{
		Reset();
		CKwTime t0(v);
		Set(t0);
	}*/


//	void Set(TPtrObj     v, int iOp = eEnc_Owner);

	void Set(C_bindata val, bool bCopy = true);
	void Set(void* p, UINT_PTR len);

	LPCWSTR GetP();
	LPCWSTR GetTxt(CStringW& sval);
	LPCSTR GetTxtA(CStringA& sval);
	LPCWSTR GetTxtW();
	LPCSTR GetTxtA();
	std::string GetString();

	//TPtrObj GetPbj(int* pOption = NULL)
	//{
	//	ASSERT(m_type == eObj);
	//	if(m_type == eObj)
	//	{
	//		if(pOption)
	//			*pOption = m_u.obj.m_option;
	//		return m_u.obj.pbj;
	//	}
	//	return NULL;
	//}

	bool IsString()
	{
		switch(m_type)
		{
		case eWstr:
		case eWstrL:
		case eUtf8:
		case eMbcs:
			return true;
		}
		return false;
	}
	bool IsPremitive()
	{
		switch(m_type)
		{
		case eInt:
		case eShort:
		case eUint:
		case eUShort:
		case eBool:
		case eChar:
		case eByte:
		case eFloat:
		case eDouble:
		case eMoney:
		case eTime:
			return true;
		}
		return false;
	}
	bool IsDigit()
	{
		switch(m_type)
		{
		case eInt:
		case eShort:
		case eUint:
		case eUShort:
		case eBool:
		case eChar:
		case eByte:
			// 		case eFloat :	
			// 		case eDouble:	
			//		case eMoney:	
		case eTime:
			return true;
		}
		return false;
	}

	template<typename TNUM>
	TNUM GetNum()// throw(int)
	{
		switch(m_type)
		{
		case eInt:		return (TNUM)m_u.i;
		case eShort:	return (TNUM)m_u.si;
		case eUint:		return (TNUM)m_u.ui;
		case eUShort:	return (TNUM)m_u.usi;
		case eBool:		return (TNUM)(m_u.b ? 1 : 0);
		case eChar:		return (TNUM)m_u.c;
		case eByte:		return (TNUM)m_u.uc;
		case eFloat:
		case eMoney:
		case eDouble:	return (TNUM)m_u.d;
		case eTime:		return (TNUM)m_u.t;
		case eLLong:	return (TNUM)m_u.ql;
		case eULLong:	return (TNUM)m_u.uql;


		default:
		{
			throw_str(L"Not a numver type");
		}	break;
		}
		return 0;
	}
	int GetNum();
	int GetI();
	__int64 GetI64();

	template<typename TFLOAT>
	TFLOAT GetDF()
	{
		if(m_type == eDouble || m_type == eFloat || m_type == eMoney)
		{
			try
			{
				return GetNum<TFLOAT>();
			}
			catch(int)	{
				return (TFLOAT)0;
			}
		}
		else if(IsDigit())
		{
			return (TFLOAT)GetNum<INT64>();
		}
		else if(IsString())
		{
			switch(m_type)
			{
			case eWstr:
				try
				{
					if(KwIsDigit(m_u.wstr[0]) || m_u.wstr[0] == '-')
					{
						WCHAR bf[50] = { '\0', };
						if(tchcpynum(bf, m_u.wstr, eCpnNormal))
							return (TFLOAT)_wtof(bf);
						else
							throw - 2;
					}
				}
				catch(...)	{
					throw_str(m_u.wstr);
				} // ���ڸ� ���ڷ� ���� �ϴ� ��������
				break;
			case eUtf8:
			case eMbcs:
				try
				{
					if(KwIsDigit(m_u.str[0]) || m_u.wstr[0] == '-')
					{
						CHAR bf[50] = { '\0', };
						if(tchcpynum(bf, m_u.str, eCpnNormal))
							return (TFLOAT)atof(bf);
						else
							throw - 2;
					}
				}
				catch(...)	{
					throw_str(CStringW(m_u.str));
				}
				break;
			default:
				throw_str(L"Invalid type");
				break;
			}
		}
		else
		{
			throw_str(L"Unsupported type");
		}
		return (TFLOAT)0;
	}

	double GetD() { return GetDF<double>(); }
	float GetF() { return GetDF<float>(); }
	UINT GetU();
	__time64_t GetT();

	C_bindata* GetBin(C_bindata* pbin);
	C_bindata GetBin() { ASSERT(m_type == eBin); return m_u.bin; }

	void CopyBin(C_bindata* pbinS, C_bindata* pbin);


	size_t GetLength();
	BOOL IsSameValue(CUbj* pbj);
	BOOL IsDoubleClose(CUbj* pbj, double maxDiff = 0.0000001);
};

class Integer
{
public:
	int v;
	//operator int(
};


/// <summary>
/// Ư�� ��ü�� �ִ� operator= �� ���Կ� Ÿ������ ����, NULL�� �ְ� ������ null�� ���������� 
/// type check ambiguous �� �߻� �ϹǷ�
/// </summary>
class ONULL
{
public:
	char* v{ nullptr };
};
