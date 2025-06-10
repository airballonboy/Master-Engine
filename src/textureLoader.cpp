#include "tools/logger.h"
#include "textureLoader.h"
#include "tools/format.h"
#include <cstring>
#include <raylib.h>
#include <cstddef>
#include <string>


namespace ME {
size_t textureLoader::isTextureLoaded(textureLoader::textureCTX ctx){
	size_t i = 0;
	for (auto& CTX : textureCTXs){
		if (strcmp(ctx.texturePath, CTX.texturePath) == 0)
			return i;
		i++;
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
	size_t clearedSize = sizeof(Texture2D) * loadedTextures.size();
	for (auto& t : loadedTextures) UnloadTexture(t);
	loadedTextures.clear();

	logger::success("textureLoader: ", (std::to_string(clearedCount) + " textures were cleared").c_str());
	if (clearedSize < 1000)
		logger::success("textureLoader: ", f("{} Bytes were cleared", clearedSize).c_str());
	else if (clearedSize < 1000000)
		logger::success("textureLoader: ", f("{} KiloBytes were cleared", clearedSize/1000.f).c_str());
	else if (clearedSize < 1000000000)
		logger::success("textureLoader: ", f("{} MegaBytes were cleared", clearedSize/1000000.f).c_str());

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
void textureLoader::unloadTexture(textureLoader::textureCTX ctx){
	auto i = isTextureLoaded(ctx);
	UnloadTexture(loadedTextures[i]);
	loadedTextures.erase(loadedTextures.begin() + i);
	textureCTXs.erase(textureCTXs.begin() + i);
	#ifndef PRINTFULLPATH
		std::string shortPath = std::string(ctx.texturePath).substr(std::string(ctx.texturePath).find("resources"));
		logger::success("textureLoader: ", (shortPath + " was unloaded").c_str());
	#else
		logger::success("textureLoader: ", (std::string(ctx.texturePath) + " was unloaded").c_str());
	#endif
}
void textureLoader::unloadTextures(){
	size_t clearedCount = loadedTextures.size();
	size_t clearedSize = 0;
	for (auto& t : loadedTextures)
		clearedSize += GetTextureSizeInBytes(t);
	//for (size_t i = 0; i < textureCTXs.size(); i++){
	//	unloadTexture(textureCTXs[i--]);
	//}	
	while (textureCTXs.size() > 0){
		unloadTexture(textureCTXs[0]);
	}
	if (clearedSize < 1000)
		logger::success("textureLoader: ", f("successfully freed {} Bytes", clearedSize).c_str());
	else if (clearedSize < 1000000)
		logger::success("textureLoader: ", f("successfully freed {} KiloBytes", clearedSize/1000.f).c_str());
	else if (clearedSize < 1000000000)
		logger::success("textureLoader: ", f("successfully freed {} MegaBytes", clearedSize/1000000.f).c_str());
}
int textureLoader::GetTextureSizeInBytes(Texture2D texture) {
    // Assuming the texture format is uncompressed RGBA (4 bytes per pixel)
    int bytesPerPixel = 0;

    switch (texture.format) {
        case PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: bytesPerPixel = 1; break;
        case PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: bytesPerPixel = 2; break;
        case PIXELFORMAT_UNCOMPRESSED_R5G6B5: bytesPerPixel = 2; break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8: bytesPerPixel = 3; break;
        case PIXELFORMAT_UNCOMPRESSED_R5G5B5A1: bytesPerPixel = 2; break;
        case PIXELFORMAT_UNCOMPRESSED_R4G4B4A4: bytesPerPixel = 2; break;
        case PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: bytesPerPixel = 4; break;
        case PIXELFORMAT_UNCOMPRESSED_R32: bytesPerPixel = 4; break;
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32: bytesPerPixel = 12; break;
        case PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: bytesPerPixel = 16; break;
        default:
            std::cerr << "Unknown or compressed format: " << texture.format << std::endl;
            return 0;
    }

    return texture.width * texture.height * bytesPerPixel;
}
}



