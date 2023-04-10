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
		std::vector<ServerData> data = getServerData();
		printServersData(data);

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









void ConfigServer::setConfigString(const std::string &configStr)
{
	this->_configString = configStr;
}

std::string ConfigServer::getConfigString()
{
	return (this->_configString);
}

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
		std::cout << BOLD_RED << "Error: opening brace not found! " << RESET << std::endl;
	}
	return (serverBlocks);
}

void ConfigServer::setServerBlocks()
{
	// std::string confStr = getConfigString();
	std::vector<std::string> blocks = getServerBlocksData(getConfigString());
	std::vector<ServerBlocks> serverBlocks(blocks.size());
	for (size_t i = 0; i < blocks.size(); i++)
	{
		std::string confStr = blocks[i];
		// std::cout << "confStr: " << confStr << std::endl;
		serverBlocks[i]._locations = getLocationBlocks(confStr);
		serverBlocks[i]._listen = getDirective(confStr, "listen");
		serverBlocks[i]._serverNames = getDirective(confStr, "server_name");
		serverBlocks[i]._root = getDirective(confStr, "root");
		serverBlocks[i]._methods = getDirective(confStr, "methods");
		serverBlocks[i]._index = getDirective(confStr, "index");
		serverBlocks[i]._errorPages = getDirective(confStr, "error_page");

		// std::cout << "confStr: " << confStr << std::endl;
	}
	// return(serverBlocks);
	this->_serverBlocks = serverBlocks;
}


std::vector<std::string> ConfigServer::getDirective(std::string &configStr, std::string directive)
{
	std::vector<std::string> directives;
	std::string::size_type pos = 0;
	while ((pos = configStr.find(directive, pos)) != std::string::npos)
	{
		std::string::size_type endPos = configStr.find(";", pos);
		if (endPos != std::string::npos)
		{
			std::string directivesStr = configStr.substr(pos, endPos - pos + 1);
			directives.push_back(directivesStr);
			configStr.erase(pos, endPos - pos + 1);
			continue;
		}
		std::cout << BOLD_RED << "Error: opening brace not found! " << RESET << std::endl;
	}
	return directives;
}

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
	// setConfigString(configStr);
	return (locationBlocks);
}

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

		std::cout << YELLOW << "Locations: ";
		for (std::vector<std::string>::iterator itLocations = it->_locations.begin(); itLocations != it->_locations.end(); ++itLocations)
		{
			std::cout << RESET << "|" << *itLocations << "|"
					  << " ";
		}
		std::cout << RESET << std::endl
				  << std::endl;
	}
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
			std::vector<std::string> serverNames = getDirectiveVal(blocks[i]._serverNames[j], "server_name");
			for (int n = 0; n < static_cast<int>(serverNames.size()); n++)
			{
				servers[i]._serverNames.push_back(serverNames[n]);
			}
		}

		// ROOT
		for (int j = 0; j < static_cast<int>(blocks[i]._root.size()); j++)
		{
			std::vector<std::string> roots = getDirectiveVal(blocks[i]._root[j], "root");
			for (int n = 0; n < static_cast<int>(roots.size()); n++)
			{
				servers[i]._root.push_back(roots[n]);
			}
		}

		// INDEXES
		for (int j = 0; j < static_cast<int>(blocks[i]._index.size()); j++)
		{
			std::vector<std::string> indexes = getDirectiveVal(blocks[i]._index[j], "index");
			for (int n = 0; n < static_cast<int>(indexes.size()); n++)
			{
				servers[i]._index.push_back(indexes[n]);
			}
		}

		// METHODS
		for (int j = 0; j < static_cast<int>(blocks[i]._methods.size()); j++)
		{
			std::vector<std::string> methods = getDirectiveVal(blocks[i]._methods[j], "methods");
			servers[i]._methods = validMethods(methods);
		}

		// ERROR_PAGES

		for (int j = 0; j < static_cast<int>(blocks[i]._errorPages.size()); j++)
		{
			//  std::cout << "blocks[j]._errorPages: |" << blocks[i]._errorPages[j] << std::endl;
			std::map<int, std::string> errorPages = getErrorPages(blocks[i]._errorPages[j]);
			for (std::map<int, std::string>::iterator it = errorPages.begin(); it != errorPages.end(); ++it)
			{
				// servers[i]._errorPages.push_back(it->second);
				servers[i]._errorPages.insert(*it);
			}
		}


		// if(!validFilePath(servers[i]._root[0] + "/" + servers[i]._index[0]))
		// 	exit_error("Error: Invalid File Path |",
		// 	servers[i]._root[0] + "/" + servers[i]._index[0] + "|");


		// SETTING LOCATIONS
		for (int j = 0; j < static_cast<int>(blocks[i]._locations.size()); j++)
		{
			std::vector<string> locations = getLocationBlocks(blocks[i]._locations[j]);
			for (int n = 0; n < static_cast<int>(locations.size()); n++)
			{
				std::string path = getLocationPath(locations[n]);
				servers[i]._locations[path] = settingLocation(locations[n], servers[i]);
			}
		}



		// MORE NECESSARY CHECKS
		if(portDup(servers[i]._serverPorts))
			exit_error("Error: Duplicate Port |", std::to_string(servers[i]._serverPorts[0]) + "|");
	}

	this->_serversData = servers;
}


