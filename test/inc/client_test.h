#ifndef GL_CLIENT_TEST_H
#define GL_CLIENT_TEST_H

#include "client/client.h"

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>


class ClientTest : public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE(ClientTest);
    CPPUNIT_TEST(testIsCorrectChar);
    CPPUNIT_TEST(testInitAndSendMoveDirection);
    CPPUNIT_TEST_SUITE_END();

protected:
    void testIsCorrectChar();
    void testInitAndSendMoveDirection();

    void getUpServer();
    void closeServer();

public:
    void setUp();
    void tearDown();

private:
   Client client;
   int server_sock;
   int remote_sock;
   sockaddr_in addr;
};

#endif