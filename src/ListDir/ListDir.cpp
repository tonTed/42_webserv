# include "ListDir.hpp"

ListDir::ListDir(){}
ListDir::~ListDir() {}

const std::string	ListDir::ListDirectory(const std::string& dirName) const
{
	Log::debugFunc(__FUNCTION__);
	Log::log(Log::DEBUG, "dirName: " + dirName);

	DIR*	dir = opendir(dirName.c_str());
	if (dir != nullptr)
	{
		std::string files;
		struct dirent* entry;
		while ((entry = readdir(dir)) != nullptr)
		{
			files += entry->d_name;
			files += "<br>";
		}
		closedir(dir);
		std::stringstream	req;
		req << "HTTP/1.1 200 OK\r\n"
			<< "Content-Type: text/html\r\n"
			<< "HTTP/1.1 200 OK\r\n"
    		<< "\r\n"
    		<< "<html><body>"
    		<< "<h1>Files in directory " << dirName << "</h1>"
    		<< files
    		<< "</body></html>";

		return req.str();
	}
	return "error";
}