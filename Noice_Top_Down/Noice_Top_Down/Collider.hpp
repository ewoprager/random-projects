#ifndef Collider_h
#define Collider_h

#include "Game.hpp"
#include "UpdateRenderParent.hpp"

class CollisionMask {
public:
	CollisionMask(Vector2D _relPos, int _width, int _height){
		relPos = _relPos;
		width = _width;
		height = _height;
	}
	
	Vector2D GetRelPos(){
		return relPos;
	}
	int GetWidth(){
		return width;
	}
	int GetHeight(){
		return height;
	}
	
private:
	Vector2D relPos;
	int width;
	int height;
};

class Collider : public UpdateRenderParent {
public:
	Collider(Game* game, Vector2D pos, CollisionMask *_mask, int *colId=nullptr);
	~Collider();
	
	CollisionMask *GetMask(){
		return mask;
	}
	
	void SetPosition(Vector2D newPos){
		position = newPos;
	}
	
private:
	CollisionMask *mask;
	int colliderId;
};

class CollisionHandler {
public:
	CollisionHandler();
	
	int AddCollider(Collider *col);
	void RemoveCollider(int id);
	
	bool CheckCollision(CollisionMask *mask1, Vector2D pos1, CollisionMask *mask2, Vector2D pos2);
	bool CheckCollisionColliders(CollisionMask *mask, Vector2D pos, int except=-1);
	
private:
	bool collidersOn[MAX_OBJECTS];
	Collider *colliders[MAX_OBJECTS];
};

#endif /* Collider_h */
