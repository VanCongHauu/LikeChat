#include "LikeChat_connection.h"
#include <iostream>
#include "LikeChat_client.h"

LikeChatConnection::LikeChatConnection() {
    WSADATA wsData;
    if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0) {
        std::cerr << "Failed to initialize Winsock. Error: " << WSAGetLastError() << std::endl;
        exit(EXIT_FAILURE);
    }
}

LikeChatConnection::~LikeChatConnection() {
    for (std::thread& thread : clientThreads) {
        if (thread.joinable()) {
            thread.join();  // Make sure all child threads have finished before exiting
        }
    }
    WSACleanup();
}

void LikeChatConnection::startServer(const std::string& username) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create server socket. Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind server socket. Error: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
    std::cerr << "Failed to listen for connections. Error: " << WSAGetLastError() << std::endl;
    closesocket(serverSocket);
    WSACleanup();
    exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening for connections..." << std::endl;

    while (true) {
        // Accept connections from new clients
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept client connection. Error: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

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
            closesocket(clientSocket);
        }
    }
}

bool LikeChatConnection::authenticateClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            std::cerr << "Client disconnected gracefully." << std::endl;
        } 
        else {
            std::cerr << "Error receiving data from client. Error: " << WSAGetLastError() << std::endl;
        }
        closesocket(clientSocket);
        return false;
    }

    buffer[bytesRead] = '\0';
    std::string username(buffer);

    // Verify authentication
    if (username == "valid_username") {
        // Send a confirmation of successful login
        const char* successMessage = "Authentication successful. Welcome!";
        if (send(clientSocket, successMessage, strlen(successMessage), 0) == SOCKET_ERROR) 
        {
            std::cerr << "Error sending data to client. Error: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            return false;
        }
        // Add the username to the list
        clientUsernames.push_back(username);

        return true;
    } else {
        // Send a login error message
        std::cerr << "Authentication failed for user: " << username << std::endl;
        const char* errorMessage = "Authentication failed. Invalid username.";
        if (send(clientSocket, errorMessage, strlen(errorMessage), 0) == SOCKET_ERROR)
        {
            std::cerr << "Error sending data to client. Error: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            return false;
        }
    }
    return false;
}

void LikeChatConnection::handleCommunication(SOCKET clientSocket) {
    char buffer[1024];

    while (true) {
        // Receive data from the client or server
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                std::cerr << "Client disconnected gracefully." << std::endl;
            } else {
                std::cerr << "Error receiving data from client. Error: " << WSAGetLastError() << std::endl;
            }
            // Remove clientSocket and username from the list if the client has disconnected
            size_t index = &clientSocket - &clientSockets[0];
            clientSockets.erase(clientSockets.begin() + index);
            clientUsernames.erase(clientUsernames.begin() + index);
            closesocket(clientSocket);
            break;  // End the thread when the client disconnects
        }

        // Display the received data
        buffer[bytesRead] = '\0';
        ChatMessage receivedMessage = ChatMessage::deserialize(buffer);
        std::cout << "[" << receivedMessage.serialize() << "]: " << receivedMessage.getContent() << std::endl;

        // Send data from the client or server
        std::cout << "Enter message: ";
        std::string inputMessage;
        std::getline(std::cin, inputMessage);

        ChatMessage chatMessage(username, inputMessage);
        sendMessage(clientSocket, chatMessage);

        if (send(clientSocket, chatMessage.serialize().c_str(), chatMessage.serialize().length(), 0) == SOCKET_ERROR) {
            std::cerr << "Error sending data to client. Error: " << WSAGetLastError() << std::endl;
            // Remove clientSocket and username from the list if the client has disconnected
            size_t index = &clientSocket - &clientSockets[0];
            clientSockets.erase(clientSockets.begin() + index);
            clientUsernames.erase(clientUsernames.begin() + index);
            closesocket(clientSocket);
            break;  // End the thread when the client disconnects
        }
    }

    closesocket(clientSocket);
}

void LikeChatConnection::sendMessage(SOCKET clientSocket, const ChatMessage& message) {
    std::string serializedMessage = message.serialize();
    send(clientSocket, serializedMessage.c_str(), serializedMessage.length(), 0);
}
