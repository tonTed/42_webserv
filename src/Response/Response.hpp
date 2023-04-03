#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../../includes/webserv.hpp"
#include "../Request/Request.hpp"

class Response {

public:
	Response(const Request &request, const int status);

	void 			sendError();

	std::string	resolvePath(const std::string &path);

public:
		const Request & _request;
		int 			_status;
		ConfigServer	*_config;
};


#endif
