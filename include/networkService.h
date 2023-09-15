#pragma once
#include "package.h"
#include "muduoPublic.h"
#include <nlohmann/json.hpp>
class networkService{
public:
    networkService(const TcpConnectionPtr &ptr_):ptr(ptr_){}
    void response(const nlohmann::json& js) const {
        response(js.dump());
    }
    const TcpConnectionPtr &ptr;
private:
    void response(const std::string& msg) const {
        // package obj;
        // obj.data = msg;
        // obj.encapsulation();
        // ptr->send(obj.buf);
        ptr->send(msg);
    }
};
