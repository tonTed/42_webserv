#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../../includes/webserv.hpp"
#include "../Request/Request.hpp"
#include "CodeResponse.hpp"
#include "DefaultHtml.hpp"

class Response : CodeResponse {

public:
	Response(const Request &request, const int status);

	// Pour envoyer une erreur
	Response(int client, int status);

	// Pour envoyer une réponse provenant d'un CGI
	Response(const Request &request);


	void		resolvePath();
	std::string 	getLocation();
	bool 			setRoot(const std::string &location);
	bool			hasExtension(const std::string &path);
	bool 			isRootValid();
	void 			addIndex(bool hasLocation, std::string location);

	void 		formatResponse();
	std::string 	setBody();
	std::string 	setHeader(int bodyLength);
	
	void 		errorResponse();

public:
		const Request & _request;
		int 			_status;
		ConfigServer	*_config;
		std::string		_root;
		std::string 	_response;
};


#endif
