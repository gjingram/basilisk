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

#include "framework/system_messaging.h"

#include <vector>
#include <cstring>
#include <string>
#include <iostream>
#include <cstdint>
#include <set>

namespace basilisk::messaging {

std::string concatenate_message_parts(std::vector<std::string> parts)
{
    std::string result = parts.front();
    if (parts.size() == 1) {
        return result;
    }
    for (auto it = parts.begin() + 1; it != parts.end(); ++it) {
        result += "::" + *it;
    }
    return result;
}

std::vector<std::string> split_message_name(std::string name)
{
    char delim[3] = "::";
    char *c_name = const_cast<char*>(name.c_str());
    char *part;

    std::vector<std::string> result;

    part = std::strtok(c_name, delim);
    result.emplace_back(part);

    while (part != nullptr) {
        part = strtok(nullptr, delim);
        if (part != nullptr) {
            result.emplace_back(part);
        }
    }
    return result;
}

std::string split_publisher(const std::string& name)
{
    BSKLogger logger;
    if (name.empty()) {
        logger.bskLog(logLevel_t::BSK_ERROR, "Message name cannot be empty in split_publisher");
        return "";
    }
    auto parts = split_message_name(name);
    if (parts.size() == 1) {
        return parts.front();
    }
    std::vector<std::string> pub_parts{};
    for (auto it = parts.begin(); it != parts.end() - 1; ++it) {
        pub_parts.emplace_back(*it);
    }
    return concatenate_message_parts(pub_parts);
}

std::string split_message( const std::string &name)
{
    BSKLogger logger;
    if (name.empty()) {
        logger.bskLog(logLevel_t::BSK_ERROR, "Message name cannot be empty in split_message");
        return "";
    }
    auto parts = split_message_name(name);
    return parts.back();
}

void MessageBroker::set_name(const std::string& name)
{
    data_buffer.buffer_name = name;
}

void MessageBroker::set_number_of_messages(int64_t message_count)
{
    reinterpret_cast<int64_t&>(data_buffer.message_storage.storage_buffer[0]) =
            message_count;
}

int64_t MessageBroker::get_number_of_messages() const
{
    return reinterpret_cast<const int64_t&>(
            data_buffer.message_storage.storage_buffer[0]
    );
}

void MessageBroker::clear_message_buffer() {
    memset(
            &data_buffer.message_storage.storage_buffer[0],
           0x0,
           data_buffer.message_storage.get_size()
   );
}

void MessageBroker::clear_messaging() {
    clear_message_buffer();
    create_fails = 0;
    write_fails = 0;
    read_fails = 0;
}

uint64_t MessageBroker::get_current_size() const {
    return data_buffer.message_storage.get_size();
}

int64_t MessageBroker::create_message(
        std::string module_name,
        std::string message_name,
        uint64_t message_size,
        uint64_t num_buffers
) {
    // Check if the message exists
    message_name = module_name + "::" + message_name;
    if (find_message_id(message_name) >= 0) {
        BSK_PRINT(
                MSG_ERROR,
                "The message %s was created more than once.",
                message_name.c_str()
        );
        create_fails++;
        return false;
    }

    if (message_name == "") {
        BSK_PRINT(
                MSG_ERROR,
                "The message name cannot be empty."
        );
        create_fails++;
        return false;
    }

    if (message_size == 0) {
        BSK_PRINT(
                MSG_ERROR,
                "The message size cannot be zero."
        );
        create_fails++;
        return false;
    }

    if (num_buffers == 0) {
        BSK_PRINT(
                MSG_ERROR,
                "The number of buffers cannot be zero."
        );
        create_fails++;
        return false;
    }

    if (num_buffers == 1) {
        BSK_PRINT(
                MSG_WARNING,
                "The number of buffers cannot be one."
        );
    }

    auto init_size = data_buffer.message_storage.get_size();
    auto header_size = MessageHeader::size_of(message_name);
    header_bytes += header_size;
    auto storage_required =
            init_size +
            header_size +
            num_buffers * (
                    sizeof(SingleMessageHeader) +
                    message_size
            );
    data_buffer.message_storage.increase_storage(storage_required);

    if (get_number_of_messages() > 0) {
        auto old_message_start = message_start;
        message_start += header_size;
        memmove(
                &data_buffer.message_storage.storage_buffer[message_start],
                &data_buffer.message_storage.storage_buffer[old_message_start],
                message_bytes
        );
        for (auto &offset_pair: header_offsets) {
            offset_pair.second += header_size;
            auto *header_at =
                    &data_buffer
                        .message_storage
                        .storage_buffer[offset_pair.second];
            MessageHeader::get_buffer_offset(header_at) = offset_pair.second;
        }
    }

    MessageHeader header_insert;
    header_insert.message_name = message_name;
    header_insert.message_name_length = message_name.size();
    header_insert.buffer_offset = header_bytes;

    auto buffer_serial = header_insert.serialize();
    memcpy(
            &data_buffer.message_storage.storage_buffer[message_start],
            buffer_serial.data(),
            MessageHeader::size_of(message_name)
    );
    subscription_monitor.add_publisher(module_name);
    return 0;
}

///*!
// * This method subscribes a module to a message (but what does that mean different than read rights?)
// * @param messageName name of the message to sub to
// * @param messageSize size in bytes of message
// * @param moduleID ID of the requesting module
// * @return int64_t messageID
// */
//int64_t MessageBroker::subscribeToMessage(std::string messageName,
//                                          uint64_t messageSize,
//                                          int64_t moduleID) {
//    int64_t messageID;
//    std::vector<AllowAccessData>::iterator it;
//    messageID = this->FindMessageID(messageName);
//    if (messageID < 0) {
//        messageID = this->CreateNewMessage(messageName, messageSize, 2);
//    }
//    if (moduleID >= 0 && messageID >= 0) {
//        it = this->messageStorage->subData.begin();
//        it += messageID;
//        it->accessList.insert(moduleID);
//        it->publishedHere = false;
//    }
//    return (messageID);
//}
//
///*!
// * This message gives the requesting module write rights if the module and message are valid
// * @param messageID the ID of the message to write to
// * @param moduleID The ID of the requesting module
// * @return bool rightsObtained True if access was granted, else false
// */
//bool MessageBroker::obtainWriteRights(int64_t messageID, int64_t moduleID) {
//    bool rightsObtained = false;
//
//    if (moduleID >= 0 && messageID < this->GetMessageCount()) {
//        std::vector<AllowAccessData>::iterator it;
//        it = this->messageStorage->pubData.begin();
//        it += messageID;
//        it->accessList.insert(moduleID);
//        rightsObtained = true;
//    }
//
//    return (rightsObtained);
//}
//
///*!
// * this method gives the requesting module permission to read the requested message
// * @param messageID The message to get read rights to
// * @param moduleID The requesting module
// * @return bool rightsObtained True if rights granted, else false
// */
//bool MessageBroker::obtainReadRights(int64_t messageID, int64_t moduleID) {
//
//    bool rightsObtained = false;
//
//    if (moduleID >= 0 && messageID < this->GetMessageCount()) {
//        std::vector<AllowAccessData>::iterator it;
//        it = this->messageStorage->subData.begin();
//        it += messageID;
//        it->accessList.insert(moduleID);
//        rightsObtained = true;
//    }
//    return (rightsObtained);
//}
//
///*!
// *  This method checks ALL message buffers for a message with the given name
// * @param messageName
// * @return MessageIdentData dataFound A chunk of info about the message including whether it was found or not
// */
//MessageIdentData MessageBroker::messagePublishSearch(std::string messageName) {
//    int64_t messageID;
//
//    MessageIdentData dataFound;
//    dataFound.itemFound = false;
//    dataFound.itemID = -1;
//    dataFound.processBuffer = ~0;
//    std::vector<MessageStorageContainer *>::iterator it;
//    for (it = this->dataBuffers.begin(); it != this->dataBuffers.end(); it++) {
//        messageID = this->FindMessageID(messageName,
//                                        it - this->dataBuffers.begin());
//        if (messageID < 0) {
//            continue;
//        }
//        dataFound.itemFound = true;
//        dataFound.itemID = messageID;
//        dataFound.processBuffer = it - this->dataBuffers.begin();
//        dataFound.bufferName = (*it)->bufferName;
//        std::vector<AllowAccessData>::iterator pubIt;
//        pubIt = (*it)->pubData.begin() + messageID;
//        if (pubIt->accessList.size() > 0 && pubIt->publishedHere) {
//            return (dataFound);
//        }
//    }
//    return (dataFound);
//}
//
///*!
// * This method writes data to an already-created message if the requester has the right to
// * @param MessageID The message to write to
// * @param ClockTimeNanos The time to say the message was written in ns since sim start
// * @param MsgSize Size of the message
// * @param MsgPayload The data in the message
// * @param moduleID The requester ID
// * @return bool -- whether or not the message was written
// */
//bool MessageBroker::WriteMessage(int64_t MessageID, uint64_t ClockTimeNanos,
//                                 uint64_t MsgSize, void *MsgPayload,
//                                 int64_t moduleID) {
//    // Check if the message is valid
//    if (MessageID >= this->GetMessageCount()) {
//        BSK_PRINT(MSG_ERROR,
//                  "Received a write request for invalid message ID: %"
//        PRId64
//        " from ModuleID: %"
//        PRId64, MessageID, moduleID);
//        this->WriteFails++;
//        return (false);
//    }
//    // Check and update the previous publisher. Deny write if requester doesn't have pub access
//    MessageHeader *MsgHdr = this->FindMsgHeader(MessageID);
//    if (MsgHdr->previousPublisher != moduleID) {
//        std::vector<AllowAccessData>::iterator it;
//        it = this->messageStorage->pubData.begin();
//        it += MessageID;
//        if ((it->accessList.find(moduleID) != it->accessList.end()) ||
//            (moduleID == -2)) {
//            MsgHdr->previousPublisher = moduleID;
//        } else {
//            BSK_PRINT(MSG_ERROR, "Received a write request from module %"
//            PRId64
//            "that doesn't publish for %s . You get nothing.",
//                    moduleID, this->FindMessageName(MessageID).c_str());
//            this->WriteFails++;
//            return (false);
//        }
//    }
//    // Check the message size
//    if (MsgSize != MsgHdr->MaxMessageSize) {
//        BSK_PRINT(MSG_ERROR, "Received a write request from module %"
//        PRId64
//        "that was incorrect size for: %s. ( %"
//        PRIu64
//        "instead of %"
//        PRIu64
//        ".) You get nothing.",
//                moduleID, MsgHdr->MessageName, MsgSize, MsgHdr->MaxMessageSize);
//        this->WriteFails++;
//        return (false);
//    }
//    // If you made it this far, write the message and return success
//    uint8_t *WriteDataBuffer =
//            &(this->messageStorage->messageStorage.StorageBuffer[MsgHdr->
//                    StartingOffset]);
//    uint64_t AccessIndex = (MsgHdr->UpdateCounter % MsgHdr->MaxNumberBuffers) *
//                           (sizeof(SingleMessageHeader) +
//                            MsgHdr->MaxMessageSize);
//    WriteDataBuffer += AccessIndex;
//    SingleMessageHeader WriteHeader;
//    WriteHeader.WriteClockNanos = ClockTimeNanos;
//    WriteHeader.WriteSize = MsgSize;
//    memcpy(WriteDataBuffer, &WriteHeader, sizeof(SingleMessageHeader));
//    WriteDataBuffer += sizeof(SingleMessageHeader);
//    memcpy(WriteDataBuffer, MsgPayload, MsgSize);
//    MsgHdr->CurrentReadSize = MsgSize;
//    MsgHdr->CurrentReadTime = ClockTimeNanos;
//    MsgHdr->CurrentReadBuffer =
//            MsgHdr->UpdateCounter % MsgHdr->MaxNumberBuffers;
//    MsgHdr->UpdateCounter++;
//    return (true);
//}

///*! This method is static and is added so that other classes (ex. messageLogger)
// that have the messaging buffer layout can easily access their own internal
// buffers without having to re-write the same code.  Kind of overkill, but
// there you go.
// @param MsgBuffer The base address of the message buffer we are reading
// @param maxMsgBytes The maximum number of bytes for a given message type
// @param CurrentOffset The message count that we want to ready out
// @param DataHeader The message header that we are writing out to
// @param maxReadBytes The maximum number of read bytes
// @param OutputBuffer The output message buffer we are writing out to
// @return void
// */
//void MessageBroker::AccessMessageData(uint8_t *MsgBuffer, uint64_t maxMsgBytes,
//                                      uint64_t CurrentOffset,
//                                      SingleMessageHeader *DataHeader,
//                                      uint64_t maxReadBytes,
//                                      uint8_t *OutputBuffer) {
//    MsgBuffer += CurrentOffset * (sizeof(SingleMessageHeader) +
//                                  maxMsgBytes);
//    memcpy(DataHeader, MsgBuffer, sizeof(SingleMessageHeader));
//    uint64_t ReadSize = maxReadBytes < DataHeader->WriteSize ? maxReadBytes :
//                        DataHeader->WriteSize;
//    MsgBuffer += sizeof(SingleMessageHeader);
//    memcpy(OutputBuffer, MsgBuffer, ReadSize);
//}
//
///*!
// * This method reads a message. A warning is thrown if the requester isn't supposed to be reading this message.
// * @param MessageID  ID of the message to read
// * @param DataHeader Message header pointer to put message header data into
// * @param MaxBytes The maximum number of bytes to read into MsgPayload
// * @param MsgPayload A pointer to memory to toss the message data into
// * @param moduleID The module requesting a read
// * @param CurrentOffset
// * @return bool -- Whether the message was read successfully or not
// */
//bool MessageBroker::ReadMessage(int64_t MessageID, SingleMessageHeader
//*DataHeader, uint64_t MaxBytes, void *MsgPayload, int64_t moduleID,
//                                uint64_t CurrentOffset) {
//    if (MessageID >= this->GetMessageCount()) {
//        BSK_PRINT(MSG_ERROR,
//                  "Received a read request for invalid message ID.  Value: %"
//        PRId64
//        " is larger than number of available messages.", MessageID);
//        this->ReadFails++;
//        return (false);
//    }
//
//    //  Zero out the message header and payload in case of a bad read
//    memset(DataHeader, 0x0, sizeof(SingleMessageHeader));
//    memset(MsgPayload, 0x0, MaxBytes);
//
//    MessageHeader *MsgHdr = this->FindMsgHeader(MessageID);
//    /// - If there is no data just alert caller that nothing came back
//    if (MsgHdr->UpdateCounter == 0) {
//        return (false);
//    }
//
//    int64_t CurrentIndex = MsgHdr->UpdateCounter % MsgHdr->MaxNumberBuffers;
//    CurrentIndex -= (1 + CurrentOffset);
//    while (CurrentIndex < 0) {
//        CurrentIndex += MsgHdr->MaxNumberBuffers;
//    }
//    std::vector<MessageExchangeData>::iterator exIt;
//    std::vector<AllowAccessData>::iterator accIt;
//    accIt = this->messageStorage->subData.begin();
//    exIt = this->messageStorage->exchangeData.begin();
//    accIt += MessageID;
//    exIt += MessageID;
//    if (accIt->accessList.find(moduleID) == accIt->accessList.end()
//        && moduleID != -1) {
//        BSK_PRINT(MSG_WARNING, "Message %s was read by module ID %"
//        PRId64
//        " who is not on access list.", MsgHdr->MessageName, moduleID);
//    }
//
//    exIt->exchangeList.insert(std::pair<long int, long int>
//                                      (MsgHdr->previousPublisher, moduleID));
//
//    uint8_t *ReadBuffer = &(this->messageStorage->messageStorage.
//            StorageBuffer[MsgHdr->StartingOffset]);
//    uint64_t MaxOutputBytes = MaxBytes < MsgHdr->MaxMessageSize ? MaxBytes :
//                              MsgHdr->MaxMessageSize;
//    this->AccessMessageData(ReadBuffer, MsgHdr->MaxMessageSize,
//                            (uint64_t) CurrentIndex,
//                            DataHeader, MaxOutputBytes,
//                            reinterpret_cast<uint8_t *>(MsgPayload));
//    return (true);
//}

///*!
// * This method prints all message data from the current buffer
// * @return void
// */
//void MessageBroker::PrintAllMessageData()
//{
//    int64_t TotalMessageCount = this->GetMessageCount();
//    BSK_PRINT(MSG_INFORMATION, "Number of Messages: %" PRId64, TotalMessageCount);
//    for(int64_t i=0; i<TotalMessageCount; i++)
//    {
//        this->PrintMessageStats(i);
//    }
//}
//
///*!
// * This method returns the MessageHeader for a MessageID in the bufferSelect buffer
// * @param MessageID The message to query for the header
// * @param bufferSelect The buffer to query for the message
// * @return MessageHeaderdata* MsgHdr The data requested
// */
//MessageHeader* MessageBroker::FindMsgHeader(int64_t MessageID, int32_t bufferSelect)
//{
//    MessageHeader* MsgHdr;
//    if(MessageID >= this->GetMessageCount(bufferSelect))
//    {
//        return NULL;
//    }
//    MessageStorageContainer *localStorage = this->messageStorage;
//    if(bufferSelect >= 0)
//    {
//        std::vector<MessageStorageContainer *>::iterator it;
//        it = this->dataBuffers.begin();
//        it += bufferSelect;
//        localStorage = *it;
//    }
//    MsgHdr = reinterpret_cast<MessageHeader*> (&(localStorage->messageStorage.
//                                                     StorageBuffer[sizeof(uint64_t)]));
//    MsgHdr += MessageID;
//    return(MsgHdr);
//}
//
///*!
// *  This message prints MessageHeader information for the requested MessageID
// * @param MessageID The message to query
// * @return void
// */
//void MessageBroker::PrintMessageStats(int64_t MessageID)
//{
//    MessageHeader* MsgHdr = this->FindMsgHeader(MessageID);
//    if(MsgHdr == NULL)
//    {
//        BSK_PRINT(MSG_ERROR, "Received a print request for ID: %" PRId64 " That ID is not valid.", MessageID);
//        return;
//    }
//    BSK_PRINT(MSG_INFORMATION, "INFORMATION:\n Name: %s\n Writes: %" PRIu64 " \n MsgSize: %" PRIu64 " \n NumberBuffers: %u\n MsgID: %" PRId64,
//              MsgHdr->MessageName, MsgHdr->UpdateCounter, MsgHdr->MaxMessageSize, MsgHdr->MaxNumberBuffers, MessageID);
//}
//
///*!
// * Finds the message name for the requested message in the selected buffer
// * @param MessageID The message to query for the name
// * @param bufferSelect The buffer to query for the message
// * @return std::string MessageName The name of the six fingered man
// */
//std::string MessageBroker::FindMessageName(int64_t MessageID, int32_t bufferSelect)
//{
//    if(MessageID >= this->GetMessageCount(bufferSelect))
//    {
//        BSK_PRINT(MSG_WARNING, "WARING: Asked to find a message for invalid ID: %" PRId64, MessageID);
//    }
//    MessageHeader* MsgHdr = this->FindMsgHeader(MessageID, bufferSelect);
//    return(MsgHdr->MessageName);
//
//}
//
///*!
// * This message takes a MessageName and gives a message ID
// * @param MessageName The name to query for the ID
// * @param bufferSelect The buffer to query for the name
// * @return int64_t -- the message ID
// */
//int64_t MessageBroker::FindMessageID(std::string MessageName, int32_t bufferSelect)
//{
//    MessageHeader* MsgHdr;
//    for(int64_t i=0; i<this->GetMessageCount(bufferSelect); i++)
//    {
//        MsgHdr = this->FindMsgHeader(i, bufferSelect);
//        if(MessageName == std::string(MsgHdr->MessageName))
//        {
//            return(i);
//        }
//    }
//    return(-1);
//}
//
///*!
// * This method assigns a module ID to a new module and increments the NextModuleID counter
// * @return uint64_t nextModuleID the newly minted module ID
// */
//int64_t MessageBroker::checkoutModuleID()
//{
//    return(this->nextModuleID++);
//}
//
///*!
// * This method finds a buffer given a name
// * @param bufferName
// * @return MessageStorageContainer* -- a pointer to the buffer. Or, return -1 if not found.
// */
//int64_t MessageBroker::findMessageBuffer(std::string bufferName)
//{
//    std::vector<MessageStorageContainer *>::iterator it;
//    for(it = this->dataBuffers.begin(); it!= this->dataBuffers.end(); it++)
//    {
//        MessageStorageContainer *localContainer = (*it);
//        if(localContainer->bufferName == bufferName)
//        {
//            return(it - this->dataBuffers.begin());
//        }
//    }
//    return(-1);
//}
//
///*!
// * This method returns a list of messages that no one has access to.
// * @return std::set<std::string> unpublishedList The list of message names that are unpublished.
// */
//std::set<std::string> MessageBroker::getUnpublishedMessages()
//{
//    std::set<std::string> unpublishedList;
//    std::vector<AllowAccessData>::iterator it;
//    for(it=this->messageStorage->pubData.begin(); it!=this->messageStorage->pubData.end();
//        it++)
//    {
//        if(it->accessList.size() <= 0)
//        {
//            std::string unknownPub = MessageBroker::GetInstance()->
//                    FindMessageName(it - this->messageStorage->pubData.begin());
//            unpublishedList.insert(unknownPub);
//        }
//    }
//    return(unpublishedList);
//}
//
///*!
// * This method searches across all message buffers to get unique message names in the sim
// * @return std::set<std::string> outputNames A list of unique message names in the whole sim
// */
//std::set<std::string> MessageBroker::getUniqueMessageNames()
//{
//    std::set<std::string> outputNames;
//    std::vector<MessageStorageContainer *>::iterator it;
//    for(it = this->dataBuffers.begin(); it != this->dataBuffers.end(); it++)
//    {
//        for(int64_t i=0; i<this->GetMessageCount(it - this->dataBuffers.begin()); i++)
//        {
//            outputNames.insert(this->FindMessageName(i, it - this->dataBuffers.begin()));
//
//        }
//    }
//    return(outputNames);
//}
//
///*!
// * This message gets the exchangeData for a given messageID
// * @param messageID
// * @return std::set<std::pair<long int, long int>> exchangeList
// */
//std::set<std::pair<long int, long int>>
//    MessageBroker::getMessageExchangeData(int64_t messageID)
//{
//    std::vector<MessageExchangeData>::iterator it;
//    it = this->messageStorage->exchangeData.begin();
//    it += messageID;
//    return(it->exchangeList);
//}
//
//namespace {
//MessageBroker sim_broker;
//}
//
//int64_t attach_storage_bucket(std::string buffer_name = "") {
//    return sim_broker.attach_storage_bucket(buffer_name);
//}

}