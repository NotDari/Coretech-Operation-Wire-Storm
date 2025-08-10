    #include "DestinationClient.h"

    #include "../CTMP.h"
    #include <iostream>
    #include <thread>


    DestinationClient::DestinationClient(int socketId) {
        this->socketId = socketId;
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
     *
     * @return share
     */
    std::shared_ptr<CTMP> DestinationClient::accessMessageItem() {
        destinationClientMutex.lock();
        std::shared_ptr<CTMP> message = queue.front();
        queue.pop();
        return message;
    }



    //MAY NOT BE NECESSARY AS CONSIDERING USING HASHMAP
    //Overriding the equals so when we want to check if two clients are equal, we can.
    //This will allow us to remove it.
    bool DestinationClient::operator==(const DestinationClient& other) const {
        return (this->socketId == other.socketId);
    }





