#include "ConfigServer.hpp"

/**
 * @brief declaring the Singleton
 *
 */
ConfigServer *ConfigServer::singleton = NULL;

/**
 * @brief Construct a new Config Server:: Config Server object
 *
 * @param paramFile the config file path
 */
void ConfigServer::setConfigServer(std::string paramFile)
{
	std::string file = getFile(paramFile);
	std::string stringFile = "";
	if (!readFile(file, stringFile))
	{
		_goodFile = false;
		throw std::runtime_error("Couldn't read!");
	}
	else
	{
		_goodFile = true;
		setConfigString(stringFile);
		setServerBlocks(); // Assign Directive ligne into each ServerBlock element
		std::vector<ServerBlocks> serverBlocks = getServerBlocks();
		setServersData(serverBlocks); // Assign every directive value to the right ServerData Structure element

		// printServerBlocks();

		// TODO COMMENT THIS TO STOP PRINTING
		// std::vector<ServerData> data = getServerData();
		// printServersData(data);

		// std::cout << "getConfigString(): " << getConfigString() << std::endl;
	}
}

/**
 * @brief Destroy the Config Server:: Config Server object
 *
 */
ConfigServer::~ConfigServer()
{
	// Destroy the Singlton
	destroy();
}

/**
 * @brief
 *
 * @param config
 * @return ConfigServer&
 */
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
 * @brief Set the Config String object
 *
 */
void ConfigServer::setConfigString(const std::string &configStr)
{
	this->_configString = configStr;
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
		// while (j > i && std::isspace(static_cast<int>(str[j])))
		j--;
	if (commentPos != string::npos && commentPos > static_cast<size_t>(i))
		j = static_cast<int>(commentPos - 1);
	std::string ret = line.substr(i, j - i + 1);
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
 * @param stringLine The std::string to store the data
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
			stringLine += cleanedLine(line);
	}
	file.close();
	return (true);
}

/**
 * @brief Get the Config String object
 *
 * @return std::string the private Config string
 */
std::string ConfigServer::getConfigString()
{
	return (this->_configString);
}

/**
 * @brief Get the Server Blocks private member
 *
 * @return std::vector<ServerBlocks> the server blocks vector
 */
std::vector<ServerBlocks> ConfigServer::getServerBlocks() const
{
	return (this->_serverBlocks);
}

/**
 * @brief Get the server data private member
 *
 * @return std::vector<ServerData> the server data vector
 */
std::vector<ServerData> ConfigServer::getServerData() const
{
	return this->_serversData;
}

/**
 * @brief Split configStr where all file is stored into blocks of servers
 *
 * @param configStr the std::string where readfile() stored the file it read.
 * @return std::vector<string>  A vector of strings one by server
 */
std::vector<string> ConfigServer::getServerBlocksData(const std::string &configStr)
{
	int braceCount;

	std::vector<string> serverBlocks;
	string::size_type pos = 0;
	if (!validBraces(configStr))
		exit_error("Error: braces not even! |", "");
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
				std::string serverBlock = configStr.substr(pos, endPos - pos);
				serverBlocks.push_back(serverBlock);
				pos = endPos;
				this->_serverNumber++;
				continue;
			}
		}
		else
			exit_error("Error: opening brace not found! ", "");
	}
	return (serverBlocks);
}

/**
 * @brief Split configStr where the server blocks are stored into blocks of location
 *
 * @param configStr the std::string every block server
 * @return std::vector<string>  A vector of strings one by location
 */
std::vector<string> ConfigServer::getLocationBlocks(std::string &configStr)
{
	std::vector<string> locationBlocks;
	string::size_type pos = 0;
	while ((pos = configStr.find("location ", pos)) != string::npos)
	{
		string::size_type endPos = configStr.find("}", pos);
		if (endPos == string::npos)
			break; // Handle error: closing brace not found
		std::string locationBlock = configStr.substr(pos, endPos - pos + 1);
		locationBlocks.push_back(locationBlock);
		configStr.erase(pos, endPos - pos + 1);
	}
	return (locationBlocks);
}

/**
 * @brief Get every part of every location block
 *
 * @param locations the vector of strings where every location block is saved
 * @return std::vector<ServerLocation> the vector of location data is saved
 */
