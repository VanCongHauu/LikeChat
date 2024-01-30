#pragma once

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <fcntl.h>

class Client {
public:
    Client(int port); // Pass port as an argument to the constructor
    ~Client();
    void start();
    Client(int port, const std::string& name);

private:
    static void receiveMessages(int clientSocket);

    int clientSocket;
    std::string clientName;
    std::thread receiveThread;
};
