//
// Created by Raymond on 19/08/2025.
//

#include <headers.hh>
#include <wasm/bindings.hh>

#include <library/texture/texture.hh>
#include <library/texture/dds.hh>

#include <library/unity/bundle.hh>
#include <library/unity/class/texture2d.hh>

void decodeTexture(Texture texture, bool isAUnityTexture) {
    auto data = texture.decodePixels(isAUnityTexture);

    std::vector<uint8_t> textureBuffer;
    fpng::fpng_encode_image_to_memory(data, texture.width, texture.height, 4, textureBuffer);

    fifoSetOutputBuffer(textureBuffer.data(), textureBuffer.size());
    textureBuffer.clear();

    free(data);
}

UnityArchive* unityArchive = nullptr;

#define TEXTURE_2D 28

extern "C" {
    EMSCRIPTEN_KEEPALIVE void loadUnityArchive(void* assetsBuffer) {
        if (unityArchive != nullptr)
            delete(unityArchive);
        unityArchive = new UnityArchive(static_cast<char*>(assetsBuffer));
    }
    EMSCRIPTEN_KEEPALIVE int unityArchiveGetTextureList() {
        if (unityArchive == nullptr) return 0;
        if (unityArchive->assets[TEXTURE_2D].empty()) return 0;

        auto buffer = malloc(unityArchive->assets[TEXTURE_2D].size() * 256); size_t idx = 0;
        memset(buffer, '\0', unityArchive->assets[TEXTURE_2D].size() * 256);
        for (auto itr = unityArchive->assets[TEXTURE_2D].begin(); itr != unityArchive->assets[TEXTURE_2D].end(); itr++) {
            memcpy(static_cast<char*>(buffer) + (idx * 256), itr->first.c_str(), std::min(itr->first.size(), static_cast<size_t>(256))); idx++;
        }
        fifoSetOutputBuffer(buffer, unityArchive->assets[TEXTURE_2D].size() * 256);
        free(buffer);

        return unityArchive->assets[TEXTURE_2D].size();
    }
    EMSCRIPTEN_KEEPALIVE bool unityArchiveGetTextureReference(char* name) {
        if (unityArchive == nullptr) return false;
        if (unityArchive->assets[TEXTURE_2D].empty()) return false;
        if (unityArchive->assets[TEXTURE_2D].find(name) == unityArchive->assets[TEXTURE_2D].end()) return false;

        auto pointer = unityArchive->assets[TEXTURE_2D].at(std::string(name));
        auto texture2D = reinterpret_cast<UnityTexture2D*>(getUnityObject(pointer, TEXTURE_2D));

        uint32_t buffer[2] = {
            texture2D->texture.offset,
            texture2D->texture.size
        };
        fifoSetOutputBuffer(buffer, 8);

        delete texture2D;
        return true;
    }
    EMSCRIPTEN_KEEPALIVE bool unityArchiveGetTexture(char* name, void* textureBuffer) {
        if (unityArchive == nullptr) return false;
        if (unityArchive->assets[TEXTURE_2D].empty()) return false;
        if (unityArchive->assets[TEXTURE_2D].find(name) == unityArchive->assets[TEXTURE_2D].end()) return false;

        auto pointer = unityArchive->assets[TEXTURE_2D].at(std::string(name));
        auto texture2D = reinterpret_cast<UnityTexture2D*>(getUnityObject(pointer, TEXTURE_2D));

        texture2D->texture.data = static_cast<char*>(textureBuffer);
        decodeTexture(texture2D->texture, true);

        delete texture2D;
        return true;
    }
    EMSCRIPTEN_KEEPALIVE bool getAssetBundleDirect(void* assetBundleBuffer) {
        auto assetBundle = new AssetBundle(static_cast<char*>(assetBundleBuffer));
        if (assetBundle->archive == nullptr) return false;
        if (assetBundle->archive->assets[TEXTURE_2D].empty()) return false;
        auto pointer = assetBundle->archive->assets[TEXTURE_2D].begin()->second;

        auto texture2D = reinterpret_cast<UnityTexture2D*>(getUnityObject(pointer, TEXTURE_2D));

        texture2D->texture.data = static_cast<char*>(assetBundle->decompressedBuffer) + texture2D->texture.offset;
        decodeTexture(texture2D->texture, true);

        delete texture2D;
        delete assetBundle;

        return true;
    }
    EMSCRIPTEN_KEEPALIVE bool getDDS(void* ddsBuffer) {
        auto texture = getDDSTexture(static_cast<char*>(ddsBuffer));
        decodeTexture(texture, false);
        return true;
    }
}