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

        # fpng
        extra/fpng/src/fpng.cpp
)
include_directories(include extra/bcdec extra/fpng/src)

# Libraries

if(EMSCRIPTEN)
    target_sources(main PUBLIC src/platform/wasm.cc)

    #set_target_properties(main PROPERTIES LINK_FLAGS    "-s WASM=1 -sALLOW_MEMORY_GROWTH -sINITIAL_MEMORY=64MB --no-entry -sENVIRONMENT=web -std=c++17 -sEXPORTED_FUNCTIONS=_open,_init")
    set_target_properties(main PROPERTIES LINK_FLAGS    "-Os -s WASM=1 -sALLOW_MEMORY_GROWTH -sINITIAL_MEMORY=64MB -sSTANDALONE_WASM=1 --no-entry -sENVIRONMENT=web -std=c++17 -sEXPORTED_FUNCTIONS=_open,_init")
else()
    target_sources(main PUBLIC src/platform/debug.cc)
endif()

## 7zip (LZMA)

find_package(7zip CONFIG REQUIRED)
target_link_libraries(main PRIVATE 7zip::7zip)

## LZ4

find_package(lz4 CONFIG REQUIRED)
target_link_libraries(main PRIVATE lz4::lz4)