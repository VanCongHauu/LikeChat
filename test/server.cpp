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

void Server::handleClient(int clientSocket) {
    char buffer[256];
    while (true) {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "Client: " << buffer << std::endl;

            // Prompt the server for a reply
            std::string serverReply;
            std::cout << "Server: ";
            std::getline(std::cin, serverReply);

            // Send the reply to the client
            ssize_t bytesSent = send(clientSocket, serverReply.c_str(), serverReply.size(), 0);
            if (bytesSent < 0) {
                std::cerr << "Error sending data to client\n";
            }
        } else if (bytesRead == 0) {
            std::cout << "Client disconnected\n";
            close(clientSocket);
            break;
        }
    }
}

void Server::start() {
    while (true) {
        int newClientSocket = accept(serverSocket, nullptr, nullptr);
        if (newClientSocket != -1) {
            fcntl(newClientSocket, F_SETFL, O_NONBLOCK);
            //clientThreads.emplace_back(std::thread(&Server::handleClient, this, newClientSocket));
            clientThreads.emplace_back([this, newClientSocket]() { handleClient(newClientSocket); });
            std::cout << "New client connected\n";
        }

        usleep(10000);
    }
}
