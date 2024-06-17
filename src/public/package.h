/**
 * @file package.h
 * @brief 解决tcp粘包
 * 
 */
#pragma once
#include <string>
#include <optional>
#define MAX_BUF 4096

#pragma pack(push)
#pragma pack(1)
class package
{
public:
    /**
     * @brief 对待发送数据进行序列化
     * 
     * @param msg 需要发送的消息
     * @return std::string 返回序列化的数据
     */
    static std::string encapsulation(const std::string & msg);

    /**
     * @brief 对接收数据进行反序列化
     * 
     * @param view 待解析数据缓冲区
     * @param msg 出参,存放解析出的数据
     * @return int 返回解析出一个包消耗的字节数量
     */
    static int parse(std::string_view view, std::string& msg);

public:
    // 0XFEFF 头部	2字节 
    static const unsigned short head = 0xFEFF;
    // 数据长度			4字节
    //int32 len;
    // 传输数据
    //std::string data;
    // 校验和			4字节
    //int32 checksum;
};
#pragma pack(pop)
