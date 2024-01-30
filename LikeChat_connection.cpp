// LikeChat_connection.cpp
#include "LikeChat_connection.h"

LikeChatConnection::LikeChatConnection() : client(nullptr), server(nullptr) {
    // Constructor logic
}

LikeChatConnection::~LikeChatConnection() {
    // Destructor logic
    delete client;
    delete server;
}

LikeChatConnection::LikeChatConnection(LikeChatServer* server) : client(nullptr), server(server) {
    // Constructor logic
}

LikeChatConnection::LikeChatConnection(LikeChatClient* client) : client(client), server(nullptr) {
    // Constructor logic
}

void LikeChatConnection::run() {
    // Logic to run the application, including connecting to server and handling communication
    if (server) {
        std::thread serverThread([this]() { server->startServer(); });
        serverThread.detach();  // Detach the server thread
    }

    if (client) {
        client->connectToServer();
        client->handleCommunication();
    }
}
