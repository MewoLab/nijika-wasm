//
// Created by Raymond on 24/08/2025.
//

#ifndef BASE_HH
#define BASE_HH

#include <headers.hh>

struct UnityNamedObject {
    explicit UnityNamedObject(char* ptr);
private:
    char* ptr = nullptr;
};

#endif //BASE_HH
