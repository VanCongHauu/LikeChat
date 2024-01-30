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
#include <algorithm>  // Add this include for std::find_if
#include <map>        // Add this include for std::map

class Server {
public:
    Server();
    ~Server();
    void start();
    // Modified handleClient function
    void handleClient(int clientSocket);
    // New function to set client name
    void setClientName(int clientSocket, const std::string& name);

private:
    //static void handleClient(int clientSocket);

    int serverSocket;
    std::vector<std::thread> clientThreads;
    std::mutex consoleMutex;
    std::map<int, std::string> clientNames; // Map to store client names

    int getNextPort(); // Function to get the next available port
    // New members for broadcasting
    std::vector<std::pair<int, int>> connectedClients; // pair: clientSocket, nextPort
    std::mutex clientMutex;

    // Helper function to broadcast messages
    void broadcastMessage(const std::string& message, int senderSocket);
};
