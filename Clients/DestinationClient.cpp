#include "DestinationClient.h"
#include "Client.h"

#include "../Networking/Protocols/CTMP.h"
#include <thread>
#include <sys/socket.h>
#include <cstring>



/**
 * Acquires the destination map lock and retrieves the next to be processed message.
 * Removes that message from the queue.
 *
 * @return - Expected containing the shared pointer to the next CTMP message if no error, the Error if there is one
 */
Expected<std::shared_ptr<CTMP>> DestinationClient::accessMessageItem() {
    std::lock_guard lock(destinationClientMutex);
    std::shared_ptr<CTMP> message = queue.front();
    queue.pop();
    if (!message) {
        return Expected<std::shared_ptr<CTMP>>("Message is null", LoggerLevel::ERROR, ErrorCode::DEFAULT);
    }
    return message;
}


/**
 * Once it gets the lock for this class,
 * pushes the message onto the queue.
 * @param message message to be added to the message queue.
 */
void DestinationClient::addMessageToQueue(std::shared_ptr<CTMP> message) {
    std::lock_guard lock(destinationClientMutex);
    queue.push(message);
}



/**
 * This method first calls accessMessageItem() to retrieve the next available message.
 * It then retrieves both the header and data of the message and combines it.
 * Next it attempts to send the entire message.
 * Returns an error if any occurs during the entire process.
 *
 * @return - Expected containing nothing if no error, the Error if there is one
 */
Expected<void> DestinationClient::sendMessage() {
    Logger::log("Sending message", LoggerLevel::DEBUG);
    //Get the next message to be processed
    auto expectedMessage = accessMessageItem();
    if (expectedMessage.hasError()) {
        return Expected<void>(expectedMessage.getError(), expectedMessage.getLoggerLevel(), expectedMessage.getErrorCode());
    }

    //Get the next message data
    int socketId = getSocketId();
    auto headerBytes = expectedMessage.getValue()->convertHeaderToBytes();
    auto dataBytes = expectedMessage.getValue()->getDataInBytes();

    //Combine the entire message(header + data)
    std::vector<uint8_t> entireMessage;
    entireMessage.reserve(headerBytes.size() + dataBytes.size());
    entireMessage.insert(entireMessage.begin(), headerBytes.begin(), headerBytes.end());
    entireMessage.insert(entireMessage.end(), dataBytes.begin(), dataBytes.end());

    //Attempt to send entire message throwing an error if it fails.
    size_t totalDataSent = 0;
    while (totalDataSent < entireMessage.size()) {
        ssize_t sent = send(socketId, entireMessage.data() + totalDataSent, entireMessage.size() - totalDataSent, MSG_NOSIGNAL);
        if (sent < 0) {
            if (errno == EPIPE){
                return Expected<void>("Connection closed, broken pipe", LoggerLevel::WARN, ErrorCode::BROKEN_PIPE);
            } else {
                return Expected<void>("Error sending data:" + std::string(strerror(errno)), LoggerLevel::ERROR, ErrorCode::DEFAULT);
            }
        }
        if (sent == 0) {
            return Expected<void>("Connection Closed during data send", LoggerLevel::WARN, ErrorCode::CONNECTION_CLOSED);
        }
        totalDataSent += sent;
    }
    Logger::log("Successfully sent data to server", LoggerLevel::DEBUG);

    return {};
}







