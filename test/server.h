#pragma once

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <thread>
#include <mutex>

class Server {
public:
    Server();
    ~Server();
    void start();

private:
    static void handleClient(int clientSocket);

    int serverSocket;
    std::vector<std::thread> clientThreads;
    std::mutex consoleMutex;
};
