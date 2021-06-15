#include "pch.h"
#include "KRect.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif






#ifdef _DEBUGx
try
{
	rv = https->start_server();// 캐시폴더 지정시 안에 파일이 Dropbox smart동기화가 안되어 있으면 crush
	if(rv != 0)
	{
		AfxMessageBox(L"Server Start Error! Make sure this computer is HTTPS certified.");
		return;
	}
	https->_server->getCache()._maxFresh = doc->_CacheLife;//순서 _server ?
}
catch(std::exception& e)
{
	auto buf = new TCHAR(1024);
	KAtEnd d_buf([&]() { delete buf; });
}
catch(CException* e)
{
	auto buf = new TCHAR(1024);
	KAtEnd d_buf([&]() { delete buf; });
	e->GetErrorMessage(buf, 1000);
	KwMessageBox(buf);
}
catch(...)
{
	auto buf = new TCHAR(1024);
	KAtEnd d_buf([&]() { delete buf; });
}









#endif
