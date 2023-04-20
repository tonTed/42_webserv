//
// Created by Teddy Blanco on 4/20/23.
//

#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include "../../includes/webserv.hpp"

class FileManager {

public:

	FileManager(const std::string &body);
	~FileManager();

	bool saveFile();
	bool deleteFile();

};


#endif //FILEMANAGER_HPP
