#pragma once
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class LikeChatClient {
public:
    LikeChatClient();
    ~LikeChatClient();

    void connectToServer();
    void handleCommunication();

private:
    int clientSocket;
};