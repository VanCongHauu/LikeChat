#include "ChatMessage.h"
#include <sstream>
#include <iomanip>
#include <chrono> 

ChatMessage::ChatMessage(const std::string& sender, const std::string& content)
    : sender(sender), content(content) {
    timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

std::string ChatMessage::serialize() const {
    std::ostringstream oss;
    oss << sender << ": " << content;
    return oss.str();
}

ChatMessage ChatMessage::deserialize(const std::string& serializedMessage) {
    std::istringstream iss(serializedMessage);
    std::string senderContent;
    if (std::getline(iss, senderContent, '|')) {
        std::string sender = senderContent.substr(0, senderContent.find_first_not_of(" "));
        std::string content = senderContent.substr(senderContent.find_first_not_of(" "));

        ChatMessage message(sender, content);
        return message;
    } else {
        return ChatMessage("", "");
    }
}
