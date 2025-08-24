//
// Created by Raymond on 24/08/2025.
//

#include <library/reader.hh>
#include <library/unity/bundle.hh>

AssetBundle::AssetBundle(char* buffer) : buffer(buffer) {
    auto reader = new BinaryReader(buffer);
    if (reader->readNullTermedString() != "UnityFS")
        return;
    reader->setPosition(12);
    for (size_t idx = 0; 2 > idx; idx++)
        reader->readNullTermedString();

    reader->addPosition(8);
    const auto compressedBlockInfoSize = reader->read<uint32_t>(true);
    const auto decompressedBlockInfoSize = reader->read<uint32_t>(true);
    const auto flags = reader->read<uint32_t>(true);

    auto blockInfo = static_cast<char*>(malloc(decompressedBlockInfoSize));
    if (!decompress(
        static_cast<AssetBundleCompression>(flags & 0x3F),
        reader->getOffset(0),
        compressedBlockInfoSize,
        blockInfo,
        decompressedBlockInfoSize
    ))
        return;
    reader->addPosition(compressedBlockInfoSize);

    const auto blockReader = new BinaryReader(blockInfo);
    blockReader->setPosition(16);
    const auto blockCount = blockReader->read<uint32_t>(true);

    std::vector<
        std::vector<char>
    > blocks;
    for (size_t idx = 0; idx < blockCount; idx++) {
        const auto uncompressedBlockSize = blockReader->read<int32_t>(true);
        const auto compressedBlockSize = blockReader->read<int32_t>(true);
        const auto flag = blockReader->read<int16_t>(true);

        std::vector<char> uncompressedBlockBuffer;
        uncompressedBlockBuffer.resize(uncompressedBlockSize);
        if (!decompress(
            static_cast<AssetBundleCompression>(flag & 0x3F),
            reader->getOffset(0), compressedBlockSize,
            uncompressedBlockBuffer.data(), uncompressedBlockSize
        )) return;

        blocks.push_back(uncompressedBlockBuffer);
        reader->addPosition(compressedBlockSize);
    };

    std::vector<char> decompressedData;
    for (auto block : blocks)
        decompressedData.insert(decompressedData.end(), block.begin(), block.end());
    blocks.clear();

    // Get entries

    const auto entryCount = blockReader->read<int32_t>(true);
    for (size_t idx = 0; entryCount > idx; idx++) {
        const auto entryOffset = blockReader->read<uint64_t>(true);
        const auto entrySize = blockReader->read<uint64_t>(true);
        const auto entryIdentifier = blockReader->read<int32_t>(true);
        auto entryName = blockReader->readNullTermedString();

        auto entryData = static_cast<char*>(malloc(entrySize));
        memcpy(entryData, decompressedData.data() + entryOffset, entrySize);

        switch (entryIdentifier) {
            case 0:
                this->decompressedBuffer = entryData;
                break;
            case 4:
                this->archive = new UnityArchive(entryData);
                break;
            default: break;
        }
    };

    if (this->archive != nullptr)
        this->archive->cleanupOnDelete = true;

    // Clean up

    free(blockInfo);
    delete blockReader;
}

AssetBundle::~AssetBundle() {
    delete this->archive;
    if (this->decompressedBuffer != nullptr)
        free(this->decompressedBuffer);
}

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