#ifndef CHAT_MESSAGE_H
#define CHAT_MESSAGE_H

#include <string>
#include <ctime>

class ChatMessage {
public:
    ChatMessage(const std::string& sender, const std::string& content);
    std::string serialize() const;
    static ChatMessage deserialize(const std::string& serializedMessage);
    std::string getContent() const {
        return content;
    }
    std::string getSender() const {
        return sender;
    }

private:
    std::string sender;
    std::string content;
    std::time_t timestamp;
};
#endif