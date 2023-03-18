#include "Request.hpp"
#include "unistd.h"

/**
 * @brief Construct a new Request object
 * @details Get the raw resources from the client
 *
 * @param client The client that sent the resources
 */
Request::Request(const int client) : _client(client) {

}

void	Request::_init() {
	try {
		try {
			_readSocketData();
		} catch (std::runtime_error &e) {
			throw e;
			//TODO: send error to client
		}
	} catch (...) {}
}


/**
 * @brief Get the raw resources from the client
 * @details Read the client socket and store the resources in _rawRequest
 *
 * @question: can we have an error reading the socket?
 */
void Request::_readSocketData() {
	char	buffer[MAX_REQUEST_SIZE + 1];

	if (read(_client, buffer, MAX_REQUEST_SIZE + 1) <= MAX_REQUEST_SIZE)
		_rawRequest << buffer;
	else
		throw RequestException::MaxSize();
}

Request::~Request() {}