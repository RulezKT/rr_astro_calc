cmake_minimum_required(VERSION 3.7)
project(rr_astro_calc)

set(CMAKE_C_STANDARD 11)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -arch x86_64")

set(SOURCE_FILES main.c auxiliary/auxiliary.c auxiliary/auxiliary.h
        430bsp/fast_de430bsp.c 430bsp/fast_de430bsp.h
        tests/tests.c tests/tests.h tests/swephexp.h tests/sweodef.h
        human_map/human_map.c human_map/human_map.h)

add_executable(rr_astro_calc ${SOURCE_FILES})
target_link_libraries(rr_astro_calc ${CMAKE_CURRENT_SOURCE_DIR}/tests/libswe.a)

