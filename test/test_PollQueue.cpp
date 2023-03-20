#include "test_header.hpp"

TEST_CASE("PollQueue::pollFdInit")
{
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
