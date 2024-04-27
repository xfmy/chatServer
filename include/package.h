#pragma once
#include <string>
#include <optional>
#define MAX_BUF 4096

// 包的设计类
/*************************************************************************************
 *9 + data（长度）
 * UINT parse(char* buf, UINT& strSz)
 *	参数：（缓冲区，数据大小）
 *	功能： 解析包的数据，
 *	返回值：0-》解析失败，其他-》buf使用大小
 ***************************************************************************************/
#pragma pack(push)
#pragma pack(1)
class package
{
public:
    package() = default;
    package(const std::string &);

    const char *encapsulation(); // 封包，返回数据
    //static std::string encapsulation(const std::string &);
    
    static std::optional<package> parse(std::string_view view);
    void parse();

    int GetSize(); // 获取包大小
public:
    // 0XFEFF 头部	2字节 
    static const unsigned short head = 0xFEFF;
    // 长度			2字节
    int len;
    // 传输数据
    std::string data;
    // 校验和			4字节
    int checksum;
    // 封装之后的数据
    std::string buf;
};
#pragma pack(pop)
