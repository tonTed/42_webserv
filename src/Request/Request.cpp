#include <unordered_map>
#include "Request.hpp"
#include "unistd.h"

#include "../Response/Response.hpp"
#include "../PathResolver/PathResolver.hpp"
#include "../CGI/Cgi.hpp"
#include "../Utils/FT.hpp"
#include "../FileManager/FileManager.hpp"

bool	isAllowedMethod(const eRequestType method) {
	return (method == GET || method == POST || method == DELETE);
}

void	Request::_setQueryString(std::string &path) {	Log::debugFunc(__FUNCTION__);
	size_t i = path.find('?');
	if (i != std::string::npos) {
		_startLine.queryString = path.substr(i + 1);
		path.erase(i);
	}
}

void	Request::_setPathInfo(std::string &path) {	Log::debugFunc(__FUNCTION__);

	size_t i = path.find(".py");
	if (i != std::string::npos) {
		size_t j = path.find('/', i);
		if (j != std::string::npos)
		{
			_startLine.pathInfo = path.substr(j + 1);
			path.erase(j);
		}
	}
}

// TODO set a path or throw, .., ., //, etc
std::string Request::getPath(const std::string &uri) {

	std::string path = uri;

	_setQueryString(path);
	_setPathInfo(path);

	return path;
}

Request::Request() : _client(-1), _serverId(-1), _status(200), _isCGI(false){ _cgiFd[PIPE_READ] = -1; _cgiFd[PIPE_WRITE] = -1; }

Request::~Request() {}

void	Request::_readSocketData() {
	Log::debugFunc(__FUNCTION__);

	char *buffer = new char[MAX_REQUEST_SIZE + 1];

	ssize_t	ret;

	//TODO: sleep for 1ms to wait for the client to send the data ???
	usleep(1000);

	ret = read(_client, buffer, MAX_REQUEST_SIZE + 1);
	Log::log(Log::DEBUG, "Char read: " + std::to_string(ret));

	if (ret == -1)
	{
		delete[] buffer;
		buffer = nullptr;
		throw RequestException::ReadError();
	}
	if (ret > MAX_REQUEST_SIZE)
	{
		delete[] buffer;
		buffer = nullptr;
		throw RequestException::MaxSize();
	}
	buffer[ret] = '\0';

	_bufferString.append(buffer, ret);

	_rawRequest << buffer;

	delete[] buffer;
	buffer = nullptr;
}

