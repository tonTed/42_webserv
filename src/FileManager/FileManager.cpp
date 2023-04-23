//
// Created by Teddy Blanco on 4/20/23.
//

#include "FileManager.hpp"


FileManager::FileManager(const std::string& body, const int& action): _body(body)
{
	if (extractFilename() == true)
	{
		switch	(action)
		{
			case FMA_UPLOAD:
				saveFile();
				break;
			
			case FMA_DELETE:
				deleteFile();
				break;

			default:
				std::cout << "FileManagement constructor bad action" << std::endl;
		}
	}
}

FileManager::~FileManager(){}



bool	FileManager::extractFilename()
{
	//extract filename
	size_t	position = _body.find("filename=");
	if (position != std::string::npos)
	{
		position += 10;
		size_t endPos = _body.substr(position).find("\"");
		 if (endPos != std::string::npos)
		 {
			_fileName = _body.substr(position, endPos - position);
			return true;
		 }
	}

	//respond filename not found 
	return false;
}

bool	FileManager::extractFileContent()
{
	//extract file content
	std::istringstream	iss(_body);
	std::string			line;
	std::string			lastLine = "";
	_fileContent = "";

	while (std::getline(iss, line) && line.length() != 0);
	
	while (std::getline(iss, line))
	{
		_fileContent += lastLine;
		lastLine = line + "\n";
	}
	if (_fileContent.length() > 0)
	{
		_fileContent = _fileContent.substr(0, _fileContent.length() - 1);
		return true;
	}
	//respond bad content
	return false;
}

bool	FileManager::saveFile()
{
	//? check if filename already exist (if yes maybe respond file updated)
	
	extractFileContent();

	//create file
	//write fileContent
	
	return true;

}
	
bool	fileNameExist()
{
	//check if file exist in upload
}
//UplaodFile(body);
		// Parse body
			// filename
			// content

		// Create file
			// Write content
			// close file


	// Delete file
