//
// Created by Teddy BLANCO on 2023-03-16.
//
#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING

#include "test_header.hpp"

int main(int argc, char** argv) {
	doctest::Context context;

	context.applyCommandLine(argc, argv);
	context.setOption("no-breaks", true);


	int res = context.run();
	if (context.shouldExit())
		return res;
	return res;
}