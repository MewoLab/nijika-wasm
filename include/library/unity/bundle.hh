//
// Created by Raymond on 24/08/2025.
//

#ifndef ASSETBUNDLE_HH
#define ASSETBUNDLE_HH

#include <library/unity/archive.hh>

enum AssetBundleCompression: int {
    NoCompression,
    LZMA,
    LZ4,
    LZ4HC
};

struct AssetBundle {
    explicit AssetBundle(char* buffer);
    ~AssetBundle();

    UnityArchive* archive = nullptr;
    void* decompressedBuffer = nullptr;
private:
    void* buffer = nullptr;
    static bool decompress(AssetBundleCompression, char*, size_t, char*, size_t);
};

#endif //ASSETBUNDLE_HH
