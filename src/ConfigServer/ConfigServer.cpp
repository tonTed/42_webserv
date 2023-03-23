#include "ConfigServer.hpp"

ConfigServer::ConfigServer()
	: _goodFile(true)
{
}

ConfigServer::ConfigServer(const ConfigServer &config)
	: _goodFile(true)
{
	*this = config;
}

ConfigServer::ConfigServer(const std::string paramFile)
{
	std::string file = getFile(paramFile);
	std::string stringFile = "";
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
		std::vector<std::string> serverBlocks = getServerBlocks(stringFile);
		printBLocks(serverBlocks);
	}
}

void ConfigServer::printBLocks(std::vector<std::string> &serverBlocks)
{
	for (std::vector<std::string>::const_iterator it = serverBlocks.begin(); it != serverBlocks.end(); ++it)
	{
		//     // Process each server block here
		std::cout << YELLOW << "Server block: " << RESET << *it << std::endl
				  << std::endl;
		std::vector<std::string> hosts = getHosts(*it);
		std::cout << YELLOW << "hosts: " << RESET;
		for (std::vector<std::string>::const_iterator itr = hosts.begin(); itr != hosts.end(); ++itr)
		{
			//     // Process each server block here
			std::cout << *itr << ' ';
		}
		std::cout << std::endl;

		std::vector<int> ports = getPorts(*it);
		std::cout << YELLOW << "Ports: " << RESET;
		for (std::vector<int>::const_iterator itr = ports.begin(); itr != ports.end(); ++itr)
		{
			//     // Process each server block here
			std::cout << *itr << ' ';
		}
		std::cout << std::endl;
		std::cout << YELLOW << "Server name: " << RESET << getKeywordValue(*it, "server_name") << std::endl;
		std::cout << YELLOW << "Client Size: " << RESET << getKeywordValue(*it, "client_size") << std::endl
				  << std::endl;
		// std::vector<std::string> locationBlocks = getLocationBlocks(*it);
		// for (std::vector<std::string>::const_iterator itr = locationBlocks.begin(); itr != locationBlocks.end(); ++itr)
		// {
		// 	//     // Process each server block here
		// 	std::cout << YELLOW << "Location block: " << RESET << *itr << std::endl;
		// }
		std::cout << "_serverNumber::" << this->_serverNumber << std::endl;
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
std::string ConfigServer::getFile(const std::string paramFile)
{
	std::string configFile = "configFiles/default.config";
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
bool ConfigServer::lineNeeded(const std::string line)
{
	size_t i = line.find_first_not_of(" \t");
	bool val = (i != std::string::npos && line[i] != '#');
	return val;
}

/**
 * @brief Remove the spaces and comments from the line
 *
 * @param line The line to be modified
 * @return std::string The modified line
 */
std::string ConfigServer::cleanedLine(std::string line)
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
		j--;
	if (commentPos != std::string::npos && commentPos > static_cast<size_t>(i))
		j = static_cast<int>(commentPos - 1);
	std::string ret = line.substr(i, j - i + 1);
	for (std::string::iterator it = ret.begin(); it != ret.end();)
	{
		if (*it != '\n' && std::isspace(*it))
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
bool ConfigServer::readFile(const std::string inFile, std::string &stringLine)
{
	std::ifstream file(inFile.c_str());
	std::string line = "";

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
 * @return std::vector<std::string>  A vector of strings one by server
 */
std::vector<std::string> ConfigServer::getServerBlocks(const std::string &configStr)
{
	int braceCount;

	std::vector<std::string> serverBlocks;
	std::string::size_type pos = 0;
	while ((pos = configStr.find("server {", pos)) != std::string::npos)
	{
		std::string::size_type bracePos = configStr.find("{", pos);
		if (bracePos != std::string::npos)
		{
			std::string::size_type endPos = bracePos + 1;
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
				std::string serverBlock = configStr.substr(pos, endPos - pos);
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

bool checkServerBlock(const std::string &server)
{
	std::string::size_type pos = 0;
	std::string::size_type bracePos = server.find("{", pos);
	if (bracePos != std::string::npos)
	{
		std::string::size_type endPos = bracePos + 1;
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
 * @return std::vector<std::string> the vector of locations.
 */
std::vector<std::string> ConfigServer::getLocationBlocks(const std::string &configStr)
{
	std::vector<std::string> locationBlocks;
	std::string::size_type pos = 0;
	while ((pos = configStr.find("location ", pos)) != std::string::npos)
	{
		std::string::size_type endPos = configStr.find("}", pos);
		if (endPos == std::string::npos)
			break; // Handle error: closing brace not found
		std::string locationBlock = configStr.substr(pos, endPos - pos + 1);
		locationBlocks.push_back(locationBlock);
		pos = endPos + 1;
	}
	return (locationBlocks);
}

std::vector<std::string>  ConfigServer::getHosts(const std::string &configStr)
{
	std::vector<std::string>  hosts;
	std::string host;
	std::string::size_type pos = 0;
	// std::cout << "pos: " << pos << std::endl;
	while ((pos = configStr.find("listen", pos)) != std::string::npos)
	{
		std::cout << "FIRST Pos " << pos << std::endl;
		pos += 6; // skip "listen "
		std::cout << "Pos + 6::: " << pos << std::endl;
		std::string::size_type endPos = configStr.find(";", pos);
		while ((endPos = configStr.find(";", pos)) != std::string::npos)
		{
			std::string::size_type colonPos = configStr.find(":", pos);
			if (colonPos != std::string::npos && colonPos < endPos)
			{
				std::cout << "colonPos::: " << colonPos << std::endl;
				std::cout << "Pos::: " << pos << std::endl;
				host = configStr.substr(pos, colonPos - pos);
				hosts.push_back(host);
				pos += host.length();
			}
			else if (colonPos == std::string::npos)
			{
				std::cout << "colonPos: " << colonPos << std::endl;
				std::cout << "Pos: " << pos << std::endl;
				host = configStr.substr(pos, endPos - pos);
				hosts.push_back(host);
			}
		}
	}
	return (hosts);
}

// {
// 	std::vector<std::string>  hosts;
// 	std::string::size_type pos = 0;
// 	// std::cout << "pos: " << pos << std::endl;
// 	while ((pos = configStr.find("listen", pos)) != std::string::npos)
// 	{
// 		pos += 6; // skip "listen "
// 		std::string::size_type endPos = configStr.find(":", pos);
// 		// std::cout << "endPos: " << endPos << std::endl;
// 		if (endPos != std::string::npos)
// 		{
// 			while (pos < endPos && isspace(configStr[pos]))
// 				pos++;
// 			hosts.push_back(configStr.substr(pos, endPos - pos));
// 		}
// 		// else
// 		// 	hosts = configStr.substr(pos);
// 	}
// 	return (hosts);
// }

/**
 * @brief Get the port from the config string
 *
 * @param configStr The block to get the port from
 * @return std::string the port value
 */
std::vector<int> ConfigServer::getPorts(const std::string &configStr)
{
	std::vector<int> ports;
	int port;
	std::string::size_type pos = 0;
	while ((pos = configStr.find("listen", pos)) != std::string::npos)
	{
		pos += 6; // skip "listen "
		std::string::size_type endPos = configStr.find(";", pos);
		if (endPos != std::string::npos)
		{
			std::string::size_type colonPos = configStr.find(":", pos);
			if (colonPos != std::string::npos && colonPos < endPos)
			{
				port = std::atoi(configStr.substr(colonPos + 1, endPos - colonPos - 1).c_str());
				ports.push_back(port);
			}
			else if (colonPos == std::string::npos)
			{
				port = std::atoi(configStr.substr(pos, endPos - pos).c_str());
				ports.push_back(port);
			}
		}
	}
	return (ports);
}

/**
 * @brief Get the value depeding on the derective from the config string
 *
 * @param configStr The block to get the server name from
 * @return std::string the server name value
 */
std::string ConfigServer::getKeywordValue(const std::string &configStr, const std::string &derective)
{
	std::string keyWord;
	std::string::size_type pos = configStr.find(derective);
	if (pos != std::string::npos)
	{
		pos += derective.length(); // skip "keyWord "
		std::string::size_type endPos = configStr.find(";", pos);
		if (endPos != std::string::npos)
		{
			while (pos < endPos && isspace(configStr[pos]))
				pos++;
			keyWord = configStr.substr(pos, endPos - pos);
		}
		else
			keyWord = configStr.substr(pos);
	}
	return (keyWord);
}
