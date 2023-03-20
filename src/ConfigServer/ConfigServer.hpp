#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "../../includes/webserv.hpp"
#include "../Server/Server.hpp"

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

	bool lineNeeded(const std::string line);
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

//#include <map>
//
//struct ServerConfig {
//
//	//Server port and name
//	std::vector<std::string>	_serverPorts;
//	std::vector<std::string>	_serverNames;
//
//	//Client size by ports
//	int 						_clientSize;
//
//	//Methods accepted by the server
//	std::vector<eRequestType>			_methods;
//
//	//The key is the location, the value is the path to the file
//	std::vector<std::string>			_root;
//
//	//The key is the location, the value is the path to the file
//	std::map<std::string, std::string>	_locations;
//
//	//The key is the error code, the value is the path to the file
//	//Create data structure for error pages default before
//	std::map<int, std::string>			_errorPages;
//
//};
//
//class ConfigServer {
//
//public:
//	std::vector<ServerConfig> _servers;
//
//	void test()
//	{
//		_servers[0]._methods.push_back(GET);
//	};
//};
#endif


