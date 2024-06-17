#pragma once
#include <nlohmann/json.hpp>
#include "network_service.hpp"
class ErrorEvent
{
public:
    static void protocolParseError(const NetworkService obj)
    {
        nlohmann::json js;
        js["msg"] = "协议解析失败,请确认是否正确";
        js["code"] = 404;
        obj.send(js);
    }
};
