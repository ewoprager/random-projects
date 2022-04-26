#include "App.hpp"

// frame rate stuff
int frameDelay = 3; // 333 steps per second
Uint32 frameStart;
int frameTime = 10;

int main() {
	
	App *app = new App();
	
	if(!app->Init("Physics 2D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 768, false)){ std::cout << "Initialisation failed.\n"; }	
	
	while(true){
		if(!app->Running()){ app->Clean(); break; }
		
		// to allow for debugging rendering
		SDL_RenderClear(app->renderer);
		
		app->HandleEvents();
		app->LoopStart(frameTime);
		app->LoopEnd(frameTime);
		
		app->Render();
		
		// frame rate handling
		int time = SDL_GetTicks();
		frameTime = time - frameStart;
		frameStart = time;
		if(frameTime < frameDelay){
			SDL_Delay(frameDelay - frameTime);
			frameTime = frameDelay;
		}
		//std::cout << frameTime << "\n";
	}
	
	return 0;
}
