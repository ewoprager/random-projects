#ifndef BigSlime_hpp
#define BigSlime_hpp

#include "Game.hpp"
#include "EnemyParent.hpp"

class BigSlime : public EnemyParent {
public:
	BigSlime(Game *game, Vector2D initialPos, CollisionMask *msk);
	~BigSlime();
	
	void Update() override;
	
	void Damage(float amount) override;
	
private:
	// attributes
	float speed = 2.5;
	float maxHealth = 700;
	float damage = 40;
	float knock = 30;
	float damageShake = 40;
	float acc = 0.1;
	
	Sprite *sprite;
	
	Collider *collider;
	int colliderId;
};

class DeadBigSlime : public UpdateRenderParent {
public:
	DeadBigSlime(Game *game, Vector2D pos);
	~DeadBigSlime();
	
	void Update() override {
		timer--;
		if(timer <= 0) delete this;
	}
	
private:
	Sprite *sprite;
	int timer;
};

#endif /* BigSlime_hpp */
