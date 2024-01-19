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
            thread.join();  // Chắc chắn tất cả các thread con đã kết thúc trước khi thoát
        }
    }
    WSACleanup();
}

void LikeChatConnection::startServer() {
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
        // Chấp nhận kết nối từ client mới
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept client connection. Error: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        // Thực hiện xác thực cho client
        if (authenticateClient(clientSocket)) {
            // Thêm clientSocket và username vào danh sách
            clientSockets.push_back(clientSocket);
            std::cout << "New client connected." << std::endl;

            // Bắt đầu một thread mới để xử lý kết nối của client
            clientThreads.emplace_back(&LikeChatConnection::handleCommunication, this, clientSocket, username);
        } else {
            // Đóng kết nối nếu xác thực thất bại
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

    // Kiểm tra xác thực (đơn giản, bạn có thể thay thế bằng phương thức an toàn hơn)
    if (username == "valid_username") {
        // Gửi xác nhận đăng nhập thành công
        const char* successMessage = "Authentication successful. Welcome!";
        if (send(clientSocket, successMessage, strlen(successMessage), 0) == SOCKET_ERROR) 
        {
            std::cerr << "Error sending data to client. Error: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            return false;
        }
        // Thêm username vào danh sách
        clientUsernames.push_back(username);

        return true;
    } else {
        // Gửi thông báo lỗi đăng nhập
        std::cerr << "Authentication failed for user: " << username << std::endl;
        const char* errorMessage = "Authentication failed. Invalid username.";
        if (send(clientSocket, errorMessage, strlen(errorMessage), 0) == SOCKET_ERROR)
        {
            std::cerr << "Error sending data to client. Error: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            return false;
        }
    }
}

void LikeChatConnection::handleCommunication(SOCKET clientSocket, const std::string& username) {
    char buffer[1024];

    while (true) {
        // Nhận dữ liệu từ client hoặc server
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            if (bytesRead == 0) {
                std::cerr << "Client disconnected gracefully." << std::endl;
            } else {
                std::cerr << "Error receiving data from client. Error: " << WSAGetLastError() << std::endl;
            }
            // Xóa clientSocket và username khỏi danh sách nếu client đã ngắt kết nối
            size_t index = &clientSocket - &clientSockets[0];
            clientSockets.erase(clientSockets.begin() + index);
            clientUsernames.erase(clientUsernames.begin() + index);
            closesocket(clientSocket);
            break;  // Kết thúc thread khi client disconnects
        }

        // Hiển thị dữ liệu nhận được
        buffer[bytesRead] = '\0';
        ChatMessage receivedMessage = ChatMessage::deserialize(buffer);
        std::cout << "[" << receivedMessage.serialize() << "]: " << receivedMessage.getContent() << std::endl;
        // std::cout << "[" << clientUsernames[&clientSocket - &clientSockets[0]] << "]: " << buffer << std::endl;

        // Gửi dữ liệu từ client hoặc server
        std::cout << "Enter message: ";
        std::string inputMessage;
        std::getline(std::cin, inputMessage);

        ChatMessage chatMessage(username, inputMessage);
        sendMessage(clientSocket, chatMessage);

        if (send(clientSocket, chatMessage.serialize().c_str(), chatMessage.serialize().length(), 0) == SOCKET_ERROR) {
            std::cerr << "Error sending data to client. Error: " << WSAGetLastError() << std::endl;
            // Xóa clientSocket và username khỏi danh sách nếu client đã ngắt kết nối
            size_t index = &clientSocket - &clientSockets[0];
            clientSockets.erase(clientSockets.begin() + index);
            clientUsernames.erase(clientUsernames.begin() + index);
            closesocket(clientSocket);
            break;  // Kết thúc thread khi client disconnects
        }
    }

    // Đóng socket khi kết thúc
    closesocket(clientSocket);
}

void LikeChatConnection::sendMessage(SOCKET clientSocket, const ChatMessage& message) {
    std::string serializedMessage = message.serialize();
    send(clientSocket, serializedMessage.c_str(), serializedMessage.length(), 0);
}
