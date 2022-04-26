#ifndef EventHandler_h
#define EventHandler_h

#define MAX_EVENTS 64

#include "Game.hpp"

class LinkParent {
public:
	virtual void Call() {}
	
	virtual void Destroy() {}
};

template <typename T> class EventHandleLink : public LinkParent {
public:
	EventHandleLink(T *_object, void (T::*_function)()) : LinkParent() {
		object = _object;
		function = _function;
	}
	
	void Call() override {
		(*object.*function)();
	}
	
	void Destroy() override {
		delete this;
	}
	
private:
	T *object;
	void (T::*function)();
};

class EventHandler {
public:
	EventHandler() {}
	~EventHandler() {
		for(int i=0; i<keyDownN; i++) keyDownLinks[i]->Destroy();
		for(int i=0; i<keyUpN; i++) keyUpLinks[i]->Destroy();
		for(int i=0; i<mouseDownN; i++) mouseDownLinks[i]->Destroy();
		for(int i=0; i<mouseUpN; i++) mouseUpLinks[i]->Destroy();
	}
	
	void HandleEvents(SDL_Event event);
	
	template <typename T> void ListenKeyDown(T *object, SDL_KeyCode keyCode, void (T::*function)()){
		keyDownLinks[keyDownN] = new EventHandleLink<T>(object, function);
		keyDownCodes[keyDownN] = keyCode;
		keyDownN++;
	}
	template <typename T> void ListenKeyUp(T *object, SDL_KeyCode keyCode, void (T::*function)()){
		keyUpLinks[keyUpN] = new EventHandleLink<T>(object, function);
		keyUpCodes[keyUpN] = keyCode;
		keyUpN++;
	}
	template <typename T> void ListenMouseDown(T *object, Uint8 mouseButton, void (T::*function)()){
		mouseDownLinks[mouseDownN] = new EventHandleLink<T>(object, function);
		mouseDownButtons[mouseDownN] = mouseButton;
		mouseDownN++;
	}
	template <typename T> void ListenMouseUp(T *object, Uint8 mouseButton, void (T::*function)()){
		mouseUpLinks[mouseUpN] = new EventHandleLink<T>(object, function);
		mouseUpButtons[mouseUpN] = mouseButton;
		mouseUpN++;
	}
	
private:
	// key down events
	LinkParent *keyDownLinks[MAX_EVENTS];
	SDL_KeyCode keyDownCodes[MAX_EVENTS];
	int keyDownN = 0;
	
	// key up events
	LinkParent *keyUpLinks[MAX_EVENTS];
	SDL_KeyCode keyUpCodes[MAX_EVENTS];
	int keyUpN = 0;
	
	// mouse down events
	LinkParent *mouseDownLinks[MAX_EVENTS];
	Uint8 mouseDownButtons[MAX_EVENTS];
	int mouseDownN = 0;
	
	// mouse up events
	LinkParent *mouseUpLinks[MAX_EVENTS];
	Uint8 mouseUpButtons[MAX_EVENTS];
	int mouseUpN = 0;
};

#endif /* EventHandler_h */
