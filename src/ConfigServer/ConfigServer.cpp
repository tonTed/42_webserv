# include "ConfigServer.hpp"

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
		std::vector<string> serverBlocks = getServerBlocks(stringFile);
		for(size_t i = 0; i < serverBlocks.size(); i++)
		{
			std::cout << BOLD_YELLOW << "serverBlocks:  " << RESET << serverBlocks[i] << std::endl << std::endl;
		}
		setServersData(serverBlocks);

		// TODO COMMENT THIS TO STOP PRINTING
		std::vector<ServerData> data = getServerData();
		printServersData(data);
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
std::vector<string> ConfigServer::getServerBlocks(const std::string &configStr)
{
	int braceCount;

	std::vector<string> serverBlocks;
	string::size_type pos = 0;
	if(!validBraces(configStr))
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

/**
 * @brief Split the server block into a std::string of location
 *
 * @param configStr the input, should be the sting server block
 * @return std::vector<string> the vector of locations.
 */
std::vector<string> ConfigServer::getLocationBlocks(const std::string &configStr)
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
		pos = endPos + 1;
	}
	return (locationBlocks);
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
	location.root = getStrValue(locString, "root");				  // TODO Check how many roots and if valid
	if(location.root.empty())
		location.root = serverBlock._root[0];
	location.index = getKeywordValue(locString, "index");
	if(location.index.empty())
		location.index = serverBlock._index;
	location.autoindex = getStrValue(locString, "autoindex");
	location.redirection = getStrValue(locString, "return"); // TODO check if the redirection path is valid
	location.methods = getMethods(locString);
	return location;
}

/**
 * @brief Get the Hosts object
 *
 * @param configStr the input, should be the sting server block
 * @return std::vector<string> the vector of hosts.
 */
std::vector<string> ConfigServer::getHosts(const std::string &configStr)
{
	std::vector<string> hosts;
	string::size_type pos = configStr.find("listen ", 0);
	string::size_type newPos = 0;
	std::string hostLine;
	std::string host;
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
						exit_error(HOST_ERR, host);
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
		exit_error("Error: too many hosts ", "");
	return (hosts);
}

/**
 * @brief Get the port from the config string
 *
 * @param configStr The block to get the port from
 * @return std::string the port value
 */
std::vector<int> ConfigServer::getPorts(const std::string &configStr)
{
	std::vector<int> ports;
	string::size_type pos = configStr.find("listen ", 0);
	string::size_type newPos = 0;
	std::string portLine;
	std::string strPort;
	int port;
	string::size_type endPos;
	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
									 ? configStr.find("{", pos)
									 : configStr.find("}", pos);
	std::vector<string> words;

	while (pos != string::npos && pos < bracePos)
	{
		endPos = configStr.find(";", pos);
		while (endPos != string::npos && pos < endPos &&
			   bracePos != string::npos && endPos < bracePos)
		{
			portLine = trim(getPortLine(configStr, pos, &newPos));
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
						if(!validPort(port))
							exit_error(PORT_ERR, strPort);
						ports.push_back(port);
						portLine = portLine.substr(portLine.find(" ", 0) + 1, portLine.length() - portLine.find(":", 0) - 1);
						j++;
					}
				}
				else // get the port if the derective has no ":"
				{
					words = splitString(portLine);
					for (int i = 0; i < static_cast<int>(words.size()); i++) // loop until all words have been returned
					{
						port = std::atoi(words[i].c_str());
						if(!validPort(port))
							exit_error(PORT_ERR, words[i]);
						ports.push_back(port);
					}
				}
				pos = newPos;
			}
			endPos = configStr.find(";", pos);
			pos = configStr.find("listen ", pos);
		}
	}
	if (ports.size() < 1 || portDup(ports))
		exit_error("Error: no ports or duplicate ports ", "");
	return (ports);
}

/**
 * @brief Get the value depeding on the directive from the config string
 *
 * @param configStr The block to get the directive value from
 * @return std::vector<string> of the directive values
 */
std::vector<string> ConfigServer::getKeywordValue(const std::string &configStr, const std::string &directive)
{
	std::vector<string> keyWord;
	string::size_type pos = configStr.find(directive, 0);
	string::size_type poSpace = 0;
	std::string word;
	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
									 ? configStr.find("{", pos)
									 : configStr.find("}", pos);
	while (pos != string::npos && pos < bracePos)
	{
		pos += directive.length(); // skip "keyWord "
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
				exit_error("Error: ", directive + " can't be empty!");
			keyWord.push_back(word);
			pos = poSpace;
			while (pos < endPos && isspace(configStr[pos]))
				pos++;
			endPos = configStr.find(";", pos);
		}
		pos = configStr.find(directive, pos);
	}
	if (directive == "root" && directive == "index" && keyWord.size() != 1)
		exit_error("Error: no ", directive + " or too many");
	if (directive == "root"  && !validRoot(keyWord.at(0)))
		exit_error("Error: bad format |", directive + "|");
	return (keyWord);
}

/**
 * @brief Get the value depeding on the directive from the config string
 *
 * @param configStr The block to get the directive from
 * @return std::string of the directive value
 */
std::string ConfigServer::getStrValue(const std::string &configStr, const std::string &directive)
{
	std::string keyWord;
	string::size_type pos = configStr.find(directive, 0);
	string::size_type poSpace = 0;
	std::string word;
	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
									 ? configStr.find("{", pos)
									 : configStr.find("}", pos);
	int presence = 0;
	while (pos != string::npos && pos < bracePos)
	{
		pos += directive.length(); // skip "keyWord "
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
				exit_error("Error: ", directive + " can't be empty!");
			keyWord = word;
			pos = poSpace;
			while (pos < endPos && isspace(configStr[pos]))
				pos++;
			endPos = configStr.find(";", pos);
		}
		pos = configStr.find(directive, endPos);
		presence++;
	}
	if (presence > 1)
		exit_error("Error: ", directive + " can't be more than one!");
	if(presence && directive == "autoindex" && !validAutoindex(keyWord))
		exit_error("Error: |", word + "| is not a valid autoindex!");		
	return (keyWord);
}

