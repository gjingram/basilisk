
#include "../system_messaging.h"

#include <doctest/doctest.h>
#include <vector>
#include <string>

using namespace basilisk::messaging;

TEST_SUITE("Test System Messaging") {

    TEST_CASE("Test concatenate_message_parts") {
        std::vector<std::string> parts{"test1", "test2", "test3"};
        auto concat = concatenate_message_parts(parts);
        CHECK(concat == "test1::test2::test3");
    }

    TEST_CASE("Test split_message_name") {
        std::string test_str = "test1::test2::test3";
        auto res = split_message_name(test_str);
        CHECK(res[0] == "test1");
        CHECK(res[1] == "test2");
        CHECK(res[2] == "test3");
    }

    TEST_CASE("Test split_publisher") {
        std::string test_str = "test1::test2::test3::message_name";
        auto res = split_publisher(test_str);
        CHECK(res == "test1::test2::test3");
    }

    TEST_CASE("Test split_message") {
        std::string test_str = "test1::test2::test3::message_name";
        auto res = split_message(test_str);
        CHECK(res == "message_name");
    }

}