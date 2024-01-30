// LikeChat_server.cpp
#include "LikeChat_server.h"
#include <algorithm>

LikeChatServer::LikeChatServer() {
    // Constructor logic
}

LikeChatServer::~LikeChatServer() {
    // Destructor logic
}

void LikeChatServer::startServer() {
    // Logic for starting the server
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Could not create socket\n";
        std::exit(-1);
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8888);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Could not bind socket\n";
        close(serverSocket);
        std::exit(-1);
    }

    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Could not listen to the client\n";
        close(serverSocket);
        std::exit(-1);
    }

    std::cout << "Listening on port 8888...\n";

    while (true) {
        int newClientSocket = accept(serverSocket, nullptr, nullptr);
        if (newClientSocket != -1) {
            clientThreads.emplace_back([this, newClientSocket]() {
                handleClient(newClientSocket);
            });
        }
    }
}

void LikeChatServer::saveUsername(int clientSocket, const std::string& username) {
    // Logic for saving the username of a client
    std::unique_lock<std::mutex> lock(clientMutex);
    clientNames[clientSocket] = username;
}

void LikeChatServer::broadcastMessage(const std::string& message, int senderSocket) {
    // Logic for broadcasting messages to all clients except the sender
    std::unique_lock<std::mutex> lock(clientMutex);

    // Get the sender's name
    std::string senderName = "Client_" + std::to_string(senderSocket);
    auto it = clientNames.find(senderSocket);
    if (it != clientNames.end()) {
        senderName = it->second;
    } else {
        std::cerr << "Sender name not found in clientNames\n";
    }

    // Construct the full message with sender's name
    std::string fullMessage = senderName + ": " + message;

    // Broadcast the message to all clients
    for (const auto& client : clientNames) {
        if (client.first != senderSocket) {
            ssize_t bytesSent = send(client.first, fullMessage.c_str(), fullMessage.size(), 0);
            if (bytesSent < 0) {
                std::cerr << "Error sending data to client: " << strerror(errno) << std::endl;
            }
        }
    }
}

void LikeChatServer::handleClient(int clientSocket) {
    // Logic for handling communication with the client
    char buffer[256];
    std::string username;

    // Prompt the client for a username
    ssize_t nameBytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (nameBytesRead > 0) {
        buffer[nameBytesRead] = '\0';
        username = buffer;
        saveUsername(clientSocket, username);
        std::cout << "New client connected with username: " << username << "\n";
    } else {
        std::cerr << "Error receiving client username\n";
        close(clientSocket);
        return;
    }

    // Handle communication with the client
    while (true) {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << username << ": " << buffer << std::endl;

            // Broadcast the message to all clients
            broadcastMessage(buffer, clientSocket);
        } else if (bytesRead == 0) {
            std::cout << "Client disconnected: " << username << "\n";

            // Remove the client from the list and names map
            std::unique_lock<std::mutex> lock(clientMutex);
            auto it = std::find_if(
                clientThreads.begin(),
                clientThreads.end(),
                [clientSocket](const std::thread& clientThread) {
                    return clientThread.get_id() == std::this_thread::get_id();
                });

            if (it != clientThreads.end()) {
                it->detach();  // Detach the thread to avoid blocking
                clientThreads.erase(it);
                clientNames.erase(clientSocket);  // Remove the client's name
            }

            close(clientSocket);
            break;
        }
    }
}
