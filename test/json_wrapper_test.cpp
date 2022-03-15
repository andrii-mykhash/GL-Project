#include "json_wrapper_test.h"
#include "../inc/json_wrapper.hpp"

void JsonWrapperTest::testConvertToJsonCBOR()
{
    auto cbor = map_to_cbor_json(test_map);
    CPPUNIT_ASSERT(cbor == predefined_cbor);
}

void JsonWrapperTest::testConvertToMap()
{
    auto map_from_json = json_cbor_to_map(predefined_cbor);
    CPPUNIT_ASSERT(test_map == map_from_json);
}

void JsonWrapperTest::setUp()
{
    User a{0, "12.21.1.11", {12, 22}}, b{1, "5.16.17.81", {23, 45}}, c{2, "8.1.81.91", {6, 7}};
    test_map[0] = a;
    test_map[1] = b;
    test_map[2] = c;
    predefined_cbor = {0xA3, 0x61, 0x30, 0xA3, 0x66, 0x63, 0x6F, 0x6F, 0x72, 0x64,
                       0x73, 0xA2, 0x61, 0x78, 0x0C, 0x61, 0x79, 0x16, 0x62, 0x69,
                       0x70, 0x6A, 0x31, 0x32, 0x2E, 0x32, 0x31, 0x2E, 0x31, 0x2E,
                       0x31, 0x31, 0x63, 0x75, 0x69, 0x64, 0x00, 0x61, 0x31, 0xA3,
                       0x66, 0x63, 0x6F, 0x6F, 0x72, 0x64, 0x73, 0xA2, 0x61, 0x78,
                       0x17, 0x61, 0x79, 0x18, 0x2D, 0x62, 0x69, 0x70, 0x6A, 0x35,
                       0x2E, 0x31, 0x36, 0x2E, 0x31, 0x37, 0x2E, 0x38, 0x31, 0x63,
                       0x75, 0x69, 0x64, 0x01, 0x61, 0x32, 0xA3, 0x66, 0x63, 0x6F,
                       0x6F, 0x72, 0x64, 0x73, 0xA2, 0x61, 0x78, 0x06, 0x61, 0x79,
                       0x07, 0x62, 0x69, 0x70, 0x69, 0x38, 0x2E, 0x31, 0x2E, 0x38,
                       0x31, 0x2E, 0x39, 0x31, 0x63, 0x75, 0x69, 0x64, 0x02};
}
// https://cbor.me/
// {"0":{"coords":{"x":12,"y":22},"ip":"12.21.1.11","uid":0},"1":{"coords":{"x":23,"y":45},"ip":"5.16.17.81","uid":1},"2":{"coords":{"x":6,"y":7},"ip":"8.1.81.91","uid":2}}
// a36130a366636f6f726473a26178c6179166269706a31322e32312e312e31316375696406131a366636f6f726473a26178176179182d6269706a352e31362e31372e38316375696416132a366636f6f726473a2617866179762697069382e312e38312e3931637569642

void JsonWrapperTest::tearDown()
{
}
