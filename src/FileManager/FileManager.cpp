#include "FileManager.hpp"


FileManager::FileManager(const int& clientFd, const std::string& body):
		_clientFd(clientFd), _body(body), _fileName(""), _contentLength(0), _boundary("") {}

FileManager::~FileManager(){Log::debugFunc(__FUNCTION__);}

const int&			FileManager::getClientFd() const		{return _clientFd;}
const std::string&	FileManager::getBody() const			{return _body;}
const std::string&	FileManager::getFileName() const		{return _fileName;}
const int&			FileManager::getContentLength() const	{return _contentLength;}
const std::string&	FileManager::getBoundary() const		{return _boundary;}


bool	FileManager::extractor()
{
	if (extractFilename() && extractBoundary() && extractContentLength())
		return true;
	return false;
}

const std::string&	FileManager::extractHeaderInfo(const std::string& title)
{
	Log::debugFunc(__FUNCTION__);
	size_t	position = _body.find(title);
	Log::log(Log::INFO, "position:" + std::to_string(position));
	if (position != std::string::npos) // filename=" trouvé
	{
		position += title.size();
		size_t endPos = _body.substr(position).find_first_of("\";\n,");	//trouve le prochain "
		 if (endPos != std::string::npos && endPos > 0)
			return _body.substr(position, endPos);
	}
	return "";
}

bool	FileManager::extractFilename()
{
	Log::debugFunc(__FUNCTION__);
	_fileName = extractHeaderInfo("filename=\"");
	if (_fileName != "")
	{
		_fileName = UPLOADFILE_PATH + _fileName;	//ajoute le path des UPLOADFILE
		Log::log(Log::INFO, "_filename:" + _fileName);
		return true;
	}
	return false;
}

bool	FileManager::extractBoundary()
{
	Log::debugFunc(__FUNCTION__);
	_boundary = extractHeaderInfo("boundary=");
	if (_boundary != "")
	{
		Log::log(Log::INFO, "_boundary:" + _boundary);
		return true;
	}
	return false;
}

bool	FileManager::extractContentLength()
{
	Log::debugFunc(__FUNCTION__);
	std::string	contentLength_str = extractHeaderInfo("Content-Length:");
	if (contentLength_str != "")
	{
		_contentLength = atoi(contentLength_str.c_str());
		Log::log(Log::INFO, "contentlength_str:" + contentLength_str + " _contentLength:" + std::to_string(_contentLength));
		return true;
	}
	return false;
}




bool	FileManager::saveFile()
{
	Log::debugFunc(__FUNCTION__);
	
	if (extractor())
	{
		std::ofstream	uploadFile(_fileName, std::ios::out | std::ios::binary);
		
	}
	
	
	
	if (extractFilename() && extractFileContent())
	{
		std::ofstream	uploadFile(_fileName, std::ios::out | std::ios::binary);
		uploadFile.write(_fileContent.c_str(), _fileContent.size());
		uploadFile.close();
		return true;
	}
	return false;
}
	
bool	FileManager::deleteFile()
{
	Log::debugFunc(__FUNCTION__);
	if (std::remove(_fileName.c_str()) == 0)
		return true;
	return false;
}
