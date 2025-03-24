#include "renderer.h"
#include "gameObject.h"
#include <raylib.h>
#include <algorithm>

namespace ME {


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
