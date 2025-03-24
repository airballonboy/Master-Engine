#include "textureLoader.h"
#include <raylib.h>
#include <cstddef>
#include <iostream>
#include <string>
#include "settings.h"


namespace ME {
size_t textureLoader::isTextureLoaded(textureLoader::textureCTX ctx){
	size_t i = 0;
	for (auto& CTX : textureCTXs){
		if (ctx.texturePath == CTX.texturePath) return i;
		else i++;
	}
	loadedTextures.push_back(LoadTexture(ctx.texturePath));
	
	if (loadedTextures[i].id == 0)
		std::cerr << "textureLoader: " << "error this file couldn't be opened\n";
	else {
		SetTextureFilter(loadedTextures[i], TEXTURE_FILTER_POINT);

		#ifndef PRINTFULLPATH
			std::cout << "textureLoader: " 
				<< std::string(ctx.texturePath).erase(0, std::string(RESOURCES_PATH).size()) 
				<< " was loaded\n";
		#else 
			std::cout << "textureLoader: " << ctx.texturePath << " was loaded" << std::endl;
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
	std::cout << "textureLoader: " << clearedCount << " textures were cleared" << std::endl;
	std::cout << "textureLoader: " << clearedSize << " bytes were cleared" << std::endl;
	size_t i = 0;
	for (auto& ctx : textureCTXs){
		loadedTextures.push_back(LoadTexture(ctx.texturePath));
		SetTextureFilter(loadedTextures[i++], TEXTURE_FILTER_POINT);
		std::cout << "textureLoader: " << ctx.texturePath << " was reloaded" << std::endl;
	}
}

}



