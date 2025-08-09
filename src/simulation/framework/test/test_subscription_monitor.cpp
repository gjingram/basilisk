
#include "../system_messaging.h"

#include <doctest/doctest.h>
#include <vector>
#include <string>

using namespace basilisk::messaging;

TEST_SUITE("Test Subscription Monitor") {

    TEST_CASE("Test add_publisher") {
        SubscriptionMonitor sm;
        std::string publisher_name = "test_publisher";
        std::vector<std::string> fs{"test_friend1", "test_friend2"};

        sm.add_publisher("", fs);
        CHECK(sm.publishers.empty());
        CHECK(sm.friends.empty());
        CHECK(sm.subscriptions.empty());
        CHECK(sm.dependency_map.empty());

        sm.add_publisher(publisher_name, fs);
        CHECK(sm.publishers.find(publisher_name) != sm.publishers.end());
        CHECK(sm.friends.find(publisher_name) != sm.friends.end());
        CHECK(sm.subscriptions.find(publisher_name) != sm.subscriptions.end());
        CHECK(sm.friends[publisher_name].find("test_friend1") != sm.friends[publisher_name].end());
        CHECK(sm.friends[publisher_name].find("test_friend2") != sm.friends[publisher_name].end());
    }

    TEST_CASE("Test add_friend") {
        SubscriptionMonitor sm;
        std::string publisher_name = "test_publisher";

        sm.add_publisher(publisher_name);

        std::string friend_name = "test_friend";
        sm.add_friend(publisher_name, friend_name);
        CHECK(sm.publishers.find(publisher_name) != sm.publishers.end());
        CHECK(sm.friends.find(publisher_name) != sm.friends.end());
        CHECK(sm.subscriptions.find(publisher_name) != sm.subscriptions.end());
        CHECK(sm.friends[publisher_name].find(friend_name) != sm.friends[publisher_name].end());
    }

    TEST_CASE("Test add_subscriber") {
        SubscriptionMonitor sm;
        std::string sub1 = "sub1";
        std::string sub2 = "sub2";
        std::string sub3 = "";

        sm.add_subscriber(sub1);
        CHECK(sm.subscribers.find(sub1) != sm.subscribers.end());
        CHECK(sm.subscribers.find(sub2) == sm.subscribers.end());
        CHECK(sm.subscribers.find(sub3) == sm.subscribers.end());

        sm.add_subscriber(sub2);
        CHECK(sm.subscribers.find(sub1) != sm.subscribers.end());
        CHECK(sm.subscribers.find(sub2) != sm.subscribers.end());
        CHECK(sm.subscribers.find(sub3) == sm.subscribers.end());

        sm.add_subscriber(sub2);
        CHECK(sm.subscribers.find(sub1) != sm.subscribers.end());
        CHECK(sm.subscribers.find(sub2) != sm.subscribers.end());
        CHECK(sm.subscribers.find(sub3) == sm.subscribers.end());
        CHECK(sm.subscribers.size() == 2);

        sm.add_subscriber(sub3);
        CHECK(sm.subscribers.find(sub1) != sm.subscribers.end());
        CHECK(sm.subscribers.find(sub2) != sm.subscribers.end());
        CHECK(sm.subscribers.find(sub3) == sm.subscribers.end());
        CHECK(sm.subscribers.size() == 2);
    }

    TEST_CASE("Test add_message_type") {
        SubscriptionMonitor sm;
        std::string message_name = "test_message";

        sm.add_message_type(message_name, MessageType::AUXILIARY);
        CHECK(sm.message_types.find(message_name) != sm.message_types.end());
        CHECK(sm.message_types[message_name] == MessageType::AUXILIARY);

        sm.add_message_type(message_name, MessageType::OUTPUT);
        CHECK(sm.message_types.find(message_name) != sm.message_types.end());
        CHECK(sm.message_types[message_name] == MessageType::AUXILIARY);

        std::string message_name2 = "";
        sm.add_message_type(message_name2, MessageType::OUTPUT);
        CHECK(sm.message_types.find(message_name2) == sm.message_types.end());
    }

    TEST_CASE("Test check_subscription") {
        SubscriptionMonitor sm;

        std::string publisher_name = "test_publisher";

        std::string test_subscriber1 = "test_subscriber1";
        std::string test_subscriber2 = "test_subscriber2";
        std::string test_subscriber3 = "test_subscriber3";

        sm.add_publisher(publisher_name);
        sm.add_friend(publisher_name, test_subscriber1);
        sm.add_friend(publisher_name, test_subscriber2);

        sm.add_message_type("test_publisher::test_public_msg", MessageType::OUTPUT);
        sm.add_message_type("test_publisher::test_private_msg", MessageType::INTERNAL);

        CHECK(!sm.check_subscription("test_publisher", test_subscriber1));
        CHECK(!sm.check_subscription("test_publisher::test_msg", test_subscriber2));
        CHECK(!sm.check_subscription("test_publisher2::test_public_msg", test_subscriber1));
        CHECK(!sm.check_subscription("", test_subscriber1));
        CHECK(!sm.check_subscription("test_publisher::test_public_msg", ""));

        CHECK(sm.check_subscription("test_publisher::test_public_msg", test_subscriber1));
        CHECK(sm.check_subscription("test_publisher::test_public_msg", test_subscriber2));
        CHECK(sm.check_subscription("test_publisher::test_public_msg", test_subscriber3));

        CHECK(sm.check_subscription("test_publisher::test_private_msg", test_subscriber1));
        CHECK(sm.check_subscription("test_publisher::test_private_msg", test_subscriber2));
        CHECK(!sm.check_subscription("test_publisher::test_private_msg", test_subscriber3));
    }

    TEST_CASE("Test register_subscription") {
        SubscriptionMonitor sm;

        std::string publisher_name = "test_publisher";

        std::string test_subscriber1 = "test_subscriber1";
        std::string test_subscriber2 = "test_subscriber2";
        std::string test_subscriber3 = "test_subscriber3";

        sm.add_publisher(publisher_name);
        sm.add_friend(publisher_name, test_subscriber1);
        sm.add_friend(publisher_name, test_subscriber2);

        sm.add_message_type("test_publisher::test_public_msg", MessageType::OUTPUT);
        sm.add_message_type("test_publisher::test_private_msg", MessageType::INTERNAL);

        CHECK(!sm.register_subscription("test_publisher", test_subscriber1));
        CHECK(!sm.register_subscription("test_publisher::test_msg", test_subscriber2));
        CHECK(!sm.register_subscription("test_publisher2::test_public_msg", test_subscriber1));
        CHECK(!sm.register_subscription("", test_subscriber1));
        CHECK(!sm.register_subscription("test_publisher::test_public_msg", ""));

        CHECK(sm.subscribers.empty());
        CHECK(sm.subscriptions[publisher_name].empty());
        CHECK(sm.dependency_map.empty());

        CHECK(sm.register_subscription("test_publisher::test_public_msg", test_subscriber1));
        CHECK(sm.subscribers.find(test_subscriber1) != sm.subscribers.end());
        CHECK(sm.subscriptions[publisher_name].find(test_subscriber1) != sm.subscriptions[publisher_name].end());

        CHECK(sm.register_subscription("test_publisher::test_public_msg", test_subscriber2));
        CHECK(sm.subscribers.find(test_subscriber2) != sm.subscribers.end());

        CHECK(sm.register_subscription("test_publisher::test_private_msg", test_subscriber1));

        CHECK(sm.register_subscription("test_publisher::test_private_msg", test_subscriber2));

        CHECK(!sm.register_subscription("test_publisher::test_private_msg", test_subscriber3));
        CHECK(sm.subscribers.find(test_subscriber3) == sm.subscribers.end());
        CHECK(sm.subscriptions[publisher_name].find(test_subscriber3) == sm.subscriptions[publisher_name].end());

    }

    TEST_CASE("Test get_dependency_type") {
        SubscriptionMonitor sm;

        std::string publisher_name = "test_publisher";
        std::string subscriber_name = "test_subscriber";

        sm.add_publisher(publisher_name);
        sm.add_friend(publisher_name, subscriber_name);

        sm.add_message_type("test_publisher::test_public_msg", MessageType::OUTPUT);
        sm.add_message_type("test_publisher::test_private_msg", MessageType::INTERNAL);

        sm.register_subscription("test_publisher::test_public_msg", subscriber_name);
        CHECK(sm.get_dependency_type("test_publisher", subscriber_name) == MessageDependency::OUTPUT_DEPENDENCY);

        sm.register_subscription("test_publisher::test_private_msg", subscriber_name);
        CHECK(sm.get_dependency_type("test_publisher", subscriber_name) == MessageDependency::STATE_DEPENDENCY);

    }

    TEST_CASE("Test find_unsubscribed_messages") {
        SubscriptionMonitor sm;

        std::string publisher1 = "test_publisher1";
        std::string publisher2 = "test_publisher2";
        std::string publisher3 = "test_publisher3";

        std::string subscriber_name = "test_subscriber";

        sm.add_publisher(publisher1);
        sm.add_publisher(publisher2);
        sm.add_publisher(publisher3);

        sm.add_message_type("test_publisher1::test_msg", MessageType::OUTPUT);
        sm.add_message_type("test_publisher2::test_msg", MessageType::OUTPUT);
        sm.add_message_type("test_publisher3::test_msg", MessageType::OUTPUT);

        sm.register_subscription("test_publisher1::test_msg", subscriber_name);
        auto unsubscribed = sm.find_unsubscribed_publishers();

        CHECK(unsubscribed.size() == 2);
        CHECK(unsubscribed.find("test_publisher2") != unsubscribed.end());
        CHECK(unsubscribed.find("test_publisher3") != unsubscribed.end());
    }

}