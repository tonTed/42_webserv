//
// Created by Teddy Blanco on 4/20/23.
//

#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include "../../includes/webserv.hpp"

#define UPLOADFILE_PATH ""

enum	FileManAction_e
{
	FMA_UPLOAD,
	FMA_DELETE
};

class FileManager {
private:
	std::string	_body;
	std::string	_fileName;
	std::string	_fileContent;

public:

	FileManager(const std::string &body, const int& action);
	~FileManager();

	bool	saveFile();
	bool	deleteFile();

	bool	extractFilename();
	bool	extractFileContent();
	bool	fileNameExist();

};


#endif //FILEMANAGER_HPP
