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
	std::vector<std::string> _root;

	// The key is the error code, the value is the path to the file
	// Create data structure for error pages default before
	std::map<int, std::string> _errorPages;

	// The key is the location, the value is the path to the file
	std::map<std::string, struct Locations> _locations; 
	// TODO fix the key issue.
	/*
	if there are multiple location blocks like:
		location / {
			...
		}
		location / {
			...
		}
		we need to be able to differentiate between them.	
	*/ 
};

class ConfigServer
{
private:
	std::vector<ServerData> _serversData;
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

	// PARSING METHODS
	std::string getFile(const std::string paramFile);
	bool lineNeeded(const std::string line);
	std::string cleanedLine(std::string line);
	bool readFile(const std::string inFile, std::string &stringLine);
	std::vector<std::string> getServerBlocks(const std::string &configStr);
	std::vector<std::string> getLocationBlocks(const std::string &configStr);

	// PARSE HOSTS
	std::vector<std::string> getHosts(const std::string &configStr);
	void setHosts(std::vector<std::string> &hosts);

	std::vector<int> getPorts(const std::string &configStr);
	std::vector<std::string> getKeywordValue(const std::string &configStr, const std::string &derective);
	string getStrValue(const string &configStr, const string &directive);
	std::map<int, std::string> getErrorPages(const std::string &configStr);
	std::vector<enum eRequestType> getMethods(const string &configStr);
	struct Locations settingLocation(string &locString);


	std::vector<ServerData> getServerData() const;
	void setServersData(std::vector<string> &serverBlocks);
	void printServersData(std::vector<ServerData> &data);

};

std::ostream &operator<<(std::ostream &o, const ConfigServer &config);


/* The functons utils */
// GENERAL UTILS FUNCTIONS
std::string getline_with_newline(std::istream &input);
bool isrealspace(char c);
std::string trim(const std::string &str);
std::vector<std::string> splitString(std::string input);

void exit_error(string str, string err);
bool validBraces(const string &str);

//  HOST PART
std::string getHostLine(const std::string &str, std::string::size_type &startPos, std::string::size_type *newPos);
std::string getHostPart(const std::string &input);
bool validHost(std::string host);

//  PORT PART
std::string getPortLine(const std::string &str,
                        string::size_type &startPos, string::size_type *newPos);
std::string getPortPart(const std::string &input);
bool portDup(std::vector<int> seq);
bool validPort(int portNumb);

//  METHODS PART
std::string getMethodsLine(const std::string &str,
                        string::size_type &startPos, string::size_type *newPos);
std::string get_next_word(const std::string& input);

//  ERROR PAGE PART
bool validErrorPage(std::string input);

// LOCATIONS PART
std::string getLocationPath(const std::string& input);
bool lactionPathValid(string &path);

//  AUTOINDEX PART
bool validAutoindex(std::string input);

//  REDIRECTION PART
bool validRedirection(std::string input);

//  INDEX PART
// bool validIndex(std::vector<std::string> &input, std::string &path);
void validIndex(std::vector<std::string> &input, std::string &path);
bool pathDup(std::vector<std::string> seq);

bool validClientBodySize(std::string input);
bool validFilePath(std::string input);

#endif
