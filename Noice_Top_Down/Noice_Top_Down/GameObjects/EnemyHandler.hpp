#ifndef EnemyHandler_hpp
#define EnemyHandler_hpp

#include "Game.hpp"
#include "UpdateRenderParent.hpp"

class EnemyHandler : public UpdateRenderParent {
public:
	EnemyHandler(Game *game);
	~EnemyHandler();
	
	void Update() override;
	
	int AddObject(EnemyParent *obj);
	void RemoveObject(int id);
	
	EnemyParent *CheckCollision(CollisionMask *mask, Vector2D pos);
	
	void Explosion(Vector2D pos, float maxDmg, float range);
	
	Emitter *slimeEmitter;
	
	void EnemyKilled();
	
	void NextWave();
	
private:
	void SpawnSlime();
	void SpawnBigSlime();
	bool CanSpawnHere(Vector2D pos, CollisionMask *mask);
	
	CollisionMask *slimeMask;
	CollisionMask *bigSlimeMask;
	int clock = 0;
	bool enemiesOn[MAX_OBJECTS];
	EnemyParent *enemies[MAX_OBJECTS];
	
	// waves
	int wave = 0;
	int toKill;
};

#endif /* EnemyHandler_hpp */
