#ifndef Sprite_h
#define Sprite_h

#include "Game.hpp"
#include "UpdateRenderParent.hpp"

class Sprite : public UpdateRenderParent {
public:
	Sprite(Game *game, UpdateRenderParent *_owner, Vector2D _relPos, const char *tex, float scale) : UpdateRenderParent(game) {
		owner = _owner;
		
		relPos = _relPos;
		
		updateId = theGame->doUpdates->AddObject(this);
		renderId = theGame->doRenders->AddObject(this);
		
		texture = theGame->LoadTexture(tex);
		
		int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        sourceRect.x = 0;
        sourceRect.y = 0;
        sourceRect.w = w;
        sourceRect.h = h;
		
		destinationRect.w = w*scale;
		destinationRect.h = h*scale;
		
		// max radius
		float topLeft = relPos.SqMag();
		float topRight = Vector2D(relPos.x + destinationRect.w, relPos.y).SqMag();
		float bottomLeft = Vector2D(relPos.x, relPos.y + destinationRect.h).SqMag();
		float bottomRight = Vector2D(relPos.x + destinationRect.w, relPos.y + destinationRect.h).SqMag();
		float max = topLeft;
		if(topRight > max){ max = topRight; }
		if(bottomLeft > max){ max = bottomLeft; }
		if(bottomRight > max){ max = bottomRight; }
		maxRadius = max;
		
	}
	~Sprite() {
		theGame->doUpdates->RemoveObject(updateId);
		theGame->doRenders->RemoveObject(renderId);
		SDL_DestroyTexture(texture);
	}
	
	void Update() override {
		position = owner->GetPosition();
		destinationRect.x = position.x + relPos.x;
		destinationRect.y = position.y + relPos.y;
	}
	
	void Render(SDL_Texture **tex, SDL_Rect *src, SDL_Rect *dest, double *ang, SDL_Point *pnt, SDL_RendererFlip *flp) override {
		*tex = texture;
		*src = sourceRect;
		*dest = destinationRect;
		*ang = angle;
		*pnt = rotPoint;
		*flp = flip;
	}
	
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	
	double angle = 0;
	
	SDL_Point rotPoint = {0, 0};
	
private:	
	UpdateRenderParent *owner;
	SDL_Texture *texture;
	
	SDL_Rect sourceRect, destinationRect;
	Vector2D relPos;
};

#endif /* Sprite_h */
