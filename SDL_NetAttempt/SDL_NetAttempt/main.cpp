#include "Server.hpp"
#include "Client.hpp"

int rs[6] = {255,	0,		0,		255,	255,	0};
int gs[6] = {0,		255,	0,		255,	0,		255};
int bs[6] = {0,		0,		255,	0,		255,	255};

std::string input;
int inint;
bool valid;

Network *network;

SDL_Event event;

// frame rate:
const int FPS = 60;
const int frameDelay = 1000/FPS;
Uint32 frameStart;
int frameTime;

SDL_Window *window;
SDL_Renderer *renderer;

bool isRunning;

bool mouseDown;

bool Init() {
	int flags = 0;
    /*
	if(fullscreen){
        flags = SDL_WINDOW_FULLSCREEN;
    }
	*/
	
	isRunning = true;
    
    // subsystems:
    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cout << "Subsystems failed to initialise.\n";
        isRunning = false;
        return false;
    }
    std::cout << "Subsystems initialised.\n";
    
    // window:
    window = SDL_CreateWindow("Networking Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 240, 240, false);
    if(!window){
        std::cout << "Window creation failed.\n";
        isRunning = false;
        return false;
    }
    std::cout << "Window created.\n";
    
    // renderer:
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer){
        std::cout << "Renderer creation failed.\n";
        isRunning = false;
        return false;
    }
    std::cout << "Renderer created.\n";
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
	return true;
}

void Render(){
	SDL_RenderClear(renderer);
	
	short int ids[MAX_SOCKETS];
	float xs[MAX_SOCKETS];
	float ys[MAX_SOCKETS];
	int num = network->GetExternalData(ids, xs, ys);
	for(short int i=0; i<num; i++) {
		SDL_SetRenderDrawColor(renderer, rs[ids[i]], gs[ids[i]], bs[ids[i]], 255);
		SDL_RenderDrawLine(renderer, xs[i]-32, ys[i], xs[i]+32, ys[i]);
		SDL_RenderDrawLine(renderer, xs[i], ys[i]-32, xs[i], ys[i]+32);
	}
	SDL_SetRenderDrawColor(renderer, rs[network->id], gs[network->id], bs[network->id], 255);
	SDL_RenderDrawLine(renderer, network->x-32, network->y, network->x+32, network->y);
	SDL_RenderDrawLine(renderer, network->x, network->y-32, network->x, network->y+32);
	SDL_RenderDrawLine(renderer, network->x-6, network->y-6, network->x+6, network->y+6);
	SDL_RenderDrawLine(renderer, network->x+6, network->y-6, network->x-6, network->y+6);
	
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	
    SDL_RenderPresent(renderer);
}

void Go() {
	if(Init()){
	
		do {
			frameStart = SDL_GetTicks();
			
			SDL_PollEvent(&event);
			if(event.type == SDL_MOUSEBUTTONDOWN){
				mouseDown = true;
			}
			if(event.type == SDL_MOUSEBUTTONUP){
				mouseDown = false;
			}
			if(mouseDown){
				int _x;
				int _y;
				SDL_GetMouseState(&_x, &_y);
				network->x = (float)_x;
				network->y = (float)_y;
			}
			
			network->Send();
			network->Recv();
			Render();
			
			// frame rate capping:
			frameTime = SDL_GetTicks() - frameStart;
			if(frameDelay > frameTime){
				SDL_Delay(frameDelay - frameTime);
			}
		} while(event.type != SDL_QUIT && network->GetRunning());
	
	} else {
		std::cout << "Init failed.\n";
	}
}

int main(int argc, const char * argv[]) {
	std::cout << "Am I a server[1] or a client[0]?\n";
	valid = false;
    do {
        std::cin >> input;
        valid = sscanf(input.c_str(),"%d",&inint);
		if(inint != 0 && inint != 1) valid = false;
    } while(valid == false);
	
	if(inint){
		network = new Server();
		
		Go();
		
		delete network;
	} else {
		std::cout << "Enter server IP.\n";
		valid = false;
		do {
			std::cin >> input;
			valid = sscanf(input.c_str(),"%d",&inint);
		} while(valid == false);
		network = new Client(input.c_str());
		
		Go();
		
		delete network;
	}
	
	SDL_Quit();
	
	return 0;
}
