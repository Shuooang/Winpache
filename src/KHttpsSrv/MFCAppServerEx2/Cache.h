#pragma once


/// KHttp\pkg\CppServer\include\server
#include "server/kwadjust.h"

/// KHttp\pkg\CppCommon\include\string
// #include "string/string_utils.h"
#include "utility/singleton.h" // CppCommon::Singleton<Cache>

using std::string;


class Cache : public CppCommon::Singleton<Cache>
{
	friend CppCommon::Singleton<Cache>;

public:
	string GetAllCache();

	bool GetCacheValue(std::string_view key, string& value);

	void PutCacheValue(std::string_view key, std::string_view value);

	bool DeleteCacheValue(std::string_view key, string& value);

private:
	std::mutex _cache_lock;
	//캐시 저장소
	std::map<string, string, std::less<>> _cache;
	//캐시 탄생시각 기록
	std::map<string, ULONGLONG, std::less<>> _fresh;
	// 최대 캐시 타임. 이시간 동안은 캐시된 것을 리턴. 넘으면 다시 DB실행.
public:
	int _maxFresh{ 2000 };//default 값: 최대 수명. 2초 지났으면 캐시 안되게
};

class CacheVal
{
public:
	CacheVal(CacheVal& cval)
		: _contentType(cval._contentType)
	{
		if(cval._data.m_len > 0)
			_data.Attach(cval._data);
	}
	CacheVal(PAS value = nullptr, INT_PTR len = 0, PAS contentType = nullptr)
		: _contentType(contentType)
	{
		if(len > 0)
			_data.SetPtr(value, len);
	}
	void Clone(CacheVal& cval)
	{
		_contentType = cval._contentType;
		if(cval._data.m_len > 0)
			_data.Attach(cval._data);
	}
	//void operator=(const CacheVal& cval)    {    } 안에 KBinary를 통쨰로 복사 할지, 포인터만 옮겨 갈지 몰느다.
	CStringA _contentType;
	KBinary _data;
};

class CacheBin : public CppCommon::Singleton<CacheBin>
{
	friend CppCommon::Singleton<CacheBin>;

public:
	~CacheBin()
	{
		for(auto& [k, v] : _cache)
			delete v;
	}
	//    string GetAllCache();
	bool GetCacheValue(PAS key, CacheVal& value);
	void PutCacheValue(PAS key, PAS value, INT_PTR len, PAS contentType);
	bool DeleteCacheValue(PAS key, CacheVal& value);

private:
	std::mutex _cache_lock;
	//캐시 저장소
	std::map<string, CacheVal*> _cache;
	//캐시 탄생시각 기록
	std::map<string, ULONGLONG> _fresh;
	// 최대 캐시 타임. 이시간 동안은 캐시된 것을 리턴. 넘으면 다시 DB실행.
public:
	int _maxFresh{ 2000 };//default 값: 최대 수명. 2초 지났으면 캐시 안되게
};

