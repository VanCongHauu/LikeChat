#include "LikeChat_client.h"

LikeChatClient::LikeChatClient(const std::string& _username) : username(_username), clientSocket(INVALID_SOCKET) {
    // Initialize any client-specific setup here
}

void LikeChatClient::connectToServer() {
    connection.startServer(username);
}

void LikeChatClient::handleCommunication() {
    connection.handleCommunication(clientSocket);
}
