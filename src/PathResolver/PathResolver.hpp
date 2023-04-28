

#ifndef PATHRESOLVER_HPP
#define PATHRESOLVER_HPP

#include "../../includes/webserv.hpp"
#include "../Request/Request.hpp"

class PathResolver {
public:
	PathResolver(Request &req);
	~PathResolver();

	void		resolvePath();
	std::string 	getLocation();
	bool 			setRoot(const std::string &location);
	bool			hasExtension(const std::string &path);
	bool 			isRootValid();
	void 			addIndex(bool hasLocation, std::string location);
	
	bool			isAllowedMethod(std::vector<enum eRequestType> vect);

	Request &		_request;
	ServerData		_serverData;
	std::string	&	_root;
};


#endif
