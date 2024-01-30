// LikeChat_server.h
#pragma once
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <memory>

class LikeChatServer {
public:
    LikeChatServer();
    ~LikeChatServer();
    LikeChatServer(const LikeChatServer&) = delete;
    LikeChatServer& operator=(const LikeChatServer&) = delete;

    void startServer();
    void saveUsername(int clientSocket, const std::string& username);

private:
    int serverSocket;
    std::map<int, std::string> clientNames;
    std::mutex clientMutex; // Use std::mutex instead of std::unique_ptr<std::mutex>
    std::vector<std::thread> clientThreads;

    void handleClient(int clientSocket);
    void broadcastMessage(const std::string& message, int senderSocket);
};
