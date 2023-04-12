#include "Response.hpp"

Response::Response(const Request &request, int status) : _request(request), _status(status){
	_config = ConfigServer::getInstance();

	if (status != 200) {
		//send error
	}
	else {
		//send file
	}
}


/**
 * @brief   If the location exists in the config file, set the root to the location root
 * 			Else, set the root to the server root using the first root in the vector
 *
 */
std::string Response::getLocation() {
	std::string	location;

	// check if path has a slash
	if (_request._startLine.path.find_first_of('/', 1) == std::string::npos)
		location = _request._startLine.path;
	else
		location = _request._startLine.path.substr(0, _request._startLine.path.find_first_of('/', 1));

	return location;
}

bool 		Response::setRoot(const std::string &location) {

	// set the root to the location root if it exists
	if (_config->_serversData[_request._serverId]._locations.find(location) != _config->_serversData[_request._serverId]._locations.end())\
	{
		_root = _config->_serversData[_request._serverId]._locations[location].root;
		_root += _request._startLine.path.substr(location.length());
		return true;
	}
	_root = _config->_serversData[_request._serverId]._root[0];
	if (_request._startLine.path.length() > 1)
		_root += _request._startLine.path.substr();
	return false;
}

bool 		Response::hasExtension(const std::string &path) {
	if (path.find('.') != std::string::npos && path.find('.') != path.size() - 1)
		return true;
	return false;
}

bool 		Response::isRootValid() {
	std::ifstream fd(_root.c_str());

	if (fd)
	{
		fd.close();
		return true;
	}
	return false;
}

void 		Response::addIndex(bool hasLocation, std::string location) {
	std::string index;

	if (hasLocation)
		index = _config->_serversData[_request._serverId]._locations[location].index[0];
	else
		index = _config->_serversData[_request._serverId]._index[0];

	_root += "/" + index;
}

void	Response::resolvePath() {

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
				_status = 404;
			else
				_status = 500;
		}
	}
	if (_status == 200)
		formatResponse();
	else
	{
		if(_config->_serversData[_request._serverId]._errorPages.find(_status) != _config->_serversData[_request._serverId]._errorPages.end())
		{
			_root = _config->_serversData[_request._serverId]._errorPages[_status];
			formatResponse();
		}
		else
		{
			std::string body;

			body = DefaultHTML::getHtml(_status);
			_response = setHeader(body.length());
			_response += body;
		}
	}

}

std::string 	Response::setBody() {
	std::ifstream fd(_root.c_str());
	std::string line;
	std::string body;

	while (std::getline(fd, line))
		body += line;

	fd.close();

	return body;
}

std::string 	Response::setHeader(int bodyLength) {
	std::string headers;

	headers = "HTTP/1.1 " + std::to_string(_status) + " " + CodeResponse::_codeResponse[_status] + "\r\n";
	headers += "Content-Type: text/html\r\n";

	if (bodyLength > 0)
		headers += "Content-Length: " + std::to_string(bodyLength) + "\r\n";

	headers += "Connection: close\r\n";
	headers += "\r\n";

	return headers;
}

void	Response::formatResponse() {

	std::string body;

	body = setBody();

	_response = setHeader(body.length());
	_response += body;
}


