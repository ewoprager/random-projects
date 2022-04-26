#ifndef Graveyard_hpp
#define Graveyard_hpp

#include "Header.hpp"
#include "Protocols.hpp"

#define MAX_GRAVEYARDS 64

class Undead : public RenderedInstanced, public Updated, public Collider {
public:
	Undead(vec2 _position);
	
	void Update(float deltaTime) override;
	
	void LeftClick() override;
	
private:
	static constexpr float speed = 4.0f;
};

class Graveyard : public RenderedInstanced, public Updated {
public:
	Graveyard(float yaw, vec2 _position);
	
	void Update(float deltaTime) override;
	
	void UpdateAnger();
	
private:
	vec2 position;
};

#endif /* Graveyard_hpp */
