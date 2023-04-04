#include "Response.hpp"

Response::Response(const Request &request, int status) : _request(request), _status(status){
	_config = ConfigServer::getInstance();

	if (status != 200) {
		//send error
	}
	else {
		//send file
	}
}

std::string Response::resolvePath(const std::string &path) {
	std::string resolvedPath = "Mock return" ;
	std::string file;
	std::string root = path;

	if (hasExtension(path))
	{
		removeFile(root, file);
		if (existInLocation(root))
		{
			//change root with local root
			if (localFileExist(root))
			{
				resolvedPath = root + file;
			}
			else
			{
				//resolve error pages 404
			}
		}
		else
		{
			if (existInRoot(root))
			{
				if (localFileExist(root))
				{
					resolvedPath = root + file;
				}
				else
				{
					//resolve error pages 404
				}
			}
			else
			{
				//resolve error pages 404
			}
		}
	}
	else
	{
		if (existInLocation(root))
		{
			//change root with local root
			//add index
		}
		else
		{
			if(localRootExist(root))
			{
				if (indexExist(root))
				{
					//resolvedPath = root + index;
				}
				else
				{
					//resolve error pages 500
				}
			}
			else
			{
				//resolve error pages 500
			}
		}
	}

	return resolvedPath;
}

bool Response::hasExtension(const std::string &path) {
	if (path.find('.') != std::string::npos && path.find('.') != path.size() - 1)
		return true;
	return false;
}

void Response::removeFile(std::string &path, std::string &file) {
	size_t pos = path.find_last_of('/');
	file = path.substr(pos + 1);
	path = path.substr(0, pos + 1);
}

bool Response::existInLocation(const std::string &path) {

	try { _config->_serversData.at(_request._serverId)._locations.at(path); }
	catch (std::out_of_range &e) { (void)e; return false; }
	return true;
}

bool Response::existInRoot(const std::string &path) {
	(void)path;
	return true;
}

bool Response::localRootExist(const std::string &path) {
	(void)path;
	return true;
}

bool Response::localFileExist(const std::string &path) {
	(void)path;
	return true;
}

bool Response::indexExist(std::string &path) {
	(void)path;
	return true;
}
