#include "ConfigServer.hpp"
#include "ctype.h"
#include "string"
#include <stdlib.h>

ConfigServer *ConfigServer::singleton = NULL;

ConfigServer::ConfigServer(const ConfigServer &config)
	: _goodFile(true)
{
	*this = config;
}

ConfigServer::ConfigServer(const string paramFile)
{
	string file = getFile(paramFile);
	string stringFile = "";
	if (!readFile(file, stringFile))
	{
		_goodFile = false;
		throw std::runtime_error("Couldn't read!");
	}
	else
	{
		_goodFile = true;
		std::vector<string> serverBlocks = getServerBlocks(stringFile);
		// printBLocks(serverBlocks);
		setServersData(serverBlocks);
		std::vector<ServerData> data = getServerData();
		printServersData(data);
	}
}

void ConfigServer::printBLocks(std::vector<string> &serverBlocks)
{
	for (std::vector<string>::const_iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it)
	{
		//     // Process each server block here
		std::cout << YELLOW << "Server block: |" << RESET << *it << "|" << std::endl
				  << std::endl;

		std::vector<string> hosts = getHosts(*it);
		std::cout << YELLOW << "hosts: |" << RESET;
		for (std::vector<string>::const_iterator itr = hosts.begin(); itr != hosts.end(); ++itr)
		{
			// Process each server block here
			std::cout << *itr << ' ';
		}
		std::cout << "|" << std::endl;

		// std::vector<int> ports = getPorts(*it);
		// std::cout << YELLOW << "Ports: " << RESET;
		// for (std::vector<int>::const_iterator itr = ports.begin(); itr != ports.end(); ++itr)
		// {
		// 	// Process each server block here
		// 	std::cout << *itr << ' ';
		// }
		// std::cout << std::endl;

		// std::vector<string> serverNames = getKeywordValue(*it, "server_name");
		// std::cout << YELLOW << "Server name: " << RESET;
		// for (std::vector<string>::const_iterator itr = serverNames.begin(); itr != serverNames.end(); ++itr)
		// {
		// 	// Process each server block here
		// 	std::cout << *itr << ' ';
		// }
		// std::cout << std::endl;

		// std::vector<string> methods = getKeywordValue(*it, "methods");
		// std::cout << YELLOW << "Methods: " << RESET;
		// for (std::vector<string>::const_iterator itr = methods.begin(); itr != methods.end(); ++itr)
		// {
		// 	// Process each server block here
		// 	std::cout << *itr << ' ';
		// }
		// std::cout << std::endl;

		// std::map<int, string> errorPages = getErrorPages(*it);
		// std::cout << YELLOW << "Error Pages: " << RESET;
		// for (std::map<int, string>::iterator it = errorPages.begin(); it != errorPages.end(); ++it)
		// {
		// 	std::cout << it->first << ": " << it->second << std::endl;
		// }
		// std::cout << std::endl;
	}
}

ConfigServer::~ConfigServer()
{
	// Destroy the Singlton
	destroy();
}

ConfigServer &ConfigServer::operator=(const ConfigServer &config)
{
	if (this != &config)
	{
		// _servers = config._servers;
		_goodFile = config._goodFile;
	}
	return (*this);
}

/**
 * @brief Check if the file exists
 *
 * @param file The path to the file

	* @return return the givent file Path if it works otherwise throw an error.
 */
string ConfigServer::getFile(const string paramFile)
{
	string configFile = "configFiles/default.config";
	if (paramFile != "default")
	{
		std::ifstream f(paramFile.c_str());
		if (f.good())
			return (paramFile);
		std::cout << BOLD_RED << "Bad File Given in Param! " << RESET << std::endl;
	}
	else
	{
		std::cout << BOLD_RED << "Using default config file! " << RESET << std::endl;
		std::ifstream f(configFile.c_str());
		if (!f.good())
			throw std::runtime_error("Default config file not found or no permissions! ⛔️");
	}
	return (configFile);
}

/**
 * @brief Check if the line is neeeded and not just a comment or just spaces
 *
 * @param file The path to the file
 * @return true if not just a comment or just spaces
 * @return false otherwise
 */
bool ConfigServer::lineNeeded(const string line)
{
	size_t i = line.find_first_not_of(" \t");
	bool val = (i != string::npos && line[i] != '#');
	return val;
}

/**
 * @brief Remove the spaces and comments from the line
 *
 * @param line The line to be modified
 * @return string The modified line
 */
