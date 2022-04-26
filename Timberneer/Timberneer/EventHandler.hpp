#ifndef EventHandler_hpp
#define EventHandler_hpp

#include "Header.hpp"

#define MAX_EVENT_LISTENERS 32

#define KEYCODE_INDEX(x) (x & 0b11111111111111111111111111111)

#define INPUTS_N 4
enum InputBinding {ib_interact_primary, ib_interact_secondary, ib_craft, ib_build};
#define KEYDOWNINPUTS_N 5
enum KeyDownInputBinding {ib_forward, ib_backward, ib_left, ib_right, ib_modifier};

struct InputBindingStruct {
	SDL_Event event;
	void (Player::*callBack)(SDL_Event);
};

struct EventListenerParent {
	virtual void Call(SDL_Event event) {}
};

template <typename T, typename returnType, typename... argTypes> class EventListener : public EventListenerParent {
public:
	EventListener(MemberFunction<T, returnType, argTypes...> _function){
		function = _function;
	}
	
	returnType Call(argTypes... args) override {
		return function.Call(args...);
	}
	
private:
	MemberFunction<T, returnType, SDL_Event> function;
};

struct eventListenerStruct {
	EventListenerParent *listener;
	SDL_Event event;
};

bool EventHandler_GetKeyDown(const KeyDownInputBinding& key);

void EventHandler_Init();
bool EventHandler_HandleEvents();
template <typename T> int EventHandler_Add(MemberFunction<T, void, SDL_Event> function, SDL_Event _event){
	extern SmartArray<MAX_EVENT_LISTENERS, eventListenerStruct> listenerArray;
	return listenerArray.Add({new EventListener<T, void, SDL_Event>(function), _event});
}
void EventHandler_Remove(int& id);


#endif /* EventHandler_hpp */
