//
// Created by Raymond on 6/4/25.
//

#ifndef HEADERS_HH
#define HEADERS_HH
    #include <cstring>
    #include <cstdint>

    #include <string>
    #include <vector>
    #include <map>

    #include <lz4.h>
    #include <LzmaDec.h>
    #include <fpng.h>

    #ifdef __EMSCRIPTEN__
        extern "C" {
            #include <libvgmstream.h>
            #include <libvgmstream_streamfile.h>
        }
    #endif

    #ifndef NDEBUG
        #include <iostream>
        #include <fstream>
    #endif
#endif //HEADERS_HH
