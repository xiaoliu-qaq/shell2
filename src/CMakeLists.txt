# Included CMakeLists for ISH source code
set(SOURCE_FILES
    ish.cpp
    command.cpp)

# The provided parser uses Boost::Spirit to parse command lines
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.70.0)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(ish ${SOURCE_FILES})
    target_link_libraries(ish ${Boost_LIBRARIES})
endif()
