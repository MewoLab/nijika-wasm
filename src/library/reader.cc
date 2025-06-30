//
// Created by Raymond on 6/4/25.
//

#include <library/bundle.hh>

BinaryReader::BinaryReader(char* buffer): internalBuffer(buffer) {};

std::string BinaryReader::readAlignedString(size_t alignment) {
    auto stringSize = read<int32_t>();
    std::string string(this->internalBuffer + this->position, static_cast<size_t>(stringSize));

    addPosition(stringSize);
    align(alignment);

    return string;
};
std::string BinaryReader::readNullTermedString(bool iterate) {
    std::string string(this->internalBuffer + this->position);
    if (iterate)
        this->position += string.size() + 1;
    return string;
}
std::string BinaryReader::readNullTermedString() {
    return readNullTermedString(true);
}
char *BinaryReader::getOffset(size_t target) {
    return this->internalBuffer + this->position + target;
}
void BinaryReader::setPosition(size_t target) {
    this->position = target;
};
void BinaryReader::addPosition(size_t target) {
    this->position += target;
};
void BinaryReader::align(size_t alignment)  {
    auto mod = this->position % alignment;
    if (mod != 0)
        this->position += (alignment - mod);
};