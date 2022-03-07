#include "field_test.h"
#include <string>

void FieldTest::testIsCollision()
{
    User temp;
    user_id_1 = test_field->createUser(std::string("10.10.10.10"));
    test_field->getUser(user_id_1,temp);
    temp.uid = 10;
    CPPUNIT_ASSERT(true == test_field->hasCollision(temp));
    temp.coords.x++;
    temp.coords.y++;
    CPPUNIT_ASSERT(false == test_field->hasCollision(temp));
}

void FieldTest::testMove()
{
    User user2, user3;
    User temp;
    test_field->getUser(user_id_2,user2);
    test_field->getUser(user_id_3,user3);
    temp.coords = user2.coords;
    temp.uid = user3.uid;
    CPPUNIT_ASSERT(test_field->move(temp) == 1);

    // temp.coords.x++;
    // CPPUNIT_ASSERT(test_field->move(temp) == 0);
}

void FieldTest::testGetUser()
{
    User temp;
    test_field->getUser(user_id_2, temp);
    CPPUNIT_ASSERT(temp.uid == user_id_2);
    CPPUNIT_ASSERT(temp.uid != user_id_3);
}

void FieldTest::setUp()
{
    test_field = new Field();
    user_id_2 = test_field->createUser(std::string("10.10.10.10"));
    user_id_3 = test_field->createUser(std::string("22.22.22.22"));
}

void FieldTest::tearDown()
{
    delete test_field;
}
