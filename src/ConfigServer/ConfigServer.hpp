#ifndef CONFIGSERVER_HPP
# define CONFIGSERVER_HPP

# include "../Server/Server.hpp"
# include "../global.hpp"

typedef std::vector<Server> serVec;

class ConfigServer
{
  private:
	serVec _servers;

  public:
	ConfigServer();
	ConfigServer(const std::string file);
	virtual ~ConfigServer();

	ConfigServer &operator=(const ConfigServer &Config);

	void readFile(const std::string &file);
	bool fileExists(const std::string &file);
	serVec getServs(void) const;
};

std::ostream &operator<<(std::ostream &o, const ConfigServer &config);

#endif
