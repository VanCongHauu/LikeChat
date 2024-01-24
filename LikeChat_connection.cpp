#include <iostream>
#include "LikeChat_client.h"
#include <cstring>
#include <unistd.h>
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


LikeChatConnection::LikeChatConnection() : serverSocket(-1) {}

LikeChatConnection::~LikeChatConnection() {
    for (std::thread& thread : clientThreads) {
        if (thread.joinable()) {
            thread.join();  // Make sure all child threads have finished before exiting
        }
    }
}

void LikeChatConnection::startServer() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create server socket. Error: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8888);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to bind server socket. Error: " << strerror(errno) << std::endl;
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, SOMAXCONN) == -1) {
    std::cerr << "Failed to listen for connections. Error: " << strerror(errno) << std::endl;
    close(serverSocket);
    exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening for connections..." << std::endl;

    while (true) {
        // Accept connections from new clients
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            std::cerr << "Failed to accept client connection. Error: " << strerror(errno) << std::endl;
            close(serverSocket);
            exit(EXIT_FAILURE);
        }
        std::cout << "Accepted a new client connection." << std::endl;

        // Perform authentication for the client
        if (authenticateClient(clientSocket)) {
            // Add clientSocket and username to the list
            clientSockets.push_back(clientSocket);
            std::cout << "New client connected." << std::endl;

            // Start a new thread to handle the client's connection
            clientThreads.emplace_back(&LikeChatConnection::handleCommunication, this, clientSocket);
        } else {
            // Close the connection if authentication fails
            std::cerr << "Authentication failed. Closing connection." << std::endl;
            close(clientSocket);
        }
    }
}

int LikeChatConnection::connectToServer(const std::string& username) {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create client socket. Error: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(8888);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to connect to server. Error: " << strerror(errno) << std::endl;
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected to the server." << std::endl;
    // Send the username to the server for authentication
    if (send(clientSocket, username.c_str(), username.length(), 0) == -1) {
        std::cerr << "Error sending username to the server." << std::endl;
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
    // Start a thread to handle communication with the server
    std::thread communicationThread(&LikeChatConnection::handleCommunication, this, clientSocket);
    communicationThread.detach(); // Detach the thread to run independently

    return clientSocket;
}

bool LikeChatConnection::authenticateClient(int clientSocket) {
    char buffer[1024];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            std::cerr << "Client disconnected gracefully." << std::endl;
        } 
        else {
            std::cerr << "Error receiving data from client. Error: " << strerror(errno) << std::endl;
        }
        close(clientSocket);
        return false;
    }

    buffer[bytesRead] = '\0';
    std::string username(buffer);

    // Verify authentication
    if (username != " ") {
        // Send a confirmation of successful login
        const char* successMessage = "Authentication successful. Welcome!";
        if (send(clientSocket, successMessage, strlen(successMessage), 0) == -1) 
        {
            std::cerr << "Error sending data to client. Error: " << strerror(errno) << std::endl;
            close(clientSocket);
            return false;
        }
        // Add the username to the list
        clientUsernames.push_back(username);

        return true;
    } else {
        // Send a login error message
        std::cerr << "Authentication failed for user: " << username << std::endl;
        const char* errorMessage = "Authentication failed. Invalid username.";
        if (send(clientSocket, errorMessage, strlen(errorMessage), 0) == -1)
        {
            std::cerr << "Error sending data to client. Error: " << strerror(errno) << std::endl;
            close(clientSocket);
            return false;
        }
    }
    return false;
}

void LikeChatConnection::handleCommunication(int clientSocket) {
    char buffer[1024];

    while (true) {
        // Receive data from the client or server
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                std::cerr << "Client disconnected gracefully." << std::endl;
            } else {
                std::cerr << "Error receiving data from client. Error: " << strerror(errno) << std::endl;
            }
            // Remove clientSocket and username from the list if the client has disconnected
            size_t index = &clientSocket - &clientSockets[0];
            clientSockets.erase(clientSockets.begin() + index);
            clientUsernames.erase(clientUsernames.begin() + index);
            close(clientSocket);
            break;  // End the thread when the client disconnects
        }

        // Display the received data
        buffer[bytesRead] = '\0';
        std::string receivedData(buffer);

        // Deserialize received data to ChatMessage
        ChatMessage receivedMessage = ChatMessage::deserialize(receivedData);
        
        // Check if deserialization was successful
        if (receivedMessage.getContent().empty()) {
            std::cerr << "Failed to deserialize ChatMessage. Invalid data format." << std::endl;
            break;  // Skip processing invalid data
        }
        std::cout << "[" << receivedMessage.serialize() << "]: " << receivedMessage.getSender() << ": " << receivedMessage.getContent() << std::endl;

        // Send data from the client or server
        std::cout << "Enter message: ";
        std::string inputMessage;
        std::getline(std::cin, inputMessage);

        if (inputMessage.empty()) {
            std::cerr << "Invalid input. Closing connection." << std::endl;
            close(clientSocket);
            break;  // End the thread on invalid input
        }
        ChatMessage chatMessage(username, inputMessage);
        sendMessage(clientSocket, chatMessage);

        if (send(clientSocket, chatMessage.serialize().c_str(), chatMessage.serialize().length(), 0) == -1) {
            std::cerr << "Error sending data to client. Error: " << strerror(errno) << std::endl;
            // Remove clientSocket and username from the list if the client has disconnected
            size_t index = &clientSocket - &clientSockets[0];
            clientSockets.erase(clientSockets.begin() + index);
            clientUsernames.erase(clientUsernames.begin() + index);
            close(clientSocket);
            break;  // End the thread when the client disconnects
        }
    }
    close(clientSocket);
}

void LikeChatConnection::sendMessage(int clientSocket, const ChatMessage& message) {
    std::string serializedMessage = message.serialize();
    send(clientSocket, serializedMessage.c_str(), serializedMessage.length(), 0);
}
