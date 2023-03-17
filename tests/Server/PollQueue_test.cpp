#define DOCTEST_CONFIG_IMPLEMENT

#include <iostream>
#include "../doctest.hpp"
#include "../../src/Server/PollQueue.hpp"

TEST_CASE("PoolQueue::pollFdInit")
{
    std::cout << std::setw(30) << " ============ Test PoolQueue::pollFdInit ============ " << std::endl;
    //declare variables test and set
	unsigned int nbServer = 2;
	unsigned int serverFds[2] = {3, 4};
	unsigned int maxClient = 10;
	PollQueue	pollTested;
	pollfd* pollFds = pollTested.pollFdInit(serverFds, nbServer, maxClient);

//test
    try
    {
        for (int i = 0; i < nbServer + maxClient; i++)
		{
			if (i < nbServer)
			{
				CHECK(pollFds[i].fd == serverFds[i]);
				CHECK(pollFds[i].events == POLLIN);
			}
			else
			{
				CHECK(pollFds[i].fd == 0);
				CHECK(pollFds[i].events == 0);	
			}
			CHECK(pollFds[i].revents == 0);
		}
    }
    catch (const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }
	//clean data
	delete [] pollFds;

}

TEST_CASE("PoolQueue::pollFdAddClient")
{
    std::cout << std::setw(30) << " ============ Test PoolQueue::pollFdAddClient ============ " << std::endl;
    //declare variables test and set
	unsigned int nbServer = 2;
	unsigned int serverFds[2] = {3, 4};
	unsigned int maxClient = 2;
	PollQueue	pollTested;
	pollfd* pollFds = pollTested.pollFdInit(serverFds, nbServer, maxClient);

//test
    try
    {
		CHECK(pollTested.getNbServer() == nbServer);
		CHECK(pollTested.getPollFdSize() == nbServer + maxClient);
		CHECK(pollTested.pollFdAddClient(pollFds, 5) == true);
        for (int i = 0; i < nbServer; i++)
		{
			CHECK(pollFds[i].fd == serverFds[i]);
			CHECK(pollFds[i].events == POLLIN);
			CHECK(pollFds[2].revents == 0);
		}
		CHECK(pollFds[2].fd == 5);
		CHECK(pollFds[2].events == POLLIN);
		CHECK(pollFds[2].revents == 0);
		CHECK(pollFds[3].fd == 0);
		CHECK(pollFds[3].events == 0);
		CHECK(pollFds[3].revents == 0);

		CHECK(pollTested.pollFdAddClient(pollFds, 6) == true);
		for (int i = 0; i < nbServer; i++)
		{
			CHECK(pollFds[i].fd == serverFds[i]);
			CHECK(pollFds[i].events == POLLIN);
			CHECK(pollFds[2].revents == 0);
		}
		CHECK(pollFds[2].fd == 5);
		CHECK(pollFds[2].events == POLLIN);
		CHECK(pollFds[2].revents == 0);
		CHECK(pollFds[3].fd == 6);
		CHECK(pollFds[3].events == POLLIN);
		CHECK(pollFds[3].revents == 0);

		CHECK(pollTested.pollFdAddClient(pollFds, 7) == false);
    }
    catch (const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }
	//clean data
	delete [] pollFds;

}

TEST_CASE("PoolQueue::pollFdRemoveClient")
{
    std::cout << std::setw(30) << " ============ Test PoolQueue::pollFdRemoveClient ============ " << std::endl;
    //declare variables test and set
	unsigned int nbServer = 2;
	unsigned int serverFds[2] = {3, 4};
	unsigned int maxClient = 2;
	PollQueue	pollTested;
	pollfd* pollFds = pollTested.pollFdInit(serverFds, nbServer, maxClient);
	pollTested.pollFdAddClient(pollFds, 5);
	pollTested.pollFdAddClient(pollFds, 6);


//test
    try
    {
		pollTested.pollFdRemoveClient(pollFds, 5);
		for (int i = 0; i < nbServer; i++)
		{
			CHECK(pollFds[i].fd == serverFds[i]);
			CHECK(pollFds[i].events == POLLIN);
			CHECK(pollFds[2].revents == 0);
		}
		CHECK(pollFds[2].fd == 0);
		CHECK(pollFds[2].events == 0);
		CHECK(pollFds[2].revents == 0);
		CHECK(pollFds[3].fd == 6);
		CHECK(pollFds[3].events == POLLIN);
		CHECK(pollFds[3].revents == 0);

		pollTested.pollFdRemoveClient(pollFds, 6);
		for (int i = 0; i < nbServer; i++)
		{
			CHECK(pollFds[i].fd == serverFds[i]);
			CHECK(pollFds[i].events == POLLIN);
			CHECK(pollFds[2].revents == 0);
		}
		CHECK(pollFds[2].fd == 0);
		CHECK(pollFds[2].events == 0);
		CHECK(pollFds[2].revents == 0);
		CHECK(pollFds[3].fd == 0);
		CHECK(pollFds[3].events == 0);
		CHECK(pollFds[3].revents == 0);

    }
    catch (const PollQueue::FdNotFoundException &e)
	{
		CHECK("PollQueue::FdNotFoundException" == "NOT SUPPOSE TO");
	}
	catch (const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }

	try
	{
		pollTested.pollFdRemoveClient(pollFds, 7);
		CHECK("PollQueue::FdNotFoundException" == "NO EXCEPTION THRO");
	}
    catch (const PollQueue::FdNotFoundException &e)
	{
		CHECK(1 == 1);
	}
	catch (const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }
	
	//clean data
	delete [] pollFds;

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
