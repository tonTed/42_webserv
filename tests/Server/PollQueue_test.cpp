#define DOCTEST_CONFIG_IMPLEMENT

#include <iostream>
#include "../doctest.hpp"
#include "../../src/Server/PollQueue.hpp"

TEST_CASE("PollQueue::pollFdInit")
{
    std::cout << std::setw(30) << " ============ Test PollQueue::pollFdInit ============ " << std::endl;
    //declare variables test and set
	unsigned int nbServer = 2;
	unsigned int serverFds[2] = {3, 4};
	unsigned int maxClient = 2;
	PollQueue	pollTested;
	pollTested.setNbServer(nbServer);
	pollTested.setPollFdSize(nbServer + maxClient);
	pollfd* pollFds = pollTested.pollFdInit();


//test
    try
    {
		CHECK(pollTested.getNbServer() == nbServer);
		CHECK(pollTested.getPollFdSize() == nbServer + maxClient);
        CHECK(pollTested.pollFdAdd(pollFds, serverFds[0]) >= 0);
        CHECK(pollTested.pollFdAdd(pollFds, serverFds[1]) >= 0);
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

TEST_CASE("PollQueue::pollFdAdd")
{
    std::cout << std::setw(30) << " ============ Test PollQueue::pollFdAdd ============ " << std::endl;
    //declare variables test and set
	unsigned int nbServer = 2;
	unsigned int serverFds[2] = {3, 4};
	unsigned int maxClient = 2;
	PollQueue	pollTested;
	pollTested.setNbServer(nbServer);
	pollTested.setPollFdSize(pollTested.getNbServer() + maxClient);
	pollfd* pollFds = pollTested.pollFdInit();
	pollTested.pollFdAdd(pollFds, serverFds[0]);
	pollTested.pollFdAdd(pollFds, serverFds[1]);

//test
    try
    {
		CHECK(pollTested.getNbServer() == nbServer);
		CHECK(pollTested.getPollFdSize() == nbServer + maxClient);
		CHECK(pollTested.pollFdAdd(pollFds, 5) >= 0);
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

		CHECK(pollTested.pollFdAdd(pollFds, 6) >= 0);
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

		CHECK(pollTested.pollFdAdd(pollFds, 7) == -1);
    }
    catch (const std::runtime_error &e)
    {
        std::cout << e.what() << std::endl;
    }
	//clean data
	delete [] pollFds;

}

TEST_CASE("PollQueue::pollFdRemoveClient")
{
    std::cout << std::setw(30) << " ============ Test PollQueue::pollFdRemove ============ " << std::endl;
    //declare variables test and set
	unsigned int nbServer = 2;
	unsigned int serverFds[2] = {3, 4};
	unsigned int maxClient = 2;
	PollQueue	pollTested;
	pollTested.setNbServer(nbServer);
	pollTested.setPollFdSize(pollTested.getNbServer() + maxClient);
	pollfd* pollFds = pollTested.pollFdInit();
	pollTested.pollFdAdd(pollFds, serverFds[0]);
	pollTested.pollFdAdd(pollFds, serverFds[1]);
	pollTested.pollFdAdd(pollFds, 5);
	pollTested.pollFdAdd(pollFds, 6);


//test
    try
    {
		pollTested.pollFdRemove(pollFds, 5);
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

		pollTested.pollFdRemove(pollFds, 6);
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
		pollTested.pollFdRemove(pollFds, 7);
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
