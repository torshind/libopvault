aux_source_directory(../include TEST_LIST)
aux_source_directory(. TEST_LIST)
add_executable(test_${PROJECT_NAME} ${TEST_LIST})
target_link_libraries(test_${PROJECT_NAME} LINK_PUBLIC ${PROJECT_NAME})
