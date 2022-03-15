#ifndef GL_JSON_WRAPPER_TEST_H
#define GL_JSON_WRAPPER_TEST_H

#include "../inc/user.hpp"
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

class JsonWrapperTest : public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE(JsonWrapperTest);
    CPPUNIT_TEST(testConvertToJsonCBOR);
    CPPUNIT_TEST(testConvertToMap);
    CPPUNIT_TEST_SUITE_END();

protected:
    void testConvertToJsonCBOR();
    void testConvertToMap();

public:
    void setUp();
    void tearDown();

private:
    std::map<int,User> test_map;
    std::vector<std::uint8_t> predefined_cbor;
};

#endif