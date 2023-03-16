#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "../global.hpp"
# include <fstream>

class ConfigServer
{
  public:
	ConfigServer();
	ConfigServer(const std::string file);
	virtual ~ConfigServer();

	ConfigServer &operator=(const ConfigServer &Config);

	void readFile(const std::string &file);
	bool fileExists(const std::string &file);
};

std::ostream &operator<<(std::ostream &o, const ConfigServer &config);

#endif
