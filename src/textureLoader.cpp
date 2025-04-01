#include "logger/logger.h"
#include "textureLoader.h"
#include <raylib.h>
#include <cstddef>
#include <string>


namespace ME {
size_t textureLoader::isTextureLoaded(textureLoader::textureCTX ctx){
	size_t i = 0;
	for (auto& CTX : textureCTXs){
		if (ctx.texturePath == CTX.texturePath) return i;
		else i++;
	}
	loadedTextures.push_back(LoadTexture(ctx.texturePath));
	
	if (loadedTextures[i].id == 0)
		logger::error("textureLoader: ", "this file couldn't be opened");
	else {
		SetTextureFilter(loadedTextures[i], TEXTURE_FILTER_POINT);

		#ifndef PRINTFULLPATH
			std::string shortPath = std::string(ctx.texturePath).substr(std::string(ctx.texturePath).find("resources"));
			logger::success("textureLoader: ", (shortPath + " was loaded").c_str());
		#else
			logger::success("textureLoader: ", (std::string(ctx.texturePath) + " was loaded").c_str());
		#endif

	}
	textureCTXs.push_back(ctx);
	return i;	
}
void textureLoader::reloadTextures(){
	size_t clearedCount = loadedTextures.size();
	size_t clearedSize = sizeof(loadedTextures);
	for (auto& t : loadedTextures) UnloadTexture(t);
	loadedTextures.clear();
	logger::success("textureLoader: ", (std::to_string(clearedCount) + " textures were cleared").c_str());
	logger::success("textureLoader: ", (std::to_string(clearedSize)  + " bytes were cleared"   ).c_str());
	size_t i = 0;
	for (auto& ctx : textureCTXs){
		loadedTextures.push_back(LoadTexture(ctx.texturePath));
		SetTextureFilter(loadedTextures[i++], TEXTURE_FILTER_POINT);
		#ifndef PRINTFULLPATH
			std::string shortPath = std::string(ctx.texturePath).substr(std::string(ctx.texturePath).find("resources"));
			logger::success("textureLoader: ", (shortPath + " was reloaded").c_str());
		#else
			logger::success("textureLoader: ", (std::string(ctx.texturePath) + " was reloaded").c_str());
		#endif
	}
}

}



