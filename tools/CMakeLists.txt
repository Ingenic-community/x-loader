cmake_minimum_required(VERSION 3.13)
project(xloader-tools C CXX ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

add_definitions(-D__HOST__)

add_compile_options(-g)

function(add_legacy_tool NAME FILE)
    add_executable(${NAME} ${FILE})
    target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    target_compile_definitions(${NAME} PUBLIC -D__HOST__)
endfunction()

#add_legacy_tool(ddr_params_creator ddr_params_creator.c)
#add_legacy_tool(efuse_params_creator efuse_params_creator.c)
#add_legacy_tool(gpt_creator gpt_creator.c)
#add_legacy_tool(mbr_creator mbr_creator.c)
#add_legacy_tool(sfc_boot_checksum sfc_boot_checksum.c)
#add_legacy_tool(sfc_timing_params_creator sfc_timing_params_creator.c)
#add_legacy_tool(spl_params_fixer spl_params_fixer.c)
#add_legacy_tool(uart_baudrate_lut uart_baudrate_lut.c)
#add_legacy_tool(wdt_params_creator wdt_params_creator.c)

add_executable(xloader_patch xloader_patch.cpp ../lib/tiny-AES-c/aes.c)
