//
// Created by Raymond on 24/08/2025.
//

#include <library/reader.hh>
#include <library/unity/class/texture2d.hh>

UnityTexture2D::UnityTexture2D(char *ptr): UnityNamedObject(ptr) {
    auto reader = new BinaryReader(ptr);
    reader->addPosition(8);

    this->texture.width = reader->read<int32_t>();
    this->texture.height = reader->read<int32_t>();

    reader->addPosition(4);

    switch (reader->read<int32_t>()) {
        case 3:
            this->texture.format = RGB; break;
        case 4:
            this->texture.format = RGBA; break;
        case 8:
            this->texture.format = BGR; break;
        case 9: // BC1
            this->texture.format = DXT1; break;
        case 10: // BC2
            this->texture.format = DXT3; break;
        case 11: // BC3
            this->texture.format = DXT5; break;
        case 13:
            this->texture.format = BGRA; break;
        case 25: // BC7
            this->texture.format = BC7; break;
        default: break;
    }

    reader->addPosition(56);

    this->texture.offset = reader->read<uint32_t>();
    this->texture.size = reader->read<uint32_t>();

    delete reader;
}
