#include "Cookie.hpp"

Cookie::Cookie(){}
Cookie::~Cookie(){}

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

void	Cookie::addListItem(const std::string& cookieId)
{
	if (_list.find(cookieId) != _list.end())
		_list.find(cookieId)->second = nowSecond();
	else
		_list.insert(std::pair<std::string, int>(cookieId, nowSecond()));
}

//add -> Set-Cookie: CookieId=xxxxx; Max-Age=CONNECT_TIME * 2

void	Cookie::removeListItem(const std::string& cookieId)
{
	if (_list.find(cookieId) != _list.end())
		_list.erase(_list.find(cookieId));
}

int	Cookie::nowSecond() const
{
	std::time_t	now = std::time(0);
	return (static_cast<int>(now));
}

void	Cookie::removeListItem(const std::string& cookieId)
{
	if (_list.find(cookieId) != _list.end())
		_list.erase(_list.find(cookieId));
}

bool	Cookie::isClientCookieId(const std::string& clientReq) const
{
	if (clientReq.find(COOKIEID_NAME) != std::string::npos)
		return true;
	return false;
}

std::string	Cookie::clientCookieId(const std::string& clientReq) const
{
	std::size_t pos = clientReq.find(COOKIEID_NAME);
	std::string cookieName = COOKIEID_NAME;
	return clientReq.substr(pos + cookieName.size() + 1, COOKIEID_SIZE);
}

bool	Cookie::auth(const std::string& clientReq)
{
	if (isClientCookieId(clientReq) == true)
	{
		std::string cookieId = clientCookieId(clientReq);
		return isValid(cookieId);
	}
	return false;
}

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
