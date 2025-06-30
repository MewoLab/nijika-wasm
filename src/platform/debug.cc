//
// Created by Raymond on 6/4/25.
//
// Arguments:
//      [filename] ["ab" / "dds"]
//

#include <headers.hh>
#include <library/bundle.hh>

#include <chrono>

int main(int argv, char** argc) {
    if (argv != 3) {
        printf("Usage: [filename] [\"ab\" / \"dds\"]\n");
        return 1;
    }

    std::ifstream file(argc[1], std::ios_base::in | std::ios_base::binary);

    if (!file.is_open())
      return 1;
    file.seekg(0, std::ios::end);

    long size = file.tellg();
    char* buffer = static_cast<char*>(malloc(size));

    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    auto time = std::chrono::high_resolution_clock::now();

    auto isAssetBundle = strcmp(argc[2], "ab") == 0;
    Texture info{};
    if (isAssetBundle) {
        auto assetbundle = new AssetBundle(buffer);
        printf("Loaded %s %s\n", argc[1], (assetbundle->load() ? "successfully" : "failed"));
        info = assetbundle->textureInfo();
    } else {
        auto dds = new DDS(buffer);
        printf("Loaded %s %s\n", argc[1], (dds->load() ? "successfully" : "failed"));
        info = dds->textureInfo();
    }

    printf("Width: %i \nHeight: %i\nFormat: %i\n", info.width, info.height, info.format);
    auto pixelData = info.decodePixels();
    printf("Took %lld microseconds to process and decode.\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - time).count());
#ifndef NDEBUG
    printf("NOTE: Built as debug, a release build will be faster\n");
#endif

    fpng::fpng_encode_image_to_file("out.png", pixelData, info.width, info.height, 4, 0);

    free(pixelData);
    free(buffer);

    // too lazy to clean up the dds / ab class, it's a debug program so it doesn't REALLY matter i think

    return 0;
}
