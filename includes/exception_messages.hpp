//
// Created by Teddy BLANCO on 2023-03-16.
//

#ifndef EXCEPTION_MESSAGES_HPP
# define EXCEPTION_MESSAGES_HPP

//Exception messages Request
# define ERR_REQUEST_SIZE "Request size is too big"
# define ERR_SOCKET_READ "Error reading socket"

# define ERR_NO_CRLF "Invalid line: no CRLF"
# define ERR_INVALID_LINE "Invalid line: Invalid line"

# define ERR_START_LINE_INVALID_METHOD "Invalid start line: invalid method"
# define ERR_START_LINE_INVALID_VERSION "Invalid start line: invalid version"
# define ERR_START_LINE_NOT_ALLOWED_METHOD "Invalid start line: not allowed method"
# define ERR_START_LINE_INVALID_URI "Invalid start line: invalid URI"

# define ERR_HEADER_DUPLICATE "Invalid header: duplicate header"
# define ERR_HEADER_MISSING_HEADER "Invalid header: missing header"
# define ERR_HEADER_INVALID_VALUE "Invalid header: invalid value"
# define ERR_HEADER_INVALID_KEY "Invalid header: invalid key"

#endif