#ifndef ESDL_EventHandler_hpp
#define ESDL_EventHandler_hpp

#include "ESDL_General.hpp"

#define MAX_EVENT_LISTENERS 64

namespace ESDL {

// Call in app initialisation.
void InitEventHandler();

// Call every step for which events are to be handled. Returns true when an exit event is received.
bool HandleEvents();

// Returns whether two events are equivalent.
bool EventEquality(const SDL_Event& event1, const SDL_Event& event2);

// Returns whether the given key is pressed.
bool GetKeyDown(const SDL_Keycode& key);

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
struct EventListenerStruct {
	EventListenerParent *listener;
	SDL_Event event;
};

// Tell the event handler to call a given member function when it detects a given event. Returns the event listener id.
template <typename T> int AddEventCallback(MemberFunction<T, void, SDL_Event> function, SDL_Event _event){
	extern SmartArray<MAX_EVENT_LISTENERS, EventListenerStruct> listenerArray;
	return listenerArray.Add({new EventListener<T, void, SDL_Event>(function), _event});
}

// Tell the event handler to remove an event listener by its id.
void RemoveEventCallback(int& id);

}

#endif /* ESDL_EventHandler_hpp */
