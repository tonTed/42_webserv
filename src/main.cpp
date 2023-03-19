#define DOCTEST_CONFIG_IMPLEMENT

#include "global.hpp"
#include "ConfigServer/ConfigServer.hpp"


int main(int argc, const char **argv)
{
	int res;

	doctest::Context context;
	context.applyCommandLine(argc, argv);
	context.setOption("abort-after", 5);
	context.setOption("no-breaks", true);
	res = context.run();
	if (context.shouldExit())
		return (res);
	std::cout << std::endl;
	// Run your program here, after the tests have finished
	std::cout << "Launching webserver 42!" << std::endl
			  << std::endl;
	ConfigServer ConfigServer(getFile(argc, argv));

	return (res);
}
