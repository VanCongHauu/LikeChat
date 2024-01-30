// LikeChat.cpp
#include "LikeChat_connection.h"
#include <iostream>
#include <string>

int main() {
    // Get user information for login
    std::string username;
    std::cout << "Enter your username: ";
    std::cin >> username;

    // Determine if it's a server or client based on user input
    char userType = '\0';
    std::cout << "Are you a server (s) or a client (c)? ";
    std::cin >> userType;

    LikeChatConnection connection;

    if (userType == 's') {
        // If server, create the server instance and pass it by pointer
        LikeChatServer* server = new LikeChatServer();
        connection = LikeChatConnection(server);
    } else if (userType == 'c') {
        // If client, create the client instance and pass it by pointer
        LikeChatClient* client = new LikeChatClient(username);
        connection = LikeChatConnection(client);
    } else {
        std::cerr << "Invalid choice. Please enter 's' for server or 'c' for client." << std::endl;
        return 1;  // Exit with an error code
    }

    connection.run();

    return 0;
}
