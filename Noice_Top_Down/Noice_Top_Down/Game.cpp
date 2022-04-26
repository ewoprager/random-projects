#include "App.hpp"
#include "audio.hpp"
#include "Game.hpp"

#include "EventHandler.hpp"
#include "View.hpp"
#include "UpdateRenderParent.hpp"

#include "Wall.hpp"
#include "Player.hpp"
#include "Bullet.hpp"
#include "Grenade.hpp"

#include "Sprite.hpp"
#include "Collider.hpp"

#include "EnemyHandler.hpp"
#include "EnemyParent.hpp"
#include "Slime.hpp"
#include "BigSlime.hpp"


Game::Game(App *_app, SDL_Renderer *ren) {
	app = _app;
	renderer = ren;
	
	eventHandler = new EventHandler();
	
	doUpdates = new DoUpdates(this);
	doRenders = new DoRenders(this);
	
	collisionHandler = new CollisionHandler();
	
	enemyHandler = new EnemyHandler(this);
	
	// top and bottom walls
	CollisionMask *wallMaskA = new CollisionMask(Vector2D(), MAP_WIDTH*TILE_SIZE, TILE_SIZE);
	topCollider = new Collider(this, Vector2D(0, 0), wallMaskA);
	bottomCollider = new Collider(this, Vector2D(0, (MAP_HEIGHT - 1)*TILE_SIZE), wallMaskA);
	//left and right walls
	CollisionMask *wallMaskB = new CollisionMask(Vector2D(), TILE_SIZE, (MAP_HEIGHT - 2)*TILE_SIZE);
	leftCollider = new Collider(this, Vector2D(0, TILE_SIZE), wallMaskB);
	rightCollider = new Collider(this, Vector2D((MAP_WIDTH - 1)*TILE_SIZE, TILE_SIZE), wallMaskB);
	
	player = new Player(this, Vector2D(5*TILE_SIZE, 5*TILE_SIZE));
	
	// walls:
	CollisionMask *wallMaskC = new CollisionMask(Vector2D(-TILE_SIZE/2, -TILE_SIZE/2), TILE_SIZE, TILE_SIZE);
	for(int i=0; i<MAP_WIDTH-2; i++){
		for(int j=0; j<MAP_HEIGHT-2; j++){
			if(walls[j][i] == 1){
				theWalls[wallsN++] = new Wall(this, Vector2D(i + 1.5, j + 1.5)*TILE_SIZE, "wall.png", 4, wallMaskC);
			}
		}
	}
	
	enemyHandler->NextWave();
}

Game::~Game() {
	for(int i=0; i<wallsN; i++) delete theWalls[i];
	delete eventHandler;
	delete doUpdates;
	delete doRenders;
	delete collisionHandler;
	delete enemyHandler;
	delete topCollider;
	delete bottomCollider;
	delete leftCollider;
	delete rightCollider;
	delete player;
}

void Game::HandleEvents(){
	while(SDL_PollEvent(&event)){
		if( event.type == SDL_QUIT ? true : ( event.type == SDL_KEYDOWN ? (event.key.keysym.sym == SDLK_ESCAPE) : false ) ){
			app->GameEnded();
		} else if( (event.type == SDL_KEYDOWN) ? (event.key.keysym.sym == (SDL_KeyCode)112) : false ){
			app->Pause();
		} else {
			eventHandler->HandleEvents(event);
		}
    }
}

void Game::Update(){
	doUpdates->Update();
}

void Game::Render(){
	SDL_RenderClear(renderer);
	
	doRenders->Render();
	
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderPresent(renderer);
}

int GetNewId(int size, bool ons[]){
	for(int i=0; i<size; i++){
		if(ons[i] == false){
			return i;
		}
	}
	return -1;
}

// audio callback function
// here you have to copy the data of your audio buffer into the
// requesting audio buffer (stream)
// you should only copy as much as the requested length (len)
void my_audio_callback(void *userdata, Uint8 *stream, int len) {
	if(audio_len == 0){ return; }
	
	len = ( (len > audio_len) ? audio_len : len );
	//SDL_memcpy (stream, audio_pos, len);						// simply copy from one buffer into the other
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);	// mix from one buffer into another
	
	audio_pos += len;
	audio_len -= len;
}