std::vector<std::string> ConfigServer::getHosts(const std::string &configStr)
{
    std::vector<std::string> hosts;
    std::istringstream iss(configStr);
    std::string token;

    while (iss >> token) {
        size_t colonPos = token.find(":");
        if (colonPos != std::string::npos) {
            std::string host = token.substr(0, colonPos);
			if(!validHost(host))
				exit_error("Error: Invalid Host |", host + "|");
            hosts.push_back(host);
        }
    }
    return hosts;
}

std::vector<int> ConfigServer::getPorts(const std::string &configStr)
{
    std::vector<int> ports;

    std::istringstream iss(configStr);
    std::string token;

    while (iss >> token) 
	{
        if (token.find(':') != std::string::npos) {
            // Token contains a colon, so extract port number
            size_t colon_pos = token.find(':');
            std::string port_str = token.substr(colon_pos + 1);
            int port = atoi(port_str.c_str());
			if(!validPort(port))
				exit_error("Error: Invalid Port |", port_str + "|");
            ports.push_back(port);
        }
		else if (isdigit(token[0])) 
		{
            // Token is a standalone port number
            int port = atoi(token.c_str());
			if(!validPort(port))
				exit_error("Error: Invalid Port |", token + "|");
            ports.push_back(port);
        }
    }
    return ports;
}

std::map<int, std::string> ConfigServer::getErrorPages(const std::string &configStr)
{
    std::map<int, std::string> errorPages;
    std::istringstream iss(configStr);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.substr(0, 10) == "error_page") 
		{
            std::istringstream lineIss(line.substr(11));
            int errorCode;
            std::string errorPage;
            while (lineIss >> errorCode >> errorPage) {
                // Remove semicolon from error page if present
                if (errorPage.size() > 0 && errorPage[errorPage.size()-1] == ';') {
                    errorPage = errorPage.substr(0, errorPage.size()-1);
                }
				if(!validErrorPage(errorCode, errorPage))
					exit_error("Error: Invalid Error Code |", std::to_string(errorCode) + "|");
                errorPages[errorCode] = errorPage;
            }
        }
    }
    return errorPages;
}

std::vector<std::string> ConfigServer::getDirectiveVal(const std::string &configStr, std::string directive)
{
    std::vector<std::string> valList;
    size_t pos = configStr.find(directive);
	int present = 0;
    if (pos != std::string::npos) 
	{
        pos += directive.length();
        size_t endPos = configStr.find(';', pos);
        if (endPos != std::string::npos) {
            std::string valStr = configStr.substr(pos, endPos - pos);
            std::stringstream ss(valStr);
            std::string val;
            while (ss >> val) {
                valList.push_back(val);
            }
        }
		present++;
    }
	if(directive == "root")
	{
		if(valList.size() != 1)
			exit_error("Error: Too many or no root |", "|");
		valList.at(0) = format_string(valList.at(0));
	}
	if(directive == "autoIndex" && present)
	{
		if(valList.size() != 1 || !validAutoindex(valList.at(0)))
			exit_error("Error: Bad or no autoIndex |", "|");
	}
    return valList;
}

/**
 * @brief Get every element from location and set it
 *
 * @param locstd::string the input, should be the sting location block
 * @return Locations the structur of location elements.
 */
