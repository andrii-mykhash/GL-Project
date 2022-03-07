#include "dot_test.h"

void DotTest::setUp()
{
    pos_base = new Dot();
    pos_base->x = 10;
    pos_base->y = 11;

    pos_big = new Dot();
    pos_big->x = 20;
    pos_big->y = 22;

    pos_low = new Dot();
    pos_low->x = 5;
    pos_low->y = 7;
}

void DotTest::tearDown()
{
    delete pos_base;
    delete pos_big;
    delete pos_low;
}

void DotTest::testEqual()
{
    CPPUNIT_ASSERT(((pos_base <=> pos_base) == 0));
    CPPUNIT_ASSERT(((pos_base <=> pos_big) != 0));
    CPPUNIT_ASSERT(((pos_low <=> pos_big) != 0));
    CPPUNIT_ASSERT(((pos_low <=> pos_base) != 0));
}
void DotTest::testGreater()
{
    CPPUNIT_ASSERT(((pos_base <=> pos_low) > 0));
    CPPUNIT_ASSERT(((pos_big <=> pos_low) > 0));
    CPPUNIT_ASSERT(((pos_base <=> pos_big) < 0));
}
void DotTest::testLower()
{
    CPPUNIT_ASSERT(((pos_big <=> pos_low) < 0));
    CPPUNIT_ASSERT(((pos_base <=> pos_big) != 0));
}
