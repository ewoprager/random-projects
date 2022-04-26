#include "EnemyParent.hpp"
#include "EnemyHandler.hpp"

EnemyParent::EnemyParent(Game *game, Vector2D initialPos, CollisionMask *msk) : UpdateRenderParent(game) {
	enemyId = theGame->enemyHandler->AddObject(this);
	position = initialPos;
	mask = msk;
}
EnemyParent::~EnemyParent(){
	theGame->enemyHandler->RemoveObject(enemyId);
	theGame->enemyHandler->EnemyKilled();
}

CollisionMask *EnemyParent::GetMask(){
	return mask;
}

void EnemyParent::Knock(Vector2D knock){
	velocity += knock * (1/mass);
}
