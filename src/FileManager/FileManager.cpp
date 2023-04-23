#include "FileManager.hpp"


FileManager::FileManager(const std::string& body): _body(body) {}

FileManager::~FileManager(){}

const std::string&	FileManager::getBody() const		{return _body;}
const std::string&	FileManager::getFileName() const	{return _fileName;}
const std::string&	FileManager::getContent() const		{return _fileContent;}

bool	FileManager::extractFilename()
{
	size_t	position = _body.find("filename=");
	if (position != std::string::npos)
	{
		position += 10;
		size_t endPos = _body.substr(position).find("\"");
		 if (endPos != std::string::npos)
		 {
			_fileName = _body.substr(position, endPos - position);
			_fileName = UPLOADFILE_PATH + _fileName;
			return true;
		 }
	}
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
	return false;
}

bool	FileManager::saveFile()
{
	//? check if filename already exist (if yes maybe respond file updated)
	if (extractFilename())
	{
		std::ofstream	uploadFile(_fileName, std::ios::trunc);
		if (extractFileContent())
		{
			uploadFile << _fileContent;
			uploadFile.close();
			return true;
		}
	}
	return false;
}
	
bool	FileManager::deleteFile()
{
	if (std::remove(_fileName.c_str()) == 0)
		return true;
	return false;
}


