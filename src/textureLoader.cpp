#include "tools/logger.h"
#include "textureLoader.h"
#include "tools/format.h"
#include <cctype>
#include <cstdio>
#include <cstring>
#include <raylib.h>
#include <cstddef>
#include <string>
#include <fstream>

enum class Type{ NONE = 0, TEXTURE, SOUND, VIDEO, FONT };
typedef struct {
    Type type;
    std::string path;
}loadable;

#define EXPECT(c)                                                                                                               \
    do {                                                                                                                        \
        if(fileContent[i] != c) {                                                                                               \
            logger::error("textureLoader: ", f("error while parsing, expected token {} but got {}", c, fileContent[i]).c_str());\
        }else {i++;}                                                                                                            \
    }while(0)

#define SKIP_SPACES()                 \
    while (isspace(fileContent[i])){  \
        i++;                          \
    }

std::string get_printable_path(std::string path) {
    #ifndef PRINTFULLPATH
        std::string shortPath = path.substr(path.find("resources"));
    #else
        std::string shortPath = path;
    #endif

    return shortPath;
}

namespace ME {
size_t textureLoader::isTextureLoaded(textureLoader::textureCTX ctx){
    size_t i = 0;
    for (auto& CTX : textureCTXs){
        if (ctx.texturePath == CTX.texturePath) {
            return i;
        }
        i++;
    }
    auto t = LoadTexture(ctx.texturePath.c_str());
    
    if (t.id == 0)
        logger::error("textureLoader: ", "this file couldn't be opened");
    else {
        loadedTextures.push_back(t);
        SetTextureFilter(loadedTextures[i], TEXTURE_FILTER_POINT);
        textureCTXs.push_back(ctx);

        logger::success("textureLoader: ", (get_printable_path(ctx.texturePath) + " was loaded").c_str());

    }
    return i;   
}
std::vector<loadable> configParser(std::string inputPath) {
    std::vector<loadable> shouldLoad;
    std::string fileContent;
    std::stringstream contents_stream;
    std::fstream input(inputPath, std::ios::in);
    contents_stream << input.rdbuf();
    fileContent = contents_stream.str();
    input.close();
    contents_stream.clear();
    size_t i = 0;
    while (i < fileContent.size()){
        if(isspace(fileContent[i])){
            i++;
        }else if(isalpha(fileContent[i])){
            loadable l;
            std::string type;
            std::string path;
            type.push_back(fileContent[i++]);
            while (isalnum(fileContent[i])){
                type.push_back(fileContent[i++]);
            }
            if      (strcmp(type.c_str(), "texture") == 0) l.type = Type::TEXTURE;
            else if (strcmp(type.c_str(), "sound"  ) == 0) l.type = Type::SOUND;
            else if (strcmp(type.c_str(), "font"   ) == 0) l.type = Type::FONT;
            else if (strcmp(type.c_str(), "video"  ) == 0) l.type = Type::VIDEO;

            SKIP_SPACES();
            EXPECT('=');
            SKIP_SPACES();
            EXPECT('>');

            SKIP_SPACES();
            EXPECT('\"');
            while (fileContent[i] != '\"') {
                if (fileContent[i] == '{') {
                    if (i < fileContent.size()) i++;
                    else {
                        logger::error("textureLoader: ", "file ended early");
                        return {};
                    }
                    std::string var;
                    
                    while (fileContent[i] != '}') {
                        if (fileContent[i] == '"'){
                            logger::error("textureLoader: ", "no matching }");
                            break;
                        }
                        var.push_back(fileContent[i]);
                        if (i < fileContent.size()) i++;
                        else {
                            logger::error("textureLoader: ", "file ended early");
                            return {};
                        }
                    }
                    if (i < fileContent.size()) i++;
                    else {
                        logger::error("textureLoader: ", "file ended early");
                        return {};
                    }
                    if (!strcmp(var.c_str(), "RESOURCES_PATH")) path.append(RESOURCES_PATH);
                    //std::cout << "resources path detected \n";
                }
                path.push_back(fileContent[i]);
                if (i < fileContent.size()) i++;
                else {
                    logger::error("textureLoader: ", "file ended early");
                    return {};
                }
            }
            l.path = path;
            while (fileContent[i] != '\n') {
                if (i < fileContent.size()) i++;
            }
            shouldLoad.push_back(l);
        }
    }
    return shouldLoad;
}
void textureLoader::loadTexturesFromConf(std::string inputPath){
    auto shouldLoad = configParser(inputPath);
    for (auto& l : shouldLoad){
        switch (l.type) {
        case Type::TEXTURE: {
            isTextureLoaded({l.path.c_str()});
            logger::success(f("                 └───► from the file {}", get_printable_path(inputPath)).c_str());
        }break;
        default:
            logger::error("textureLoader: ", f("type not yet implemented").c_str());
        }
    }

}
void textureLoader::unloadTexturesFromConf(std::string inputPath){
    auto shouldLoad = configParser(inputPath);
    for (auto& l : shouldLoad){
        switch (l.type) {
        case Type::TEXTURE: {
            unloadTexture({l.path.c_str()});
            logger::success(f("                 └───► from the file {}", get_printable_path(inputPath)).c_str());
        }break;
        default: {
            logger::error("textureLoader: ", f("type not yet implemented").c_str());
        }
        }
    }
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
        loadedTextures.push_back(LoadTexture(ctx.texturePath.c_str()));
        SetTextureFilter(loadedTextures[i++], TEXTURE_FILTER_POINT);
        logger::success("textureLoader: ", (get_printable_path(ctx.texturePath) + " was reloaded").c_str());
    }
}
void textureLoader::unloadTexture(textureLoader::textureCTX ctx){
    auto i = isTextureLoaded(ctx);
    UnloadTexture(loadedTextures[i]);
    loadedTextures.erase(loadedTextures.begin() + i);
    textureCTXs.erase(textureCTXs.begin() + i);
    logger::success("textureLoader: ", (get_printable_path(ctx.texturePath) + " was unloaded").c_str());
}
void textureLoader::unloadTextures(){
    size_t clearedCount = loadedTextures.size();
    size_t clearedSize = 0;
    for (auto& t : loadedTextures)
        clearedSize += GetTextureSizeInBytes(t);
    //for (size_t i = 0; i < textureCTXs.size(); i++){
    //  unloadTexture(textureCTXs[i--]);
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



