//
// Created by Teddy BLANCO on 2023-04-13.
//

#include "Log.h"

#define DEBUG_ 1

void Log::debugFunc(std::string function) {

	if (DEBUG_)
		std::cerr << BOLD_MAGENTA << "DEBUG: " << function << RESET << std::endl;
}

void		Log::log(std::string type, std::string message) {

//	std::cout << "[" << getTime() << "]";

	if (type == Log::DEBUG)
		std::cerr << BLUE << "[" << type << "] " RESET;
	else if (type == Log::INFO)
		std::cerr << GREEN << "[" << type << "] " RESET;
	else if (type == Log::WARNING)
		std::cerr << YELLOW << "[" << type << "] " RESET;
	else if (type == Log::ERROR)
		std::cerr << RED << "[" << type << "] " RESET;

	std::cerr << message << std::endl;
}

std::string Log::getTime()
{
	time_t now = time(0);
	tm *ltm = localtime(&now);
	std::string date = std::to_string(1900 + ltm->tm_year) + "-" + std::to_string(1 + ltm->tm_mon) + "-" + std::to_string(ltm->tm_mday) + " " + std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string(ltm->tm_sec);
	return date;
}

std::string Log::DEBUG = "DEBUG";
std::string Log::INFO = "INFO";
std::string Log::WARNING = "WARNING";
std::string Log::ERROR = "ERROR";