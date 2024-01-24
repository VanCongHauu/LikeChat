#ifndef LIKECHAT_SERVER_H
#define LIKECHAT_SERVER_H

#include "LikeChat_connection.h"

class LikeChatServer {
public:
    LikeChatServer();
    void startServer();

private:
    LikeChatConnection connection;
};
#endif