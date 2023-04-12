#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP
#include "../../includes/webserv.hpp"

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
	std::vector<std::string> _hosts;

	// Methods accepted by the server
	std::vector<enum eRequestType> _methods;

	// The key is the location, the value is the path to the file
	std::vector<std::string> _root;	 // TODO make it a string
	std::vector<std::string> _index; // TODO add index directive
	// std::string _root;

	// The key is the error code, the value is the path to the file
	// Create data structure for error pages default before
	std::map<int, std::string> _errorPages;

	// The key is the location, the value is the path to the file
	std::map<std::string, struct Locations> _locations;
};

struct ServerLocation
{
	std::string _path;
	std::vector<std::string> _root;
	std::vector<std::string> _index;
	std::vector<std::string> _autoindex;
	std::vector<std::string> _redirection;
	std::vector<std::string> _methods;
};

struct ServerBlocks
{
	// Server port and name
	std::vector<std::string> _listen;
	std::vector<std::string> _serverNames;
	std::vector<std::string> _root;
	std::vector<std::string> _methods;
	std::vector<std::string> _index;
	std::vector<std::string> _errorPages;
	std::vector<ServerLocation> _locations;
};

class ConfigServer
{
private:
	std::vector<ServerData> _serversData;
	std::vector<ServerBlocks> _serverBlocks;
	std::string _configString;
	bool _goodFile;
	unsigned int _serverNumber;
	static ConfigServer *singleton;
	ConfigServer(){};

public:
	void setConfigServer(const std::string file);
	virtual ~ConfigServer();

	static ConfigServer *getInstance()
	{
		if (singleton == NULL)
		{
			singleton = new ConfigServer();
		}
		return singleton;
	};

	static void destroy()
	{
		if (singleton != NULL)
			delete singleton;
		singleton = NULL;
	};

	ConfigServer &operator=(const ConfigServer &Config);
	void setConfigString(const std::string &configStr);

	std::string getConfigString();
	std::vector<ServerBlocks> getServerBlocks() const;
	std::vector<ServerData> getServerData() const;
	std::vector<std::string> getServerBlocksData(const std::string &configStr);
	std::vector<string> getLocationBlocks(std::string &configStr);

	std::vector<ServerLocation> getLocationPart(std::vector<std::string> locations);
	std::vector<std::string> getDirective(std::string &configStr, std::string directive);
	std::vector<std::string> getDirectiveVal(const std::string &configStr, std::string directive);
	std::vector<std::string> getHosts(const std::string &configStr);
	std::vector<int> getPorts(const std::string &configStr);
	std::map<int, std::string> getErrorPages(const std::string &configStr);

	Locations settingLocation(ServerLocation &locStruct, ServerData &serverBlock);
	void setServerBlocks();
	void setServersData(std::vector<ServerBlocks> &blocks);
	void printServerBlocks();
	void printServersData(std::vector<ServerData> &data);

	// PARSING METHODS
	std::string getFile(const std::string paramFile);
	std::string cleanedLine(std::string line);
	bool readFile(const std::string inFile, std::string &stringLine);
};

std::ostream &operator<<(std::ostream &o, const ConfigServer &config);

/* The functons utils */

// GENERAL UTILS FUNCTIONS
bool lineNeeded(const std::string line);
std::string getline_with_newline(std::istream &input);
bool isrealspace(char c);
bool validBraces(const string &str);
void exit_error(string str, string err);

//  HOST PART
bool validHost(std::string host);

//  PORT PART
bool validPort(int portNumb);
bool portDup(std::vector<int> seq);

//  ERROR PAGE PART
bool validErrorPage(int code, std::string input);

//  METHODS PART
std::vector<enum eRequestType> validMethods(std::vector<std::string> words);

//  ROOT PART
bool validFilePath(std::string input);
bool validRoot(std::string input);
std::string format_string(const std::string &str);

//  INDEX PART
void validIndex(std::string &input, std::string &root);

//  AUTOINDEX PART
bool validAutoindex(std::string input);

// LOCATIONS PART
std::string getLocationPath(std::string &input);
bool lactionPathValid(string &path);
bool pathDup(std::vector<std::string> seq);

//  REDIRECTION PART
bool validRedirection(std::string input);

void validRemaining(std::string &confStr);

#endif
