#include "LikeChat_client.h"

LikeChatClient::LikeChatClient(const std::string& _username) : username(_username) {
    // Initialize any client-specific setup here
}

void LikeChatClient::connectToServer() {
    connection.connectToServer(username);
}

void LikeChatClient::handleCommunication() {
    connection.handleCommunication();
}
