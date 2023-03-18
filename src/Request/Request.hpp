#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <map>
#include <sstream>
#include <iostream>

#include "../../includes/webserv.hpp"
#include "RequestException.hpp"

struct RequestLine {
	eRequestType	_type;
	std::string		_path;
	std::string 	_version;
};

/**
 *
 * @attention	while developing (unit test):
 * 	- Calls <instance>._init() before instantiate object.
 * 	- Change public block with methods beginning with `_`
 *
 * */
class Request {
public:
	Request(const int client);
	~Request();


public:
	void _init();

	void _getRawRequest();


private:
	Request();

	const int 							_client;

	std::stringstream 					_rawRequest;
	RequestLine							_requestLine;
	std::map<std::string, std::string>	_headers;
	std::string							_body;

};


#endif
