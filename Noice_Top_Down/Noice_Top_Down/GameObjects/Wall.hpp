#ifndef Wall_hpp
#define Wall_hpp

#include "Game.hpp"
#include "UpdateRenderParent.hpp"
#include "Collider.hpp"

class Wall : public UpdateRenderParent {
public:
	Wall(Game *game, Vector2D pos, const char *tex, float scale, CollisionMask *msk) : UpdateRenderParent(game){
		position = pos;
		renderId = theGame->doRenders->AddObject(this);
		
		// walls deal with their own textures
		texture = theGame->LoadTexture(tex);
		int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        sourceRect.x = 0;
        sourceRect.y = 0;
        sourceRect.w = w;
        sourceRect.h = h;
		destinationRect.x = position.x - TILE_SIZE/2;
		destinationRect.y = position.y - TILE_SIZE/2;
		destinationRect.w = w*scale;
		destinationRect.h = h*scale;
		
		// collider
		mask = msk;
		collider = new Collider(theGame, position, mask);
		
		maxRadius = TILE_SIZE * ONE_OVER_ROOT_TWO;
	}
	~Wall(){
		delete collider;
		theGame->doRenders->RemoveObject(renderId);
		SDL_DestroyTexture(texture);
	}
	
	void Render(SDL_Texture **tex, SDL_Rect *src, SDL_Rect *dest, double *ang, SDL_Point *pnt, SDL_RendererFlip *flp) override {
		*tex = texture;
		*src = sourceRect;
		*dest = destinationRect;
		*ang = 0;
		*pnt = {0, 0};
		*flp = SDL_FLIP_NONE;
	}
	
private:
	// rendering
	SDL_Texture *texture;
	SDL_Rect sourceRect;
	SDL_Rect destinationRect;
	
	// collider
	CollisionMask *mask;
	Collider *collider;
};

#endif /* Wall_hpp */
