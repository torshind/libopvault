aux_source_directory(../include SRC_LIST)
aux_source_directory(. SRC_LIST)
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} cryptopp sqlite3)
