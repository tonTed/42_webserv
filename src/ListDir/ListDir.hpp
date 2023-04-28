#ifndef LISTDIR_HPP
# define LISTDIR_HPP

# include "../../includes/webserv.hpp"

class ListDir
{
private:
				ListDir(const ListDir& rhs);
	ListDir&	operator=(const ListDir& rhs);
	

public:
				ListDir();
	virtual		~ListDir();

	const std::string	ListDirectory(const std::string& dirName) const;
	
};


#endif