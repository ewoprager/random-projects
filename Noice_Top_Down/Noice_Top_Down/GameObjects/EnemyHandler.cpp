#include "EnemyHandler.hpp"
#include "Collider.hpp"
#include "Particles.hpp"
#include "EnemyParent.hpp"
#include "Slime.hpp"
#include "BigSlime.hpp"
#include "Player.hpp"

EnemyHandler::EnemyHandler(Game *game) : UpdateRenderParent(game) {
	for(int i=0; i<MAX_OBJECTS; i++) enemiesOn[i] = false;
	updateId = theGame->doUpdates->AddObject(this);
	
	slimeMask = new CollisionMask(Vector2D(-15, -8), 30, 20);
	bigSlimeMask = new CollisionMask(Vector2D(-30, -16), 60, 40);
	slimeEmitter = new Emitter(theGame, this, 150, 80, 105, 130,
							   5, 5, // radius
							   8, 5, // life
							   3, 5); // speed
}
EnemyHandler::~EnemyHandler(){
	theGame->doUpdates->RemoveObject(updateId);
	delete slimeMask;
	delete bigSlimeMask;
	delete slimeEmitter;
}

void EnemyHandler::Update() {
	if(toKill <= 0) NextWave();
}

int EnemyHandler::AddObject(EnemyParent *obj){
	int newId = GetNewId(MAX_OBJECTS, enemiesOn);
	enemies[newId] = obj;
	enemiesOn[newId] = true;
	return newId;
}
void EnemyHandler::RemoveObject(int id){
	enemiesOn[id] = false;
}
void EnemyHandler::SpawnSlime(){
	toKill++;
	Vector2D spawnPos;
	do {
		spawnPos = Vector2D(2.5 + (rand() % (MAP_WIDTH - 4)), 2.5 + (rand() % (MAP_HEIGHT - 4))) * TILE_SIZE;
	} while(!CanSpawnHere(spawnPos, slimeMask));
	new Slime(theGame, spawnPos, slimeMask);
}
void EnemyHandler::SpawnBigSlime(){
	toKill++;
	Vector2D spawnPos;
	do {
		spawnPos = Vector2D(3.5 + (rand() % (MAP_WIDTH - 6)), 3.5 + (rand() % (MAP_HEIGHT - 6))) * TILE_SIZE;
	} while(!CanSpawnHere(spawnPos, bigSlimeMask));
	new BigSlime(theGame, spawnPos, slimeMask);
}
bool EnemyHandler::CanSpawnHere(Vector2D pos, CollisionMask *mask){
	if(theGame->collisionHandler->CheckCollisionColliders(mask, pos)){
		return false;
	}
	Vector2D rel = pos - theGame->player->GetPosition();
	if(rel.x*rel.x + rel.y*rel.y < 9216){
		return false;
	}
	return true;
}
void EnemyHandler::EnemyKilled(){
	toKill--;
}
void EnemyHandler::NextWave(){
	wave++;
	int slimes = 2 + (int)(pow(wave, 0.7));
	for(int i=0; i<slimes; i++){
		SpawnSlime();
		if((i % 8) == 0){
			SpawnBigSlime();
		}
	}
}
EnemyParent *EnemyHandler::CheckCollision(CollisionMask *mask, Vector2D pos){
	for(int i=0; i<MAX_OBJECTS; i++){
		if(enemiesOn[i]){
			if(theGame->collisionHandler->CheckCollision(mask, pos, enemies[i]->GetMask(), enemies[i]->GetPosition())){
				return enemies[i];
			}
		}
	}
	return nullptr;
}
void EnemyHandler::Explosion(Vector2D pos, float maxDmg, float range){
	float sqRange = range*range;
	Vector2D rel;
	float sqDist;
	float amount;
	for(int i=0; i<MAX_OBJECTS; i++){
		if(enemiesOn[i]){
			rel = enemies[i]->GetPosition() - pos;
			sqDist = rel.x*rel.x + rel.y*rel.y;
			rel *= 1/sqrt(sqDist);
			if(sqDist < sqRange){
				amount = ExplosionFunction(maxDmg, sqRange, sqDist);
				enemies[i]->Damage(amount);
				enemies[i]->Knock(rel * amount);
			}
		}
	}
}
