/*
应该包含网络层于,业务层

*/
#include "chatNetworkLayer.h"
#include "chatBusiness.h"
class chatServer{
public:
    chatServer(int port);
    void init();
    void start();
    void initDB();
private:
    chatBusiness business;
    chatNetworkLayer netWork;
};