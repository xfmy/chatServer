#pragma once

#include <string>
#include <vector>
class messageModel
{
public:
    void storingMessage(int id, std::string msg);
    std::vector<std::string> pullOfflineMessages(int id);
private:
};