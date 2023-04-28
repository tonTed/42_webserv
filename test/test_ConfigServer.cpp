
#include "test_header.hpp"

TEST_CASE("ConfigServer::lineNeeded")
{
    SUBCASE("Returns false for an empty line")
    {
        std::string line = "";
        CHECK_FALSE(lineNeeded(line));
    }
    SUBCASE("Returns false for a line with only whitespace")
    {
        std::string line = "   \t\t ";
        CHECK_FALSE(lineNeeded(line));
    }
    SUBCASE("Returns false for a commented line")
    {
        std::string line = "# this is a comment";
        CHECK_FALSE(lineNeeded(line));
    }
    SUBCASE("Returns true for a line with non-whitespace characters")
    {
        std::string line = "listen 127.0.0.1:8080";
        CHECK(lineNeeded(line));
    }
    SUBCASE("Returns true for a line with leading whitespace")
    {
        std::string line = "\t    listen 127.0.0.1:8080";
        CHECK(lineNeeded(line));
    }
}

TEST_CASE("ConfigServer class")
{
    ConfigServer::getInstance()->setConfigServer("test/test_config.conf");
    ConfigServer *cs = ConfigServer::getInstance();
    std::vector<ServerData> serversData;

    SUBCASE("getServerData")
    {
        serversData = cs->getServerData();

        CHECK(serversData.size() == 2);

        CHECK(serversData[0]._serverNames[0] == "server1");
        CHECK(serversData[0]._serverNames[1] == "server2");
        CHECK(serversData[0]._serverPorts[0] == 8081);
        CHECK(serversData[0]._serverPorts[1] == 8082);
        CHECK(serversData[0]._hosts[0] == "127.0.0.1");
        CHECK(serversData[0]._hosts[1] == "localhost");
        CHECK(serversData[0]._methods[0] == GET);
        CHECK(serversData[0]._methods[1] == POST);
        CHECK(serversData[0]._methods[2] == DELETE);

        CHECK(serversData[1]._serverNames[0] == "server1");
        CHECK(serversData[1]._serverPorts[0] == 8081);
        CHECK(serversData[1]._serverPorts[1] == 8087);
        CHECK(serversData[1]._hosts[0] == "127.0.0.1");
        CHECK(serversData[1]._methods[0] == GET);
        CHECK(serversData[1]._methods[1] == POST);
        CHECK(serversData[1]._methods[2] == DELETE);
    }

    // Tests for the getKeywordValue method
    SUBCASE("getDirective")
    {
        std::string configStr = "server { listen 8080;\n server_name localhost; \n root data/www;\n }";
        std::string directive = " server_name ";

        std::vector<std::string> value = cs->getDirectiveVal(configStr, directive);
        CHECK(value.size() == 1);
        CHECK(value[0] == "localhost");

        directive = " listen ";
        value = cs->getDirectiveVal(configStr, directive);
        CHECK(value[0] == "8080");

        directive = " root ";
        value = cs->getDirectiveVal(configStr, directive);
        CHECK(value[0] == "data/www");
    }

    // Tests for the cleanedLine method
    SUBCASE("cleanedLine")
    {
        std::string line = "  listen   8080;  # server port \n";
        std::string cleaned = cs->cleanedLine(line);
        CHECK(cleaned == "listen 8080; ");
    }

    // Tests for the getPortPart and validPort functions
    SUBCASE("getPortPart and validPort")
    {
        std::string input = "localhost:80 127.0.0.1:8080";
        std::vector<int> portPart = cs->getPorts(input);
        CHECK(portPart[0] == atoi("80"));
        CHECK(portPart[1] == atoi("8080"));

        bool valid = validPort(80);
        CHECK(valid == true);

        valid = validPort(100000);
        CHECK(valid == false);

        valid = validPort(1025);
        CHECK(valid == true);

        valid = validPort(100);
        CHECK(valid == false);
    }

    // Tests for the getMethods method
    SUBCASE("getMethods")
    {
        std::string configStr = "location / { methods GET POST; }\n";
        std::vector<std::string> strMethods = cs->getDirectiveVal(configStr, " methods ");
        std::vector<enum eRequestType> methods = validMethods(strMethods);

        CHECK(methods.size() == 2);
        CHECK(methods[0] == GET);
        CHECK(methods[1] == POST);
    }
}

