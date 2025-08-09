/*
 ISC License

 Copyright (c) 2016, Autonomous Vehicle Systems Lab, University of Colorado at Boulder

 Permission to use, copy, modify, and/or distribute this software for any
 purpose with or without fee is hereby granted, provided that the above
 copyright notice and this permission notice appear in all copies.

 THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

 */

#pragma once

#include <vector>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <map>
#include <set>
#include <mutex>
#include <type_traits>

#include "framework/blank_storage.h"
#include "utilities/bsk_logging.h"
#include "utilities/bsk_print.h"


namespace basilisk::messaging {

/*!
 * @brief Given a vector of name parts, concatenates them into a message name
 *
 * @param[in] parts  Vector of name parts
 *
 * @return std::string The concatenated name
 */
std::string concatenate_message_parts(std::vector<std::string> parts);

/*!
 * @brief Given a message name, splits it into a vector of name parts
 *
 * @param[in] name  The message name
 *
 * @return std::vector<std::string> The vector of name parts
 */
std::vector<std::string> split_message_name(std::string name);

/*!
 * @brief Given a fully-qualified message name, gets the publisher
 *
 * @param[in] name  The message name
 *
 * @return std::string The publisher name
 */
std::string split_publisher(const std::string& name);

/*!
* @brief Given a fully-qualified message name, gets the message name
*
* @param[in] name  The message name
*
* @return std::string The message name
*/
std::string split_message(const std::string& name);


/*!
 * @brief This enumeration is used to indicate the type of message dependency
 *
 * Message routing requirements indicate the dependency relationship between
 * a publisher and a subscriber, and can be used to form a directed graph for
 * sim execution. Identified dependency types are:
 *
 *  - Output dependency: The subscriber requires a valid output upon request,
 *                       e.g., a sensor reading at time T requires a valid
 *                       state output at time T.
 *
 *  - State dependency: The subscriber requires a message that might not be
 *                      have been published at the time of the request, e.g.,
 *                      a coupled-dynamics context in which subsystems
 *                      require access to each others' internal states.
 *
 *  - Stale dependency: The subscriber requires the most recent message, but
 *                      the message might be state. Quite stale. An example
 *                      here might be embedded software requiring the output
 *                      from a sensor via a buffer that might not have been
 *                      updated for one reason or another.
 *
 *  - No dependency: Take a wild guess.
 *
 */
enum class MessageDependency : int32_t {

    NO_DEPENDENCY, //!< No message is necessary

    STALE_DEPENDENCY, //!< The most recent message is sufficient

    OUTPUT_DEPENDENCY, //!< The message must be available when requested

    STATE_DEPENDENCY //!< The message might not be available when requested

};

/*!
 * @brief This enumeration characterizes the type of message
 */
 enum class MessageType : int32_t {

     AUXILIARY, //!< Auxiliary messages contain static data

     INITIALIZATION, //!< Initialization messages contain static initialization data

     INTERNAL, //!< Internal messages contain intermediate results and are only
               //!< guaranteed to be complete before output messages are created

     OUTPUT //!< Output messages contain final results
 };


/*!
 * @brief This structure contains summary information for a message
 */
struct MessageHeader {

    uint64_t update_counter{0}; //!< Number of times this message has been updated

    uint32_t current_read_buffer{0}; //!< Current buffer to read the message from

    uint32_t max_number_buffers{0}; //!< Max buffers this message will have

    uint64_t message_size{0}; //!< Message size in bytes

    uint64_t current_read_frame{0}; //!< Current frame of last read

    uint64_t buffer_offset{0}; //!< Starting offset in the storage buffer

    MessageType type{MessageType::INTERNAL}; //!< Message type

    uint32_t message_name_length{0}; //!< Length of message name

    std::string_view message_name; //!< Message name

    static uint64_t size_of(const std::string& message_name)
    {
        return sizeof(uint64_t) + 2*sizeof(uint32_t) + sizeof(uint64_t) +
               sizeof(uint64_t) + sizeof(uint64_t) + sizeof(MessageType) +
               sizeof(uint32_t) + message_name.size();
    }

