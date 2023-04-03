#include "test_header.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>


int writeCloseOpen(int client, const char *buffer){
	write(client, buffer, strlen(buffer));
	close(client);
	return open("test/test_data_file", O_RDONLY);
}

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


		Request request(writeCloseOpen(client, buffer));
		CHECK_THROWS_AS(request._readSocketData(), RequestException::MaxSize);
		close(client);
	}

	SUBCASE("Size is smaller than MAX_REQUEST_SIZE") {

		char buffer[MAX_REQUEST_SIZE];
		memset(buffer, 'a', MAX_REQUEST_SIZE);
		write(client, buffer, MAX_REQUEST_SIZE);
		close(client);


		Request request(writeCloseOpen(client, buffer));
		CHECK_NOTHROW(request._readSocketData());
		close(client);
	}

	SUBCASE("Buffer is equal to input") {

		char buffer[] = "GET / HTTP/1.1\r\n";
		write(client, buffer, strlen(buffer));
		close(client);


		Request request(writeCloseOpen(client, buffer));
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

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::NoCRLF);
		close(client);
	}

	SUBCASE("With LF") {
		char buffer[] = "GET / HTTP/1.1\n";
		
		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::NoCRLF);
		close(client);
	}

	SUBCASE("With CRLF") {
		char buffer[] = "GET / HTTP/1.1\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		CHECK_NOTHROW(request._parseStartLine());
		close(client);
	}

	SUBCASE("With CR") {
		char buffer[] = "GET / HTTP/1.1\r";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::NoCRLF);
		close(client);
	}
}

TEST_CASE("Request::_setType() / method check"){

	Request request(-1);
	std::string token;

	token = "GGET";
	CHECK_THROWS_AS(request._setType(token), RequestException::StartLine::InvalidMethod);

	token = "gET";
	CHECK_THROWS_AS(request._setType(token), RequestException::StartLine::InvalidMethod);

	token = "GET";
	CHECK_NOTHROW(request._setType(token));
}

TEST_CASE("Request::_setVersion / version check"){
	Request request(-1);
	std::string token;

	token = "HTTP/1.0";
	CHECK_THROWS_AS(request._setVersion(token), RequestException::StartLine::InvalidVersion);
	token = "HTTP/2.0";
	CHECK_THROWS_AS(request._setVersion(token), RequestException::StartLine::InvalidVersion);
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
		
		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::InvalidLine);
		close(client);
	}

	SUBCASE("One argument") {
		char buffer[] = "GET\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::InvalidLine);
		close(client);
	}

	SUBCASE("Two arguments") {
		char buffer[] = "GET /\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::InvalidLine);
		close(client);
	}

	SUBCASE("Three arguments") {
		char buffer[] = "GET / HTTP/1.1\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		CHECK_NOTHROW(request._parseStartLine());
		close(client);
	}
}

TEST_CASE("Request::_parseHeader / Key check"){
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);

	SUBCASE("No Headers") {
		char buffer[] = "GET / HTTP/1.1\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		CHECK_NOTHROW(request._parseHeaders());
		close(client);
	}

	SUBCASE("One Header") {
		char buffer[] = "GET / HTTP/1.1\r\nHost:localhost\r\n";

		Request request(writeCloseOpen(client, buffer));
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

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		CHECK_THROWS_AS(request._parseHeaders(), RequestException::Header::DuplicateKey);
		close(client);
	}

	SUBCASE("Space in key before") {
		char buffer[] = "GET / HTTP/1.1\r\n Host :localhost\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		CHECK_THROWS_AS(request._parseHeaders(), RequestException::Header::InvalidKey);
		close(client);
	}

	SUBCASE("Space in key after") {
		char buffer[] = "GET / HTTP/1.1\r\nHost :localhost\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		CHECK_THROWS_AS(request._parseHeaders(), RequestException::Header::InvalidKey);
		close(client);
	}

	SUBCASE("Valid no throw") {
		char buffer[] = "GET / HTTP/1.1\r\nHost:localhost\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		CHECK_NOTHROW(request._parseHeaders());
		close(client);
	}
}

