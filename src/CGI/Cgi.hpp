#ifndef CGI_HPP
#define CGI_HPP

/*
Key				Value
DOCUMENT_ROOT	The root directory of your server
HTTP_COOKIE		The visitor's cookie, if one is set
HTTP_HOST		The hostname of the page being attempted
HTTP_REFERER	The URL of the page that called your program
HTTP_USER_AGENT	The browser type of the visitor
HTTPS			"on" if the program is being called through a secure server
PATH			The system path your server is running under
QUERY_STRING	The query string (see GET, below)
REMOTE_ADDR		The IP address of the visitor
REMOTE_HOST		The hostname of the visitor (if your server has reverse-name-lookups on; otherwise this is the IP address again)
REMOTE_PORT		The port the visitor is connected to on the web server
REMOTE_USER		The visitor's username (for .htaccess-protected pages)
REQUEST_METHOD	GET or POST
REQUEST_URI		The interpreted pathname of the requested document or CGI (relative to the document root)
SCRIPT_FILENAME	The full pathname of the current CGI
SCRIPT_NAME		The interpreted pathname of the current CGI (relative to the document root)
SERVER_ADMIN	The email address for your server's webmaster
SERVER_NAME		Your server's fully qualified domain name (e.g. www.cgi101.com)
SERVER_PORT		The port number your server is listening on
SERVER_SOFTWARE	The server software you're using (e.g. Apache 1.3)

AUTH_TYPE		Il s'agit de la méthode d'authentification qui a été utilisée par le client pour accéder au programme CGI
CONTENT_LENGTH	Longueur du corps de la requête. Il s'agit de la taille des données envoyées au CGI par l'intermédiaire de la méthode POST d'un formulaire
CONTENT_TYPE	Type de données contenu dans le corps de la requête. Il s'agit du type MIME des données
HTTP_COOKIE		Cookie du client si jamais un cookie est effectivement présent sur le disque du client
HTTP_REFERER	URL de la page qui a appelé le script CGI
HTTP_REQUEST_METHOD	Cette variable indique le type de méthode utilisée pour envoyer les données au programme CGI. Il s'agit de GET, HEAD, POST, PUT ou DELETE. Dans le cas de la méthode GET, les données sont encodées avec l'URL de requête, dans le cas de la méthode POST, par contre, les données présentes dans le corps de la requête sont accessibles via le fichier de flux d'entrée standard nommé STDIN
HTTP_USER_AGENT	Cette variable permet d'avoir des informations sur le type de navigateur utilisé par le client, ainsi que son système d'exploitation
PATH	Il s'agit du chemin d'accès au script CGI
PATH_INFO	Il s'agit de la partie de l'URL (ayant servi à accéder au script CGI) située avant le point d'interrogation. Il s'agit donc du chemin d'accès au script CGI sans prendre en compte les données supplémentaires
PATH_TRANSLATED	Il s'agit du chemin d'accès absolu au script CGI, c'est-à-dire l'emplacement absolu du script sur le serveur
QUERY_STRING	Il s'agit de la partie de l'URL (ayant servi à accéder au script CGI) située après le point d'interrogation. C'est de cette manière que sont transmises les données d'un formulaire dans le cas de la méthode GET
REMOTE_ADDR	Cette variable contient l'adresse IP du client appelant le script CGI
REMOTE_USER	Cette variable n'existe que si le client a été soumis à une authentification, et contient alors le couple nom_d_utilisateur/mot_de_passe
SCRIPT_FILENAME	Chemin d'accès complet au script CGI
SCRIPT_NAME	Chemin d'accès relatif (par rapport au chemin d'accès à la racine web) au script CGI
SERVER_PORT	Port TCP auquel les données ont été envoyées (généralement le port 80)
SERVER_PROTOCOL	Nom et version du protocole utilisé pour envoyer la requête au script CGI
*/

#include "../../includes/webserv.hpp"
#include "../Request/Request.hpp"


class CGI {

public:
	CGI(const Request &request);
	~CGI();

public:

	void setEnvp();

	const Request & 	_request;
	std::vector<string>	_envp;
	const ServerData	_serverData;


};


#endif
