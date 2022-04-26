#ifndef App_hpp
#define App_hpp

#include "Header.hpp"

class App {
public:
	App() {}
	~App() {}
	
	bool Init(const char *title, int windowW, int windowH, bool fullscreen);
	
	void Clean();
	
	SDL_Renderer *renderer;
	
	bool GetRunning(){
		return running;
	}
	
	void Play();
	void Resume();
	void Next();
	
	void Pause();
	void Paused();
	void GameEnded();
	
private:
	int state = -1;
	
	bool running;
	
	Game *game;
	
	SDL_Window *window;
	SDL_Event event;
	
	// frame rate stuff
	const int FPS = 60;
	const int frameDelay = 1000/FPS;
	Uint32 frameStart;
	int frameTime;
};


#endif /* App_hpp */
