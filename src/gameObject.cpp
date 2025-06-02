#include "gameObject.h"
#include "raylib.h"
#include "raymath.h"
#include "textureLoader.h"
#include <cassert>
#include <cstddef>
#include <algorithm>
#include <cstdio>
#include <iostream>


namespace ME{



gameObject::gameObject(const char* _texturePath, Vector2 _textureSize, Vector2 _framePos){
	id = ++currentObjectCount;
	renderer.texturePath = _texturePath;
	renderer.textureSize = _textureSize;
	renderer.framePos	 = _framePos;
	renderer.textureIter = textureLoader::isTextureLoaded({_texturePath, _textureSize, _framePos});
	renderer.texture	 = textureLoader::loadedTextures[renderer.textureIter]; 	
	if(!checkLayer("default", &renderer.layers)) newLayer("default", 10);
	addToLayer(this, "default");
}
gameObject::gameObject(){}
gameObject::~gameObject(){
	// TODO: understand how destructors work LOL
	//std::cout << "destroyed succ" << std::endl;
}

gameObject* gameObject::gameObjectWithId(size_t id, std::vector<gameObject*>& GV){
	for (auto& go : GV){ 
		if (go->id == id) 
			return go;
	}
	return nullptr;
}
size_t gameObject::getIterator(size_t id, std::vector<gameObject*>& GV){
	size_t i = 0;
	for (auto& go : GV){ 
		if (go->id == id) return i;
		i++;
	}
	std::cerr << "tried to get iterator of object not in vector\n";
	return 0;
}
void gameObject::updateVector(float DT, std::vector<gameObject*>& GV){
	int currentRenderOrder = 1;

	// For y level sorting
	std::sort(GV.begin(), GV.end(), [](gameObject*& o1, gameObject*& o2){
		return o1->transform.pos.y + o1->transform.dim.y * o2->transform.pivot.y <
				o2->transform.pos.y + o2->transform.dim.y * o2->transform.pivot.y;
	});
	
	for (auto& layer : Renderer::layers) {
		currentRenderOrder = currentObjectCount * layer.order;
		for (auto& go : GV){
			if (go->erased || go->renderer.rendered) continue;
			if (go->renderer.renderLayer.name == layer.name) {
				Renderer::renderOrder.push_back({currentRenderOrder++, go->id, go});
				//std::cout << "rendered object: " << go->id << std::endl;
				go->renderer.rendered = true;
			}
		}
	}
}
void gameObject::updateByRef(float DT, gameObject* go){
	
	updateRects(go);
	DrawTexturePro(go->renderer.texture, go->renderer.textureRect, go->transform.posRect, 
				go->transform.pivot * go->transform.dim, go->transform.rotation, go->renderer.textureColor);
	if (showColliderBorders)
		DrawRectangleLinesEx(go->collision.collisionBox, 2, WHITE);
}
void gameObject::updateRects(gameObject* go){
	// TODO: make Textures with 1px gap between each texture and making a system for it
	go->renderer.textureRect = {
		// Origin
		go->renderer.framePos.x * go->renderer.textureSize.x + go->renderer.texturePadding.x * go->renderer.framePos.x,
		go->renderer.framePos.y * go->renderer.textureSize.y + go->renderer.texturePadding.y * go->renderer.framePos.y,
		// Size
		go->renderer.textureSize.x - 1,
		go->renderer.textureSize.y - 1 
	};
	go->transform.posRect = {
		go->transform.pos.x,
		go->transform.pos.y,
		go->transform.dim.x,
		go->transform.dim.y
	};
}
void gameObject::transformStruct::movement(){
	if (move == Vector2Zero()) {
		vel = {baseVel,baseVel};
		return;
	}
	move = Vector2Normalize(move);

	// Adding half of the speed before moving and the other half after is to make sure that 
	// movement is consistant with every frameTime because speed is just increasing linearly
	vel += acc * GetFrameTime() * 0.5;

	if (vel.x > maxVel) vel.x = maxVel;
	if (vel.y > maxVel) vel.y = maxVel;

	pos += move * vel * velMulti * GetFrameTime();

	vel += acc * GetFrameTime() * 0.5;
}

void gameObject::setDimension(Vector2 d){
	if (this->collision.colliderSize.x < d.x) this->collision.colliderSize.x = d.x;
	if (this->collision.colliderSize.y < d.y) this->collision.colliderSize.y = d.y;
	this->transform.dim = d;
}

void gameObject::collisionStruct::updateCollision(std::vector<gameObject*>& GV){
	auto checkCollision = [](gameObject* o1, gameObject* o2){
		assert(o1 != nullptr && o2 != nullptr && "accessing a nullptr");
		o1->collision.collisionBox = {
			o1->transform.pos.x - (o1->collision.colliderSize.x / 2),
			o1->transform.pos.y - (o1->collision.colliderSize.y / 2),
			o1->collision.colliderSize.x,
			o1->collision.colliderSize.y
		};
		o2->collision.collisionBox = {
			o2->transform.pos.x - (o2->collision.colliderSize.x / 2),
			o2->transform.pos.y - (o2->collision.colliderSize.y / 2),
			o2->collision.colliderSize.x,
			o2->collision.colliderSize.y
		};
		return CheckCollisionRecs(o1->collision.collisionBox, o2->collision.collisionBox);
	};
	for (auto& objA : GV) {
		if (objA->erased || !objA->collision.enableCollision) continue;
		if (objA->collision.collided){
			if (objA->collision.collidedWith->erased || !checkCollision(objA, objA->collision.collidedWith)){
				objA->collision.collidedWith->collision.collided 	 = false;
				if(objA->collision.collidedWith->collision.onCollisionExit != nullptr)
					objA->collision.collidedWith->collision.onCollisionExit(objA);
				objA->collision.collidedWith->collision.collidedWith = nullptr;

				objA->collision.collided 	 = false;
				if(objA->collision.onCollisionExit != nullptr)
					objA->collision.onCollisionExit(objA->collision.collidedWith);
				objA->collision.collidedWith = nullptr;
			}
			continue;
		}
		for (auto& objB : GV) {
			if ((objB->erased) || (objA->id == objB->id) || (!objB->collision.enableCollision)) continue;	
			if (checkCollision(objA, objB)) {
				//std::cout << "obj: " << objA->id << " collided with obj: " << objB->id << std::endl;
				objB->collision.collided 	 = true;
				objB->collision.collidedWith = objA;
				if(objB->collision.onCollisionEnter != nullptr)
					objB->collision.onCollisionEnter(objA);
				objA->collision.collided 	 = true;
				objA->collision.collidedWith = objB;
				if(objA->collision.onCollisionEnter != nullptr)
					objA->collision.onCollisionEnter(objB);
				break;
			}
		}

	}
}

void gameObject::collisionStruct::setColliderSize(Vector2 vec){
	this->colliderSize = vec;
}

void gameObject::collisionStruct::updateCollision(gameObject* go, std::vector<std::vector<gameObject*>>& gameVectors){
	//Unused
	if (go->erased) return;
	if (!go->collision.enableCollision) return;
	if (go->collision.currentColliderType != colliderType::Box) return;
	go->collision.collisionBox = {go->transform.pos.x, go->transform.pos.y, go->transform.dim.x, go->transform.dim.y};
	for (auto& gameVector : gameVectors) {
		for (auto& other : gameVector) {
			if (other->erased) continue;
			if (!other->collision.enableCollision) continue;
			if (other->collision.currentColliderType != colliderType::Box) continue;
			other->collision.collisionBox = {other->transform.pos.x, other->transform.pos.y, other->transform.dim.x, other->transform.dim.y};
			bool colided = CheckCollisionRecs(go->collision.collisionBox, other->collision.collisionBox);
			if (colided) {
				go->collision.collided = true;
				go->collision.collidedWith = other;
				other->collision.collided = true;
				other->collision.collidedWith = go;
				std::cout << "collision: object with id: " << go->id << " collided with object with id: " << other->id << std::endl; 
				return;
			}
					
		}
	}
}

void gameObject::newLayer(std::string name, size_t order){
	for (auto& l : Renderer::layers) {
		if(l.name == name){
			std::cerr << "layer already exists" << std::endl;
			return;
		};
	}
	Renderer::layers.push_back({name, order});
    std::sort(Renderer::layers.begin(), Renderer::layers.end(), 
		[](const Renderer::renderLayerStruct& a, const Renderer::renderLayerStruct& b){
			return a.order < b.order;
		}
	);
}
bool gameObject::checkLayer(std::string thisName, std::vector<Renderer::renderLayerStruct>* L){
	bool C;
	for (Renderer::renderLayerStruct RL : *L) {
		if (RL.name == thisName) { C = true;break; }
		C = false;
	}
	return C;
}

void gameObject::addToLayer(gameObject* GO, std::string name){
	auto find = [](std::string thisName, std::vector<Renderer::renderLayerStruct>* L){ 
        for(Renderer::renderLayerStruct RL : *L){
            if (RL.name == thisName) return RL.order;
        }
        return (size_t)0;
    };
    
    if (!checkLayer (name, &Renderer::layers)) 
		std::cerr << "name doesn't match any layer";
    
    GO->renderer.renderLayer.name = name;
    GO->renderer.renderLayer.order = find(name, &Renderer::layers);
}




}

