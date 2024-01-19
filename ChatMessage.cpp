#include "ChatMessage.h"
#include <sstream>

ChatMessage::ChatMessage(const std::string& sender, const std::string& content)
    : sender(sender), content(content), timestamp(std::time(nullptr)) {}

std::string ChatMessage::serialize() const {
    std::ostringstream oss;
    oss << timestamp << "|" << sender << "|" << content;
    return oss.str();
}

ChatMessage ChatMessage::deserialize(const std::string& serializedMessage) {
    std::istringstream iss(serializedMessage);
    std::string timestampStr, sender, content;
    std::getline(iss, timestampStr, '|');
    std::getline(iss, sender, '|');
    std::getline(iss, content, '|');

    ChatMessage message(sender, content);
    message.timestamp = std::stoll(timestampStr);
    return message;
}
