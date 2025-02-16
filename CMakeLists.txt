# CMake Specification for C++ Version of the CS857 ISH Shell Infrastructure

# Basic CMake Setup
cmake_minimum_required(VERSION 3.10)

project(ish)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Boost库
find_package(Boost 1.65 REQUIRED COMPONENTS system filesystem)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost libraries")
endif()

# 添加编译选项
add_compile_options(-Wall -Wextra)

# 添加Boost头文件路径
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src)

# 添加源文件
add_executable(ish
    src/main.cpp
    src/shell.cpp
    src/command.cpp
)

# 添加Unix特定的编译选项
if(UNIX)
    target_compile_definitions(ish PRIVATE 
        __unix__
        _POSIX_C_SOURCE=200809L
        _GNU_SOURCE
    )
endif()

# 链接Boost库
target_link_libraries(ish PRIVATE
    ${Boost_LIBRARIES}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
)

# Comment outthe line above and uncomment this instead if you want 
# to use the old C parser instead of the new C++ parser.
#add_subdirectory(src-c) 
