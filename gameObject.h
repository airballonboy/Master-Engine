#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <functional>
#include <cstddef>
#include <string>
#include <vector>
#include <raylib.h>
#include "renderer.h"

// TODO: Need to comment more on the code and make documentation

namespace ME {
class gameObject {
public:
    inline static size_t currentObjectCount = 0;

    inline static bool showColliderBorders = false;

    size_t id = 0;

    //Indicates that an object currently should not get updated, 
    //but may get updated later when erased is false agian
    bool erased = false;

    gameObject(const char* texturePath, Vector2 textureSize = {128,128}, Vector2 framePos = {0,0});
    gameObject();
    ~gameObject();
    


    //Updates the object referenced
    static void updateByRef(float deltaTime, gameObject* object);

    //Updates the full Vector of pointers to objects
    static void updateVector(float deltaTime, std::vector<gameObject*>& vector);

    //Update the textureRect and posRect to the values of current frame
    static void updateRects(gameObject* object);
    
    //Gets the iterator of the object with the id in this Vector
    static size_t getIterator(size_t id, std::vector<gameObject*>& vector);
    
    //Gets the object with the id refernced in this Vector
    static gameObject* gameObjectWithId(size_t id, std::vector<gameObject*>& vector);

    std::string texturePath;
    Renderer renderer;

    // NOTE: circle not implemented yet
    enum colliderType{ Box, Circle };

    void setDimension(Vector2 d);

    struct transformStruct {
        //Updates the movment and pos of this object
        void movement();

        Vector2 move     = {  0,0  };
        Vector2 pos      = {  0,0  };//[pos]ition
        Vector2 vel      = {  0,0  };//[vel]ocity
        Vector2 acc      = {200,200};//[acc]elration
        Vector2 dim      = {128,128};//[dim]ension
        Vector2 pivot    = {0.5,0.5};
        float   baseVel  = 400;
        float   maxVel   = 400;
        float   velMulti = 1;//speed multiplier
        float   rotation = 0.f;
        float   baseGravity = 9.8;
        float   gravity = baseGravity;
        bool    enableGravity;
        Rectangle posRect;
    }transform;

    struct collisionStruct {
        //Checks every object collision in the Vector refernced
        static void updateCollision(std::vector<gameObject*>& gameVector);

        //Sets the collider size of the gameObject
        void setColliderSize(Vector2 rect);

        //Collision functions
        std::function<void(gameObject* other)> onCollisionEnter = nullptr;
        std::function<void(gameObject* other)> onCollisionExit  = nullptr;

        //How to use vector of vectors collisions
            //std::vector<std::vector<gameObject*>> a;
            //a.push_back(vector<gameObject*>);
            //gameObject::collisionStruct::updateCollision(object, a);
        //checks the collision of the refernced object with every object in the Vector
        static void updateCollision(gameObject* object, std::vector<std::vector<gameObject*>>& gameVectors);


        std::string tag = "default";
        colliderType currentColliderType = colliderType::Box;
        Rectangle collisionBox   = {0,0,0,0};
        Vector2   colliderSize   = {128,128};
        bool enableCollision     = false;
        bool enableBlocking      = false;
        bool collided            = false;
        gameObject* collidedWith = nullptr;
    }collision;
    
    // Adds a new layer if there is no layer with this name
    static void newLayer(std::string layerName, size_t layerOrder);
    // Adds a gameObject to existing layer
    static void addToLayer(gameObject* gameObject, std::string layerName);
    // Checks if a layer with this name exists
    static bool checkLayer(std::string layerName, std::vector<Renderer::renderLayerStruct>* layersVector);
 

};
}


#endif //GAMEOBJECT_H