float findVolume(Vector2D pos1, Vector2D pos2, float sqMaxDist){
	Vector2D rel = pos1 - pos2;
	float sqDist = rel.x*rel.x + rel.y*rel.y;
	return ( (sqDist > sqMaxDist) ? 0 : (1 - sqDist/sqMaxDist) );
}

void DrawCircle(SDL_Renderer * renderer, Vector2D pos, int32_t radius){
	const int32_t diameter = (radius * 2);
	int32_t centreX = pos.x;
	int32_t centreY = pos.y;
	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y){
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

		if (error <= 0){
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0){
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}

void MoveWithoutCollision(Game *theGame, Vector2D *pos, Vector2D *vel, Vector2D*desiredVel, CollisionMask *mask, float acc, int except){
	Vector2D desiredChange = *desiredVel - *vel;
	float sqmag = desiredChange.x*desiredChange.x + desiredChange.y*desiredChange.y;
	if(abs(sqmag) > 0){
		*vel += (sqmag > acc*acc) ? (desiredChange * (acc/sqrt(sqmag))) : (desiredChange);
	} else {
		desiredChange = Vector2D();
	}
	
	if(abs((*vel).x) > TILE_SIZE){
		(*vel).x = ((*vel).x > 0) ? TILE_SIZE : -TILE_SIZE;
	}
	if(abs((*vel).y) > TILE_SIZE){
		(*vel).y = ((*vel).y > 0) ? TILE_SIZE : -TILE_SIZE;
	}
	
	bool stoppedx = false;
	bool stoppedy = false;
	int hori = 0;
	int vert = 0;
	
	if(abs((*vel).x) >= 0.01){
		hori = ((*vel).x > 0) ? 1 : -1;
		if(!theGame->collisionHandler->CheckCollisionColliders(mask, Vector2D((*pos).x + (*vel).x, (*pos).y), except)){
			(*pos).x += (*vel).x;
		} else if(!theGame->collisionHandler->CheckCollisionColliders(mask, Vector2D((*pos).x + hori, (*pos).y), except)){
			(*pos).x += hori;
		} else {
			(*vel).x = 0;
			stoppedx = true;
		}
	} else {
		(*vel).x = 0;
	}
	if(abs((*vel).y) >= 0.01){
		vert = ((*vel).y > 0) ? 1 : -1;
		if(!theGame->collisionHandler->CheckCollisionColliders(mask, Vector2D((*pos).x, (*pos).y + (*vel).y), except)){
			(*pos).y += (*vel).y;
		} else if(!theGame->collisionHandler->CheckCollisionColliders(mask, Vector2D((*pos).x, (*pos).y + vert), except)){
			(*pos).y += vert;
		} else {
			(*vel).y = 0;
			stoppedy = true;
		}
	} else {
		(*vel).y = 0;
	}
	
	// avoiding getting stuck
	if(stoppedx == true && stoppedy == true){ // we can't move but we want to
		if(theGame->collisionHandler->CheckCollisionColliders(mask, Vector2D((*pos).x, (*pos).y), except)){ // we are in something
			(*pos).x += hori;
			(*pos).y += vert;
		}
	}
	
	// not going out the map
	if((*pos).x < 0){
		(*pos).x = TILE_SIZE + mask->GetWidth()/2;
	}
	if((*pos).x > MAP_WIDTH*TILE_SIZE){
		(*pos).x = TILE_SIZE*(MAP_WIDTH - 1) - mask->GetWidth()/2;
	}
	if((*pos).y < 0){
		(*pos).y = TILE_SIZE + mask->GetHeight()/2;
	}
	if((*pos).y > MAP_HEIGHT*TILE_SIZE){
		(*pos).y = TILE_SIZE*(MAP_HEIGHT - 1) - mask->GetHeight()/2;
	}
}

float ExplosionFunction(float maxDmg, float sqRange, float sqDist){
	return maxDmg * exp(- sqDist / (0.16 * sqRange));
}
