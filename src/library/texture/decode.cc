//
// Created by Raymond on 24/08/2025.
//

#include <headers.hh>
#include <library/texture/texture.hh>

#define BCDEC_IMPLEMENTATION
#include <bcdec.h>

char* Texture::decodePixels(bool isAUnityTexture) const {
    auto pixels = static_cast<char*>(
        malloc(width * height * 4)
    );
    auto src = this->data;
    auto dst = pixels;

    auto depth = format > DXT5 ? 1 : 4; // Only compressed formats

    for (uint32_t y = 0; height > y; y += depth)
        for (uint32_t x = 0; width > x; x += depth) {
            dst = pixels + (y * width + x) * 4;
            switch (this->format) {
                case DXT1:
                    bcdec_bc1(src, dst, width * 4); break;
                case DXT3:
                    bcdec_bc2(src, dst, width * 4); break;
                case DXT5:
                    bcdec_bc3(src, dst, width * 4); break;
                case BC7:
                    bcdec_bc7(src, dst, width * 4); break;
                case RGB:
                case RGBA:
                case BGR:
                case BGRA: {
                    const auto swap = this->format == BGRA || this->format == BGR;
                    dst[0] = src[swap ? 2 : 0];
                    dst[1] = src[1];
                    dst[2] = src[swap ? 0 : 2];

                    dst[3] = this->format == BGRA ||
                        this->format == RGBA ? src[3] : '\255';
                }
                default: break;
            };
            src += this->format != DXT1 ? (depth * 4) : 8;
        }

    if (isAUnityTexture) {
        const auto ref = static_cast<char*>(malloc(width * height * 4));
        memcpy(ref, pixels, width * height * 4);
        for (uint32_t y = 0; y < height; ++y)
            for (uint32_t x = 0; x < width; ++x)
                memcpy(
                    pixels + (y * width + x) * 4,
                    ref + ((height - 1 - y) * width + x) * 4,
                    4
                );
        free(ref);
    }

    return pixels;
}
