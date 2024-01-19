#include "server.h"
#include "client.h"
#include <iostream>

int main() {
    std::cout << "Choose mode: (1) Server or (2) Client: ";
    int choice;
    std::cin >> choice;

    if (choice == 1) {
        Server server;
        server.start();
    } else if (choice == 2) {
        Client client;
        client.start();
    } else {
        std::cout << "Invalid choice. Exiting...\n";
        return -1;
    }

    return 0;
}