    static uint64_t& get_update_counter(uint8_t* buffer)
    {
        return reinterpret_cast<uint64_t&>(buffer[0]);
    }

    static uint32_t& get_current_read_buffer(uint8_t* buffer)
    {
        return reinterpret_cast<uint32_t&>(buffer[sizeof(uint64_t)]);
    }

    static uint32_t& get_max_number_buffers(uint8_t* buffer)
    {
        return reinterpret_cast<uint32_t&>(
                buffer[sizeof(uint64_t) + sizeof(uint32_t)]
        );
    }

    static uint64_t& get_message_size(uint8_t* buffer)
    {
        return reinterpret_cast<uint64_t&>(
                buffer[sizeof(uint64_t) + 2*sizeof(uint32_t)]
        );
    }

    static uint64_t& get_current_read_frame(uint8_t* buffer)
    {
        return reinterpret_cast<uint64_t&>(
                buffer[2*sizeof(uint64_t) + 2*sizeof(uint32_t)]
        );
    }

    static uint64_t& get_buffer_offset(uint8_t* buffer)
    {
        return reinterpret_cast<uint64_t&>(
                buffer[3*sizeof(uint64_t) + 2*sizeof(uint32_t)]
        );
    }

    static MessageType& get_message_type(uint8_t* buffer)
    {
        return reinterpret_cast<MessageType&>(
                buffer[4*sizeof(uint64_t) + 2*sizeof(uint32_t)]
        );
    }

    static uint32_t& get_message_name_length(uint8_t* buffer)
    {
        return reinterpret_cast<uint32_t&>(
                buffer[4*sizeof(uint64_t) + 3*sizeof(uint32_t)]
        );
    }

    [[nodiscard]] std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> bytes;
        bytes.resize(MessageHeader::size_of(std::string(message_name)));

        uint32_t offset = 0;

