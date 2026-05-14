cmake_minimum_required(VERSION 3.16)

project(Conduit VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/cpp)

set(PROJECT_SOURCES
    src/cpp/main.cpp
    src/cpp/kioskserver.cpp
)

add_executable(Conduit ${PROJECT_SOURCES})

target_link_libraries(Conduit PRIVATE Threads::Threads)

# Windows requires Winsock for networking (used by cpp-httplib)
if(WIN32)
    target_link_libraries(Conduit PRIVATE ws2_32 wsock32)
endif()

# macOS requires CoreFoundation for filesystem support
if(APPLE)
    target_link_libraries(Conduit PRIVATE "-framework CoreFoundation")
endif()
