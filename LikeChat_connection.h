#pragma once
#include <string>
#include <vector>
#include <WinSock2.h>
#include <thread>

class LikeChatConnection {
public:
    LikeChatConnection();
    ~LikeChatConnection();
    void startServer();
    void connectToServer(const std::string& username);
    bool authenticateClient(SOCKET clientSocket);
    void handleCommunication(SOCKET clientSocket);

private:
    SOCKET serverSocket;
    std::vector<std::thread> clientThreads;
    std::vector<SOCKET> clientSockets;
    std::vector<std::string> clientUsernames;
};
