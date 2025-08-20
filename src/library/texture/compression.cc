//
// Created by Raymond on 6/4/25.
//

#include <library/texture/bundle.hh>

static void *SzAlloc(ISzAllocPtr, size_t size) { return malloc(size); }
static void SzFree(ISzAllocPtr, void *address) { free(address); }
static ISzAlloc g_Alloc = { SzAlloc, SzFree };

bool AssetBundle::decompress(AssetBundleCompression type, char* input, size_t inputSize, char* output, size_t outputSize) {
    switch (type) {
        case NoCompression: {
            memcpy(output, input, inputSize);
            break;
        }
        case LZMA: {
            CLzmaDec state;
            LzmaDec_Construct(&state);

            SRes alloc_res = LzmaDec_Allocate(&state, reinterpret_cast<Byte*>(input), LZMA_PROPS_SIZE, &g_Alloc);
            if (alloc_res != SZ_OK)
                return false;

            LzmaDec_Init(&state);

            SizeT dest_len = outputSize;
            SizeT src_len = inputSize - 5;

            ELzmaStatus status;

            SRes response = LzmaDec_DecodeToBuf(
                &state,
                reinterpret_cast<Byte*>(output),
                &dest_len,
                reinterpret_cast<const Byte *>(input + 5),
                &src_len,
                LZMA_FINISH_END,
                &status
            );

            LzmaDec_Free(&state, &g_Alloc);
            if (response != SZ_OK)
                return false;
            break;
        }
        case LZ4:
        case LZ4HC: {
            if (LZ4_decompress_safe(input, output, inputSize, outputSize) < 0)
                return false;
            break;
        }
    }
    return true;
}