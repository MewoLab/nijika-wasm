//
// Created by Raymond on 19/08/2025.
//

#include <headers.hh>
#include <wasm.hh>

#include <library/texture/bundle.hh>
#include <library/texture/texture.hh>

void decode(Texture texture) {
    auto data = texture.decodePixels();

    std::vector<uint8_t> textureBuffer;
    fpng::fpng_encode_image_to_memory(data, texture.width, texture.height, 4, textureBuffer);

    fifoSetOutputBuffer(textureBuffer.data(), textureBuffer.size());
    textureBuffer.clear();

    free(data);
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    bool getDDS(void* buffer) {;
        auto dds = DDS(static_cast<char*>(buffer));
        if (!dds.load())
            return false;

        decode(dds.textureInfo());
        return true;
    }
    EMSCRIPTEN_KEEPALIVE
    bool getAssetBundleDirect(void* buffer) {;
        auto ab = AssetBundle(static_cast<char*>(buffer));
        if (!ab.load())
            return false;

        decode(ab.textureInfo());
        return true;
    }
    // TODO: full loading of AB?
}