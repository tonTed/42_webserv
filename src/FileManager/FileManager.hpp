#ifndef FILEMANAGER_HPP
# define FILEMANAGER_HPP

# include "../../includes/webserv.hpp"

# define UPLOADFILE_PATH ""

class FileManager {
private:
	std::string	_body;
	std::string	_fileName;
	std::string	_fileContent;

public:

	FileManager(const std::string &body);
	~FileManager();

	bool	saveFile();
	bool	deleteFile();

	bool	extractFilename();
	bool	extractFileContent();

	const std::string&	getBody() const;
	const std::string&	getFileName() const;
	const std::string&	getContent() const;

};


#endif
