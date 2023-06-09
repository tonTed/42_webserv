//
// Created by Teddy BLANCO on 2023-03-17.
//

#ifndef REQUESTEXCEPTION_HPP
#define REQUESTEXCEPTION_HPP

#include <exception>

class RequestException {

public:
	class ReadError : public std::exception {
	public:
		const char * what() const throw() {
			return ERR_SOCKET_READ;
		}
	};

	class MaxSize : public std::exception {
	public:
		const char * what() const throw() {
			return ERR_REQUEST_SIZE;
		}
	};

	class NoCRLF : public std::exception {
	public:
		const char * what() const throw() {
			return ERR_NO_CRLF;
		}
	};

	class InvalidLine : public std::exception {
	public:
		const char * what() const throw() {
			return ERR_INVALID_LINE;
		}
	};

	class StartLine {
	public:

		class InvalidMethod : public std::exception {
		public:
			const char * what() const throw() {
				return ERR_START_LINE_INVALID_METHOD;
			}
		};

		class InvalidVersion : public std::exception {
		public:
			const char * what() const throw() {
				return ERR_START_LINE_INVALID_VERSION;
			}
		};

		class NotAllowedMethod : public std::exception {
		public:
			const char * what() const throw() {
				return ERR_START_LINE_NOT_ALLOWED_METHOD;
			}
		};

		class InvalidURI : public std::exception {
		public:
			const char * what() const throw() {
				return ERR_START_LINE_INVALID_URI;
			}
		};

	};

	class Header {
	public:
		class DuplicateKey : public std::exception {
		public:
			const char * what() const throw() {
				return ERR_HEADER_DUPLICATE;
			}
		};

		class MissingHeader : public std::exception {
		public:
			const char * what() const throw() {
				return ERR_HEADER_MISSING_HEADER;
			}
		};

		class InvalidValue : public std::exception {
		public:
			const char * what() const throw() {
				return ERR_HEADER_INVALID_VALUE;
			}
		};

		class InvalidKey : public std::exception {
		public:
			const char * what() const throw() {
				return ERR_HEADER_INVALID_KEY;
			}
		};
	};
};


#endif
