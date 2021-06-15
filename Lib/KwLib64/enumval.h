#pragma once

/// //////////////////////////////////////
/// level 1.1
/// ����� � �Լ��� �Ⱦ��� ��� �ִ´�.

enum ECmpr
{
	eCmprNone       = 0,	// ���ڿ��� �ƴ� ���
	// eCmprNone�� incase �ΰ��� ���� �ȵȴ�.
	// eCmprInCase �� ���� �ݵ�� eCmprInCase�� ���� �ؾ߸� �ϵ��� �Ѵ�.
	//?warning �ߺ� ������ ���Ŀ��� ��� �Ǿ�� �Ѵ�.
	eCmprInCase     = eCmprNone,//1 << 3, // �ݵ�� 1�̾�� �Ѵ�.  == TRUE : bCaseSensitive

	eCmprCase       = 1 << 0, // deprecated �ݵ�� 1�̾�� �Ѵ�.  == TRUE : bCaseSensitive
	eCmprChoseong   = 1 << 1,
	eCmprDescending = 1 << 2,

	eCmprMultiMap   = 1 << 4,


	eCmprEndInclusive = 1 <<8, // ���� �˻��� end �� exclusive �� �⺻�ε� inclusive �� ��� �̰� �ش�.
	eCmprStartExclusive = 1 <<9, // ���� �˻��� start �� inclusive �� �⺻�ε� exclusive �� ��� �̰� �ش�.
	eCmprDateOnly = 1 <<10, // �ð� type �� ��� ��¥�� 

	eCmprExact     = 0, //1 << 12, default 
	eCmprBegin     = 1 << 13,
	eCmprEnd       = 1 << 14,
	eCmprEBMask = eCmprExact|eCmprBegin|eCmprEnd,

	eCmprDoNotSort = 1 << 16, // �̹� ���� �Ǿ� �־ refresh�� �ϰ� 

	// 1 << 16 ~ 20 ������ eType �� ��� �ǹǷ� ��� ����
	// Ư�� 20�� ��Ʈ�� eType ������ �ֳ� ������ ���� �Ѵ�.
	//eCmpTableField = 1 << 8, // len �� talble.field���� table�� ���̷� ���
};
// ECmpr �� ���Ƿ� ���յ� �ɼǰ�

typedef int ECmpOp;
