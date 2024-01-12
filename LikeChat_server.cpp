#include "LikeChat_server.h"

LikeChatServer::LikeChatServer() {
    // Initialize any server-specific setup here
}

void LikeChatServer::startServer() {
    connection.startServer();
    connection.handleCommunication();
}
