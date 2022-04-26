#ifndef Grenade_hpp
#define Grenade_hpp

#include "Game.hpp"
#include "UpdateRenderParent.hpp"
#include "Sprite.hpp"
#include "Collider.hpp"

class Grenade : public UpdateRenderParent {
public:
	Grenade(Game *game, Vector2D pos, double angle, float speed, float rng, float maxDmg, float ms, CollisionMask *msk);
	~Grenade();
	
	void Update() override;
	
	void Explode();
	
private:
	float range;
	float maxDmg;
	float mass;
	int life = 42;
	
	int updateId;
	Vector2D velocity;
	
	Sprite *sprite;
	CollisionMask *mask;
};

#endif /* Grenade_hpp */
