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

TEST_CASE("_setType() / method check"){

	Request request(-1);
	std::string token;

	token = "GGET";
	CHECK_THROWS_AS(request._setType(token), RequestException::FirstLine::InvalidMethod);

	token = "gET";
	CHECK_THROWS_AS(request._setType(token), RequestException::FirstLine::InvalidMethod);

	token = "GET";
	CHECK_NOTHROW(request._setType(token));
}

TEST_CASE("_setVersion / version check"){
	Request request(-1);
	std::string token;

	token = "HTTP/1.0";
	CHECK_THROWS_AS(request._setVersion(token), RequestException::FirstLine::InvalidVersion);
	token = "HTTP/2.0";
	CHECK_THROWS_AS(request._setVersion(token), RequestException::FirstLine::InvalidVersion);
	token = "HTTP/1.1";
	CHECK_NOTHROW(request._setVersion(token));

}

TEST_CASE("_set<functions> / amount of arguments"){

	Request				request(-1);
	std::string 		token;

	SUBCASE("No arguments") {
		char buffer[] = "";
		std::stringstream ss(buffer);

		ss >> token;
		CHECK_THROWS_AS(request._setType(token), RequestException::FirstLine::InvalidLine);

		ss >> token;
		CHECK_THROWS_AS(request._setPath(token), RequestException::FirstLine::InvalidLine);

		ss >> token;
		CHECK_THROWS_AS(request._setVersion(token), RequestException::FirstLine::InvalidLine);
	}

	SUBCASE("One argument") {
		char buffer[] = "GET";
		std::stringstream ss(buffer);

		ss >> token;
		CHECK_NOTHROW(request._setType(token));

		ss >> token;
		std::cout << "token: " << token << std::endl;
		CHECK_THROWS_AS(request._setPath(token), RequestException::FirstLine::InvalidLine);

		ss >> token;
		CHECK_THROWS_AS(request._setVersion(token), RequestException::FirstLine::InvalidLine);
	}

	SUBCASE("Two arguments") {
		char buffer[] = "GET /";
		std::stringstream ss(buffer);

		ss >> token;
		CHECK_NOTHROW(request._setType(token));

		ss >> token;
		CHECK_NOTHROW(request._setPath(token));

		ss >> token;
		CHECK_THROWS_AS(request._setVersion(token), RequestException::FirstLine::InvalidLine);
	}

	SUBCASE("Three arguments") {
		char buffer[] = "GET / HTTP/1.1";
		std::stringstream ss(buffer);

		ss >> token;
		CHECK_NOTHROW(request._setType(token));

		ss >> token;
		CHECK_NOTHROW(request._setPath(token));

		ss >> token;
		CHECK_NOTHROW(request._setVersion(token));
	}
}

TEST_CASE("_parseStartLine / amount of arguments"){
	int client;
	remove("test/resources/test_data_file");
	client = creat("test/resources/test_data_file", 0666);

	SUBCASE("No arguments") {
		char buffer[] = "\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/resources/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::FirstLine::InvalidLine);
		close(client);
	}

	SUBCASE("One argument") {
		char buffer[] = "GET\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/resources/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::FirstLine::InvalidLine);
		close(client);
	}

	SUBCASE("Two arguments") {
		char buffer[] = "GET /\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/resources/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::FirstLine::InvalidLine);
		close(client);
	}

	SUBCASE("Three arguments") {
		char buffer[] = "GET / HTTP/1.1\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/resources/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_NOTHROW(request._parseStartLine());
		close(client);
	}
}

TEST_CASE("clean") { remove("test/resources/test_data_file");}