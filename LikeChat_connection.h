#pragma once
#include <string>
#include <vector>
#include <WinSock2.h>
#include <thread>
#include <mutex>
#include "ChatMessage.h"

class LikeChatConnection {
public:
    LikeChatConnection();
    ~LikeChatConnection();
    void startServer(const std::string& username);
    bool authenticateClient(SOCKET clientSocket);
    void handleCommunication(SOCKET clientSocket);
    void sendMessage(SOCKET clientSocket, const ChatMessage& message);

private:
    SOCKET serverSocket;
    std::vector<std::thread> clientThreads;
    std::vector<SOCKET> clientSockets;
    std::vector<std::string> clientUsernames;
    std::mutex clientMutex;
    std::string username;
};
