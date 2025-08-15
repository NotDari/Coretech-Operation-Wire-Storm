//
// Created by Dariush Tomlinson on 14/08/2025.
//

#include <cstdint>

/**
 * This struct is used to set the default values that can be overidden, via command flags.
 * Used for altering the behaviour of the program slightly without having to edit the code.
 */
struct DefaultConfig {
    //The port to listen for source clients on
    uint16_t sourcePort = 33333;
    //The port to listen to destination clients on
    uint16_t destPort = 44444;
    //The max number of destination Clients to have in the queue
    int maxClientQueue = 10;
    //The number of threads in the threadpool sending messages
    int threadCount = 4;
};
