cmake_minimum_required(VERSION 3.20)
project(semb C)

add_executable(semb semb.c)
set_target_properties(semb PROPERTIES
        C_STANDARD 90
        C_EXTENSIONS OFF)

function(semb_generate target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "OUT" "FILES")
    list(TRANSFORM arg_FILES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

    cmake_path(GET arg_OUT PARENT_PATH out_dir)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${out_dir}")

    add_custom_command(
            OUTPUT ${arg_OUT}
            DEPENDS ${arg_FILES}
            COMMAND semb ${arg_FILES} > ${arg_OUT}
    )
    target_sources(${target} PRIVATE ${arg_OUT})
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

option(SEMB_BUILD_TEST "Build semb-test target; requires <sys/stat.h>" OFF)
if (SEMB_BUILD_TEST)
    add_executable(semb-test test.c)
    set_target_properties(semb-test PROPERTIES
            C_STANDARD 90
            C_EXTENSIONS OFF)
    semb_generate(semb-test OUT embed/test.h FILES
            semb.c
            test.c)
endif()
