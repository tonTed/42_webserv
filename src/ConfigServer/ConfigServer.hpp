#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "../Server/Server.hpp"
# include "../global.hpp"


class ConfigServer
{
  private:
	std::vector<Server> _servers;
	bool _useDefault;

  public:
	ConfigServer();
	ConfigServer(const std::string file);
	ConfigServer(const ConfigServer &Config);
	virtual ~ConfigServer();

	ConfigServer &operator=(const ConfigServer &Config);

	bool notNeeded(const std::string line);
	std::string removeSpaceComments(std::string line);
	bool readFile(const std::string inFile, std::string &stringLine);

};

std::ostream &operator<<(std::ostream &o, const ConfigServer &config);
std::string getFile(int argc, const char **argv);

#endif