std::vector<ServerLocation> ConfigServer::getLocationPart(std::vector<std::string> locations)
{
	std::vector<ServerLocation> locationPart(locations.size());

	for (size_t i = 0; i < locations.size(); i++)
	{
		std::string confStr = locations[i];
		locationPart[i]._path = getLocationPath(confStr);
		locationPart[i]._root = getDirective(confStr, " root ");
		locationPart[i]._index = getDirective(confStr, " index ");
		locationPart[i]._autoindex = getDirective(confStr, " autoindex ");
		locationPart[i]._methods = getDirective(confStr, " methods ");
		locationPart[i]._redirection = getDirective(confStr, " return ");
		validRemaining(confStr);
	}
	return (locationPart);
}

/**
 * @brief Get the location directive line
 *
 * @param configStr the string where the directive line is saved
 * @param directive the directive to get from configStr
 * @returnstd::vector<std::string> the vector of strings of the directive
 */
std::vector<std::string> ConfigServer::getDirective(std::string &configStr, std::string directive)
{
	std::vector<std::string> directives;
	std::string::size_type pos = 0;
	int present = 0;
	while ((pos = configStr.find(directive, pos)) != std::string::npos)
	{
		std::string::size_type endPos = configStr.find(";", pos);
		if (endPos != std::string::npos)
		{
			std::string directivesStr = configStr.substr(pos, endPos - pos + 1);
			directives.push_back(directivesStr);
			configStr.erase(pos, endPos - pos + 1);
			// continue;
		}
		else
			exit_error("Error: ; not found! |", "");
		present++;
	}
	if (present && directives.empty())
		exit_error("Error: Directive: " + directive + " present but value is empty!", "");
	return directives;
}

/**
 * @brief Get the location directive value
 *
 * @param configStr the string where the directive line is saved
 * @param directive the directive value to get from configStr
 * @returnstd::vector<std::string> the vector of strings of the directive values
 */
std::vector<std::string> ConfigServer::getDirectiveVal(const std::string &configStr, std::string directive)
{
	std::vector<std::string> valList;
	size_t pos = configStr.find(directive);
	int present = 0;
	if (pos != std::string::npos)
	{
		pos += directive.length();
		size_t endPos = configStr.find(';', pos);
		if (endPos != std::string::npos)
		{
			std::string valStr = configStr.substr(pos, endPos - pos);
			std::stringstream ss(valStr);
			std::string val;
			while (ss >> val)
			{
				valList.push_back(val);
			}
		}
		present++;
	}
	if (present && valList.empty())
		exit_error("Error: Directive:   |" + directive + "|   present but value is empty!", "");
	if (directive == " root ")
	{
		if (valList.size() != 1)
			exit_error("Error: Too many or no root |", "|");
		valList.at(0) = format_string(valList.at(0));
	}
	if (directive == " body_size ")
	{
		if (valList.size() != 1)
			exit_error("Error: Too many or no body_size value |", "|");
	}
	if (directive == " autoindex " && present)
	{
		if (valList.size() != 1 || !validAutoindex(valList.at(0)))
			exit_error("Error: Bad or no autoIndex |", valList.at(0) + "|");
	}
	return valList;
}

/**
 * @brief Get the hosts from the listen directive
 *
 * @param configStr the string where the directive line is saved
 * @return std::vector<std::string> the vector of strings of the hosts
 */
std::vector<std::string> ConfigServer::getHosts(const std::string &configStr)
{
	std::vector<std::string> hosts;
	std::istringstream iss(configStr);
	std::string token;

	while (iss >> token)
	{
		size_t colonPos = token.find(":");
		if (colonPos != std::string::npos)
		{
			std::string host = token.substr(0, colonPos);
			if (!validHost(host))
				exit_error("Error: Invalid Host |", host + "|");
			hosts.push_back(host);
		}
	}
	return hosts;
}

/**
 * @brief Get the ports from the listen directive
 *
 * @param configStr the string where the directive line is saved
 * @return std::vector<std::string> the vector of strings of the ports
 */
std::vector<int> ConfigServer::getPorts(const std::string &configStr)
{
	std::vector<int> ports;

	std::istringstream iss(configStr);
	std::string token;

	while (iss >> token)
	{
		if (token.find(':') != std::string::npos)
		{
			// Token contains a colon, so extract port number
			size_t colon_pos = token.find(':');
			std::string port_str = token.substr(colon_pos + 1);
			int port = atoi(port_str.c_str());
			if (!validPort(port))
				exit_error("Error: Invalid Port |", port_str + "|");
			ports.push_back(port);
		}
		else if (isdigit(token[0]))
		{
			// Token is a standalone port number
			int port = atoi(token.c_str());
			if (!validPort(port))
				exit_error("Error: Invalid Port |", token + "|");
			ports.push_back(port);
		}
	}
	return ports;
}

