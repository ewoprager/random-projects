#include "EventHandler.hpp"

void EventHandler::HandleEvents(SDL_Event event){
	for(int i=0; i<keyDownN; i++){
		if(event.type == SDL_KEYDOWN){
			if(event.key.keysym.sym == keyDownCodes[i]){
				keyDownLinks[i]->Call();
			}
		}
	}
	for(int i=0; i<keyUpN; i++){
		if(event.type == SDL_KEYUP){
			if(event.key.keysym.sym == keyUpCodes[i]){
				keyUpLinks[i]->Call();
			}
		}
	}
	for(int i=0; i<mouseDownN; i++){
		if(event.type == SDL_MOUSEBUTTONDOWN){
			if(event.button.button == mouseDownButtons[i]){
				mouseDownLinks[i]->Call();
			}
		}
	}
	for(int i=0; i<mouseUpN; i++){
		if(event.type == SDL_MOUSEBUTTONUP){
			if(event.button.button == mouseUpButtons[i]){
				mouseUpLinks[i]->Call();
			}
		}
	}
}
