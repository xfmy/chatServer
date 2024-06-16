#include "package.h"
#include <memory>
#include <muduo/net/Endian.h>
using namespace muduo::net::sockets;
const unsigned short package::head;
#define htonl(xx) hostToNetwork32(xx);
#define ntohl(xx) networkToHost32(xx);

std::string package::encapsulation(const std::string &msg)
{
    std::string data;

    // 添加包头
    short _head = hostToNetwork16(head);
    data.append((const char *)(&_head), sizeof _head);

    // 添加包大小
    int32_t len = msg.size();
    len = htonl(len);
    data.append((const char *)(&len), sizeof len);

    // 添加包数据
    data.append(msg);

    // 添加包校验码
    int32_t checkSum = std::hash<std::string>{}(msg);
    checkSum = htonl(checkSum);
    data.append((const char *)(&checkSum), sizeof checkSum);

    return data;
}

int package::parse(std::string_view view, std::string &data)
{
    // 寻找包头
    short _head = networkToHost16(head);
    size_t index =
        view.find(reinterpret_cast<const char *>(&_head), 0, sizeof _head);
    if (index == std::string_view::npos) return 0;
    int size = view.size();

    //计算空数据包长度(最小包长度)
    int emptyPackageSize = index + sizeof(package::head) + 8;

    //判断最低长度是否满足
    if (emptyPackageSize > size)
        return 0;

    // 获取包长度数据
    index += sizeof package::head;
    int32_t len = ntohl(*(int32_t *)(view.begin() + index));

    if (len + emptyPackageSize > size) 
        return 0;
    index += 4;

    // 获取数据
    data = std::string(view.begin() + index, len);
    index += len;
    int32_t checksum = ntohl(*(int *)(view.begin() + index));

    //校验和
    if (checksum != static_cast<int32_t>(std::hash<std::string>{}(data)))
        return 0;
    index += 4;

    //返回解析成功的包长度
    return index;
}
