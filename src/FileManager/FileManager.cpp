#include "FileManager.hpp"


FileManager::FileManager(const std::string& request_str):
		_request_str(request_str), _fileName(""), _boundary("") {}

FileManager::~FileManager(){Log::debugFunc(__FUNCTION__);}

const std::string&	FileManager::getResquestStr() const		{return _request_str;}
const std::string&	FileManager::getFileName() const		{return _fileName;}
const std::string&	FileManager::getBoundary() const		{return _boundary;}

bool	FileManager::extractor()
{
	Log::debugFunc(__FUNCTION__);
	if (extractFilename() && extractBoundary())
		return true;
	return false;
}

const std::string	FileManager::extractHeaderInfo(const std::string& title)
{
	Log::debugFunc(__FUNCTION__);
	size_t	position = _request_str.find(title);
	Log::log(Log::INFO, "position:" + std::to_string(position));
	if (position != std::string::npos) // filename=" trouvé
	{
		position += title.size();
		size_t endPos = _request_str.substr(position).find_first_of("\";\n,");	//trouve le prochain "
		if (endPos != std::string::npos && endPos > 0)
			return _request_str.substr(position, endPos);
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

bool	FileManager::saveFile()
{
	Log::debugFunc(__FUNCTION__);

	if (extractor() == true)
	{
		writeUpload();
		return true;
	}
	Log::log(Log::ERROR, "File not uploaded");
	return false;
}

void	FileManager::writeUpload()
{
	Log::debugFunc(__FUNCTION__);
	const int	fileLen = endPos() - startPos();
	std::ofstream	uploadFile(_fileName, std::ios::out | std::ios::binary);
	uploadFile.write(_request_str.substr(startPos(), fileLen).c_str(), fileLen);
	uploadFile.close();
}

//1) search boundary.2) from this position, search \r\n\r\n 3) add 4
int	FileManager::startPos()
{
	Log::debugFunc(__FUNCTION__);
	return _request_str.find("\r\n\r\n", _request_str.find(_boundary, _request_str.find("/r/n/r/n"))) + 4;
}

int	FileManager::endPos()
{
	Log::debugFunc(__FUNCTION__);
	return (_request_str.find_last_of(_boundary) - 2);
}

bool	FileManager::deleteFile()
{
	Log::debugFunc(__FUNCTION__);
	if (std::remove(_fileName.c_str()) == 0)
		return true;
	return false;
}
