#include "physics/physics.h"
#include "gameObject.h"
#include "raymath.h"
#include <raylib.h>
#include <cstdio>

namespace ME {
physicsObject::physicsObject(const char* _texturePath, Vector2 _textureSize, Vector2 _framePos) 
                                : gameObject(_texturePath, _textureSize, _framePos){
    

    
}
physicsObject::physicsObject(){}


void physicsObject::updatePhysics(){
    if (this->enableGravity)
        this->applyGravity();
    forceToAcc();
    
}

void physicsObject::applyGravity(){
    force.v4Force.w += gravity*mass;
}

void physicsObject::vec4ToVec2(Vector4& vec4, Vector2& vec2){
    vec2 = {
        .x = vec4.x - vec4.z,
        .y = vec4.y - vec4.w
    };
}
void physicsObject::forceToAcc(){
    vec4ToVec2(force.v4Force, force.v2Force);
    if (mass == 0) transform.acc = {0,0};
    transform.acc = {
        force.v2Force.x / mass,
        force.v2Force.y / mass
    };
}


}
