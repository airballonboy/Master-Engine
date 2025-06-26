#include "renderer.h"
#include "gameObject.h"
#include "textureLoader.h"
#include <raylib.h>
#include <algorithm>

namespace ME {

bool Renderer::resetTexture(){
    this->textureIter = textureLoader::isTextureLoaded({this->texturePath.c_str(), this->textureSize, this->framePos});
    this->texture = textureLoader::loadedTextures[this->textureIter];
    return true;
}

void Renderer::renderAll(){
    std::sort(Renderer::renderOrder.begin(), Renderer::renderOrder.end(),
                [](const Renderer::renderOrderStruct& a, const Renderer::renderOrderStruct& b){
        return a.order < b.order;
    });
    for (auto& ro : Renderer::renderOrder) {
        gameObject::updateByRef(GetFrameTime(), ro.go);
        ro.go->renderer.rendered = false;
    }
    Renderer::renderOrder.clear();
}


}