void	Request::_parseStartLine() {

	Log::debugFunc(__FUNCTION__);
	std::cout << _rawRequest.str() << std::endl;

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

	Log::debugFunc(__FUNCTION__);

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

void	Request::_setPath(std::string &path) {

	Log::debugFunc(__FUNCTION__);

	if (path.empty())
		throw RequestException::InvalidLine();
	if (path[0] != '/')
		throw RequestException::StartLine::InvalidURI();
	_startLine.path = getPath(path);
	if (_startLine.path.empty())
		throw RequestException::StartLine::InvalidURI();
	path.clear();
}

void	Request::_setVersion(std::string &version) {

	Log::debugFunc(__FUNCTION__);

	if (version.empty())
		throw RequestException::InvalidLine();
	_startLine.version = version;
	if (_startLine.version.compare(HTTP_VERSION))
		throw RequestException::StartLine::InvalidVersion();
	version.clear();
}

void	Request::_parseHeaders() {

	Log::debugFunc(__FUNCTION__);

	std::string line;
	std::string key;
	std::string value;
	unsigned long i;

	std::getline(_rawRequest, line);

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

void	Request::_parseBody() {

	Log::debugFunc(__FUNCTION__);

	std::string line;

	// Check if the Content-Length header is present
	//TODO: Needs Content-Length to be mandatory?
	if (_headers.find("CONTENT-LENGTH") == _headers.end())
	{
		return;
		throw RequestException::Header::MissingHeader();
	}

	int contentLength;
	try { contentLength = FT::atoi(_headers["CONTENT-LENGTH"]); }
	catch (std::exception &e) { throw RequestException::Header::InvalidValue(); }

	//Skip the Headers
//	while (std::getline(_rawRequest, line) && !line.empty());

	// Read the body
	while (std::getline(_rawRequest, line)) {
		_body += line + "\n";
	}
	if (_body.size() > static_cast<unsigned long>(contentLength))
		_body.erase(contentLength, _body.size() - contentLength);

	std::cout << YELLOW << _body << RESET << std::endl;
}

void 	Request::_manageOurTrigger() {	Log::debugFunc(__FUNCTION__);


	//Upload
	if (_startLine.path == "/upload" && _headers.find("REFERER") != _headers.end()
		&& _headers["REFERER"].find("televerser.html") != std::string::npos) {
		Log::log(Log::INFO, "UPLOAD");

		FileManager fileManager(_bufferString);
		if (fileManager.saveFile())
			_status = 200;
		else
			_status = 500;

		_startLine.path = "/uploaded.html";
		PathResolver pathResolver(*this);
	}
	else if (_root.find("/cgi/") != std::string::npos)
	{
		Log::log(Log::INFO, "CGI");
		Log::log(Log::DEBUG, "Root: " + _startLine.path);

		_isCGI = true;
		CGI cgi(*this);
		cgi.executeCgi();
	}
}

void 	Request::_manageRequest() {	Log::debugFunc(__FUNCTION__);

	PathResolver pathResolver(*this);

	_manageOurTrigger();

	if (isCGI())
		return ;
	if (_status != 200)
	{
		Response response(*this);
		response.sendResponse();
		return ;
	}

	Log::log(Log::INFO, "GET or POST");

	Response response(*this);
	response.sendResponse();
}

void	Request::initRequest() {
	Log::debugFunc(__FUNCTION__);

	try {
		_readSocketData();
		_parseStartLine();
		_parseHeaders();
		_parseBody();
	} catch (RequestException::ReadError &e) {
		Log::log(Log::DEBUG ,std::string(e.what()) + " client:" + std::to_string(_client));
		_status = 500;
	} catch (RequestException::MaxSize &e) {
		Log::log(Log::DEBUG ,std::string(e.what()));
		_status = 413;
	} catch (RequestException::NoCRLF &e) {
		Log::log(Log::DEBUG ,std::string(e.what()));
		_status = 400;
	} catch (RequestException::StartLine::InvalidMethod &e) {
		Log::log(Log::DEBUG ,std::string(e.what()));
		_status = 400;
	} catch (RequestException::StartLine::InvalidVersion &e) {
		Log::log(Log::DEBUG ,std::string(e.what()));
		_status = 505;
	} catch (RequestException::InvalidLine &e) {
		Log::log(Log::DEBUG ,std::string(e.what()));
		_status = 400;
	} catch (RequestException::Header::DuplicateKey &e) {
		Log::log(Log::DEBUG ,std::string(e.what()));
		_status = 400;
	} catch (RequestException::Header::InvalidKey &e) {
		Log::log(Log::DEBUG ,std::string(e.what()));
		_status = 400;
	} catch (RequestException::Header::InvalidValue &e) {
		Log::log(Log::DEBUG ,std::string(e.what()));
		_status = 400;
	} catch (RequestException::StartLine::NotAllowedMethod &e) {
		Log::log(Log::DEBUG ,std::string(e.what()));
		_status = 405;
	} catch (RequestException::StartLine::InvalidURI &e) {
		Log::log(Log::DEBUG ,std::string(e.what()));
		_status = 400;
	}  catch (RequestException::Header::MissingHeader &e) {
		Log::log(Log::DEBUG ,std::string(e.what()));
		_status = 400;
	}

	_manageRequest();
}

void 	Request::resetRequest() {
	_rawRequest.str("");
	_rawRequest.clear();
	_startLine.type = UNKNOWN;
	_startLine.path = "";
	_startLine.version = "";
	_startLine.queryString = "";
	_startLine.pathInfo = "";
	_headers.clear();
	_body.clear();
	_serverId = -1;
	_client = -1;
	_cgiFd[0] = -1;
	_cgiFd[1] = -1;
	_isCGI = false;
	_root = "";
	_status = 200;
	_bufferString.clear();
}

void	Request::setClient(int client) {

	Log::debugFunc(__FUNCTION__);

	_client = client;
}

void	Request::setServerId(int serverId) {

	Log::debugFunc(__FUNCTION__);

	_serverId = serverId;
}

void	Request::setCGIFd(int cgiFd[2]) {

	Log::debugFunc(__FUNCTION__);

	_cgiFd[PIPE_READ] = cgiFd[PIPE_READ];
	_cgiFd[PIPE_WRITE] = cgiFd[PIPE_WRITE];
}

int		Request::getClient() const{

	Log::debugFunc(__FUNCTION__);

	return _client;
}

bool	Request::isCGI() const {

	Log::debugFunc(__FUNCTION__);

	return _isCGI;
}