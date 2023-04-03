#include "Response.hpp"

Response::Response(const Request &request, int status) : _request(request), _status(status){
	_config = ConfigServer::getInstance();

	if (status != 200) {
		//send error
	}
	else {
		//send file
	}
}

void Response::sendError() {
	std::string	path;
	std::string errorPage;

	path = resolvePath(_request._startLine.path);
}

std::string Response::resolvePath(const std::string &path) {
	(void)path;

	std::string resolvedPath = "Hello" ;

	return resolvedPath;
}