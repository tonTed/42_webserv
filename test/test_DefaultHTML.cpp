#include "test_header.hpp"

std::string setHtml(int status, const std::string &message)
{
	return "<!DOCTYPE html>\n"
		   "<html>\n"
		   "<head>\n"
		   "\t<title> error: " + std::to_string(status) +
		   "</title>\n"
		   "</head>\n"
		   "<body>\n"
		   "\t<h1>Default HTML</h1>\n"
		   "\t<p>status: " + std::to_string(status) + " " + message +
		   " </p>\n"
		   "</body>\n"
		   "</html>\n";
}

TEST_CASE("DefaultHTML") {

	int 		status = 0;
	std::string message = "";
	std::string html_expected = "";
	std::string html = "";
	DefaultHTML defaultHTML;

	SUBCASE("200") {
		status = 200;
		message = defaultHTML._codeResponse.at(status);
		html_expected = setHtml(status, message);
		html = defaultHTML.getHtml(status);
		CHECK_MESSAGE(html == html_expected, "should return html 200");
	}

	SUBCASE("999 return 500 with bad status") {
		status = 999;
		message = defaultHTML._codeResponse.at(500);
		html_expected = setHtml(500, message);
		html = defaultHTML.getHtml(status);
		CHECK_MESSAGE(html == html_expected, "should return html 500");
	}
}