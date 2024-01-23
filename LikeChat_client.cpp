#include "LikeChat_client.h"
#include <iostream>

LikeChatClient::LikeChatClient(const std::string& _username) : username(_username), clientSocket(-1) {
    // Initialize any client-specific setup here
}

void LikeChatClient::connectToServer() {
    connection.startServer(username);
}

void LikeChatClient::handleCommunication() {
    if (clientSocket == -1) {
        // Handle the case when the client socket is invalid
        std::cerr << "Invalid client socket." << std::endl;
        return;
    }
    connection.handleCommunication(clientSocket);
}
