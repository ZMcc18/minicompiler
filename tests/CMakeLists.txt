set(TEST_SOURCES
    lexer_test.cpp
    parser_test.cpp
    ir_test.cpp
)

add_executable(minicompiler_tests ${TEST_SOURCES})

target_include_directories(minicompiler_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# 链接Google Test
find_package(GTest REQUIRED)
target_link_libraries(minicompiler_tests PRIVATE GTest::GTest GTest::Main)

# 添加测试
add_test(NAME minicompiler_tests COMMAND minicompiler_tests) 