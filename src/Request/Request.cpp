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

void Request::_parseStartLine() {
	std::string line;
	std::getline(_rawRequest, line);

	if (line.find('\r') == std::string::npos || _rawRequest.eof())
		throw RequestException::FirstLine::NoCRLF();
}

Request::~Request() {}