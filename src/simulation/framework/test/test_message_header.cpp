//
// Created by gabriel on 9/11/23.
//

#include "../system_messaging.h"

#include <doctest/doctest.h>

using namespace basilisk::messaging;

TEST_SUITE("Test MessageHeader") {

    TEST_CASE("Test size_of") {
        auto expected_size = 8 + 4 + 4 + 8 + 8 + 8 + 4 + 4 + 12;
        CHECK(MessageHeader::size_of("test_message") == expected_size);
    }

    TEST_CASE("Test get_update_counter") {
        MessageHeader test;
        test.update_counter = 2;

        auto test_bin = test.serialize();
        auto &update_ctr_ref = MessageHeader::get_update_counter(test_bin.data());

        CHECK(update_ctr_ref == 2);
        update_ctr_ref = 3;

        auto deserial = MessageHeader::deserialize(test_bin.data());
        CHECK(deserial.update_counter == 3);
    }

    TEST_CASE("Test get_current_read_buffer") {
        MessageHeader test;
        test.current_read_buffer = 2;

        auto test_bin = test.serialize();
        auto &update_ctr_ref = MessageHeader::get_current_read_buffer(test_bin.data());

        CHECK(update_ctr_ref == 2);
        update_ctr_ref = 3;

        auto deserial = MessageHeader::deserialize(test_bin.data());
        CHECK(deserial.current_read_buffer == 3);
    }

    TEST_CASE("Test get_max_number_buffers") {
        MessageHeader test;
        test.max_number_buffers = 2;

        auto test_bin = test.serialize();
        auto &update_ctr_ref = MessageHeader::get_max_number_buffers(test_bin.data());

        CHECK(update_ctr_ref == 2);
        update_ctr_ref = 3;

        auto deserial = MessageHeader::deserialize(test_bin.data());
        CHECK(deserial.max_number_buffers == 3);
    }

    TEST_CASE("Test get_message_size") {
        MessageHeader test;
        test.message_size = 2;

        auto test_bin = test.serialize();
        auto &update_ctr_ref = MessageHeader::get_message_size(test_bin.data());

        CHECK(update_ctr_ref == 2);
        update_ctr_ref = 3;

        auto deserial = MessageHeader::deserialize(test_bin.data());
        CHECK(deserial.message_size == 3);
    }

    TEST_CASE("Test get_current_read_frame") {
        MessageHeader test;
        test.current_read_frame = 2;

        auto test_bin = test.serialize();
        auto &update_ctr_ref = MessageHeader::get_current_read_frame(test_bin.data());

        CHECK(update_ctr_ref == 2);
        update_ctr_ref = 3;

        auto deserial = MessageHeader::deserialize(test_bin.data());
        CHECK(deserial.current_read_frame == 3);
    }

    TEST_CASE("Test get_buffer_offset") {
        MessageHeader test;
        test.buffer_offset = 2;

        auto test_bin = test.serialize();
        auto &buffer_offset_ref = MessageHeader::get_buffer_offset(test_bin.data());

        CHECK(buffer_offset_ref == 2);
        buffer_offset_ref = 3;

        auto deserial = MessageHeader::deserialize(test_bin.data());
        CHECK(deserial.buffer_offset == 3);
    }

    TEST_CASE("Test get_message_type") {
        MessageHeader test;
        test.type = MessageType::AUXILIARY;

        auto test_bin = test.serialize();
        auto &type_ref = MessageHeader::get_message_type(test_bin.data());

        CHECK(type_ref == MessageType::AUXILIARY);
    }

    TEST_CASE("Test get_message_name_length") {
        MessageHeader test;
        test.message_name_length = 12;
        test.message_name = "test_message";

        auto test_bin = test.serialize();
        auto &message_name_length_ref = MessageHeader::get_message_name_length(test_bin.data());

        CHECK(message_name_length_ref == 12);
    }
}