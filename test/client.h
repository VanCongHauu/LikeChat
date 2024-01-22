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
    Client();
    ~Client();
    void start();

private:
    static void receiveMessages(int clientSocket);

    int clientSocket;
    std::thread receiveThread;
};
