#include "Request.hpp"
#include "unistd.h"

Request::Request(const int client) : _client(client) {}

/**
 * @brief	Initialise the request.
 * 			- Read the client socket and store the resources in _rawRequest.
 * 			- Parse the first line of the request.
 * 			- Parse the headers of the request.
 * 			- Parse the body of the request.
 *
 * 	@note if an error occurs, the server will send an error to the client.
 *
 * 	@throw RequestException::ReadError if the client socket is invalid.
 * 			RequestException::MaxSize if the request size is bigger than MAX_REQUEST_SIZE.
 *
 *
 */
void	Request::_init() {
	try {
		_readSocketData();
		_parseStartLine();
		_parseHeaders();
	} catch (RequestException::ReadError &e) {
		//TODO: send[500] error to client
	} catch (RequestException::MaxSize &e) {
		//TODO: send[494] error to client
	} catch (RequestException::NoCRLF &e) {
		//TODO: send[400] error to client
	} catch (RequestException::FirstLine::InvalidMethod &e) {
		//TODO: send[400] error to client
	} catch (RequestException::FirstLine::InvalidVersion &e) {
		//TODO: send[505] error to client
	} catch (RequestException::InvalidLine &e) {
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
		throw RequestException::NoCRLF();
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
		throw RequestException::NoCRLF();

}

void	Request::_setType(std::string &type) {
	if (type.empty())
		throw RequestException::InvalidLine();
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


/**
 * @brief	Set the path of the request.
 * 			Store the path in _startLine.
 *
 * @param path The path of the request
 *
 * @throw RequestException::InvalidLine if the path is empty
 *
 * @todo	Check here if the path is valid?
 */
void	Request::_setPath(std::string &path) {
	if (path.empty())
		throw RequestException::InvalidLine();
	_startLine.path = path;
	path.clear();
}


/**
 * @brief	Set the version of the request.
 * 			Store the version in _startLine.
 *
 * @param version The version of the request
 *
 * @throw RequestException::InvalidLine if the version is empty
 * @throw RequestException::FirstLine::InvalidVersion if the version is not HTTP/1.1
 *
 */
void	Request::_setVersion(std::string &version) {
	if (version.empty())
		throw RequestException::InvalidLine();
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
 * @throw RequestException::NoCRLF if the line doesn't end with CRLF
 *
 * @todo	Spaces in the header value are not allowed?
 * @todo	What if the header value is empty?
 * @todo	length of the header value?
 * @todo	key or value can't be empty?
 *
 */
void	Request::_parseHeaders() {
	std::string line;
	std::string key;
	std::string value;
	int i;

	while (std::getline(_rawRequest, line)) {

		// Check if the line ends with CRLF
		if ((i = line.find(CR)) == std::string::npos || _rawRequest.eof())
			throw RequestException::NoCRLF();
		line.erase(i, 1);

		// Check if the line is empty
		if (line.empty())
			break;

		// Split the line into 2 parts
		i = line.find(':');

		// Check if the line as a : separator
		if (i == std::string::npos)
			throw RequestException::InvalidLine();

		// Get the key
		key = line.substr(0, i);

		// Uppercase the key
		std::transform(key.begin(), key.end(), key.begin(), ::toupper);

		// Check if the key is already in the map
		if (_headers.find(key) != _headers.end())
			throw RequestException::Header::DuplicateKey();

		// Get the value
		value = line.substr(i + 1, line.size() - i - 1);

		// Add the key and value to the map
		_headers[key] = value;

		// Clear the key and value
		key.clear();
		value.clear();
	}
}

/**
 * @brief	Parse the body of the request.
 * 			Store the body in _body.
 *
 * @source	https://www.rfc-editor.org/rfc/rfc9110#section-5.5
 * @note	Content-Length is mandatory
 * @note	Content-Type is mandatory
 * @note	Transfer-Encoding is not supported
 *
 */
 void	Request::_parseBody() {
	std::string line;

	// Check if the Content-Length header is present
	if (_headers.find("CONTENT-LENGTH") == _headers.end())
		throw RequestException::Header::MissingHeader();

	// Check if the Content-Type header is present
	if (_headers.find("CONTENT-TYPE") == _headers.end())
		throw RequestException::Header::MissingHeader();

	// Check if the Transfer-Encoding header is present
	if (_headers.find("TRANSFER-ENCODING") != _headers.end())
		throw RequestException::Header::MissingHeader();

	// Get the Content-Length
		// check if the value is a number
	if (std::for_each(_headers["CONTENT-LENGTH"].begin(), _headers["CONTENT-LENGTH"].end(), ::isdigit))
		int contentLength = std::stoi(_headers["CONTENT-LENGTH"]);
	else
		throw RequestException::Header::InvalidValue();



 }

Request::~Request() {}