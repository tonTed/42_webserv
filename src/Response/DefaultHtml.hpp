//
// Created by Teddy Blanco on 4/4/23.
//

#ifndef DEFAULTHTML_HPP
# define DEFAULTHTML_HPP

#include "../../includes/webserv.hpp"
#include "CodeResponse.hpp"

class DefaultHTML : public CodeResponse {

public:
	static std::string getHtml(int status);
	static std::string getHtmlError(int status);

};


#endif
