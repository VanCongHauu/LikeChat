#pragma once
#include "LikeChat_connection.h"

class LikeChatServer {
public:
    LikeChatServer();
    void startServer();

private:
    LikeChatConnection connection;
};
