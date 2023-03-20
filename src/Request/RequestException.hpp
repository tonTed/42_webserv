//
// Created by Teddy BLANCO on 2023-03-17.
//

#ifndef REQUESTEXCEPTION_HPP
#define REQUESTEXCEPTION_HPP

#include <exception>

class RequestException {

public:
	class ReadError : public std::exception {
		const char * what() const throw() {
			return ERR_SOCKET_READ;
		}
	};

	class MaxSize : public std::exception {
		const char * what() const throw() {
			return ERR_REQUEST_SIZE;
		}
	};

	class NoCRLF : public std::exception {
		const char * what() const throw() {
			return ERR_START_LINE_NO_CRLF;
		}
	};

	class InvalidLine : public std::exception {
		const char * what() const throw() {
			return ERR_START_LINE_INVALID_LINE;
		}
	};

	class FirstLine {
	public:

		class InvalidMethod : public std::exception {
			const char * what() const throw() {
				return ERR_START_LINE_INVALID_METHOD;
			}
		};

		class InvalidVersion : public std::exception {
			const char * what() const throw() {
				return ERR_START_LINE_INVALID_VERSION;
			}
		};

	};

	class Header {
	public:
		class DuplicateKey : public std::exception {
			const char * what() const throw() {
				return ERR_HEADER_DUPLICATE;
			}
		};
	};
};


#endif
