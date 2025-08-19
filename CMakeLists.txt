cmake_minimum_required(VERSION 3.30)
project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

add_executable(main)
target_sources(main PUBLIC
        # Library
        src/library/bundle.cc
        src/library/reader.cc
        src/library/compression.cc
        src/library/texture.cc
)
# Can't figure out how to move bcdec out of here, sorry
include_directories(include modules/bcdec)

# Libraries

if(EMSCRIPTEN)
    target_sources(main PUBLIC src/platform/wasm.cc)

    #set_target_properties(main PROPERTIES LINK_FLAGS    "-s WASM=1 -sALLOW_MEMORY_GROWTH -sINITIAL_MEMORY=64MB --no-entry -sENVIRONMENT=web -std=c++17 -sEXPORTED_FUNCTIONS=_open,_init")
    set_target_properties(main PROPERTIES LINK_FLAGS    "-Os -s WASM=1 -sALLOW_MEMORY_GROWTH -sINITIAL_MEMORY=64MB -sSTANDALONE_WASM=1 --no-entry -sENVIRONMENT=web -std=c++17 -sEXPORTED_FUNCTIONS=_open,_init")
else()
    target_sources(main PUBLIC src/platform/debug.cc)
endif()

## 7zip (LZMA)

file(GLOB SEVENZIP_SRC modules/7zip/C/*.c)
add_library(7zip ${SEVENZIP_SRC})
target_include_directories(7zip PUBLIC modules/7zip/C)
target_link_libraries(main PRIVATE 7zip)

## LZ4

add_subdirectory(modules/lz4/build/cmake)
target_link_libraries(main PRIVATE lz4)

## fpng

add_library(fpng modules/fpng/src/fpng.cpp)
target_include_directories(fpng PUBLIC modules/fpng/src)
target_link_libraries(main PRIVATE fpng)