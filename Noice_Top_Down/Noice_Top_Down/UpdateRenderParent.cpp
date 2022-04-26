#include "UpdateRenderParent.hpp"
#include "View.hpp"

DoUpdates::DoUpdates(Game *game) {
	theGame = game;
	for(int i=0; i<MAX_OBJECTS; i++) objectsOn[i] = false;
}
void DoUpdates::Update() {
	for(int i=0; i<MAX_OBJECTS; i++) if(objectsOn[i]) objects[i]->Update();
}
int DoUpdates::AddObject(UpdateRenderParent *object){
	int newId = GetNewId(MAX_OBJECTS, objectsOn);
	objects[newId] = object;
	objectsOn[newId] = true;
	return newId;
}
void DoUpdates::RemoveObject(int id){
	objectsOn[id] = false;
}


DoRenders::DoRenders(Game *game) {
	theGame = game;
	for(int i=0; i<MAX_OBJECTS; i++) objectsOn[i] = false;
	for(int i=0; i<MAX_OBJECTS; i++) topsOn[i] = false;
	
	view = new View(theGame);
	
	backTex = theGame->LoadTexture("floor.png");
	wallTex = theGame->LoadTexture("wall.png");
}
void DoRenders::Render() {
	Vector2D viewPos = view->GetPosition();
	
	float xoff = (int)viewPos.x % TILE_SIZE;
	float yoff = (int)viewPos.y % TILE_SIZE;
	
	// background
	for(int i=0; i<(VIEW_WIDTH + 1); i++){
		for(int j=0; j<(VIEW_HEIGHT + 1); j++){
			tempDest.x = i*TILE_SIZE - xoff;
			tempDest.y = j*TILE_SIZE - yoff;
			SDL_RenderCopy(theGame->renderer, backTex, &backSrc, &tempDest);
		}
	}
	
	// walls
	// boundary walls
	if(viewPos.x < TILE_SIZE){ // can see some left walls
		tempDest.x = -xoff;
		for(int j=0; j<(VIEW_HEIGHT + 1); j++){
			tempDest.y = j*TILE_SIZE - yoff;
			SDL_RenderCopy(theGame->renderer, wallTex, &wallSrc, &tempDest);
		}
	}
	if(viewPos.x > TILE_SIZE*(MAP_WIDTH - VIEW_WIDTH - 1)){ // can see some right walls
		tempDest.x = (viewPos.x >= TILE_SIZE*(MAP_WIDTH - VIEW_WIDTH)) ? (TILE_SIZE*(VIEW_WIDTH - 1) - xoff) : (TILE_SIZE*VIEW_WIDTH - xoff);
		for(int j=0; j<(VIEW_HEIGHT + 1); j++){
			tempDest.y = j*TILE_SIZE - yoff;
			SDL_RenderCopy(theGame->renderer, wallTex, &wallSrc, &tempDest);
		}
	}
	if(viewPos.y < TILE_SIZE){ // can see some top walls
		tempDest.y = -yoff;
		for(int i=0; i<(VIEW_WIDTH + 1); i++){
			tempDest.x = i*TILE_SIZE - xoff;
			SDL_RenderCopy(theGame->renderer, wallTex, &wallSrc, &tempDest);
		}
	}
	if(viewPos.y > TILE_SIZE*(MAP_HEIGHT - VIEW_HEIGHT - 1)){ // can see some bottom walls
		tempDest.y = (viewPos.y >= TILE_SIZE*(MAP_HEIGHT - VIEW_HEIGHT)) ? (TILE_SIZE*(VIEW_HEIGHT - 1) - yoff) : (TILE_SIZE*VIEW_HEIGHT - yoff);
		for(int i=0; i<(VIEW_WIDTH + 1); i++){
			tempDest.x = i*TILE_SIZE - xoff;
			SDL_RenderCopy(theGame->renderer, wallTex, &wallSrc, &tempDest);
		}
	}
	
	
	// game objects
	SDL_Texture *tex;
	SDL_Rect src;
	SDL_Rect dest;
	double ang;
	SDL_Point pnt;
	SDL_RendererFlip flp;
	
	Vector2D objPos;
	
	float objRad;
	for(int i=0; i<MAX_OBJECTS; i++){
		if(objectsOn[i] == true){
			objPos = objects[i]->GetPosition();
			objRad = objects[i]->GetMaxRadius();
			// checking object is in view
			if(objPos.x > viewPos.x - objRad &&
			   objPos.x < viewPos.x + VIEW_WIDTH*TILE_SIZE + objRad &&
			   objPos.y > viewPos.y - objRad &&
			   objPos.y < viewPos.y + VIEW_HEIGHT*TILE_SIZE + objRad){
				// getting draw info
				objects[i]->Render(&tex, &src, &dest, &ang, &pnt, &flp);
				// adjusting for view position
				dest.x -= viewPos.x;
				dest.y -= viewPos.y;
				// drawing object
				SDL_RenderCopyEx(theGame->renderer, tex, &src, &dest, ang, &pnt, flp);
			}
		}
	}
	
	// top render
	for(int i=0; i<MAX_OBJECTS; i++) if(topsOn[i]) tops[i]->RenderTop(viewPos);
}
int DoRenders::AddObject(UpdateRenderParent *object){
	int newId = GetNewId(MAX_OBJECTS, objectsOn);
	objects[newId] = object;
	objectsOn[newId] = true;
	return newId;
}
void DoRenders::RemoveObject(int id){
	objectsOn[id] = false;
}

int DoRenders::AddTop(UpdateRenderParent *object){
	int newId = GetNewId(MAX_OBJECTS, topsOn);
	tops[newId] = object;
	topsOn[newId] = true;
	return newId;
}
void DoRenders::RemoveTop(int id){
	topsOn[id] = false;
}
