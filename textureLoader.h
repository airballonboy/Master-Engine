#pragma once
#include <raylib.h>
#include <cstddef>
#include <vector>

namespace ME {
class textureLoader{
public:
	struct textureCTX{
		const char* texturePath;
		Vector2 	textureSize  = {128,128};
		Vector2 	texturePoint = {  0,0  };
	};
	inline static std::vector<Texture2D>  loadedTextures;
	inline static std::vector<textureCTX> textureCTXs;
	
	static size_t isTextureLoaded(textureCTX);
	static void   reloadTextures();
	static void   unloadTexture(textureCTX);
	static void   unloadTextures();
	static int    GetTextureSizeInBytes(Texture2D texture);
};
}

