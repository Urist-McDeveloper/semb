cmake_minimum_required(VERSION 3.20)
project(semb C)

add_executable(semb main.c)
set_target_properties(semb PROPERTIES
        C_STANDARD 90
        C_EXTENSIONS OFF)

function(semb_generate target output)
    set(out "${CMAKE_CURRENT_BINARY_DIR}/${output}")
    cmake_path(GET out PARENT_PATH out_dir)
    file(MAKE_DIRECTORY ${out_dir})

    add_custom_command(
            OUTPUT ${output}
            DEPENDS ${ARGN}
            COMMAND semb ${ARGN} > ${output}
    )
    target_sources(${target} PRIVATE ${output})
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

option(SEMB_BUILD_TEST "Build semb-test target; requires <sys/stat.h>" OFF)
if (SEMB_BUILD_TEST)
    add_executable(semb-test test.c)
    set_target_properties(semb-test PROPERTIES
            C_STANDARD 90
            C_EXTENSIONS OFF)
    semb_generate(semb-test embed/test.h
            ${CMAKE_CURRENT_LIST_DIR}/main.c
            ${CMAKE_CURRENT_LIST_DIR}/test.c)
endif()
