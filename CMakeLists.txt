cmake_minimum_required(VERSION 3.10)
project(idobata C)

set(CMAKE_C_STANDARD 99)

include_directories(../mynet)
link_directories(../mynet/cmake-build-debug)

add_executable(idobata
        src/idobata.c
        src/idobata.h
        src/idobata_client.c
        src/idobata_helo.c
        src/idobata_server.c
        src/idobata_util.c
        src/linkedlist.c)

target_link_libraries(idobata mynet)