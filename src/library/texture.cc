//
// Created by Raymond on 6/4/25.
//

#define BCDEC_IMPLEMENTATION
#include <library/bundle.hh>
#include <bcdec.h>

#define TEXTURE2D 28

#define DDS_DXT1_CODE 827611204
#define DDS_DXT3_CODE 861165636
#define DDS_DXT5_CODE 894720068

Texture getTextureInfo(size_t position, char* buffer, char* assetBuffer) {
    auto reader = new BinaryReader(buffer);
    reader->setPosition(position);

    auto string = reader->readAlignedString(4);
    reader->addPosition(5);
    reader->align(4);

    Texture texture {};
    texture.width = reader->read<int32_t>();
    texture.height = reader->read<int32_t>();

    reader->addPosition(4);
    auto format = reader->read<int32_t>();

    switch (format) {
        case 9: // DXT1
            texture.format = DXT1; break;
        case 10: // DXT3
            texture.format = DXT3; break;
        case 11: // DXT5
            texture.format = DXT5; break;
        case 25: // BC7
            texture.format = BC7; break;
        default:
            // No handler, let's just tell it to screw off
            return {};
    }

    reader->addPosition(6);
    reader->align(4);
    reader->addPosition(48);

    auto offset = reader->read<uint32_t>();
    texture.size = reader->read<uint32_t>();
    texture.data = assetBuffer + offset;

    delete reader;
    return texture;
};

Texture DDS::textureInfo() {
    if (this->reader == nullptr)
        return {};

    reader->setPosition(12);

    Texture texture {};
    texture.height = reader->read<uint32_t>();
    texture.width = reader->read<uint32_t>();

    reader->setPosition(80);
    auto flags = reader->read<uint32_t>();
    auto fmt = reader->read<uint32_t>();
    switch (fmt) {
        case DDS_DXT1_CODE:
            texture.format = DXT1; break;
        case DDS_DXT3_CODE:
            texture.format = DXT3; break;
        case DDS_DXT5_CODE:
            texture.format = DXT5; break;
        default: {
            reader->read<uint32_t>();
            auto channelR = reader->read<uint32_t>();
            // This is making bold assumptions for the sake of simplicity.
            if (flags & 0x40)
                if (flags & 0x1) {
                    texture.format = channelR != 255 ? BGRA : RGBA;
                } else
                    texture.format = channelR != 255 ? BGR : RGB;
            break;
        }
    }
    if (!texture.format) return {};

    reader->setPosition(128);
    texture.data = reader->getOffset(0);
    return texture;
}

Texture AssetBundle::textureInfo() {
    if (this->metadata.empty())
        return {};

    auto metadataReader = new BinaryReader(this->metadata.data());
    metadataReader->setPosition(8);

    auto version = metadataReader->read<uint32_t>(true);
    auto dataOffset = metadataReader->read<uint32_t>(true);

    if (version != 17)
        return {};

    metadataReader->addPosition(4);
    metadataReader->readNullTermedString();
    metadataReader->addPosition(4);

    auto enableTypeTree = metadataReader->read<bool>();

    auto typeCount = metadataReader->read<int32_t>();
    std::vector<int> types;

    for (size_t idx = 0; idx < typeCount; idx++) {
        auto id = metadataReader->read<int32_t>();
        types.push_back(id);
        metadataReader->addPosition(19);

        if (enableTypeTree) {
            auto nodeCount = metadataReader->read<int32_t>();
            auto stringSize = metadataReader->read<int32_t>();
            metadataReader->addPosition(stringSize + (24 * nodeCount));
        }
    }

    auto objectCount = metadataReader->read<int32_t>();
    for (size_t idx = 0; idx < objectCount; idx++) {
        metadataReader->align(4);

        metadataReader->addPosition(8);
        auto byteStart = metadataReader->read<uint32_t>() + dataOffset;
        metadataReader->addPosition(4);
        auto typeIndex = metadataReader->read<int32_t>();

        if (types.at(typeIndex) == TEXTURE2D) {
            delete metadataReader;
            return getTextureInfo(byteStart, metadata.data(), asset.data());
        }
    }
    delete metadataReader;
    return {};
}

char* Texture::decodePixels() const {
    auto pixels = static_cast<char*>(
        malloc(width * height * 4)
    );
    auto src = this->data;
    auto dst = pixels;

    auto depth = this->format > DXT5 ? 1 : 4; // Only compressed formats

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

    // Flip image on the Y-axis
    if (format == BC7) {
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
