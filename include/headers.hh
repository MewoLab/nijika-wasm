//
// Created by Raymond on 6/4/25.
//

#ifndef HEADERS_HH
#define HEADERS_HH
    #include <cstring>
    #include <cstdint>

    #include <string>
    #include <vector>

    #include <lz4.h>
    #include <LzmaDec.h>
    #include <fpng.h>

    extern "C" {
        #include <libvgmstream.h>
        #include <libvgmstream_streamfile.h>
    }

    #ifndef NDEBUG
        #include <iostream>
    #endif
#endif //HEADERS_HH
