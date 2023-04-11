#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../../includes/webserv.hpp"
#include "../Request/Request.hpp"
#include "CodeResponse.hpp"

class Response : CodeResponse {

public:
	Response(const Request &request, const int status);


	void 		resolveErrorPages(const int status);

	void		resolvePath();
	std::string 	getLocation();
	bool 			setRoot(const std::string &location);
	bool			hasExtension(const std::string &path);
	bool 			isRootValid();
	void 			addIndex(bool hasLocation, std::string location);

public:
		const Request & _request;
		int 			_status;
		ConfigServer	*_config;
		std::string		_root;
};


#endif
