//
// Created by Teddy BLANCO on 2023-03-16.
//

#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../includes/webserv.hpp"


#define stringify( name ) #name

int main() {


	if (true) {

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

	return 0;

}
