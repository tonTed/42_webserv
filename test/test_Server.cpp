#include "test_header.hpp"
#include <curl/curl.h>

TEST_CASE("Server::booting independant function")
{
		//record port simulation
	uint16_t	port[POLLFD_LIMIT];
	port[0] = 9000;
	port[1] = 9001;
	
	//test booting process
	Server serverTest;

	serverTest.indexInfoInit();
	serverTest.pollFdsInit();
	for (int index = 0; index < POLLFD_LIMIT; index++)
	{
		CHECK(serverTest.getIIServerNum(index) == UNSET);
		CHECK(serverTest.getIIClientIndex(index) == UNSET);
		CHECK(serverTest.getIICGIReadIndex(index) == UNSET);
	}

	serverTest.pollFdsInit();
	for (int index = 0; index < POLLFD_LIMIT; index++)
	{
		CHECK(serverTest.getPFFd(index) == UNSET);
		CHECK(serverTest.getPFEvents(index) == POLLIN);
		CHECK(serverTest.getPFRevents(index) == 0);
	}

	serverTest.setNbFdPort(2);
	CHECK(serverTest.getNbFdPort() == 2);
	try
	{
		serverTest.bootSocket(0);
		CHECK(serverTest.getPFFd(0) >= 3);
	}
	catch(const std::exception& e)
	{
		CHECK(1 == 2);
	}
	
	try
	{
		serverTest.bootSetSockOpt(0);
		CHECK(1 == 1);
	}
	catch(const std::exception& e)
	{
		CHECK(1 == 2);
	}

	try
	{
		serverTest.bootSetSockOpt(-1);
		CHECK(1 == 2);
	}
	catch(const std::exception& e)
	{
		CHECK(1 == 1);
	}

	try
	{
		serverTest.bootBind(0, port);
		CHECK(1 == 1);
	}
	catch(const std::exception& e)
	{
		CHECK(1 == 2);
	}
	

	try
	{
		serverTest.bootListen(0);
		CHECK(1 == 1);
	}
	catch(const std::exception& e)
	{
		CHECK(1 == 2);
	}

}

TEST_CASE("Server::setPortSocket")
{
		//record port simulation
	uint16_t	port[POLLFD_LIMIT];
	port[0] = 9000;
	port[1] = 9001;
	
	//test booting process
	Server serverTest;

	serverTest.indexInfoInit();
	serverTest.pollFdsInit();
	serverTest.setNbFdPort(2);

	try
	{
		serverTest.setPortSocket(port);
		CHECK(serverTest.getPFFd(0) >= 3);
		CHECK(serverTest.getPFFd(1) >= 3);
		CHECK(serverTest.getPFFd(2) == UNSET);

	}
	catch(const std::exception& e)
	{
		CHECK(1 == 2);
	}

}

TEST_CASE("Server::setPortSocket2")
{
		//record port simulation
	uint16_t	port[POLLFD_LIMIT];
	port[0] = 9000;
	port[1] = 9001;
	
	//test booting process
	Server serverTest;

	serverTest.indexInfoInit();
	serverTest.pollFdsInit();
	serverTest.setNbFdPort(2);
	serverTest.setPortSocket(port);

	CHECK(serverTest.pollIndexSignal() == -1);

	serverTest.setPFRevents(0, POLLIN);
	int signalIndex = serverTest.pollIndexSignal();
	CHECK(signalIndex == 0);
	CHECK(serverTest.getPFRevents(0) == 0);

	serverTest.setPFRevents(1, POLLIN);
	signalIndex = serverTest.pollIndexSignal();
	CHECK(signalIndex == 1);
	CHECK(serverTest.getPFRevents(1) == 0);

	CHECK(serverTest.pollFdsAvailable(2) == true);
	for (int index = 2; index < POLLFD_LIMIT; index++)
		serverTest.setPFFd(index, 10);
	CHECK(serverTest.pollFdsAvailable(2) == false);
	

}