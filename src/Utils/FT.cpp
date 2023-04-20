//
// Created by Teddy BLANCO on 2023-04-19.
//

#include "FT.hpp"

int	FT::atoi(const std::string &str) {
	int result = 0;
	int sign = 1;

	// skip leading whitespace
	size_t i = 0;
	while (i < str.length() && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r' || str[i] == '\f' || str[i] == '\v')) {
		++i;
	}

	// handle optional sign
	if (i < str.length() && str[i] == '-') {
		sign = -1;
		++i;
	} else if (i < str.length() && str[i] == '+') {
		++i;
	}

	// process digits
	while (i < str.length() && str[i] >= '0' && str[i] <= '9') {
		result = result * 10 + (str[i] - '0');
		++i;
	}

	// check for remaining non-whitespace characters
	while (i < str.length()) {
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r' && str[i] != '\f' && str[i] != '\v') {
			throw std::invalid_argument("invalid character in atoi input");
		}
		++i;
	}

	return sign * result;
}