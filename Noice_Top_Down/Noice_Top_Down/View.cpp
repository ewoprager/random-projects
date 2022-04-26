#include "View.hpp"

View::View(Game *game) : UpdateRenderParent(game) {	
	updateId = theGame->doUpdates->AddObject(this);	
}
View::~View(){
	theGame->doUpdates->RemoveObject(updateId);
}

void View::Update(){
	intendedPosition += (theGame->player->GetPosition() - Vector2D(VIEW_WIDTH*TILE_SIZE/2, VIEW_HEIGHT*TILE_SIZE/2) - intendedPosition) * followSpd;
	if(intendedPosition.x < 0){ intendedPosition.x = 0; }
	if(intendedPosition.x + VIEW_WIDTH*TILE_SIZE > MAP_WIDTH*TILE_SIZE){ intendedPosition.x = MAP_WIDTH*TILE_SIZE - VIEW_WIDTH*TILE_SIZE; }
	if(intendedPosition.y < 0){ intendedPosition.y = 0; }
	if(intendedPosition.y + VIEW_HEIGHT*TILE_SIZE > MAP_HEIGHT*TILE_SIZE){ intendedPosition.y = MAP_HEIGHT*TILE_SIZE - VIEW_HEIGHT*TILE_SIZE; }
	
	position = intendedPosition + CalcShake();
}

Vector2D View::CalcShake(){
	if(shake > shakeMax){
		shake = shakeMax;
	}
	if(shake > shakeMin){
		Vector2D random = Vector2D((rand() % 100)/50 - 1, (rand() % 100)/50 - 1);
		random *= shake;
		shake -= shakeDamp;
		return random;
	} else {
		shake = 0;
		return Vector2D();
	}
}
