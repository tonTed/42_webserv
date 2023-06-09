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
	std::string		pathInfo;
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
			void _setQueryString(std::string &path);
			void _setPathInfo(std::string &path);
		void _setVersion(std::string &version);
		std::string getPath(const std::string &uri);

	void _parseHeaders();

	void _parseBody();

	void	_manageRequest();
		void _manageOurTrigger();

	std::stringstream 					_rawRequest;
	startLine							_startLine;
	std::map<std::string, std::string>	_headers;
	std::string							_body;
	std::string 						_root;

	std::string 						_bufferString;

	int 								_client;
	int 								_serverId;
	int 								_status;
	int 								_cgiFd[2];
	bool 								_isCGI;
	bool								_autoIndex;

private:


};


#endif
