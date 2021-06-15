#pragma once

/// //////////////////////////////////////
/// level 1.1
/// 여기는 어떤 함수도 안쓰는 경우 넣는다.

enum ECmpr
{
	eCmprNone       = 0,	// 문자열이 아닌 경우
	// eCmprNone가 incase 인것은 말이 안된다.
	// eCmprInCase 인 경우는 반드시 eCmprInCase를 지정 해야만 하도록 한다.
	//?warning 중복 가능한 정렬에만 사용 되어야 한다.
	eCmprInCase     = eCmprNone,//1 << 3, // 반드시 1이어야 한다.  == TRUE : bCaseSensitive

	eCmprCase       = 1 << 0, // deprecated 반드시 1이어야 한다.  == TRUE : bCaseSensitive
	eCmprChoseong   = 1 << 1,
	eCmprDescending = 1 << 2,

	eCmprMultiMap   = 1 << 4,


	eCmprEndInclusive = 1 <<8, // 범위 검색시 end 는 exclusive 가 기본인데 inclusive 할 경우 이걸 준다.
	eCmprStartExclusive = 1 <<9, // 범위 검색시 start 는 inclusive 가 기본인데 exclusive 할 경우 이걸 준다.
	eCmprDateOnly = 1 <<10, // 시간 type 일 경우 날짜만 

	eCmprExact     = 0, //1 << 12, default 
	eCmprBegin     = 1 << 13,
	eCmprEnd       = 1 << 14,
	eCmprEBMask = eCmprExact|eCmprBegin|eCmprEnd,

	eCmprDoNotSort = 1 << 16, // 이미 정렬 되어 있어서 refresh만 하고 

	// 1 << 16 ~ 20 까지는 eType 이 사용 되므로 사용 금지
	// 특히 20번 비트는 eType 정보가 있나 없나를 감안 한다.
	//eCmpTableField = 1 << 8, // len 을 talble.field에서 table의 길이로 사용
};
// ECmpr 의 값의로 조합된 옵션값

typedef int ECmpOp;
