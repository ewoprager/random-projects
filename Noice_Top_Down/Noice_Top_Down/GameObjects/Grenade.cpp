#include "Grenade.hpp"
#include "Player.hpp"
#include "View.hpp"
#include "EnemyHandler.hpp"
#include "EnemyParent.hpp"

Grenade::Grenade(Game *game, Vector2D pos, double angle, float speed, float rng, float dmg, float ms, CollisionMask *msk) : UpdateRenderParent(game) {
	position = pos;
	velocity = Vector2D(cos(angle), sin(angle)) * speed;
	
	updateId = theGame->doUpdates->AddObject(this);
	
	sprite = new Sprite(game, this, Vector2D(-22, -6), "grenade.png", 2);
	sprite->angle = angle * 180 / PI;
	sprite->rotPoint = {22, 6};
		
	mask = msk;
	
	range = rng;
	maxDmg = dmg;
	mass = ms;
}
Grenade::~Grenade(){
	theGame->doUpdates->RemoveObject(updateId);
	delete sprite;
}

void Grenade::Update(){
	position += velocity;
	life--;
	
	// possible deaths
	bool explode = false;
	if(theGame->collisionHandler->CheckCollisionColliders(mask, position)){
		playSound("/Users/Edzen098/Projects_OneDrive/Noice_Top_Down/Noice_Top_Down/Assets/ting.wav", SDL_MIX_MAXVOLUME * findVolume(position, theGame->player->GetPosition(), 65536));
		explode = true;
	}
	EnemyParent *enemyHit = theGame->enemyHandler->CheckCollision(mask, position);
	if(enemyHit != nullptr){
		enemyHit->Knock(velocity * mass);
		playSound("/Users/Edzen098/Projects_OneDrive/Noice_Top_Down/Noice_Top_Down/Assets/hit.wav", SDL_MIX_MAXVOLUME * findVolume(position, theGame->player->GetPosition(), 65536));
		explode = true;
	}
	if(life <= 0) explode = true;
	if(explode) Explode();
}

void Grenade::Explode(){
	float vol = findVolume(position, theGame->player->GetPosition(), 589824);
	playSound("/Users/Edzen098/Projects_OneDrive/Noice_Top_Down/Noice_Top_Down/Assets/grenade.wav", SDL_MIX_MAXVOLUME * vol);
	theGame->doRenders->view->Shake(vol*40);
	theGame->enemyHandler->Explosion(position, maxDmg, range);
	theGame->player->Explosion(position, maxDmg, range);
	delete this;
}
