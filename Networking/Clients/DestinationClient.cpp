    #include "DestinationClient.h"
    #include "Client.h"

    #include "../../CTMP.h"
    #include "../../Utils/Expected.h"
    #include <thread>

#include <sys/socket.h>






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
     *
     * @return share
     */
    Expected<std::shared_ptr<CTMP>> DestinationClient::accessMessageItem() {
        std::lock_guard lock(destinationClientMutex);
        std::shared_ptr<CTMP> message = queue.front();
        queue.pop();
        if (!message) {
            return {"Message is null", LoggerLevel::ERROR, ErrorCode::Default};
        }
        return message;
    }

    Expected<void> DestinationClient::sendMessage() {
        Logger::log("Sending message", LoggerLevel::INFO);
        auto expectedMessage = accessMessageItem();
        if (expectedMessage.hasError()) {
            return {expectedMessage.getError(), expectedMessage.getLoggerLevel(), expectedMessage.getErrorCode()};
        }
        int socketId = getSocketId();
        auto headerBytes = expectedMessage.getValue()->convertHeaderToBytes();
        auto dataBytes = expectedMessage.getValue()->getDataInBytes();

        std::vector<uint8_t> entireMessage;
        entireMessage.reserve(headerBytes.size() + dataBytes.size());

        entireMessage.insert(entireMessage.begin(), headerBytes.begin(), headerBytes.end());
        entireMessage.insert(entireMessage.end(), dataBytes.begin(), dataBytes.end());


        size_t totalDataSent = 0;
        while (totalDataSent < entireMessage.size()) {
            ssize_t sent = send(socketId, entireMessage.data() + totalDataSent, entireMessage.size() - totalDataSent, MSG_NOSIGNAL);
            if (sent < 0) {
                return {"Error sending data:" + std::string(strerror(errno)), LoggerLevel::ERROR, ErrorCode::Default};
            }
            if (sent == 0) {
                return {"Connection Closed during data send", LoggerLevel::WARN, ErrorCode::ConnectionClosed};
            }
            totalDataSent += sent;
        }
        Logger::log("Successfully sent data to server", LoggerLevel::INFO);

        return {};
    }
/**
Logger::log("Sending message", LoggerLevel::INFO);
        auto expectedMessage = accessMessageItem();
        if (expectedMessage.hasError()) {
            return {expectedMessage.getError(), expectedMessage.getLoggerLevel(), expectedMessage.getErrorCode()};
        }
        int socketId = getSocketId();
        auto headerBytes = expectedMessage.getValue()->convertHeaderToBytes();
        Logger::log("headerSize" + std::to_string(headerBytes.size()), LoggerLevel::DEBUG);


        size_t totalHeaderSent = 0;
        while (totalHeaderSent < headerBytes.size()) {
            ssize_t sent = send(socketId, headerBytes.data() + totalHeaderSent, headerBytes.size() - totalHeaderSent, 0);
            if (sent < 0) {
                return {"Error sending header:" + std::string(strerror(errno)), LoggerLevel::ERROR, ErrorCode::Default};
            }
            if (sent == 0) {
                return {"Connection Closed during header", LoggerLevel::WARN, ErrorCode::ConnectionClosed};
            }
            totalHeaderSent += sent;
        }



        size_t totalDataSent = 0;
        auto dataBytes = expectedMessage.getValue()->getDataInBytes();
        while (totalDataSent < dataBytes.size()) {
            ssize_t sent = send(socketId, dataBytes.data() + totalDataSent, dataBytes.size() - totalDataSent, MSG_NOSIGNAL);
            if (sent < 0) {
                return {"Error sending data:" + std::string(strerror(errno)), LoggerLevel::ERROR, ErrorCode::Default};
            }
            if (sent == 0) {
                return {"Connection Closed during data", LoggerLevel::WARN, ErrorCode::ConnectionClosed};
            }
            totalDataSent += sent;
        }
        Logger::log("Done" + std::to_string(totalDataSent), LoggerLevel::DEBUG);
        return {};
*/






