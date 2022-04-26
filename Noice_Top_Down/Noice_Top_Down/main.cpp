#include "App.hpp"

App *app;

int main() {
	// game handling object
	app = new App();
	if(!app->Init("Noice Top-Down", VIEW_WIDTH*TILE_SIZE, VIEW_HEIGHT*TILE_SIZE, false)){ return -1; }
	
	// initialising randomness
	srand(0);
	
	app->Play();
	
	app->Clean();
	
	return 0;
}
