//
// Created by Raymond on 6/4/25.
//

#ifndef ASSETBUNDLE_HH
#define ASSETBUNDLE_HH

#include <headers.hh>
#include <library/reader.hh>
#include <library/texture.hh>

enum AssetBundleCompression: int {
	NoCompression,
  	LZMA,
  	LZ4,
  	LZ4HC
};

class DDS {
public:
	DDS(char* buffer);
	~DDS();

	bool load();
	Texture textureInfo();
private:
	BinaryReader* reader = nullptr;
};

class AssetBundle {
public:
    explicit AssetBundle(char* buffer);
	~AssetBundle();

    bool load();
	Texture textureInfo();

    std::vector<char> asset;
private:
  	static bool decompress(AssetBundleCompression type, char* input, size_t inputSize, char* output, size_t outputSize);

    BinaryReader* reader = nullptr;
    std::vector<char> metadata;
};

#endif //ASSETBUNDLE_HH
