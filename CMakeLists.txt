cmake_minimum_required(VERSION 3.20)
project(operation_wire_storm)

set(CMAKE_CXX_STANDARD 20)

include_directories(
        ${CMAKE_SOURCE_DIR}/Clients
        ${CMAKE_SOURCE_DIR}/Clients/Receivers
        ${CMAKE_SOURCE_DIR}/Networking
        ${CMAKE_SOURCE_DIR}/Networking/Protocols
        ${CMAKE_SOURCE_DIR}/Handlers
        ${CMAKE_SOURCE_DIR}/Utils
)


add_executable(operation_wire_storm main.cpp
        Networking/Protocols/CTMP.h
        Networking/Protocols/CMTP.cpp
        Clients/DestinationClient.cpp
        Clients/DestinationClient.h
        Handlers/DestinationClientHandler.cpp
        Handlers/DestinationClientHandler.h
        Handlers/ThreadPool.cpp
        Handlers/ThreadPool.h
        Clients/Client.h
        Utils/Logger.h
        Networking/Server.cpp
        Networking/Server.h
        Utils/Expected.h
        Clients/SourceClient.cpp
        Clients/SourceClient.h
        Utils/Logger.cpp
        Utils/ErrorCode.h
        Utils/DefaultConfig.h
        Clients/Receivers/ClientReceiver.cpp
        Clients/Receivers/ClientReceiver.h
        Clients/Receivers/DestinationClientReceiver.cpp
        Clients/Receivers/DestinationClientReceiver.h
        Clients/Receivers/SourceClientReceiver.cpp
        Clients/Receivers/SourceClientReceiver.h
)