TEST_CASE("Request::_parseHeader / value check") {
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);

	SUBCASE("Remove OWS before"){
		char buffer[] = "GET / HTTP/1.1\r\nHost: localhost\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		request._parseHeaders();
		CHECK_MESSAGE(request._headers["HOST"] == "localhost", "Host header value should be 'localhost'");
	}

	SUBCASE("Remove OWS after"){
		char buffer[] = "GET / HTTP/1.1\r\nHost:localhost\t\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		request._parseHeaders();
		CHECK_MESSAGE(request._headers["HOST"] == "localhost", "Host header value should be 'localhost'");
	}

	SUBCASE("Remove OWS before & after"){
		char buffer[] = "GET / HTTP/1.1\r\nHost: localhost\t\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		request._parseHeaders();
		CHECK_MESSAGE(request._headers["HOST"] == "localhost", "Host header value should be 'localhost'");
	}

	SUBCASE("Without OWS"){
		char buffer[] = "GET / HTTP/1.1\r\nHost:localhost\t\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		request._parseHeaders();
		CHECK_MESSAGE(request._headers["HOST"] == "localhost", "Host header value should be 'localhost'");
	}

	SUBCASE("Empty value"){
		char buffer[] = "GET / HTTP/1.1\r\nHost:\r\n";


		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		CHECK_THROWS_AS(request._parseHeaders(), RequestException::Header::InvalidValue);
	}

	SUBCASE("Just one OWS"){
		char buffer[] = "GET / HTTP/1.1\r\nHost: \r\n";
		write(client, buffer, strlen(buffer));
		close(client);

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		CHECK_THROWS_AS(request._parseHeaders(), RequestException::Header::InvalidValue);
	}

	SUBCASE("Just two OWS"){
		char buffer[] = "GET / HTTP/1.1\r\nHost:\t \r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		CHECK_THROWS_AS(request._parseHeaders(), RequestException::Header::InvalidValue);
	}

	SUBCASE("Just tree OWS"){
		char buffer[] = "GET / HTTP/1.1\r\nHost:\t \t\t\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		CHECK_THROWS_AS(request._parseHeaders(), RequestException::Header::InvalidValue);
	}

	SUBCASE("Just a letter between OWS"){
		char buffer[] = "GET / HTTP/1.1\r\nHost:\t a\t\t\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		CHECK_NOTHROW_MESSAGE(request._parseHeaders(), "Host header value should be 'a'");
		CHECK_MESSAGE(request._headers["HOST"] == "a", "Host header value should be 'a'");
	}
}

TEST_CASE("Request::_parseStartLine / Method not allowed"){
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);

	SUBCASE("Method not allowed HEAD"){
		char buffer[] = "HEAD / HTTP/1.1\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::StartLine::NotAllowedMethod);
	}

	SUBCASE("Method not allowed PATCH"){
		char buffer[] = "PATCH / HTTP/1.1\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		CHECK_THROWS_AS(request._parseStartLine(), RequestException::StartLine::NotAllowedMethod);
	}

	SUBCASE("Method allowed POST"){
		char buffer[] = "POST / HTTP/1.1\r\n";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		CHECK_NOTHROW_MESSAGE(request._parseStartLine(), "Method POST should be allowed");
	}
}

TEST_CASE("Request::_parseBody / Content-Length"){
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);

	SUBCASE("Content-Length is valid"){
		char buffer[] = "GET / HTTP/1.1\r\nCONTENT-LENGTH: 5\r\n\r\nHello";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		request._parseHeaders();
		CHECK_NOTHROW_MESSAGE(request._parseBody(), "Bode is valid");
	}

	SUBCASE("Content-Length is not a number"){
		char buffer[] = "GET / HTTP/1.1\r\nCONTENT-LENGTH: t\r\n\r\nHello";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		request._parseHeaders();
		CHECK_THROWS_AS(request._parseBody(), RequestException::Header::InvalidValue);
	}
}

TEST_CASE("Request::_parseBody / Value check"){
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);

	SUBCASE("Body should be 'Hello', with matching Content-Length"){
		char buffer[] = "GET / HTTP/1.1\r\nCONTENT-LENGTH: 5\r\n\r\nHello";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		request._parseHeaders();
		request._parseBody();
		CHECK_MESSAGE(request._body == "Hello", "Body should be 'Hello'");
	}

	SUBCASE("Body should be 'Hello', with Bigger Content-Length"){
		char buffer[] = "GET / HTTP/1.1\r\nCONTENT-LENGTH: 6\r\n\r\nHello";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		request._parseHeaders();
		request._parseBody();
		CHECK_MESSAGE(request._body == "Hello", "Body should be 'Hello'");
		CHECK_MESSAGE(request._body.size() == 5, "Body size should be 5");
	}

	SUBCASE("Body should be 'Hell', with smaller Content-Length"){
		char buffer[] = "GET / HTTP/1.1\r\nCONTENT-LENGTH: 4\r\n\r\nHello World";

		Request request(writeCloseOpen(client, buffer));
		request._readSocketData();
		request._parseStartLine();
		request._parseHeaders();
		request._parseBody();
		CHECK_MESSAGE(request._body == "Hell", "Body should be 'Hell'");
		CHECK_MESSAGE(request._body.size() == 4, "Body size should be 4");
	}
}

TEST_CASE("Request::clean") { remove("test/test_data_file");}