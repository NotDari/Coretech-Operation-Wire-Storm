//
// Created by Dariush Tomlinson on 14/08/2025.
//

#include <cstdint>

/**
 * This struct is used to set the default values that can be overidden via command flags.
 * Used for altering the behaviour of the program slightly without having to edit the code.
 *
 * Members:
 * sourcePort - The port to listen for source clients on
 * destPort - The port to listen for destination clients on
 * maxClientQueue - The max number of destination Clients to have in the queue
 * threadCount - The number of threads in the threadpool sending messages
 */
struct DefaultConfig {
    uint16_t sourcePort = 33333;
    uint16_t destPort = 44444;
    int maxClientQueue = 10;
    int threadCount = 4;
};
