aux_source_directory(. SRC_LIST)
aux_source_directory(./model MODEL_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./redis REDIS_LIST)

include_directories(./redis)
include_directories(./db)
include_directories(./model)

add_executable(chat-server ${SRC_LIST} ${MODEL_LIST} ${DB_LIST} ${REDIS_LIST})

target_link_libraries(chat-server muduo_net muduo_base pthread mysqlcppconn config++ config fmt redis++ hiredis) 