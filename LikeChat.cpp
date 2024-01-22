#include "LikeChat_server.h"
#include "LikeChat_client.h"
#include <iostream>

int main() {
    // Get user information for login
    std::string username;
    std::cout << "Enter your username: ";
    std::cin >> username;

    // Determine if it's a server or client based on user input
    char userType;
    std::cout << "Are you a server (s) or a client (c)? ";
    std::cin >> userType;

    if (userType == 's') {
        LikeChatServer server;
        server.startServer();
    } else if (userType == 'c') {
        LikeChatClient client(username);
        client.connectToServer();
        client.handleCommunication();
    } else {
        std::cerr << "Invalid choice. Please enter 's' for server or 'c' for client." << std::endl;
    }

    return 0;
}
