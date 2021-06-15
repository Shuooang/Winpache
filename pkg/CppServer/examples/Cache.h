#pragma once


#include "server/kwadjust.h"

// CppCommon
#include "string/string_utils.h"
#include "utility/singleton.h"

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
    int _maxFresh {7000};
};


