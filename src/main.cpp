#define DOCTEST_CONFIG_IMPLEMENT

#include "global.hpp"
#include <iostream>

// TEST_CASE("Test Int Span")
// {
//     std::cout << std::setw(30) << " ============ Test Case 1 ============ " << std::endl;

// }

int	main(int argc, char **argv)
{
	int	res;

	doctest::Context context;
	context.applyCommandLine(argc, argv);
	context.setOption("abort-after", 5);
	context.setOption("no-breaks", true);
	res = context.run();
	if (context.shouldExit())
		return (res);
	std::cout << std::endl;
	std::cout << "Launching webserver 42!" << std::endl;
	// Run your program here, after the tests have finished
	// ...
	return (res);
}
