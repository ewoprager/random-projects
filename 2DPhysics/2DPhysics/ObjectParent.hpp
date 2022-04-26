#ifndef ObjectParent_hpp
#define ObjectParent_hpp

#include "App.hpp"

class ObjectParent {
public:
	virtual void Render(SDL_Renderer *renderer) {}
	virtual void Update(float deltaT) {}
};

#endif /* ObjectParent_hpp */