/**
 * @brief Get the error pages from the error_page directive
 *
 * @param configStr the string where the directive line is saved
 * @return std::map<int, std::string> the map of error codes and error pages
 */
std::map<int, std::string> ConfigServer::getErrorPages(const std::string &configStr)
{
	std::map<int, std::string> errorPages;
	std::istringstream iss(configStr);
	std::string line;
	while (std::getline(iss, line))
	{
		if (line.substr(0, 12) == " error_page ")
		{
			std::istringstream lineIss(line.substr(11));
			int errorCode;
			std::string errorPage;
			while (lineIss >> errorCode >> errorPage)
			{
				// Remove semicolon from error page if present
				if (errorPage.size() > 0 && errorPage[errorPage.size() - 1] == ';')
					errorPage = errorPage.substr(0, errorPage.size() - 1);
				if (!validErrorPage(errorCode, errorPage))
				{
					std::stringstream ss;
					ss << errorCode;
					exit_error("Error: Invalid Error Code |", ss.str() + "|");
				}
				// exit_error("Error: Invalid Error Code |", std::to_string(errorCode) + "|");
				errorPages[errorCode] = errorPage;
			}
		}
	}
	return errorPages;
}

/**
 * @brief Get every element from location and set it
 *
 * @param locstd::string the input, should be the sting location block
 * @return Locations the structur of location elements.
 */
Locations ConfigServer::settingLocation(ServerLocation &locStruct, ServerData &serverBlock)
{
	Locations location;
	// // ROOT
	for (size_t i = 0; i < locStruct._root.size(); i++)
	{
		std::vector<std::string> roots = getDirectiveVal(locStruct._root[i], " root ");
		location.root = roots[0]; // TODO Check how many roots and if valid
		if (location.root.empty())
			location.root = serverBlock._root[0];
	}

	// // INDEXES
	for (size_t i = 0; i < locStruct._root.size(); i++)
	{
		std::vector<std::string> indexes = getDirectiveVal(locStruct._index[i], " index ");
		for (size_t i = 0; i < indexes.size(); i++)
		{
			validIndex(indexes[i], location.root);
		}
		location.index = indexes; // TODO Check how many roots and if valid
		if (location.index.empty())
			location.index = serverBlock._index;
	}

	// // AUTOINDEX
	for (size_t j = 0; j < locStruct._autoindex.size(); j++)
	{
		std::vector<std::string> autoIndex = getDirectiveVal(locStruct._autoindex[j], " autoindex ");
		if (!autoIndex.empty())
			location.autoindex = autoIndex[0];
		
	}

	// // REDIRECTION
	for (size_t j = 0; j < locStruct._redirection.size(); j++)
	{
		std::vector<std::string> redirection = getDirectiveVal(locStruct._redirection[j], " return ");
		for (size_t i = 1; i < redirection.size(); i++)
		{
			validIndex(redirection[i], location.root);
		}
		if (!redirection.empty())
			location.redirection = redirection[0] + " " + redirection[1];
	}

	// // METHODS
	for (size_t j = 0; j < locStruct._methods.size(); j++)
	{
		std::vector<std::string> methods = getDirectiveVal(locStruct._methods[j], " methods ");
		location.methods = validMethods(methods);
	}

	if (location.methods.empty())
		location.methods = serverBlock._methods;

	return location;
}

/**
 * @brief Save directives from the file into ServerBlocks to parse from
 *
 */
void ConfigServer::setServerBlocks()
{
	std::vector<std::string> blocks = getServerBlocksData(getConfigString());
	std::vector<ServerBlocks> serverBlocks(blocks.size());
	for (size_t i = 0; i < blocks.size(); i++)
	{
		std::string confStr = blocks[i];
		std::vector<std::string> locationBlocks = getLocationBlocks(confStr);
		serverBlocks[i]._locations = getLocationPart(locationBlocks);
		serverBlocks[i]._listen = getDirective(confStr, " listen ");
		serverBlocks[i]._serverNames = getDirective(confStr, " server_name ");
		serverBlocks[i]._root = getDirective(confStr, " root ");
		serverBlocks[i]._methods = getDirective(confStr, " methods ");
		serverBlocks[i]._index = getDirective(confStr, " index ");
		serverBlocks[i].bodySize = getDirective(confStr, " body_size ");
		serverBlocks[i]._errorPages = getDirective(confStr, " error_page ");
		validRemaining(confStr); // Validate that all that's remaining is: "server{}"
	}
	this->_serverBlocks = serverBlocks;
}