/**
 * @brief Get the methods from the config string
 *
 * @param configStr The block to get the methods from
 * @return std::string the methods value
 */
std::vector<enum eRequestType> ConfigServer::getMethods(const std::string &configStr )
{
	std::vector<enum eRequestType> methods;
	string::size_type pos = configStr.find("methods", 0);
	string::size_type bracePos = configStr.find("{", pos) < configStr.find("}", pos)
									 ? configStr.find("{", pos)
									 : configStr.find("}", pos);
	string::size_type newPos = 0;
	std::string wordLine;
	std::vector<string> words;
	while (pos != string::npos && pos < bracePos)
	{
		string::size_type endPos = configStr.find(";", pos);
		while (endPos != string::npos && endPos < bracePos && pos < endPos)
		{
			wordLine = trim(getMethodsLine(configStr, pos, &newPos));
			if (wordLine.empty())
				exit_error("Error: methods can't be empty!", "");
			words = splitString(wordLine);
			for (int i = 0; i < static_cast<int>(words.size()); i++) // loop until all words have been returned
			{
				if (words[i].empty())
					exit_error("Error: methods can't be empty!","");
				else if (words[i] == "GET")
					methods.push_back(GET);
				else if (words[i] == "POST")
					methods.push_back(POST);
				else if (words[i] == "DELETE")
					methods.push_back(DELETE);
				else
					exit_error("Error: Invalid HTTP method: ", words[i]);
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
 * @return std::string the server name value
 */
std::map<int, string> ConfigServer::getErrorPages(const std::string &configStr)
{
	std::map<int, string> errorPages;
	string::size_type pos = 0;
	string::size_type brace;
	std::string errorStrCode;
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
			errorStrCode = configStr.substr(pos, poSpace - pos);
			int errorCode = std::atoi(errorStrCode.c_str());
			if (errorCode < 0)
				exit_error("Error: Invalid error code: ", errorStrCode);
			pos = poSpace;
			if (endPos != string::npos)
			{
				std::string errorPage = configStr.substr(pos, endPos - pos);
				if (!validErrorPage(errorPage))
					exit_error("Error: error_page can't be empty!", "");
				errorPages[errorCode] = errorPage;
			}
		}
		pos = endPos + 1;
		brace = configStr.find("{", endPos);
	}
	return errorPages;
};

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
			// std::cout << RESET << "|" << it->_root << "|"
			// 		  << " ";
		std::cout << std::endl;

		std::cout << YELLOW << "Error pages: ";
		for (std::map<int, std::string>::iterator itErrorPages = it->_errorPages.begin(); itErrorPages != it->_errorPages.end(); ++itErrorPages)
		{
			std::cout << RESET << "|" << itErrorPages->first << ": "
					  << itErrorPages->second << "|"
					  << " ";
		}
		std::cout << std::endl;

		std::cout << YELLOW << "Locations: " << std::endl;
		for (std::map<std::string, struct Locations>::iterator itLocations = it->_locations.begin(); itLocations != it->_locations.end(); ++itLocations)
		{
			std::cout << BOLD_MAGENTA << "|Root:" << RESET << itLocations->first << ": |"
					  << itLocations->second.root << "|" << std::endl;
			std::cout << BOLD_MAGENTA << "|Autoindex:" << RESET << itLocations->first << ": |"
					  << itLocations->second.autoindex << "|" << std::endl;
			std::cout << BOLD_MAGENTA << "|Redirection:" << RESET << itLocations->first << ": |"
					  << itLocations->second.redirection << "|" << std::endl;
			for (int n = 0; n < static_cast<int>(itLocations->second.index.size()); n++)
			{
				std::cout << BOLD_MAGENTA << "|Index:" << RESET << itLocations->second.index[n]
						  << "| " << std::endl;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}

/**
 * @brief Set the server data one by one after getting each element
 *
 * @param the std::vector<string> &serverBlocks to use to set the data
 */
void ConfigServer::setServersData(std::vector<string> &serverBlocks)
{
	std::vector<ServerData> servers(serverBlocks.size());
	
	for (int i = 0; i < static_cast<int>(serverBlocks.size()); i++)
	{
		
		servers[i]._hosts = getHosts(serverBlocks[i]);
		servers[i]._serverPorts = getPorts(serverBlocks[i]);
		servers[i]._serverPorts = getPorts(serverBlocks[i]);
		servers[i]._serverNames = getKeywordValue(serverBlocks[i], "server_name");
		servers[i]._methods = getMethods(serverBlocks[i]);
		servers[i]._errorPages = getErrorPages(serverBlocks[i]);
		servers[i]._root = getKeywordValue(serverBlocks[i], "root");
		servers[i]._index = getKeywordValue(serverBlocks[i], "index");
		servers[i]._errorPages = getErrorPages(serverBlocks[i]);

		// Setting locations
		std::vector<string> locations = getLocationBlocks(serverBlocks[i]);
		std::vector<std::string> paths;
		for (int j = 0; j < static_cast<int>(locations.size()); j++)
		{
			std::string path = getLocationPath(locations[j]);;
			paths.push_back(path);
			if(!pathDup(paths))
				servers[i]._locations[path] = settingLocation(locations[j], servers[i]);
			else
				exit_error("Error: Location Path |", path + "| Already Exists!");
			// std::cout << "Out !" << std::endl;
			validIndex(servers[i]._locations[path]);
		}
	}
	this->_serversData = servers;
}

