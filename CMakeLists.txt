cmake_minimum_required(VERSION 3.20)

project(lua-test)

set(CMAKE_C_STANDARD 11)

add_executable(lua-test src/main.c)
include_directories(src/lua)
target_link_libraries(lua-test m)
target_link_libraries(lua-test lua5.4)
target_link_libraries(lua-test SDL2)