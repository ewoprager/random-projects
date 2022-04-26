#ifndef Slime_hpp
#define Slime_hpp

#include "Game.hpp"
#include "EnemyParent.hpp"

class Slime : public EnemyParent {
public:
	Slime(Game *game, Vector2D initialPos, CollisionMask *msk);
	~Slime();
	
	void Update() override;
	
	void Damage(float amount) override;
	
private:
	// attributes
	float speed = 3.5;
	float maxHealth = 100;
	float damage = 20;
	float knock = 7;
	float damageShake = 20;
	float acc = 0.3;
	
	Sprite *sprite;
	
	Collider *collider;
	int colliderId;
};

class DeadSlime : public UpdateRenderParent {
public:
	DeadSlime(Game *game, Vector2D pos);
	~DeadSlime();
	
	void Update() override {
		timer--;
		if(timer <= 0) delete this;
	}
	
private:
	Sprite *sprite;
	int timer;
};

#endif /* Slime_hpp */
