#include "client.h"

Client::Client(int port, const std::string& name) : clientName(name) {
    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Could not create socket\n";
        std::exit(-1);
    }

    // Configure IP and port of server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(port);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Could not connect to server\n";
        close(clientSocket);
        std::exit(-1);
    }

    // Set the socket to non-blocking
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
}

Client::~Client() {
    // Close client socket
    close(clientSocket);

    // Join the receive thread
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
}

void Client::receiveMessages(int clientSocket) {
    char buffer[256];
    while (true) {
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "\nFrom " << buffer << std::endl;
        } else if (bytesRead == 0) {
            std::cout << "Connection closed by server.\n";
            break;
        }
    }
}

void Client::start() {
    std::string message;
    char buffer[256];

    // Send the client name to the server
    ssize_t nameBytesSent = send(clientSocket, clientName.c_str(), clientName.size(), 0);
    if (nameBytesSent < 0) {
        std::cerr << "Error sending client name to server\n";
        return;
    }

    std::cout << "Connected to server with name: " << clientName << "\n";
    // Create a thread for receiving messages from the server
    //receiveThread = std::thread(&Client::receiveMessages, this, clientSocket);
    receiveThread = std::thread([this]() { receiveMessages(clientSocket); });


    while (true) {
        // Send messages to server
        std::cout << "You: ";
        std::getline(std::cin, message);

        // Send the message to the server
        ssize_t bytesSent = send(clientSocket, message.c_str(), message.size(), 0);
        if (bytesSent < 0) {
            std::cerr << "Error sending data to server\n";
            break;
        }

        // Sleep for a short duration to avoid high CPU usage
        usleep(10000);
    }

    // Close client socket
    close(clientSocket);

    // Join the receive thread
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
}