// LikeChat_connection.h
#pragma once
#include "LikeChat_client.h"
#include "LikeChat_server.h"

class LikeChatConnection {
public:
    LikeChatConnection();
    ~LikeChatConnection();
    LikeChatConnection(LikeChatServer* server);
    LikeChatConnection(LikeChatClient* client);

    void run();

private:
    LikeChatClient* client;
    LikeChatServer* server;
};
