#include "Player.hpp"
#include "EventHandler.hpp"
#include "Bullet.hpp"
#include "Grenade.hpp"
#include "View.hpp"
#include "audio.hpp"

Player::Player(Game *game, Vector2D initialPos) : UpdateRenderParent(game){
	updateId = theGame->doUpdates->AddObject(this);
	renderId = theGame->doRenders->AddTop(this);
	
	sprite = new Sprite(theGame, this, Vector2D(-14, -16), "player.png", 1);
	mask = new CollisionMask(Vector2D(-14, -15), 28, 30);
	bulletMask = new CollisionMask(Vector2D(-4, -4), 8, 8);
	
	position = initialPos;
	
	// movement
	theGame->eventHandler->ListenKeyDown<Player>(this, (SDL_KeyCode)119, &Player::W_Pressed);
	theGame->eventHandler->ListenKeyDown<Player>(this, (SDL_KeyCode)100, &Player::D_Pressed);
	theGame->eventHandler->ListenKeyDown<Player>(this, (SDL_KeyCode)115, &Player::S_Pressed);
	theGame->eventHandler->ListenKeyDown<Player>(this, (SDL_KeyCode)97, &Player::A_Pressed);
	theGame->eventHandler->ListenKeyUp<Player>(this, (SDL_KeyCode)119, &Player::W_Released);
	theGame->eventHandler->ListenKeyUp<Player>(this, (SDL_KeyCode)100, &Player::D_Released);
	theGame->eventHandler->ListenKeyUp<Player>(this, (SDL_KeyCode)115, &Player::S_Released);
	theGame->eventHandler->ListenKeyUp<Player>(this, (SDL_KeyCode)97, &Player::A_Released);
	walkEmitter = new Emitter(theGame, this, 105, 105, 105, 100,
							  3, 2, // radius
							  6, 8, // life
							  2, 3); // speed
	
	// gun
	theGame->eventHandler->ListenMouseDown<Player>(this, SDL_BUTTON_LEFT, &Player::LMB_Pressed);
	theGame->eventHandler->ListenMouseUp<Player>(this, SDL_BUTTON_LEFT, &Player::LMB_Released);
	theGame->eventHandler->ListenKeyDown<Player>(this, (SDL_KeyCode)114, &Player::R_Pressed);
	gunEmitter = new Emitter(theGame, this, 255, 255, 0, 140,
							 2, 2, // radius
							 8, 8, // life
							 13, 13); // speed
	
	// grenade launcher
	theGame->eventHandler->ListenMouseDown<Player>(this, SDL_BUTTON_RIGHT, &Player::RMB_Pressed);
	theGame->eventHandler->ListenMouseUp<Player>(this, SDL_BUTTON_RIGHT, &Player::RMB_Released);
	
	// hp
	hurtEmitter = new Emitter(theGame, this, 200, 0, 0, 210,
							  4, 2, // radius
							  20, 8, // life
							  2, 3); // speed
}
Player::~Player(){
	theGame->doUpdates->RemoveObject(updateId);
	theGame->doRenders->RemoveTop(renderId);
	delete sprite;
	delete mask;
	delete bulletMask;
	delete walkEmitter;
	delete gunEmitter;
	delete hurtEmitter;
}

