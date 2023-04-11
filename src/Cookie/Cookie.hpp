#ifndef COOKIE_HPP
# define COOKIE_HPP

//need to add define command in bonus compilation for cookie_activated

# ifndef COOKIE_ACTIVATED
#  define COOKIE_ACTIVATED 0
# endif

# include <ctime>	//std::time
# include <map>
# include <iostream>
# include <sstream>	//stringstream
# include <string>
# include <ctime>	//std::time
# include <cstdlib> //std:rand

typedef std::map<std::string, int> cookie_list_t;

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

//*****************************SET/GET/TERS*************************************

	int				getListSecond(const std::string& cookieId) const;
	void			setListSecond(const std::string& cookieId, const int& second);
	int				getNbListItem() const;

//****************************VERIFICATION SIDE*********************************

	bool			isCookieValid(const std::string& clientReq);

	bool			isValid(const std::string& cookieId);
	void			removeListItem(const std::string& cookieId);


//*******************************SETTER SIDE************************************

	std::string		cookieLine(const std::string& clientReq);

	bool			isClientCookieId(const std::string& clientReq) const;
	std::string		clientCookieId(const std::string& clientReq) const;
	std::string		createCookieId() const;
	void			addListItem(const std::string& cookieId);
	std::string		ClientCookieSet(const std::string& cookieId);


//********************************UTILS*****************************************
	int				nowSecond() const;

};



#endif