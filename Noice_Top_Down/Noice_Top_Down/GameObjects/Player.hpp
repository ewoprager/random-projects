#ifndef Player_hpp
#define Player_hpp

#include "Game.hpp"
#include "UpdateRenderParent.hpp"
#include "Sprite.hpp"
#include "Collider.hpp"
#include "Particles.hpp"

class Player : public UpdateRenderParent {
public:
	Player(Game *game, Vector2D initialPos);
	~Player();
	
	void Update() override;
	void RenderTop(Vector2D viewPos) override;
	
	// movement
	void W_Pressed();
	void D_Pressed();
	void S_Pressed();
	void A_Pressed();
	void W_Released();
	void D_Released();
	void S_Released();
	void A_Released();
	
	// gun
	void LMB_Pressed();
	void LMB_Released();
	void R_Pressed();
	
	// grenade aluncher
	void RMB_Pressed();
	void RMB_Released();
	
	CollisionMask *GetMask(){
		return mask;
	}
	
	int GetAmmo(){
		return ammo;
	}
	
	int GetMagSize(){
		return magSize;
	}
	
	void Damage(float amount){
		hurtEmitter->position = position;
		hurtEmitter->angleSpread = 2*PI;
		hurtEmitter->Emit(3*amount);
		health -= amount;
		if(health <= 0){
			// died
		}
	}
	void Explosion(Vector2D pos, float maxDmg, float range);
	
	
	void Knock(Vector2D knock){
		velocity += knock;
	}
	
private:
	int clock = 0;
	int clockMax = 180;
	
	// attributes
	int walkSpeed = 11;
	float walkAcc = 0.7;
	
	// gun attributes
	int bulletSpeed = 14;
	float bulletMass = 0.5;
	int magSize = 12;
	int fireTime = 8; // steps per shot
	int reloadTime = 35;
	float shotShake = 10;
	int shotDamage = 40;
	int shotDmgRng = 70;
	// grenade launcher attributes
	int grenadeSpeed = 10;
	float grenadeMass = 2;
	int grenTime = 180; // steps per shot
	float grenShake = 13;
	float grenadeRng = 200;
	float grenadeMaxDmg = 150;
	
	float maxHealth = 100;
	float health = maxHealth;
	float healRate = 0.06;
	Emitter *hurtEmitter;
	
	// gun
	int ammo = 0;
	bool reloading = false;
	Emitter *gunEmitter;
	bool fireDown = false;
	bool grenDown = false;
	CollisionMask *bulletMask;
	
	Sprite *sprite;
	CollisionMask *mask;
	
	// movement
	int hori = 0;
	int vert = 0;
	Vector2D velocity = Vector2D();
	Emitter *walkEmitter;
	
	Vector2D relMousePos = Vector2D();
	
	// top rendering
	SDL_Rect round = {0, 0, 8, 2};
	SDL_Rect hpBar = {4, 4, 256, 16};
	SDL_Rect reloadingBar = {0, 0, 30, 7};
};

#endif /* Player_hpp */
