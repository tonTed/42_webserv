#ifndef FILEMANAGER_HPP
# define FILEMANAGER_HPP

# include "../../includes/webserv.hpp"

//TODO Modifier le path pour celui ou non
# define UPLOADFILE_PATH "data/files/"

class FileManager {
private:
	std::string			_request_str;
	std::string			_fileName;
	std::string			_boundary;

public:

	FileManager(const std::string& request_str);
	~FileManager();

	bool				saveFile();
	bool				deleteFile(std::string fileName);

	bool				extractor();
	bool				reqToStr();	
	bool				extractFilename();
	bool				extractBoundary();
	const std::string	extractHeaderInfo(const std::string& title);
	int					startPos();
	int					endPos();
	void				writeUpload();

	const int&			getClientFd() const;
	const std::string&	getResquestStr() const;
	const std::string&	getFileName() const;
	const std::string&	getBoundary() const;

};


#endif
