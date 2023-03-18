cmake_minimum_required(VERSION 3.25)
project(semb C)

add_executable(semb main.c test.h)
set_target_properties(semb PROPERTIES
        C_STANDARD 90
        C_EXTENSIONS OFF)

option(SEMB_BUILD_TEST "Build test.c; the file is not checked into git repository" OFF)
if (SEMB_BUILD_TEST)
    add_executable(semb-test test.c test.h)
    set_target_properties(semb-test PROPERTIES
            C_STANDARD 90
            C_EXTENSIONS OFF)
endif()
