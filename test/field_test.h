#ifndef GL_FIELD_TEST_H
#define GL_FIELD_TEST_H

#include "../inc/field.h"
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>


class FieldTest : public CppUnit::TestFixture
{
private:
    CPPUNIT_TEST_SUITE(FieldTest);
    CPPUNIT_TEST(testIsCollision);
    CPPUNIT_TEST(testGetUser);
    CPPUNIT_TEST(testMove);
    CPPUNIT_TEST_SUITE_END();

protected:
    void testIsCollision();
    void testMove();
    void testGetUser();

public:
    void setUp();
    void tearDown();

private:
    Field *test_field;
    int user_id_1,  user_id_2,  user_id_3;
};

#endif