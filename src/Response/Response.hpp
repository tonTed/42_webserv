#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../../includes/webserv.hpp"
#include "../Request/Request.hpp"
#include "CodeResponse.hpp"
#include "DefaultHtml.hpp"

class Response : CodeResponse {

public:

	Response(const Request &request);

	void 		formatResponse();
	void 		formatErrorDefaultResponse();

	std::string 	setBody() const;
	std::string 	setHeader(size_t bodyLength) const;
	void 			setResponse(const std::string &header, const std::string &body);

	void 			sendResponse() const;

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
