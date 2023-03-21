//
// Created by Teddy Blanco on 3/21/23.
//

#include <iostream>
#include <vector>
#include <map>
#include "../../includes/webserv.hpp"

#ifndef CONFIGSERVERV2_HPP
#define CONFIGSERVERV2_HPP

/*
	#define F_GET 0x1
	#define F_POST 0x2
	#define F_DELETE 0x4
	unsigned char flags;
	_locations["/"].flags |= F_GET;
*/

struct location {
	std::string 				root;
	std::vector<eRequestType>	allowedMethods;
	std::vector<std::string>	index;

};

class ConfigServerV2 {
public:
	ConfigServerV2();
	~ConfigServerV2();

	uint16_t					_port;			//Port number
	std::string					_host;			//IP address
	std::vector<std::string>	_hostnames; 	//Hostnames
	uint32_t 					_maxBodySize;	//

	std::map<std::string, location>			_locations;		//Locations

	void test()
	{
		_locations["/"].allowedMethods.push_back(tRequestType::GET);
		_locations["/"].allowedMethods.push_back(POST);


		_locations["/"].allowedMethods[GET];


	}
};
#endif CONFIGSERVERV2_HPP
