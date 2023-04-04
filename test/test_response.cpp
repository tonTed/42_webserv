//
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
	Request request(writeCloseOpen(client, buffer), 0, 0);
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

TEST_CASE("Response::removeFile")
{
	int client;
	remove("test/test_data_file");
	client = creat("test/test_data_file", 0666);
	char buffer[MAX_REQUEST_SIZE];
	Request request(writeCloseOpen(client, buffer), 0, 0);
	Response response(request, 200);

	std::string path = "/test/test.txt";
	std::string file = "test.txt";
	response.removeFile(path, file);
	REQUIRE(path == "/test/");
	REQUIRE(file == "test.txt");

	path = "/test.txt";
	file = "test.txt";
	response.removeFile(path, file);
	REQUIRE(path == "/");
	REQUIRE(file == "test.txt");
}