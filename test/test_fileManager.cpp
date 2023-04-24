#include "test_header.hpp"

using namespace std;

# define NB_GOOD_CASE 2
string	goodBody[] = {
	"------WebKitFormBoundary7MA4YWxkTrZu0gW\n"
	"Content-Disposition: form-data; name=\"fieldName\"; filename=\"example.txt\"\n"
	"Content-Type: text/plain\n"
	"\n"
	"<file contents here>\n"
	"------WebKitFormBoundary7MA4YWxkTrZu0gW-",
	"------WebKitFormBoundary7MA4YWxkTrZu0gW\n"
	"Content-Disposition: form-data; name=\"fieldName\"; filename=\"example2.txt\"\n"
	"Content-Type: text/plain\n"
	"\n"
	"plein\n"
	"de\n"
	"lignes\n"
	"de\n"
	"contenu\n"
	"------WebKitFormBoundary7MA4YWxkTrZu0gW-\n"
	"body qui sert pas\n"
	"autre.....\n"
	"bla bla bla"
};

string	goodFilename[] = {
	"example.txt",
	"example2.txt"
};

string	goodContent[] {
	"<file contents here>",
	"plein\n"
	"de\n"
	"lignes\n"
	"de\n"
	"contenu"
};


# define NB_BAD_CASE_FILENAME 3
string	badBody_filename[] = {
	"------WebKitFormBoundary7MA4YWxkTrZu0gW\n"
	"Content-Disposition: form-data; name=\"fieldName\";\n"
	"Content-Type: text/plain\n"
	"\n"
	"<file contents here>\n"
	"------WebKitFormBoundary7MA4YWxkTrZu0gW-",
	"------WebKitFormBoundary7MA4YWxkTrZu0gW\n"
	"Content-Disposition: form-data; name=\"fieldName\"; filename=\"\"\n"
	"Content-Type: text/plain\n"
	"\n"
	"<file contents here>\n"
	"------WebKitFormBoundary7MA4YWxkTrZu0gW-",
		"------WebKitFormBoundary7MA4YWxkTrZu0gW\n"
	"Content-Disposition: form-data; name=\"fieldName\"; filename=\"\n"
	"Content-Type: \"text/plain\n"
	"\n"
	"<file contents here>\n"
	"------WebKitFormBoundary7MA4YWxkTrZu0gW-"
};

# define NB_BAD_CASE_CONTENT 3
string	badBody_content[] = {
	"Content-Disposition: form-data; name=\"fieldName\"; filename=\"example.txt\"\n"
	"Content-Type: text/plain\n"
	"<file contents here>\n"
	"------WebKitFormBoundary7MA4YWxkTrZu0gW-",
		"Content-Disposition: form-data; name=\"fieldName\"; filename=\"example.txt\"\n"
	"Content-Type: text/plain\n"
	"\n"
	"<file contents here>\n",
	"Content-Disposition: form-data; name=\"fieldName\"; filename=\"example.txt\"\n"
	"Content-Type: text/plain\n"
	"\n"
	"\n"
	"------WebKitFormBoundary7MA4YWxkTrZu0gW-"
};

TEST_CASE("FileManager:extractFct")
{
	
	SUBCASE("GOOD CASES")
	{
		for (int i = 0; i < NB_GOOD_CASE; i++)
		{
			{
				FileManager	fileM(goodBody[i]);
				CHECK(fileM.getBody() == goodBody[i]);
				CHECK(fileM.extractFilename() == true);
				CHECK(fileM.getFileName() == UPLOADFILE_PATH + goodFilename[i]);
				CHECK(fileM.extractFileContent() == true);
				CHECK(fileM.getContent() == goodContent[i]);
			}
		}
	}

	SUBCASE("BAD CASES: FILENAME")
	{
		for (int i = 0; i < NB_BAD_CASE_FILENAME; i++)
		{
			{
				FileManager	fileM(badBody_filename[i]);;

				CHECK(fileM.getBody() == badBody_filename[i]);
				CHECK(fileM.extractFilename() == false);
				if (fileM.extractFilename() == true)
					MESSAGE(badBody_filename[i]);
				CHECK(fileM.getFileName() == "");
			}
		}
	}

	SUBCASE("BAD CASES: FILECONTENT")
	{
		for (int i = 0; i < NB_BAD_CASE_CONTENT; i++)
		{
			{
				FileManager	fileM(badBody_content[i]);;

				CHECK(fileM.getBody() == badBody_content[i]);
				CHECK(fileM.extractFileContent() == false);
				CHECK(fileM.getContent() == "");
				if (fileM.extractFileContent() == true || fileM.getContent() != "")
					MESSAGE("Case:" + to_string(i) + "\n" + badBody_content[i]);
			}
		}
	}
}

bool	isFileExist(const std::string& file)
{
	std::ifstream	ifs(file);
	if (ifs.is_open())
	{
		ifs.close();
		return true;
	}
	return false;
}

std::string	filebodycontent(const std::string& file)
{
	if (isFileExist(file) == true)
	{
		std::ifstream	ifs(file);
		std::ostringstream	oss;
		oss << ifs.rdbuf();
		ifs.close();
		return oss.str();
	}
	return "";
}



TEST_CASE("FileManager: Save & delete")
{
	SUBCASE("GOOD CASE")
	{
		for (int i = 0; i < NB_GOOD_CASE; i++)
		{
			{
				FileManager	fileM(goodBody[i]);

				CHECK(fileM.saveFile() == true);
				CHECK(isFileExist(fileM.getFileName()) == true);
				CHECK(filebodycontent(fileM.getFileName()) == goodContent[i]);
				CHECK(fileM.deleteFile() == true);
				CHECK(isFileExist(fileM.getFileName()) == false);
			}
		}
	}

	SUBCASE("BAD CASES: FILENAME")
	{
		for (int i = 0; i < NB_BAD_CASE_FILENAME; i++)
		{
			{
				FileManager	fileM(badBody_filename[i]);;

				CHECK(fileM.saveFile() == false);
			}
		}
	}

	SUBCASE("BAD CASES: FILECONTENT")
	{
		for (int i = 0; i < NB_BAD_CASE_CONTENT; i++)
		{
			{
				FileManager	fileM(badBody_content[i]);;

				CHECK(fileM.saveFile() == false);
			}
		}
	}

	SUBCASE("BAD CASES: DELETE MISSING FILE")
	{
		FileManager fileM(goodBody[0]);
		CHECK(fileM.deleteFile() == false);
	}
}
