//
// Created by Raymond on 24/08/2025.
//

#ifndef TEXTURE2D_HH
#define TEXTURE2D_HH

#include <headers.hh>
#include <library/unity/class/base.hh>
#include <library/texture/texture.hh>

struct UnityTexture2D : UnityNamedObject {
    explicit UnityTexture2D(char* ptr);

    Texture texture{};
};

#endif //TEXTURE2D_HH
