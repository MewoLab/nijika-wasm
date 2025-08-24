//
// Created by Raymond on 24/08/2025.
//

#ifndef ASSET_HH
#define ASSET_HH

#include <headers.hh>
#include <library/unity/class/base.hh>

UnityNamedObject* getUnityObject(char* ptr, uint32_t id);

struct UnityArchive {
    explicit UnityArchive(char* assetsBuffer);
    ~UnityArchive();

    std::map<uint32_t,
        std::map<std::string, char*>
    > assets{};
    bool cleanupOnDelete = false;
private:
    char* assetsBuffer = nullptr;

    uint32_t version = 0;
};

#endif //ASSET_HH
