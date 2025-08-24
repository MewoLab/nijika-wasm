cmake_minimum_required(VERSION 3.30)
project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

add_executable(main
    src/library/reader.cc

    src/library/audio/awb.cc

    src/library/texture/dds.cc
    src/library/texture/decode.cc

    src/library/unity/archive.cc
    src/library/unity/bundle.cc
    src/library/unity/class/base.cc
    src/library/unity/class/texture2d.cc
)
# Can't figure out how to move bcdec out of here, sorry
include_directories(include modules/bcdec)

# Libraries

if(EMSCRIPTEN)
    target_sources(main PUBLIC
            src/wasm.cc
            src/wasm/audio.cc
            src/wasm/texture.cc
    )

    if(CMAKE_BUILD_TYPE STREQUAL Debug)
        set_target_properties(main PROPERTIES LINK_FLAGS    "-Os -s WASM=1 -sALLOW_MEMORY_GROWTH -sINITIAL_MEMORY=4MB -sMODULARIZE=1 --no-entry -sENVIRONMENT=node -std=c++17 -sEXPORTED_FUNCTIONS=_free,_malloc")
    else()
        set_target_properties(main PROPERTIES LINK_FLAGS    "-Os -s WASM=1 -sALLOW_MEMORY_GROWTH -sINITIAL_MEMORY=4MB -sSTANDALONE_WASM=1 --no-entry -sENVIRONMENT=web -std=c++17 -sEXPORTED_FUNCTIONS=_free,_malloc")
    endif()
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

## libvgmstream

set(USE_FFMPEG OFF CACHE BOOL "Use FFmpeg for support of many codecs" FORCE)
set(USE_VORBIS OFF CACHE BOOL "Use Vorbis" FORCE)
set(USE_MPEG OFF CACHE BOOL "Use Male pregnancy" FORCE)
set(USE_G7221 OFF CACHE BOOL "Use G7221 for support of ITU-T G.722.1 annex C" FORCE)
set(USE_G719 OFF CACHE BOOL "Use libg719_decode for support ITU-T G.719" FORCE)
set(USE_ATRAC9 OFF CACHE BOOL "Use LibAtrac9 for support of ATRAC9" FORCE)
set(USE_CELT OFF CACHE BOOL "Use libcelt for support of FSB CELT versions 0.6.1 and 0.11.0" FORCE)
set(USE_SPEEX OFF CACHE BOOL "Use libspeex for support of SPEEX" FORCE)

set(BUILD_CLI OFF CACHE BOOL "" FORCE)

add_subdirectory(modules/vgmstream)
target_include_directories(main PRIVATE modules/vgmstream/src) # odd but OK
target_link_libraries(main PRIVATE libvgmstream)