//
// Created by Raymond on 6/4/25.
//

#include <library/bundle.hh>

#define DDS_MAGIC_BYTES 0x20534444

DDS::DDS(char* buffer) {
    this->reader = new BinaryReader(buffer);
};
DDS::~DDS() {
    if (this->reader != nullptr)
        delete this->reader;
}
bool DDS::load() {
    if (reader != nullptr)
        return (reader->read<uint32_t>() == DDS_MAGIC_BYTES);
    return false;
}

AssetBundle::AssetBundle(char* buffer) {
    this->reader = new BinaryReader(buffer);
}
AssetBundle::~AssetBundle() {
    if (this->reader != nullptr)
        delete this->reader;
    this->metadata.clear();
    this->asset.clear();
}

bool AssetBundle::load() {
    if (strcmp(reader->getOffset(0), "UnityFS\0") != 0)
        return false;

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
        return false;
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
        )) return false;

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

        std::vector<char> entry;
        entry.resize(entrySize);
        memcpy(entry.data(), decompressedData.data() + entryOffset, entrySize);

        switch (entryIdentifier) {
            case 4:
                this->metadata = entry;
                break;
            case 0:
                this->asset = entry;
                break;
            default: break;
        }
    };

    // Clean up

    free(blockInfo);
    delete blockReader;

    return true;
}
