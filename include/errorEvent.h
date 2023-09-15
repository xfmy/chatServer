#pragma once
#include "muduoPublic.h"
#include <nlohmann/json.hpp>
#include "package.h"
#include "networkService.h"
class errorEvent
{
public:
    static void protocolParseError(const networkService obj)
    {
        nlohmann::json js;
        js["msg"] = "协议解析失败,请确认是否正确";
        js["code"] = 404;
        obj.response(js);
    }
};


