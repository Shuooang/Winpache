#pragma once


// _Clone  함수 에서 조건별 clone할때 쓰도록 옵션을 정해 두었다.
enum ECloneOp
{
	eCloneDefault	= 1 << 0,
	eCloneAppend	= 1 << 1, /// 항목을 추가 or 덮어쓴다.  array나 map인경우
	eCloneLogin		= 1 << 2,
	eCloneNoRecData	= 1 << 3, // KCol 이 데이터를 복사 하지 않고, 껍질만 복사
	eCloneUndeRedo	= 1 << 4,
	eCloneShellOnly	= 1 << 5, // CMapKeyPbj 몸체는 복사 하지 않고 주변 변수만 복사
	eCloneReference	= 1 << 6,
	eCloneExceptBlob= 1 << 7,
	eCloneDataOnly  = 1 << 8, //CDbConnect 에서 m_pDB 는 복사 하지 않는다.
	eCloneChildOnly	= 1 << 9,
	eCloneBodyOnly	= 1 << 10, 	//eReserved07 // hash인경우 hash만 복사
	eCloneExceptNull= 1 << 11, // data 없는 키는 복사 제외
	eReserved09		= 1 << 12,
	eReserved10		= 1 << 13,
	eReserved11		= 1 << 14,
	eReserved12		= 1 << 15,
	eCloneUser		= 1 << 16, // 더 이상 만들면 안되고 16부터는 모듈별 특성상 처리
};

enum ESameOp
{
	eSameDefault = 1 << 0,
	eSameExceptBinary = 1 << 1,
};

class IClonable
{
public:
	virtual ~IClonable() {}
	virtual void _Clone(IClonable* pSrc0, int iOp = eCloneDefault)
	{
	}
	// 	{
	// 		__Super_Clone(CKoSeek);
	// 		Clone_Val(m_serial);// 1 
	// 		Clone_Val(m_offset);// 2 
	// 	}
	virtual bool _IsSame(IClonable* pSrc, int iOp = eSameDefault)
	{
		ASSERT(0);
		return false;
	}
};

