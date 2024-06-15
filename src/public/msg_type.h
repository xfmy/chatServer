#pragma once
/**
 * @brief EnMsgType枚举对应着这种聊天事件类型
 *
 */
enum EnMsgType
{
    LOGIN_MSG = 1,    // 登录消息
    LOGIN_MSG_ACK,    // 登录响应消息
    LOGINOUT_MSG,     // 退出登录
    REG_MSG,          // 注册消息
    REG_MSG_ACK,      // 注册响应消息
    ONE_CHAT_MSG,     // 一对一聊天消息
    ONE_CHAT_MSG_ACK, // 一对一聊天响应消息
    GROUP_CHAT_MSG,   // 多人群聊消息
    ADD_FRIEND_MSG,   // 添加好友消息
    CREATE_GROUP_MSG, // 创建群组
    ADD_GROUP_MSG,    // 加入群组
};