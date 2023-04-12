#include "Cookie.hpp"

Cookie::Cookie(){}
Cookie::~Cookie(){}

//*****************************SET/GET/TERS*************************************

	int		Cookie::getListSecond(const std::string& cookieId) const
	{
		if (_list.find(cookieId) != _list.end())
			return _list.find(cookieId)->second;
		return -1;
	}
	
	void	Cookie::setListSecond(const std::string& cookieId, const int& second)
	{
		if (_list.find(cookieId) != _list.end())
			_list.find(cookieId)->second = second;
	}
	
	int		Cookie::getNbListItem() const
	{
		return _list.size();
	}


//****************************VERIFICATION SIDE*********************************

//TO USE IN REQUEST PARSING
//Return true if cookieId still valid
bool	Cookie::isCookieValid(const std::string& clientReq)
{
	if (COOKIE_ACTIVATED == 0)
		return true;
	if (isClientCookieId(clientReq) == true)
	{
		std::string cookieId = clientCookieId(clientReq);
		return isValid(cookieId);
	}
	return false;
}

//Return true if cookie is in _list and still valid
//Return false if not and remove the cookie from _list
bool	Cookie::isValid(const std::string& cookieId)
{
	if (_list.find(cookieId) == _list.end())
		return false;
	if (_list.find(cookieId)->second < nowSecond() - CONNECT_TIME)
	{
		removeListItem(cookieId);
		return false;
	}
	return true;
}

//Remove cookieId from -list if exist
void	Cookie::removeListItem(const std::string& cookieId)
{
	if (_list.find(cookieId) != _list.end())
		_list.erase(_list.find(cookieId));
}


//*******************************SETTER SIDE************************************

//TO USE IN RESPOND
//create or update servercookie
//return cookie respond command
std::string	Cookie::cookieLine(const std::string& clientReq)
{
	std::string	cookieId;
	if (COOKIE_ACTIVATED == 1)
	{
		if (isClientCookieId(clientReq) == true)
			cookieId = clientCookieId(clientReq);
		else
			cookieId = createCookieId();
		addListItem(cookieId);
		return ClientCookieSet(cookieId);
	}
	return "";
}

//true if client have the cookieId
bool	Cookie::isClientCookieId(const std::string& clientReq) const
{
	if (clientReq.find(COOKIEID_NAME) != std::string::npos)
		return true;
	return false;
}

//return the cookieId of clientRequest
//(must be validated before with isClientCookieId)
std::string	Cookie::clientCookieId(const std::string& clientReq) const
{
	std::size_t pos = clientReq.find(COOKIEID_NAME);
	std::string cookieName = COOKIEID_NAME;
	return clientReq.substr(pos + cookieName.size() + 1, COOKIEID_SIZE);
}

//Return a unique cookieId
std::string	Cookie::createCookieId() const
{
	std::srand(std::time(0));
	std::string	cookieId;
	char	idLetter;
	do
	{
		cookieId.erase();
		for (int index = 0; index < COOKIEID_SIZE; index++)
		{
			idLetter = 141 + std::rand() % 26;
			cookieId += idLetter;
		}

	} while (_list.find(cookieId) != _list.end());
	return cookieId;
}

//add the cookieId to _list
void	Cookie::addListItem(const std::string& cookieId)
{
	if (_list.find(cookieId) != _list.end())
		_list.find(cookieId)->second = nowSecond();
	else
		_list.insert(std::pair<std::string, int>(cookieId, nowSecond()));
}

//return cookie respond command
std::string	Cookie::ClientCookieSet(const std::string& cookieId)
{
	std::stringstream	ss;
	ss << "Set-Cookie: " << COOKIEID_NAME << "=" << cookieId 
			<< "; Max-Age=" << CONNECT_TIME * 2;
	return ss.str();
}


//********************************UTILS*****************************************

//return a int representation of now in second
int	Cookie::nowSecond() const
{
	std::time_t	now = std::time(0);
	return (static_cast<int>(now));
}