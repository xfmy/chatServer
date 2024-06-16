# 添加所有的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./redis REDIS_LIST)

# 添加头文件搜索目录
include_directories(./redis)
include_directories(./db)
include_directories(./model)

# 生成可执行程序
add_executable(chat-server ${SRC_LIST} ${MODEL_LIST} ${DB_LIST} ${REDIS_LIST} ../public/package.cpp)

# 链接库文件
target_link_libraries(chat-server muduo_net muduo_base pthread mysqlcppconn config++ config fmt redis++ hiredis) 