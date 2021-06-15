#pragma once


// _Clone  �Լ� ���� ���Ǻ� clone�Ҷ� ������ �ɼ��� ���� �ξ���.
enum ECloneOp
{
	eCloneDefault	= 1 << 0,
	eCloneAppend	= 1 << 1, /// �׸��� �߰� or �����.  array�� map�ΰ��
	eCloneLogin		= 1 << 2,
	eCloneNoRecData	= 1 << 3, // KCol �� �����͸� ���� ���� �ʰ�, ������ ����
	eCloneUndeRedo	= 1 << 4,
	eCloneShellOnly	= 1 << 5, // CMapKeyPbj ��ü�� ���� ���� �ʰ� �ֺ� ������ ����
	eCloneReference	= 1 << 6,
	eCloneExceptBlob= 1 << 7,
	eCloneDataOnly  = 1 << 8, //CDbConnect ���� m_pDB �� ���� ���� �ʴ´�.
	eCloneChildOnly	= 1 << 9,
	eCloneBodyOnly	= 1 << 10, 	//eReserved07 // hash�ΰ�� hash�� ����
	eCloneExceptNull= 1 << 11, // data ���� Ű�� ���� ����
	eReserved09		= 1 << 12,
	eReserved10		= 1 << 13,
	eReserved11		= 1 << 14,
	eReserved12		= 1 << 15,
	eCloneUser		= 1 << 16, // �� �̻� ����� �ȵǰ� 16���ʹ� ��⺰ Ư���� ó��
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

