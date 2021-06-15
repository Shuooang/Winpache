// CppCommon3.cpp : 정적 라이브러리를 위한 함수를 정의합니다.
//

//#include "pch.h"
#include "framework.h"
#include <string>
#include <sstream>
// TODO: 라이브러리 함수의 예제입니다.
void fnCppCommon3()
{
    std::string _message = "_message";
    std::string _path = "_path";
    std::string _src = "_src";
    std::string _system_error = "_system_error";
    std::string _system_message = "_system_message";
    std::string _location = "_location";
    std::string _dst = "_dst";
    std::string _cache = "_cache";

    //using stringstream  = basic_stringstream<char, char_traits<char>, allocator<char>>;
    //std::string FileSystemException::string() const
    std::stringstream stream;
    stream << "File system exception: " << _message << std::endl;
    if(!_path.empty())
        stream << "File system path: " << _path << std::endl;
    if(!_src.empty())
        stream << "File system source path: " << _src << std::endl;
    if(!_dst.empty())
        stream << "File system destination path: " << _dst << std::endl;
    stream << "System error: " << _system_error << std::endl;
    stream << "System message: " << _system_message << std::endl;
    std::string location = _location;//.string();
    if(!location.empty())
        stream << "Source location: " << location << std::endl;
    _cache = stream.str();

}