TEST_CASE("ConfigServer::lineNeeded")
{
    SUBCASE("SERVER DATA TESTING")
    {
        ConfigServer::getInstance()->setConfigServer("test/test_config.conf");
        ConfigServer *cs = ConfigServer::getInstance();
        std::vector<ServerData> serversData = cs->getServerData();
        CHECK(serversData.size() == 2);

        for (int i = 0; i < static_cast<int>(serversData.size()); i++)
        {
            CHECK(static_cast<int>(serversData.size()) == 2);

            for (int j = 0; j < static_cast<int>(serversData[i]._hosts.size()); j++)
            {
                 CHECK(serversData[0]._hosts.size() == 2);
                CHECK(serversData[1]._hosts.size() == 2);
            }

            for (int j = 0; j < static_cast<int>(serversData[i]._serverPorts.size()); j++)
            {
                CHECK(serversData[0]._serverPorts.size() == 5);
                CHECK(serversData[1]._serverPorts.size() == 5);
            }
        }
    }
}


TEST_CASE("ConfigServer class")
{
    ConfigServer::getInstance()->setConfigServer("test/test_config.conf");
    ConfigServer *cs = ConfigServer::getInstance();
    
    std::vector<ServerLocation> locations;

    SUBCASE("getConfigString")
    {
        std::string confStr  = cs->getConfigString();
        CHECK_FALSE(confStr.empty());
    }

    std::string confString  = cs->getConfigString();
    CHECK_FALSE(confString.empty());

    std::vector<std::string> blocks = cs->getServerBlocksData(confString);
    CHECK(blocks.size() == 2);

    std::vector<ServerBlocks> serverBlocks(blocks.size());
    CHECK(serverBlocks.size() == 2);

    std::vector<ServerData> serversData(blocks.size());
    CHECK(serversData.size() == 2);

    SUBCASE("getServerBlocksData")
    {
        for (size_t i = 0; i < serverBlocks.size(); i++)
        {
            std::string confStr = blocks[i];
            std::vector<std::string> locationBlocks = cs->getLocationBlocks(confStr);
            CHECK(locationBlocks.size() == 3);

            serverBlocks[i]._locations = cs->getLocationPart(locationBlocks);
            locations = serverBlocks[i]._locations;
            CHECK(serverBlocks[i]._locations.size() == 3);

            serverBlocks[i]._listen = cs->getDirective(confStr, " listen ");
            CHECK(serverBlocks[i]._listen.size() == 3);

            serverBlocks[i]._serverNames = cs->getDirective(confStr, " server_name ");
            CHECK(serverBlocks[i]._serverNames.size() == 1);

            serverBlocks[i]._root = cs->getDirective(confStr, " root ");
            CHECK(serverBlocks[i]._root.size() == 1);

            serverBlocks[i]._methods = cs->getDirective(confStr, " methods ");
            CHECK(serverBlocks[i]._methods.size() == 1);

            serverBlocks[i]._index = cs->getDirective(confStr, " index ");
            CHECK(serverBlocks[i]._index.size() == 1);

            serverBlocks[i]._errorPages = cs->getDirective(confStr, " error_page ");
            CHECK(serverBlocks[i]._errorPages.size() == 3);

            validRemaining(confStr); // Validate that all that's remaining is: "server{}"

            CHECK(confStr == "server{}");

        }
    }
    // std::cout << " Root! " << &serverBlocks[0]._locations[0]._root[0] << std::endl;
    
    SUBCASE("Locations ")
    {
        for (size_t i = 0; i < locations.size(); i++)
        {
            CHECK(locations[i]._methods.size() == 1);
            CHECK(locations[i]._root.size() == 1);
            CHECK(locations[i]._index.size() == 1);
            CHECK(locations[i]._autoindex.size() == 1);
            CHECK(locations[i]._redirection.size() == 1);
        } 
    }

    SUBCASE("SETTING LOCATIONS")
    {

        // // SETTING LOCATIONS        
        for (size_t i = 0; i < serverBlocks.size(); i++)
        {
            std::vector<std::string> locationBlocks = cs->getLocationBlocks(blocks[i]);
            CHECK(locationBlocks.size() == 3);

            serverBlocks[i]._locations = cs->getLocationPart(locationBlocks);
            CHECK(serverBlocks[i]._locations.size() == 3);

            std::vector<std::string> paths;
            for (size_t j = 0; j < serverBlocks[i]._locations.size(); j++)
            {
                paths.push_back(serverBlocks[i]._locations[j]._path);
                CHECK_FALSE(pathDup(paths));
                serversData[i]._locations[serverBlocks[i]._locations[j]._path]
                = cs->settingLocation(serverBlocks[i]._locations[j], serversData[i]);
            }
        
        }
        /* FIRST SERVER BLOCK */
        // CHECK ROOTS
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[0]._path].root == "data");
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[1]._path].root == "data/www");
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[2]._path].root == "data/www");

        // CHECK INDEXES
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[0]._path].index[0] == "index.html");
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[1]._path].index[0] == "index.html");
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[2]._path].index[0] == "ours.html");

        // CHECK METHODS
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[0]._path].methods[0] == 0);
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[0]._path].methods[1] == 1);
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[0]._path].methods[2] == 3);

        CHECK( serversData[0]._locations[serverBlocks[0]._locations[1]._path].methods[0] == 0);
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[1]._path].methods[1] == 1);
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[1]._path].methods[2] == 3);

        CHECK( serversData[0]._locations[serverBlocks[0]._locations[2]._path].methods[0] == 0);
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[2]._path].methods[1] == 1);


        // CHECK AUTOINDEX
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[0]._path].autoindex == "OFF");
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[1]._path].autoindex == "ON");
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[2]._path].autoindex == "ON");

         // CHECK REDIRECTIONS
        CHECK( serversData[0]._locations[serverBlocks[0]._locations[2]._path].redirection == "301 index.html");

        /* SECOND SERVER BLOCK */
        CHECK( serversData[1]._locations[serverBlocks[1]._locations[0]._path].root == "data/www");
        CHECK( serversData[1]._locations[serverBlocks[1]._locations[1]._path].root == "data/www");
        CHECK( serversData[1]._locations[serverBlocks[1]._locations[2]._path].root == "data/www");

        // CHECK INDEXES
        CHECK( serversData[1]._locations[serverBlocks[0]._locations[0]._path].index[0] == "about.html");
        CHECK( serversData[1]._locations[serverBlocks[0]._locations[1]._path].index[0] == "contact.html");
        CHECK( serversData[1]._locations[serverBlocks[0]._locations[1]._path].index[1] == "about.html");
        CHECK( serversData[1]._locations[serverBlocks[0]._locations[2]._path].index[0] == "index.html");

        // CHECK METHODS
        CHECK( serversData[1]._locations[serverBlocks[1]._locations[0]._path].methods[0] == 0);
        CHECK( serversData[1]._locations[serverBlocks[1]._locations[0]._path].methods[1] == 1);
        CHECK( serversData[1]._locations[serverBlocks[1]._locations[0]._path].methods[2] == 3);

        CHECK( serversData[1]._locations[serverBlocks[1]._locations[1]._path].methods[0] == 0);
        CHECK( serversData[1]._locations[serverBlocks[1]._locations[1]._path].methods[1] == 1);
        CHECK( serversData[1]._locations[serverBlocks[1]._locations[1]._path].methods[2] == 3);

        CHECK( serversData[1]._locations[serverBlocks[1]._locations[2]._path].methods[0] == 0);
        CHECK( serversData[1]._locations[serverBlocks[1]._locations[2]._path].methods[1] == 3);

        // CHECK AUTOINDEX
        CHECK( serversData[1]._locations[serverBlocks[0]._locations[0]._path].autoindex == "OFF");
        CHECK( serversData[1]._locations[serverBlocks[0]._locations[1]._path].autoindex == "ON");
        CHECK( serversData[1]._locations[serverBlocks[0]._locations[2]._path].autoindex == "OFF");

        // CHECK REDIRECTIONS
        CHECK( serversData[1]._locations[serverBlocks[0]._locations[2]._path].redirection == "301 home.html");
    }
    
}