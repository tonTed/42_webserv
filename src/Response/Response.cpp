#include "Response.hpp"

template <typename T, typename Y>
bool	existsInMap(const std::map<T, Y> &map, const T &key) {
	return map.find(key) != map.end();
}

Response::Response(const Request &request) : _request(request), _status(request._status){	Log::debugFunc(__FUNCTION__);

	Log::log(Log::DEBUG ,"Request Status: " + std::to_string(_request._status));

	_serverData = ConfigServer::getInstance()->getServerData()[_request._serverId];

	if (_request._status != 200)
		manageErrorResponse();
	else
	{
		resolvePath();
		manageValidResponse();
	}
}

void		Response::resolvePath() {	Log::debugFunc(__FUNCTION__);

	std::string location;
	bool 		hasLocation = false;

	location = getLocation();
	hasLocation = setRoot(location);
	if (hasExtension(_root))
	{
		if (!isRootValid())
			_status = 404;
	}
	else //no extension
	{
		addIndex(hasLocation, location);
		if (!isRootValid())
		{
			if (hasLocation)
				_status = 500;
			else
				_status = 404;
		}
	}
}

	std::string	Response::getLocation() {	Log::debugFunc(__FUNCTION__);

		if (_request._startLine.path.find_first_of('/', 1) == std::string::npos)
			return _request._startLine.path;
		else
			return _request._startLine.path.substr(0, _request._startLine.path.find_first_of('/', 1));
	}

	bool 		Response::setRoot(const std::string &location) {	Log::debugFunc(__FUNCTION__);

		// set the root to the location root if it exists
		if (existsInMap(_serverData._locations, location))
		{
			_root = _serverData._locations[location].root;
			_root += _request._startLine.path.substr(location.length());
			return true;
		}
		_root = _serverData._root[0];
		if (_request._startLine.path.length() > 1)
			_root += _request._startLine.path.substr();
		return false;
	}

	bool 		Response::hasExtension(const std::string &path) {	Log::debugFunc(__FUNCTION__);

		if (path.find('.') != std::string::npos && path.find('.') != path.size() - 1)
			return true;
		return false;
	}

	bool 		Response::isRootValid() { 	Log::debugFunc(__FUNCTION__);
		std::ifstream fd(_root.c_str());

		if (fd)
		{
			Log::log(Log::DEBUG, "Root is valid: " + _root);
			fd.close();
			return true;
		}
		Log::log(Log::DEBUG, "Root is not valid: " + _root);
		return false;
	}

	void 		Response::addIndex(bool hasLocation, std::string location) {	Log::debugFunc(__FUNCTION__);
		std::string index;

		if (hasLocation)
			index = _serverData._locations[location].index[0];
		else
			index = _serverData._index[0];

		_root += "/" + index;
	}

void	Response::formatResponse() {	Log::debugFunc(__FUNCTION__);

	std::string body;

	body = setBody();

	setResponse(setHeader(body.length()), body);
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

	std::string 	Response::setBody() {	Log::debugFunc(__FUNCTION__);

		std::ifstream fd(_root.c_str());
		std::string line;
		std::string body;

		while (std::getline(fd, line))
			body += line;

		fd.close();

		return body;
	}

	std::string 	Response::setHeader(int bodyLength) {	Log::debugFunc(__FUNCTION__);

		std::string headers;

		headers = "HTTP/1.1 " + std::to_string(_status) + " " + CodeResponse::_codeResponse[_status] + "\r\n";
		headers += "Content-Type: text/html\r\n";

		if (bodyLength > 0)
			headers += "Content-Length: " + std::to_string(bodyLength) + "\r\n";

		headers += "Connection: close\r\n";
		headers += "\r\n";

		return headers;
	}

void	Response::sendResponse() {	Log::debugFunc(__FUNCTION__);

	if (send(_request._client, _response.c_str(), _response.length(), MSG_DONTWAIT) != -1)
		Log::log(Log::DEBUG , "Response sent");
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