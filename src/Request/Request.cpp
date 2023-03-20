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
		_parseHeaders();
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
	} catch (RequestException::FirstLine::InvalidLine &e) {
		//TODO: send[400] error to client
	}
}


/**
 * @brief	Get the raw resources from the client.
 * 			Read the client socket and store the resources in _rawRequest.
 *
 */
void	Request::_readSocketData() {
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
void	Request::_parseStartLine() {

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

void	Request::_setType(std::string &type) {
	if (type.empty())
		throw RequestException::FirstLine::InvalidLine();
	else if (type == "GET")
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
	type.clear();
}

void	Request::_setPath(std::string &path) {
	if (path.empty())
		throw RequestException::FirstLine::InvalidLine();
	_startLine.path = path;
	path.clear();
}

void	Request::_setVersion(std::string &version) {
	if (version.empty())
		throw RequestException::FirstLine::InvalidLine();
	_startLine.version = version;
	if (_startLine.version.compare(HTTP_VERSION))
		throw RequestException::FirstLine::InvalidVersion();
	version.clear();
}


/**
 * @brief	Parse the headers of the request.
 * 			Store the headers in _headers.
 *
 * @source	https://www.rfc-editor.org/rfc/rfc9110#section-5.5
 * @note	header-field = field-name ":" OWS field-value OWS
 * @note	case-insensitive
 *
 */
void	Request::_parseHeaders() {
	std::string line;
	std::string key;
	std::string value;
	int i;

	while (std::getline(_rawRequest, line)) {
		if ((i = line.find(CR)) == std::string::npos || _rawRequest.eof())
			throw RequestException::FirstLine::NoCRLF();
		line.erase(i, 1);

		if (line.empty())
			break;

		i = line.find(':');
		key = line.substr(0, i);
		std::transform(key.begin(), key.end(), key.begin(), ::toupper);
		value = line.substr(i + 1, line.size() - i - 1);
		_headers[key] = value;
		key.clear();
		value.clear();
	}
}

Request::~Request() {}