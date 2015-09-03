#ifndef _MessageLogger_HH_
#define _MessageLogger_HH_

#include <vector>
#include <string>
#include <stdint.h>
#include "architecture/messaging/system_messaging.h"

/*! \addtogroup SimArchGroup
 * @{
 */

typedef struct {
    std::string messageName;    //!< -- The message name associated with the log
    int32_t messageID;          //!< -- The message ID associated with the log
    uint64_t lastLogTime;       //!< ns The last valid log time observed
    uint64_t logInstanceCount;  //!< -- The number of message logs that we have made
    uint64_t lastWriteCheck;    //!< ns The last write count that we examined
    uint64_t writeDelta;        //!< ns The minimum time between log steps
    uint64_t bufferOffset;      //!< -- The current offset in the log to access
    BlankStorage messageBuffer; //!< The storage buffer associated with the log
    std::vector<uint64_t> storOff; //!< -- Vector of storage buffer offset offsets for access
}messageLogContainer;

//! The top-level container for an entire simulation
class messageLogger
{
public:
    messageLogger(); //!< The MessageLogger constructor
    ~messageLogger();//!< MessageLogger destructor
    void addMessageLog(std::string messageName, uint64_t messagePeriod=0);
    void linkMessages();
    bool messagesLinked() {return allLogsLinked;} //!< Getter for link success
    void logAllMessages();
    bool readLog(int64_t messageID, SingleMessageHeader *dataHeader,
                 uint64_t maxBytes, uint8_t *msgPayload, uint64_t currentOffset=0);
    
public:
    uint64_t initBufferSize; //!< Default buffer size fo message log storage
    std::vector<messageLogContainer> logData; //!< Vector of log elements
private:
    bool allLogsLinked; //!< Indicator of whether or not messages are all linked
};

/*! @} */
#endif /* _MessageLogger_H_ */