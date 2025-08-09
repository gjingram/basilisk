//
// Created by gabriel on 9/11/23.
//

#include "framework/system_messaging.h"
#include "utilities/bsk_logging.h"

namespace basilisk::messaging {

void SubscriptionMonitor::add_publisher(
        const std::string& publisher_name,
        const std::vector<std::string>& fs
)
{
    if (publisher_name.empty()) {
        logger.bskLog(logLevel_t::BSK_ERROR, "Publisher name cannot be empty");
        return;
    }
    publishers.insert(publisher_name);
    if (friends.find(publisher_name) == friends.end()) {
        friends.insert(std::make_pair(publisher_name, std::set<std::string>()));
    }
    for (const auto& f : fs) {
        if (f.empty()) {
            std::string msg = "Empty friend string for publisher: " + publisher_name;
            logger.bskLog(
                    logLevel_t::BSK_WARNING,
                    msg.c_str()
            );
            continue;
        }
        friends[publisher_name].insert(f);
    }
    if (subscriptions.find(publisher_name) == subscriptions.end()) {
        subscriptions.insert(std::make_pair(publisher_name, std::set<std::string>()));
    }
}

void SubscriptionMonitor::add_friend(
        const std::string& publisher_name,
        const std::string& friend_name
        )
{
    if (publisher_name.empty()) {
        std::string msg = "Publisher name empty in SubscriptionMonitor::add_friend";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return;
    }
    if (friend_name.empty()) {
        std::string msg = "Friend name empty in SubscriptionMonitor::add_friend";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return;
    }
    if (publishers.find(publisher_name) == publishers.end()) {
        std::string msg = "Publisher name not found in SubscriptionMonitor::add_friend";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return;
    }
    add_publisher(publisher_name, {friend_name});
}

void SubscriptionMonitor::add_subscriber(const std::string& name) {
    if (name.empty()) {
        std::string msg = "Subscriber name empty in SubscriptionMonitor::add_subscriber";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return;
    }
    subscribers.insert(name);
}

void SubscriptionMonitor::add_message_type(
        const std::string& message_name,
        MessageType type
        )
{
    if (message_name.empty()) {
        std::string msg = "Message name empty in SubscriptionMonitor::add_message_type";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return;
    }
    if (message_types.find(message_name) != message_types.end()) {
        std::string msg = "Message name already exists in SubscriptionMonitor::add_message_type";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return;
    }
    message_types.insert(std::make_pair(message_name, type));
}

bool SubscriptionMonitor::check_subscription(
        const std::string &message_name,
        const std::string &subscriber
        )
{
    if (message_name.empty()) {
        std::string msg = "Message name empty in SubscriptionMonitor::check_subscription";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return false;
    }
    if (subscriber.empty()) {
        std::string msg = "Subscriber name empty in SubscriptionMonitor::check_subscription";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return false;
    }
    auto msg_parts = split_message_name(message_name);
    if (msg_parts.size() < 2) {
        std::string msg =
                "Message name " +
                message_name +
                " not formatted correctly in SubscriptionMonitor::check_subscription";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return false;
    }
    auto pub_name = split_publisher(message_name);

    auto pub_found = publishers.find(pub_name);
    if (pub_found == publishers.end()) {
        std::string msg =
                "Publisher name " +
                pub_name +
                " not found in SubscriptionMonitor::check_subscription";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return false;
    }
    auto &pub_friends = friends[pub_name];

    auto type_found = message_types.find(message_name);
    if (type_found == message_types.end()) {
        std::string msg =
                "Message type for " +
                message_name +
                " not found in SubscriptionMonitor::check_subscription";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return false;
    }

    if (type_found->second == MessageType::INTERNAL) {
        return pub_friends.find(subscriber) != pub_friends.end();
    }
    return true;
}

bool SubscriptionMonitor::register_subscription(
        const std::string &message_name,
        const std::string &subscriber)
{
    if (message_name.empty()) {
        std::string msg = "Message name empty in SubscriptionMonitor::register_subscription";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return false;
    }
    if (subscriber.empty()) {
        std::string msg = "Subscriber name empty in SubscriptionMonitor::register_subscription";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return false;
    }
    if (message_types.find(message_name) == message_types.end()) {
        std::string msg =
                "Message name " +
                message_name +
                " not found in SubscriptionMonitor::register_subscription";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return false;
    }
    if (!check_subscription(message_name, subscriber)) {
        std::string msg =
                "Subscription " +
                subscriber +
                " to " +
                message_name +
                " not allowed in SubscriptionMonitor::register_subscription";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return false;
    }

    add_subscriber(subscriber);
    auto pub = split_publisher(message_name);

    // Update the dependency map
    if (dependency_map.find(std::make_pair(pub, subscriber)) == dependency_map.end()) {
        dependency_map.insert(
                std::make_pair(
                        std::make_pair(
                                pub,
                                subscriber
                        ), MessageDependency::NO_DEPENDENCY
                )
        );
    }
    auto dep_type = MessageDependency::NO_DEPENDENCY;
    switch (message_types[message_name]) {
        case MessageType::AUXILIARY: // fall through
        case MessageType::INITIALIZATION:
            dep_type = MessageDependency::STALE_DEPENDENCY;
            break;
        case MessageType::INTERNAL:
            dep_type = MessageDependency::STATE_DEPENDENCY;
            break;
        case MessageType::OUTPUT:
            dep_type = MessageDependency::OUTPUT_DEPENDENCY;
            break;
    }
    auto dep = dependency_map.find(std::make_pair(pub, subscriber));
    dep->second = dep->second > dep_type ? dep->second : dep_type;

    subscriptions[pub].insert(subscriber);
    return true;
}

MessageDependency SubscriptionMonitor::get_dependency_type(
        const std::string& publisher,
        const std::string& subscriber
        )
{
    if (publisher.empty()) {
        std::string msg = "Publisher name empty in SubscriptionMonitor::get_dependency_type";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return MessageDependency::NO_DEPENDENCY;
    }
    if (subscriber.empty()) {
        std::string msg = "Subscriber name empty in SubscriptionMonitor::get_dependency_type";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return MessageDependency::NO_DEPENDENCY;
    }
    if (publishers.find(publisher) == publishers.end()) {
        std::string msg = "Publisher name not found in SubscriptionMonitor::get_dependency_type";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return MessageDependency::NO_DEPENDENCY;
    }
    if (subscribers.find(subscriber) == subscribers.end()) {
        std::string msg = "Subscriber name not found in SubscriptionMonitor::get_dependency_type";
        logger.bskLog(logLevel_t::BSK_ERROR, msg.c_str());
        return MessageDependency::NO_DEPENDENCY;
    }

    auto pair_found = dependency_map.find(std::make_pair(publisher, subscriber));
    if (pair_found == dependency_map.end()) {
        return MessageDependency::NO_DEPENDENCY;
    }
    return pair_found->second;
}

std::set<std::string> SubscriptionMonitor::find_unsubscribed_publishers() const
{
    std::set<std::string> out;
    for (const auto &pub_sub : subscriptions) {
        if (pub_sub.second.empty()) {
            out.insert(pub_sub.first);
        }
    }
    return out;
}

}