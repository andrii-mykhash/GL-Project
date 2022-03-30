#ifndef GL_DOT_TEST_H
#define GL_DOT_TEST_H

#include "dot.h"

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>


class DotTest : public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE(DotTest);
    CPPUNIT_TEST(testEqual);
    CPPUNIT_TEST(testGreater);
    CPPUNIT_TEST(testLower);
    CPPUNIT_TEST_SUITE_END();

protected:
    void testEqual();
    void testGreater();
    void testLower();

public:
    void setUp();
    void tearDown();

private:
    Dot *pos_base, *pos_big, *pos_low;
};

#endif