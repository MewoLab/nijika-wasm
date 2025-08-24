//
// Created by Raymond on 24/08/2025.
//

#include <library/unity/archive.hh>
#include <library/reader.hh>

UnityArchive::UnityArchive(char *assetsBuffer) : assetsBuffer(assetsBuffer) {
    auto reader = new BinaryReader(this->assetsBuffer);
    reader->setPosition(8);

    this->version = reader->read<uint32_t>(true);
    auto dataOffset = reader->read<uint32_t>(true);

    // Currently we only care to support archives of Unity 5.?.? to Unity 2018.4
    if (this->version != 17)
        return;

    reader->addPosition(4);
    reader->readNullTermedString();
    reader->addPosition(4);

    auto enableTypeTree = reader->read<bool>();

    auto typeCount = reader->read<int32_t>();
    std::vector<int> types;

    for (size_t idx = 0; idx < typeCount; idx++) {
        auto id = reader->read<int32_t>();
        types.push_back(id);
        reader->addPosition(19 + (id == 114 ? 16 : 0));

        if (enableTypeTree) {
            auto nodeCount = reader->read<int32_t>();
            auto stringSize = reader->read<int32_t>();
            reader->addPosition(stringSize + (24 * nodeCount));
        }
    }

    auto objectCount = reader->read<int32_t>();
    for (size_t idx = 0; idx < objectCount; idx++) {
        reader->align(4);

        reader->addPosition(8);
        auto byteStart = reader->read<uint32_t>() + dataOffset;
        reader->addPosition(4);
        auto typeIndex = reader->read<int32_t>();

        const auto currentPosition = reader->getPosition();
        reader->setPosition(byteStart);
        auto objectName = reader->readAlignedString(4);
        auto objectOffset = reader->getPosition();
        reader->setPosition(currentPosition);

        this->assets[types.at(typeIndex)][objectName] = assetsBuffer + objectOffset;
    }
}

UnityArchive::~UnityArchive() {
    if (cleanupOnDelete)
        free(this->assetsBuffer);
}

#include <library/unity/class/texture2d.hh>

UnityNamedObject* getUnityObject(char* ptr, uint32_t id) {
    switch (id) {
        case 28:
            return new UnityTexture2D(ptr);
        default: break;
    }
    return nullptr;
}