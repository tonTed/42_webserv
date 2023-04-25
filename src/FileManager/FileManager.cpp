#include "FileManager.hpp"


FileManager::FileManager(const int& clientFd, const std::string& body):
		_clientFd(clientFd), _body(body), _fileName(""), _contentLength(0), _boundary("") {}

FileManager::~FileManager(){Log::debugFunc(__FUNCTION__);}

const int&			FileManager::getClientFd() const		{return _clientFd;}
const std::string&	FileManager::getBody() const			{return _body;}
const std::string&	FileManager::getFileName() const		{return _fileName;}
const int&			FileManager::getContentLength() const	{return _contentLength;}
const std::string&	FileManager::getBoundary() const		{return _boundary;}


const std::string&	FileManager::extractHeaderInfo(const std::string& title)
{
	Log::debugFunc(__FUNCTION__);
	size_t	position = _body.find(title + "=\"");
	Log::log(Log::INFO, "position:" + std::to_string(position));
	if (position != std::string::npos) // filename=" trouvé
	{
		position += title.size() + 2; //ajoute la longueur de title + =" a position
		size_t endPos = _body.substr(position).find_first_of('\"');	//trouve le prochain "
		 if (endPos != std::string::npos && endPos > 0							// 1) " trouvé 2)length > 0 
		 		&& _body.substr(position, endPos).find("\n") == std::string::npos	// 3) pas de \n dans le nom
		 		&& _body.substr(position, endPos).find(":") == std::string::npos	// 4) pas de : dans le nom
		 		&& _body.substr(position, endPos).find(";") == std::string::npos)	// 5) pas de ; dans le nom
			return _body.substr(position, endPos);
	}
	return "";
}

bool	FileManager::extractFilename()
{
	_fileName = extractHeaderInfo("filename");
	if (_fileName != "")
	{
		_fileName = UPLOADFILE_PATH + _fileName;	//ajoute le path des UPLOADFILE
		Log::log(Log::INFO, "_filename:\n" + _fileName);
		return true;
	}
	return false;
}

bool	FileManager::extractBoundary()
{
	_boundary = extractHeaderInfo("boundary");
	if (_boundary != "")
	{
		Log::log(Log::INFO, "_boundary:\n" + _boundary);
		return true;
	}
	return false;
}


bool	FileManager::extractFileContent()
{
	Log::debugFunc(__FUNCTION__);
	std::istringstream	iss(_body);
	std::string			line;
	_fileContent = "";
	std::string			boundary;

	while (std::getline(iss, line) && line.find("--") == std::string::npos);

	if (line.find("--") != std::string::npos)
			boundary = line.substr(0, line.length() - 1);
	else
		return false;

	Log::log(Log::INFO, "boundary:" + boundary);

	while (std::getline(iss, line) && line != "\r");
	
	while (std::getline(iss, line) && line.find(boundary) == std::string::npos)
	{
		_fileContent += line + "\n";
		Log::log(Log::INFO, "line:" + line);
	}

	if (_fileContent.length() > 1)
	{
		_fileContent = _fileContent.substr(0, _fileContent.length() - 1);
		Log::log(Log::INFO, "_fileContent:\n" + _fileContent);
		return true;
	}
	_fileContent = "";
	return false;
}

bool	FileManager::saveFile()
{
	Log::debugFunc(__FUNCTION__);
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
