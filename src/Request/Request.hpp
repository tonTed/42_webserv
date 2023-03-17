#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <sstream>
#include <iostream>

#include "../../includes/webserv.hpp"

struct RequestLine {
	eRequestType	_type;
	std::string		_path;
	std::string 	_version;
};

class Request {
public:
	Request(const int client);
	~Request();

private:
	Request();

	const int 							_client;

	std::string							_rawRequest;
	RequestLine							_requestLine;
	std::map<std::string, std::string>	_headers;
	std::string							_body;

	void _getRawRequest();
};


#endif
