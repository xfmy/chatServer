#include <string>

class OffLineMessage
{
private:
    int userId;          //用户id
    std::string message; //离线存储的消息(json格式)
};
