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
	Request(const int client, const int serverId);
	Request();

	~Request();

	void resetRequest();

public:
	void _initRequest();

	void _readSocketData();

	void _parseStartLine();
		void _setType(std::string &type);
		void _setPath(std::string &path);
		void _setVersion(std::string &version);

	void _parseHeaders();

	void _parseBody();

	void _resetRequest();

	std::stringstream 					_rawRequest;
	startLine							_startLine;
	std::map<std::string, std::string>	_headers;
	std::string							_body;

	int 								_client;
	int 								_serverId;

private:


};


#endif
