//
// Created by Raymond on 6/5/25.
//

#include <headers.hh>
#include <library/bundle.hh>

#include <emscripten.h>

// We have to allocate 32 megabytes for the image buffer
//		because we have some uncompressed textures

#define BUFFER_SIZE 33554432
void* buffer = nullptr;

enum Game: char {
	Chu3,
	Mai,
	Ongeki // TODO, do not use
};

extern "C" {
	EMSCRIPTEN_KEEPALIVE
	void* init() {
		if (!(buffer != nullptr))
        	buffer = malloc(BUFFER_SIZE);
        return buffer;
    }
	EMSCRIPTEN_KEEPALIVE
	int open(Game game) {
		Texture texture{};

		switch (game) {
			case Mai:
			case Ongeki: {
				auto assetBundle = AssetBundle(static_cast<char*>(buffer));
				if (!assetBundle.load())
					return 0;
				texture = assetBundle.textureInfo();
				break;
			}
			case Chu3: {
				texture = DDS(static_cast<char*>(buffer)).textureInfo();
				break;
			}
		}

		if (texture.format == 0)
			return 0;
		auto data = texture.decodePixels();

		std::vector<uint8_t> textureBuffer;
		fpng::fpng_encode_image_to_memory(data, texture.width, texture.height, 4, textureBuffer);

		auto textureBufferSize = textureBuffer.size();
		memcpy(buffer, textureBuffer.data(), textureBuffer.size());
		textureBuffer.clear();

		free(data);

		return textureBufferSize;
	}
}

