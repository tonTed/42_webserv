#include "Response.hpp"
#include "../Utils/FT.hpp"

template <typename T, typename Y>
bool	existsInMap(const std::map<T, Y> &map, const T &key) {
	return map.find(key) != map.end();
}

Response::Response(const Request &request) :	_request(request),
												_status(request._status),
												_root(_request._root){	Log::debugFunc(__FUNCTION__);

	Log::log(Log::DEBUG ,"Request Status: " + std::to_string(_request._status));

	_serverData = ConfigServer::getInstance()->getServerData()[_request._serverId];

	if (_status == 301)
	{
		formatResponse();
	}
	else if (_request._status != 200)
	{
		if (_request._autoIndex)
		{
			ListDir listDir;
			_response = listDir.ListDirectory("data" + _request._startLine.path);

			std::cout << RED << _response << RESET << std::endl;

			sendResponse();
			_response.clear();
			return;
		}
		manageErrorResponse();
	}
	else if (_request.isCGI())
	{
		Log::log(Log::DEBUG ,"CGI Response");
		char	buffer[MAX_REQUEST_SIZE + 1];
		size_t 	ret;

		ret = read(_request._cgiFd[PIPE_READ], buffer, MAX_REQUEST_SIZE + 1);
		buffer[ret] = '\0';

		_response = "HTTP/1.1 " + std::to_string(_status) + " " + CodeResponse::_codeResponse[_status] + "\r\n";
		_response += buffer;

		sendResponse();
		return;
	}
	else
	{
		manageValidResponse();
	}
}

void	Response::formatResponse() {	Log::debugFunc(__FUNCTION__);

	std::string body;

	body = setBody();

	setResponse(setHeader((body.length())), body);
}

void	Response::formatErrorDefaultResponse() {	Log::debugFunc(__FUNCTION__);

	std::string body;

	body = DefaultHTML::getHtml(_status);
	setResponse(setHeader(body.length()), body);
}

	void			Response::setResponse(const std::string &header, const std::string &body) {	Log::debugFunc(__FUNCTION__);

		_response = header;
		_response += body;
	}

	std::string 	Response::setBody() const {	Log::debugFunc(__FUNCTION__);

		std::ifstream fd(_root.c_str());
		std::string line;
		std::string body;

		while (std::getline(fd, line))
			body += line;

		fd.close();

		return body;
	}

	std::string 	Response::setHeader(size_t bodyLength) const {	Log::debugFunc(__FUNCTION__);

		std::string headers;

		headers = "HTTP/1.1 " + std::to_string(_status) + " " + CodeResponse::_codeResponse[_status] + "\r\n";
		headers += "Content-Type: text/html\r\n";

		if (bodyLength > 0)
			headers += "Content-Length: " + std::to_string(bodyLength) + "\r\n";
		if (_status == 301)
			headers += "Location: " + _request._startLine.path + "\r\n";

		headers += "Connection: close\r\n";
		headers += "\r\n";

		return headers;
	}

void	Response::sendResponse() const {	Log::debugFunc(__FUNCTION__);


	if (send(_request._client, _response.c_str(), _response.length(), MSG_DONTWAIT) != -1)
		Log::log(Log::INFO , "Response sent to client successfully");
	else
		Log::log(Log::WARNING , "Response not sent");
}

void 	Response::manageErrorResponse() {	Log::debugFunc(__FUNCTION__);


	if(existsInMap(_serverData._errorPages, _status))
	{
		Log::log(Log::DEBUG,"[" + std::to_string(_status) + "] Error page found");

		_root = _serverData._root[0] + "/" + _serverData._errorPages[_status];

		Log::log(Log::DEBUG, "Error Root: " + _root);

		formatResponse();
	}
	else
	{
		Log::log(Log::DEBUG, "[" + std::to_string(_status) + "] Error page not found");

		formatErrorDefaultResponse();
	}
}

void 	Response::manageValidResponse() {	Log::debugFunc(__FUNCTION__);

	if (_status == 200)
		formatResponse();
	else
		manageErrorResponse();
}