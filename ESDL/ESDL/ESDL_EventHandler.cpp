#include "ESDL_EventHandler.hpp"

namespace ESDL {

static SmartArray<MAX_EVENT_LISTENERS, EventListenerStruct> listenerArray = SmartArray<MAX_EVENT_LISTENERS, EventListenerStruct>();
static SDL_Event event;
static SDL_Event escapeEvent;
static bool keysDown[256];

constexpr int KEYCODE_INDEX(const SDL_Keycode& x){ return x & 0b11111111111111111111111111111; }

bool GetKeyDown(const SDL_Keycode& key){
	return keysDown[KEYCODE_INDEX(key)];
}

bool EventEquality(const SDL_Event& event1, const SDL_Event& event2){
	if(event1.type != event2.type) return false;
	switch(event1.type){
		case SDL_KEYUP:
		case SDL_KEYDOWN:
			if(event1.key.keysym.sym != event2.key.keysym.sym) return false;
			break;
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			if(event1.button.button != event2.button.button) return false;
			break;
		default:
			break;
	}
	return true;
}

void InitEventHandler(){
	for(int i=0; i<256; i++) keysDown[i] = false;
	
	escapeEvent.type = SDL_KEYDOWN;
	escapeEvent.key.keysym.sym = SDLK_ESCAPE;
}

bool HandleEvents(){
	while(SDL_PollEvent(&event)){
		// quitting
		if(event.type == SDL_QUIT) return true;
		if(EventEquality(event, escapeEvent)) return true;
		// saving keys down
		if(event.type == SDL_KEYDOWN){
			keysDown[KEYCODE_INDEX(event.key.keysym.sym)] = true;
		} else if(event.type == SDL_KEYUP){
			keysDown[KEYCODE_INDEX(event.key.keysym.sym)] = false;
		}
		// listener callback functions
		for(int i=0; i<listenerArray.GetN(); i++){
			if(EventEquality(event, listenerArray[i].event)) listenerArray[i].listener->Call(event);
		}
	}
	return false;
}

void RemoveEventCallback(int& id){
	listenerArray.Remove(id);
}

}


