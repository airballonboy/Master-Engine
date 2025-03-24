#pragma once
#include <raylib.h>
#include <string>
#include <vector>

namespace ME {

class gameObject;

class Renderer{
public:
	struct renderLayerStruct
	{
		std::string name;
		size_t order = 10;
	};
	inline static std::vector<renderLayerStruct> layers;

	struct renderOrderStruct
	{
		int order = 0;
		size_t goId;
		gameObject* go;
	};
	inline static std::vector<renderOrderStruct> renderOrder;

	Texture2D texture;
	size_t	  textureIter;
	Vector2 textureSize 	= {128,128};
	Vector2 framePos 		= {0,0};
	Vector2 texturePadding 	= {0,0};
	std::string texturePath;
	renderLayerStruct renderLayer = {"default",10};	
	Color textureColor = WHITE;
	bool rendered = false;
	bool animation = false;
	Rectangle textureRect;
	std::vector<Vector2>frames = {};
	float animationFrameTime = 0.2f;
	float currentAnimationTime = 0.f;

	static void renderAll();


};

}
