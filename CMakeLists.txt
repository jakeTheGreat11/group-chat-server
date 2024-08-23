cmake_minimum_required(VERSION 3.27)
project(groupChatClients)

set(CMAKE_CXX_STANDARD 17)

add_executable(groupChatClients main.cpp)

target_link_libraries(groupChatClients Ws2_32)