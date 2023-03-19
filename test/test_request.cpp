#include "test_header.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>


TEST_CASE("_readSocketData() / invalid client") {

	SUBCASE("Invalid client") {
		Request request(-1);
		CHECK_THROWS_AS(request._readSocketData(), RequestException::ReadError);
	}
}

TEST_CASE("_readSocketData() / valid client") {

	int client;
	remove("test/resources/test_data_file");
	client = creat("test/resources/test_data_file", 0666);

	SUBCASE("Size is bigger than MAX_REQUEST_SIZE") {

		char buffer[MAX_REQUEST_SIZE + 12];
		memset(buffer, 'a', MAX_REQUEST_SIZE + 1);
		write(client, buffer, MAX_REQUEST_SIZE + 1);
		close(client);
		client = open("test/resources/test_data_file", O_RDONLY);

		Request request(client);
		CHECK_THROWS_AS(request._readSocketData(), RequestException::MaxSize);
		close(client);
	}

	SUBCASE("Size is smaller than MAX_REQUEST_SIZE") {

		char buffer[MAX_REQUEST_SIZE];
		memset(buffer, 'a', MAX_REQUEST_SIZE);
		write(client, buffer, MAX_REQUEST_SIZE);
		close(client);
		client = open("test/resources/test_data_file", O_RDONLY);

		Request request(client);
		CHECK_NOTHROW(request._readSocketData());
		close(client);
	}

	SUBCASE("Buffer is equal to input") {

		char buffer[] = "GET / HTTP/1.1\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/resources/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK(request._rawRequest.str() == std::string(buffer));
		close(client);
	}
}

TEST_CASE("_parseStartLine() / CRLF check") {

	int client;
	remove("test/resources/test_data_file");
	client = creat("test/resources/test_data_file", 0666);

	SUBCASE("No CRLF") {
		char buffer[] = "GET / HTTP/1.1";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/resources/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::FirstLine::NoCRLF);
		close(client);
	}

	SUBCASE("With LF") {
		char buffer[] = "GET / HTTP/1.1\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/resources/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::FirstLine::NoCRLF);
		close(client);
	}

	SUBCASE("With CRLF") {
		char buffer[] = "GET / HTTP/1.1\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/resources/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_NOTHROW(request._parseStartLine());
		close(client);
	}

	SUBCASE("With CR") {
		char buffer[] = "GET / HTTP/1.1\r";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/resources/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::FirstLine::NoCRLF);
		close(client);
	}
}

TEST_CASE("clean") { remove("test/resources/test_data_file");}