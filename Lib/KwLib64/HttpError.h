#pragma once

class HttpError
{
public:
	int err;
	char errMsg[92];

};

extern HttpError g_hErr[100];
/// 참조: CppServer HTTPResponse::SetBegin
enum
{
	eHttp_Continue = 100,
	eHttp_Switching = 101,
	eHttp_Processing = 102,
	eHttp_Early = 103,

	eHttp_OK = 200,
	eHttp_Created = 201,
	eHttp_Accepted = 202,
	eHttp_Non = 203,
	eHttp_No = 204,
	eHttp_Reset = 205,
	eHttp_Partial = 206,
	eHttp_Multi = 207,
	eHttp_Already = 208,
	eHttp_IM = 226,
	
	eHttp_Multiple = 300,
	eHttp_Moved = 301,
	eHttp_Found = 302,
	eHttp_See = 303,
	eHttp_Not = 304,
	eHttp_Use = 305,
	eHttp_Switch = 306,
	eHttp_Temporary = 307,
	eHttp_Permanent = 308,
	// 자주 쓰이는 거
	eHttp_Bad_Request = 400,
	eHttp_Unauthorized = 401,
	eHttp_Not_Found = 404,

	eHttp_Forbidden = 403,
	eHttp_Upgrade_Required = 426,


	eHttp_Payment_Required = 402,
	eHttp_Method_Not_Allowed = 405,
	eHttp_Not_Acceptable = 406,
	eHttp_Proxy_Authentication_Required = 407,
	eHttp_Request_Timeout = 408,
	eHttp_Conflict = 409,
	eHttp_Gone = 410,
	eHttp_Length_Required = 411,
	eHttp_Precondition_Failed = 412,
	eHttp_Payload_Too_Large = 413,
	eHttp_URI_Too_Long = 414,
	eHttp_Unsupported_Media_Type = 415,
	eHttp_Requested_Range_Not_Satisfiable = 416,
	eHttp_Expectation_Failed = 417,
	eHttp_Im_a_teapot = 418,
	eHttp_Misdirected_Request = 421,
	eHttp_Unprocessable_Entity_WebDAV = 422,
	eHttp_Locked_WebDAV = 423,
	eHttp_Failed_Dependency_WebDAV = 424,
	eHttp_Precondition_Required = 428,
	eHttp_Too_Many_Requests = 429,
	eHttp_Request_Header_Fields_Too_Large = 431,
	eHttp_Unavailable_For_Legal_Reasons = 451,
	eHttp_Internal_Server_Error = 500,
	eHttp_Not_Implemented = 501,
	eHttp_Bad_Gateway = 502,
	eHttp_Service_Unavailable = 503,
	eHttp_Gateway_Timeout = 504,
	eHttp_HTTP_Version_Not_Supported = 505,
	eHttp_Variant_Also_Negotiates = 506,
	eHttp_Insufficient_Storage = 507,
	eHttp_Loop_Detected_WebDAV = 508,
	eHttp_Not_Extended = 510,
	eHttp_Network_Authentication_Required = 511,
};
