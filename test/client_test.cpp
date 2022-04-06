#include "inc/client_test.h"

#include <arpa/inet.h>
#include <memory.h>
#include <thread>
#include <unistd.h>

void ClientTest::testIsCorrectChar()
{
    CPPUNIT_ASSERT_EQUAL(true, client.isMoveCorrectChar('q'));
    CPPUNIT_ASSERT_EQUAL(false, client.isMoveCorrectChar('c'));
    CPPUNIT_ASSERT_EQUAL(true, client.isMoveCorrectChar('w'));
    CPPUNIT_ASSERT_EQUAL(false, client.isMoveCorrectChar('1'));
    CPPUNIT_ASSERT_EQUAL(true, client.isMoveCorrectChar('a'));
    CPPUNIT_ASSERT_EQUAL(false, client.isMoveCorrectChar('+'));
    CPPUNIT_ASSERT_EQUAL(true, client.isMoveCorrectChar('s'));
    CPPUNIT_ASSERT_EQUAL(false, client.isMoveCorrectChar('`'));
    CPPUNIT_ASSERT_EQUAL(true, client.isMoveCorrectChar('d'));
    CPPUNIT_ASSERT_EQUAL(false, client.isMoveCorrectChar('/'));
}

void ClientTest::testInitAndSendMoveDirection()
{
    getUpServer();
    char recv_char;
    socklen_t length = sizeof(addr);
    std::thread t(
        [&]()
        {
            client.init("127.0.0.1", true);
            client.sendMoveDirection('q');
        });
    t.detach();

    if ((remote_sock = accept(server_sock, (sockaddr *)&addr, &length)) < 0)
    {
        CPPUNIT_FAIL("Accept client error");
    }
    CPPUNIT_ASSERT(remote_sock > -1);
    CPPUNIT_ASSERT(remote_sock == 4);

    int res = recv(remote_sock, &recv_char, sizeof(recv_char), 0);

    CPPUNIT_ASSERT_EQUAL(true, res > 0);
    CPPUNIT_ASSERT_EQUAL(true, recv_char == 'q');
    closeServer();
}

void ClientTest::setUp()
{}

void ClientTest::tearDown()
{}

void ClientTest::getUpServer()
{
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1)
    {
        CPPUNIT_FAIL("Socket creation error");
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8088);
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);

    if (bind(server_sock, (sockaddr *)&addr, sizeof(addr)) == -1)
    {
        CPPUNIT_FAIL("Bind socket error");
    }

    if (listen(server_sock, 1) < 0)
    {
        CPPUNIT_FAIL("Set socket in listen state error");
    }
}

void ClientTest::closeServer()
{
    shutdown(server_sock, SHUT_RDWR);
    close(server_sock);
}