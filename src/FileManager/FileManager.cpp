#include "FileManager.hpp"


FileManager::FileManager(const int& clientFd):
		_clientFd(clientFd), _request_str(""), _fileName(""), _contentLength(0), _boundary("") {}

FileManager::~FileManager(){Log::debugFunc(__FUNCTION__);}

const int&			FileManager::getClientFd() const		{return _clientFd;}
const std::string&	FileManager::getResquestStr() const		{return _request_str;}
const std::string&	FileManager::getFileName() const		{return _fileName;}
const int&			FileManager::getContentLength() const	{return _contentLength;}
const std::string&	FileManager::getBoundary() const		{return _boundary;}


bool	FileManager::extractor()
{
	Log::debugFunc(__FUNCTION__);
	if (reqToStr() && extractFilename() && extractBoundary() && extractContentLength())
		return true;
	return false;
}

bool	FileManager::reqToStr()
{
	Log::debugFunc(__FUNCTION__);
	const int bufSize = startPos();
	char		buffer[bufSize];
	ssize_t		nread;

	lseek(_clientFd, 0, SEEK_SET);
	Log::log(Log::INFO, "before recv");
	nread = recv(_clientFd, buffer, bufSize, 0);
	Log::log(Log::INFO, "nbRead:" + std::to_string(nread));
	_request_str.append(buffer, nread);
	if (_request_str.size() > 0)
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

bool	FileManager::extractContentLength()
{
	Log::debugFunc(__FUNCTION__);
	std::string	contentLength_str = extractHeaderInfo("Content-Length:");
	if (contentLength_str != "")
	{
		_contentLength = std::stoi(contentLength_str.c_str());
		Log::log(Log::INFO, "contentlength_str:" + contentLength_str + " _contentLength:" + std::to_string(_contentLength));
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
	std::ofstream	uploadFile(_fileName, std::ios::out | std::ios::binary);
	char	buffer[1024];
	int		nbRecv;
	int		nbLeft = _contentLength;
	lseek(_clientFd, startPos(), SEEK_SET);
	while (nbLeft > 1024)
	{
		nbRecv = recv(_clientFd, buffer, 1024, 0);
		uploadFile.write(buffer, nbRecv);
		if (nbRecv < 1024)
		{
			Log::log(Log::ERROR, "nbRecv != 1024");
			nbLeft = 0;
		}
		nbLeft -= nbRecv;
	}
	nbRecv = recv(_clientFd, buffer, nbLeft, 0);
	uploadFile.write(buffer, nbRecv);
	uploadFile.close();
}
// *const_cast<int*>(&nbLeft)
/* void	FileManager::writeUpload(const int contentLength_const)
{
	lseek(_clientFd, startPos(), SEEK_SET);
	
	std::ofstream	uploadFile(_fileName, std::ios::out | std::ios::binary);
	char	buffer[contentLength_const];
	int		nbRecv = recv(_clientFd, buffer, contentLength_const, 0);
	uploadFile.write(buffer, nbRecv);
	uploadFile.close();
} */

//1) search boundary.2) from this position, search \r\n\r\n 3) add 4
int	FileManager::startPos()
{
	Log::debugFunc(__FUNCTION__);
	int	boundary_pos =  _request_str.find(_boundary);
	int fileContentStartFromBoundary = _request_str.substr(boundary_pos).find("\r\n\r\n") + 4;
	Log::log(Log::INFO, "_boundary:" + _boundary);
	Log::log(Log::INFO, "Boundary position:" + std::to_string(boundary_pos));
	Log::log(Log::INFO, "fileContentStartFromBoundary:" + std::to_string(fileContentStartFromBoundary));	
	return (boundary_pos + fileContentStartFromBoundary);
}



bool	FileManager::deleteFile()
{
	Log::debugFunc(__FUNCTION__);
	if (std::remove(_fileName.c_str()) == 0)
		return true;
	return false;
}






// Fonction de lecture de _client fd

#include <list>
void	recordSocket()
{
	int client;
	std::list<char[1024]>	bufferBlock;
	char	buffer[1024];
	int	nbrecv = 1;
	int	bufferBlockLen;
	int	lastBlockLen;
	std::string	request_str = "";

	while (nbrecv == 1024)
	{
		nbrecv = recv(client, buffer, 1024, 0);
		bufferBlock.push_back(buffer);
		request_str.append(buffer);
	}
	bufferBlock.push_back(buffer);
	bufferBlockLen = bufferBlock.size() * 1024 + nbrecv;
	lastBlockLen = nbrecv;
	
}