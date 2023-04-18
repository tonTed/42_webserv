#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "../../includes/webserv.hpp"
#include "RequestException.hpp"
//#include "../Response/Response.hpp"

struct startLine {
	eRequestType	type;
	std::string		path;
	std::string 	version;
	std::string		queryString;
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
	Request();
	~Request();

	void 	resetRequest();

	void 	setClient(int client);
	void 	setServerId(int serverId);
	void	setCGIFd(int cgiFd[2]);

	int		getClient() const;

	bool 	isCGI() const;

public:
	void initRequest();

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

	int 								_client;
	int 								_serverId;
	int 								_status;
	int 								_cgiFd[2];
	bool 								_isCGI;

private:


};


#endif