string ConfigServer::cleanedLine(string line)
{
	int i;
	int j;
	const char *str;
	size_t commentPos;
	bool found_first_space = false;

	i = 0;
	j = static_cast<int>(line.length()) - 1;
	str = line.c_str();
	commentPos = line.find("#");
	while (i < j && std::isspace(static_cast<int>(str[i])))
		i++;
	while (j > i && isrealspace(static_cast<int>(str[j])))
		// while (j > i && std::isspace(static_cast<int>(str[j])))
		j--;
	if (commentPos != string::npos && commentPos > static_cast<size_t>(i))
		j = static_cast<int>(commentPos - 1);
	string ret = line.substr(i, j - i + 1);
	for (string::iterator it = ret.begin(); it != ret.end();)
	{
		if (std::isspace(*it))
		{
			if (!found_first_space)
			{
				*it = ' ';
				found_first_space = true;
				++it;
			}
			else
				it = ret.erase(it);
		}
		else
		{
			found_first_space = false;
			++it;
		}
	}
	return (ret);
}

/**
 * @brief Read the file and store the data in a string
 *
 * @param inFile The path to the file
 * @param stringLine The string to store the data
 * @return true if the file could be open
 * @return false otherwise
 */
bool ConfigServer::readFile(const string inFile, string &stringLine)
{
	std::ifstream file(inFile.c_str());
	string line = "";

	while (!(line = getline_with_newline(file)).empty())
	{
		if (lineNeeded(line))
		{
			// std::cout << "line: " << cleanedLine(line) ;
			stringLine += cleanedLine(line);
		}
	}
	file.close();
	return (true);
}

/**
 * @brief Split configStr where all file is stored into blocks of servers
 *
 * @param configStr the string where readfile() stored the file it read.
 * @return std::vector<string>  A vector of strings one by server
 */
std::vector<string> ConfigServer::getServerBlocks(const string &configStr)
{
	int braceCount;

	std::vector<string> serverBlocks;
	string::size_type pos = 0;
	while ((pos = configStr.find("server {", pos)) != string::npos)
	{
		string::size_type bracePos = configStr.find("{", pos);
		if (bracePos != string::npos)
		{
			string::size_type endPos = bracePos + 1;
			braceCount = 1;
			while (braceCount > 0 && endPos < configStr.length())
			{
				if (configStr[endPos] == '{')
					braceCount++;
				else if (configStr[endPos] == '}')
					braceCount--;
				endPos++;
			}
			if (braceCount == 0)
			{
				string serverBlock = configStr.substr(pos, endPos - pos);
				serverBlocks.push_back(serverBlock);
				pos = endPos;
				this->_serverNumber++;
				continue;
			}
		}
		std::cout << BOLD_RED << "Error: opening brace not found! " << RESET << std::endl;
		break; // Handle error: opening brace not found
	}
	return (serverBlocks);
}

bool checkServerBlock(const string &server)
{
	string::size_type pos = 0;
	string::size_type bracePos = server.find("{", pos);
	if (bracePos != string::npos)
	{
		string::size_type endPos = bracePos + 1;
		int braceCount = 1;
		while (braceCount > 0 && endPos < server.length())
		{
			if (server[endPos] == '{')
				braceCount++;
			else if (server[endPos] == '}')
				braceCount--;
			endPos++;
		}
		if (braceCount == 0)
			return (true);
	}
	return (false);
}

/**
 * @brief Split the server block into a string of location
 *
 * @param configStr the input, should be the sting server block
 * @return std::vector<string> the vector of locations.
 */
std::vector<string> ConfigServer::getLocationBlocks(const string &configStr)
{
	std::vector<string> locationBlocks;
	string::size_type pos = 0;
	while ((pos = configStr.find("location ", pos)) != string::npos)
	{
		string::size_type endPos = configStr.find("}", pos);
		if (endPos == string::npos)
			break; // Handle error: closing brace not found
		string locationBlock = configStr.substr(pos, endPos - pos + 1);
		locationBlocks.push_back(locationBlock);
		pos = endPos + 1;
	}
	return (locationBlocks);
}

/**
 * @brief Get the Hosts object
 *
 * @param configStr the input, should be the sting server block
 * @return std::vector<string> the vector of hosts.
 */
