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
    //ĳ�� �����
    std::map<string, string, std::less<>> _cache;
    //ĳ�� ź���ð� ���
    std::map<string, ULONGLONG, std::less<>> _fresh;
    // �ִ� ĳ�� Ÿ��. �̽ð� ������ ĳ�õ� ���� ����. ������ �ٽ� DB����.
public:
    int _maxFresh {7000};
};


