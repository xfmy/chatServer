aux_source_directory(. SRC_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./db DB_LIST)

include_directories(./db)
include_directories(./model)

add_executable(chatServer ${SRC_LIST} ${MODEL_LIST} ${DB_LIST})

target_link_libraries(chatServer muduo_net muduo_base pthread mysqlcppconn config++ config fmt) 