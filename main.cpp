
#include <thread>
#include "Networking/Protocols/CTMP.h"
#include "Handlers/DestinationClientHandler.h"
#include "Handlers/ThreadPool.h"
#include "Networking/Server.h"
#include "Clients/SourceClient.h"
#include "Utils/Logger.h"
#include <csignal>
#include "Clients/Receivers/SourceClientReceiver.h"
#include "Clients/Receivers/DestinationClientReceiver.h"


//Atomic variable determining whether threads stop



std::atomic<bool> stop{false};






/**
 * This function loops through all the arguments provided by the command line, and
 * checks if they are a correct flag(with a potential correct value aswell).
 * If so, it alters the default value of the config file to match the user-set value,
 * so that it can later alter the values used for this program.
 *
 * @param argc number of arguments
 * @param argv vector of arguments
 * @return (DefaultConfig) - A struct containing all the values that could be set by this
 */
DefaultConfig getCommandLineFlags(int argc, char* argv[]) {
    DefaultConfig config;
    //Loop through arguments
    for (int i = 1; i < argc; ++i) {
        std::string flag = argv[i];

        //Check if user is altering source port argument
        if (flag == "-sp" && i + 1 < argc) {
            try {
                config.sourcePort = static_cast<uint16_t>(std::stoi(argv[++i]));
                Logger::log("Set source port: " + std::to_string(config.sourcePort), LoggerLevel::INFO);
            } catch (...) {
                Logger::log("Invalid flag argument", LoggerLevel::WARN);
            }
        }
        // Check if user is altering destination port argument
        else if (flag == "-dp" && i + 1 < argc) {
            try {
                config.destPort = static_cast<uint16_t>(std::stoi(argv[++i]));
                Logger::log("Set destination port: " + std::to_string(config.destPort), LoggerLevel::INFO);
            } catch (...) {
                Logger::log("Invalid flag argument", LoggerLevel::WARN);
            }
        }
        //Check if user is altering the max number of clients that can be waiting in the connection queue
        //Only for destination Clients as only one soruce client at a time
        else if (flag == "-mc" && i + 1 < argc) {
            try {
                config.maxClientQueue = std::stoi(argv[++i]);
                Logger::log("Set max client queue: " + std::to_string(config.maxClientQueue), LoggerLevel::INFO);
            } catch (...) {
                Logger::log("Invalid flag argument", LoggerLevel::WARN);
            }
        }
        //Check if user is altering the number of threads in the thread pool
        else if (flag == "-tc" && i + 1 < argc) {
            try {
                config.threadCount = std::stoi(argv[++i]);
                Logger::log("Set thread count: " + std::to_string(config.threadCount), LoggerLevel::INFO);

            } catch (...) {
                Logger::log("Invalid flag argument", LoggerLevel::WARN);
            }
        }
        //Check if user is enabling debug mode
        else if (flag == "-db" ) {
            Logger::includeDebug = true;
            Logger::log("Debug mode enabled", LoggerLevel::INFO);
        }
        //User entered an invalid flag
        else {
            Logger::log("Invalid Flag:" + flag, LoggerLevel::WARN);
        }
    }

    return config;
}

/**
 * Called when the user sends a signal to the program. Sets shutdown stop to true.
 * @param signal (int) - not used, just required for the signal function
 */
void handleShutdown(int signal) {
    stop = true;
}

/**
 * The main loop which creates both threads, initiates the thread pool and
 * @return (int) - whether or not the program run was successful
 */
int main(int argc, char* argv[]) {
    Logger::log("Starting application", LoggerLevel::INFO);
    DefaultConfig config = getCommandLineFlags(argc, argv);

    //Handle stopping process.
    signal(SIGINT, handleShutdown);
    //Handle stopping process. CLION uses SIGTERM
    signal(SIGTERM, handleShutdown);

    auto destinationClientHandler = std::make_shared<DestinationClientHandler>();


    SourceClientReceiver sourceClientReceiver(destinationClientHandler, stop, config);
    DestinationClientReceiver destinationClientReceiver(destinationClientHandler, stop, config);


    //Create Client Threads
    std::thread receiveDestThread(&DestinationClientReceiver::receiveClients, &destinationClientReceiver);
    std::thread receiveSourceThread(&SourceClientReceiver::receiveClients, &sourceClientReceiver);


    ThreadPool threadPool(config.threadCount, destinationClientHandler, stop);

    //Join the two threads
    receiveDestThread.join();
    receiveSourceThread.join();


    return 0;
}
