cmake_minimum_required(VERSION 3.7)
project(bsp_file_reader)

set(CMAKE_C_STANDARD 11)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -arch x86_64")

set(SOURCE_FILES main.c auxiliary.c auxiliary.h
        fast_de430bsp/fast_de430bsp.c fast_de430bsp/fast_de430bsp.h
        slow_de430bsp/slow_de430bsp.c slow_de430bsp/slow_de430bsp.h
        tests/tests.c tests/tests.h tests/swephexp.h tests/sweodef.h)

add_executable(bsp_file_reader ${SOURCE_FILES})
target_link_libraries(bsp_file_reader ${CMAKE_CURRENT_SOURCE_DIR}/tests/libswe.a)

