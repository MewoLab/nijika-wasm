//
// Created by Raymond on 24/08/2025.
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
	uint32_t offset;
	char* data = nullptr;

	char* decodePixels(bool) const;
};

#endif //TEXTURE_HH
