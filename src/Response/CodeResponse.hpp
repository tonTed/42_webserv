
#ifndef CODERESPONSE_HPP
# define CODERESPONSE_HPP

#include "../../includes/webserv.hpp"

class CodeResponse {

public:
	typedef std::map<int, std::string> CodeResponseMap;
	static CodeResponseMap _codeResponse;

private:
	static CodeResponseMap init_map() {
		CodeResponseMap _map;
		_map[100] = "Continue";
		_map[101] = "Switching Protocols";
		_map[102] = "Processing";
		_map[103] = "Early Hints";
		_map[200] = "OK";
		_map[201] = "Created";
		_map[202] = "Accepted";
		_map[203] = "Non-Authoritative Information";
		_map[204] = "No Content";
		_map[205] = "Reset Content";
		_map[206] = "Partial Content";
		_map[207] = "Multi-Status";
		_map[208] = "Already Reported";
		_map[226] = "IM Used";
		_map[300] = "Multiple Choices";
		_map[301] = "Moved Permanently";
		_map[302] = "Found";
		_map[303] = "See Other";
		_map[304] = "Not Modified";
		_map[307] = "Temporary Redirect";
		_map[308] = "Permanent Redirect";
		_map[400] = "Bad Request";
		_map[401] = "Unauthorized";
		_map[402] = "Payment Required";
		_map[403] = "Forbidden";
		_map[404] = "Not Found";
		_map[405] = "Method Not Allowed";
		_map[406] = "Not Acceptable";
		_map[407] = "Proxy Authentication Required";
		_map[408] = "Request Timeout";
		_map[409] = "Conflict";
		_map[410] = "Gone";
		_map[411] = "Length Required";
		_map[412] = "Precondition Failed";
		_map[413] = "Payload Too Large";
		_map[414] = "URI Too Long";
		_map[415] = "Unsupported Media Type";
		_map[416] = "Range Not Satisfiable";
		_map[417] = "Expectation Failed";
		_map[418] = "I'm a teapot";
		_map[421] = "Misdirected Request";
		_map[422] = "Unprocessable Entity";
		_map[423] = "Locked";
		_map[424] = "Failed Dependency";
		_map[425] = "Too Early";
		_map[426] = "Upgrade Required";
		_map[428] = "Precondition Required";
		_map[429] = "Too Many Requests";
		_map[431] = "Request Header Fields Too Large";
		_map[451] = "Unavailable For Legal Reasons";
		_map[500] = "Internal Server Error";
		_map[501] = "Not Implemented";
		_map[502] = "Bad Gateway";
		_map[503] = "Service Unavailable";
		_map[504] = "Gateway Timeout";
		_map[505] = "HTTP Version Not Supported";
		_map[506] = "Variant Also Negotiates";
		_map[507] = "Insufficient Storage";
		_map[508] = "Loop Detected";
		_map[510] = "Not Extended";
		return _map;
	}
};

#endif
