//
// Created by Raymond on 24/08/2025.
//

#include <library/texture/dds.hh>
#include <library/reader.hh>

#define DDS_DXT1_CODE 827611204
#define DDS_DXT3_CODE 861165636
#define DDS_DXT5_CODE 894720068

Texture getDDSTexture(char* buffer) {
    Texture texture{};
    auto reader = new BinaryReader(buffer);

    reader->setPosition(12);

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

    delete reader;
    return texture;
};