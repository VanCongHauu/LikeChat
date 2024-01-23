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
    oss << std::put_time(std::localtime(&timestamp), "%Y-%m-%d %H:%M:%S") << "|" << sender << "|" << content;
    return oss.str();
}

ChatMessage ChatMessage::deserialize(const std::string& serializedMessage) {
    std::istringstream iss(serializedMessage);
    std::string timestampStr, sender, content;
    std::getline(iss, timestampStr, '|');
    std::getline(iss, sender, '|');
    std::getline(iss, content, '|');

    ChatMessage message(sender, content);
    std::istringstream timestampIss(timestampStr);
    timestampIss >> std::get_time(std::localtime(&message.timestamp), "%Y-%m-%d %H:%M:%S");

    return message;
}