std::vector<string> ConfigServer::getHosts(const string &configStr)
{
	std::vector<string> hosts;
	string::size_type pos = configStr.find("listen ", 0);
	string::size_type newPos = 0;
	string hostLine;
	string host;
	string::size_type endPos;
	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
									 ? configStr.find("{", pos)
									 : configStr.find("}", pos);

	while (pos != string::npos && pos < bracePos)
	{
		endPos = configStr.find(";", pos);
		while (endPos != string::npos && pos < endPos &&
			   bracePos != string::npos && endPos < bracePos)
		{
			hostLine = trim(getHostLine(configStr, pos, &newPos));
			if (!hostLine.empty())
			{
				int i = 0, j = 0;
				while (hostLine[j])
					if (hostLine[j++] == ':')
						i++;
				j = 0;
				while (j < i)
				{
					host = getHostPart(hostLine);
					if (!validHost(host))
					{
						std::cout << BOLD_RED << "Error: invalid host |" << host << "|" << RESET << std::endl;
						exit(1); // TODO fix the error!
					}
					hosts.push_back(host);
					hostLine = hostLine.substr(hostLine.find(":", 0) + 1, hostLine.length() - hostLine.find(":", 0) - 1);
					j++;
				}
				pos = newPos;
			}
			endPos = configStr.find(";", pos);
			pos = configStr.find("listen ", pos);
		}
	}
	if (hosts.size() > 2)
	{
		std::cout << BOLD_RED << "Error: too many hosts " << RESET << std::endl;
		exit(1); // TODO fix the error!
	}
	return (hosts);
}

/**
 * @brief Get the port from the config string
 *
 * @param configStr The block to get the port from
 * @return string the port value
 */
std::vector<int> ConfigServer::getPorts(const string &configStr)
{
	std::vector<int> ports;
	string::size_type pos = configStr.find("listen ", 0);
	string::size_type newPos = 0;
	string portLine;
	string strPort;
	int port;
	string::size_type endPos;
	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
									 ? configStr.find("{", pos)
									 : configStr.find("}", pos);

	while (pos != string::npos && pos < bracePos)
	{
		endPos = configStr.find(";", pos);
		while (endPos != string::npos && pos < endPos &&
			   bracePos != string::npos && endPos < bracePos)
		{
			portLine = trim(getHostLine(configStr, pos, &newPos));
			if (!portLine.empty())
			{
				int i = 0, j = 0;
				while (portLine[j])
					if (portLine[j++] == ':')
						i++;
				j = 0;
				if (i) // get th port incase of the presence of ":"
				{
					while (j < i)
					{
						strPort = getPortPart(portLine);
						port = std::atoi(strPort.c_str());
						if (!validPort(port))
						{
							std::cout << BOLD_RED << "Error: invalid host |" << strPort << "|" << RESET << std::endl;
							exit(1); // TODO fix the error!
						}
						ports.push_back(port);
						portLine = portLine.substr(portLine.find(" ", 0) + 1, portLine.length() - portLine.find(":", 0) - 1);
						j++;
					}
				}
				else // get the port if the derective has no ":"
				{
					strPort = getPortPart(portLine);
					port = std::atoi(strPort.c_str());
					if (!validPort(port))
					{
						std::cout << BOLD_RED << "Error: invalid host |" << strPort << "|" << RESET << std::endl;
						exit(1); // TODO fix the error!
					}
					ports.push_back(port);
				}
				pos = newPos;
			}
			endPos = configStr.find(";", pos);
			pos = configStr.find("listen ", pos);
		}
	}
	if (ports.size() < 1 || portDup(ports))
	{
		std::cout << BOLD_RED << "Error: no ports or duplicate ports" << RESET << std::endl;
		exit(1); // TODO fix the error!
	}

	return (ports);
}

/**
 * @brief Get the value depeding on the derective from the config string
 *
 * @param configStr The block to get the server name from
 * @return string the server name value
 */
std::vector<string> ConfigServer::getKeywordValue(const string &configStr, const string &derective)
{
	std::vector<string> keyWord;
	string::size_type pos = configStr.find(derective, 0);
	string::size_type poSpace = 0;
	string word;
	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
									 ? configStr.find("{", pos)
									 : configStr.find("}", pos);
	while (pos != string::npos && pos < bracePos)
	{
		pos += derective.length(); // skip "keyWord "
		string::size_type endPos = configStr.find(";", pos);
		while (endPos != string::npos && pos < endPos && bracePos != string::npos && endPos < bracePos)
		{
			while (pos < endPos && isspace(configStr[pos]))
				pos++;
			poSpace = pos;
			while (poSpace < endPos && !isspace(configStr[poSpace]))
				poSpace++;
			word = configStr.substr(pos, poSpace - pos);
			if (word.empty())
			{
				std::cout << BOLD_RED << "Error: " << derective << " can't be empty!" << RESET << std::endl;
				exit(1); // TODO fix the error!
			}
			keyWord.push_back(word);
			pos = poSpace;
			while (pos < endPos && isspace(configStr[pos]))
				pos++;
			endPos = configStr.find(";", pos);
		}
		pos = configStr.find(derective, pos);
	}
	return (keyWord);
}

