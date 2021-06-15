//#include "../../MFCAppServerEx2/pch.h"
// pch.h �� precompiled header �̹Ƿ� �տ� ��ΰ� ��� �������� �ȴ�.
#include "pch.h"    //C:\Dropbox\Proj\STUDY\boostEx\CppServer\MFCAppServerEx2\pch.h
#include "Cache.h"  //C:\Dropbox\Proj\STUDY\boostEx\CppServer\CppServer-master\examples\Cache.cpp



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

bool Cache::GetCacheValue(std::string_view key, string& value)
{
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

void Cache::PutCacheValue(std::string_view key, std::string_view value)
{
	std::scoped_lock locker(_cache_lock);
	auto it = _cache.emplace(key, value);
	if(!it.second)
		it.first->second = value;
	auto itf = _fresh.emplace(key, GetTickCount64());
	//if(!itf.second)
	//	itf.first->second = GetTickCount64();
}

bool Cache::DeleteCacheValue(std::string_view key, string& value)
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