void Player::Update(){
	// clock
	clock--;
	
	// movement
	float factor = walkSpeed;
	if(hori && vert){
		factor *= ONE_OVER_ROOT_TWO;
	}
	Vector2D desiredVel = Vector2D(factor*hori, factor*vert);
	
	MoveWithoutCollision(theGame, &position, &velocity, &desiredVel, mask, walkAcc);
	
	walkEmitter->centreAngle = velocity.Angle() + PI;
	walkEmitter->angleSpread = 0.52;
	walkEmitter->position = Vector2D(position.x, position.y + 13);
	walkEmitter->Emit(3);
	
	// mouse
	int mouse_x;
	int mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	relMousePos = Vector2D(mouse_x, mouse_y) + theGame->doRenders->view->GetPosition() - position;
	if(relMousePos.x > 0){
		sprite->flip = SDL_FLIP_NONE;
	} else {
		sprite->flip = SDL_FLIP_HORIZONTAL;
	}
	
	// gun
	if(reloading){
		if(clock <= 0){
			ammo = magSize;
			reloading = false;
		}
	} else {
		if(clock <= 0){
			if(ammo > 0 && fireDown){
				// bullet
				float damage = shotDamage + (rand() % shotDmgRng);
				double bulletAngle = relMousePos.Angle();
				new Bullet(theGame, position, bulletAngle, bulletSpeed, damage, bulletMass, bulletMask);
				// view
				theGame->doRenders->view->Shake(shotShake);
				// ammo
				ammo--;
				// noise
				playSound("/Users/Edzen098/Projects_OneDrive/Noice_Top_Down/Noice_Top_Down/Assets/gunShot.wav", SDL_MIX_MAXVOLUME / 3);
				// particles
				gunEmitter->centreAngle = bulletAngle;
				gunEmitter->angleSpread = 0.21;
				gunEmitter->position = position;
				gunEmitter->Emit(20);
				// clock
				clock = fireTime;
			} else if(grenDown){
				// grenade
				double grenadeAngle = relMousePos.Angle();
				new Grenade(theGame, position, grenadeAngle, grenadeSpeed, grenadeRng, grenadeMaxDmg, grenadeMass, bulletMask);
				// view
				theGame->doRenders->view->Shake(grenShake);
				// ammo
				ammo--;
				// noise
				playSound("/Users/Edzen098/Projects_OneDrive/Noice_Top_Down/Noice_Top_Down/Assets/grenadeLauncher.wav", SDL_MIX_MAXVOLUME);
				// clock
				clock = grenTime;
			}
		}
	}
	
	// hp
	if(health <= 0){
		// die
		
	}
	health += healRate;
	if(health > maxHealth){
		health = maxHealth;
	}
}

void Player::RenderTop(Vector2D viewPos){
	// magazine
	SDL_SetRenderDrawColor(theGame->renderer, 255, 255, 255, 255);
	round.x = position.x - 30 - viewPos.x;
	round.y = position.y + ((round.h + 1)*(magSize - 2) - 1)/2 - viewPos.y;
	for(int i=0; i<ammo; i++){
		SDL_RenderFillRect(theGame->renderer, &round);
		round.y -= round.h + 1;
	}
	
	// health
	SDL_SetRenderDrawColor(theGame->renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(theGame->renderer, &hpBar);
	SDL_SetRenderDrawColor(theGame->renderer, 0, 255, 0, 255);
	SDL_Rect overTop = hpBar;
	overTop.w = hpBar.w * health / maxHealth;
	SDL_RenderFillRect(theGame->renderer, &overTop);
	
	// taking our time
	if(clock > fireTime){
		SDL_Rect barDraw = reloadingBar;
		barDraw.x = position.x - reloadingBar.w/2 - viewPos.x;
		barDraw.y = position.y - 20 - reloadingBar.h - viewPos.y;
		barDraw.w = reloadingBar.w * clock/clockMax;
		SDL_SetRenderDrawColor(theGame->renderer, 255, 255, 255, 255);
		SDL_RenderFillRect(theGame->renderer, &barDraw);
	}
}

void Player::W_Pressed(){
	vert = -1;
}
void Player::S_Pressed(){
	vert = 1;
}
void Player::D_Pressed(){
	hori = 1;
}
void Player::A_Pressed(){
	hori = -1;
}
void Player::W_Released(){
	if(vert == -1){
		vert = 0;
	}
}
void Player::S_Released(){
	if(vert == 1){
		vert = 0;
	}
}
void Player::D_Released(){
	if(hori == 1){
		hori = 0;
	}
}
void Player::A_Released(){
	if(hori == -1){
		hori = 0;
	}
}

void Player::LMB_Pressed(){
	fireDown = true;
}
void Player::LMB_Released(){
	fireDown = false;
}

void Player::R_Pressed(){
	if(ammo < magSize && !reloading && clock <= 0){
		reloading = true;
		clock = reloadTime;
		playSound("/Users/Edzen098/Projects_OneDrive/Noice_Top_Down/Noice_Top_Down/Assets/load.wav", SDL_MIX_MAXVOLUME);
	}
}

void Player::RMB_Pressed(){
	grenDown = true;
}
void Player::RMB_Released(){
	grenDown = false;
}

void Player::Explosion(Vector2D pos, float maxDmg, float range){
	float sqRange = range*range;
	Vector2D rel = position - pos;
	float sqDist = rel.x*rel.x + rel.y*rel.y;
	rel *= 1/sqrt(sqDist);
	if(sqDist < sqRange){
		float amount = ExplosionFunction(maxDmg, sqRange, sqDist);
		Damage(amount);
		Knock(rel * amount);
	}
}
