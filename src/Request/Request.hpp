#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../../includes/webserv.hpp"
#include "RequestException.hpp"

struct startLine {
	eRequestType	type;
	std::string		path;
	std::string 	version;
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
	Request(const int client, const int cgiFd, const int serverId);
	~Request();


public:
	void _init();

	void _readSocketData();

	void _parseStartLine();
		void _setType(std::string &type);
		void _setPath(std::string &path);
		void _setVersion(std::string &version);

	void _parseHeaders();

	void _parseBody();

	std::stringstream 					_rawRequest;
	startLine							_startLine;
	std::map<std::string, std::string>	_headers;
	std::string							_body;

	const int 							_client;
	const int 							_cgiFd;
	const int 							_serverId;

private:
	Request();


};


#endif