Locations ConfigServer::settingLocation(std::string &locString, ServerData &serverBlock)
{
	Locations location;
	std::vector<std::string> roots = getDirectiveVal(locString, "root");
	location.root = roots[0]; // TODO Check how many roots and if valid
	if (location.root.empty())
		location.root = serverBlock._root[0];
	location.index = getDirectiveVal(locString, "index");
	if (location.index.empty())
		location.index = serverBlock._index;
	std::vector<std::string> autoIndex = getDirectiveVal(locString, "autoIndex");
	if (!autoIndex.empty())
		location.autoindex = autoIndex[0];
	std::vector<std::string> redirection = getDirectiveVal(locString, "return");
	if (!redirection.empty())
		location.redirection = redirection[0];
	// std::map<int, std::string> redirection = getErrorPages(locString);
	// location.redirection  = getErrorPages(locString);
	// location.redirection = getStrValue(locString, "return"); // TODO check if the redirection path is valid
	std::vector<std::string> methods = getDirectiveVal(locString, "methods");
	location.methods = validMethods(methods);
	// location.methods = getMethods(locString);
	return location;
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
			std::cout << RESET << "|" << *itHosts << "|" << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Server ports: ";
		for (std::vector<int>::iterator itPorts = it->_serverPorts.begin(); itPorts != it->_serverPorts.end(); ++itPorts)
		{
			std::cout << RESET << "|" << *itPorts << "|" << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Server names: ";
		for (std::vector<std::string>::iterator itNames = it->_serverNames.begin(); itNames != it->_serverNames.end(); ++itNames)
		{
			std::cout << RESET << "|" << *itNames << "|" << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Methods: ";
		for (std::vector<enum eRequestType>::iterator itMethods = it->_methods.begin(); itMethods != it->_methods.end(); ++itMethods)
		{
			std::cout << RESET << "|" << *itMethods << "|" << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Root: ";
		for (std::vector<std::string>::iterator itRoot = it->_root.begin(); itRoot != it->_root.end(); ++itRoot)
		{
			std::cout << RESET << "|" << *itRoot << "|" << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Index: ";
		for (std::vector<std::string>::iterator itIndex = it->_index.begin(); itIndex != it->_index.end(); ++itIndex)
		{
			std::cout << RESET << "|" << *itIndex << "|" << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Error pages: ";
		for (std::map<int, std::string>::iterator itErrorPages = it->_errorPages.begin(); itErrorPages != it->_errorPages.end(); ++itErrorPages)
		{
			std::cout << RESET << "|" << itErrorPages->first << ": " << itErrorPages->second << "|"  << " ";
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
					  << itLocations->second.redirection << "|"  << " " << std::endl;

			std::cout << BOLD_MAGENTA << "|Methods: ";
			for (std::vector<enum eRequestType>::iterator itMethods = itLocations->second.methods.begin();
					itMethods != itLocations->second.methods.end(); ++itMethods)
			{
				std::cout << RESET << "|" << *itMethods << "|" << " ";
			}
			std::cout << std::endl;
			
			std::cout << BOLD_MAGENTA << "|Index: ";
			for (int n = 0; n < static_cast<int>(itLocations->second.index.size()); n++)
			{
				std::cout << RESET << " |" << itLocations->second.index[n] << "| " ;
			}
			std::cout << std::endl << std::endl;
		}
		std::cout << std::endl;
	}
}























































/**
 * @brief Split the server block into a std::string of location
 *
 * @param configStr the input, should be the sting server block
 * @return std::vector<string> the vector of locations.
 */
// std::vector<string> ConfigServer::getLocationBlocks(const std::string &configStr)
// {
// 	std::vector<string> locationBlocks;
// 	string::size_type pos = 0;
// 	while ((pos = configStr.find("location ", pos)) != string::npos)
// 	{
// 		string::size_type endPos = configStr.find("}", pos);
// 		if (endPos == string::npos)
// 			break; // Handle error: closing brace not found
// 		std::string locationBlock = configStr.substr(pos, endPos - pos + 1);
// 		locationBlocks.push_back(locationBlock);
// 		pos = endPos + 1;
// 	}
// 	return (locationBlocks);
// }

/**
 * @brief Get the Hosts object
 *
 * @param configStr the input, should be the sting server block
 * @return std::vector<string> the vector of hosts.
 */
// std::vector<string> ConfigServer::getHosts(const std::string &configStr)
// {
// 	std::vector<string> hosts;
// 	string::size_type pos = configStr.find("listen ", 0);
// 	string::size_type newPos = 0;
// 	std::string hostLine;
// 	std::string host;
// 	string::size_type endPos;
// 	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
// 									 ? configStr.find("{", pos)
// 									 : configStr.find("}", pos);
// 	while (pos != string::npos && pos < bracePos)
// 	{
// 		endPos = configStr.find(";", pos);
// 		while (endPos != string::npos && pos < endPos &&
// 			   bracePos != string::npos && endPos < bracePos)
// 		{
// 			hostLine = trim(getHostLine(configStr, pos, &newPos));
// 			if (!hostLine.empty())
// 			{
// 				int i = 0, j = 0;
// 				while (hostLine[j])
// 					if (hostLine[j++] == ':')
// 						i++;
// 				j = 0;
// 				while (j < i)
// 				{
// 					host = getHostPart(hostLine);
// 					if (!validHost(host))
// 						exit_error(HOST_ERR, host);
// 					hosts.push_back(host);
// 					hostLine = hostLine.substr(hostLine.find(":", 0) + 1, hostLine.length() - hostLine.find(":", 0) - 1);
// 					j++;
// 				}
// 				pos = newPos;
// 			}
// 			endPos = configStr.find(";", pos);
// 			pos = configStr.find("listen ", pos);
// 		}
// 	}
// 	if (hosts.size() > 2)
// 		exit_error("Error: too many hosts ", "");
// 	return (hosts);
// }

/**
 * @brief Get the port from the config string
 *
 * @param configStr The block to get the port from
 * @return std::string the port value
 */
// std::vector<int> ConfigServer::getPorts(const std::string &configStr)
// {
// 	std::vector<int> ports;
// 	string::size_type pos = configStr.find("listen ", 0);
// 	string::size_type newPos = 0;
// 	std::string portLine;
// 	std::string strPort;
// 	int port;
// 	string::size_type endPos;
// 	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
// 									 ? configStr.find("{", pos)
// 									 : configStr.find("}", pos);
// 	std::vector<string> words;
// 	while (pos != string::npos && pos < bracePos)
// 	{
// 		endPos = configStr.find(";", pos);
// 		while (endPos != string::npos && pos < endPos &&
// 			   bracePos != string::npos && endPos < bracePos)
// 		{
// 			portLine = trim(getPortLine(configStr, pos, &newPos));
// 			if (!portLine.empty())
// 			{
// 				int i = 0, j = 0;
// 				while (portLine[j])
// 					if (portLine[j++] == ':')
// 						i++;
// 				j = 0;
// 				if (i) // get th port incase of the presence of ":"
// 				{
// 					while (j < i)
// 					{
// 						strPort = getPortPart(portLine);
// 						port = std::atoi(strPort.c_str());
// 						if (!validPort(port))
// 							exit_error(PORT_ERR, strPort);
// 						ports.push_back(port);
// 						portLine = portLine.substr(portLine.find(" ", 0) + 1, portLine.length() - portLine.find(":", 0) - 1);
// 						j++;
// 					}
// 				}
// 				else // get the port if the derective has no ":"
// 				{
// 					words = splitString(portLine);
// 					for (int i = 0; i < static_cast<int>(words.size()); i++) // loop until all words have been returned
// 					{
// 						port = std::atoi(words[i].c_str());
// 						if (!validPort(port))
// 							exit_error(PORT_ERR, words[i]);
// 						ports.push_back(port);
// 					}
// 				}
// 				pos = newPos;
// 			}
// 			endPos = configStr.find(";", pos);
// 			pos = configStr.find("listen ", pos);
// 		}
// 	}
// 	if (ports.size() < 1 || portDup(ports))
// 		exit_error("Error: no ports or duplicate ports ", "");
// 	return (ports);
// }

/**
 * @brief Get the value depeding on the directive from the config string
 *
 * @param configStr The block to get the directive value from
 * @return std::vector<string> of the directive values
 */
// std::vector<string> ConfigServer::getKeywordValue(const std::string &configStr, const std::string &directive)
// {
// 	std::vector<string> keyWord;
// 	string::size_type pos = configStr.find(directive, 0);
// 	string::size_type poSpace = 0;
// 	std::string word;
// 	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
// 									 ? configStr.find("{", pos)
// 									 : configStr.find("}", pos);
// 	while (pos != string::npos && pos < bracePos)
// 	{
// 		pos += directive.length(); // skip "keyWord "
// 		string::size_type endPos = configStr.find(";", pos);
// 		while (endPos != string::npos && pos < endPos && bracePos != string::npos && endPos < bracePos)
// 		{
// 			while (pos < endPos && isspace(configStr[pos]))
// 				pos++;
// 			poSpace = pos;
// 			while (poSpace < endPos && !isspace(configStr[poSpace]))
// 				poSpace++;
// 			word = configStr.substr(pos, poSpace - pos);
// 			if (word.empty())
// 				exit_error("Error: ", directive + " can't be empty!");
// 			keyWord.push_back(word);
// 			pos = poSpace;
// 			while (pos < endPos && isspace(configStr[pos]))
// 				pos++;
// 			endPos = configStr.find(";", pos);
// 		}
// 		pos = configStr.find(directive, pos);
// 	}
// 	if (directive == "root" && directive == "index" && keyWord.size() != 1)
// 		exit_error("Error: no ", directive + " or too many");
// 	if (directive == "root")
// 	{
// 		if (!validRoot(keyWord.at(0)))
// 			exit_error("Error: bad format |", directive + "|");
// 		keyWord.at(0) = format_string(keyWord.at(0));
// 	}
// 	return (keyWord);
// }

/**
 * @brief Get the value depeding on the directive from the config string
 *
 * @param configStr The block to get the directive from
 * @return std::string of the directive value
 */
// std::string ConfigServer::getStrValue(const std::string &configStr, const std::string &directive)
// {
// 	std::string keyWord;
// 	string::size_type pos = configStr.find(directive, 0);
// 	string::size_type poSpace = 0;
// 	std::string word;
// 	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
// 									 ? configStr.find("{", pos)
// 									 : configStr.find("}", pos);
// 	int presence = 0;
// 	while (pos != string::npos && pos < bracePos)
// 	{
// 		pos += directive.length(); // skip "keyWord "
// 		string::size_type endPos = configStr.find(";", pos);
// 		while (endPos != string::npos && pos < endPos && bracePos != string::npos && endPos < bracePos)
// 		{
// 			while (pos < endPos && isspace(configStr[pos]))
// 				pos++;
// 			poSpace = pos;
// 			while (poSpace < endPos && !isspace(configStr[poSpace]))
// 				poSpace++;
// 			word = configStr.substr(pos, poSpace - pos);
// 			if (word.empty())
// 				exit_error("Error: ", directive + " can't be empty!");
// 			keyWord = word;
// 			pos = poSpace;
// 			while (pos < endPos && isspace(configStr[pos]))
// 				pos++;
// 			endPos = configStr.find(";", pos);
// 		}
// 		pos = configStr.find(directive, endPos);
// 		presence++;
// 	}
// 	if (presence > 1)
// 		exit_error("Error: ", directive + " can't be more than one!");
// 	if (presence && directive == "autoindex" && !validAutoindex(keyWord))
// 		exit_error("Error: |", word + "| is not a valid autoindex!");
// 	return (keyWord);
// }

/**
 * @brief Get the methods from the config string
 *
 * @param configStr The block to get the methods from
 * @return std::string the methods value
 */
// std::vector<enum eRequestType> ConfigServer::getMethods(const std::string &configStr)
// {
// 	std::vector<enum eRequestType> methods;
// 	string::size_type pos = configStr.find("methods", 0);
// 	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
// 									 ? configStr.find("{", pos)
// 									 : configStr.find("}", pos);
// 	string::size_type newPos = 0;
// 	std::string wordLine;
// 	std::vector<string> words;
// 	while (pos != string::npos && pos < bracePos)
// 	{
// 		string::size_type endPos = configStr.find(";", pos);
// 		while (endPos != string::npos && endPos < bracePos && pos < endPos)
// 		{
// 			wordLine = trim(getMethodsLine(configStr, pos, &newPos));
// 			if (wordLine.empty())
// 				exit_error("Error: methods can't be empty!", "");
// 			words = splitString(wordLine);
// 			for (int i = 0; i < static_cast<int>(words.size()); i++) // loop until all words have been returned
// 			{
// 				if (words[i].empty())
// 					exit_error("Error: methods can't be empty!", "");
// 				else if (words[i] == "GET")
// 					methods.push_back(GET);
// 				else if (words[i] == "POST")
// 					methods.push_back(POST);
// 				else if (words[i] == "DELETE")
// 					methods.push_back(DELETE);
// 				else
// 					exit_error("Error: Invalid HTTP method: ", words[i]);
// 			}
// 			pos = newPos;
// 			endPos = configStr.find(";", pos);
// 		}
// 		pos = configStr.find("methods", pos);
// 	}
// 	return methods;
// }

/**
 * @brief Get the value depeding on the derective from the config string
 *
 * @param configStr The block to get the server name from
 * @return std::string the server name value
 */
// std::map<int, string> ConfigServer::getErrorPages(const std::string &configStr)
// {
// 	std::map<int, string> errorPages;
// 	string::size_type pos = 0;
// 	string::size_type brace;
// 	std::string errorStrCode;
// 	while ((pos = configStr.find("error_page", pos)) != string::npos)
// 	{
// 		pos += 10; // skip "error_page "
// 		string::size_type poSpace = pos;
// 		string::size_type endPos = configStr.find(";", pos);
// 		if (endPos != string::npos && (brace = configStr.find("{", endPos)) != string::npos && endPos < brace)
// 		{
// 			while (pos < endPos && isspace(configStr[pos]))
// 				pos++;
// 			poSpace = pos;
// 			while (poSpace < endPos && !isspace(configStr[poSpace]))
// 				poSpace++;
// 			errorStrCode = configStr.substr(pos, poSpace - pos);
// 			int errorCode = std::atoi(errorStrCode.c_str());
// 			if (errorCode < 0)
// 				exit_error("Error: Invalid error code: ", errorStrCode);
// 			pos = poSpace;
// 			if (endPos != string::npos)
// 			{
// 				std::string errorPage = configStr.substr(pos, endPos - pos);
// 				if (!validErrorPage(errorPage))
// 					exit_error("Error: error_page can't be empty!", "");
// 				errorPages[errorCode] = errorPage;
// 			}
// 		}
// 		pos = endPos + 1;
// 		brace = configStr.find("{", endPos);
// 	}
// 	return errorPages;
// };

/**
 * @brief Set the server data one by one after getting each element
 *
 * @param the std::vector<string> &serverBlocks to use to set the data
 */
// void ConfigServer::setServersData(std::vector<string> &serverBlocks)
// {
// 	std::vector<ServerData> servers(serverBlocks.size());
// 	for (int i = 0; i < static_cast<int>(serverBlocks.size()); i++)
// 	{
// 		servers[i]._hosts = getHosts(serverBlocks[i]);
// 		servers[i]._serverPorts = getPorts(serverBlocks[i]);
// 		servers[i]._serverPorts = getPorts(serverBlocks[i]);
// 		servers[i]._serverNames = getKeywordValue(serverBlocks[i], "server_name");
// 		servers[i]._methods = getMethods(serverBlocks[i]);
// 		servers[i]._errorPages = getErrorPages(serverBlocks[i]);
// 		servers[i]._root = getKeywordValue(serverBlocks[i], "root");
// 		servers[i]._index = getKeywordValue(serverBlocks[i], "index");
// 		servers[i]._errorPages = getErrorPages(serverBlocks[i]);
// 		if(!validFilePath(servers[i]._root[0] + "/" + servers[i]._index[0]))
// 			exit_error("Error: Invalid File Path |",
// 				servers[i]._root[0] + "/" + servers[i]._index[0] + "|");
// 		// Setting locations
// 		std::vector<string> locations = getLocationBlocks(serverBlocks[i]);
// 		std::vector<std::string> paths;
// 		for (int j = 0; j < static_cast<int>(locations.size()); j++)
// 		{
// 			std::string path = getLocationPath(locations[j]);;
// 			paths.push_back(path);
// 			if(!pathDup(paths))
// 				servers[i]._locations[path] = settingLocation(locations[j], servers[i]);
// 			else
// 				exit_error("Error: Location Path |", path + "| Already Exists!");
// 			validIndex(servers[i]._locations[path]);
// 		}
// 	}
// 	this->_serversData = servers;
// }
