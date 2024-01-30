#include "server.h"

Server::Server() {
    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Could not create socket\n";
        std::exit(-1);
    }

    // Configure IP/Port of server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8888);

    // Bind socket to address and port
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Could not bind socket\n";
        close(serverSocket);
        std::exit(-1);
    }

    // Set the socket to non-blocking
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);

    // Listen
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Could not listen to the client\n";
        close(serverSocket);
        std::exit(-1);
    }

    std::cout << "Listening on port 8888...\n";
}

Server::~Server() {
    // Close server socket
    close(serverSocket);

    // Join client threads
    for (auto& clientThread : clientThreads) {
        if (clientThread.joinable()) {
            clientThread.join();
        }
    }
}

void Server::setClientName(int clientSocket, const std::string& name) {
    std::lock_guard<std::mutex> lock(clientMutex);
    clientNames[clientSocket] = name;
}

void Server::broadcastMessage(const std::string& message, int senderSocket) {
    std::cout << "Entering broadcastMessage";
    std::lock_guard<std::mutex> lock(clientMutex);
    std::cout << "Mutex locked" << std::endl;

    // Get the sender's name
    std::string senderName = "Client_" + std::to_string(senderSocket);
    std::cout << "Name0 to send:" << senderName;
    auto it = clientNames.find(senderSocket);
    if (it != clientNames.end()) {
        senderName = it->second;
        std::cout << "Name to send:" << senderName << std::endl;
    } else {
        std::cerr << "Sender name not found in clientNames" << std::endl;
    }

    // Construct the full message with sender's name
    std::cout << "Sender Name: " << senderName << std::endl;
    std::cout << "Message: " << message << std::endl;

    // Debug prints for each part of the message
    std::string part1 = senderName;
    std::cout << "Part1 to fw: " << part1 << std::endl;

    std::string part2 = ": ";
    std::cout << "Part2 to fw: " << part2 << std::endl;

    std::string part3 = message;
    std::cout << "Part3 to fw: " << part3 << std::endl;

    std::string fullMessage = part1 + part2 + part3;
    std::cout << "Mess to fw:" << fullMessage << std::endl;

    // Broadcast the message to all clients except the sender
    std::cout << "Inside broadcastMessage - Connected clients: " << connectedClients.size() << std::endl;

    for (const auto& client : connectedClients) {
        std::cout << "Inside broadcastMessage - Iterating over client: " << client.first << std::endl;

        // Skip the sender
        if (client.first != senderSocket) {
            std::cout << "Inside broadcastMessage - Sending to client: " << client.first << std::endl;
            ssize_t bytesSent = send(client.first, fullMessage.c_str(), fullMessage.size(), 0);
            std::cout << "Inside broadcastMessage - Sent to client: " << client.first << std::endl;

            if (bytesSent < 0) {
                std::cerr << "Error sending data to client: " << strerror(errno) << std::endl;
            }
        }
    }

}


void Server::handleClient(int clientSocket) {
    std::cout << "Handling client: " << clientSocket << std::endl;
    char buffer[256];
    while (true) {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';

            // Get the client's name
            std::string clientName = "Client_" + std::to_string(clientSocket);
            auto it = clientNames.find(clientSocket);
            if (it != clientNames.end()) {
                clientName = it->second;
            }

            std::cout << clientName << ": " << buffer << std::endl;
            std::cout << "Before broadcastMessage call" << std::endl;
            // Broadcast the message to all clients
            broadcastMessage(buffer, clientSocket);
        } else if (bytesRead == 0) {
            std::cout << "Client disconnected\n";

            // Remove the client from the list and names map
            std::lock_guard<std::mutex> lock(clientMutex);
            auto it = std::find_if(
                connectedClients.begin(),
                connectedClients.end(),
                [clientSocket](const std::pair<int, int>& client) {
                    return client.first == clientSocket;
                });

            if (it != connectedClients.end()) {
                connectedClients.erase(it);
                clientNames.erase(clientSocket); // Remove the client's name
            }

            close(clientSocket);
            break;
        }
    }
}

void Server::start() {
    while (true) {
        int newClientSocket = accept(serverSocket, nullptr, nullptr);
        if (newClientSocket != -1) {
            int nextPort = getNextPort();
            fcntl(newClientSocket, F_SETFL, O_NONBLOCK);

            // Prompt the client for a name
            char buffer[256];
            ssize_t nameBytesRead = recv(newClientSocket, buffer, sizeof(buffer), 0);

            if (nameBytesRead > 0) {
                buffer[nameBytesRead] = '\0';
                std::string clientName(buffer);
                setClientName(newClientSocket, clientName);

                // Add the new client to the connectedClients vector
                std::lock_guard<std::mutex> lock(clientMutex);
                connectedClients.emplace_back(newClientSocket, nextPort);

                clientThreads.emplace_back([this, newClientSocket, nextPort]() {
                    handleClient(newClientSocket);
                });

                std::cout << "New client connected on port " << nextPort << " with name: " << clientName << "\n";
            } else {
                std::cerr << "Error receiving client name\n";
                close(newClientSocket);
            }
        }

        usleep(10000);
    }
}


int Server::getNextPort() {
    static int portCounter = 8888;
    return portCounter++;
}

