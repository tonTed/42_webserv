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
		CHECK(serverTest.getIIServerNum(index) == -1);
		CHECK(serverTest.getIIClientIndex(index) == -1);
		CHECK(serverTest.getIICGIReadIndex(index) == -1);
	}

	serverTest.pollFdsInit();
	for (int index = 0; index < POLLFD_LIMIT; index++)
	{
		CHECK(serverTest.getPFFd(index) == -1);
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
		CHECK(serverTest.getPFFd(2) == -1);

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

	serverTest.setPFRevents(1, POLLIN);
	int signalIndex = serverTest.pollIndexSignal();
	CHECK(signalIndex == 1);
	CHECK(serverTest.getPFRevents(1) == 0);

}