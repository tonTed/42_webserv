#include "Request.hpp"
#include "unistd.h"

/**
 * @brief Construct a new Request object
 * @details Get the raw resources from the client
 *
 * @param client The client that sent the resources
 */
Request::Request(const int client) : _client(client) {}

void	Request::_init() {
	try {
		_readSocketData();
		_parseStartLine();
	} catch (RequestException::ReadError &e) {
		//TODO: send[500] error to client
	} catch (RequestException::MaxSize &e) {
		//TODO: send[494] error to client
	} catch (RequestException::FirstLine::NoCRLF &e) {
		//TODO: send[400] error to client
	} catch (RequestException::FirstLine::InvalidMethod &e) {
		//TODO: send[400] error to client
	} catch (RequestException::FirstLine::InvalidVersion &e) {
		//TODO: send[505] error to client
	}
}


/**
 * @brief	Get the raw resources from the client.
 * 			Read the client socket and store the resources in _rawRequest.
 *
 */
void Request::_readSocketData() {
	char	buffer[MAX_REQUEST_SIZE + 1];
	int 	ret;

	ret = read(_client, buffer, MAX_REQUEST_SIZE + 1);
	if (ret == -1)
		throw RequestException::ReadError();
	if (ret > MAX_REQUEST_SIZE)
		throw RequestException::MaxSize();
	buffer[ret] = '\0';

	_rawRequest << buffer;
}

/**
 * @brief	Parse the first line of the request.
 * 			Store the type, path and version in _startLine.
 *
 * @source	https://www.rfc-editor.org/rfc/rfc9112.html#section-3
 * @note	request-line = method SP request-target SP HTTP-version CRLF
 *
 */
void Request::_parseStartLine() {

	// Check if the line ends with CRLF
	std::string line;
	std::getline(_rawRequest, line);
	int i;

	if ((i = line.find('\r')) == std::string::npos || _rawRequest.eof())
		throw RequestException::FirstLine::NoCRLF();
	line.erase(i, 1);

	// Split the line into 3 parts
	std::stringstream	ss(line);
	std::string			tmp;

	try {
		// Get the method
		ss >> tmp;
		_setType(tmp);

		// Get the path
		ss >> tmp;
		_setPath(tmp);

		// Get the version
		ss >> tmp;
		_setVersion(tmp);

	} catch (RequestException::FirstLine &e) {
		throw e;
	}

	// Check if there is no more data in the
	if (!ss.eof())
		throw RequestException::FirstLine::NoCRLF();

}

void Request::_setType(const std::string &type) {
	if (type == "GET")
		_startLine.type = GET;
	else if (type == "POST")
		_startLine.type = POST;
	else if (type == "PUT")
		_startLine.type = PUT;
	else if (type == "DELETE")
		_startLine.type = DELETE;
	else if (type == "HEAD")
		_startLine.type = HEAD;
	else if (type == "OPTIONS")
		_startLine.type = OPTIONS;
	else if (type == "CONNECT")
		_startLine.type = CONNECT;
	else if (type == "TRACE")
		_startLine.type = TRACE;
	else if (type == "PATCH")
		_startLine.type = PATCH;
	else
		throw RequestException::FirstLine::InvalidMethod();
}

void Request::_setPath(const std::string &path) {
	_startLine.path = path;
}

void Request::_setVersion(const std::string &version) {
	_startLine.version = version;
	if (_startLine.version.compare(HTTP_VERSION))
		throw RequestException::FirstLine::InvalidVersion();
}

Request::~Request() {}