//
// Created by Raymond on 6/4/25.
//

#include <headers.hh>

#ifndef READER_HH
#define READER_HH

class BinaryReader {
public:
    explicit BinaryReader(char* buffer);

    template <typename T>
    T read(bool reverseOrder) {
        T value = (reinterpret_cast<T*>(this->internalBuffer + this->position))[0];
        this->position += sizeof(T);
        if (reverseOrder) {
            auto p = reinterpret_cast<uint8_t*>(&value);
            T result = 0;
            for (int i = 0; i < sizeof(T); i++)
                result |= static_cast<T>(p[(sizeof(T) - 1) - i]) << (8 * i);
            return result;
        } else
            return value;
    }
    template <typename T>
    T read() {
        return read<T>(false);
    }

    std::string readNullTermedString();
    std::string readNullTermedString(bool);

    char* getOffset(size_t target);
    
    void setPosition(size_t target);
    void addPosition(size_t target);

    void align(size_t alignment);
    std::string readAlignedString(size_t alignment);
private:
    size_t position = 0;
    char* internalBuffer = nullptr;
};

#endif //READER_HH
