#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "../Server/Server.hpp"
# include "../global.hpp"

class ConfigServer
{
  private:
	std::vector<Server> _servers;
	bool _goodFile;
	// std::string _host;
	// std::string _port;
	// std::string _server_name;

  public:
	ConfigServer();
	ConfigServer(const std::string file);
	ConfigServer(const ConfigServer &Config);
	virtual ~ConfigServer();

	ConfigServer &operator=(const ConfigServer &Config);

	bool needed(const std::string line);
	std::string cleanedLine(std::string line);
	bool readFile(const std::string inFile, std::string &stringLine);
	std::vector<std::string> getServerBlocks(const std::string& configStr);
	std::vector<std::string> getLocationBlocks(const std::string& configStr);
	std::string getHost(const std::string& configStr);
	std::string getPort(const std::string& configStr);
	std::string getKeywordValue(const std::string& configStr, const std::string& derective);

	void printBLocks(std::vector<std::string> &serverBlocks);
};

std::ostream &operator<<(std::ostream &o, const ConfigServer &config);
std::string getFile(int argc, const char **argv);

#endif
