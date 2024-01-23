#ifndef LIKECHAT_CONNECTION_H
#define LIKECHAT_CONNECTION_H

#include <string>
#include <vector>
// #include <netinet/in.h>
#include <thread>
#include <mutex>
#include "ChatMessage.h"

class LikeChatConnection {
public:
    LikeChatConnection();
    ~LikeChatConnection();
    void startServer(const std::string& username);
    bool authenticateClient(int clientSocket);
    void handleCommunication(int clientSocket);
    void sendMessage(int clientSocket, const ChatMessage& message);

private:
    int clientSocket;
    std::vector<std::thread> clientThreads;
    std::vector<int> clientSockets;
    std::vector<std::string> clientUsernames;
    std::mutex clientMutex;
    std::string username;
};
#endif