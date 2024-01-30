// LikeChat_client.cpp
#include "LikeChat_client.h"
#include "LikeChat_client.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

LikeChatClient::LikeChatClient() {
    // Constructor logic
}

LikeChatClient::~LikeChatClient() {
    // Destructor logic
}

void LikeChatClient::connectToServer() {
    // Logic for connecting to the server
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Could not create socket\n";
        std::exit(-1);
    }

    // Configure IP and port of server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr));
    // serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(8888);

    std::string clientName;
    std::cout << "Enter your name: ";
    std::getline(std::cin, clientName);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Could not connect to server\n";
        close(clientSocket);
        std::exit(-1);
    }
    // Send the username to the server
    ssize_t nameBytesSent = send(clientSocket, clientName.c_str(), clientName.size(), 0);
    if (nameBytesSent < 0) {
        std::cerr << "Error sending client name to server\n";
        return;
    }
    std::cout << "Connected to server with name: " << clientName << "\n";

}

void LikeChatClient::handleCommunication() {
    // Logic for handling communication with the server
    char buffer[256];
    std::string message;

    while (true) {
        // Receive messages from the server
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "Server: " << buffer << std::endl;
        } else if (bytesRead == 0) {
            std::cout << "Connection closed by server.\n";
            break;
        }

        // Send messages to the server
        std::cout << "You: ";
        std::getline(std::cin, message);

        // Send the message to the server
        ssize_t bytesSent = send(clientSocket, message.c_str(), message.size(), 0);
        if (bytesSent < 0) {
            std::cerr << "Error sending data to server\n";
            break;
        }
    }

    // Close client socket
    close(clientSocket);
}
