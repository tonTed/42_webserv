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
		   "<meta charset=\"UTF-8\">\n"
		   "<title>Title</title>"
		   " <style>\n"
		   "    .navbar .active > a {\n"
		   "      background-color: #343a40;\n"
		   "      color: white;\n"
		   "    }\n"
		   "  </style>"
		   "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-KK94CHFLLe+nY2dmCWGMq91rCGa5gtU4mk92HdvYe+M/SXH301p5ILy+dN9+nJOZ\" crossorigin=\"anonymous\">\n"
		   "</head>\n"
		   "<body>\n"
		   "<nav class=\"navbar navbar-expand navbar-dark bg-dark\">\n"
		   "  <a class=\"navbar-brand\" href=\"#\">webserv42</a>\n"
		   "  <ul class=\"navbar-nav\">\n"
		   "    <li class=\"nav-item\">\n"
		   "      <a class=\"nav-link active\" href=#>Accueil</a>\n"
		   "    </li>\n"
		   "    <li class=\"nav-item\">\n"
		   "      <a class=\"nav-link\" href=\"televerser.html\">Téléverser</a>\n"
		   "    </li>\n"
		   "    <li class=\"nav-item\">\n"
		   "      <a class=\"nav-link\" href=\"team.html\">Notre équipe</a>\n"
		   "    </li>\n"
		   "    <li class=\"nav-item\">\n"
		   "      <a class=\"nav-link\" href=\"cgi.html\">CGI</a>\n"
		   "    </li>\n"
		   "  </ul>\n"
		   "</nav>"
		   "\t<title> error: " + std::to_string(status) + "</title>\n"
		   "</head>\n"
		   "<body>\n"
		   "\t<h1>Default HTML</h1>\n"
		   "\t<p>status: " + std::to_string(status) + " " + message + " </p>\n"
		   "</body>\n"
		   "</html>\n";

	return html;
}

std::string DefaultHTML::getHtmlError(int status) {
	std::string html;
	std::string headers;

	try {
		_codeResponse.at(status);
	}
	catch (std::out_of_range &e) {
		(void)e;
		status = 500;
	}

	const std::string& message = _codeResponse.at(status);

	html = DefaultHTML::getHtml(status);

	headers = "HTTP/1.1 " + std::to_string(status) + " " + message + "\r\n";
	headers += "Content-Type: text/html\r\n";
	headers += "Content-Length: " + std::to_string(html.length()) + "\r\n";
	headers += "Connection: close\r\n";

	html = headers + "\r\n" + html;

	return html;
}

