# CMake Specification for C++ Version of the CS857 ISH Shell Infrastructure

# Basic CMake Setup
cmake_minimum_required(VERSION 3.10)

project(ish)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Boost库
find_package(Boost REQUIRED)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost libraries")
endif()

# 添加Boost头文件路径
include_directories(${Boost_INCLUDE_DIRS})

# 添加源文件
add_executable(ish
    src/main.cpp
    src/shell.cpp
    src/command.cpp
)

# 链接Boost库
target_link_libraries(ish ${Boost_LIBRARIES})

# 添加编译选项
if(UNIX)
    target_compile_definitions(ish PRIVATE __unix__)
endif()

# Comment outthe line above and uncomment this instead if you want 
# to use the old C parser instead of the new C++ parser.
#add_subdirectory(src-c) 
