#ifndef Bullet_hpp
#define Bullet_hpp

#include "Game.hpp"
#include "UpdateRenderParent.hpp"
#include "Sprite.hpp"
#include "Collider.hpp"

class Bullet : public UpdateRenderParent {
public:
	Bullet(Game *game, Vector2D pos, double angle, float speed, float dmg, float ms, CollisionMask *msk);
	~Bullet();
	
	void Update() override;
	
private:
	float damage;
	float mass;
	int life = 60;
	
	int updateId;
	Vector2D velocity;
	
	Sprite *sprite;
	CollisionMask *mask;
};

#endif /* Bullet_hpp */
