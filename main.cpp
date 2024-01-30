#include "server.h"
#include "client.h"
#include <iostream>
#define serverPort 8888

int main() {
    std::cout << "Choose mode: (1) Server or (2) Client: ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        Server server;
        server.start();
    } else if (choice == 2) {
        // Prompt the user to enter the server's port
        //std::cout << "Enter server port: ";
        //int serverPort;
        //std::cin >> serverPort;

        // Prompt the user to enter the client's name
        std::cin.ignore(); // Clear the newline character
        std::string clientName;
        std::cout << "Enter your name: ";
        std::getline(std::cin, clientName);

        // Create the client with the specified server port and client name
        Client client(serverPort, clientName);
        client.start();
    } else {
        std::cout << "Invalid choice. Exiting...\n";
        return -1;
    }

    return 0;
}

// g++ -o chatApp main.cpp server.cpp client.cpp -pthread