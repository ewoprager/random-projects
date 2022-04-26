#ifndef EnemyParent_hpp
#define EnemyParent_hpp

#include "Game.hpp"
#include "UpdateRenderParent.hpp"

class EnemyParent : public UpdateRenderParent {
public:
	EnemyParent(Game *game, Vector2D initialPos, CollisionMask *msk);
	~EnemyParent();
	
	CollisionMask *GetMask();
	
	virtual void Damage(float amount) {} // must be overridden because destructor is called on self
	
	void Knock(Vector2D knock);
	
protected:
	float mass;
	int enemyId;
	float health;
	Vector2D velocity = Vector2D();
	CollisionMask *mask;
};

#endif /* EnemyParent_hpp */
