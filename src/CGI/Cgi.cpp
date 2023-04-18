#include "Cgi.hpp"
#include <unistd.h>

char **vectorToChar(std::vector<std::string> &vec) {
	char **ret = new char*[vec.size() + 1];
	for (size_t i = 0; i < vec.size(); i++) {
		ret[i] = strdup(vec[i].c_str());
	}
	ret[vec.size()] = NULL;
	return ret;
}

CGI::CGI(const Request &request) :	_request(request),
									_serverData(ConfigServer::getInstance()->getServerData()[_request._serverId]) {
	_setEnv();
}

void CGI::_setEnv() { Log::debugFunc(__FUNCTION__);

	_env.push_back("QUERY_STRING=name=Teddy&age=36" + _request._startLine.queryString);

//	_env.push_back("SERVER_SOFTWARE=webserv/1.0");
//	_env.push_back("SERVER_NAME=" + _serverData._serverNames[0]);
//	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
//
//	_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
//	_env.push_back("SERVER_PORT=" + std::to_string(_serverData._serverPorts[0]));
//	_env.push_back("REQUEST_METHOD=" + _request._startLine.type);
//	_env.push_back("PATH_INFO=");
//	_env.push_back("SCRIPT_NAME=" + _request._startLine.path);
//	_env.push_back("CONTENT_TYPE=");
//	_env.push_back("CONTENT_LENGTH=");
//
//	_env.push_back("HTTP_ACCEPT=");
//	_env.push_back("HTTP_USER_AGENT=");
//	_env.push_back("HTTP_COOKIE=");
}

void	CGI::executeCgi() { Log::debugFunc(__FUNCTION__);

	char *pathToCgi = strdup("./data/CGI/CGI.py");
	char *args[] = {pathToCgi, NULL};

	pid_t pid = fork();


	if (pid == -1) {
		Log::log(Log::ERROR,"Error forking process.");
		// reponse 500
	}
	else if (pid == 0) {

		dup2(_request._cgiFd[PIPE_WRITE], 1);
		close(_request._cgiFd[PIPE_READ]);

		char **_envp = vectorToChar(_env);

		execve(args[0], args, _envp);
		Log::log(Log::ERROR,"Error executing CGI script.");
		close(_request._cgiFd[PIPE_WRITE]);
		// reponse 500
		exit(1);
	}
}

CGI::~CGI() {
}