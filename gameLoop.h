#pragma once

namespace ME {
	//Gets called before the first frame is rendered
	bool initGame();
	
	//Gets called 5 times per seconed (might not be true)
	void fixedUpdate();
	
	//Gets called every frame within the Draw region
	bool gameLoop();
	
	//Gets caller every frame within the rlImGui region
	void imGuiLoop();
	
	//Gets called one time before the game closes
	int closeGame();
}
