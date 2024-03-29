#pragma once
#include <string>
#include "LikeChat_connection.h"

class LikeChatClient {
public:
    LikeChatClient(const std::string& username);
    void connectToServer();
    void handleCommunication();

private:
    std::string username;
    LikeChatConnection connection;
    SOCKET clientSocket;
};
