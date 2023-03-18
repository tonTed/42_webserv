#include "test_header.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>


TEST_CASE("_readSocketData()") {

	int client;

	{
		Request request(-1);
		CHECK_THROWS_AS(request._readSocketData(), RequestException::ReadError);
	}

	client = open("test/resources/request_err_size.http", O_RDONLY);
	CHECK_MESSAGE(client != -1, "Failed to open test/resources/request_err_size");
	if (client != -1)
	{
		Request request(client);
		CHECK_THROWS_AS(request._readSocketData(), RequestException::MaxSize);
	}
	close(client);

	client = open("test/resources/request_validSize_errFirstLine.http", O_RDONLY);
	CHECK_MESSAGE(client != -1, "Failed to open test/resources/request_validSize_errFirstLine");
	if (client != -1)
	{
		Request request(client);
		CHECK_NOTHROW(request._readSocketData());
	}
	close(client);
}
