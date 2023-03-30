#include "ConfigServer.hpp"
#include "ctype.h"
#include "string"
#include <stdlib.h>

// ConfigServer::ConfigServer()
// : _goodFile(true)
// {
// }

ConfigServer::ConfigServer(const ConfigServer &config)
	: _goodFile(true)
{
	*this = config;
}

ConfigServer::ConfigServer(const string paramFile)
{
	string file = getFile(paramFile);
	string stringFile = "";
	// std::cout << "File: " << file << std::endl;
	if (!readFile(file, stringFile))
	{
		_goodFile = false;
		throw std::runtime_error("Couldn't read!");
	}
	else
	{
		// for (int i = 0; i < (int)stringFile.length(); i++)
		// 	std::cout << stringFile[i];
		_goodFile = true;
		std::vector<string> serverBlocks = getServerBlocks(stringFile);
		printBLocks(serverBlocks);
	}
}

void ConfigServer::printBLocks(std::vector<string> &serverBlocks)
{
	for (std::vector<string>::const_iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it)
	{
		//     // Process each server block here
		std::cout << YELLOW << "Server block: " << RESET << *it << std::endl
				  << std::endl;

		std::vector<string> hosts = getHosts(*it);
		std::cout << YELLOW << "hosts: " << RESET;
		for (std::vector<string>::const_iterator itr = hosts.begin(); itr != hosts.end(); ++itr)
		{
			// Process each server block here
			std::cout << *itr << ' ';
		}
		std::cout << std::endl;

		std::vector<int> ports = getPorts(*it);
		std::cout << YELLOW << "Ports: " << RESET;
		for (std::vector<int>::const_iterator itr = ports.begin(); itr != ports.end(); ++itr)
		{
			// Process each server block here
			std::cout << *itr << ' ';
		}
		std::cout << std::endl;

		std::vector<string> serverNames = getKeywordValue(*it, "server_name");
		std::cout << YELLOW << "Server name: " << RESET;
		for (std::vector<string>::const_iterator itr = serverNames.begin(); itr != serverNames.end(); ++itr)
		{
			// Process each server block here
			std::cout << *itr << ' ';
		}
		std::cout << std::endl;

		std::vector<string> methods = getKeywordValue(*it, "methods");
		std::cout << YELLOW << "Methods: " << RESET;
		for (std::vector<string>::const_iterator itr = methods.begin(); itr != methods.end(); ++itr)
		{
			// Process each server block here
			std::cout << *itr << ' ';
		}
		std::cout << std::endl;

		std::map<int, string> errorPages = getErrorPages(*it);
		std::cout << YELLOW << "Error Pages: " << RESET;
		for (std::map<int, string>::iterator it = errorPages.begin(); it != errorPages.end(); ++it)
		{
			std::cout << it->first << ": " << it->second << std::endl;
		}
		std::cout << std::endl;

		// for (std::map<int, string>::const_iterator itr = errorPages.begin(); itr != errorPages.end(); ++itr)
		// {
		// 	//     // Process each server block here
		// 	std::cout << *itr << ' ';
		// }

		// std::cout << YELLOW << "Client Size: " << RESET << getKeywordValue(*it, "client_size") << std::endl
		// 		  << std::endl;
		// std::vector<string> locationBlocks = getLocationBlocks(*it);
		// for (std::vector<string>::const_iterator itr = locationBlocks.begin(); itr != locationBlocks.end(); ++itr)
		// {
		// 	//     // Process each server block here
		// 	std::cout << YELLOW << "Location block: " << RESET << *itr << std::endl;
		// }
		// std::cout << "_serverNumber::" << this->_serverNumber << std::endl;
	}
}

ConfigServer::~ConfigServer()
{
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
	string host;
	string::size_type pos = 0;
	string::size_type endPos;
	string::size_type bracePos;
	string::size_type colonPos;

	while ((pos = configStr.find("listen", pos)) != string::npos && configStr.find(":", pos)!= string::npos)
	{
		pos += 6; // skip "listen "
		bracePos = configStr.find("{", pos) < configStr.find("}", pos) ? configStr.find("{", pos) : configStr.find("}", pos);
		while ((endPos = configStr.find(";", pos)) != string::npos && pos < endPos &&
			bracePos != string::npos && endPos < bracePos )
		{
			colonPos = configStr.find(":", pos);
			if ((configStr.find(":", pos)!= string::npos) && colonPos < endPos)
			{
				host = trim(configStr.substr(pos, colonPos - pos));
				if(!validHost(host))
				{
					std::cout << BOLD_RED << "Error: invalid host " << host << RESET << std::endl;
					exit(1); // TODO fix the error!
				}
				hosts.push_back(host);
				pos += host.length() + 1;
				while(!isspace(configStr[pos]))
					pos++;
			}
			else if (colonPos == string::npos)
				break;
			bracePos = configStr.find("{", pos) < configStr.find("}", pos) ? configStr.find("{", pos) : configStr.find("}", pos);
		}
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
	string strPort;
	int port;
	string::size_type pos = 0;
	string::size_type colonPos;
	string::size_type endPos;
	string::size_type bracePos;

	while ((pos = configStr.find("listen", pos)) != string::npos)
	{
		pos += 6; // skip "listen "
		bracePos = configStr.find("{", pos) < configStr.find("}", pos) ? configStr.find("{", pos) : configStr.find("}", pos);
		endPos = configStr.find(";", pos);
		while (endPos != string::npos && bracePos != string::npos && pos < endPos && endPos < bracePos)
		{
			colonPos = configStr.find(":", pos);
			if (colonPos != string::npos && colonPos < endPos)
			{
				pos  = colonPos;
				while(!isspace(configStr[pos]) && pos < endPos)
					pos++;				
				strPort = configStr.substr(colonPos + 1, pos - colonPos - 1);
				port = std::atoi(strPort.c_str());
				if (!validPort(port))
				{
					std::cout << BOLD_RED << "Error: invalid port " << strPort << RESET << std::endl;
					exit(1); // TODO fix the error!
				}
				ports.push_back(port);
			}
			else if (colonPos == string::npos)
			{
				strPort = configStr.substr(pos, endPos - pos);
				port = std::atoi(strPort.c_str());
				if (!validPort(port))
				{
					std::cout << BOLD_RED << "Error: invalid port " << strPort << RESET << std::endl;
					exit(1); // TODO fix the error!
				}
				ports.push_back(port);
				pos += strPort.length() + 1;
			}
		}
		bracePos = configStr.find("{", pos) < configStr.find("}", pos) ? configStr.find("{", pos) : configStr.find("}", pos);
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
	string::size_type pos = 0;
	string::size_type poSpace = 0;
	string::size_type brace;

	if ((pos = configStr.find(derective)) != string::npos)
	{
		pos += derective.length(); // skip "keyWord "
		string::size_type endPos = configStr.find(";", pos);
		while (endPos != string::npos && pos < endPos && endPos < brace &&
			   (brace = configStr.find("{", endPos)) != string::npos)
		{
			while (pos < endPos && isspace(configStr[pos]))
				pos++;
			poSpace = pos;
			while (poSpace < endPos && !isspace(configStr[poSpace]))
				poSpace++;

			keyWord.push_back(configStr.substr(pos, poSpace - pos));
			pos = poSpace;
			brace = configStr.find("{", endPos);
		}
	}
	// handle error ULAC derective!
	return (keyWord);
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
}
