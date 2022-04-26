#include "EventHandler.hpp"

SmartArray<MAX_EVENT_LISTENERS, eventListenerStruct> listenerArray = SmartArray<MAX_EVENT_LISTENERS, eventListenerStruct>();
static SDL_Event event;
static SDL_Event escapeEvent;
static bool keysDown[512];

InputBindingStruct inputBindings[INPUTS_N];
SDL_KeyCode keyDownInputBindings[KEYDOWNINPUTS_N];

bool EventHandler_GetKeyDown(const KeyDownInputBinding& key){
	return keysDown[KEYCODE_INDEX(keyDownInputBindings[key])];
}

void EventHandler_Init(){
	for(int i=0; i<512; i++) keysDown[i] = false;
	
	escapeEvent.type = SDL_KEYDOWN;
	escapeEvent.key.keysym.sym = SDLK_ESCAPE;
}

bool EventHandler_HandleEvents(){
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

void EventHandler_Remove(int& id){
	listenerArray.Remove(id);
}

