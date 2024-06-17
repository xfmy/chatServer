# 指定头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/src/public)

# 构建子目录
add_subdirectory(server)
add_subdirectory(client) 
