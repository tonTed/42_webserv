
#include "test_header.hpp"

TEST_CASE("ConfigServer::lineNeeded")
{
    ConfigServer *cs = ConfigServer::getInstance();

    SUBCASE("Returns false for an empty line")
    {
        std::string line = "";
        CHECK_FALSE(cs->lineNeeded(line));
    }
    SUBCASE("Returns false for a line with only whitespace")
    {
        std::string line = "   \t\t ";
        CHECK_FALSE(cs->lineNeeded(line));
    }
    SUBCASE("Returns false for a commented line")
    {
        std::string line = "# this is a comment";
        CHECK_FALSE(cs->lineNeeded(line));
    }
    SUBCASE("Returns true for a line with non-whitespace characters")
    {
        std::string line = "listen 127.0.0.1:8080";
        CHECK(cs->lineNeeded(line));
    }
    SUBCASE("Returns true for a line with leading whitespace")
    {
        std::string line = "\t    listen 127.0.0.1:8080";
        CHECK(cs->lineNeeded(line));
    }
}

TEST_CASE("ConfigServer class")
{
    ConfigServer::getInstance()->setConfigServer("test/test_config.conf");
    ConfigServer *cs = ConfigServer::getInstance();
    std::vector<ServerData> serversData;

    // // Tests for the constructor and getInstance method
    // SUBCASE("Constructor and getInstance **SHOULD FAIL** ")
    // {
    // 	ConfigServer*  config1 = config.getInstance("test/test_config.conf");
    // 	ConfigServer*  config2 = config.getInstance("test/test_config.conf");

    // 	// SHOULD FAIL
    // 	CHECK(&config1 == &config2);
    // 	// CHECK_FALSE(!(&config1 == &config2));
    // }

    // Tests for the getServerData method
    SUBCASE("getServerData")
    {
        serversData = cs->getServerData();

        CHECK(serversData.size() == 2);

        CHECK(serversData[0]._serverNames[0] == "server1");
        CHECK(serversData[0]._serverNames[1] == "server2");
        CHECK(serversData[0]._serverPorts[0] == 8081);
        CHECK(serversData[0]._serverPorts[1] == 8080);
        CHECK(serversData[0]._hosts[0] == "127.0.0.1");
        CHECK(serversData[0]._hosts[1] == "localhost");
        CHECK(serversData[0]._methods[0] == GET);
        CHECK(serversData[0]._methods[1] == POST);
        CHECK(serversData[0]._methods[2] == DELETE);

        CHECK(serversData[1]._serverNames[0] == "server_test");
        CHECK(serversData[1]._serverPorts[0] == 8084);
        CHECK(serversData[1]._serverPorts[1] == 8085);
        CHECK(serversData[1]._hosts[0] == "127.0.0.1");
        CHECK(serversData[1]._methods[0] == POST);
    }

    // Tests for the getKeywordValue method
    SUBCASE("getKeywordValue")
    {
        std::string configStr = "server { listen 8080;\nserver_name localhost;\nroot /var/www;\n }";
        std::string directive = "server_name";

        std::vector<std::string> value = cs->getKeywordValue(configStr, directive);
        CHECK(value.size() == 1);
        CHECK(value[0] == "localhost");

        directive = "listen";
        value = cs->getKeywordValue(configStr, directive);
        CHECK(value[0] == "8080");

        directive = "root";
        value = cs->getKeywordValue(configStr, directive);
        CHECK(value[0] == "/var/www");
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
        std::string input = "localhost:80";
        std::string portPart = getPortPart(input);
        CHECK(portPart == "80");

        bool valid = validPort(80);
        CHECK(valid == true);

        valid = validPort(100000);
        CHECK(valid == false);
    }

    // Tests for the getMethods method
    SUBCASE("getMethods")
    {
        std::string configStr = "location / { methods GET POST; }\n";
        std::vector<enum eRequestType> methods = cs->getMethods(configStr);

        CHECK(methods.size() == 2);
        CHECK(methods[0] == GET);
        CHECK(methods[1] == POST);
    }

    //     // ... add more test cases for other methods and functions ...
}

TEST_CASE("ConfigServer::lineNeeded")
{
    SUBCASE("SERVER DATA TESTING")
    {
        ConfigServer::getInstance()->setConfigServer("test/test_config.conf");
        ConfigServer *cs = ConfigServer::getInstance();
        std::vector<ServerData> serversData = cs->getServerData();

        CHECK(serversData.size() == 2);
        // char buffer[] = "/var/temp/";
        // std::string expected = "/var/temp/";
        // std::string actual = serversData[0]._root;
        // CHECK_EQ(expected, actual);

        for (int i = 0; i < static_cast<int>(serversData.size()); i++)
        {
            // CHECK(static_cast<int>(serversData.size()) == 2);

            // CHECK_EQ(serversData[0]._root == expected);
            // char buffer[] = "/var/temp/";
            // std::string expected = "/var/temp/";
            // std::string actual = serversData[0]._root;
            // CHECK_EQ(expected, actual);
            // CHECK(serversData[1]._serverPorts.size() == 2);

            for (int j = 0; j < static_cast<int>(serversData[i]._hosts.size()); j++)
            {
                 CHECK(serversData[0]._hosts.size() == 2);
                CHECK(serversData[1]._hosts.size() == 1);
            }

            for (int j = 0; j < static_cast<int>(serversData[i]._serverPorts.size()); j++)
            {
                CHECK(serversData[0]._serverPorts.size() == 6);
                CHECK(serversData[1]._serverPorts.size() == 2);
            }


            // for (int j = 0; j < static_cast<int>(serversData[i]. .size()); j++)
            // {
            // }

        }
    }
}
