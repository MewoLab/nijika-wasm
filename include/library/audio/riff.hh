//
// Created by Raymond on 19/08/2025.
//

#ifndef RIFF_HH
#define RIFF_HH

#include <headers.hh>

// reference: http://www.topherlee.com/software/pcm-tut-wavformat.html ♥
struct RiffHeader {
    char magic[4] = { 'R', 'I', 'F', 'F' };

    uint32_t size = 0;

    char format[4] = { 'W', 'A', 'V', 'E' };
    char chunkHeader[4] = { 'f', 'm', 't', ' ' };

    uint32_t headerSize = 16;
    uint16_t formatType = 1;

    uint16_t channels = 2;
    uint32_t sampleRate = 0;
    uint32_t byteRate = 0;
    uint16_t blockAlign = 2;
    uint16_t bitsPerSample = 16;

    char dataChunkHeader[4] = { 'd', 'a', 't', 'a' };

    uint32_t dataChunkSize = 0;
};

#endif //RIFF_HH
