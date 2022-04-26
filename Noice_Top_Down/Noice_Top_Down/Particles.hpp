#ifndef Particles_hpp
#define Particles_hpp

#define EMITTER_PARTICLES 15

#include "Game.hpp"
#include "UpdateRenderParent.hpp"

class Emitter : public UpdateRenderParent {
public:
	Emitter(Game *game, UpdateRenderParent *_owner, int red, int green, int blue, int initialAlpha,
			float centreRad, float radSpread,
			int centreLife, int lifeSpread,
			float centreSpeed, float speedSpread);
	~Emitter();
	
	void Update() override;
	void RenderTop(Vector2D viewPos) override;
	
	void Hide(int id){
		poolOn[id] = false;
	}
	
	bool emitting = false;
	
	void Emit(int n);
	void EmitOne();
	
	double centreAngle = 0;
	double angleSpread = 2*PI;
	
	Vector2D position = Vector2D();
	
private:
	int nextP = 0;
	
	UpdateRenderParent *owner;
	Particle *pool[EMITTER_PARTICLES];
	bool poolOn[EMITTER_PARTICLES];
};

class Particle : public UpdateRenderParent {
public:
	Particle(Game *game, Emitter *_owner, int _id, int _red, int _green, int _blue, int _initialAlpha, float _rad, int _life, float speed);
	~Particle();
	
	void Update() override;
	void RenderTop(Vector2D viewPos) override;
	
	void Show(Vector2D pos, double angle);
	void Hide();
	
private:
	int lifeLeft;
	
	int id;
	Emitter *owner;
	int red;
	int green;
	int blue;
	int initialAlpha;
	float rad;
	int life;
	float speed;
	Vector2D velocity;
};

#endif /* Particles_hpp */
