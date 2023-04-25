/* //
// Created by Teddy Blanco on 4/4/23.
//

#include "test_header.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int writeCloseOpen(int client, const char *buffer);

TEST_CASE("Response::hasExtension")
{
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);
	char buffer[MAX_REQUEST_SIZE];
	Request request(writeCloseOpen(client, buffer), 0);
	Response response(request, 200);

	std::string path = "test/test.txt";
	REQUIRE(response.hasExtension(path) == true);

	path = "test/test";
	REQUIRE(response.hasExtension(path) == false);

	path = "test/test.";
	REQUIRE(response.hasExtension(path) == false);

	path = "test/";
	REQUIRE(response.hasExtension(path) == false);
}

TEST_CASE("Response::getLocation")
{
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);
	char buffer[MAX_REQUEST_SIZE];
	Request request(writeCloseOpen(client, buffer), 0);
	Response response(request, 200);
	ConfigServer *config = ConfigServer::getInstance();
	config->setConfigServer("test/default.conf");

	request._startLine.path = "/toto";
	std::string ret = response.getLocation();
	CHECK_MESSAGE(ret == "/toto", "should be /toto");

	request._startLine.path = "/toto/tata";
	ret = response.getLocation();
	CHECK_MESSAGE(ret == "/toto", "should be /toto");

	request._startLine.path = "/";
	ret = response.getLocation();
	CHECK_MESSAGE(ret == "/", "should be /");
}

TEST_CASE("Response::setRoot")
{
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);
	char buffer[MAX_REQUEST_SIZE];
	Request request(writeCloseOpen(client, buffer), 0);
	Response response(request, 200);
	ConfigServer *config = ConfigServer::getInstance();
	config->setConfigServer("test/default.conf");
	std::string location;

	request._startLine.path = "/tata";
	location = response.getLocation();
	CHECK_MESSAGE(response.setRoot(location) == true, "should be true");
	CHECK_MESSAGE(response._root == "test/data/www/toto", "should be test/data/www/toto");

	request._startLine.path = "/tata/dali";
	location = response.getLocation();
	CHECK_MESSAGE(response.setRoot(location) == true, "should be true");
	CHECK_MESSAGE(response._root == "test/data/www/toto/dali", "should be test/data/www/toto/dali");

	request._startLine.path = "/";
	location = response.getLocation();
	CHECK_MESSAGE(response.setRoot(location) == false, "should be false");
	CHECK_MESSAGE(response._root == "test/data/www", "should be test/data/www");

	request._startLine.path = "/dali";
	location = response.getLocation();
	CHECK_MESSAGE(response.setRoot(location) == false, "should be false");
	CHECK_MESSAGE(response._root == "test/data/www/dali", "should be test/data/www/dali");

	request._startLine.path = "/dali/index.html";
	location = response.getLocation();
	CHECK_MESSAGE(response.setRoot(location) == false, "should be false");
	CHECK_MESSAGE(response._root == "test/data/www/dali/index.html", "should be test/data/www/dali/index.html");
}

TEST_CASE("Response::isRootValid")
{
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);
	char buffer[MAX_REQUEST_SIZE];
	Request request(writeCloseOpen(client, buffer), 0);
	Response response(request, 200);
	ConfigServer *config = ConfigServer::getInstance();
	config->setConfigServer("test/default.conf");

	response._root = "test/data/www/toto/index.html";
	REQUIRE(response.isRootValid() == true);

	response._root = "test/data/www/tata/test.txt";
	REQUIRE(response.isRootValid() == false);
}

TEST_CASE("Response::addIndex")
{
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);
	char buffer[MAX_REQUEST_SIZE];
	Request request(writeCloseOpen(client, buffer), 0);
	Response response(request, 200);
	ConfigServer *config = ConfigServer::getInstance();
	config->setConfigServer("test/default.conf");

	//TODO remove this when config is done
	config->_serversData[0]._index.push_back("index.html");

	response._root = "test/data/www/toto";
	response.addIndex(false, response.getLocation());
	REQUIRE_MESSAGE(response._root == "test/data/www/toto/index.html", "should be test/data/www/toto/index.html");

	response._root = "test/data/www/toto";
	response.addIndex(true, "/tata");
	REQUIRE_MESSAGE(response._root == "test/data/www/toto/index.html", "should be test/data/www/toto/index.html");

}

TEST_CASE("Response::setHeader")
{
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);
	char buffer[MAX_REQUEST_SIZE];
	Request request(writeCloseOpen(client, buffer), 0);
	Response response(request, 200);
	ConfigServer *config = ConfigServer::getInstance();
	config->setConfigServer("test/default.conf");

	std::string expected;

	expected = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 10\r\nConnection: close\r\n\r\n";
	response._status = 200;
	CHECK(response.setHeader(10) == expected );

	expected = "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\nContent-Length: 50\r\nConnection: close\r\n\r\n";
	response._status = 500;
	CHECK(response.setHeader(50) == expected );
}

TEST_CASE("Response::setBody")
{
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);
	char buffer[MAX_REQUEST_SIZE];
	Request request(writeCloseOpen(client, buffer), 0);
	Response response(request, 200);
	ConfigServer *config = ConfigServer::getInstance();
	config->setConfigServer("test/default.conf");

	std::string expected;

	expected = "<!DOCTYPE html><html lang=\"en\"><head> <meta charset=\"UTF-8\"> <title>42 webserv</title></head><body> <h1>42 webserv</h1> <p>42 webserv is a web server written in C++.</p> <p>It is a project of the 42 cursus.</p> <p>Write by Hafid, Martin and Teddy</p></body></html>";
	response._root = "test/data/www/index.html";
	REQUIRE(response.setBody() == expected);
}



TEST_CASE("Request::clean") { remove("test/test_data_file");} */