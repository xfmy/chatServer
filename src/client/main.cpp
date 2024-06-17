#include <iostream>
#include "chat_session.h"


int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "command invalid! example: ./ChatClient 127.0.0.1 6000"
                  << std::endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    uint16_t port = atoi(argv[2]);
    ChatSession session(argv[1], port);

    // main线程用于接收用户输入，负责发送数据
    while (true)
    {
        // 显示首页面菜单 登录、注册、退出
        std::cout << "========================" << std::endl;
        std::cout << "1. login" << std::endl;
        std::cout << "2. register" << std::endl;
        std::cout << "3. quit" << std::endl;
        std::cout << "========================" << std::endl;
        std::cout << "choice:";
        int choice = 0;
        std::cin >> choice;

        switch (choice)
        {
            case 1: // login业务
            {
                int id;
                std::string password;
                std::cout << "userId:";
                std::cin >> id;
                std::cout << "userpassword:";
                std::cin >> password;

                session.Login(id, password);
            }
            break;
            case 2: // register业务
            {
                std::string userName;
                std::string password;
                std::cout << "username:";
                std::cin >> userName;
                std::cout << "userpassword:";
                std::cin >> password;

                session.registerUser(userName, password);
            }
            break;
            case 3: // quit业务
                exit(0);
                break;
            default: std::cerr << "invalid input!" << std::endl; break;
        }
    }
    return 0;
}