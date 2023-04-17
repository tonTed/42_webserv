#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../../includes/webserv.hpp"
#include "../Request/Request.hpp"
#include "CodeResponse.hpp"
#include "DefaultHtml.hpp"

class Response : CodeResponse {

public:

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
	void 		formatErrorDefaultResponse();

	std::string 	setBody();
	std::string 	setHeader(int bodyLength);
	void 			setResponse(const std::string &header, const std::string &body);

	void 			sendResponse();

	void 			manageErrorResponse();
	void 			manageValidResponse();


public:
		const Request & _request;
		int 			_status;
		ServerData		_serverData;
		std::string		_root;
		std::string 	_response;
};


#endif
