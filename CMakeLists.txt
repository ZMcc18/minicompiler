cmake_minimum_required(VERSION 3.10)
project(MiniCompiler VERSION 0.1.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_COMPILE "gcc")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加子目录
add_subdirectory(src)

# 启用测试
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装规则
install(TARGETS minicompiler
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# 打包配置
set(CPACK_PACKAGE_NAME "MiniCompiler")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple compiler for learning compiler principles")
include(CPack) 