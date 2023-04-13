//
// Created by Teddy BLANCO on 2023-04-13.
//

#include "Log.h"

void		Log::log(std::string message, std::string type) {

	std::cout << "[" << getTime() << "]";

	if (type == LOG_DEBUG)
		std::cout << BLUE << "[" << type << "] " RESET;
	else if (type == LOG_INFO)
		std::cout << GREEN << "[" << type << "] " RESET;
	else if (type == LOG_WARNING)
		std::cout << YELLOW << "[" << type << "] " RESET;
	else if (type == LOG_ERROR)
		std::cout << RED << "[" << type << "] " RESET;

	std::cout << message << std::endl;
}

std::string Log::getTime()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	std::string date = std::to_string(1900 + ltm->tm_year) + "-" + std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(ltm->tm_mday) + " " + std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string(ltm->tm_sec);
	return date;
}
