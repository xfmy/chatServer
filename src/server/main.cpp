/*
网络层
|
^
业务层==>数据层

*/
#include <muduo/base/Logging.h>
#include <chatServer.h>
int main(int argc,char** argv)
{
    try{
        int port = 12345;
        if (argc == 2)
        {
            port = std::atoi(argv[1]);
        }
        chatServer server(port);
        server.init();
        server.start();
    }
    catch (...)
    {
        LOG_FATAL << "捕捉到位置崩溃错误,纪念一下";
    }
}

