#include "package.h"
#include <strings.h>
#include <muduo/net/Endian.h>
using namespace muduo::net::sockets;
const unsigned short package::head;

package::package(const std::string& buf)
{
    this->len = hostToNetwork32(buf.size());
    this->data = buf;
    this->checksum = std::hash<std::string>{}(buf);
    this->encapsulation();
}

const char *package::encapsulation()
{
    buf.clear();
    buf.append((const char *)&package::head, sizeof package::head);
    buf.append((const char *)&len, sizeof len);
    buf.append(data);
    buf.append((const char *)&checksum, sizeof checksum);
    return buf.c_str();
}

// std::string package::encapsulation(const std::string &)
// {

//     return std::string();
// }

void package::parse()
{
    parse(this->buf);
}

std::optional<package> package::parse(std::string_view view)
{
    std::optional<package> ret;
    package obj;
    // 寻找包头
    size_t index = view.find(reinterpret_cast<const char*>(&package::head), sizeof package::head);
    if (index == std::string_view::npos)
        return ret;
    int size = view.size();
    int emptyPackageSize = index + sizeof(package::head) + sizeof(package::len) + sizeof(package::checksum);
    //判断最低长度是否满足
    if (emptyPackageSize < size)
        return ret;
    index += sizeof package::head;
    obj.len = networkToHost32(*(int *)(view.begin() + index));
    if (index - package::head + obj.len + emptyPackageSize > size)
        return ret;
    index += sizeof package::len;
    obj.data = std::string(view.begin() + index, obj.len);
    index += obj.data.size();
    obj.checksum = networkToHost32(*(int *)(view.begin() + index));
    //校验和
    if (obj.checksum == std::hash<std::string>{}(obj.data))
        ret.emplace(obj);
    return ret;
}

int package::GetSize()
{
    return buf.empty() ? sizeof head + sizeof len + buf.size() + sizeof checksum : buf.size();
}