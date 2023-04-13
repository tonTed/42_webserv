//
// Created by Teddy BLANCO on 2023-04-13.
//

#ifndef LOG_H
# define LOG_H

#include <iostream>
#include "../../includes/colors.hpp"

#define LOG_DEBUG		"[DEBUG]"
#define LOG_INFO		"[INFO]"
#define LOG_WARNING		"[WARNING]"
#define LOG_ERROR		"[ERROR]"

class Log {
public:
	static void			log(std::string message, std::string type);

private:
	static std::string	getTime();
};


#endif //LOG_H
