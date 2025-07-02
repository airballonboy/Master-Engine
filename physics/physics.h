#ifndef PHYSICS_H
#define PHYSICS_H

#include "gameObject.h"
#include <raylib.h>

namespace ME {

class physicsObject : public gameObject {
public:
    physicsObject(const char* texturePath, Vector2 textureSize = {128,128}, Vector2 framePos = {0,0});
    physicsObject();

    void update();

    struct Force
    {
        Vector4 v4Force;
        Vector2 v2Force;

    }force;

    void vec4ToVec2(Vector4& vec4, Vector2& vec2);
    void forceToAcc();
    void applyGravity();

    float mass;
    float baseGravity = 9.8;
    float gravity = baseGravity;
    bool  enableGravity;
};


}

#endif //PHYSICS_H
