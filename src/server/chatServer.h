/*
应该包含网络层于,业务层

*/
#include "tcp_service.h"
#include "chat_session.h"
class ChatServer{
public:
    ChatServer(int port);
    void init();
    void start();
    void initDB();
private:
    ChatSession session;
    TcpService netWork;
};