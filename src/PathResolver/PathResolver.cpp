//
// Created by Teddy BLANCO on 2023-04-19.
//

#include "PathResolver.hpp"
#include "../Utils/FT.hpp"

template <typename T, typename Y>
bool	_existsInMap(const std::map<T, Y> &map, const T &key) {
	return map.find(key) != map.end();
}

PathResolver::PathResolver(Request &req) : _request(req), _root(_request._root) {	Log::debugFunc(__FUNCTION__);

	_serverData = ConfigServer::getInstance()->getServerData()[_request._serverId];
	resolvePath();
}

PathResolver::~PathResolver() {	Log::debugFunc(__FUNCTION__);
}

void		PathResolver::resolvePath() {	Log::debugFunc(__FUNCTION__);

	std::string location;
	bool 		hasLocation = false;

	location = getLocation();
	hasLocation = setRoot(location);
	if (hasExtension(_root))
	{
		if (!isRootValid())
			_request._status = 404;
	}
	else //no extension
	{
		if (!_serverData._locations[location].redirection.empty())
		{
			_request._status = 301;
			_request._startLine.path = _serverData._locations[location].redirection;
			return;
		}
		addIndex(hasLocation, location);
		if (!isRootValid())
		{
			if (hasLocation)
				_request._status = 500;
			else
				_request._status = 404;
		}
	}
}

std::string	PathResolver::getLocation() {	Log::debugFunc(__FUNCTION__);

	if (_request._startLine.path.find_first_of('/', 1) == std::string::npos)
		return _request._startLine.path;
	else
		return _request._startLine.path.substr(0, _request._startLine.path.find_first_of('/', 1));
}

bool 		PathResolver::setRoot(const std::string &location) {	Log::debugFunc(__FUNCTION__);

	// set the root to the location root if it exists
	if (_existsInMap(_serverData._locations, location))
	{
		_root = _serverData._locations[location].root;
		_root += _request._startLine.path.substr(location.length());
		return true;
	}
	_root = _serverData._root[0];
	if (_request._startLine.path.length() > 1)
		_root += _request._startLine.path.substr();
	return false;
}

bool 		PathResolver::hasExtension(const std::string &path) {	Log::debugFunc(__FUNCTION__);

	if (path.find('.') != std::string::npos && path.find('.') != path.size() - 1)
		return true;
	return false;
}

bool 		PathResolver::isRootValid() { 	Log::debugFunc(__FUNCTION__);
	std::ifstream fd(_root.c_str());

	if (fd)
	{
		Log::log(Log::DEBUG, "Root is valid: " + _root);
		fd.close();
		return true;
	}
	Log::log(Log::DEBUG, "Root is not valid: " + _root);
	return false;
}

void 		PathResolver::addIndex(bool hasLocation, std::string location) {	Log::debugFunc(__FUNCTION__);
	std::string index;

	if (hasLocation)
		index = _serverData._locations[location].index[0];
	else
		index = _serverData._index[0];

	_root += "/" + index;
}