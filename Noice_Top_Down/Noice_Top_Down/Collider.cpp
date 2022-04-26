#include "Collider.hpp"

Collider::Collider(Game* game, Vector2D pos, CollisionMask *_mask, int *colId) : UpdateRenderParent(game) {
	position = pos;
	mask = _mask;
	
	colliderId = theGame->collisionHandler->AddCollider(this);
	if(colId != nullptr) *colId = colliderId;
}
Collider::~Collider(){
	theGame->collisionHandler->RemoveCollider(colliderId);
}


CollisionHandler::CollisionHandler() {
	for(int i=0; i<MAX_OBJECTS; i++) collidersOn[i] = false;
}

int CollisionHandler::AddCollider(Collider *col){
	int newId = GetNewId(MAX_OBJECTS, collidersOn);
	colliders[newId] = col;
	collidersOn[newId] = true;
	return newId;
}

void CollisionHandler::RemoveCollider(int id){
	collidersOn[id] = false;
}

bool CollisionHandler::CheckCollision(CollisionMask *mask1, Vector2D pos1, CollisionMask *mask2, Vector2D pos2){
	Vector2D acPos1 = pos1 + mask1->GetRelPos();
	Vector2D acPos2 = pos2 + mask2->GetRelPos();
	if(acPos1.x + mask1->GetWidth() > acPos2.x &&
	   acPos1.x < acPos2.x + mask2->GetWidth() &&
	   acPos1.y + mask1->GetHeight() > acPos2.y &&
	   acPos1.y < acPos2.y + mask2->GetHeight()){
		return true;
	} else {
		return false;
	}
}

bool CollisionHandler::CheckCollisionColliders(CollisionMask *mask, Vector2D pos, int except){
	Vector2D acPos = pos + mask->GetRelPos();
	Vector2D acPosCol;
	for(int i=0; i<MAX_OBJECTS; i++){
		if(!collidersOn[i]) continue;
		if(i == except) continue;
		acPosCol = colliders[i]->GetPosition() + colliders[i]->GetMask()->GetRelPos();
		if(acPos.x + mask->GetWidth() > acPosCol.x &&
		   acPos.x < acPosCol.x + colliders[i]->GetMask()->GetWidth() &&
		   acPos.y + mask->GetHeight() > acPosCol.y &&
		   acPos.y < acPosCol.y + colliders[i]->GetMask()->GetHeight()){
			return true;
		}
	}
	return false;
}
