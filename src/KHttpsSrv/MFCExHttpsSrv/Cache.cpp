//#include "../../MFCAppServerEx2/pch.h"
// pch.h �� precompiled header �̹Ƿ� �տ� ��ΰ� ��� �������� �ȴ�.
#include "pch.h"    //C:\Dropbox\Proj\STUDY\boostEx\CppServer\MFCAppServerEx2\pch.h
#include "Cache.h"  //C:\Dropbox\Proj\STUDY\boostEx\CppServer\CppServer-master\examples\Cache.cpp

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


string Cache::GetAllCache()
{
	std::scoped_lock locker(_cache_lock);
	string result;
	result += "[\n";
	for(const auto& item : _cache)
	{
		result += "  {\n";
		result += "    \"key\": \"" + item.first + "\",\n";
		result += "    \"value\": \"" + item.second + "\",\n";
		result += "  },\n";
	}
	result += "]\n";
	return result;
}

#include <unordered_map>

bool Cache::GetCacheValue(stringv key, string& value)
{
#ifdef _DEBUG
	std::hash<string> hsr;
	auto hsv = hsr(key.data());
#endif // _DEBUG

	std::scoped_lock locker(_cache_lock);
	auto it = _cache.find(key);
	if(it != _cache.end())
	{
		auto itk2 = _fresh.find(key);//ĳ�ø� ã������ �ʹ� ���� �ȰŸ� û�Ҹ� �Ѵ�.
		if(itk2 != _fresh.end())//�̰� ���� ����
		{
			auto elp = GetTickCount64() - itk2->second;
			if(elp > _maxFresh)
			{
				_cache.erase(string(key));//old cache erase
				_fresh.erase(string(key));
				return false;
			}
		}
		value = it->second;
		return true;
	}
	else
		return false;
}

void Cache::PutCacheValue(stringv key, stringv value)
{
	std::scoped_lock locker(_cache_lock);
	auto k = key.data();
	_cache[k] = value;
// 	auto it = _cache.emplace(key, value);// it.first�� pair, second�� �߰� ���� boolean��
// 	if(!it.second)
// 		it.first->second = value;
	_fresh[k] = GetTickCount64();
	//auto itf = _fresh.emplace(key, GetTickCount64());
	//if(!itf.second)
	//	itf.first->second = GetTickCount64();
}

bool Cache::DeleteCacheValue(stringv key, string& value)
{
	std::scoped_lock locker(_cache_lock);
	auto it = _cache.find(key);
	if(it != _cache.end())
	{
		value = it->second;
		_cache.erase(it);

		auto itf = _fresh.find(key);
		_fresh.erase(itf);
		return true;
	}
	else
		return false;
}



bool CacheBin::GetCacheValue(stringv key, CacheVal& value)
{
	auto pkey = key.data();
	std::scoped_lock locker(_cache_lock);
	
	std::hash<string> hsr;
	auto hsv = hsr(pkey);

	CacheVal* cval = nullptr;
	auto it = _cacheH.find(hsv);// Lookup(key, cval);
	if (it != _cacheH.end())
	{
		cval = it->second;
		assert(cval);
		auto itk2 = _fresh.find(hsv);//ĳ�ø� ã������ �ʹ� ���� �ȰŸ� û�Ҹ� �Ѵ�.
		if (itk2 != _fresh.end())//�̰� ���� ����
		{
			auto elp = GetTickCount64() - itk2->second;
			if (elp > _maxFresh)
			{
				cval->_data.Detach();
				value._data.Attach(cval->_data);//�����Ǿ��� ������ �Ѵ�.
				value._contentType = cval->_contentType;

				auto val = it->second;
				if(val) delete val;
				_cacheH.erase(hsv);
				//_cache.DeleteKey(key);//old cache erase
				_fresh.erase(hsv);
				return false;
			}
		}
		value._contentType = cval->_contentType;
		value._data.Wrap(cval->_data);//�����Ǿ��� ������ �Ѵ�.
		return true;
	}
	else
		return false;
}

void CacheBin::PutCacheValue(stringv key, stringv value, stringv contentType)
{
	std::scoped_lock locker(_cache_lock);
	// _cache�� value�� pointer of CachVal ����.
	auto pkey = key.data();
	std::hash<string> hsr;
	auto hsv = hsr(pkey);
	auto it = _cacheH.find(hsv);
	if(it != _cacheH.end())
	{
		delete it->second; // free�����
		it->second = nullptr;
	}
	//_cache.erase((key))
	CacheVal* cval = new CacheVal(value, contentType);
	//auto it = _cache.SetAt(key, cval);
	_cacheH[hsv] = cval;
	//auto itf = _fresh.emplace(key, GetTickCount64());//emplace�� ������ �ȵȴ�.
	_fresh[hsv] = GetTickCount64();
}

// ���� �ϸ鼭 ������ ��� ����.
bool CacheBin::DeleteCacheValue(stringv key, CacheVal& box)
{
	std::scoped_lock locker(_cache_lock);
	auto pkey = key.data();
	std::hash<string> hsr;
	auto hsv = hsr(pkey);

	auto it = _cacheH.find(hsv);// Lookup(key, cval);
	if(it != _cacheH.end())
	{
		CacheVal* cval = it->second;
		if(cval)
		{
			box.Clone(*cval);
			delete cval;
		}
		_cacheH.erase(hsv);

		auto itf = _fresh.find(hsv);
		if(itf != _fresh.end())
			_fresh.erase(itf);//itterator�ε� ���� ����. _fresh.erase(key);
		return true;
	}
	else
		return false;
}