/**
 * @brief Set the server data one by one after getting each element
 *
 * @param the std::vector<string> &serverBlocks to use to set the data
 */
void ConfigServer::setServersData(std::vector<ServerBlocks> &serverBlocks)
{
	std::vector<ServerBlocks> blocks = serverBlocks;
	std::vector<ServerData> servers(blocks.size());

	for (int i = 0; i < static_cast<int>(blocks.size()); i++)
	{
		// LISTEN:: GET HOSTS AND PORTS
		for (int j = 0; j < static_cast<int>(blocks[i]._listen.size()); j++)
		{
			// std::cout << "Listen: " << blocks[i]._listen[j] << std::endl;
			std::vector<std::string> hosts = getHosts(blocks[i]._listen[j]);
			for (int n = 0; n < static_cast<int>(hosts.size()); n++)
			{
				servers[i]._hosts.push_back(hosts[n]);
			}
			std::vector<int> ports = getPorts(blocks[i]._listen[j]);
			for (int x = 0; x < static_cast<int>(ports.size()); x++)
			{
				servers[i]._serverPorts.push_back(ports[x]);
			}
		}

		// SERVER NAMES
		for (int j = 0; j < static_cast<int>(blocks[i]._serverNames.size()); j++)
		{
			std::vector<std::string> serverNames = getDirectiveVal(blocks[i]._serverNames[j], " server_name ");
			for (int n = 0; n < static_cast<int>(serverNames.size()); n++)
			{
				servers[i]._serverNames.push_back(serverNames[n]);
			}
		}

		// ROOT
		for (int j = 0; j < static_cast<int>(blocks[i]._root.size()); j++)
		{
			std::vector<std::string> roots = getDirectiveVal(blocks[i]._root[j], " root ");
			for (int n = 0; n < static_cast<int>(roots.size()); n++)
			{
				servers[i]._root.push_back(roots[n]);
			}
		}

		// INDEXES
		for (int j = 0; j < static_cast<int>(blocks[i]._index.size()); j++)
		{
			std::vector<std::string> indexes = getDirectiveVal(blocks[i]._index[j], " index ");
			for (int n = 0; n < static_cast<int>(indexes.size()); n++)
			{
				validIndex(indexes[n], servers[i]._root[0]);
				servers[i]._index.push_back(indexes[n]);
			}
		}

		// BODY_SIZE
		for (int j = 0; j < static_cast<int>(blocks[i]._index.size()); j++)
		{
			std::vector<std::string> body = getDirectiveVal(blocks[i].bodySize[j], " body_size ");
			for (int n = 0; n < static_cast<int>(body.size()); n++)
			{
				validBodySize(body[n]);
				servers[i].bodySize.push_back(body[n]);
			}
		}

		// METHODS
		for (int j = 0; j < static_cast<int>(blocks[i]._methods.size()); j++)
		{
			std::vector<std::string> methods = getDirectiveVal(blocks[i]._methods[j], " methods ");
			servers[i]._methods = validMethods(methods);
		}

		// ERROR_PAGES
		for (int j = 0; j < static_cast<int>(blocks[i]._errorPages.size()); j++)
		{
			std::map<int, std::string> errorPages = getErrorPages(blocks[i]._errorPages[j]);
			for (std::map<int, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); ++it)
			{
				servers[i]._errorPages.insert(*it);
			}
		}

		// SETTING LOCATIONS
		std::vector<std::string> paths;
		for (size_t j = 0; j < blocks[i]._locations.size(); j++)
		{
			paths.push_back(blocks[i]._locations[j]._path);
			if (pathDup(paths))
				exit_error("Error: Duplicate Path |", blocks[i]._locations[j]._path + "|");
			servers[i]._locations[blocks[i]._locations[j]._path] = settingLocation(blocks[i]._locations[j], servers[i]);
		}

		// MORE NECESSARY CHECKS
		if (portDup(servers[i]._serverPorts))
		{
			std::stringstream ss;
			ss << servers[i]._serverPorts[0];
			exit_error("Error: Invalid Error Code |", ss.str() + "|");
			// exit_error("Error: Duplicate Port |", std::to_string(servers[i]._serverPorts[0]) + "|");
		}

		// if(!validFilePath(servers[i]._root[0] + "/" + servers[i]._index[0])) // TODO to fix find a better way
		// 	exit_error("Error: Invalid File Path |",
		// 	servers[i]._root[0] + "/" + servers[i]._index[0] + "|");
	}
	this->_serversData = servers;
}

