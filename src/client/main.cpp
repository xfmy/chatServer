#include <iostream>
#include <nlohmann/json.hpp>
#include "chat_session.h"
//#include "msgType.h"
using json = nlohmann::json;

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatClient 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    uint16_t port = atoi(argv[2]);
    ChatSession session(argv[1], port);

    // main线程用于接收用户输入，负责发送数据
    while(true)
    {
        // 显示首页面菜单 登录、注册、退出
        cout << "========================" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "3. quit" << endl;
        cout << "========================" << endl;
        cout << "choice:";
        int choice = 0;
        cin >> choice;

        switch (choice)
        {
            case 1: // login业务
            {
                int id;
                std::string password;
                std::cout << "userId:";
                std::cin >> id;
                cout << "userpassword:";
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
                cout << "userpassword:";
                std::cin >> password;

                session.registerUser(userName,password);
            }
            break;
            case 3: // quit业务
                exit(0);
                break;
            default: 
                cerr << "invalid input!" << endl; 
                break;
        }
    }
    return 0;
}