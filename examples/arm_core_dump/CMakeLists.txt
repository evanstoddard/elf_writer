cmake_minimum_required(VERSION 3.20)
project(arm_core_dump)

set(ELF_WRITER_DIR
    ${CMAKE_SOURCE_DIR}/../../src
)

set(ELF_WRITER_SOURCES
    ${ELF_WRITER_DIR}/elf_writer.c
)

set(SOURCES
    main.c
    ${ELF_WRITER_SOURCES}
)

include_directories(
    ${ELF_WRITER_DIR}
)

add_executable(arm_core_dump
    ${SOURCES}
)