std::vector<enum eRequestType> ConfigServer::getMethods(const string &configStr)
{
	std::vector<enum eRequestType> methods;
	string::size_type pos = configStr.find("methods", 0);
	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
									 ? configStr.find("{", pos)
									 : configStr.find("}", pos);
	string::size_type newPos = 0;
	string wordLine;
	std::vector<string> words;
	while (pos != string::npos && pos < bracePos)
	{
		string::size_type endPos = configStr.find(";", pos);
		while (endPos != string::npos && endPos < bracePos && pos < endPos)
		{
			wordLine = trim(getMethodsLine(configStr, pos, &newPos));
			if (wordLine.empty())
			{
				std::cout << BOLD_RED << "Error: methods can't be empty!" << RESET << std::endl;
				exit(1); // TODO fix the error!
			}
			words = splitString(wordLine);
			for (int i = 0; i < static_cast<int>(words.size()); i++) // loop until all words have been returned
			{
				if (words[i].empty())
				{
					std::cout << BOLD_RED << "Error: methods can't be empty!" << RESET << std::endl;
					exit(1); // TODO fix the error!
				}
				else if (words[i] == "GET")
					methods.push_back(GET);
				else if (words[i] == "POST")
					methods.push_back(POST);
				else if (words[i] == "DELETE")
					methods.push_back(DELETE);
				else
				{
					std::cout << BOLD_RED << "Error: Invalid HTTP method: " << words[i] << RESET << std::endl;
					exit(1); // TODO fix the error!
				}
			}
			pos = newPos;
			endPos = configStr.find(";", pos);
		}
		pos = configStr.find("methods", pos);
	}
	return methods;
}

/**
 * @brief Get the value depeding on the derective from the config string
 *
 * @param configStr The block to get the server name from
 * @return string the server name value
 */
std::map<int, string> ConfigServer::getErrorPages(const string &configStr)
{
	std::map<int, string> errorPages;
	string::size_type pos = 0;
	string::size_type brace;
	while ((pos = configStr.find("error_page", pos)) != string::npos)
	{
		pos += 10; // skip "error_page "
		string::size_type poSpace = pos;
		string::size_type endPos = configStr.find(";", pos);
		if (endPos != string::npos && (brace = configStr.find("{", endPos)) != string::npos && endPos < brace)
		{
			while (pos < endPos && isspace(configStr[pos]))
				pos++;
			poSpace = pos;
			while (poSpace < endPos && !isspace(configStr[poSpace]))
				poSpace++;
			int errorCode = std::atoi(configStr.substr(pos, poSpace - pos).c_str());
			pos = poSpace;
			if (endPos != string::npos)
			{
				string errorPage = configStr.substr(pos, endPos - pos);
				errorPages[errorCode] = errorPage;
			}
		}
		pos = endPos + 1;
		brace = configStr.find("{", endPos);
	}
	return errorPages;
};

std::vector<ServerData> ConfigServer::getServerData() const
{
	return this->_serversData;
}

