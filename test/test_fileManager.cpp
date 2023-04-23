#include "test_header.hpp"

using namespace std;

# define NB_GOOD_CASE 1

string	goodBody[] = {
	"------WebKitFormBoundary7MA4YWxkTrZu0gW\n"
	"Content-Disposition: form-data; name=\"fieldName\"; filename=\"example.txt\"\n"
	"Content-Type: text/plain\n"
	"\n"
	"<file contents here>\n"
	"------WebKitFormBoundary7MA4YWxkTrZu0gW-"
};

string	goodFilename[] = {
	"example.txt"
};

string	goodContent[] {
	"<file contents here>"
};

TEST_CASE("FileManager:extractFct")
{
	
	for (int i = 0; i < NB_GOOD_CASE; i++)
	{
		{
			FileManager	fileM(goodBody[i]);;
			
			CHECK(fileM.getBody() == goodBody[i]);
			CHECK(fileM.extractFilename() == true);
			CHECK(fileM.getFileName() == goodFilename[i]);
			CHECK(fileM.extractFileContent() == true);
			CHECK(fileM.getContent() == goodContent[i]);
		}
	}
}