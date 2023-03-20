#include "test_header.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>


TEST_CASE("Request::_readSocketData() / invalid client") {

	SUBCASE("Invalid client") {
		Request request(-1);
		CHECK_THROWS_AS(request._readSocketData(), RequestException::ReadError);
	}
}

TEST_CASE("Request::_readSocketData() / valid client") {

	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);

	SUBCASE("Size is bigger than MAX_REQUEST_SIZE") {

		char buffer[MAX_REQUEST_SIZE + 12];
		memset(buffer, 'a', MAX_REQUEST_SIZE + 1);
		write(client, buffer, MAX_REQUEST_SIZE + 1);
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		CHECK_THROWS_AS(request._readSocketData(), RequestException::MaxSize);
		close(client);
	}

	SUBCASE("Size is smaller than MAX_REQUEST_SIZE") {

		char buffer[MAX_REQUEST_SIZE];
		memset(buffer, 'a', MAX_REQUEST_SIZE);
		write(client, buffer, MAX_REQUEST_SIZE);
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		CHECK_NOTHROW(request._readSocketData());
		close(client);
	}

	SUBCASE("Buffer is equal to input") {

		char buffer[] = "GET / HTTP/1.1\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK(request._rawRequest.str() == std::string(buffer));
		close(client);
	}
}

TEST_CASE("Request::_parseStartLine() / CRLF check") {

	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);

	SUBCASE("No CRLF") {
		char buffer[] = "GET / HTTP/1.1";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::NoCRLF);
		close(client);
	}

	SUBCASE("With LF") {
		char buffer[] = "GET / HTTP/1.1\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::NoCRLF);
		close(client);
	}

	SUBCASE("With CRLF") {
		char buffer[] = "GET / HTTP/1.1\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_NOTHROW(request._parseStartLine());
		close(client);
	}

	SUBCASE("With CR") {
		char buffer[] = "GET / HTTP/1.1\r";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::NoCRLF);
		close(client);
	}
}

TEST_CASE("Request::_setType() / method check"){

	Request request(-1);
	std::string token;

	token = "GGET";
	CHECK_THROWS_AS(request._setType(token), RequestException::FirstLine::InvalidMethod);

	token = "gET";
	CHECK_THROWS_AS(request._setType(token), RequestException::FirstLine::InvalidMethod);

	token = "GET";
	CHECK_NOTHROW(request._setType(token));
}

TEST_CASE("Request::_setVersion / version check"){
	Request request(-1);
	std::string token;

	token = "HTTP/1.0";
	CHECK_THROWS_AS(request._setVersion(token), RequestException::FirstLine::InvalidVersion);
	token = "HTTP/2.0";
	CHECK_THROWS_AS(request._setVersion(token), RequestException::FirstLine::InvalidVersion);
	token = "HTTP/1.1";
	CHECK_NOTHROW(request._setVersion(token));

}

TEST_CASE("Request::_set<functions> / amount of arguments"){

	Request				request(-1);
	std::string 		token;

	SUBCASE("No arguments") {
		char buffer[] = "";
		std::stringstream ss(buffer);

		ss >> token;
		CHECK_THROWS_AS(request._setType(token), RequestException::InvalidLine);

		ss >> token;
		CHECK_THROWS_AS(request._setPath(token), RequestException::InvalidLine);

		ss >> token;
		CHECK_THROWS_AS(request._setVersion(token), RequestException::InvalidLine);
	}

	SUBCASE("One argument") {
		char buffer[] = "GET";
		std::stringstream ss(buffer);

		ss >> token;
		CHECK_NOTHROW(request._setType(token));

		ss >> token;
		CHECK_THROWS_AS(request._setPath(token), RequestException::InvalidLine);

		ss >> token;
		CHECK_THROWS_AS(request._setVersion(token), RequestException::InvalidLine);
	}

	SUBCASE("Two arguments") {
		char buffer[] = "GET /";
		std::stringstream ss(buffer);

		ss >> token;
		CHECK_NOTHROW(request._setType(token));

		ss >> token;
		CHECK_NOTHROW(request._setPath(token));

		ss >> token;
		CHECK_THROWS_AS(request._setVersion(token), RequestException::InvalidLine);
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

TEST_CASE("Request::_parseStartLine / amount of arguments"){
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);

	SUBCASE("No arguments") {
		char buffer[] = "\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::InvalidLine);
		close(client);
	}

	SUBCASE("One argument") {
		char buffer[] = "GET\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::InvalidLine);
		close(client);
	}

	SUBCASE("Two arguments") {
		char buffer[] = "GET /\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::InvalidLine);
		close(client);
	}

	SUBCASE("Three arguments") {
		char buffer[] = "GET / HTTP/1.1\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		CHECK_NOTHROW(request._parseStartLine());
		close(client);
	}
}

TEST_CASE("Request::_parseHeader"){
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);

	SUBCASE("No Headers") {
		char buffer[] = "GET / HTTP/1.1\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY, 0666);

		Request request(client);
		request._readSocketData();
		request._parseStartLine();
		CHECK_NOTHROW(request._parseHeaders());
		close(client);
	}

	SUBCASE("One Header") {
		char buffer[] = "GET / HTTP/1.1\r\nHost:localhost\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		request._parseStartLine();
		CHECK_NOTHROW(request._parseHeaders());
		CHECK_MESSAGE(request._headers.size() == 1, "One header should be parsed");
		CHECK_MESSAGE(request._headers.count("HOST") == 1, "Host header should be converted to uppercase");
		CHECK_MESSAGE(request._headers["HOST"] == "localhost", "Host header value should be 'localhost'");
		close(client);
	}

	SUBCASE("Duplicate Header") {
		char buffer[] = "GET / HTTP/1.1\r\nHost:localhost\r\nHost:localhost\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		request._parseStartLine();
		CHECK_THROWS_AS(request._parseHeaders(), RequestException::Header::DuplicateKey);
		close(client);
	}

	SUBCASE("Space in key before") {
		char buffer[] = "GET / HTTP/1.1\r\n Host :localhost\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		request._parseStartLine();
		CHECK_THROWS_AS(request._parseHeaders(), RequestException::Header::InvalidKey);
		close(client);
	}

	SUBCASE("Space in key after") {
		char buffer[] = "GET / HTTP/1.1\r\nHost :localhost\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		request._parseStartLine();
		CHECK_THROWS_AS(request._parseHeaders(), RequestException::Header::InvalidKey);
		close(client);
	}

	SUBCASE("Valid no throw") {
		char buffer[] = "GET / HTTP/1.1\r\nHost:localhost\r\n";
		write(client, buffer, strlen(buffer));
		close(client);
		client = open("test/test_data_file", O_RDONLY);

		Request request(client);
		request._readSocketData();
		request._parseStartLine();
		CHECK_NOTHROW(request._parseHeaders());
		close(client);
	}

}

TEST_CASE("Request::clean") { remove("test/test_data_file");}