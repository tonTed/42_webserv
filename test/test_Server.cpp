#include "test_header.hpp"

TEST_CASE("Server")
{
	//test booting process
	Server serverTest;

	serverTest.initIndexInfo();
	for (int index = 0; index < POLLFD_LIMIT; index++)
	{
		CHECK(serverTest.getIIServerNum(index) == -1);
		CHECK(serverTest.getIIClientIndex(index) == -1);
		CHECK(serverTest.getIICGIReadIndex(index) == -1);
	}

	//record port simulation
	uint16_t	port[POLLFD_LIMIT];
	port[0] = 8080;
	port[1] = 8081;
	serverTest.setNbFdPort(2);
	CHECK(serverTest.getNbFdPort() == 2);
	try
	{
		serverTest.setPortSocket(port);
	}
	catch(const std::exception& e)
	{
		CHECK("EXCEPTION ON GOOD SETPORTPARAMETER" == 1);
	}
	
	


}