        std::memcpy(bytes.data() + offset, &update_counter, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        std::memcpy(bytes.data() + offset, &current_read_buffer, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::memcpy(bytes.data() + offset, &max_number_buffers, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::memcpy(bytes.data() + offset, &message_size, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        std::memcpy(bytes.data() + offset, &current_read_frame, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        std::memcpy(bytes.data() + offset, &buffer_offset, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        std::memcpy(bytes.data() + offset, &type, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::memcpy(bytes.data() + offset, &message_name_length, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::memcpy(bytes.data() + offset, message_name.data(), message_name_length);

        return bytes;
    }

    static MessageHeader deserialize(uint8_t* header)
    {
        MessageHeader out;
        uint32_t offset = 0;

        std::memcpy(&out.update_counter, header + offset, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        std::memcpy(&out.current_read_buffer, header + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::memcpy(&out.max_number_buffers, header + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::memcpy(&out.message_size, header + offset, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        std::memcpy(&out.current_read_frame, header + offset, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        std::memcpy(&out.buffer_offset, header + offset, sizeof(uint64_t));
        offset += sizeof(uint64_t);

        std::memcpy(&out.type, header + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::memcpy(&out.message_name_length, header + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        out.message_name = std::string_view(
                reinterpret_cast<char*>(header + offset),
                out.message_name_length
        );
        return out;
    }

};


/*!
 * @brief This structure contains the write frame of the message
 */
struct SingleMessageHeader {
    uint64_t write_frame{0}; //!< Frame that the message was written
};


/*!
 * @brief This class monitors subscriptions and assigns dependency types to
 *        them.
 *
 * The subscription monitor is used to watch incoming subscriptions and check
 * them for validity. Most subscriptions will be valid, but subscriptions to
 * a publisher's private message must be checked against the publisher's
 * friend subscribers. The subscription monitor also assigns dependency types
 * between a publisher and a subscriber to keep a record for queries and
 * directed graph construction.
 *
 */
class SubscriptionMonitor {

    public:

    using publisher_list_t = std::set<std::string>;
    using subscriber_list_t = std::set<std::string>;
    using friend_map_t = std::map<std::string, std::set<std::string>>;
    using sub_map_t = std::map<std::string, std::set<std::string>>;
    using pub_sub_pair_t = std::pair<std::string, std::string>;
    using dependency_map_t = std::map<pub_sub_pair_t, MessageDependency>;
    using msg_type_map_t = std::map<std::string, MessageType>;

    publisher_list_t  publishers;     //!< List of publishers
    subscriber_list_t subscribers;    //!< List of subscribers
    friend_map_t      friends;        //!< Map of publishers to their friends
    dependency_map_t  dependency_map; //!< Map of publisher-subscriber pairs to
                                      //!< dependency types
    sub_map_t         subscriptions;  //!< Map of subscribers to their
                                      //!< subscriptions
    msg_type_map_t    message_types;  //!< Map of message names to their types
    BSKLogger         logger;         //!< Logger

    SubscriptionMonitor() = default;
    ~SubscriptionMonitor() = default;

    /*!
     * @brief Adds a publisher to the list of publishers
     *
     * @param[in] publisher_name Name of the publisher
     * @param[in] friends Optional vector of friend names
     */
    void add_publisher(const std::string& publisher_name,
                       const std::vector<std::string>& friends = {});

    /*!
     * @brief Adds a friend to an existing publisher
     *
     * @param[in] publisher_name Name of publisher
     * @param[in] friend_name    Name of friend
     *
     */
    void add_friend(const std::string& publisher_name,
                    const std::string& friend_name);

    /*!
     * @brief Adds a subscriber
     *
     * @param[in] subscriber_name Name of the subscriber
     *
     */
     void add_subscriber(const std::string &subscriber);

    /*!
     * @brief Registers a message type
     *
     * @param[in] message_name Name of the message
     * @param[in] type         Type of the message
     *
     */
     void add_message_type(const std::string& message_name,
                           MessageType type);

    /*!
     * @brief Checks for validity of a subscription
     *
     * @param[in] message_name Name of the message being subscribed to
     * @param[in] subscriber   Name of the subscriber
     *
     * @return True if the subscription is valid, false otherwisereturn
     */
    bool check_subscription(const std::string& message_name,
                            const std::string& subscriber);

    /*!
     * @brief Registers a subscription
     *
     * @param[in] message_name Name of the message being subscribed to
     * @param[in] subscriber   Name of the subscriber
     *
     * @return True if the subscription was registered, false otherwise
     */
     bool register_subscription(const std::string& message_name,
                                const std::string& subscriber);

    /*!
     * @brief Determines the dependency type between a publisher and a
     * subscriber given a publisher and subscriber name.
     *
     * @param[in] publisher    Name of the publisher
     * @param[in] subscriber   Name of the subscriber
     * @return The dependency type
     */
    [[nodiscard]] MessageDependency get_dependency_type(const std::string& publisher,
                                                        const std::string& subscriber);

    /*!
     * @brief Finds publishers without subscribers
     */
    [[nodiscard]] std::set<std::string> find_unsubscribed_publishers() const;

};


/*!
 * The container for a single message buffer
 */
struct MessageStorageContainer {

    std::string buffer_name; //!< Name of this process buffer for application access

    uint64_t n_messages{0};

    BlankStorage message_storage; //!< The storage buffer associated with this process

};

/*!
 * another header/key to a message
 */

struct MessageData {

    std::string buffer_name;  //!< String associated with the message buffer

    int64_t buffer_id;  //!< Buffer ID for where this message originally lives

    int64_t item_id;  //!< ID associated with request

    bool item_found;  //!< Indicates whether the buffer was found
};

/*!
 * @brief system messaging class
 */
class MessageBroker
{

    using offset_map_t = std::map<std::string, uint64_t>;
    using id_map_t     = std::map<std::string, uint64_t>;

    public:

    /*!
     * @brief Sets the name of the broker
     *
     * @param[in] name
     */
    void set_name(const std::string& name);

    /*!
     * @brief Sets the number of messages in the broker
     *
     * @param[in] message_count Number of messages
     *
     */
    void set_number_of_messages(int64_t);

    /*!
     * @brief Gets the number of messages in the broker
     *
     * @return int64_t Number of messages
     */
    [[nodiscard]] int64_t get_number_of_messages() const;

    /*!
     * @brief Clears the underlying memory in the broker
     */
    void clear_message_buffer();

    /*!
     * @brief Gets the size of underyling memory in the broker
     *
     * @return uint64_t Size in bytes
     */
    [[nodiscard]] uint64_t get_current_size() const;

    /*!
     * @brief Creates a new message managed by this broker
     *
     * @param[in] publisher_name Name of the publisher
     * @param[in] message_name   Name of the message
     * @param[in] message_size   Size in bytes of the message
     * @param[in] num_buffers    Number of buffers to allocate
     *
     * @returns int64_t          ID of the message
     */
    int64_t create_message(
            std::string,
            std::string,
            uint64_t,
            uint64_t num_buffers = 2
    );

    bool write_message(
            int64_t,
            uint64_t,
            uint64_t,
            void*,
            int64_t module_id = -1
    );

    bool read_message(
            int64_t,
            SingleMessageHeader*,
            uint64_t,
            void*,
            int64_t module_id = -1,
            uint64_t current_offset = 0
    );

    static void access_message_data(
            uint8_t*,
            uint64_t,
            uint64_t,
            SingleMessageHeader*,
            uint64_t,
            uint8_t*
    );

    MessageHeader* find_message_header(
            int64_t,
            int32_t buffer_select = -1
    );  //!< Returns a MessageHeader

    void print_all_message_data();  //!< Prints data for messages in current
                                    //!< buffer

    void print_message_stats(int64_t);  //!< Prints data for a
                                        //!< single message by ID

    std::string find_message_name(
            int64_t,
            int32_t buffer_select = -1
    );  //!< searches only the selected buffer

    int64_t find_message_id(
            std::string,
            int32_t buffer_select = -1
    );  //!< Searches only the selected buffer

    int64_t subscribe_to_message(
            std::string,
            uint64_t,
            int64_t
    );

    int64_t checkout_module_id();  //!< Assigns next integer module ID

    MessageData message_publish_search(std::string);  //!< Returns
                                                      //!< MessageData if found

    int64_t find_message_buffer(std::string);

    std::set<std::string> get_unsubscribed_messages();  //!< Returns msgs no
                                                        //!< one has access
                                                        //!< rights to

    std::set<std::string> get_unique_message_names();  //!< Searched across
                                                       //!< all buffers

    std::set<std::pair<long int, long int>>
    get_message_exchange_data(int64_t);

    void clear_messaging();  //!< Wipes out all messages and buffers. Total
                             //!< messaging system reset.

    bool obtain_write_rights(int64_t, int64_t);  //!< Grants rights to the
                                                 //!< requesting module

    bool obtain_read_rights(int64_t, int64_t);  //!< Grants rights to the
                                                //!< requesting module

    uint64_t get_failure_count()
    {
        return (this->create_fails + this->read_fails + this->write_fails);
    }

    SubscriptionMonitor subscription_monitor;

    private:

    MessageStorageContainer data_buffer;

    uint64_t                write_fails{0};   //!< the number of times we
                                              //!< tried to write invalidly

    uint64_t                read_fails{0};    //!< the number of times we
                                              //!< tried to read invalidly

    uint64_t                create_fails{0};  //!< the number of times we
                                              //!< tried to create invalidly

    int64_t                 next_module_id{0};

    offset_map_t            header_offsets;   //!< Map of message names to
                                              //!< the offset of their header

    offset_map_t            read_offsets;     //!< Map of message names to
                                              //!< the offset of their read
                                              //!< buffer

    offset_map_t            write_offsets;   //!< Map of message names to
                                              //!< the offset of their write
                                              //!< buffer

    uint64_t                header_bytes{0};

    uint64_t                message_bytes{0};

    uint64_t                message_start{0};

    id_map_t                id_map;


};

}