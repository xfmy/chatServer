# 设置cmkae最小版本
cmake_minimum_required(VERSION  3.2)

#定义项目名
project(TinyIM)

# 设置编译调试版本
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#设置c++标准
set(CMAKE_CXX_STANDARD 17)

# 指定可执行程序输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 构建子目录
add_subdirectory(src) 
