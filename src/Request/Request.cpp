#include <unordered_map>
#include "Request.hpp"
#include "unistd.h"

bool	isAllowedMethod(const eRequestType method) {
	return (method == GET || method == POST || method == DELETE);
}

// TODO set a path or throw, .., ., //, etc
std::string getPath(const std::string &uri, const eRequestType method) {
	(void)uri;
	(void)method;
	if (uri == "/" || uri == "/index.html")
		return "./data/www/index.html";
	return "";
}

Request::Request() : _client(-1), _serverId(-1) {}

void Request::resetRequest() {
	_rawRequest.str("");
	_rawRequest.clear();
	_startLine.type = UNKNOWN;
	_startLine.path = "";
	_startLine.version = "";
	_headers.clear();
	_body.clear();
	_serverId = -1;
	_client = -1;
}

Request::Request(const int client, const int serverId) : _client(client), _serverId(serverId) {}

/**
 * @brief	Initialise the request.
 * 			- Read the client socket and store the resources in _rawRequest.
 * 			- Parse the first line of the request.
 * 			- Parse the headers of the request.
 * 			- Parse the body of the request.
 *
 * 	@note if an error occurs, the server will send an error to the client.
 *
 */
void	Request::_initRequest() {
	try {
		_readSocketData();
		_parseStartLine();
		_parseHeaders();
		_parseBody();
	} catch (RequestException::ReadError &e) {
		//TODO: send[500] error to client
	} catch (RequestException::MaxSize &e) {
		//TODO: send[494] error to client
	} catch (RequestException::NoCRLF &e) {
		//TODO: send[400] error to client
	} catch (RequestException::StartLine::InvalidMethod &e) {
		//TODO: send[400] error to client
	} catch (RequestException::StartLine::InvalidVersion &e) {
		//TODO: send[505] error to client
	} catch (RequestException::InvalidLine &e) {
		//TODO: send[400] error to client
	} catch (RequestException::Header::DuplicateKey &e) {
		//TODO: send[400] error to client
	} catch (RequestException::Header::InvalidKey &e) {
		//TODO: send[400] error to client
	} catch (RequestException::Header::InvalidValue &e) {
		//TODO: send[400] error to client
	} catch (RequestException::StartLine::NotAllowedMethod &e) {
		//TODO: send[405] error to client
	} catch (RequestException::StartLine::InvalidURI &e) {
		//TODO: send[404] error to client
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
	unsigned long i;

	if ((i = line.find('\r')) == static_cast<unsigned long>(std::string::npos) || _rawRequest.eof())
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

	} catch (RequestException::StartLine &e) {
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
	else if (type == "DELETE")
		_startLine.type = DELETE;
	else if (type == "PUT")
		_startLine.type = PUT;
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
		throw RequestException::StartLine::InvalidMethod();
	if (!isAllowedMethod(_startLine.type))
		throw RequestException::StartLine::NotAllowedMethod();
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
	if (path[0] != '/')
		throw RequestException::StartLine::InvalidURI();
	_startLine.path = getPath(path, _startLine.type);
	if (_startLine.path.empty())
		throw RequestException::StartLine::InvalidURI();
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
		throw RequestException::StartLine::InvalidVersion();
	version.clear();
}

void print_map(std::map<std::string, std::string> &m) {
	std::map<std::string, std::string>::iterator it = m.begin();
	while (it != m.end()) {
		std::cout << it->first << " : " << it->second << std::endl;
		it++;
	}
}

/**
 * @brief	Parse the headers of the request.
 * 			Store the headers in _headers.
 *
 * @source	https://www.rfc-editor.org/rfc/rfc9110#section-5.5
 * @source	https://www.rfc-editor.org/rfc/rfc7230#section-3.2
 *
 * @note	header-field	= field-name ":" OWS field-value OWS
 *     		field-name     = token
 *     		field-value    = *( field-content / obs-fold )
 *     		field-content  = field-vchar [ 1*( SP / HTAB ) field-vchar ]
 *     		field-vchar    = VCHAR / obs-text
 *     		obs-fold       = CRLF 1*( SP / HTAB )
 *     						; obsolete line folding
 *
 *     		OWS     : *( SP / HTAB ) ; optional whitespace
 *			RWS     :1*( SP / HTAB ) ; required whitespace
 *			BWS		: OWS ; bad whitespace
 *     		OWS		: Optional whitespace
 *     		RWS		: Required whitespace
 *     		BWS		: Bad whitespace
 *     		SP		: Space
 *     		HTAB	: Horizontal tab
 *     		VCHAR	: Visible characters
 *
 * @note	case-insensitive
 *
 * @throw RequestException::NoCRLF if the line doesn't end with CRLF
 *
 */
void	Request::_parseHeaders() {
	std::string line;
	std::string key;
	std::string value;
	unsigned long i;

	while (std::getline(_rawRequest, line)) {

		// Check if the line ends with CRLF
		if ((i = line.find(CR)) == static_cast<unsigned long>(std::string::npos) || _rawRequest.eof())
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

		// Check if the key as white spaces
		if (std::any_of(key.begin(), key.end(), ::isspace))
			throw RequestException::Header::InvalidKey();

		// Uppercase the key
		std::transform(key.begin(), key.end(), key.begin(), ::toupper);

		// Check if the key is already in the map
		if (_headers.find(key) != _headers.end())
			throw RequestException::Header::DuplicateKey();

		// Get the value
		value = line.substr(i + 1, line.size() - i - 1);

		//remove OWS before and after the value if any
		value.erase(0, value.find_first_not_of(OWS));
		value.erase(value.find_last_not_of(OWS) + 1);
		if (value.empty())
			throw RequestException::Header::InvalidValue();

		// Add the key and value to the map
		_headers.insert(std::pair<std::string, std::string>(key, value));

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

	int contentLength;
	try { contentLength = std::stoi(_headers["CONTENT-LENGTH"]); }
	catch (std::exception &e) { throw RequestException::Header::InvalidValue(); }

	// Read the body
	while (std::getline(_rawRequest, line)) {
		_body += line;
	}
	if (_body.size() > static_cast<unsigned long>(contentLength))
		_body.erase(contentLength, _body.size() - contentLength);
 }

Request::~Request() {}