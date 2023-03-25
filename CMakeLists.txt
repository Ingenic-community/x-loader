cmake_minimum_required(VERSION 3.8)
project(x-loader C ASM)

set(CMAKE_C_STANDARD 99)

include(ExternalProject)

ExternalProject_Add(
        xloader-tools
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/tools
        PREFIX ${CMAKE_BINARY_DIR}/tools
        BINARY_DIR ${CMAKE_BINARY_DIR}/tools
        INSTALL_COMMAND ""
        BUILD_ALWAYS 1
)

MACRO(SUBDIRLIST result curdir)
    FILE(GLOB children ${curdir}/*) # This was changed
    SET(dirlist "")
    FOREACH(child ${children})
        IF(IS_DIRECTORY ${child}) # This was changed
            get_filename_component(dnn ${child} NAME_WE)
            LIST(APPEND dirlist ${dnn})
        ENDIF()
    ENDFOREACH()
    SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(BOARD_LIST ${CMAKE_CURRENT_SOURCE_DIR}/boards)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE TINYLIBC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tinylibc/*.*")
file(GLOB_RECURSE LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lib/*.*")
file(GLOB_RECURSE STAGE2_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/stage2/*.*")
file(GLOB_RECURSE COMMON_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/common/*.*")
file(GLOB_RECURSE DRIVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/drivers/*.*")

set(COMPILE_OPTIONS -Os -g -G 0 -march=mips32r2 -mtune=mips32r2 -mabi=32 -fno-pic
-fno-builtin -mno-abicalls -nostdlib -EL -mhard-float -std=gnu11 -mno-long-calls
-ffunction-sections -fdata-sections)

add_compile_options(${COMPILE_OPTIONS})

add_link_options(
        ${COMPILE_OPTIONS}
        -Wl,--print-memory-usage
        -Wl,--gc-sections
        -Wl,--build-id=none
        -T${CMAKE_CURRENT_SOURCE_DIR}/linker/x-loader.lds
)

string(TIMESTAMP DATE "%Y-%m-%d")
string(TIMESTAMP TIME "%H:%M:%S")

add_definitions(-DX_LOADER_DATE="${DATE}")
add_definitions(-DX_LOADER_TIME="${TIME}")

set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/stage2/init.c PROPERTIES COMPILE_FLAGS -mlong-calls)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/stage2/boot.c PROPERTIES COMPILE_FLAGS -mlong-calls)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/stage2/lib/lzma/LzmaDec.c PROPERTIES COMPILE_FLAGS -mlong-calls)

foreach(CUR_BOARD ${BOARD_LIST})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/boards/${CUR_BOARD}/board.c)
        message(${CUR_BOARD})
        set(BOARD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/boards/${CUR_BOARD}/)
        file(GLOB_RECURSE BOARD_SOURCE_FILE_LIST "${BOARD_DIR}/*.c")
        file(GLOB_RECURSE BOARD_CONFIG_FILE_LIST "${BOARD_DIR}/*.h")
        list(LENGTH BOARD_CONFIG_FILE_LIST BOARD_CONFIG_FILE_LIST_LEN)

        foreach(CUR_BOARD_CONFIG_FILE ${BOARD_CONFIG_FILE_LIST})
            get_filename_component(CUR_BOARD_CONFIG ${CUR_BOARD_CONFIG_FILE} NAME_WE)
            message(${CUR_BOARD_CONFIG_FILE})
            if (BOARD_CONFIG_FILE_LIST_LEN EQUAL 1)
                set(CUR_TARGET_NAME xloader_${CUR_BOARD})
            else()
                set(CUR_TARGET_NAME xloader_${CUR_BOARD}_${CUR_BOARD_CONFIG})
            endif()
            add_executable(${CUR_TARGET_NAME}.elf startup/start.S xloader.c storage.c ${BOARD_SOURCE_FILE_LIST} ${COMMON_SOURCES} ${DRIVER_SOURCES} ${LIB_SOURCES} ${TINYLIBC_SOURCES} ${STAGE2_SOURCES})
            # target_link_libraries(${CUR_TARGET_NAME}.elf gcc)
            target_compile_definitions(${CUR_TARGET_NAME}.elf PUBLIC -DXLOADER_BOARD="${CUR_BOARD}")
            target_compile_definitions(${CUR_TARGET_NAME}.elf PUBLIC -DXLOADER_BOARD_CONFIG_FILE="${CUR_BOARD_CONFIG_FILE}")
            add_custom_command(TARGET ${CUR_TARGET_NAME}.elf POST_BUILD
                    COMMAND ${CMAKE_OBJCOPY}
                    ARGS --gap-fill=0xff --remove-section=.dynsym* --remove-section=.MIPS* -O binary -j .text -j .data -j .bss -j .rodata
                        ${PROJECT_BINARY_DIR}/${CUR_TARGET_NAME}.elf ${CUR_TARGET_NAME}_stage1.bin
                    COMMENT "Building stage1 binary: ${CUR_TARGET_NAME}_stage1.bin")
            add_custom_command(TARGET ${CUR_TARGET_NAME}.elf POST_BUILD
                    COMMAND ${CMAKE_OBJCOPY}
                    ARGS --gap-fill=0xff --remove-section=.dynsym* -O binary -j .text_dram -j .data_dram -j .rodata_dram
                    ${PROJECT_BINARY_DIR}/${CUR_TARGET_NAME}.elf ${CUR_TARGET_NAME}_stage2.bin
                    COMMENT "Building stage2 binary: ${CUR_TARGET_NAME}_stage2.bin")
        endforeach()
    endif()
endforeach()
