#include "LikeChat_server.h"
#include "LikeChat_client.h"

LikeChatServer::LikeChatServer() {
    // Initialize any server-specific setup here
}

void LikeChatServer::startServer() {
    connection.startServer();
}
