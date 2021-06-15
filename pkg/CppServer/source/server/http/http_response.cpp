/*!
    \file http_response.cpp
    \brief HTTP response implementation
    \author Ivan Shynkarenka
    \date 15.02.2019
    \copyright MIT License
*/
#include "server/kwadjust.h"

#include "server/http/http_response.h"

#include "errors/exceptions.h"
#include "string/format.h"
#include "string/string_utils.h"
#include "utility/countof.h"

#include <cassert>

namespace CppServer {
namespace HTTP {

std::tuple<std::string_view, std::string_view> HTTPResponse::header(size_t i) const noexcept
{
    assert((i < _headers.size()) && "Index out of bounds!");
    if (i >= _headers.size())
        return std::make_tuple(std::string_view(), std::string_view());

    auto item = _headers[i];

    return std::make_tuple(std::string_view(_cache.data() + std::get<0>(item), std::get<1>(item)), std::string_view(_cache.data() + std::get<2>(item), std::get<3>(item)));
}

HTTPResponse& HTTPResponse::Clear()
{
    _error = false;
    _status = 0;
    _status_phrase_index = 0;
    _status_phrase_size = 0;
    _protocol_index = 0;
    _protocol_size = 0;
    _headers.clear();
    _body_index = 0;
    _body_size = 0;
    _body_length = 0;
    _body_length_provided = false;

    _cache.clear();
    _cache_size = 0;
    return *this;
}

HTTPResponse& HTTPResponse::SetBegin(int status, std::string_view protocol)
{
    std::string status_phrase;

    switch (status)
    {
        case 100: status_phrase = "Continue"; break;
        case 101: status_phrase = "Switching Protocols"; break;
        case 102: status_phrase = "Processing"; break;
        case 103: status_phrase = "Early Hints"; break;

        case 200: status_phrase = "OK"; break;
        case 201: status_phrase = "Created"; break;
        case 202: status_phrase = "Accepted"; break;
        case 203: status_phrase = "Non-Authoritative Information"; break;
        case 204: status_phrase = "No Content"; break;
        case 205: status_phrase = "Reset Content"; break;
        case 206: status_phrase = "Partial Content"; break;
        case 207: status_phrase = "Multi-Status"; break;
        case 208: status_phrase = "Already Reported"; break;

        case 226: status_phrase = "IM Used"; break;

        case 300: status_phrase = "Multiple Choices"; break;
        case 301: status_phrase = "Moved Permanently"; break;
        case 302: status_phrase = "Found"; break;
        case 303: status_phrase = "See Other"; break;
        case 304: status_phrase = "Not Modified"; break;
        case 305: status_phrase = "Use Proxy"; break;
        case 306: status_phrase = "Switch Proxy"; break;
        case 307: status_phrase = "Temporary Redirect"; break;
        case 308: status_phrase = "Permanent Redirect"; break;

        case 400: status_phrase = "Bad Request"; break;
        case 401: status_phrase = "Unauthorized"; break;
        case 402: status_phrase = "Payment Required"; break;
        case 403: status_phrase = "Forbidden"; break;
        case 404: status_phrase = "Not Found"; break;
        case 405: status_phrase = "Method Not Allowed"; break;
        case 406: status_phrase = "Not Acceptable"; break;
        case 407: status_phrase = "Proxy Authentication Required"; break;
        case 408: status_phrase = "Request Timeout"; break;
        case 409: status_phrase = "Conflict"; break;
        case 410: status_phrase = "Gone"; break;
        case 411: status_phrase = "Length Required"; break;
        case 412: status_phrase = "Precondition Failed"; break;
        case 413: status_phrase = "Payload Too Large"; break;
        case 414: status_phrase = "URI Too Long"; break;
        case 415: status_phrase = "Unsupported Media Type"; break;
        case 416: status_phrase = "Range Not Satisfiable"; break;
        case 417: status_phrase = "Expectation Failed"; break;

        case 421: status_phrase = "Misdirected Request"; break;
        case 422: status_phrase = "Unprocessable Entity"; break;
        case 423: status_phrase = "Locked"; break;
        case 424: status_phrase = "Failed Dependency"; break;
        case 425: status_phrase = "Too Early"; break;
        case 426: status_phrase = "Upgrade Required"; break;
        case 427: status_phrase = "Unassigned"; break;
        case 428: status_phrase = "Precondition Required"; break;
        case 429: status_phrase = "Too Many Requests"; break;
        case 431: status_phrase = "Request Header Fields Too Large"; break;

        case 451: status_phrase = "Unavailable For Legal Reasons"; break;

        case 500: status_phrase = "Internal Server Error"; break;
        case 501: status_phrase = "Not Implemented"; break;
        case 502: status_phrase = "Bad Gateway"; break;
        case 503: status_phrase = "Service Unavailable"; break;
        case 504: status_phrase = "Gateway Timeout"; break;
        case 505: status_phrase = "HTTP Version Not Supported"; break;
        case 506: status_phrase = "Variant Also Negotiates"; break;
        case 507: status_phrase = "Insufficient Storage"; break;
        case 508: status_phrase = "Loop Detected"; break;

        case 510: status_phrase = "Not Extended"; break;
        case 511: status_phrase = "Network Authentication Required"; break;

        default: status_phrase = "Unknown"; break;
    }

    SetBegin(status, status_phrase, protocol);
    return *this;
}

HTTPResponse& HTTPResponse::SetBegin(int status, std::string_view status_phrase, std::string_view protocol)
{
    // Clear the HTTP response cache
    Clear();

    size_t index = 0;

    // Append the HTTP response protocol version
    _cache.append(protocol);
    _protocol_index = index;
    _protocol_size = protocol.size();

    _cache.append(" ");
    index = _cache.size();

    // Append the HTTP response status
    char buffer[32];
    _cache.append(FastConvert(status, buffer, CppCommon::countof(buffer)));
    _status = status;

    _cache.append(" ");
    index = _cache.size();

    // Append the HTTP response status phrase
    _cache.append(status_phrase);
    _status_phrase_index = index;
    _status_phrase_size = status_phrase.size();

    _cache.append("\r\n");
    return *this;
}

HTTPResponse& HTTPResponse::SetContentType(std::string_view extension)
{
    // Base content types
    if (extension == ".html")
        return SetHeader("Content-Type", "text/html");
    else if (extension == ".css")
        return SetHeader("Content-Type", "text/css");
    else if (extension == ".js")
        return SetHeader("Content-Type", "text/javascript");
    else if (extension == ".xml")
        return SetHeader("Content-Type", "text/xml");

    // Common content types
    if (extension == ".gzip")
        return SetHeader("Content-Type", "application/gzip");
    else if (extension == ".json")
        return SetHeader("Content-Type", "application/json");
    else if (extension == ".map")
        return SetHeader("Content-Type", "application/json");
    else if (extension == ".pdf")
        return SetHeader("Content-Type", "application/pdf");
    else if (extension == ".zip")
        return SetHeader("Content-Type", "application/zip");
    else if (extension == ".mp3")
        return SetHeader("Content-Type", "audio/mpeg");
    else if (extension == ".jpg")
        return SetHeader("Content-Type", "image/jpeg");
    else if (extension == ".gif")
        return SetHeader("Content-Type", "image/gif");
    else if (extension == ".png")
        return SetHeader("Content-Type", "image/png");
    else if (extension == ".svg")
        return SetHeader("Content-Type", "image/svg+xml");
    else if (extension == ".mp4")
        return SetHeader("Content-Type", "video/mp4");

    // Application content types
    if (extension == ".atom")
        return SetHeader("Content-Type", "application/atom+xml");
    else if (extension == ".fastsoap")
        return SetHeader("Content-Type", "application/fastsoap");
    else if (extension == ".ps")
        return SetHeader("Content-Type", "application/postscript");
    else if (extension == ".soap")
        return SetHeader("Content-Type", "application/soap+xml");
    else if (extension == ".sql")
        return SetHeader("Content-Type", "application/sql");
    else if (extension == ".xslt")
        return SetHeader("Content-Type", "application/xslt+xml");
    else if (extension == ".zlib")
        return SetHeader("Content-Type", "application/zlib");

    // Audio content types
    if (extension == ".aac")
        return SetHeader("Content-Type", "audio/aac");
    else if (extension == ".ac3")
        return SetHeader("Content-Type", "audio/ac3");
    else if (extension == ".ogg")
        return SetHeader("Content-Type", "audio/ogg");

    // Font content types
    if (extension == ".ttf")
        return SetHeader("Content-Type", "font/ttf");

    // Image content types
    if (extension == ".bmp")
        return SetHeader("Content-Type", "image/bmp");
    else if (extension == ".jpm")
        return SetHeader("Content-Type", "image/jpm");
    else if (extension == ".jpx")
        return SetHeader("Content-Type", "image/jpx");
    else if (extension == ".jrx")
        return SetHeader("Content-Type", "image/jrx");
    else if (extension == ".tiff")
        return SetHeader("Content-Type", "image/tiff");
    else if (extension == ".emf")
        return SetHeader("Content-Type", "image/emf");
    else if (extension == ".wmf")
        return SetHeader("Content-Type", "image/wmf");

    // Message content types
    if (extension == ".http")
        return SetHeader("Content-Type", "message/http");
    else if (extension == ".s-http")
        return SetHeader("Content-Type", "message/s-http");

    // Model content types
    if (extension == ".mesh")
        return SetHeader("Content-Type", "model/mesh");
    else if (extension == ".vrml")
        return SetHeader("Content-Type", "model/vrml");

    // Text content types
    if (extension == ".csv")
        return SetHeader("Content-Type", "text/csv");
    else if (extension == ".plain")
        return SetHeader("Content-Type", "text/plain");
    else if (extension == ".richtext")
        return SetHeader("Content-Type", "text/richtext");
    else if (extension == ".rtf")
        return SetHeader("Content-Type", "text/rtf");
    else if (extension == ".rtx")
        return SetHeader("Content-Type", "text/rtx");
    else if (extension == ".sgml")
        return SetHeader("Content-Type", "text/sgml");
    else if (extension == ".strings")
        return SetHeader("Content-Type", "text/strings");
    else if (extension == ".url")
        return SetHeader("Content-Type", "text/uri-list");

    // Video content types
    if (extension == ".H264")
        return SetHeader("Content-Type", "video/H264");
    else if (extension == ".H265")
        return SetHeader("Content-Type", "video/H265");
    else if (extension == ".mpeg")
        return SetHeader("Content-Type", "video/mpeg");
    else if (extension == ".raw")
        return SetHeader("Content-Type", "video/raw");

    return *this;
}

HTTPResponse& HTTPResponse::SetHeader(std::string_view key, std::string_view value)
{
    size_t index = _cache.size();

    // Append the HTTP response header's key
    _cache.append(key);
    size_t key_index = index;
    size_t key_size = key.size();

    _cache.append(": ");
    index = _cache.size();

    // Append the HTTP response header's value
    _cache.append(value);
    size_t value_index = index;
    size_t value_size = value.size();

    _cache.append("\r\n");

    // Add the header to the corresponding collection
    _headers.emplace_back(key_index, key_size, value_index, value_size);
    return *this;
}

HTTPResponse& HTTPResponse::SetCookie(std::string_view name, std::string_view value, size_t max_age, std::string_view path, std::string_view domain, bool secure, bool http_only)
{
    size_t index = _cache.size();

    // Append the HTTP response header's key
    _cache.append("Set-Cookie");
    size_t key_index = index;
    size_t key_size = 10;

    _cache.append(": ");
    index = _cache.size();

    // Append the HTTP response header's value
    size_t value_index = index;

    char buffer[32];

    // Append cookie
    _cache.append(name);
    _cache.append("=");
    _cache.append(value);
    _cache.append("; Max-Age=");
    _cache.append(FastConvert(max_age, buffer, CppCommon::countof(buffer)));
    if (!domain.empty())
    {
        _cache.append("; Domain=");
        _cache.append(domain);
    }
    if (!path.empty())
    {
        _cache.append("; Path=");
        _cache.append(path);
    }
    if (secure)
        _cache.append("; Secure");
    if (http_only)
        _cache.append("; HttpOnly");

    size_t value_size = _cache.size() - value_index;

    _cache.append("\r\n");

    // Add the header to the corresponding collection
    _headers.emplace_back(key_index, key_size, value_index, value_size);
    return *this;
}

HTTPResponse& HTTPResponse::SetBody(std::string_view body)
{
    // Append non empty content length header
    char buffer[32];
    SetHeader("Content-Length", FastConvert(body.size(), buffer, CppCommon::countof(buffer)));

    _cache.append("\r\n");

    size_t index = _cache.size();

    // Append the HTTP response body
    _cache.append(body);
    _body_index = index;
    _body_size = body.size();
    _body_length = body.size();
    _body_length_provided = true;
    return *this;
}
HTTPResponse& HTTPResponse::SetBody(const char* body, size_t len)
{
    // Append non empty content length header
    char buffer[32];
    SetHeader("Content-Length", FastConvert(len, buffer, CppCommon::countof(buffer)));

    _cache.append("\r\n");

    size_t index = _cache.size();

    // Append the HTTP response body
    _cache.append(body, len);
    _body_index = index;
    _body_size = len;
    _body_length = len;
    _body_length_provided = true;
    return *this;
}

HTTPResponse& HTTPResponse::SetBodyLength(size_t length)
{
    // Append content length header
    char buffer[32];
    SetHeader("Content-Length", FastConvert(length, buffer, CppCommon::countof(buffer)));

    _cache.append("\r\n");

    size_t index = _cache.size();

    // Clear the HTTP response body
    _body_index = index;
    _body_size = 0;
    _body_length = length;
    _body_length_provided = true;
    return *this;
}

HTTPResponse& HTTPResponse::MakeOKResponse(int status)
{
    Clear();
    SetBegin(status);
    SetBody();
    return *this;
}

HTTPResponse& HTTPResponse::MakeErrorResponse(std::string_view error, int status)
{
    Clear();
    SetBegin(status);
    SetBody(error);
    return *this;
}

HTTPResponse& HTTPResponse::MakeHeadResponse()
{
    Clear();
    SetBegin(200);
    SetBody();
    return *this;
}

HTTPResponse& HTTPResponse::MakeGetResponse(std::string_view content, std::string_view content_type)
{
    Clear();
    SetBegin(200);
    if (!content_type.empty())
        SetHeader("Content-Type", content_type);
    SetBody(content);
    return *this;
}
HTTPResponse& HTTPResponse::MakeGetResponse(const char* body, size_t len, std::string_view content_type)
{
    Clear();
    SetBegin(200);
    if (!content_type.empty())
        SetHeader("Content-Type", content_type);
    SetBody(body, len);
    return *this;
}
HTTPResponse& HTTPResponse::MakeOptionsResponse(std::string_view allow)
{
    Clear();
    SetBegin(200);
    SetHeader("Allow", allow);
    SetBody();
    return *this;
}

HTTPResponse& HTTPResponse::MakeTraceResponse(std::string_view request)
{
    Clear();
    SetBegin(200);
    SetHeader("Content-Type", "message/http");
    SetBody(request);
    return *this;
}

bool HTTPResponse::IsPendingHeader() const
{
    return (!_error && (_body_index == 0));
}

bool HTTPResponse::IsPendingBody() const
{
    return (!_error && (_body_index > 0) && (_body_size > 0));
}

bool HTTPResponse::ReceiveHeader(const void* buffer, size_t size)
{
    // Update the response cache
    _cache.insert(_cache.end(), (const char*)buffer, (const char*)buffer + size);

    // Try to seek for HTTP header separator
    for (size_t i = _cache_size; i < _cache.size(); ++i)
    {
        // Check for the response cache out of bounds
        if ((i + 3) >= _cache.size())
            break;

        // Check for the header separator
        if ((_cache[i + 0] == '\r') && (_cache[i + 1] == '\n') && (_cache[i + 2] == '\r') && (_cache[i + 3] == '\n'))
        {
            size_t index = 0;

            // Set the error flag for a while...
            _error = true;

            // Parse protocol version
            _protocol_index = index;
            _protocol_size = 0;
            while (_cache[index] != ' ')
            {
                ++_protocol_size;
                ++index;
                if (index >= _cache.size())
                    return false;
            }
            ++index;
            if (index >= _cache.size())
                return false;

            // Parse status code
            size_t status_index = index;
            size_t status_size = 0;
            while (_cache[index] != ' ')
            {
                if ((_cache[index] < '0') || (_cache[index] > '9'))
                    return false;
                ++status_size;
                ++index;
                if (index >= _cache.size())
                    return false;
            }
            _status = 0;
            for (size_t j = status_index; j < (status_index + status_size); ++j)
            {
                _status *= 10;
                _status += _cache[j] - '0';
            }
            ++index;
            if (index >= _cache.size())
                return false;

            // Parse status phrase
            _status_phrase_index = index;
            _status_phrase_size = 0;
            while (_cache[index] != '\r')
            {
                ++_status_phrase_size;
                ++index;
                if (index >= _cache.size())
                    return false;
            }
            ++index;
            if ((index >= _cache.size()) || (_cache[index] != '\n'))
                return false;
            ++index;
            if (index >= _cache.size())
                return false;

            // Parse headers
            while ((index < _cache.size()) && (index < i))
            {
                // Parse header name
                size_t header_name_index = index;
                size_t header_name_size = 0;
                while (_cache[index] != ':')
                {
                    ++header_name_size;
                    ++index;
                    if (index >= i)
                        break;
                    if (index >= _cache.size())
                        return false;
                }
                ++index;
                if (index >= i)
                    break;
                if (index >= _cache.size())
                    return false;

                // Skip all prefix space characters
                while (std::isspace(_cache[index]))
                {
                    ++index;
                    if (index >= i)
                        break;
                    if (index >= _cache.size())
                        return false;
                }

                // Parse header value
                size_t header_value_index = index;
                size_t header_value_size = 0;
                while (_cache[index] != '\r')
                {
                    ++header_value_size;
                    ++index;
                    if (index >= i)
                        break;
                    if (index >= _cache.size())
                        return false;
                }
                ++index;
                if ((index >= _cache.size()) || (_cache[index] != '\n'))
                    return false;
                ++index;
                if (index >= _cache.size())
                    return false;

                // Validate header name and value (sometimes value can be empty)
                if (header_name_size == 0)
                    return false;

                // Add a new header
                _headers.emplace_back(header_name_index, header_name_size, header_value_index, header_value_size);

                // Try to find the body content length
                if (CppCommon::StringUtils::CompareNoCase(std::string_view(_cache.data() + header_name_index, header_name_size), "Content-Length"))
                {
                    _body_length = 0;
                    for (size_t j = header_value_index; j < (header_value_index + header_value_size); ++j)
                    {
                        if ((_cache[j] < '0') || (_cache[j] > '9'))
                            return false;
                        _body_length *= 10;
                        _body_length += _cache[j] - '0';
                        _body_length_provided = true;
                    }
                }
            }

            // Reset the error flag
            _error = false;

            // Update the body index and size
            _body_index = i + 4;
            _body_size = _cache.size() - i - 4;

            // Update the parsed cache size
            _cache_size = _cache.size();

            return true;
        }
    }

    // Update the parsed cache size
    _cache_size = (_cache.size() >= 3) ? (_cache.size() - 3) : 0;

    return false;
}

bool HTTPResponse::ReceiveBody(const void* buffer, size_t size)
{
    // Update HTTP response cache
    _cache.insert(_cache.end(), (const char*)buffer, (const char*)buffer + size);

    // Update the parsed cache size
    _cache_size = _cache.size();

    // Update body size
    _body_size += size;

    // Check if the body was fully parsed
    if (_body_length_provided && (_body_size >= _body_length))
    {
        _body_size = _body_length;
        return true;
    }

    return false;
}

std::string_view HTTPResponse::FastConvert(size_t value, char* buffer, size_t size)
{
    size_t index = size;
    do
    {
        buffer[--index] = '0' + (value % 10);
        value /= 10;
    }
    while (value > 0);
    return std::string_view(buffer + index, size - index);
}

std::ostream& operator<<(std::ostream& os, const HTTPResponse& response)
{
    os << "Status: " << response.status() << std::endl;
    os << "Status phrase: " << response.status_phrase() << std::endl;
    os << "Protocol: " << response.protocol() << std::endl;
    os << "Headers: " << response.headers() << std::endl;
    for (size_t i = 0; i < response.headers(); ++i)
    {
        auto header = response.header(i);
        os << std::get<0>(header) << ": " << std::get<1>(header) << std::endl;
    }
    os << "Body:" << response.body_length() << std::endl;
    os << response.body() << std::endl;
    return os;
}

void HTTPResponse::swap(HTTPResponse& response) noexcept
{
    using std::swap;
    swap(_error, response._error);
    swap(_status, response._status);
    swap(_status_phrase_index, response._status_phrase_index);
    swap(_status_phrase_size, response._status_phrase_size);
    swap(_protocol_index, response._protocol_index);
    swap(_protocol_size, response._protocol_size);
    swap(_headers, response._headers);
    swap(_body_index, response._body_index);
    swap(_body_size, response._body_size);
    swap(_body_length, response._body_length);
    swap(_body_length_provided, response._body_length_provided);
    swap(_cache, response._cache);
    swap(_cache_size, response._cache_size);
}

} // namespace HTTP
} // namespace CppServer
