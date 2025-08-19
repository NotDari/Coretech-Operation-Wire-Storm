//
// Created by Dariush Tomlinson on 19/08/2025.
//

#include "ClientReceiver.h"


/**
 * Sets a file descriptor on the chosen socket. Checks to see if there is a value to read from this socket.
 * If there isn't a response withing the set timeout or an error occurs, returns the error.
 * If there is a value, returns Expected<void> returning nothing.
 *
 * @param socketId (int) id of the socket to select/ wait on
 * @param timeoutSeconds (int) number of seconds to return in, if no value is found
 * @return - returns an expected containing nothing if successful, or the specific error if it fails or times out.
 */
Expected<void> ClientReceiver::selectSocketTimeout(int socketId, int timeoutSeconds) {
    //Create file descriptor for given socket
    fd_set socketFD;
    FD_ZERO(&socketFD);
    FD_SET(socketId, &socketFD);


    //Timeout time
    timeval waitTime;
    waitTime.tv_sec = timeoutSeconds;
    waitTime.tv_usec = 0;

    int result = select(socketId + 1, &socketFD, nullptr, nullptr, &waitTime);
    //Error with select, return an error
    if (result < 0) {
        return Expected<void>("Error with select" , LoggerLevel::ERROR, ErrorCode::DEFAULT);
    }
    //Nothing connected in time, not an actual error, so return a log and a NoConnectionAttempt
    if (result == 0){
        return Expected<void>("No connection, trying again", LoggerLevel::DEBUG, ErrorCode::NO_CONNECTION_ATTEMPT);
    }
    return {};
}