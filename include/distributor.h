// #pragma once
// #include "baseVersion.h"
// #include "chatBusiness.h"
// #include "msgType.h"
// #include <map>
// #include <string>
// #include <memory>
// #include <mutex>
// #include <unordered_map>

// class distributor
// {
// public:
//     distributor();
//     ~distributor();
//     //void addVersion(const string protocol, std::unique_ptr<baseVersion> version);
//     //void handlerEvent(std::string, const TcpConnectionPtr &ptr, nlohmann::json& js);

//     void onMessageCallback(const TcpConnectionPtr &ptr, const std::string& buf, Timestamp);
//     //void onThreadInitCallback(EventLoop *);
//     void onConnectCallback(const TcpConnectionPtr &ptr);
//     void onWriteCompleteCallback(const TcpConnectionPtr &ptr);

    
//     //void onMessageCompleteCallback(const TcpConnectionPtr &ptr, const std::string &mes, Timestamp time);

// private:
//     //TODO: 要默认添加一个异常处理 404,,,函数感觉不行,应该写一个errorevent类处理
//     //void errorEvent(const TcpConnectionPtr &ptr);
//     //void send(const TcpConnectionPtr &ptr, const std::string &buf);
//     // void response(const TcpConnectionPtr &ptr, const nlohmann::json &);

//     // 记录不同的解析类
//     //std::map<std::string, std::unique_ptr<baseVersion>> map;
//     //const std::string version;
//     chatBusiness business;
//     // 记录所有用户连接的map
//     //std::unordered_map<int, TcpConnectionPtr> userConnectMap;
//     //注册所有的事件回调函数
//     std::unordered_map<EnMsgType, businessEventCallback> userEventCallbackMap;
//     // 对于map操作保证其线程安全性
//     //std::mutex mutex;
// };
