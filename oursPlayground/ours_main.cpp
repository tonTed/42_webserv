//
// Created by Teddy BLANCO on 2023-03-16.
//

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../includes/webserv.hpp"
#include <string.h>
#include <errno.h>
#include <fstream>
#include <string>

#define stringify( name ) #name

int main() {


	if (false) {

		int fd = open("oursPlayground/test.http", O_RDONLY);
		if (fd == -1)
			printf("Error opening file: %s\n", strerror(errno));
		printf("fd: %d\n", fd);

		char buffer[MAX_REQUEST_SIZE + 2];
		std::string _rawRequest;

		if (read(fd, buffer, MAX_REQUEST_SIZE + 2) <= MAX_REQUEST_SIZE)
			_rawRequest = buffer;
		else
			throw std::runtime_error(ERR_REQUEST_SIZE);
		std::cout << _rawRequest << std::endl;

		close(fd);
	}

	//Convert enum to string
	if (false)
	{
		std::string str = stringify(GET);
		std::cout << str << std::endl;
	}

	if (true)
	{
		char str[] = "GET www.google.com HTTP/1.1\r\n";
		std::string line = str;
		std::cout << strlen(str) << std::endl;
		std::cout << line.length() << std::endl;

		if (str[strlen(str) - 2] == '\r')
			std::cout << "true" << std::endl;
		else
			std::cout << "false" << std::endl;

		if (line.substr(line.length() - 2, 1) == "\r")
			std::cout << "true" << std::endl;
		else
			std::cout << "false" << std::endl;

		std::ifstream ifs("oursPlayground/test.http");
		std::string line2;
		std::getline(ifs, line2);
		std::cout << line2 << std::endl;
		std::cout << line2.length() << std::endl;
		if (line2.length() > 1 || line2.substr(line2.length() - 1, 1) == "\r")
			std::cout << "true" << std::endl;
		else
			std::cout << "false" << std::endl;

		std::ifstream ifs2("test/resources/request_errFirstLine.http");
		std::string line3;
		std::getline(ifs2, line3);
		std::cout << line3 << std::endl;
		std::cout << line3.length() << std::endl;
		if (line3.length() > 1 || line3.substr(line3.length() - 1, 1) == "\r")
			std::cout << "true" << std::endl;
		else
			std::cout << "false" << std::endl;
	}
	return 0;

}
