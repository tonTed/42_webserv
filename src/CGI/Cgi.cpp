#include "Cgi.hpp"

CGI::CGI(const Request &request) :	_request(request),
									_serverData(ConfigServer::getInstance()->getServerData()[_request._serverId]) {
	setEnvp();
}

void CGI::setEnvp() {

//	_envp.push_back("SERVER_SOFTWARE=webserv/1.0");
//	_envp.push_back("SERVER_NAME=" + _serverData._serverNames[0]);
//	_envp.push_back("GATEWAY_INTERFACE=CGI/1.1");
//
//	_envp.push_back("SERVER_PROTOCOL=HTTP/1.1");
//	_envp.push_back("SERVER_PORT=" + std::to_string(_serverData._serverPorts[0]));
//	_envp.push_back("REQUEST_METHOD=" + _request._startLine.type);
//	_envp.push_back("PATH_INFO=");
//	_envp.push_back("SCRIPT_NAME=" + _request._startLine.path);
//	_envp.push_back("QUERY_STRING=" + _request._startLine.queryString);
//	_envp.push_back("CONTENT_TYPE=");
//	_envp.push_back("CONTENT_LENGTH=");
//
//	_envp.push_back("HTTP_ACCEPT=");
//	_envp.push_back("HTTP_USER_AGENT=");
//	_envp.push_back("HTTP_COOKIE=");
}