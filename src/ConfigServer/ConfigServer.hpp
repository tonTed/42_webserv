#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "../Server/Server.hpp"
# include "../global.hpp"

class ConfigServer
{
  private:
	std::vector<Server> _servers;
	bool _goodFile;

  public:
	ConfigServer();
	ConfigServer(const std::string file);
	ConfigServer(const ConfigServer &Config);
	virtual ~ConfigServer();

	ConfigServer &operator=(const ConfigServer &Config);

	bool needed(const std::string line);
	std::string cleanedLine(std::string line);
	bool readFile(const std::string inFile, std::string &stringLine);
	// std::vector<std::string> getServerBlocks(std::string input);
	std::vector<std::string> getServerBlocks(const std::string& configStr);
	std::vector<std::string> getLocationBlocks(const std::string& configStr);
};

std::ostream &operator<<(std::ostream &o, const ConfigServer &config);
std::string getFile(int argc, const char **argv);

#endif
