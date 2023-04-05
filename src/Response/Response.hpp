#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../../includes/webserv.hpp"
#include "../Request/Request.hpp"
#include "CodeResponse.hpp"

class Response : CodeResponse {

public:
	Response(const Request &request, const int status);

	void 			sendError();

	std::string	resolvePath(const std::string &path);
	bool		hasExtension(const std::string &path);
	void		removeFile(std::string &path, std::string &file);
	bool		existInLocation(const std::string &path);
	bool		existInRoot(const std::string &path);
	bool		localRootExist(const std::string &path);
	bool		localFileExist(const std::string &path);
	bool		indexExist(std::string &path);
	void		setLocalRoot(const std::string path);

public:
		const Request & _request;
		int 			_status;
		ConfigServer	*_config;
		std::string		_root;
};


#endif
