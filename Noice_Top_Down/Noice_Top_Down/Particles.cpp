#include "Particles.hpp"

Emitter::Emitter(Game *game, UpdateRenderParent *_owner, int red, int green, int blue, int initialAlpha,
			   float centreRad, float radSpread,
			   int centreLife, int lifeSpread,
			   float centreSpeed, float speedSpread) : UpdateRenderParent(game) {
	
	for(int i=0; i<EMITTER_PARTICLES; i++){
		poolOn[i] = false;
		pool[i] = new Particle(theGame, this, i, red, green, blue, initialAlpha,
							   centreRad + radSpread*(-0.5 + (float)(rand() % 100)/100),
							   centreLife + lifeSpread*(-0.5 + (float)(rand() % 100)/100),
							   centreSpeed + speedSpread*(-0.5 + (float)(rand() % 100)/100));
	}
	updateId = theGame->doUpdates->AddObject(this);
	renderId = theGame->doRenders->AddTop(this);
	owner = _owner;
}
Emitter::~Emitter(){
	theGame->doUpdates->RemoveObject(updateId);
	theGame->doRenders->RemoveTop(renderId);
	for(int i=0; i<EMITTER_PARTICLES; i++) delete pool[i];
}
void Emitter::Update(){
	if(emitting){
		EmitOne();
	}
	for(int i=0; i<EMITTER_PARTICLES; i++){
		if(poolOn[i]){
			pool[i]->Update();
		}
	}
}
void Emitter::RenderTop(Vector2D viewPos){
	for(int i=0; i<EMITTER_PARTICLES; i++){
		if(poolOn[i]){
			pool[i]->RenderTop(viewPos);
		}
	}
}
void Emitter::Emit(int n){
	int ri = 0;
	int _n = 0;
	for(int i=0; i<EMITTER_PARTICLES && _n < n; i++){
		ri = (i + nextP) % EMITTER_PARTICLES;
		if(poolOn[ri] == false){
			pool[ri]->Show(position, centreAngle + angleSpread*(-0.5 + (float)(rand() % 100)/100));
			poolOn[ri] = true;
			_n++;
		}
	}
	nextP = ri + 1;
}
void Emitter::EmitOne(){
	int ri;
	for(int i=0; i<EMITTER_PARTICLES; i++){
		ri = (i + nextP) % EMITTER_PARTICLES;
		if(poolOn[ri] == false){
			pool[ri]->Show(position, centreAngle + angleSpread*(-0.5 + (float)(rand() % 100)/100));
			poolOn[ri] = true;
			nextP = ri + 1;
			break;
		}
	}
}


Particle::Particle(Game *game, Emitter *_owner, int _id, int _red, int _green, int _blue, int _initialAlpha, float _rad, int _life, float _speed) : UpdateRenderParent(game) {
	owner = _owner;
	id = _id;
	red = _red;
	green = _green;
	blue = _blue;
	initialAlpha = _initialAlpha;
	rad = _rad;
	life = _life;
	speed = _speed;
}
Particle::~Particle(){
	//
}
void Particle::Update(){
	position += velocity;
	lifeLeft--;
	if(lifeLeft <= 0){
		Hide();
	}
}
void Particle::RenderTop(Vector2D viewPos){
	SDL_SetRenderDrawColor(theGame->renderer, red, green, blue, 255*lifeLeft/life);
	DrawCircle(theGame->renderer, position - viewPos, rad);
}
void Particle::Show(Vector2D pos, double angle){
	velocity = Vector2D(cos(angle), sin(angle)) * speed;
	position = pos;
	lifeLeft = life;
}
void Particle::Hide(){
	owner->Hide(id);
}
