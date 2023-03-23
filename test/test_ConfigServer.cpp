
#include "test_header.hpp"


TEST_CASE("ConfigServer::lineNeeded")
{
	ConfigServer cs;
	SUBCASE("Returns false for an empty line")
	{
		std::string line = "";
		CHECK_FALSE(cs.lineNeeded(line));
	}
	SUBCASE("Returns false for a line with only whitespace")
	{
		std::string line = "   \t\t ";
		CHECK_FALSE(cs.lineNeeded(line));
	}
	SUBCASE("Returns false for a commented line")
	{
		std::string line = "# this is a comment";
		CHECK_FALSE(cs.lineNeeded(line));
	}
	SUBCASE("Returns true for a line with non-whitespace characters")
	{
		std::string line = "listen 127.0.0.1:8080";
		CHECK(cs.lineNeeded(line));
	}
	SUBCASE("Returns true for a line with leading whitespace")
	{
		std::string line = "\t    listen 127.0.0.1:8080";
		CHECK(cs.lineNeeded(line));
	}
}

TEST_CASE("ConfigServer::getFile")
{
	// Should pass
	SUBCASE("valid file provided as command line argument")
	{
		const char *argv[] = {"test", "configFiles/default.config"};
		std::string configFile = getFile(2, argv);
		CHECK(configFile == "configFiles/default.config");
	}

	// Should pass
	SUBCASE("no command line argument, default config file exists")
	{
		const char *argv[] = {"test"};
		std::string configFile = getFile(1, argv);
		CHECK(configFile == "configFiles/default.config");
	}

	// Should fail
	// SUBCASE("no command line argument, default config file does not exist")
	// {
	// 	const char *argv[] = {"test"};
	// 	std::string configFile = getFile(1, argv);
	// 	CHECK_THROWS(getFile(1, argv));
	// }

	// Should pass
	SUBCASE("invalid file provided as command line argument")
	{
		const char *argv[] = {"test", "configFiles/nonexistent.config"};
		CHECK_THROWS(getFile(2, argv));
	}
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