//
// Created by Raymond on 6/5/25.
//

#include <headers.hh>
#include <wasm.hh>

void* outputBuffer = nullptr;
size_t outputBufferSize = 0;

void fifoSetOutputBuffer(const void* buffer, const size_t size) {
    if (outputBuffer != nullptr)
        free(outputBuffer);

    outputBufferSize = size;
    outputBuffer = malloc(size);

    memcpy(outputBuffer, buffer, size);
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE size_t fifoOutputSize() { return outputBufferSize; }
    EMSCRIPTEN_KEEPALIVE void* fifoOutput() { return outputBuffer; }
}