/**
 * @brief Print the Server Blocks data
 *
 */
void ConfigServer::printServerBlocks()
{
	std::vector<ServerBlocks> data = getServerBlocks();
	for (std::vector<ServerBlocks>::iterator it = data.begin(); it != data.end(); ++it)
	{

		std::cout << YELLOW << "Listens: ";
		for (std::vector<std::string>::iterator itListen = it->_listen.begin(); itListen != it->_listen.end(); ++itListen)
		{
			std::cout << RESET << "|" << *itListen << "|"
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

		std::cout << YELLOW << "Root: ";
		for (std::vector<std::string>::iterator itRoot = it->_root.begin(); itRoot != it->_root.end(); ++itRoot)
		{
			std::cout << RESET << "|" << *itRoot << "|"
					  << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Methods: ";
		for (std::vector<std::string>::iterator itMethods = it->_methods.begin(); itMethods != it->_methods.end(); ++itMethods)
		{
			std::cout << RESET << "|" << *itMethods << "|"
					  << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Index: ";
		for (std::vector<std::string>::iterator itIndex = it->_index.begin(); itIndex != it->_index.end(); ++itIndex)
		{
			std::cout << RESET << "|" << *itIndex << "|"
					  << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Error pages: ";
		for (std::vector<std::string>::iterator itErrorPages = it->_errorPages.begin(); itErrorPages != it->_errorPages.end(); ++itErrorPages)
		{
			std::cout << RESET << "|" << *itErrorPages << "|"
					  << " ";
		}
		std::cout << std::endl;

		// std::cout << YELLOW << "Locations: ";
		// for (std::vector<std::string>::iterator itLocations = it->_locations.begin(); itLocations != it->_locations.end(); ++itLocations)
		// {
		// 	std::cout << RESET << "|" << *itLocations << "|"
		// 			  << " ";
		// }
		std::cout << YELLOW << "Locations: ";
		for (size_t i = 0; i < it->_locations.size(); i++)
		{
			std::cout << RESET << "|" << it->_locations[i]._root[0] << "|"
					  << " ";
		}
		std::cout << RESET << std::endl
				  << std::endl;
	}
}

/**
 * @brief Print the server data one by one
 *
 * @param data the vector<ServerData> to print
 */
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

		std::cout << YELLOW << "Root: ";
		for (std::vector<std::string>::iterator itRoot = it->_root.begin(); itRoot != it->_root.end(); ++itRoot)
		{
			std::cout << RESET << "|" << *itRoot << "|"
					  << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Index: ";
		for (std::vector<std::string>::iterator itIndex = it->_index.begin(); itIndex != it->_index.end(); ++itIndex)
		{
			std::cout << RESET << "|" << *itIndex << "|"
					  << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Error pages: ";
		for (std::map<int, std::string>::iterator itErrorPages = it->_errorPages.begin(); itErrorPages != it->_errorPages.end(); ++itErrorPages)
		{
			std::cout << RESET << "|" << itErrorPages->first << ": " << itErrorPages->second << "|"
					  << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Locations: " << std::endl;
		for (std::map<std::string, struct Locations>::iterator itLocations = it->_locations.begin();
			 itLocations != it->_locations.end(); ++itLocations)
		{
			std::cout << BOLD_MAGENTA << "|Root:" << RESET << itLocations->first << ": |"
					  << itLocations->second.root << "|" << std::endl;
			std::cout << BOLD_MAGENTA << "|Autoindex:" << RESET << itLocations->first << ": |"
					  << itLocations->second.autoindex << "|" << std::endl;
			std::cout << BOLD_MAGENTA << "|Redirection: " << RESET << "|" << itLocations->first << ": "
					  << itLocations->second.redirection << "|"
					  << " " << std::endl;

			std::cout << BOLD_MAGENTA << "|Methods: ";
			for (std::vector<enum eRequestType>::iterator itMethods = itLocations->second.methods.begin();
				 itMethods != itLocations->second.methods.end(); ++itMethods)
			{
				std::cout << RESET << "|" << *itMethods << "|"
						  << " ";
			}
			std::cout << std::endl;

			std::cout << BOLD_MAGENTA << "|Index: ";
			for (int n = 0; n < static_cast<int>(itLocations->second.index.size()); n++)
			{
				std::cout << RESET << " |" << itLocations->second.index[n] << "| ";
			}
			std::cout << std::endl
					  << std::endl;
		}
		std::cout << std::endl;
	}
}

