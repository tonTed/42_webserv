#include "ConfigServer.hpp"

ConfigServer::ConfigServer()
	: _useDefault(true)
{
}

ConfigServer::ConfigServer(const ConfigServer &config)
	: _useDefault(true)
{
	*this = config;
}

ConfigServer::ConfigServer(const std::string file)
{
	std::cout << "File: " << file << std::endl;
}

ConfigServer::~ConfigServer()
{
}

ConfigServer &ConfigServer::operator=(const ConfigServer &config)
{
	if (this != &config)
	{
		// _servers = config._servers;
		_useDefault = config._useDefault;
	}
	return (*this);
}

/**
 * @brief Check if the file exists
 * 
 * @param file The path to the file
 * @return return the givent file Path if it works otherwise return the default one.
 */
std::string getFile(int argc, const char **argv)
{
	std::string configFile = "configFiles/default.config";
	if (argc == 2)
	{
		configFile = argv[1];
		std::ifstream f(configFile.c_str());
		return ((f.good()) ? configFile : "configFiles/default.config");
	}
	return (configFile);
}

/**
 * @brief Check if the line is neeeded and not just a comment or just spaces
 * 
 * @param file The path to the file
 * @return true if just a comment or just spaces
 * @return false otherwise
 */
bool ConfigServer::notNeeded(const std::string line)
{
	size_t	i;

	i = line.find_first_not_of(" \t");
	return (i == std::string::npos || line[i] == '#');
}

/**
 * @brief Remove the spaces and comments from the line
 * 
 * @param line The line to be modified
 * @return std::string The modified line
 */
std::string ConfigServer::removeSpaceComments(std::string line)
{
	int			i;
	int			j;
	const char	*str;
	size_t		commentPos;

	i = 0;
	j = static_cast<int>(line.length()) - 1;
	str = line.c_str();
	commentPos = line.find("#");
	while (i < j && std::isspace(static_cast<int>(str[i])))
		i++;
	while (j > i && std::isspace(static_cast<int>(str[j])))
		j--;
	if (commentPos != std::string::npos && commentPos > static_cast<size_t>(i))
	{
		j = static_cast<int>(commentPos - 1);
	}
	return (line.substr(i, j - i + 1) + "\n");
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
	if (!file.is_open())
		return (false);
	while (std::getline(file, line))
	{
		if (!notNeeded(line))
			stringLine += removeSpaceComments(line);
	}
	file.close();
	return (true);
}


// TEST_CASE("test ConfFIle param ")
// {
// 	ConfigServer config;
// 	ConfigServer config1;

// 	/* Check If file could be open // Should pass */
// 	CHECK(config.fileExists("configFiles/default.config") == 1);

// 	/* Check If file could be open // Should fail */
// 	CHECK(config1.fileExists("configFiles/default.configur") == 1);

// 	ConfigServer ConfigT("configFiles/default.config");
// 	ConfigServer ConfigT1("configFiles/default.configur");
// }