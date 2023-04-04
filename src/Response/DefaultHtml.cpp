//
// Created by Teddy Blanco on 4/4/23.
//

#include "DefaultHtml.hpp"

std::string DefaultHTML::getHtml(int status) {
	std::string html;

	try {
		_codeResponse.at(status);
	}
	catch (std::out_of_range &e) {
		(void)e;
		status = 500;
	}

	const std::string& message = _codeResponse.at(status);

	html = "<!DOCTYPE html>\n"
		   "<html>\n"
		   "<head>\n"
		   "\t<title> error: " + std::to_string(status) + "</title>\n"
		   "</head>\n"
		   "<body>\n"
		   "\t<h1>Default HTML</h1>\n"
		   "\t<p>status: " + std::to_string(status) + " " + message + " </p>\n"
		   "</body>\n"
		   "</html>\n";

	return html;
}