
#include "../includes/webserv.hpp"
#include "ConfigServer/ConfigServer.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// TODO: remove this
void mock_server()
{
	std::cout << "Launching mock webserver 42!" << std::endl;

	int sock_server = socket(AF_INET, SOCK_STREAM, 0);
	{
		if (sock_server == -1)
		{
			std::cout << "Error creating socket" << std::endl;
			return;
		}
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(sock_server, (struct sockaddr *)&addr, sizeof(addr));
	{
		if (ret == -1)
		{
			std::cout << "Error binding socket" << std::endl;
			return;
		}
	}

	ret = listen(sock_server, 10);
	{
		if (ret == -1)
		{
			std::cout << "Error listening socket" << std::endl;
			return;
		}
	}

	while (true)
	{

		int sock_client = accept(sock_server, NULL, NULL);
		{
			if (sock_client == -1)
			{
				std::cout << "Error accepting socket" << std::endl;
				return;
			}
		}

		char buffer[MAX_REQUEST_SIZE + 2];
		int ret_read = read(sock_client, buffer, MAX_REQUEST_SIZE + 2);
		{
			if (ret_read == -1)
			{
				std::cout << "Error reading socket" << std::endl;
				return;
			}
		}
		buffer[ret_read] = '\0';
		std::cout << buffer << std::endl;

		std::string response = "HTTP/1.1 200 OK\nContent-Length: 12\r\n\r\nHello World!";
		int ret_write = write(sock_client, response.c_str(), response.size());
		{
			if (ret_write == -1)
			{
				std::cout << "Error writing socket" << std::endl;
				return;
			}
		}

		close(sock_client);
	}
	close(sock_server);
}

int main(int ac, char **av)
{

	std::cout << "Launching webserver 42!" << std::endl;
	if (ac > 2)
	{
		std::cout << "Usage: ./webserv or ./webserv [config_file]" << std::endl;
		return (EXIT_FAILURE);
	}
	else if (av[1] && std::string(av[1]) == "-m") // TODO: remove this
	{
		mock_server();
		return (EXIT_SUCCESS);
	}

	else if (av[1])
		ConfigServer::getInstance()->setConfigServer(av[1]);

	else
		ConfigServer::getInstance()->setConfigServer("default");

	return (EXIT_SUCCESS);
}
