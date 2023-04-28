//
// Created by Teddy BLANCO on 2023-04-13.
//

#ifndef LOG_H
# define LOG_H

#include <iostream>
#include <ctime>

#include "../../includes/colors.hpp"

class Log {
public:
	static void			log(std::string type, std::string message);
	static void			debugFunc(std::string function);
	
	static std::string	DEBUG;
	static std::string	INFO;
	static std::string	WARNING;
	static std::string	ERROR;

private:
	static std::string	getTime();
};


#endif //LOG_H
