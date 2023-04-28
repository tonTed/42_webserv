
#include "../includes/webserv.hpp"
#include "ConfigServer/ConfigServer.hpp"
#include "Server/Server.hpp"

int main(int ac, char **av)
{
	std::cout << "Launching webserver 42!" << std::endl;
	if (ac > 2)
	{
		std::cout << "Usage: ./webserv or ./webserv [config_file]" << std::endl;
		return (EXIT_FAILURE);
	}
	else if (av[1])
		ConfigServer::getInstance()->setConfigServer(av[1]);
	else
		ConfigServer::getInstance()->setConfigServer("default");

	Server	WebServer;

	return (EXIT_SUCCESS);
}
