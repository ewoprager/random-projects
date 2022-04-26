#ifndef UpdateRenderParent_h
#define UpdateRenderParent_h

#define WALLN 30

#include "Game.hpp"

class UpdateRenderParent {
public:
	UpdateRenderParent(Game *game){
		theGame = game;
	}
	
	virtual void Update() {}
	virtual void Render(SDL_Texture **tex, SDL_Rect *src, SDL_Rect *dest, double *ang, SDL_Point *pnt, SDL_RendererFlip *flp) {}
	virtual void RenderTop(Vector2D viewPos) {}
	
	Vector2D GetPosition(){
		return position;
	}
	
	float GetMaxRadius(){
		return maxRadius;
	}
	
protected:
	int updateId;
	int renderId;
	Game *theGame;
	Vector2D position = Vector2D();
	float maxRadius = 0;
};

class DoUpdates {
public:
	DoUpdates(Game *game);
	~DoUpdates() {}
	
	void Update();
	
	int AddObject(UpdateRenderParent *object);
	void RemoveObject(int id);
	
private:
	Game *theGame;
	bool objectsOn[MAX_OBJECTS];
	UpdateRenderParent *objects[MAX_OBJECTS];
};

class DoRenders {
public:
	DoRenders(Game *game);
	~DoRenders() {
		SDL_DestroyTexture(backTex);
		SDL_DestroyTexture(wallTex);
	}
	
	void Render();
	
	int AddObject(UpdateRenderParent *object);
	void RemoveObject(int id);
	
	int AddTop(UpdateRenderParent *object);
	void RemoveTop(int id);
	
	View *view;
	
private:
	Game *theGame;
	
	// game objects
	bool objectsOn[MAX_OBJECTS];
	UpdateRenderParent *objects[MAX_OBJECTS];
	
	// top drawers
	bool topsOn[MAX_OBJECTS];
	UpdateRenderParent *tops[MAX_OBJECTS];
	
	// background
	SDL_Texture *backTex;
	SDL_Rect backSrc = {0, 0, SPRITE_SIZE, SPRITE_SIZE};
	
	// walls
	SDL_Texture *wallTex;
	SDL_Rect wallSrc = {0, 0, SPRITE_SIZE, SPRITE_SIZE};
	
	SDL_Rect tempDest = {0, 0, TILE_SIZE, TILE_SIZE};
};

#endif /* UpdateRenderParent_h */
