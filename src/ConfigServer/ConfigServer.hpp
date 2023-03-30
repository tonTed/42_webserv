#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP

#include "../../includes/webserv.hpp"
#include "../Server/Server.hpp"

#include <map>

/*

*/
struct Locations
{
	std::string root;
	std::vector<std::string> index;
	std::string autoindex; // ON or OFF
	std::string redirection;
	std::vector<enum eRequestType> methods;
};

struct ServerData
{
	// Server port and name
	std::vector<int> _serverPorts; // Martin
	std::vector<std::string> _serverNames;

	// Methods accepted by the server
	std::vector<enum eRequestType> _methods;

	// The key is the location, the value is the path to the file
	std::vector<std::string> _root;

	// The key is the error code, the value is the path to the file
	// Create data structure for error pages default before
	std::map<int, std::string> _errorPages;

	// The key is the location, the value is the path to the file
	std::map<std::string, struct Locations> _locations;
};

// ServerData[0]._serverPorts

class ConfigServer
{
	private:
		std::vector<ServerData> _serversData;
		bool _goodFile;
		unsigned int _serverNumber;
		static ConfigServer *singleton;
		ConfigServer(){};

	public:
		// ConfigServer();
		ConfigServer(const std::string file);
		ConfigServer(const ConfigServer &Config);
		virtual ~ConfigServer();

		static ConfigServer *getInstance()
		{
			if (singleton == NULL)
			{
				singleton = new ConfigServer();
			}
			return singleton;
		}

		ConfigServer &operator=(const ConfigServer &Config);

		std::string getFile(const std::string paramFile);
		bool lineNeeded(const std::string line);
		std::string cleanedLine(std::string line);
		bool readFile(const std::string inFile, std::string &stringLine);
		std::vector<std::string> getServerBlocks(const std::string &configStr);
		std::vector<std::string> getLocationBlocks(const std::string &configStr);
		// std::string getHost(const std::string& configStr);
		std::vector<std::string> getHosts(const std::string &configStr);
		std::vector<int> getPorts(const std::string &configStr);
		// std::string getKeywordValue(const std::string &configStr, const std::string &derective);
		std::vector<std::string> getKeywordValue(const std::string &configStr, const std::string &derective);
		std::map<int, std::string> getErrorPages(const std::string &configStr);

		void printBLocks(std::vector<std::string> &serverBlocks);
};

std::ostream &operator<<(std::ostream &o, const ConfigServer &config);
std::string getline_with_newline(std::istream &input);
bool isrealspace(char c);
bool validHost(std::string host);
std::string trim(const std::string& str);


#endif
