#include <raylib.h>
#include <raymath.h>
#include <imgui.h>
#include <cmath>
#include <cstdio>
#include <cstddef>
#include "gameLoop.h"
#include "gameObject.h"
#include "physics.h"
#include "settings.h"
#include "textureLoader.h"
#include "UI/UI.h"
#include "gameFuncs/publicVariables.h"
#include "gameFuncs/player.h"
#include "gameFuncs/bgRenderer.h"


int targetFps = 144;


bool ME::initGame(){
	vectors::PV.reserve(1000);
	vectors::BV.reserve(1000);
	vectors::GV.reserve(2000);
	vectors::updatableVectors.push_back(&vectors::GV);
	vectors::updatableVectors.push_back(&vectors::PV);
	Player::create();
	camera.target = player.transform.pos;
	camera.zoom = 0.7f;
	camera.rotation = 0.f;
	BG::Array = {
		bg{RESOURCES_PATH "background1.png", 0.6f},
		bg{RESOURCES_PATH "background2.png", 0.5f},
		bg{RESOURCES_PATH "background3.png", 0.3f},
		bg{RESOURCES_PATH "background4.png", 0.1f}
	};
	physicsObject* phy = new physicsObject(RESOURCES_PATH "spaceShip/ships/Blue.png", {16,16});
	vectors::PV.push_back(phy);
	return true;
}
void ME::fixedUpdate(){
	camera.offset = {GetRenderWidth()/2.f,GetRenderHeight()/2.f};
	SetTargetFPS(targetFps);

	for (size_t i = 0; i < vectors::BV.size(); i++) {
		if (Vector2Length(player.transform.pos - vectors::BV[i]->transform.pos) > 5000) {
			vectors::GV.erase(vectors::GV.begin() + gameObject::getIterator(vectors::BV[i]->id, vectors::GV));
			delete vectors::BV[i];
			vectors::BV.erase(vectors::BV.begin() + i);
			i--;continue;
		}
	}

}
bool ME::gameLoop(){
	if(IsKeyPressed(KEY_F11)) ME::settings::ToggleFullScreen();

	for (size_t i = 0; i < vectors::BV.size(); i++) vectors::BV[i]->transform.movement();


	for (auto& e : vectors::EV) {
		// want to add margin of error or even rotation
		// and create a func for it
		e->transform.pos = Vector2MoveTowards(
			e->transform.pos,
			player.transform.pos,
			e->transform.baseVel*GetFrameTime()
		);
		if (e->transform.pos != player.transform.pos) 
			e->transform.rotation = 
				atan2((e->transform.pos - player.transform.pos).y, (e->transform.pos - player.transform.pos).x) * RAD2DEG - 90.f;

	}	


	gameObject::collisionStruct::updateCollision(vectors::GV);

	Player::move(); // checks input
	Player::shoot();// checks input
	Player::healthSystem.hpUpdate();	
	Player::updateCam();

	for (auto& vector : vectors::updatableVectors) gameObject::updateVector(GetFrameTime(), *vector);

	BeginMode2D(camera);
		BG::update();
		Renderer::renderAll();
		DrawCircleV(player.transform.pos, 5, RED);
	EndMode2D(); 


	UI::update();
	return true;
}
void ME::imGuiLoop(){
	if (ImGui::Begin("Debug")) {
		ImGui::Text("fps: %d", (int)roundf(1.f/GetFrameTime()));
		ImGui::InputInt("targetFps", &targetFps);
		ImGui::Text("playerPos:\n  x: %.1f y: %.1f", player.transform.pos.x, player.transform.pos.y);
		ImGui::Text("bulletCount: %d", (int)vectors::BV.size());
		ImGui::Text("cameraZoom");
		ImGui::SliderFloat(" ",  &camera.zoom, 0.1f, 1.9f);
		if (ImGui::Button("Reload Textures")) {
			textureLoader::reloadTextures();
			for (auto& bg : BG::Array)	bg.texture = textureLoader::loadedTextures[bg.textureIter];
			for (auto& vector : vectors::updatableVectors) {
				for (auto& go : *vector) go->renderer.texture = textureLoader::loadedTextures[go->renderer.textureIter];
			}
		}
		if (ImGui::Button("Spawn Enemy")) Player::spawnEnemy(player.transform.pos);
		if (ImGui::Button("Reduce HP")) Player::healthSystem.hpChange(-100);
		if (ImGui::Button("QUIT")) state.run = false;
		ImGui::End();
	}
}
int ME::closeGame(){

	return 0;
}


