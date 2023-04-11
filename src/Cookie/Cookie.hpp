#ifndef COOKIE_HPP
# define COOKIE_HPP

# ifndef COOKIE_ACTIVATED
#  define COOKIE_ACTIVATED 0
# endif


# include <ctime>
# include <map>
# include <iostream>
# include <string>
# include <ctime>	//std::time
# include <cstdlib> //std:rand

typedef std::map<std::string, int> cookie_list_t;
typedef cookie_list_t::iterator cookie_list_it;
# define CONNECT_TIME 60
# define COOKIEID_NAME "CookieId"
# define COOKIEID_SIZE 10

class Cookie
{
private:
	cookie_list_t	_list;

public:
					Cookie();
	virtual			~Cookie();

	void			add(const std::string& clientReq);
	bool			auth(const std::string& clientReq);

	std::string		createCookieId() const;

	bool			isValid(const std::string& cookieId);
	void			addListItem(const std::string& cookieId);
	void			removeListItem(const std::string& cookieId);
	std::string		clientCookieId(const std::string& clientReq) const;
	bool			isClientCookieId(const std::string& clientReq) const;

	int				nowSecond() const;

};



#endif