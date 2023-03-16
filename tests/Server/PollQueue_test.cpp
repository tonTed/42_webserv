#define DOCTEST_CONFIG_IMPLEMENT

#include <iostream>
#include "../doctest.hpp"
#include "../../src/Server/PollQueue.hpp"

TEST_CASE("Test Int Span")
{
    std::cout << std::setw(30) << " ============ Test Case 1 ============ " << std::endl;
    //declare variables test
	PollQueue::pollFdResetFd(1);

	//set variable test
    for (int i = 0; i < (int)count; i++)
        s.addNumber(i);

	
    try
    {
        CHECK(s.shortestSpan() == 1);
    }
    catch (const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }
    try
    {

        CHECK(s.longestSpan() == 99999);
    }
    catch (const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }
}

int main(int argc, char **argv)
{
    // Including
    doctest::Context ctx;
    ctx.setOption("abort-after", 5);  // default - stop after 5 failed asserts
    ctx.applyCommandLine(argc, argv); // apply command line - argc / argv
    ctx.setOption("no-breaks", true); // override - don't break in the debugger
    int res = ctx.run();              // run test cases unless with --no-run
    if (ctx.shouldExit())             // query flags (and --exit) rely on this
        return res;                   // propagate the result of the tests


    return res; // + your_program_res
}
