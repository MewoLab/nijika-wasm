//
// Created by Raymond on 6/5/25.
//

#ifndef TEXTURE_HH
#define TEXTURE_HH

#include <headers.hh>

enum TextureFormat: char {
	BC7 = 1,
  	DXT1 = 2,
  	DXT3 = 3,
  	DXT5 = 4,

	RGBA = 5,
	RGB = 6,
	BGRA = 7,
	BGR = 8
};

struct Texture {
    uint32_t width;
    uint32_t height;
    TextureFormat format;

    uint32_t size;
    char* data;

	char* decodePixels() const;
};

#endif //TEXTURE_HH
