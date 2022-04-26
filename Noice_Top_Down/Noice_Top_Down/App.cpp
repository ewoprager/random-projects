#include "App.hpp"
#include "Game.hpp"

bool App::Init(const char *title, int windowW, int windowH, bool fullscreen){
	
	int flags = 0;
    if(fullscreen){
        flags = SDL_WINDOW_FULLSCREEN;
    }
	
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "Initializtaion failed.\n";
		return false;
	}
	
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowW, windowH, flags);
	if(!window){
		std::cout << "Window creation failed.\n";
		return false;
	}
	
	renderer = SDL_CreateRenderer(window, -1, 0);
	if(!renderer){
		std::cout << "Renderer creation failed.\n";
		return false;
	}
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	if(SDL_Init(SDL_INIT_AUDIO) != 0){
		std::cout << "Audio initiallistaion failed.\n";
		return false;
	}
	// Initialize Simple-SDL2-Audio
	initAudio();
	
	running = true;
	return true;
}

void App::Clean(){
	delete game;
	SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

void App::Play(){
	game = new Game(this, renderer);
	Resume();
}
void App::Resume(){
	state = 0;
	// game loop
	while(state == 0){
		frameStart = SDL_GetTicks();
		
		game->HandleEvents();
		game->Update();
		game->Render();
		
		// frame rate capping:
		frameTime = SDL_GetTicks() - frameStart;
		if(frameDelay > frameTime){
			SDL_Delay(frameDelay - frameTime);
		}
	}
	Next();
}

void App::Pause(){
	state = 1;
}
void App::GameEnded(){
	state = -1;
}

void App::Next(){
	switch(state){
		case -1:
			Clean();
			break;
		case 0:
			Resume();
			break;
		case 1:
			Paused();
			break;
			
		default:
			break;
	}
}

void App::Paused(){
	bool movingOn = false;
	while(!movingOn){
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT){
				state = -1;
				movingOn = true;
				break;
			} else if( (event.type == SDL_KEYDOWN) ? (event.key.keysym.sym == (SDL_KeyCode)112) : false ){
				state = 0;
				movingOn = true;
				break;
			}
		}
		SDL_Delay(100);
	}
	Next();
}
