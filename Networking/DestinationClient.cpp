    #include "DestinationClient.h"
    #include "Client.h"

    #include "../CTMP.h"
    #include <iostream>
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
    std::shared_ptr<CTMP> DestinationClient::accessMessageItem() {
        std::lock_guard lock(destinationClientMutex);
        std::shared_ptr<CTMP> message = queue.front();
        queue.pop();
        if (!message) {
            //ERROR Handling if message is empty;
        }
        return message;
    }

    void DestinationClient::sendMessage() {
        std::shared_ptr<CTMP> message = accessMessageItem();
        int socketId = getSocketId();
        auto headerBytes = message->convertHeaderToBytes();

        size_t totalHeaderSent = 0;
        while (totalHeaderSent < headerBytes.size()) {
            ssize_t sent = send(socketId, headerBytes.data() + totalHeaderSent, headerBytes.size() - totalHeaderSent, 0);
            if (sent <= 0) {
                // Handle error
                return;
            }
            totalHeaderSent += sent;
        }


        size_t totalDataSent = 0;
        auto dataBytes = message->getDataInBytes();
        while (totalDataSent < dataBytes.size()) {
            ssize_t sent = send(socketId, dataBytes.data() + totalDataSent, dataBytes.size() - totalDataSent, 0);
            if (sent <= 0) {
                //Handle error
                break;
            }
            totalDataSent += sent;
        }


    }







