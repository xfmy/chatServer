// #include "distributor.h"
// #include "package.h"
// #include "errorEvent.h"

// distributor::distributor(){
//     userEventCallbackMap.emplace(EnMsgType::LOGIN_MSG,std::bind(&chatBusiness::login,&business,_1,_2,_3));
//     userEventCallbackMap.emplace(EnMsgType::REG_MSG, std::bind(&chatBusiness::registerUser, &business, _1, _2, _3));
//     userEventCallbackMap.emplace(EnMsgType::ONE_CHAT_MSG, std::bind(&chatBusiness::message, &business, _1, _2, _3));
// }

// distributor::~distributor()
// {
// }

// void distributor::onMessageCallback(const TcpConnectionPtr &ptr, const std::string &mes, Timestamp time)
// {
//     try
//     {
//         nlohmann::json js = nlohmann::json::parse(mes);
//         int a = js["msgType"];
//         EnMsgType msgType = static_cast<EnMsgType>(a);
//         const auto &it = userEventCallbackMap.find(msgType);
//         if (it != userEventCallbackMap.end())
//         {
//             it->second(ptr, js["user"], time);
//             // if(msgType == EnMsgType::LOGIN_MSG){

//             // }
//         }
//         else
//         {
//             errorEvent::protocolParseError(ptr);
//         }
//     }
//     catch (nlohmann::json::exception &e)
//     {
//         // output exception information
//         LOG_ERROR << "message: " << e.what() << '\n'
//                   << "exception id: " << e.id;
//         errorEvent::protocolParseError(ptr);
//     }
// }

// void distributor::onConnectCallback(const TcpConnectionPtr &ptr)
// {
// }

// void distributor::onWriteCompleteCallback(const TcpConnectionPtr &ptr)
// {
// }

// void distributor::addVersion(const string protocol, std::unique_ptr<baseVersion> version)
// {
//     map.emplace(protocol, version);
// }

// void distributor::handlerEvent(std::string version, const TcpConnectionPtr &ptr, nlohmann::json& js)
// {
//     const auto& it = map.find(version);
//     if(it == map.end())
//         errorEvent(ptr);
//     else{
//         it->second->entrance(ptr, js);
//     }
// }

// void distributor::errorEvent(const TcpConnectionPtr &ptr)
// {
//     nlohmann::json js;
//     js["msg"] = "协议解析失败,请确认是否正确";
//     js["code"] = 404;
//     ptr->send(package(js.dump()).buf);
// }

// void distributor::send(const TcpConnectionPtr &ptr, const std::string &buf)
// {
//     ptr->send(package(buf).buf);
// }

// void distributor::response(const TcpConnectionPtr &ptr, const nlohmann::json& data)
// {
//     nlohmann::json js;

// }