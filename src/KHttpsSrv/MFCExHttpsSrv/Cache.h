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

	bool GetCacheValue(stringv key, string& value);

	void PutCacheValue(stringv key, stringv value);

	bool DeleteCacheValue(stringv key, string& value);

private:
	std::mutex _cache_lock;
	//ĳ�� �����
	std::map<string, string, std::less<>> _cache;
	//ĳ�� ź���ð� ���
	std::map<string, ULONGLONG, std::less<>> _fresh;
	// �ִ� ĳ�� Ÿ��. �̽ð� ������ ĳ�õ� ���� ����. ������ �ٽ� DB����.
public:
	int _maxFresh{ 2000 };//default ��: �ִ� ����. 2�� �������� ĳ�� �ȵǰ�
};

class CacheVal
{
public:
	CacheVal()
	{
	}
	CacheVal(CacheVal & cval)
		: _contentType(cval._contentType)
	{
		if(cval._data.m_len > 0)
			_data.Attach(cval._data);
	}
	CacheVal(stringv value, stringv contentType)//INT_PTR len, 
		: _contentType(contentType)
	{
		if(value.length() > 0)
			_data.SetPtr(value.data(), value.length());
	}
	void Clone(CacheVal& cval)
	{
		_contentType = cval._contentType;
		if(cval._data.m_len > 0)
			_data.Attach(cval._data);
	}
	//void operator=(const CacheVal& cval)    {    } �ȿ� KBinary�� �뤊�� ���� ����, �����͸� �Ű� ���� ������.
	string _contentType;
	KBinary _data;
};

class CacheBin : public CppCommon::Singleton<CacheBin>
{
	friend CppCommon::Singleton<CacheBin>;

public:
	~CacheBin()
	{
		for(auto& [k, v] : _cacheH)
			delete v;
	}
	//    string GetAllCache();
	bool GetCacheValue(stringv key, CacheVal& value);
	void PutCacheValue(stringv key, stringv value, stringv contentType);
	bool DeleteCacheValue(stringv key, CacheVal& value);

private:
	std::mutex _cache_lock;
	//ĳ�� �����
	//std::map<string, CacheVal*> _cache;
	std::map<size_t, CacheVal*> _cacheH;
	//ĳ�� ź���ð� ���
	//std::map<string, ULONGLONG> _fresh;
	std::map<size_t, ULONGLONG> _fresh;
	// �ִ� ĳ�� Ÿ��. �̽ð� ������ ĳ�õ� ���� ����. ������ �ٽ� DB����.
public:
	int _maxFresh{ 2000 };//default ��: �ִ� ����. 2�� �������� ĳ�� �ȵǰ�
};

