
#include <thread>
#include "CTMP.h"
#include "Handlers/DestinationClientHandler.h"
#include "Handlers/ThreadPool.h"
#include "Networking/Server.h"
#include "Networking/Clients/SourceClient.h"
#include "Utils/DefaultConfig.h"
#include "Utils/Logger.h"
#include <csignal>

std::atomic<bool> stop{false};

using namespace std;



/**
 * Thread for receiving the source client.
 * Will loop through the messages sent, passing them to the Destination Client Handler.
 */
void receiveSourceClients(std::shared_ptr<DestinationClientHandler> destinationClientHandler, std::atomic<bool>* stop, DefaultConfig config) {
    Logger::log("Source Client Thread Starting", LoggerLevel::INFO);
    Server server(config.sourcePort, 1);
    Expected<int> expectedServer = server.initiateProtocol();
    if (expectedServer.hasError()) {
        Logger::log(expectedServer.getError(), expectedServer.getLoggerLevel());
        *stop = true;
        return;
    }
    while (!(*stop)){
        Logger::log("Searching for source client", LoggerLevel::INFO);
        //Accepting a connection to the client socket and assigning it
        Expected<int> expectedClient = server.initiateClient();
        if (expectedClient.hasError()) {
            Logger::log(expectedClient.getError(), expectedClient.getLoggerLevel());
            *stop = true;
            continue;
        }

        SourceClient sourceClient(expectedClient.getValue(), 8);
        Logger::log("Connected to Source Client", LoggerLevel::INFO);

        //Looping for messages from source client
        while (!(*stop)) {
            Expected<CTMP> expectedCTMP = sourceClient.readMessage();
            Logger::log("Started reading message", LoggerLevel::INFO);
            if (expectedCTMP.hasError()) {
                LoggerLevel level = expectedCTMP.getLoggerLevel();
                Logger::log(expectedCTMP.getError(), expectedCTMP.getLoggerLevel());
                if (level == LoggerLevel::ERROR) {
                    *stop = true;
                    break;
                }
                if (expectedCTMP.getErrorCode() == ErrorCode::ConnectionClosed) {
                    break;
                }
                continue;


            }
            //MESSAGE is fine- Continue

            Logger::log("Sending message2", LoggerLevel::INFO);
            auto expectedSendMessageAttempt = destinationClientHandler->addMessage(std::make_shared<CTMP>(std::move(expectedCTMP.getValue())));
            if (expectedSendMessageAttempt.hasError()) {
                Logger::log(expectedSendMessageAttempt.getError(), expectedSendMessageAttempt.getLoggerLevel());
            }
            break;

        }
        Logger::log("Source Client Thread Closing", LoggerLevel::INFO);
        sourceClient.closeClient();

    }


    server.stop();



}


/**
 * Thread for receiving the destination clients.
 * At the moment only gets 1 connection, but in future will loop through connections
 * and then send it to the DestinationClientHandler.
 */
void receiveDestinationClients(std::shared_ptr<DestinationClientHandler> destinationClientHandler, std::atomic<bool>* stop, DefaultConfig config) {
    Logger::log("Destination Client Thread Starting", LoggerLevel::INFO);
    Server server(config.destPort, config.maxClientQueue);
    Expected<int> expectedServer = server.initiateProtocol();
    if (expectedServer.hasError()) {
        Logger::log(expectedServer.getError(), expectedServer.getLoggerLevel());
        *stop = true;
        return;
    }

    Logger::log("Destination client server created successfully", LoggerLevel::INFO);
    while (!(*stop)) {
        Expected<int> destinationClient = server.initiateClient();
        if (destinationClient.hasError()) {
            Logger::log(destinationClient.getError(), destinationClient.getLoggerLevel());
            *stop = true;
            break;
        }

        Logger::log("Attempting to add new destination", LoggerLevel::INFO);
        auto expectedAddDestination = destinationClientHandler->addNewDestination(destinationClient.getValue());
        if (expectedAddDestination.hasError()) {
            Logger::log(expectedAddDestination.getError(), expectedAddDestination.getLoggerLevel());
        } else {
            Logger::log("Successfully added new destination", LoggerLevel::INFO);
        }
    }
    Logger::log("Destination client thread closing", LoggerLevel::INFO);
    server.stop();

}


/**
 * 
 * @param argc
 * @param argv 
 * @return 
 */
DefaultConfig getCommandLineFlags(int argc, char* argv[]) {
    DefaultConfig config;
    for (int i = 1; i < argc; ++i) {
        std::string flag = argv[i];

        if (flag == "-sp" && i + 1 < argc) {
            try {
                config.sourcePort = static_cast<uint16_t>(std::stoi(argv[++i]));
                Logger::log("Set source port: " + std::to_string(config.sourcePort), LoggerLevel::INFO);
            } catch (...) {
                Logger::log("Invalid flag argument", LoggerLevel::WARN);
            }
        } else if (flag == "-dp" && i + 1 < argc) {
            try {
                config.destPort = static_cast<uint16_t>(std::stoi(argv[++i]));
                Logger::log("Set destination port: " + std::to_string(config.destPort), LoggerLevel::INFO);
            } catch (...) {
                Logger::log("Invalid flag argument", LoggerLevel::WARN);
            }
        } else if (flag == "-mc" && i + 1 < argc) {
            try {
                config.maxClientQueue = std::stoi(argv[++i]);
                Logger::log("Set max client queue: " + std::to_string(config.maxClientQueue), LoggerLevel::INFO);
            } catch (...) {
                Logger::log("Invalid flag argument", LoggerLevel::WARN);
            }
        } else if (flag == "-tc" && i + 1 < argc) {
            try {
                config.threadCount = std::stoi(argv[++i]);
                Logger::log("Set thread count: " + std::to_string(config.threadCount), LoggerLevel::INFO);

            } catch (...) {
                Logger::log("Invalid flag argument", LoggerLevel::WARN);
            }
        } else if (flag == "-db" ) {
            Logger::includeDebug = true;
            Logger::log("Debug mode enabled", LoggerLevel::INFO);
        }
        else {
            Logger::log("Invalid Flag:" + flag, LoggerLevel::WARN);
        }
    }

    return config;
}

void handleShutdown(int signal) {
    Logger::log("Requested Shutdown", LoggerLevel::DEBUG);
    stop = true;
}

/**
 * The main loop which creates both threads.
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

    //Create Client Threads
    thread receiveDestThread(receiveDestinationClients, destinationClientHandler, &stop, config);
    thread receiveSourceThread(receiveSourceClients, destinationClientHandler, &stop, config);


    ThreadPool threadPool(config.threadCount, destinationClientHandler);

    //Join the two threads
    receiveDestThread.join();
    receiveSourceThread.join();


    return 0;
}
