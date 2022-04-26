#include "BigSlime.hpp"
#include "Sprite.hpp"
#include "Collider.hpp"
#include "Player.hpp"
#include "View.hpp"
#include "EnemyHandler.hpp"

BigSlime::BigSlime(Game *game, Vector2D initialPos, CollisionMask *msk) : EnemyParent(game, initialPos, msk) {
	sprite = new Sprite(theGame, this, Vector2D(-32, -40), "bigSlime.png", 8);
	updateId = theGame->doUpdates->AddObject(this);
	health = maxHealth;
	
	collider = new Collider(theGame, position, mask, &colliderId);
	
	mass = 20;
}
BigSlime::~BigSlime(){
	theGame->doUpdates->RemoveObject(updateId);
	delete sprite;
	delete collider;
	new DeadBigSlime(theGame, position);
}

void BigSlime::Update(){
	collider->SetPosition(position);
	
	Vector2D desiredVelocity = theGame->player->GetPosition() - position;
	desiredVelocity.Normalise();
	desiredVelocity *= speed;
	
	MoveWithoutCollision(theGame, &position, &velocity, &desiredVelocity, mask, acc, colliderId);
	
	// hitting player?
	if(theGame->collisionHandler->CheckCollision(mask, position, theGame->player->GetMask(), theGame->player->GetPosition())){
		theGame->player->Damage(damage);
		Vector2D relPos = theGame->player->GetPosition() - position;
		relPos.Normalise();
		theGame->player->Knock(relPos * knock);
		theGame->doRenders->view->Shake(damageShake);
		playSound("/Users/Edzen098/Projects_OneDrive/Noice_Top_Down/Noice_Top_Down/Assets/melee.wav", SDL_MIX_MAXVOLUME);
	}
	
	// noise
	if((rand() % 90) == 0){
		playSound("/Users/Edzen098/Projects_OneDrive/Noice_Top_Down/Noice_Top_Down/Assets/slime.wav", SDL_MIX_MAXVOLUME * findVolume(position, theGame->player->GetPosition(), 65536));
	}
}
void BigSlime::Damage(float amount) {
	theGame->enemyHandler->slimeEmitter->position = position;
	theGame->enemyHandler->slimeEmitter->angleSpread = 2*PI;
	theGame->enemyHandler->slimeEmitter->Emit(amount);
	
	health -= amount;
	if(health <= 0) delete this;
}

DeadBigSlime::DeadBigSlime(Game *game, Vector2D pos) : UpdateRenderParent(game) {
	position = pos;
	sprite = new Sprite(theGame, this, Vector2D(-32, -40), "deadBigSlime.png", 8);
	timer = 60*5;
	updateId = theGame->doUpdates->AddObject(this);
}
DeadBigSlime::~DeadBigSlime(){
	theGame->doUpdates->RemoveObject(updateId);
	delete sprite;
}
