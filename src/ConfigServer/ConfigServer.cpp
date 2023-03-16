#include "ConfigServer.hpp"


ConfigServer::ConfigServer()
{
}


ConfigServer::ConfigServer(const std::string file)
{
	if (!fileExists(file))
	{
		throw std::runtime_error("File does not exist");
	}
}

ConfigServer::~ConfigServer()
{
}

bool ConfigServer::fileExists(const std::string &file)
{
	std::ifstream f(file.c_str());
	return (f.good());
}



TEST_CASE("test ConfFIle param ") {

    ConfigServer config;
    ConfigServer Config1;

    /* Check If file could be open // Should pass */
    CHECK(config.fileExists("configFiles/default.config") == 1);

     /* Check If file could be open // Should fail */
    CHECK(config.fileExists("configFiles/default.configur") == 1);

    // ConfigServer Config("configFiles/default.config");
    // ConfigServer Config1("configFiles/default.configur");
}