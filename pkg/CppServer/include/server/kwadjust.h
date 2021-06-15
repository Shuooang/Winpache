#pragma once

#ifndef KWADJUST_H
#define KWADJUST_H



//
// _WIN32_WINNT version constants
//
#define _WIN32_WINNT_NT4                    0x0400 // Windows NT 4.0
#define _WIN32_WINNT_WIN2K                  0x0500 // Windows 2000
#define _WIN32_WINNT_WINXP                  0x0501 // Windows XP
#define _WIN32_WINNT_WS03                   0x0502 // Windows Server 2003
#define _WIN32_WINNT_WIN6                   0x0600 // Windows Vista
#define _WIN32_WINNT_VISTA                  0x0600 // Windows Vista
#define _WIN32_WINNT_WS08                   0x0600 // Windows Server 2008
#define _WIN32_WINNT_LONGHORN               0x0600 // Windows Vista
#define _WIN32_WINNT_WIN7                   0x0601 // Windows 7
#define _WIN32_WINNT_WIN8                   0x0602 // Windows 8
#define _WIN32_WINNT_WINBLUE                0x0603 // Windows 8.1
#define _WIN32_WINNT_WINTHRESHOLD           0x0A00 // Windows 10
#define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10

#if !defined(_WIN32_WINNT) && !defined(_CHICAGO_)
#define  _WIN32_WINNT   _WIN32_WINNT_WIN10
#endif


#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>


// boost::asio 가 되도록 include path를 프로젝트 설정에 넣었으므로 asio 만 쓰면 오류가 나는 문제를 해결 하기 위해
namespace BAsio = boost::asio;
namespace BAssl = boost::asio::ssl;
namespace BAIP  = boost::asio::ip;
typedef boost::asio::ip::tcp BATcp;
typedef boost::system::system_error TException;
typedef boost::system::error_code TErrCode; //?코드변경 3 std::error_code 가 오류 나서 boost::system::error_code  로 바꿔

///
//catch(const boost::system::system_error& ex)
//{    TErrCode ec = ex.code();
//catch(const TException& ex)
//{    TErrCode ec = ex.code();

/* //kdw
//boost::asio
        boost::system::error_code ec; 

        
        
        */
#endif // KWADJUST_H

/// C:\Dropbox\Proj\CmnJ\KwLib64\Kw_tool.h
///    에도 중복 되어 있다. 편집 하려거든 같이 해줘라.
/// 

#ifndef KTRACEOUTPUT
#define KTRACEOUTPUT
#  ifdef _MSC_VER
#    include <windows.h>
#    include <sstream>
#    define TRACEX(x)                           \
     do {  std::stringstream s;  s << (x);     \
           OutputDebugStringA(s.str().c_str()); \
        } while(0)
#  else
#    include <iostream>
#    define TRACEX(x)  std::clog << (x)
#  endif        // or std::cerr << (x) << std::flush

/// <summary>
/// 이 class는 std consol로 << 하여 출력 하는 stream 코드를 그대로 살릴 목적으로 만든 거다.
/// std_cout << "~CMyHttp() destroyed." << std_endl;  처럼. CKTrace std_cout; 을 미리 정의 하고.쓴다. std_endl 도 만들어 놓는다.
/// </summary>
class CKTrace
{
public:
	CKTrace(bool bDebug = false)
		: _debug(bDebug)
	{
	}
	bool _debug;
	std::stringstream _s;

	std::string str()
	{
		return _s.str();
	}

	std::shared_ptr<std::function<void(std::string)>> _fncTrace;
	void AddCallbackOnTrace(std::shared_ptr<std::function<void(std::string)>> fnc)//?ExTrace 5 CKTrace::fnc 에 저장
	{
		if(!_fncTrace)
			_fncTrace = fnc;
	}

	void Output(const char* txt)
	{
		if(strcmp(txt, "\r\n") == 0)
		{
			if(_fncTrace)
			{
				if(!_debug)
					(*_fncTrace)(_s.str());//?ExTrace 6 Output에서 실행. 추가 출력은 줄바꿈을 안 넣고 내 보낸다.
			}
			_s << txt;
			//TRACEX(txt);
			OutputDebugStringA(_s.str().c_str());
			_s.str("");// = "";//.clear(); 이게 말을 안듣네.
		}
		else
			_s << txt;
	}

    CKTrace& operator<<(const char* ctr) {
		Output(ctr);
        return *this;
    }
    CKTrace& operator<<(const std::string& ctr) {
        //_s << ctr;//= _s + str;
		Output(ctr.c_str());
        return *this;
    }
	CKTrace& operator<<(const std::wstring& ctr)
	{
        std::string ctra;
        ctra.assign(ctr.begin(), ctr.end());
		//CStringA ctra(ctr.c_str());
		Output(ctra.c_str());
		return *this;
	}

    CKTrace& operator<<(const std::stringstream& ctr) {
		Output(ctr.str().c_str());
        return *this;
    }
//     CKTrace& operator<<(int ctr) {
//         char buf[24];
//         _itoa_s(ctr, buf, 10);
//         TRACEX(buf);
//         return *this;
//     }
//     CKTrace& operator<<(unsigned long long ctr) {
//         std::string str = std::to_string(ctr);
//         TRACEX(str.c_str());
//         return *this;
//     }
//     CKTrace& operator<<(double ctr) {
//         char buf[100];
//         sprintf_s(buf, "%.6f", ctr);
//         TRACEX(buf);
//         return *this;
//     }
	template<typename TNUM>
	CKTrace& operator<<(TNUM ctr)
	{
		std::string str = std::to_string(ctr);
		Output(str.c_str());
		return *this;
	}

};
#define std_endl "\r\n" //std::endl

//CKTrace _trace;
#endif
