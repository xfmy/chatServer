# C++实现集群聊天服务器

## 项目介绍

基于muduo网络模型多线程多Reactor模型和one loop per thread线程模型实现高并发，通过Nginx tcp负载均衡模块和redis的发布订阅功能实现集群聊天服务器

![IM server 架构图](https://s2.loli.net/2024/06/17/3vIYTpXJ4S6wdqZ.png)

## 开发环境

+ Linux kernel version 5.15.146.1(ubuntu 22.04)
+ Gcc version 11.4.0 
+ Cmake version 3.22.1
+ Git version 2.34.1
+ Gdb version 12.1
+ boost version 1.85
+ Mysql version 5.7
+ Redis version 3.0.6
+ Nginx version 1.18.0 
+ muduo 网络库
+ libfmt 格式化库
+ libconfig 配置文件
+ mysqlcppconn Mysql JDBC库
+ nlohmann-json json库
+ redis-plus-plus 基于hiredis的cpp包装库

## 技术栈

+ 使用第三方 JSON 库实现通信数据的序列化和反序列化
+ 自定义数据传输协议,解决tcp粘包问题
+ 基于 muduo 网络库开发网络核心模块，实现高效通信
+ 使用 Nginx 的 TCP 负载均衡功能，将客户端请求分派到多个服务器上，以提高并发处理能力
+ 基于发布-订阅的服务器中间件redis消息队列，解决跨服务器通信难题
+ MySQL 连接池与数据库编程
+ CMake构建编译环境

## 项目功能

1. 客户端新用户注册
2. 客户端用户登录
3. 添加好友和添加群组
4. 好友聊天
5. 群组聊天
6. 离线消息
7. nginx配置tcp负载均衡
8. 集群聊天系统支持客户端跨服务器通信

## 启动项目

### 配置项目环境

#### 导入mysql数据库sql文件

导入***chat.sql***文件

#### 修改数据库配置文件

> 配置文件目录 **conf/database.conf**

+ **host:**数据库ip和port
+ **username**:修改为自己的数据库登录用户名
+ **password:**登陆密码
+ **database:**这个不需要修改,导入的数据库就是**chat**

```
database = {
host = "tcp://127.0.0.1:3306";
userName = "xf";
password = "200212";
databaseName = "chat"
};
```

#### 配置Nginx tcp负载均衡

找到`nginx.conf`文件(`/usr/local/nginx/conf/nginx.conf`或者`/etc/nginx/nginx.conf`)

打开文件添加如下配置信息

> Nginx的权重策略有五种,感兴趣的可以看我另一篇博客[Nginx指令配置大全](https://blog.csdn.net/weixin_50448879/article/details/136801698),这里weight=1就相当于采用**轮询**策略
>
> 配置完成后，`nginx -s reload`平滑重启Nginx。

```nginx
stream {
    # 自定义MyServer负载均衡模块
     upstream MyServer {
        #weight:权重
        #max_fails:超市次数
        #fail_timeout:超时时间
        
        #一个server就相当于一个需要转发的服务器程序,这里仅供测试配了两个
        server 127.0.0.1:8001 weight=1 max_fails=3 fail_timeout=30s;
        server 127.0.0.1:8002 weight=1 max_fails=3 fail_timeout=30s;
     }
     server {
         proxy_connect_timeout 1s;	# 连接超时时间(Nginx与代理服务器)
         listen 8000; 				# 需要监听转发的端口
         proxy_timeout 3600s;		# 默认为10分钟,10分钟内没有网络数据交互就会断开连接
         proxy_pass MyServer;		# 指定8000端口连入后发送至MyServer负载均衡模块中
         tcp_nodelay on;			# Nagle算法
     }
}
```

### 启动服务端

```shell
# 进入可执行文件目录
cd bin/
# 单服务器启动,默认端口为12345
./chat-server

# 配合nginx聊天服务器集群
./chat-server 8001
./chat-server 8002
```

### 启动客户端

```shell
# 进入可执行文件目录
cd bin/
# 直接与服务器连接
./chat-client 127.0.0.1 12345
# 服务器集群
./chat-client 127.0.0.1 8000
```

## 项目解析

服务器采用分层的设计思想，分为网络I/O模块、业务模块、数据模块

![聊天服务器](https://s2.loli.net/2024/06/17/eH7QFmRcNtgbvYz.png)

### 一、网络I/O模块



基于muduo库采用的网络并发模型为**one loop per thread**，即通过一个**main reactor**来负责accept连接，然后该连接的I/O事件，计算任务等都在**sub reactor**中完成。多个连接可能被分派到多个线程中，来充分利用CPU。

利用muduo库写网络模块的代码非常简洁，只需要在定义的ChatServer中定义muduo的TcpServer和EventLoop，然后注册onMessage回调，就可以很方便的起一个服务。

onMessage则是根据不同的json消息中的msgid字段，来在业务层调用不同的回调函数来处理请求。

### 二、业务模块



设计一个负责处理业务的ChatService类，成员：所有的数据库操作类的对象，一个用来映射msgid到回调函数的map，一个互斥锁保证map的访问安全，还有一个存储在线用户的通信连接的map，redis的操作对象。然后所有的业务处理函数都在构造函数中注册到了对应的map上。

**2.1 登陆**

首先判断是否已经登陆，如已经登陆则返回错误提示信息

登陆成功，记录用户的连接信息，更新用户的状态

向redis订阅channel(id)

将离线消息封装到json消息中，然后调用offlinemessageModel中的`remove`方法删除读过的离线消息

显示好友列表：通过friendModel中的`query`方法，通过用户id查找所有的好友id，返回对应的好友user对象

显示群组列表：同显示好友列表，通过groupuserModel中的`query`方法，返回这个用户所在的所有群组的信息

登陆失败，提示错误信息（用户不存在或密码错误）

**2.2 注册**

根据json中的相关信息，新建一个user对象，调用userModel中的`insert`方法插入到数据库中

**2.3 注销**

加锁，删掉userConnMap中的对应的id的TCP连接信息

调用userModel中的`updateState`更新用户的状态信息

**2.4 客户端异常退出**

加锁，删掉对应的连接信息

取消redis的订阅通道

更新当时客户端登陆的用户的状态信息

**2.5 添加好友**

解析json中的字段，调用friendModel的`insert`方法就可以

**2.6 创建群组**

调用groupModel中的`createGroup`和`addGroup`方法，创建群，然后将当前创建人的权限改为创建者

**2.7 添加群组**

同理，非常简单

**2.8 好友聊天**

查询对方id是否在线（首先在本服务器中的userConnMap中查，如在，则将请求的json消息推给该用户即可；如果不在，需要在数据库中查是否在线，如果在线则在redis中发布对方id的channel， 推送消息）

如果不在线，直接存为离线消息

**2.9 群组聊天**

类似好友聊天，区别在于需要获取当前群id的所有user，然后根据他们在线与否，选择直接推送还是存为离线消息。（当不在本机找不到时，通过redis推送）

**2.10 从redis中获取订阅消息**

订阅的channel有消息了，查看对应id的TCP连接，然后通过绑定的回调函数处理(service直接把Json推送到对应的客户端)。

### 三、数据模块



为user, gourpuser, group表设计一个[ORM](http://www.ruanyifeng.com/blog/2019/02/orm-tutorial.html)类，为每一个ORM类添加一个数据操作类，操作ORM类对象。这样的设计将复杂的业务逻辑和数据处理逻辑分离，降低系统的耦合度，有利于扩展。

#### 3.1 数据库设计

**user表**

| 字段名称 | 字段类型                  | 字段说明       | 约束                        |
| -------- | ------------------------- | -------------- | --------------------------- |
| id       | INT                       | 用户id         | PRIMARY KEY, AUTO_INCREMENT |
| name     | VARCHAR(50)               | 用户名         | NOT NULL, UNIQUE            |
| password | VARCHAR(50)               | 用户密码       | NOT NULL                    |
| state    | ENUM('online', 'offline') | 当前的登陆状态 | DEFAULT 'offline'           |

**Friend表**

| 字段名称 | 字段类型 | 字段说明 | 约束               |
| -------- | -------- | -------- | ------------------ |
| userid   | INT      | 用户id   | NOT NULL、联合主键 |
| friendid | INT      | 好友id   | NOT NULL、联合主键 |

**ALLGroup表**

| 字段名称  | 字段类型     | 字段说明 | 约束                        |
| --------- | ------------ | -------- | --------------------------- |
| id        | INT          | 群id     | PRIMARY KEY、AUTO_INCREMENT |
| roupname  | INT          | 群名称   | NOT NULL、UNIQUE            |
| groupdesc | VARCHAR(200) | 群描述   | DEFAULT ''                  |

**GroupUser表**

| 字段名称  | 字段类型                  | 字段说明 | 约束               |
| --------- | ------------------------- | -------- | ------------------ |
| groupid   | INT                       | 群id     | NOT NULL、联合主键 |
| userid    | INT                       | 群成员id | NOT NULL、联合主键 |
| grouprole | ENUM('creator', 'normal') | 群内权限 | DEFAULT 'normal'   |

**OfflineMessage表**

| 字段名称 | 字段类型     | 字段说明                 | 约束     |
| -------- | ------------ | ------------------------ | -------- |
| userid   | INT          | 用户id                   | NOT NULL |
| message  | VARCHAR(200) | 离线消息(存储Json字符串) | NOT NULL |

#### 3.2 连接池设计

为了提高MySQL数据库（基于C/S设计）的访问瓶颈，除了在服务器端增加缓存服务器缓存常用的数据 之外（例如redis），还可以增加连接池，来提高MySQL Server的访问效率，在高并发情况下，大量的 TCP三次握手、MySQL Server连接认证、MySQL Server关闭连接回收资源和TCP四次挥手所耗费的 性能时间也是很明显的，增加连接池就是为了减少这一部分的性能损耗。