void ConfigServer::printServersData(std::vector<ServerData> &data)
{
	for (std::vector<ServerData>::iterator it = data.begin(); it != data.end(); ++it)
	{
		std::cout << std::endl
				  << YELLOW << "Hosts: ";
		for (std::vector<string>::iterator itHosts = it->_hosts.begin(); itHosts != it->_hosts.end(); ++itHosts)
		{
			std::cout << RESET << "|" << *itHosts << "|"
					  << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Server ports: ";
		for (std::vector<int>::iterator itPorts = it->_serverPorts.begin(); itPorts != it->_serverPorts.end(); ++itPorts)
		{
			std::cout << RESET << "|" << *itPorts << "|"
					  << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Server names: ";
		for (std::vector<std::string>::iterator itNames = it->_serverNames.begin(); itNames != it->_serverNames.end(); ++itNames)
		{
			std::cout << RESET << "|" << *itNames << "|"
					  << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Methods: ";
		for (std::vector<enum eRequestType>::iterator itMethods = it->_methods.begin(); itMethods != it->_methods.end(); ++itMethods)
		{
			std::cout << RESET << "|" << *itMethods << "|"
					  << " ";
		}
		std::cout << std::endl;

		// std::cout << "Root: ";
		// for (std::vector<std::string>::iterator itRoot = it->_root.begin(); itRoot != it->_root.end(); ++itRoot)
		// {
		//     std::cout << *itRoot << " ";
		// }
		// std::cout << std::endl;

		// std::cout << "Error pages: ";
		// for (std::map<int, std::string>::iterator itErrorPages = it->_errorPages.begin(); itErrorPages != it->_errorPages.end(); ++itErrorPages)
		// {
		//     std::cout << itErrorPages->first << ": " << itErrorPages->second << " ";
		// }
		// std::cout << std::endl;

		// std::cout << "Locations: ";
		// for (std::map<std::string, struct Locations>::iterator itLocations = it->_locations.begin(); itLocations != it->_locations.end(); ++itLocations)
		// {
		//     std::cout << itLocations->first << ": " << itLocations->second.path << " ";
		// }
		// std::cout << std::endl;
	}
}

void ConfigServer::setServersData(std::vector<string> &serverBlocks)
{
	std::vector<ServerData> servers(serverBlocks.size());
	for (int i = 0; i < static_cast<int>(serverBlocks.size()); i++)
	{

		// std::cout << YELLOW << "Server block: |" << RESET << serverBlocks[i] << "|" << std::endl
		// << std::endl;

		servers[i]._hosts = getHosts(serverBlocks[i]);
		servers[i]._serverPorts = getPorts(serverBlocks[i]);
		servers[i]._serverPorts = getPorts(serverBlocks[i]);
		servers[i]._serverNames = getKeywordValue(serverBlocks[i], "server_name");
		servers[i]._methods = getMethods(serverBlocks[i]);

		// std::vector<string> serverNames = getKeywordValue(*it, "server_name");
		// std::cout << YELLOW << "Server name: " << RESET;
		// for (std::vector<string>::const_iterator itr = serverNames.begin(); itr != serverNames.end(); ++itr)
		// {
		// 	// Process each server block here
		// 	std::cout << *itr << ' ';
		// }
		// std::cout << std::endl;

		// std::vector<string> methods = getKeywordValue(*it, "methods");
		// std::cout << YELLOW << "Methods: " << RESET;
		// for (std::vector<string>::const_iterator itr = methods.begin(); itr != methods.end(); ++itr)
		// {
		// 	// Process each server block here
		// 	std::cout << *itr << ' ';
		// }
		// std::cout << std::endl;

		// std::map<int, string> errorPages = getErrorPages(*it);
		// std::cout << YELLOW << "Error Pages: " << RESET;
		// for (std::map<int, string>::iterator it = errorPages.begin(); it != errorPages.end(); ++it)
		// {
		// 	std::cout << it->first << ": " << it->second << std::endl;
		// }
		// std::cout << std::endl;
	}

	this->_serversData = servers;
	//     {
	//         std::cout << "Server ports: ";
	//         for (std::vector<int>::iterator itPorts = it->_serverPorts.begin(); itPorts != it->_serverPorts.end(); ++itPorts)
	//         {
	//             std::cout << *itPorts << " ";
	//         }
	//         std::cout << std::endl;
	//         std::cout << "Server names: ";
	//         for (std::vector<std::string>::iterator itNames = it->_serverNames.begin(); itNames != it->_serverNames.end(); ++itNames)
	//         {
	//             std::cout << *itNames << " ";
	//         }
	//         std::cout << std::endl;
	//         std::cout << "Methods: ";
	//         for (std::vector<enum eRequestType>::iterator itMethods = it->_methods.begin(); itMethods != it->_methods.end(); ++itMethods)
	//         {
	//             std::cout << *itMethods << " ";
	//         }
	//         std::cout << std::endl;
	//         std::cout << "Root: ";
	//         for (std::vector<std::string>::iterator itRoot = it->_root.begin(); itRoot != it->_root.end(); ++itRoot)
	//         {
	//             std::cout << *itRoot << " ";
	//         }
	//         std::cout << std::endl;
	//         std::cout << "Error pages: ";
	//         for (std::map<int, std::string>::iterator itErrorPages = it->_errorPages.begin(); itErrorPages != it->_errorPages.end(); ++itErrorPages)
	//         {
	//             std::cout << itErrorPages->first << ": " << itErrorPages->second << " ";
	//         }
	//         std::cout << std::endl;
	//         // std::cout << "Locations: ";
	//         // for (std::map<std::string, struct Locations>::iterator itLocations = it->_locations.begin(); itLocations != it->_locations.end(); ++itLocations)
	//         // {
	//         //     std::cout << itLocations->first << ": " << itLocations->second.path << " ";
	//         // }
	//         // std::cout << std::endl;
	//     }
}
