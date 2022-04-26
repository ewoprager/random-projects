#include "Bullet.hpp"
#include "Player.hpp"
#include "EnemyHandler.hpp"
#include "EnemyParent.hpp"

Bullet::Bullet(Game *game, Vector2D pos, double angle, float speed, float dmg, float ms, CollisionMask *msk) : UpdateRenderParent(game) {
	position = pos;
	velocity = Vector2D(cos(angle), sin(angle)) * speed;
	
	updateId = theGame->doUpdates->AddObject(this);
	
	sprite = new Sprite(theGame, this, Vector2D(-22, -6), "bullet.png", 2);
	sprite->angle = angle * 180 / PI;
	sprite->rotPoint = {22, 6};
		
	mask = msk;
	
	damage = dmg;
	mass = ms;
}
Bullet::~Bullet(){
	theGame->doUpdates->RemoveObject(updateId);
	delete sprite;
}

void Bullet::Update(){
	position += velocity;
	life--;
	
	// possible deaths
	EnemyParent *enemyHit = theGame->enemyHandler->CheckCollision(mask, position);
	bool deleteSelf = false;
	if(enemyHit != nullptr){
		enemyHit->Damage(damage);
		enemyHit->Knock(velocity * mass);
		playSound("/Users/Edzen098/Projects_OneDrive/Noice_Top_Down/Noice_Top_Down/Assets/hit.wav", SDL_MIX_MAXVOLUME * findVolume(position, theGame->player->GetPosition(), 65536));
		deleteSelf = true;
	}
	if(theGame->collisionHandler->CheckCollisionColliders(mask, position)){
		playSound("/Users/Edzen098/Projects_OneDrive/Noice_Top_Down/Noice_Top_Down/Assets/ricochet.wav", SDL_MIX_MAXVOLUME * findVolume(position, theGame->player->GetPosition(), 65536));
		deleteSelf = true;
	}
	if(life <= 0) deleteSelf = true;
	if(deleteSelf) delete this;
}
