cmake_minimum_required(VERSION 3.0.0)

project(rpc++ VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILE_FEATURES "${CMAKE_CXX_COMPILE_FEATURES} /MT /GR-")
add_definitions(-DYASIO_HEADER_ONLY /MT /GR-)

include_directories(modules include contract third_party third_party/yasio)

add_executable(client src/client.cc)
add_executable(server src/server.cc)