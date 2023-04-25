#ifndef FILEMANAGER_HPP
# define FILEMANAGER_HPP

# include "../../includes/webserv.hpp"

//TODO Modifier le path pour celui ou non
# define UPLOADFILE_PATH "data/files/"

class FileManager {
private:
	int					_clientFd;
	std::string			_request_str;
	std::string			_fileName;
	int					_contentLength;
	std::string			_boundary;

public:

	FileManager(const int& clientFd);
	~FileManager();

	bool				saveFile();
	bool				deleteFile();

	bool				extractor();
	bool				reqToStr();	
	bool				extractFilename();
	bool				extractBoundary();
	bool				extractContentLength();
	const std::string	extractHeaderInfo(const std::string& title);
	int					startPos();
	void				writeUpload();

	const int&			getClientFd() const;
	const std::string&	getResquestStr() const;
	const std::string&	getFileName() const;
	const int&			getContentLength() const;
	const std::string&	getBoundary() const;

};


#endif
